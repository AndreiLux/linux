/*
 * linux/drivers/gpu/exynos/g2d/g2d_drv.c
 *
 * Copyright (C) 2017 Samsung Electronics Co., Ltd.
 *
 * Contact: Hyesoo Yu <hyesoo.yu@samsung.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/exynos_iovmm.h>
#include <linux/interrupt.h>
#include <linux/suspend.h>
#include <linux/uaccess.h>
#include <linux/module.h>
#include <linux/compat.h>

#include "g2d.h"
#include "g2d_regs.h"
#include "g2d_task.h"
#include "g2d_uapi_process.h"
#include "g2d_debug.h"
#include "g2d_perf.h"

#define MODULE_NAME "exynos-g2d"

static int g2d_update_priority(struct g2d_context *ctx,
					    enum g2d_priority priority)
{
	struct g2d_device *g2d_dev = ctx->g2d_dev;
	struct g2d_task *task;
	unsigned long flags;

	if (ctx->priority == priority)
		return 0;

	atomic_dec(&g2d_dev->prior_stats[ctx->priority]);
	atomic_inc(&g2d_dev->prior_stats[priority]);
	ctx->priority = priority;

	/*
	 * check lower priority task in use, and return EBUSY
	 * for higher priority to avoid waiting lower task completion
	 */
	spin_lock_irqsave(&g2d_dev->lock_task, flags);

	for (task = g2d_dev->tasks; task != NULL; task = task->next) {
		if (!is_task_state_idle(task) && (task->priority < priority)) {
			spin_unlock_irqrestore(&g2d_dev->lock_task, flags);
			return -EBUSY;
		}
	}

	spin_unlock_irqrestore(&g2d_dev->lock_task, flags);

	return 0;
}

void g2d_hw_timeout_handler(unsigned long arg)
{
	struct g2d_task *task = (struct g2d_task *)arg;
	struct g2d_device *g2d_dev = task->g2d_dev;
	unsigned long flags;
	u32 job_state;

	spin_lock_irqsave(&g2d_dev->lock_task, flags);

	job_state = g2d_hw_get_job_state(g2d_dev, task->job_id);

	g2d_stamp_task(task, G2D_STAMP_STATE_TIMEOUT_HW, job_state);

	dev_err(g2d_dev->dev, "%s: Time is up: %d msec for job %u %lu %u\n",
		__func__, G2D_HW_TIMEOUT_MSEC,
		task->job_id, task->state, job_state);

	if (!is_task_state_active(task))
		/*
		 * The task timed out is not currently running in H/W.
		 * It might be just finished by interrupt.
		 */
		goto out;

	if (job_state == G2D_JOB_STATE_DONE)
		/*
		 * The task timed out is not currently running in H/W.
		 * It will be processed in the interrupt handler.
		 */
		goto out;

	if (is_task_state_killed(task)) {
		/* The killed task is not died in the time out priod. */
		g2d_hw_global_reset(g2d_dev);

		g2d_flush_all_tasks(g2d_dev);

		dev_err(g2d_dev->dev,
			"GLOBAL RESET: killed task not dead in %d msec.\n",
			G2D_HW_TIMEOUT_MSEC);
		goto out;
	}

	mod_timer(&task->timer,
	  jiffies + msecs_to_jiffies(G2D_HW_TIMEOUT_MSEC));

	if (job_state != G2D_JOB_STATE_RUNNING)
		/* G2D_JOB_STATE_QUEUEING or G2D_JOB_STATE_SUSPENDING */
		/* Time out is not caused by this task */
		goto out;

	g2d_dump_info(g2d_dev, task);

	mark_task_state_killed(task);

	g2d_hw_kill_task(g2d_dev, task->job_id);

out:
	spin_unlock_irqrestore(&g2d_dev->lock_task, flags);
}

int g2d_device_run(struct g2d_device *g2d_dev, struct g2d_task *task)
{
	g2d_hw_push_task(g2d_dev, task);

	task->ktime_end = ktime_get();

	/* record the time between user request and H/W push */
	g2d_stamp_task(task, G2D_STAMP_STATE_PUSH,
		(int)ktime_us_delta(task->ktime_end, task->ktime_begin));

	task->ktime_begin = ktime_get();

	if (IS_HWFC(task->flags))
		hwfc_set_valid_buffer(task->job_id, task->job_id);

	return 0;
}

static irqreturn_t g2d_irq_handler(int irq, void *priv)
{
	struct g2d_device *g2d_dev = priv;
	unsigned int id;
	u32 intflags, errstatus;

	spin_lock(&g2d_dev->lock_task);

	intflags = g2d_hw_finished_job_ids(g2d_dev);

	g2d_stamp_task(NULL, G2D_STAMP_STATE_INT, intflags);

	if (intflags != 0) {
		for (id = 0; id < G2D_MAX_JOBS; id++) {
			if ((intflags & (1 << id)) == 0)
				continue;

			g2d_finish_task_with_id(g2d_dev, id, true);
		}

		g2d_hw_clear_job_ids(g2d_dev, intflags);
	}

	errstatus = g2d_hw_errint_status(g2d_dev);
	if (errstatus != 0) {
		int job_id = g2d_hw_get_current_task(g2d_dev);
		struct g2d_task *task =
				g2d_get_active_task_from_id(g2d_dev, job_id);

		if (job_id < 0)
			dev_err(g2d_dev->dev, "No task is running in HW\n");
		else if (task == NULL)
			dev_err(g2d_dev->dev,
				"%s: Current job %d in HW is not active\n",
				__func__, job_id);
		else
			dev_err(g2d_dev->dev,
				"%s: Error occurred during running job %d\n",
				__func__, job_id);

		g2d_stamp_task(task, G2D_STAMP_STATE_ERR_INT, errstatus);

		g2d_dump_info(g2d_dev, task);

		g2d_flush_all_tasks(g2d_dev);

		g2d_hw_global_reset(g2d_dev);

		g2d_hw_clear_int(g2d_dev, errstatus);
	}

	spin_unlock(&g2d_dev->lock_task);

	wake_up(&g2d_dev->freeze_wait);

	return IRQ_HANDLED;
}

static int g2d_iommu_fault_handler(struct iommu_domain *domain,
				struct device *dev, unsigned long fault_addr,
				int fault_flags, void *token)
{
	struct g2d_device *g2d_dev = token;
	struct g2d_task *task;
	int job_id = g2d_hw_get_current_task(g2d_dev);
	unsigned long flags;

	spin_lock_irqsave(&g2d_dev->lock_task, flags);
	task = g2d_get_active_task_from_id(g2d_dev, job_id);
	spin_unlock_irqrestore(&g2d_dev->lock_task, flags);

	g2d_dump_info(g2d_dev, task);

	g2d_stamp_task(task, G2D_STAMP_STATE_MMUFAULT, 0);

	return 0;
}

static __u32 get_hw_version(struct g2d_device *g2d_dev, __u32 *version)
{
	int ret;

	ret = pm_runtime_get_sync(g2d_dev->dev);
	if (ret < 0) {
		dev_err(g2d_dev->dev, "Failed to enable power (%d)\n", ret);
		return ret;
	}

	ret = clk_prepare_enable(g2d_dev->clock);
	if (ret < 0) {
		dev_err(g2d_dev->dev, "Failed to enable clock (%d)\n", ret);
	} else {
		*version = readl_relaxed(g2d_dev->reg + G2D_VERSION_INFO_REG);
		clk_disable(g2d_dev->clock);
	}

	pm_runtime_put(g2d_dev->dev);

	return ret;
}

static void g2d_timeout_perf_work(struct work_struct *work)
{
	struct g2d_context *ctx = container_of(work, struct g2d_context,
					      dwork.work);

	g2d_put_performance(ctx, false);
}

static int g2d_open(struct inode *inode, struct file *filp)
{
	struct g2d_device *g2d_dev;
	struct g2d_context *g2d_ctx;
	struct miscdevice *misc = filp->private_data;

	g2d_ctx = kzalloc(sizeof(*g2d_ctx), GFP_KERNEL);
	if (!g2d_ctx)
		return -ENOMEM;

	if (!strcmp(misc->name, "g2d")) {
		g2d_dev = container_of(misc, struct g2d_device, misc[0]);
		g2d_ctx->authority = G2D_AUTHORITY_HIGHUSER;
	} else {
		g2d_dev = container_of(misc, struct g2d_device, misc[1]);
	}

	filp->private_data = g2d_ctx;

	g2d_ctx->g2d_dev = g2d_dev;

	g2d_ctx->priority = G2D_DEFAULT_PRIORITY;
	atomic_inc(&g2d_dev->prior_stats[g2d_ctx->priority]);

	INIT_LIST_HEAD(&g2d_ctx->qos_node);
	INIT_DELAYED_WORK(&(g2d_ctx->dwork), g2d_timeout_perf_work);

	return 0;
}

static int g2d_release(struct inode *inode, struct file *filp)
{
	struct g2d_context *g2d_ctx = filp->private_data;
	struct g2d_device *g2d_dev = g2d_ctx->g2d_dev;

	atomic_dec(&g2d_dev->prior_stats[g2d_ctx->priority]);

	if (g2d_ctx->hwfc_info) {
		int i;

		for (i = 0; i < g2d_ctx->hwfc_info->buffer_count; i++)
			dma_buf_put(g2d_ctx->hwfc_info->bufs[i]);
		kfree(g2d_ctx->hwfc_info);
	}

	g2d_put_performance(g2d_ctx, true);

	kfree(g2d_ctx);

	return 0;
}

static long g2d_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct g2d_context *ctx = filp->private_data;
	struct g2d_device *g2d_dev = ctx->g2d_dev;
	int ret = 0;

	switch (cmd) {
	case G2D_IOC_PROCESS:
	{
		struct g2d_task_data __user *uptr =
				(struct g2d_task_data __user *)arg;
		struct g2d_task_data data;
		struct g2d_task *task;
		int i;

		/*
		 * A process that has lower priority is not allowed
		 * to execute and simply returns -EBUSY
		 */
		for (i = ctx->priority + 1; i < G2D_PRIORITY_END; i++) {
			if (atomic_read(&g2d_dev->prior_stats[i]) > 0) {
				ret =  -EBUSY;
				break;
			}
		}
		if (ret)
			break;

		if (copy_from_user(&data, uptr, sizeof(data))) {
			dev_err(g2d_dev->dev,
				"%s: Failed to read g2d_task_data\n", __func__);
			ret = -EFAULT;
			break;
		}

		/*
		 * If the task should be run by hardware flow control with MFC,
		 * driver must request shared buffer to repeater driver if it
		 * has not been previously requested at current context.
		 * hwfc_request_buffer has increased the reference count inside
		 * function, so driver must reduce the reference
		 * when context releases.
		 */
		if (IS_HWFC(data.flags) && !ctx->hwfc_info) {
			ctx->hwfc_info = kzalloc(
					sizeof(*ctx->hwfc_info), GFP_KERNEL);
			if (!ctx->hwfc_info) {
				ret = -ENOMEM;
				break;
			}

			ret = hwfc_request_buffer(ctx->hwfc_info, 0);
			if (ret ||
				(ctx->hwfc_info->buffer_count >
				 MAX_SHARED_BUF_NUM)) {
				kfree(ctx->hwfc_info);
				ctx->hwfc_info = NULL;
				dev_err(g2d_dev->dev,
					"%s: Failed to read hwfc info\n",
					__func__);
				break;
			}
		}

		task = g2d_get_free_task(g2d_dev, ctx, IS_HWFC(data.flags));
		if (task == NULL) {
			ret = -EBUSY;
			break;
		}

		kref_init(&task->starter);

		ret = g2d_get_userdata(g2d_dev, ctx, task, &data);
		if (ret < 0) {
			g2d_put_free_task(g2d_dev, task);
			break;
		}

		g2d_stamp_task(task, G2D_STAMP_STATE_BEGIN, task->priority);

		g2d_start_task(task);

		if (!(task->flags & G2D_FLAG_NONBLOCK))
			ret = g2d_wait_put_user(g2d_dev, task,
						uptr, data.flags);

		break;
	}
	case G2D_IOC_PRIORITY:
	{
		enum g2d_priority data;

		if (!(ctx->authority & G2D_AUTHORITY_HIGHUSER)) {
			ret = -EPERM;
			break;
		}

		if (copy_from_user(&data, (void __user *)arg, sizeof(data))) {
			dev_err(g2d_dev->dev,
				"%s: Failed to get priority\n", __func__);
			ret = -EFAULT;
			break;
		}

		if ((data < G2D_LOW_PRIORITY) || (data >= G2D_PRIORITY_END)) {
			dev_err(g2d_dev->dev,
				"%s: Wrong priority %u\n", __func__, data);
			ret = -EINVAL;
			break;
		}

		ret = g2d_update_priority(ctx, data);

		break;
	}
	case G2D_IOC_PERFORMANCE:
	{
		struct g2d_performance_data data;

		if (!(ctx->authority & G2D_AUTHORITY_HIGHUSER)) {
			ret = -EPERM;
			break;
		}

		if (copy_from_user(&data, (void __user *)arg, sizeof(data))) {
			dev_err(g2d_dev->dev,
				"%s: Failed to read perf data\n", __func__);
			ret = -EFAULT;
			break;
		}
		g2d_set_performance(ctx, &data, false);

		break;
	}
	}

	return ret;
}

#ifdef CONFIG_COMPAT
struct compat_g2d_commands {
	__u32		target[G2DSFR_DST_FIELD_COUNT];
	compat_uptr_t	source[G2D_MAX_IMAGES];
	compat_uptr_t	extra;
	__u32		num_extra_regs;
};

struct compat_g2d_buffer_data {
	union {
		compat_ulong_t userptr;
		struct {
			__s32 fd;
			__u32 offset;
		} dmabuf;
	};
	__u32		length;
};

struct compat_g2d_layer_data {
	__u32			flags;
	__s32			fence;
	__u32			buffer_type;
	__u32			num_buffers;
	struct compat_g2d_buffer_data	buffer[G2D_MAX_BUFFERS];
};

struct compat_g2d_task_data {
	__u32			version;
	__u32			flags;
	__u32			laptime_in_usec;
	__u32			priority;
	__u32			num_source;
	__u32			num_release_fences;
	compat_uptr_t	release_fences;
	struct compat_g2d_layer_data	target;
	compat_uptr_t	source;
	struct compat_g2d_commands	commands;
};

#define COMPAT_G2D_IOC_PROCESS	_IOWR('M', 4, struct compat_g2d_task_data)

static int g2d_compat_get_layerdata(struct device *dev,
				struct g2d_layer_data __user *img,
				struct compat_g2d_layer_data __user *cimg)
{
	__u32 uw;
	__s32 sw;
	compat_ulong_t l;
	unsigned int i;
	int ret;

	ret =  get_user(uw, &cimg->flags);
	ret |= put_user(uw, &img->flags);
	ret |= get_user(sw, &cimg->fence);
	ret |= put_user(sw, &img->fence);
	ret |= get_user(uw, &cimg->buffer_type);
	ret |= put_user(uw, &img->buffer_type);
	ret |= get_user(uw, &cimg->num_buffers);
	ret |= put_user(uw, &img->num_buffers);

	for (i = 0; i < uw; i++) { /* uw contains num_buffers */
		ret |= get_user(l, &cimg->buffer[i].userptr);
		ret |= put_user(l, &img->buffer[i].userptr);
		ret |= get_user(uw, &cimg->buffer[i].dmabuf.offset);
		ret |= put_user(uw, &img->buffer[i].dmabuf.offset);
		ret |= get_user(sw, &cimg->buffer[i].dmabuf.fd);
		ret |= put_user(sw, &img->buffer[i].dmabuf.fd);
		ret |= get_user(uw, &cimg->buffer[i].length);
		ret |= put_user(uw, &img->buffer[i].length);
	}

	return ret ? -EFAULT : 0;
}

static long g2d_compat_ioctl(struct file *filp,
			     unsigned int cmd, unsigned long arg)
{
	struct g2d_context *ctx = filp->private_data;
	struct device *dev = ctx->g2d_dev->dev;
	struct g2d_task_data __user *data;
	struct g2d_layer_data __user *src;
	struct g2d_commands __user *command;
	struct g2d_reg __user *extra;
	struct compat_g2d_task_data __user *cdata = compat_ptr(arg);
	struct compat_g2d_layer_data __user *csc;
	struct compat_g2d_commands __user *ccmd;
	size_t alloc_size;
	__s32 __user *fences;
	__u32 __user *ptr;
	compat_uptr_t cptr;
	__u32 w, num_source, num_release_fences;
	int ret;

	switch (cmd) {
	case COMPAT_G2D_IOC_PROCESS:
		cmd = (unsigned int)G2D_IOC_PROCESS;
		break;
	case G2D_IOC_PRIORITY:
	case G2D_IOC_PERFORMANCE:
		if (!filp->f_op->unlocked_ioctl)
			return -ENOTTY;

		return filp->f_op->unlocked_ioctl(filp, cmd,
						(unsigned long)compat_ptr(arg));
	default:
		dev_err(dev, "%s: unknown ioctl command %#x\n", __func__, cmd);
		return -EINVAL;
	}

	alloc_size = sizeof(*data);
	data = compat_alloc_user_space(alloc_size);

	ret  = get_user(w, &cdata->version);
	ret |= put_user(w, &data->version);
	ret |= get_user(w, &cdata->flags);
	ret |= put_user(w, &data->flags);
	ret |= get_user(w, &cdata->laptime_in_usec);
	ret |= put_user(w, &data->laptime_in_usec);
	ret |= get_user(w, &cdata->priority);
	ret |= put_user(w, &data->priority);
	ret |= get_user(num_source, &cdata->num_source);
	ret |= put_user(num_source, &data->num_source);
	ret |= get_user(num_release_fences, &cdata->num_release_fences);
	ret |= put_user(num_release_fences, &data->num_release_fences);
	alloc_size += sizeof(__s32) * num_release_fences;
	fences = compat_alloc_user_space(alloc_size);
	ret |= put_user(fences, &data->release_fences);
	if (ret) {
		dev_err(dev, "%s: failed to read task data\n", __func__);
		return -EFAULT;
	}

	ret = g2d_compat_get_layerdata(
			dev, &data->target, &cdata->target);
	if (ret) {
		dev_err(dev, "%s: failed to read the target data\n",
			__func__);
		return ret;
	}

	ret = get_user(cptr, &cdata->source);
	csc = compat_ptr(cptr);
	alloc_size += sizeof(*src) * num_source;
	src = compat_alloc_user_space(alloc_size);
	for (w = 0; w < num_source; w++)
		ret |= g2d_compat_get_layerdata(dev, &src[w], &csc[w]);
	ret |= put_user(src, &data->source);
	if (ret) {
		dev_err(dev,
		"%s: failed to read source layer data\n", __func__);
		return ret;
	}

	command = &data->commands;
	ccmd = &cdata->commands;
	ret = copy_in_user(&command->target, &ccmd->target,
			sizeof(__u32) * G2DSFR_DST_FIELD_COUNT);
	if (ret) {
		dev_err(dev,
			"%s: failed to read target command data\n", __func__);
		return ret;
	}

	for (w = 0; w < num_source; w++) {
		get_user(cptr, &ccmd->source[w]);
		alloc_size += sizeof(__u32) * G2DSFR_SRC_FIELD_COUNT;
		ptr = compat_alloc_user_space(alloc_size);
		ret = copy_in_user(ptr, compat_ptr(cptr),
				sizeof(__u32) * G2DSFR_SRC_FIELD_COUNT);
		ret |= put_user(ptr, &command->source[w]);
		if (ret) {
			dev_err(dev,
				"%s: failed to read source %u command data\n",
					__func__, w);
			return ret;
		}
	}

	ret = get_user(w, &ccmd->num_extra_regs);
	ret |= put_user(w, &command->num_extra_regs);

	/* w contains num_extra_regs */
	get_user(cptr, &ccmd->extra);
	alloc_size += sizeof(*extra) * w;
	extra = compat_alloc_user_space(alloc_size);
	ret |= copy_in_user(extra, compat_ptr(cptr),
				sizeof(*extra) * w);
	ret |= put_user(extra, &command->extra);
	if (ret) {
		dev_err(dev,
			"%s: failed to read extra command data\n", __func__);
		return ret;
	}

	ret = g2d_ioctl(filp, cmd, (unsigned long)data);
	if (ret)
		return ret;

	ret = get_user(w, &data->laptime_in_usec);
	ret |= put_user(w, &cdata->laptime_in_usec);

	get_user(cptr, &cdata->release_fences);
	ret |= copy_in_user(compat_ptr(cptr), fences,
					sizeof(__s32) * num_release_fences);
	if (ret)
		dev_err(dev, "%s: failed to write userdata\n", __func__);

	return ret;
}
#endif

static const struct file_operations g2d_fops = {
	.owner          = THIS_MODULE,
	.open           = g2d_open,
	.release        = g2d_release,
	.unlocked_ioctl	= g2d_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl	= g2d_compat_ioctl,
#endif
};

static int g2d_notifier_event(struct notifier_block *this,
			      unsigned long event, void *ptr)
{
	struct g2d_device *g2d_dev;

	g2d_dev = container_of(this, struct g2d_device, pm_notifier);

	switch (event) {
	case PM_SUSPEND_PREPARE:
		g2d_prepare_suspend(g2d_dev);
		break;

	case PM_POST_SUSPEND:
		g2d_suspend_finish(g2d_dev);
		break;
	}

	return NOTIFY_OK;
}

static unsigned int g2d_default_ppc[] = {
	/* sc_up none x1 x1/4 x1/9 x1/16 */
	3400, 3100, 2200, 3600, 5100, 7000, //rgb32 non-rotated
	3300, 2700, 2000, 3000, 5200, 6500, //rgb32 rotated
	3000, 2900, 2600, 3400, 5100, 11900, //yuv2p non-rotated
	3200, 2000, 1900, 3300, 5200, 7000, //yuv2p rotated
	2400, 1900, 1900, 2700, 3100, 4100, //8+2 non-rotated
	2500, 900, 900, 2200, 2900, 3700, //8+2 rotated
	3800, //colorfill
};

static struct g2d_dvfs_table g2d_default_dvfs_table[] = {
	{534000, 711000},
	{400000, 534000},
	{336000, 400000},
	{267000, 356000},
	{178000, 200000},
	{107000, 134000},
};

static int g2d_parse_dt(struct g2d_device *g2d_dev)
{
	struct device *dev = g2d_dev->dev;
	int len;

	if (of_property_read_u32_array(dev->of_node, "hw_ppc",
			(u32 *)g2d_dev->hw_ppc,
			(size_t)(ARRAY_SIZE(g2d_dev->hw_ppc)))) {
		dev_err(dev, "Failed to parse device tree for hw ppc");

		memcpy(g2d_dev->hw_ppc, g2d_default_ppc,
			sizeof(g2d_default_ppc[0]) * PPC_END);
	}

	len = of_property_count_u32_elems(dev->of_node, "g2d_dvfs_table");
	if (len < 0)
		g2d_dev->dvfs_table_cnt = ARRAY_SIZE(g2d_default_dvfs_table);
	else
		g2d_dev->dvfs_table_cnt = len / 2;

	g2d_dev->dvfs_table = devm_kzalloc(dev,
				sizeof(struct g2d_dvfs_table) *
				g2d_dev->dvfs_table_cnt,
				GFP_KERNEL);
	if (!g2d_dev->dvfs_table)
		return -ENOMEM;

	if (len < 0) {
		memcpy(g2d_dev->dvfs_table, g2d_default_dvfs_table,
			sizeof(struct g2d_dvfs_table) *
			g2d_dev->dvfs_table_cnt);
	} else {
		of_property_read_u32_array(dev->of_node, "g2d_dvfs_table",
				(unsigned int *)g2d_dev->dvfs_table, len);
	}

	return 0;
}

static int g2d_probe(struct platform_device *pdev)
{
	struct g2d_device *g2d_dev;
	struct resource *res;
	__u32 version;
	int ret;

	g2d_dev = devm_kzalloc(&pdev->dev, sizeof(*g2d_dev), GFP_KERNEL);
	if (!g2d_dev)
		return -ENOMEM;

	platform_set_drvdata(pdev, g2d_dev);
	g2d_dev->dev = &pdev->dev;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	g2d_dev->reg = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(g2d_dev->reg))
		return PTR_ERR(g2d_dev->reg);

	res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (!res) {
		dev_err(&pdev->dev, "Failed to get IRQ resource");
		return -ENOENT;
	}

	ret = devm_request_irq(&pdev->dev, res->start,
			       g2d_irq_handler, 0, pdev->name, g2d_dev);
	if (ret) {
		dev_err(&pdev->dev, "Failed to install IRQ handler");
		return ret;
	}

	g2d_dev->clock = devm_clk_get(&pdev->dev, "gate");
	if (IS_ERR(g2d_dev->clock)) {
		dev_err(&pdev->dev, "Failed to get clock (%ld)\n",
					PTR_ERR(g2d_dev->clock));
		return PTR_ERR(g2d_dev->clock);
	}

	iovmm_set_fault_handler(&pdev->dev, g2d_iommu_fault_handler, g2d_dev);

	ret = g2d_parse_dt(g2d_dev);
	if (ret < 0)
		return ret;

	ret = iovmm_activate(&pdev->dev);
	if (ret < 0) {
		dev_err(&pdev->dev, "Failed to activate iommu\n");
		return ret;
	}

	/* prepare clock and enable runtime pm */
	pm_runtime_enable(&pdev->dev);

	ret = get_hw_version(g2d_dev, &version);
	if (ret < 0)
		goto err;

	g2d_dev->misc[0].minor = MISC_DYNAMIC_MINOR;
	g2d_dev->misc[0].name = "g2d";
	g2d_dev->misc[0].fops = &g2d_fops;

	/* misc register */
	ret = misc_register(&g2d_dev->misc[0]);
	if (ret) {
		dev_err(&pdev->dev, "Failed to register misc device for 0");
		goto err;
	}

	g2d_dev->misc[1].minor = MISC_DYNAMIC_MINOR;
	g2d_dev->misc[1].name = "fimg2d";
	g2d_dev->misc[1].fops = &g2d_fops;

	ret = misc_register(&g2d_dev->misc[1]);
	if (ret) {
		dev_err(&pdev->dev, "Failed to register misc device for 1");
		goto err_misc;
	}

	spin_lock_init(&g2d_dev->lock_task);

	INIT_LIST_HEAD(&g2d_dev->tasks_free);
	INIT_LIST_HEAD(&g2d_dev->tasks_free_hwfc);
	INIT_LIST_HEAD(&g2d_dev->tasks_prepared);
	INIT_LIST_HEAD(&g2d_dev->tasks_active);
	INIT_LIST_HEAD(&g2d_dev->qos_contexts);

	mutex_init(&g2d_dev->lock_qos);

	ret = g2d_create_tasks(g2d_dev);
	if (ret < 0) {
		dev_err(&pdev->dev, "Failed to create tasks");
		goto err_task;
	}

	init_waitqueue_head(&g2d_dev->freeze_wait);

	g2d_dev->pm_notifier.notifier_call = &g2d_notifier_event;
	ret = register_pm_notifier(&g2d_dev->pm_notifier);
	if (ret)
		goto err_pm;

	spin_lock_init(&g2d_dev->fence_lock);
	g2d_dev->fence_context = fence_context_alloc(1);

	dev_info(&pdev->dev, "Probed FIMG2D version %#010x\n", version);

	g2d_init_debug(g2d_dev);

	return 0;
err_pm:
	g2d_destroy_tasks(g2d_dev);
err_task:
	misc_deregister(&g2d_dev->misc[1]);
err_misc:
	misc_deregister(&g2d_dev->misc[0]);
err:
	pm_runtime_disable(&pdev->dev);
	iovmm_deactivate(g2d_dev->dev);

	dev_err(&pdev->dev, "Failed to probe FIMG2D\n");

	return ret;
}

static void g2d_shutdown(struct platform_device *pdev)
{
	struct g2d_device *g2d_dev = platform_get_drvdata(pdev);

	g2d_stamp_task(NULL, G2D_STAMP_STATE_SHUTDOWN, 0);
	g2d_prepare_suspend(g2d_dev);

	wait_event(g2d_dev->freeze_wait, list_empty(&g2d_dev->tasks_active));

	if (test_and_set_bit(G2D_DEVICE_STATE_IOVMM_DISABLED, &g2d_dev->state))
		iovmm_deactivate(g2d_dev->dev);

	g2d_stamp_task(NULL, G2D_STAMP_STATE_SHUTDOWN, 1);
}

static int g2d_remove(struct platform_device *pdev)
{
	struct g2d_device *g2d_dev = platform_get_drvdata(pdev);

	g2d_destroy_debug(g2d_dev);

	g2d_shutdown(pdev);

	g2d_destroy_tasks(g2d_dev);

	misc_deregister(&g2d_dev->misc[0]);
	misc_deregister(&g2d_dev->misc[1]);

	pm_runtime_disable(&pdev->dev);

	return 0;
}

#ifdef CONFIG_PM
static int g2d_runtime_resume(struct device *dev)
{
	g2d_stamp_task(NULL, G2D_STAMP_STATE_RUNTIME_PM, 0);

	return 0;
}

static int g2d_runtime_suspend(struct device *dev)
{
	struct g2d_device *g2d_dev = dev_get_drvdata(dev);

	clk_unprepare(g2d_dev->clock);
	g2d_stamp_task(NULL, G2D_STAMP_STATE_RUNTIME_PM, 1);

	return 0;
}
#endif

static const struct dev_pm_ops g2d_pm_ops = {
	SET_RUNTIME_PM_OPS(NULL, g2d_runtime_resume, g2d_runtime_suspend)
};

static const struct of_device_id of_g2d_match[] = {
	{
		.compatible = "samsung,exynos9810-g2d",
	},
	{},
};

static struct platform_driver g2d_driver = {
	.probe		= g2d_probe,
	.remove		= g2d_remove,
	.shutdown	= g2d_shutdown,
	.driver = {
		.name	= MODULE_NAME,
		.owner	= THIS_MODULE,
		.pm	= &g2d_pm_ops,
		.of_match_table = of_match_ptr(of_g2d_match),
	}
};

module_platform_driver(g2d_driver);
