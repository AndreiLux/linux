/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
 *	      http://www.samsung.com/
 *
 * Samsung's POC Driver
 * Author: ChangJae Jang <cj1225.jang@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef SAMSUNG_POC_COMMON_H
#define SAMSUNG_POC_COMMON_H

#include <linux/kernel.h>
#include <linux/poll.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/err.h>
#include <linux/mutex.h>

#ifdef CONFIG_SUPPORT_POC_2_0
#define POC_IMG_SIZE    (546008)
#else
#define POC_IMG_SIZE    (532816)
#endif

#define POC_IMG_ADDR	(0x000000)
#define POC_PAGE		(4096)
#define POC_TEST_PATTERN_SIZE	(1024)

/* Register to cnotrol POC */
#define POC_CTRL_REG	0xEB

#endif
