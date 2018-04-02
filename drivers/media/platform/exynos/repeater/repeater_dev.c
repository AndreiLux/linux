/*
 * copyright (c) 2017 Samsung Electronics Co., Ltd.
 *             http://www.samsung.com
 *
 * Core file for Samsung EXYNOS TSMUX driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <asm/uaccess.h>
#include <linux/of.h>
#include <linux/slab.h>
#include <linux/ion.h>
#include <linux/delay.h>
#include <linux/moduleparam.h>
#include <linux/pm_runtime.h>

#include "repeater_dev.h"
#include "repeater_dbg.h"

#define REPEATER_CUR_CONTEXTS_NUM		0
#define USES_WORKQUEUE

static struct repeater_device *g_repeater_device;

#define ENCODING_PERIOD_TIME_US			1000000
#define MAX_WAIT_TIME_DUMP				10000000

int g_repeater_debug_level;
module_param(g_repeater_debug_level, int, 0600);

/*
 * If fps is 60, MFC encoding is called when hwfc_set_valid_buffer is called
 */

int hwfc_request_buffer(struct shared_buffer_info *info, int owner)
{
	int ret = 0;
	struct repeater_context *ctx;
	int i = 0;
	int *buf_fd;
	int idx = REPEATER_CUR_CONTEXTS_NUM;

	print_repeater_debug(RPT_EXT_INFO, "%s++\n", __func__);

	if (!g_repeater_device || !info)
		return -EFAULT;

	ctx = g_repeater_device->ctx[idx];
	if (!ctx)
		return -EFAULT;

	if (ctx->ctx_status == REPEATER_CTX_INIT)
		return -EFAULT;

	info->pixel_format = ctx->info.pixel_format;
	info->width = ctx->info.width;
	info->height = ctx->info.height;
	info->buffer_count = ctx->info.buffer_count;
	buf_fd = ctx->info.buf_fd;

	for (i = 0; i < info->buffer_count; i++) {
		get_dma_buf(ctx->dmabufs[i]);
		info->bufs[i] = ctx->dmabufs[i];
	}

	print_repeater_debug(RPT_EXT_INFO,
		"f 0x%x, w %d, h %d, buf_cnt %d\n", info->pixel_format,
		info->width, info->height, info->buffer_count);
	for (i = 0; i < info->buffer_count; i++)
		print_repeater_debug(RPT_EXT_INFO,
			"owner %d, dma_buf[%d] %p\n", owner, i, info->bufs[i]);

	print_repeater_debug(RPT_EXT_INFO, "%s--\n", __func__);

	return ret;
}

int hwfc_get_valid_buffer(int *buf_idx)
{
	int ret = 0;
	struct repeater_context *ctx;
	struct shared_buffer *shr_bufs;
	int idx = REPEATER_CUR_CONTEXTS_NUM;
	unsigned long flags;

	print_repeater_debug(RPT_EXT_INFO, "%s++\n", __func__);

	if (!g_repeater_device || !buf_idx)
		return -EFAULT;

	ctx = g_repeater_device->ctx[idx];
	if (!ctx)
		return -EFAULT;

	if (ctx->ctx_status == REPEATER_CTX_INIT)
		return -EFAULT;

	spin_lock_irqsave(&ctx->ctx_spinlock, flags);

	shr_bufs = &ctx->shared_bufs;
	ret = get_capturing_buf_idx(shr_bufs, buf_idx);

	print_repeater_debug(RPT_EXT_INFO, "ret %d, buf_idx %d\n", ret, *buf_idx);

	spin_unlock_irqrestore(&ctx->ctx_spinlock, flags);

	print_repeater_debug(RPT_EXT_INFO, "%s--\n", __func__);

	return ret;
}

int hwfc_set_valid_buffer(int buf_idx, int capture_idx)
{
	int ret = 0;
	struct repeater_context *ctx;
	struct shared_buffer *shr_bufs;
	int idx = REPEATER_CUR_CONTEXTS_NUM;
	unsigned long flags;
	ktime_t cur_ktime;
	uint64_t cur_timestamp;

	print_repeater_debug(RPT_EXT_INFO, "%s++\n", __func__);

	print_repeater_debug(RPT_EXT_INFO, "buf_idx %d, capture_idx %d\n",
		buf_idx, capture_idx);

	if (!g_repeater_device)
		return -EFAULT;

	ctx = g_repeater_device->ctx[idx];
	if (!ctx)
		return -EFAULT;

	if (ctx->ctx_status == REPEATER_CTX_INIT)
		return -EFAULT;

	if (capture_idx == -1)
		return -EFAULT;

	spin_lock_irqsave(&ctx->ctx_spinlock, flags);

	shr_bufs = &ctx->shared_bufs;

	ret = set_captured_buf_idx(shr_bufs, buf_idx);

	if (ctx->info.fps == 60) {
		cur_ktime = ktime_get();
		cur_timestamp = ktime_to_us(cur_ktime);
		if (ret == 0) {
			ctx->enc_param.time_stamp = cur_timestamp;
			set_encoding_start(shr_bufs, buf_idx);
			ret = s5p_mfc_hwfc_encode(buf_idx, capture_idx, &ctx->enc_param);
			if (ret != HWFC_ERR_NONE) {
				print_repeater_debug(RPT_ERROR, "s5p_mfc_hwfc_encode failed %d\n", ret);
				ret = set_encoding_done(shr_bufs);
			}
		}
	}

	spin_unlock_irqrestore(&ctx->ctx_spinlock, flags);

	print_repeater_debug(RPT_EXT_INFO, "%s--\n", __func__);

	return ret;
}

int hwfc_encoding_done(int encoding_ret)
{
	int ret = 0;
	struct repeater_context *ctx;
	struct shared_buffer *shr_bufs;
	int idx = REPEATER_CUR_CONTEXTS_NUM;
	unsigned long flags;

	print_repeater_debug(RPT_EXT_INFO, "%s++\n", __func__);

	if (!g_repeater_device)
		return -ENODEV;

	ctx = g_repeater_device->ctx[idx];
	if (!ctx)
		return -ENODEV;

	spin_lock_irqsave(&ctx->ctx_spinlock, flags);

	shr_bufs = &ctx->shared_bufs;
	ret = set_encoding_done(shr_bufs);

	spin_unlock_irqrestore(&ctx->ctx_spinlock, flags);

	print_repeater_debug(RPT_EXT_INFO, "%s--\n", __func__);

	return ret;
}

void encoding_work_handler(struct work_struct *work)
{
	struct repeater_context *ctx;
	struct shared_buffer *shr_bufs;
	int32_t buf_idx = -1;
	int32_t ret;
	ktime_t cur_ktime;
	uint64_t target_timestamp;
	uint64_t cur_timestamp;
	int32_t required_delay;
	unsigned long flags;

	print_repeater_debug(RPT_INT_INFO, "%s++\n", __func__);

	ctx = container_of(work, struct repeater_context, encoding_work);
	if (!ctx || (ctx->ctx_status != REPEATER_CTX_START)) {
		print_repeater_debug(RPT_ERROR, "no repeater ctx\n");
		return;
	}

	spin_lock_irqsave(&ctx->ctx_spinlock, flags);

	shr_bufs = &ctx->shared_bufs;
	ret = get_latest_captured_buf_idx(shr_bufs, &buf_idx);

	spin_unlock_irqrestore(&ctx->ctx_spinlock, flags);

	cur_ktime = ktime_get();
	cur_timestamp = ktime_to_us(cur_ktime);
	if (ctx->encoding_start_timestamp == 0)
		ctx->encoding_start_timestamp = cur_timestamp;
	ctx->enc_param.time_stamp = ctx->encoding_start_timestamp +
		ctx->video_frame_count * ctx->encoding_period_us + ctx->paused_time;
	print_repeater_debug(RPT_EXT_INFO, "time_stamp %lld\n",
	ctx->enc_param.time_stamp);

	if (ret == 0 && buf_idx >= 0) {
		print_repeater_debug(RPT_EXT_INFO, "buf_idx %d\n", buf_idx);
		ctx->buf_idx_dump = buf_idx;
		wake_up_interruptible(&ctx->wait_queue_dump);
		spin_lock_irqsave(&ctx->ctx_spinlock, flags);
		ret = s5p_mfc_hwfc_encode(buf_idx, buf_idx, &ctx->enc_param);
		if (ret == 0)
			set_encoding_start(shr_bufs, buf_idx);
		spin_unlock_irqrestore(&ctx->ctx_spinlock, flags);
	}

	do {
		ctx->video_frame_count++;
		target_timestamp = ctx->encoding_start_timestamp +
			ctx->video_frame_count * ctx->encoding_period_us + ctx->paused_time;
		required_delay = target_timestamp - cur_timestamp;
	} while (required_delay < 0);

	print_repeater_debug(RPT_EXT_INFO, "timestamp cur %lld, target %lld\n",
		cur_timestamp, target_timestamp);

	usleep_range(required_delay, required_delay + 1);

	if (ctx->ctx_status == REPEATER_CTX_START)
		schedule_work(&ctx->encoding_work);

	print_repeater_debug(RPT_INT_INFO, "%s--\n", __func__);
}

void encoding_timer_callback(unsigned long arg)
{
	struct repeater_context *ctx = (struct repeater_context *)arg;
	struct shared_buffer *shr_bufs;
	int buf_idx = -1;
	int ret;
	int jiffies_delay;
	uint64_t my_jiffies;
	uint64_t cur_time_us;
	uint64_t elapsed_time_us;

	/* Fix Me: it is for 30 fps */
	jiffies_delay = 8;

	print_repeater_debug(RPT_EXT_INFO, "%s++\n", __func__);

	if (!ctx) {
		print_repeater_debug(RPT_ERROR, "no repeater device\n");
		return;
	}

	print_repeater_debug(RPT_EXT_INFO, "spin_trylock()\n");
	if (spin_trylock(&ctx->ctx_spinlock) == 0) {
		print_repeater_debug(RPT_EXT_INFO, "spin_trylock(), failed");
		ctx->encoding_timer.expires += jiffies_delay;
		add_timer(&ctx->encoding_timer);

		print_repeater_debug(RPT_EXT_INFO, "%s--\n", __func__);
		return;
	}

	shr_bufs = &ctx->shared_bufs;

	ret = get_latest_captured_buf_idx(shr_bufs, &buf_idx);

	my_jiffies = jiffies;
	cur_time_us = jiffies_to_usecs(my_jiffies);
	if (ctx->last_encoding_time_us == 0)
		ctx->last_encoding_time_us = cur_time_us;

	if (ctx->last_encoding_time_us <= cur_time_us) {
		elapsed_time_us = cur_time_us - ctx->last_encoding_time_us;
	} else {
		/* time was toggled*/
		elapsed_time_us = cur_time_us;
	}
	ctx->time_stamp_us += elapsed_time_us;
	ctx->last_encoding_time_us = cur_time_us;

	ctx->enc_param.time_stamp = ctx->time_stamp_us;

	if (ret == 0 && buf_idx >= 0) {
		print_repeater_debug(RPT_EXT_INFO, "buf_idx %d\n", buf_idx);
		ctx->buf_idx_dump = buf_idx;
		wake_up_interruptible(&ctx->wait_queue_dump);
		if (s5p_mfc_hwfc_encode(buf_idx, buf_idx, &ctx->enc_param) == 0) {
			set_encoding_start(shr_bufs, buf_idx);
		}
	}

	spin_unlock(&ctx->ctx_spinlock);
	print_repeater_debug(RPT_EXT_INFO, "spin_unlock()\n");

	ctx->encoding_timer.expires += jiffies_delay;
	add_timer(&ctx->encoding_timer);

	print_repeater_debug(RPT_EXT_INFO, "%s--\n", __func__);
}

int repeater_ioctl_map_buf(struct repeater_context *ctx)
{
	int ret = 0;
	int i = 0;

	print_repeater_debug(RPT_INT_INFO, "%s++\n", __func__);

	if (ctx == NULL || ctx->repeater_dev == NULL)
		return -ENOMEM;

	if (ctx->ctx_status != REPEATER_CTX_INIT)
		return -ENOMEM;

	if (ctx->info.buffer_count > MAX_SHARED_BUF_NUM) {
		ctx->info.buffer_count = 0;
		return -ENOMEM;
	}

	if (ctx->info.fps != 30 && ctx->info.fps != 60)
		return -ENOMEM;

	for (i = 0; i < ctx->info.buffer_count; i++) {
		ctx->dmabufs[i] = dma_buf_get(ctx->info.buf_fd[i]);
		print_repeater_debug(RPT_INT_INFO,
			"dmabufs[%i] %p\n", i, ctx->dmabufs[i]);
	}

	ctx->ctx_status = REPEATER_CTX_MAP;
	ctx->encoding_period_us = ENCODING_PERIOD_TIME_US / ctx->info.fps;

	print_repeater_debug(RPT_INT_INFO, "%s--\n", __func__);

	return ret;
}

int repeater_ioctl_unmap_buf(struct repeater_context *ctx)
{
	int ret = 0;
	int i;

	print_repeater_debug(RPT_INT_INFO, "%s++\n", __func__);

	if (ctx == NULL || ctx->repeater_dev == NULL)
		return -ENOMEM;

	if (ctx->ctx_status != REPEATER_CTX_MAP)
		return -ENOMEM;

	ctx->ctx_status = REPEATER_CTX_INIT;
	for (i = 0; i < ctx->info.buffer_count; i++) {
		if (!IS_ERR_OR_NULL(ctx->dmabufs[i])) {
			dma_buf_put(ctx->dmabufs[i]);
			ctx->dmabufs[i] = 0;
		}
	}

	print_repeater_debug(RPT_INT_INFO, "%s--\n", __func__);

	return ret;
}

int repeater_ioctl_start(struct repeater_context *ctx)
{
	int ret = 0;

	print_repeater_debug(RPT_INT_INFO, "%s++\n", __func__);

	if (ctx == NULL || ctx->repeater_dev == NULL)
		return -ENOMEM;

	if (ctx->ctx_status != REPEATER_CTX_MAP)
		return -ENOMEM;

	ctx->ctx_status = REPEATER_CTX_START;

	if (ctx->info.fps != 60) {
#ifdef USES_WORKQUEUE
		schedule_work(&ctx->encoding_work);
#else
		ctx->encoding_timer.expires = jiffies +
			usecs_to_jiffies(ctx->encoding_period_us);
#endif
	}
	print_repeater_debug(RPT_INT_INFO, "%s--\n", __func__);

	return ret;
}

int repeater_ioctl_stop(struct repeater_context *ctx)
{
	int ret = 0;

	print_repeater_debug(RPT_INT_INFO, "%s++\n", __func__);

	if (ctx == NULL || ctx->repeater_dev == NULL)
		return -ENOMEM;

	if (ctx->ctx_status != REPEATER_CTX_START &&
			ctx->ctx_status != REPEATER_CTX_PAUSE)
		return -ENOMEM;

	ctx->ctx_status = REPEATER_CTX_MAP;
#ifndef USES_WORKQUEUE
	del_timer_sync(&ctx->encoding_timer);
#endif
	print_repeater_debug(RPT_INT_INFO, "%s--\n", __func__);

	return ret;
}

int repeater_ioctl_pause(struct repeater_context *ctx)
{
	int ret = 0;
	ktime_t cur_ktime;

	print_repeater_debug(RPT_INT_INFO, "%s++\n", __func__);

	if (ctx == NULL || ctx->repeater_dev == NULL)
		return -ENOMEM;

	cur_ktime = ktime_get();
	ctx->pause_time = ktime_to_us(cur_ktime);
	ctx->ctx_status = REPEATER_CTX_PAUSE;

	print_repeater_debug(RPT_INT_INFO, "%s--\n", __func__);

	return ret;
}

int repeater_ioctl_resume(struct repeater_context *ctx)
{
	int ret = 0;
	ktime_t cur_ktime;

	print_repeater_debug(RPT_INT_INFO, "%s++\n", __func__);

	if (ctx == NULL || ctx->repeater_dev == NULL)
		return -ENOMEM;

	if (ctx->ctx_status != REPEATER_CTX_PAUSE)
		print_repeater_debug(RPT_ERROR, "repeater is not pause status\n");

	cur_ktime = ktime_get();
	ctx->resume_time = ktime_to_us(cur_ktime);
	ctx->paused_time += ctx->resume_time - ctx->pause_time;

	ctx->ctx_status = REPEATER_CTX_START;
	schedule_work(&ctx->encoding_work);

	print_repeater_debug(RPT_INT_INFO, "%s--\n", __func__);

	return ret;
}

int repeater_ioctl_dump(struct repeater_context *ctx)
{
	int ret = 0;
	unsigned long jiffies_from_usec = usecs_to_jiffies(MAX_WAIT_TIME_DUMP);

	print_repeater_debug(RPT_INT_INFO, "%s++\n", __func__);

	if (ctx == NULL || ctx->repeater_dev == NULL)
		return -ENOMEM;

	if (ctx->ctx_status != REPEATER_CTX_START)
		return -ENOMEM;

	ret = wait_event_interruptible_timeout(ctx->wait_queue_dump,
		ctx->buf_idx_dump >= 0, jiffies_from_usec);

	print_repeater_debug(RPT_INT_INFO,
		"wait_event_interruptible_timeout(%lu) ret %d, ctx->buf_idx_dump %d\n",
		jiffies_from_usec, ret, ctx->buf_idx_dump);

	print_repeater_debug(RPT_INT_INFO, "%s--\n", __func__);

	return ret;
}

static int repeater_open(struct inode *inode, struct file *filp)
{
	int ret = 0;
	struct repeater_device *repeater_dev = container_of(filp->private_data,
		struct repeater_device, misc_dev);
	struct repeater_context *ctx;
	struct shared_buffer *shr_bufs;

	print_repeater_debug(RPT_INT_INFO, "%s++\n", __func__);

	if (repeater_dev->ctx_num >= REPEATER_MAX_CONTEXTS_NUM) {
		print_repeater_debug(RPT_ERROR, "too many context\n");
		ret = -ENOMEM;
		goto err_init;
	}

	ctx = kzalloc(sizeof(struct repeater_context), GFP_KERNEL);
	if (!ctx) {
		ret = -ENOMEM;
		goto err_init;
	}

	ctx->repeater_dev = repeater_dev;
	repeater_dev->ctx[repeater_dev->ctx_num] = ctx;
	repeater_dev->ctx_num++;

	spin_lock_init(&ctx->ctx_spinlock);

	filp->private_data = ctx;

	ctx->ctx_status = REPEATER_CTX_INIT;

	shr_bufs = &ctx->shared_bufs;
	init_shared_buffer(shr_bufs, MAX_SHARED_BUF_NUM);
	ctx->enc_param.time_stamp = 0;
#ifdef USES_WORKQUEUE
	ctx->encoding_start_timestamp = 0;
	ctx->video_frame_count = 0;
	ctx->paused_time = 0;
	ctx->pause_time = 0;
	ctx->resume_time = 0;
	INIT_WORK(&ctx->encoding_work, encoding_work_handler);
#else
	init_timer(&ctx->encoding_timer);
	ctx->encoding_timer.data = (unsigned long)ctx;
	ctx->encoding_timer.function = encoding_timer_callback;
#endif
	ctx->time_stamp_us = 33333;
	ctx->last_encoding_time_us = 0;

	g_repeater_device = repeater_dev;

	init_waitqueue_head(&ctx->wait_queue_dump);
	ctx->buf_idx_dump = -1;

	pm_runtime_get_sync(repeater_dev->dev);

	print_repeater_debug(RPT_INT_INFO, "%s--\n", __func__);

	return ret;

err_init:
	print_repeater_debug(RPT_ERROR, "%s--, err_init\n", __func__);

	return ret;
}

static int repeater_release(struct inode *inode, struct file *filp)
{
	int ret = 0;
	struct repeater_context *ctx = filp->private_data;
	struct repeater_device *repeater_dev = ctx->repeater_dev;

	print_repeater_debug(RPT_INT_INFO, "%s++\n", __func__);

	if (ctx->ctx_status == REPEATER_CTX_START ||
			ctx->ctx_status == REPEATER_CTX_PAUSE)
		repeater_ioctl_stop(ctx);

	if (ctx->ctx_status == REPEATER_CTX_MAP)
		repeater_ioctl_unmap_buf(ctx);

	pm_runtime_put_sync(repeater_dev->dev);

	ctx->repeater_dev->ctx_num--;
	kfree(ctx);
	filp->private_data = NULL;

	g_repeater_device = NULL;

	print_repeater_debug(RPT_INT_INFO, "%s--\n", __func__);
	return ret;
}

static long repeater_ioctl(struct file *filp,
	unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	struct repeater_context *ctx;

	print_repeater_debug(RPT_INT_INFO, "%s++\n", __func__);

	ctx = filp->private_data;
	if (!ctx) {
		print_repeater_debug(RPT_ERROR, "ctx is NULL\n");
		ret = -ENOTTY;
		return ret;
	}

	switch (cmd) {
	case REPEATER_IOCTL_MAP_BUF:
		if (copy_from_user(&ctx->info,
			(struct repeater_info __user *)arg,
			sizeof(struct repeater_info))) {
			ret = -EFAULT;
			break;
		}

		ret = repeater_ioctl_map_buf(ctx);

		if (copy_to_user((struct repeater_info __user *)arg,
			&ctx->info,
			sizeof(struct repeater_info))) {
			ret = -EFAULT;
			break;
		}
	break;

	case REPEATER_IOCTL_UNMAP_BUF:
		ret = repeater_ioctl_unmap_buf(ctx);
	break;

	case REPEATER_IOCTL_START:
		ret = repeater_ioctl_start(ctx);
	break;

	case REPEATER_IOCTL_STOP:
		ret = repeater_ioctl_stop(ctx);
	break;

	case REPEATER_IOCTL_PAUSE:
		ret = repeater_ioctl_pause(ctx);
	break;

	case REPEATER_IOCTL_RESUME:
		ret = repeater_ioctl_resume(ctx);
	break;

	case REPEATER_IOCTL_DUMP:
		ret = repeater_ioctl_dump(ctx);
		if (copy_to_user((int __user *)arg,
			&ctx->buf_idx_dump,
			sizeof(int))) {
			ret = -EFAULT;
			break;
		}
		ctx->buf_idx_dump = -1;
	break;

	default:
		ret = -ENOTTY;
	}

	print_repeater_debug(RPT_INT_INFO, "%s--\n", __func__);
	return ret;

}

static const struct file_operations repeater_fops = {
	.owner          = THIS_MODULE,
	.open           = repeater_open,
	.release        = repeater_release,
	.unlocked_ioctl = repeater_ioctl,
	.compat_ioctl = repeater_ioctl,
};

static int repeater_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct repeater_device *repeater_dev;

	print_repeater_debug(RPT_INT_INFO, "%s++\n", __func__);

	repeater_dev = devm_kzalloc(&pdev->dev, sizeof(struct repeater_device),
					GFP_KERNEL);
	if (!repeater_dev)
		return -ENOMEM;

	repeater_dev->ctx_num = 0;
	repeater_dev->dev = &pdev->dev;
	repeater_dev->misc_dev.minor = MISC_DYNAMIC_MINOR;
	repeater_dev->misc_dev.fops = &repeater_fops;
	repeater_dev->misc_dev.name = NODE_NAME;
	ret = misc_register(&repeater_dev->misc_dev);
	if (ret)
		goto err_misc_register;

	platform_set_drvdata(pdev, repeater_dev);

	pm_runtime_enable(&pdev->dev);

	g_repeater_debug_level = RPT_ERROR;

	print_repeater_debug(RPT_INT_INFO, "%s--\n", __func__);

	return ret;

err_misc_register:
	print_repeater_debug(RPT_ERROR, "%s--, err_misc_dev\n", __func__);

	return ret;
}

static int repeater_remove(struct platform_device *pdev)
{
	struct repeater_device *repeater_dev = platform_get_drvdata(pdev);

	print_repeater_debug(RPT_INT_INFO, "%s++\n", __func__);

	pm_runtime_disable(&pdev->dev);

	if (repeater_dev) {
		misc_deregister(&repeater_dev->misc_dev);
		kfree(repeater_dev);
	}

	print_repeater_debug(RPT_INT_INFO, "%s--\n", __func__);

	return 0;
}

static void repeater_shutdown(struct platform_device *pdev)
{
	print_repeater_debug(RPT_INT_INFO, "%s++\n", __func__);

	print_repeater_debug(RPT_INT_INFO, "%s--\n", __func__);
}

static const struct of_device_id exynos_repeater_match[] = {
	{
		.compatible = "samsung,exynos-repeater",
	},
	{},
};
MODULE_DEVICE_TABLE(of, exynos_repeater_match);

static struct platform_driver repeater_driver = {
	.probe		= repeater_probe,
	.remove		= repeater_remove,
	.shutdown	= repeater_shutdown,
	.driver = {
		.name	= MODULE_NAME,
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(exynos_repeater_match),
	}
};

module_platform_driver(repeater_driver);

MODULE_AUTHOR("Shinwon Lee <shinwon.lee@samsung.com>");
MODULE_DESCRIPTION("EXYNOS repeater driver");
MODULE_LICENSE("GPL");
