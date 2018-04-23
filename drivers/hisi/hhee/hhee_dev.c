/*
 * Hisilicon HHEE exception driver .
 *
 * Copyright (c) 2012-2013 Linaro Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/io.h>
#include <linux/kthread.h>
#include <linux/hisi/rdr_pub.h>
#include <linux/hisi/util.h>
#include <linux/hisi/rdr_hisi_platform.h>
#include <linux/compiler.h>
#include <linux/interrupt.h>
#include <asm/compiler.h>
#include <linux/debugfs.h>
#include "hhee.h"
#include "hhee_msg.h"

struct rdr_exception_info_s hhee_excetption_info[] = {
	{
		.e_modid            = MODID_AP_S_HHEE_PANIC,
		.e_modid_end        = MODID_AP_S_HHEE_PANIC,
		.e_process_priority = RDR_ERR,
		.e_reboot_priority  = RDR_REBOOT_NOW,
		.e_notify_core_mask = RDR_AP,
		.e_reset_core_mask  = RDR_AP,
		.e_from_core        = RDR_AP,
		.e_reentrant        = (u32)RDR_REENTRANT_DISALLOW,
		.e_exce_type        = AP_S_HHEE_PANIC,
		.e_upload_flag      = (u32)RDR_UPLOAD_YES,
		.e_from_module      = "RDR HHEE PANIC",
		.e_desc             = "RDR HHEE PANIC"
	}/*lint !e785*/
};

int hhee_panic_handle(unsigned short handle, int len, char *buf)
{
	pr_info("hhee_panic %u, %d\n", handle, len);

	while (1) {
		pr_err("hhee panic trigger system_error.\n");
		rdr_syserr_process_for_ap((u32)MODID_AP_S_HHEE_PANIC, 0ULL, 0ULL);
		break;
	}
	return 0;/*lint !e533*/
}/*lint !e715*/

/*check hhee enable*/
static int g_hhee_enable = HHEE_ENABLE;
int hhee_check_enable(void)
{
	return g_hhee_enable;
}
EXPORT_SYMBOL(hhee_check_enable);

#define CPU_MASK		(0xF)

static int hisi_hhee_probe(struct platform_device *pdev)
{
	int ret, irq;
	struct device *dev = &pdev->dev;

	pr_info("hhee panic probe\n");

	if (HHEE_DISABLE == hhee_check_enable())
		return 0;

	/*irq num get and register*/
	irq = platform_get_irq(pdev, 0);
	if (irq < 0) {
		pr_err("hhee: get irq failed\n");
		return -ENXIO;
	}
	ret = devm_request_irq(dev, (unsigned int)irq,
						hhee_irq_handle, 0ul, "hisi-hhee", pdev);
	if (ret < 0) {
		pr_err("devm request irq failed\n");
		return -EINVAL;
	}
	if(cpu_online(CPU_MASK) && (irq_set_affinity(irq, cpumask_of(CPU_MASK)) < 0)){
		pr_err("set affinity failed\n");
		return -ENXIO;
	}
	/*rdr struct register*/
	ret = (s32)rdr_register_exception(&hhee_excetption_info[0]);
	if (!ret)
		pr_err("register hhee exception fail.\n");

	ret = hhee_logger_init();
	if (ret < 0) {
		ret = -EINVAL;
		goto err_free_hhee;
	}


	if (!hhee_open_msg("Crash", hhee_panic_handle)) {
		pr_err("create hhee_panic_handle faild.\n");
		ret = -EINVAL;
		goto err_free_hhee;
	}

	pr_info("hhee probe done\n");
	return 0;

err_free_hhee:
	return ret;
}

void hhee_version(void)
{
	hhee_fn_hvc((unsigned long)ARM_STD_HVC_VERSION, 0UL, 0UL, 0UL);
}

static int hisi_hhee_remove(struct platform_device *pdev)
{
	return 0;
}/*lint !e715*/

/*lint -e785 -esym(785,*)*/
static const struct of_device_id hisi_hhee_of_match[] = {
	{.compatible = "hisi,hisi-hhee"},
	{},
};

static struct platform_driver hisi_hhee_driver = {
	.driver = {
		.owner = THIS_MODULE, /*lint !e64*/
		.name = "hisi-hhee",
		.of_match_table = of_match_ptr(hisi_hhee_of_match),
	},
	.probe = hisi_hhee_probe,
	.remove = hisi_hhee_remove,
};
/*lint -e785 +esym(785,*)*/

static int __init hisi_hhee_cmd_get(char *arg)
{
	if (!arg)
		return -EINVAL;

	if (!strncmp(arg, "false", strlen("false"))){
		g_hhee_enable = HHEE_DISABLE;
	}

	pr_err("hhee enable = %d \n", g_hhee_enable);
	return 0;
}
early_param("hhee_enable", hisi_hhee_cmd_get); /*lint -e528 */

static int __init hisi_hhee_panic_init(void)
{
	int ret;

	pr_info("hhee panic init\n");
	ret = platform_driver_register(&hisi_hhee_driver);/*lint !e64*/
	if (ret)
		pr_err("register hhee driver fail\n");

	return ret;
}

static void __exit hisi_hhee_panic_exit(void)
{
	platform_driver_unregister(&hisi_hhee_driver);
}

/*lint -e528 -esym(528,*)*/
/*lint -e753 -esym(753,*)*/
module_init(hisi_hhee_panic_init);
module_exit(hisi_hhee_panic_exit);

MODULE_DESCRIPTION("hisi hhee exception driver");
MODULE_ALIAS("hisi hhee exception module");
MODULE_LICENSE("GPL");
/*lint -e528 +esym(528,*)*/
/*lint -e753 +esym(753,*)*/
