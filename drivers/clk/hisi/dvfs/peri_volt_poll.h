/*
 * Copyright (C) 2015
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __PERIVOLT_POLL_EXTERN_H
#define __PERIVOLT_POLL_EXTERN_H
#include "peri_volt_internal.h"
/*
 *960 : 0.7v ,  ,  , 0.8v
 *970 ES : 0.7v ,  ,  , 0.8v
 *970 CS : 0.65v , , 0.75v, 0.8v
 */
enum {
	PERI_VOLT_0 = 0,
	PERI_VOLT_1,
	PERI_VOLT_2,
	PERI_VOLT_3,
};

struct peri_volt_poll *peri_volt_poll_get(unsigned int dev_id, const char *name);
unsigned int peri_get_volt(struct peri_volt_poll *pvp);
int peri_set_volt(struct peri_volt_poll *pvp, unsigned int volt);
int peri_poll_stat(struct peri_volt_poll *pvp);

#endif /* __PERIVOLT_POLL_INTERNAL_H */
