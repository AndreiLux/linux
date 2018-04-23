/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2012-2015. All rights reserved.
 * foss@huawei.com
 *
 * If distributed as part of the Linux kernel, the following license terms
 * apply:
 *
 * * This program is free software; you can redistribute it and/or modify
 * * it under the terms of the GNU General Public License version 2 and
 * * only version 2 as published by the Free Software Foundation.
 * *
 * * This program is distributed in the hope that it will be useful,
 * * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * * GNU General Public License for more details.
 * *
 * * You should have received a copy of the GNU General Public License
 * * along with this program; if not, write to the Free Software
 * * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA
 *
 * Otherwise, the following license terms apply:
 *
 * * Redistribution and use in source and binary forms, with or without
 * * modification, are permitted provided that the following conditions
 * * are met:
 * * 1) Redistributions of source code must retain the above copyright
 * *    notice, this list of conditions and the following disclaimer.
 * * 2) Redistributions in binary form must reproduce the above copyright
 * *    notice, this list of conditions and the following disclaimer in the
 * *    documentation and/or other materials provided with the distribution.
 * * 3) Neither the name of Huawei nor the names of its contributors may
 * *    be used to endorse or promote products derived from this software
 * *    without specific prior written permission.
 *
 * * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include <linux/slab.h>
#include <linux/syscalls.h>
#include <linux/kernel.h>
#include <linux/rtc.h>
#include <asm/string.h>
#include "drv_comm.h"
#include "osl_types.h"
#include "bsp_dump.h"
#include "bsp_slice.h"
#include "bsp_reset.h"
#include "bsp_coresight.h"
#include "bsp_wdt.h"
#include "gunas_errno.h"
#include "dump_modem.h"
#include "dump_modem_rdr.h"
#include "dump_config.h"
#include "dump_print.h"
#include "dump_modem_baseinfo.h"
#include "dump_cp_agent.h"
#include "dump_modem_hook.h"
#include "dump_modem_area.h"
#include "dump_modem_field.h"
#include "dump_cp_wdt.h"
#include "dump_modem_save.h"
#include "dump_exc_ctrl.h"
#include "dump_config.h"


rdr_exc_info_s     g_rdr_exc_info;

struct rdr_exception_info_s g_phone_exc_info[] = {
    {
        .e_modid            = (unsigned int)RDR_MODEM_AP_MOD_ID,
        .e_modid_end        = (unsigned int)RDR_MODEM_AP_MOD_ID,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_LPM3 ,
        .e_reset_core_mask  = RDR_AP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_DISALLOW,
        .e_exce_type        = CP_S_MODEMAP,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP",
        .e_desc             = "modem ap reset system",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_AP_DRV_MOD_ID,
        .e_modid_end        = (unsigned int)RDR_MODEM_AP_DRV_MOD_ID,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP ,
        .e_reset_core_mask  = RDR_CP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_DISALLOW,
        .e_exce_type        = CP_S_MODEMAP,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP",
        .e_desc             = "modem ap drv reset system",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_CP_MOD_ID,
        .e_modid_end        = (unsigned int)RDR_MODEM_CP_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_CP| RDR_HIFI | RDR_LPM3,
        .e_reset_core_mask  = RDR_CP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type        = CP_S_EXCEPTION,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP",
        .e_desc             = "modem cp exc",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_CP_DRV_MOD_ID,
        .e_modid_end        = (unsigned int)RDR_MODEM_CP_DRV_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_CP| RDR_HIFI | RDR_LPM3,
        .e_reset_core_mask  = RDR_CP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type        = CP_S_DRV_EXC,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP",
        .e_desc             = "modem cp drv exc",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_CP_OSA_MOD_ID,
        .e_modid_end        = (unsigned int)RDR_MODEM_CP_OSA_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_CP| RDR_HIFI | RDR_LPM3,
        .e_reset_core_mask  = RDR_CP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type        = CP_S_PAM_EXC,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP",
        .e_desc             = "modem cp osa exc",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_CP_OAM_MOD_ID,
        .e_modid_end        = (unsigned int)RDR_MODEM_CP_OAM_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_CP| RDR_HIFI | RDR_LPM3,
        .e_reset_core_mask  = RDR_CP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type        = CP_S_PAM_EXC,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP",
        .e_desc             = "modem cp oam exc",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_CP_GUL2_MOD_ID,
        .e_modid_end        = (unsigned int)RDR_MODEM_CP_GUL2_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_CP| RDR_HIFI | RDR_LPM3,
        .e_reset_core_mask  = RDR_CP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type        = CP_S_GUAS_EXC,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP",
        .e_desc             = "modem cp gul2 exc",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_CP_CTTF_MOD_ID,
        .e_modid_end        = (unsigned int)RDR_MODEM_CP_CTTF_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_CP| RDR_HIFI | RDR_LPM3,
        .e_reset_core_mask  = RDR_CP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type        = CP_S_CTTF_EXC,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP",
        .e_desc             = "modem cp cttf exc",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_CP_GUWAS_MOD_ID,
        .e_modid_end        = (unsigned int)RDR_MODEM_CP_GUWAS_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_CP| RDR_HIFI | RDR_LPM3,
        .e_reset_core_mask  = RDR_CP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type        = CP_S_GUAS_EXC,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP",
        .e_desc             = "modem cp guwas exc",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_CP_CAS_MOD_ID,
        .e_modid_end        = (unsigned int)RDR_MODEM_CP_CAS_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_CP| RDR_HIFI | RDR_LPM3,
        .e_reset_core_mask  = RDR_CP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type        = CP_S_CAS_CPROC_EXC,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP",
        .e_desc             = "modem cp cas exc",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_CP_CPROC_MOD_ID,
        .e_modid_end        = (unsigned int)RDR_MODEM_CP_CPROC_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_CP| RDR_HIFI | RDR_LPM3,
        .e_reset_core_mask  = RDR_CP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type        = CP_S_CAS_CPROC_EXC,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP",
        .e_desc             = "modem cp cproc exc",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_CP_GUGAS_MOD_ID,
        .e_modid_end        = (unsigned int)RDR_MODEM_CP_GUGAS_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_CP| RDR_HIFI | RDR_LPM3,
        .e_reset_core_mask  = RDR_CP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type        = CP_S_GUAS_EXC,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP",
        .e_desc             = "modem cp guas exc",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_CP_GUCNAS_MOD_ID,
        .e_modid_end        = (unsigned int)RDR_MODEM_CP_GUCNAS_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_CP| RDR_HIFI | RDR_LPM3,
        .e_reset_core_mask  = RDR_CP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type        = CP_S_GUCNAS_EXC,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP",
        .e_desc             = "modem cp gucnas exc",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_CP_GUDSP_MOD_ID,
        .e_modid_end        = (unsigned int)RDR_MODEM_CP_GUDSP_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_CP| RDR_HIFI | RDR_LPM3,
        .e_reset_core_mask  = RDR_CP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type        = CP_S_GUDSP_EXC,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP",
        .e_desc             = "modem cp gudsp exc",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_CP_LPS_MOD_ID,
        .e_modid_end        = (unsigned int)RDR_MODEM_CP_LPS_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_CP| RDR_HIFI | RDR_LPM3,
        .e_reset_core_mask  = RDR_CP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type        = CP_S_TLPS_EXC,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP",
        .e_desc             = "modem cp tlps exc",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_CP_LMSP_MOD_ID,
        .e_modid_end        = (unsigned int)RDR_MODEM_CP_LMSP_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_CP| RDR_HIFI | RDR_LPM3,
        .e_reset_core_mask  = RDR_CP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type        = CP_S_DRV_EXC,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP",
        .e_desc             = "modem cp lmsp exc",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_CP_TLDSP_MOD_ID,
        .e_modid_end        = (unsigned int)RDR_MODEM_CP_TLDSP_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_CP| RDR_HIFI | RDR_LPM3,
        .e_reset_core_mask  = RDR_CP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type        = CP_S_TLDSP_EXC,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP",
        .e_desc             = "modem cp tldsp exc",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_CP_CPHY_MOD_ID,
        .e_modid_end        = (unsigned int)RDR_MODEM_CP_CPHY_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_CP| RDR_HIFI | RDR_LPM3,
        .e_reset_core_mask  = RDR_CP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type        = CP_S_CPHY_EXC,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP",
        .e_desc             = "modem cp cphy exc",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_CP_IMS_MOD_ID,
        .e_modid_end        = (unsigned int)RDR_MODEM_CP_IMS_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_CP| RDR_HIFI | RDR_LPM3,
        .e_reset_core_mask  = RDR_CP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type        = CP_S_EXCEPTION,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP",
        .e_desc             = "modem cp ims exc",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_CP_RESET_SIM_SWITCH_MOD_ID,
        .e_modid_end        = (unsigned int)RDR_MODEM_CP_RESET_SIM_SWITCH_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = 0,
        .e_reset_core_mask  = RDR_CP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type        = CP_S_NORMALRESET,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP",
        .e_desc             = "modem normal reboot",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_CP_RESET_FAIL_MOD_ID,
        .e_modid_end        = (unsigned int)RDR_MODEM_CP_RESET_FAIL_MOD_ID,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_LPM3,
        .e_reset_core_mask  = RDR_AP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_DISALLOW,
        .e_exce_type        = CP_S_RESETFAIL,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP",
        .e_desc             = "modem self-reset fail",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_CP_RESET_FREQUENTLY_MOD_ID,
        .e_modid_end        = (unsigned int)RDR_MODEM_CP_RESET_FREQUENTLY_MOD_ID,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_LPM3,
        .e_reset_core_mask  = RDR_AP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_DISALLOW,
        .e_exce_type        = CP_S_RESETFAIL,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP",
        .e_desc             = "modem reset frequently",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_CP_WDT_MOD_ID,
        .e_modid_end        = (unsigned int)RDR_MODEM_CP_WDT_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_CP | RDR_LPM3,
        .e_reset_core_mask  = RDR_CP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type        = CP_S_EXCEPTION,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP",
        .e_desc             = "modem self-reset wdt",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_CP_RESET_RILD_MOD_ID,
        .e_modid_end        = (unsigned int)RDR_MODEM_CP_RESET_RILD_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_CP | RDR_LPM3,
        .e_reset_core_mask  = RDR_CP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type        = CP_S_RILD_EXCEPTION,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP",
        .e_desc             = "modem reset by rild",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_CP_RESET_3RD_MOD_ID,
        .e_modid_end        = (unsigned int)RDR_MODEM_CP_RESET_3RD_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_CP | RDR_HIFI | RDR_LPM3,
        .e_reset_core_mask  = RDR_CP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type        = CP_S_3RD_EXCEPTION,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP",
        .e_desc             = "modem reset by 3rd modem",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_CP_NOC_MOD_ID,
        .e_modid_end        = (unsigned int)RDR_MODEM_CP_NOC_MOD_ID,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_CP| RDR_LPM3 ,
        .e_reset_core_mask  = RDR_CP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type        = CP_S_MODEMNOC,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP",
        .e_desc             = "modem noc reset",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_CP_RESET_REBOOT_REQ_MOD_ID,
        .e_modid_end        = (unsigned int)RDR_MODEM_CP_RESET_REBOOT_REQ_MOD_ID,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_LPM3 ,
        .e_reset_core_mask  = RDR_AP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_DISALLOW,
        .e_exce_type        = CP_S_NORMALRESET,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP",
        .e_desc             = "modem reset stub",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_NOC_MOD_ID,
        .e_modid_end        = (unsigned int)RDR_MODEM_NOC_MOD_ID,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP| RDR_LPM3 ,
        .e_reset_core_mask  = RDR_AP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_DISALLOW,
        .e_exce_type        = CP_S_MODEMNOC,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP",
        .e_desc             = "modem noc error",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_AP_NOC_MOD_ID,
        .e_modid_end        = (unsigned int)RDR_MODEM_AP_NOC_MOD_ID,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP| RDR_LPM3 ,
        .e_reset_core_mask  = RDR_AP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_DISALLOW,
        .e_exce_type        = CP_S_MODEMNOC,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP",
        .e_desc             = "modem noc reset system",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_CP_RESET_USER_RESET_MOD_ID,
        .e_modid_end        = (unsigned int)RDR_MODEM_CP_RESET_USER_RESET_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = 0,
        .e_reset_core_mask  = RDR_CP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type        = CP_S_NORMALRESET,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP",
        .e_desc             = "modem user reset without log",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_DMSS_MOD_ID,
        .e_modid_end        = (unsigned int)RDR_MODEM_DMSS_MOD_ID,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_LPM3 ,
        .e_reset_core_mask  = RDR_AP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_DISALLOW,
        .e_exce_type        = CP_S_MODEMDMSS,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP",
        .e_desc             = "modem dmss error",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_CP_RESET_DLOCK_MOD_ID,
        .e_modid_end        = (unsigned int)RDR_MODEM_CP_RESET_DLOCK_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP |RDR_CP | RDR_LPM3,
        .e_reset_core_mask  = RDR_CP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type        = CP_S_EXCEPTION,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP DLOCK",
        .e_desc             = "modem reset by bus error",
    },

};
struct rdr_exception_info_s g_mbb_exc_info[] =
{
    {
        .e_modid            = (unsigned int)RDR_MODEM_AP_MOD_ID,
        .e_modid_end        = (unsigned int)RDR_MODEM_AP_MOD_ID,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_CP | RDR_LPM3 ,
        .e_reset_core_mask  = RDR_AP,
        .e_from_core        = RDR_AP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_DISALLOW,
        .e_exce_type        = CP_S_MODEMAP,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "MDMAP",
        .e_desc             = "modem ap reset system",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_CP_DRV_MOD_ID_START,
        .e_modid_end        = (unsigned int)RDR_MODEM_CP_DRV_MOD_ID_END,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_LPM3,
        .e_reset_core_mask  = RDR_AP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type        = CP_S_EXCEPTION,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP DRV",
        .e_desc             = "modem cp DRV reset",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_CP_OSA_MOD_ID_START,
        .e_modid_end        = (unsigned int)RDR_MODEM_CP_OSA_MOD_ID_END,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_LPM3,
        .e_reset_core_mask  = RDR_AP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type        = CP_S_EXCEPTION,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP OSA",
        .e_desc             = "modem cp OSA reset",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_CP_OAM_MOD_ID_START,
        .e_modid_end        = (unsigned int)RDR_MODEM_CP_OAM_MOD_ID_END,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_LPM3,
        .e_reset_core_mask  = RDR_AP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type        = CP_S_EXCEPTION,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP OAM",
        .e_desc             = "modem cp OAM reset",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_CP_GUL2_MOD_ID_START,
        .e_modid_end        = (unsigned int)RDR_MODEM_CP_GUL2_MOD_ID_END,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_LPM3,
        .e_reset_core_mask  = RDR_AP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type        = CP_S_EXCEPTION,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP GUL2",
        .e_desc             = "modem cp GUL2 reset",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_CP_CTTF_MOD_ID_START,
        .e_modid_end        = (unsigned int)RDR_MODEM_CP_CTTF_MOD_ID_END,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_LPM3,
        .e_reset_core_mask  = RDR_AP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type        = CP_S_EXCEPTION,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP CTTF",
        .e_desc             = "modem cp CTTF reset",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_CP_GUWAS_MOD_ID_START,
        .e_modid_end        = (unsigned int)RDR_MODEM_CP_GUWAS_MOD_ID_END,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_LPM3,
        .e_reset_core_mask  = RDR_AP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type        = CP_S_EXCEPTION,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP GUWAS",
        .e_desc             = "modem cp GUWAS reset",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_CP_CAS_MOD_ID_START,
        .e_modid_end        = (unsigned int)RDR_MODEM_CP_CAS_MOD_ID_END,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_LPM3,
        .e_reset_core_mask  = RDR_AP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type        = CP_S_EXCEPTION,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP CAS",
        .e_desc             = "modem cp CAS reset",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_CP_CPROC_MOD_ID_START,
        .e_modid_end        = (unsigned int)RDR_MODEM_CP_CPROC_MOD_ID_END,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_LPM3,
        .e_reset_core_mask  = RDR_AP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type        = CP_S_EXCEPTION,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP CPROC",
        .e_desc             = "modem cp CPROC reset",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_CP_GUCNAS_MOD_ID_START,
        .e_modid_end        = (unsigned int)RDR_MODEM_CP_GUCNAS_MOD_ID_END,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_LPM3,
        .e_reset_core_mask  = RDR_AP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type        = CP_S_EXCEPTION,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP GUCNAS",
        .e_desc             = "modem cp GUCNAS reset",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_CP_GUDSP_MOD_ID_START,
        .e_modid_end        = (unsigned int)RDR_MODEM_CP_GUDSP_MOD_ID_END,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_LPM3,
        .e_reset_core_mask  = RDR_AP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type        = CP_S_EXCEPTION,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP GUDSP",
        .e_desc             = "modem cp GUDSP reset",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_CP_LPS_MOD_ID_START,
        .e_modid_end        = (unsigned int)RDR_MODEM_CP_LPS_MOD_ID_END,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_LPM3,
        .e_reset_core_mask  = RDR_AP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type        = CP_S_EXCEPTION,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP LPS",
        .e_desc             = "modem cp LPS reset",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_CP_LMSP_MOD_ID_START,
        .e_modid_end        = (unsigned int)RDR_MODEM_CP_LMSP_MOD_ID_END,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_LPM3,
        .e_reset_core_mask  = RDR_AP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type        = CP_S_EXCEPTION,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP LMSP",
        .e_desc             = "modem cp LMSP reset",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_CP_TLDSP_MOD_ID_START,
        .e_modid_end        = (unsigned int)RDR_MODEM_CP_TLDSP_MOD_ID_END,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_LPM3,
        .e_reset_core_mask  = RDR_AP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type        = CP_S_EXCEPTION,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP TLDSP",
        .e_desc             = "modem cp TLDSP reset",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_CP_CPHY_MOD_ID_START,
        .e_modid_end        = (unsigned int)RDR_MODEM_CP_CPHY_MOD_ID_END,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_LPM3,
        .e_reset_core_mask  = RDR_AP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type        = CP_S_EXCEPTION,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP CPHY",
        .e_desc             = "modem cp CPHY reset",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_CP_IMS_MOD_ID_START,
        .e_modid_end        = (unsigned int)RDR_MODEM_CP_IMS_MOD_ID_END,
        .e_process_priority = RDR_ERR,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP | RDR_LPM3,
        .e_reset_core_mask  = RDR_AP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type        = CP_S_EXCEPTION,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP IMS",
        .e_desc             = "modem cp IMS reset",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_CP_WDT_MOD_ID,
        .e_modid_end        = (unsigned int)RDR_MODEM_CP_WDT_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_CP | RDR_LPM3,
        .e_reset_core_mask  = RDR_CP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type        = CP_S_EXCEPTION,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP WDT",
        .e_desc             = "modem self-reset wdt",
    },
    {
        .e_modid            = (unsigned int)RDR_MODEM_CP_RESET_DLOCK_MOD_ID,
        .e_modid_end        = (unsigned int)RDR_MODEM_CP_RESET_DLOCK_MOD_ID,
        .e_process_priority = RDR_WARN,
        .e_reboot_priority  = RDR_REBOOT_WAIT,
        .e_notify_core_mask = RDR_AP |RDR_CP | RDR_LPM3,
        .e_reset_core_mask  = RDR_AP,
        .e_from_core        = RDR_CP,
        .e_reentrant        = (unsigned int)RDR_REENTRANT_ALLOW,
        .e_exce_type        = CP_S_EXCEPTION,
        .e_upload_flag      = (unsigned int)RDR_UPLOAD_YES,
        .e_from_module      = "CP DLOCK",
        .e_desc             = "modem reset by bus error",
    },
};
DUMP_MOD_ID g_dump_cp_mod_id[] ={
    {RDR_MODEM_CP_DRV_MOD_ID_START,RDR_MODEM_CP_DRV_MOD_ID_END,RDR_MODEM_CP_DRV_MOD_ID},
    {RDR_MODEM_CP_OSA_MOD_ID_START,RDR_MODEM_CP_OSA_MOD_ID_END,RDR_MODEM_CP_OSA_MOD_ID},
    {RDR_MODEM_CP_OAM_MOD_ID_START,RDR_MODEM_CP_OAM_MOD_ID_END,RDR_MODEM_CP_OAM_MOD_ID},
    {RDR_MODEM_CP_GUL2_MOD_ID_START,RDR_MODEM_CP_GUL2_MOD_ID_END,RDR_MODEM_CP_GUL2_MOD_ID},
    {RDR_MODEM_CP_CTTF_MOD_ID_START,RDR_MODEM_CP_CTTF_MOD_ID_END,RDR_MODEM_CP_CTTF_MOD_ID},
    {RDR_MODEM_CP_GUWAS_MOD_ID_START,RDR_MODEM_CP_GUWAS_MOD_ID_END,RDR_MODEM_CP_GUWAS_MOD_ID},
    {RDR_MODEM_CP_CAS_MOD_ID_START,RDR_MODEM_CP_CAS_MOD_ID_END,RDR_MODEM_CP_CAS_MOD_ID},
    {RDR_MODEM_CP_CPROC_MOD_ID_START,RDR_MODEM_CP_CPROC_MOD_ID_END,RDR_MODEM_CP_CPROC_MOD_ID},
    {RDR_MODEM_CP_GUGAS_MOD_ID_START,RDR_MODEM_CP_GUGAS_MOD_ID_END,RDR_MODEM_CP_GUGAS_MOD_ID},
    {RDR_MODEM_CP_GUCNAS_MOD_ID_START,RDR_MODEM_CP_GUCNAS_MOD_ID_END,RDR_MODEM_CP_GUCNAS_MOD_ID},
    {RDR_MODEM_CP_GUDSP_MOD_ID_START,RDR_MODEM_CP_GUDSP_MOD_ID_END,RDR_MODEM_CP_GUDSP_MOD_ID},
    {RDR_MODEM_CP_LPS_MOD_ID_START,RDR_MODEM_CP_LPS_MOD_ID_END,RDR_MODEM_CP_LPS_MOD_ID},
    {RDR_MODEM_CP_LMSP_MOD_ID_START,RDR_MODEM_CP_LMSP_MOD_ID_END,RDR_MODEM_CP_LMSP_MOD_ID},
    {RDR_MODEM_CP_TLDSP_MOD_ID_START,RDR_MODEM_CP_TLDSP_MOD_ID_END,RDR_MODEM_CP_TLDSP_MOD_ID},
    {RDR_MODEM_CP_CPHY_MOD_ID_START,RDR_MODEM_CP_CPHY_MOD_ID_END,RDR_MODEM_CP_CPHY_MOD_ID},
    {RDR_MODEM_CP_IMS_MOD_ID_START,RDR_MODEM_CP_IMS_MOD_ID_END,RDR_MODEM_CP_IMS_MOD_ID},
};

/*****************************************************************************
* �� �� ��  : dump_convert_id_mdmcp2rdr
* ��������  : ת��mdmcp��rdr֮��Ĵ�����
*
* �������  :
* �������  :

* �� �� ֵ  :

*
* �޸ļ�¼  : 2016��1��4��17:05:33   lixiaofan  creat
*
*****************************************************************************/

u32 dump_convert_id_mdmcp2rdr(u32 mdmcp_mod_id)
{
    u32 i = 0;
    u32 rdr_id = (u32)RDR_MODEM_CP_DRV_MOD_ID;
    for(i = 0; i < sizeof(g_dump_cp_mod_id)/sizeof(g_dump_cp_mod_id[0]);i++)
    {
        if(mdmcp_mod_id >= g_dump_cp_mod_id[i].mdm_id_start 
            && mdmcp_mod_id <= g_dump_cp_mod_id[i].mdm_id_end)
        {
            rdr_id =  g_dump_cp_mod_id[i].rdr_id;
        }
    }
    return rdr_id;
}

/*****************************************************************************
* �� �� ��  : dump_save_balong_rdr_info
* ��������  : ���ֻ�ƽ̨�ϸ���rdr��global ͷ
*
* �������  :
* �������  :

* �� �� ֵ  :

*
* �޸ļ�¼  : 2016��1��4��17:05:33   lixiaofan  creat
*
*****************************************************************************/
void dump_save_balong_rdr_info(u32 mod_id)
{
    struct timex txc = {0,};
    struct rtc_time tm = {0,};
    char temp[32] = {0};
    struct dump_global_base_info_s* global_base_info = NULL;
    struct rdr_exception_info_s* rdr_exc_info = NULL;
    u32 i = 0;
    dump_reboot_cpu_t core = DUMP_CPU_BUTTON;

    dump_get_reboot_contex((u32*)&core,NULL);

    do_gettimeofday(&(txc.time));
    rtc_time_to_tm((unsigned long)(txc.time.tv_sec), &tm);
    snprintf(temp, (sizeof(temp)-1), "%04d-%02d-%02d %02d:%02d:%02d", tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    global_base_info = (struct dump_global_base_info_s*)dump_get_global_baseinfo();
    if(global_base_info == NULL)
    {
        dump_fetal("get global_base_info error\n");
        return;
    }

    for(i = 0; i < (sizeof(g_phone_exc_info)/sizeof(g_phone_exc_info[0]));i++)
    {
       if(g_phone_exc_info[i].e_modid == mod_id)
       {
            rdr_exc_info = &g_phone_exc_info[i];
       }
    }
    if(rdr_exc_info == NULL)
    {
        dump_fetal("find rdr exc info error\n");
        return;
    }
    global_base_info->modid = rdr_exc_info->e_modid;
    global_base_info->arg1 = 0;
    global_base_info->arg2 = 0;
    /*coverity[secure_coding]*/
    memcpy(global_base_info->e_module, rdr_exc_info->e_from_module,(unsigned long)16);
    global_base_info->e_type = rdr_exc_info->e_exce_type;

    /*����Ϊ��hids������ʾ���������⴦�������rdr��ecore��ע���rdr�Ĳ�һ��*/
    if(core  == DUMP_CPU_COMM)
    {
        global_base_info->e_core = RDR_CP;
    }
    else if(core  == DUMP_CPU_APP)
    {
        global_base_info->e_core = RDR_MODEMAP;
    }
    global_base_info->start_flag = DUMP_START_EXCH;
    global_base_info->savefile_flag = DUMP_SAVE_FILE_END;

    /*coverity[secure_coding]*/
    memcpy((void*)(global_base_info->e_desc), rdr_exc_info->e_desc,(u32)(strlen((const char*)(rdr_exc_info->e_desc)) < 48 ? strlen((const char*)(rdr_exc_info->e_desc)): 48 ));
    /*coverity[secure_coding]*/
    memcpy(global_base_info->datetime,temp,(unsigned long)24);

    dump_fetal("update modem rdr global info finish\n");

}

/*****************************************************************************
* �� �� ��  : dump_save_modem_exc_info
* ��������  : ����rdr���ݵĲ���
*
* �������  :
* �������  :

* �� �� ֵ  :

*
* �޸ļ�¼  : 2016��1��4��17:05:33   lixiaofan  creat
*
*****************************************************************************/
void dump_save_rdr_exc_info(u32 modid, u32 etype, u64 coreid, char* logpath, pfn_cb_dump_done fndone)
{

    g_rdr_exc_info.modid  = modid;
    g_rdr_exc_info.coreid = coreid;
    g_rdr_exc_info.dump_done = fndone;

    if((strlen(logpath) + strlen(RDR_DUMP_FILE_CP_PATH)) >= RDR_DUMP_FILE_PATH_LEN - 1)
    {
        dump_fetal("log path is too long %s\n", logpath);
        return ;
    }
    /*coverity[secure_coding]*/
    memset(g_rdr_exc_info.log_path,'\0',(unsigned long)RDR_DUMP_FILE_PATH_LEN);
    /*coverity[secure_coding]*/
    memcpy(g_rdr_exc_info.log_path, logpath, strlen(logpath));
    /*coverity[secure_coding]*/
    memcpy(g_rdr_exc_info.log_path + strlen(logpath) , RDR_DUMP_FILE_CP_PATH, strlen(RDR_DUMP_FILE_CP_PATH));

    dump_fetal("log path is %s\n", g_rdr_exc_info.log_path);

}
/*****************************************************************************
* �� �� ��  : modem_error_proc
* ��������  : modem�쳣�����⴦����Ҫ���dmss��noc�쳣
*
* �������  :
* �������  :

* �� �� ֵ  :

*
* �޸ļ�¼  : 2016��1��4��17:05:33   lixiaofan  creat
*
*****************************************************************************/
void dump_callback_dmss_noc_proc(u32 modid)
{
    dump_reboot_reason_t reason ;

    if(modid == RDR_MODEM_NOC_MOD_ID)
    {
        dump_fetal("[0x%x] modem NOC process\n", bsp_get_slice_value());
    }
    else if(modid == RDR_MODEM_DMSS_MOD_ID)
    {
        dump_fetal("[0x%x] modem DMSS process\n", bsp_get_slice_value());
    }

    if (dump_get_init_phase() < DUMP_INIT_FLAG_CP_AGENT)
    {
        dump_fetal("modem dump not init direct reboot\n");
        return;
    }

    bsp_coresight_disable();

    dump_set_exc_flag(true);

    reason = ((modid == RDR_MODEM_NOC_MOD_ID) ?  DUMP_REASON_NOC : DUMP_REASON_DMSS);

    dump_set_reboot_contex(DUMP_CPU_APP, reason);

    dump_save_base_info(modid,0,0,0,0);

    if(DUMP_PHONE == dump_get_product_type())
    {
        dump_save_modem_sysctrl();
        dump_save_balong_rdr_info(modid);
    }

}

/*****************************************************************************
* �� �� ��  : dump_callback
* ��������  : modem�쳣�Ļص�������
*
* �������  :
* �������  :

* �� �� ֵ  :

*
* �޸ļ�¼  : 2016��1��4��17:05:33   lixiaofan  creat
*
*****************************************************************************/
u32 dump_callback(u32 modid, u32 etype, u64 coreid, char* logpath, pfn_cb_dump_done fndone)
{
    if(modid == RDR_MODEM_NOC_MOD_ID || modid == RDR_MODEM_DMSS_MOD_ID  )
    {
        dump_callback_dmss_noc_proc(modid);
    }
    else
    {
        dump_fetal("enter dump callback, mod id:0x%x\n", modid);
    }

    if(bsp_reset_ccore_is_reboot() == 0)
    {
        dump_notify_cp(modid);
    }
    else
    {
        dump_fetal("modem is reseting now,do not notify\n");
    }

    dump_save_rdr_exc_info(modid, etype, coreid, logpath, fndone);

    bsp_dump_save_self_addr();

    bsp_dump_hook_callback();

    dump_save_and_reboot();

    return BSP_OK;
}

/*****************************************************************************
* �� �� ��  : dump_reset_fail_proc
* ��������  : ������λʧ�ܵĴ���
*
* �������  :
* �������  :

* �� �� ֵ  :

*
* �޸ļ�¼  : 2016��1��4��17:05:33   lixiaofan  creat
*
*****************************************************************************/
void dump_reset_fail_proc(u32 rdr_modid)
{
    dump_reboot_reason_t reason = DUMP_REASON_RST_NOT_SUPPORT;
    u32 fail_id = RDR_MODEM_CP_RESET_REBOOT_REQ_MOD_ID;
    char* desc = "MDM_RST_OFF";

    if(rdr_modid == RDR_MODEM_CP_RESET_REBOOT_REQ_MOD_ID)
    {
        dump_fetal("bsp_cp_reset is stub,reset ap\n");
    }
    else if(rdr_modid == RDR_MODEM_CP_RESET_FAIL_MOD_ID)
    {
        dump_fetal("modem signal reset fail, notify rdr\n");
        reason = DUMP_REASON_RST_FAIL;
        desc = "MDM_RST_FAIL";
        fail_id =  modem_reset_fail_id_get();
    }
    else
    {
        dump_fetal("modem signal reset fail, notify rdr\n");
        reason = DUMP_REASON_RST_FREQ;
        desc = "MDM_RST_FREQ";
        fail_id =  RDR_MODEM_CP_RESET_FREQUENTLY_MOD_ID; 
    }

    dump_set_reboot_contex(DUMP_CPU_APP,reason);

    dump_save_momdem_reset_baseinfo(fail_id,desc);

    dump_save_balong_rdr_info(rdr_modid);

    rdr_system_error(rdr_modid, 0, 0);
}

/*****************************************************************************
* �� �� ��  : dump_reset_success_proc
* ��������  : ������λ�ɹ�����
*
* �������  :
* �������  :

* �� �� ֵ  :

*
* �޸ļ�¼  : 2016��1��4��17:05:33   lixiaofan  creat
*
*****************************************************************************/
void dump_reset_success_proc(void)
{
    u32 core = DUMP_CPU_BUTTON;

    dump_set_exc_flag(false);

    dump_get_reboot_contex(&core,NULL);

    dump_fetal("core = 0x%x\n",core);

    if(core == DUMP_CPU_COMM)
    {
        bsp_wdt_irq_enable(WDT_CCORE_ID);
        dump_fetal("modem reset success enable cp wdt\n");

    }

    dump_base_info_init();

    dump_set_reboot_contex(DUMP_CPU_BUTTON,DUMP_REASON_UNDEF);
}


/*****************************************************************************
* �� �� ��  : dump_check_single_set
* ��������  : modem Ƶ��������λ�����⴦��
*
* �������  :
* �������  :

* �� �� ֵ  :

*
* �޸ļ�¼  : 2016��1��4��17:05:33   lixiaofan  creat
*
*****************************************************************************/
s32 dump_check_single_reset_by_nv(void)
{

    NV_DUMP_STRU* cfg = NULL;
    cfg = dump_get_feature_cfg();
    if( cfg!= NULL && cfg->dump_cfg.Bits.sysErrReboot == 0)
    {
        dump_fetal("close modem sigle reset\n");
        return BSP_ERROR;
    }
    
    dump_fetal(" modem sigle reset open\n");
    return BSP_OK;
}
/*****************************************************************************
* �� �� ��  : dump_check_single_reset_by_modid
* ��������  : modem �жϴ������Ƿ���Ҫ������λ
*
* �������  :
* �������  :

* �� �� ֵ  :

*
* �޸ļ�¼  : 2016��1��4��17:05:33   lixiaofan  creat
*
*****************************************************************************/

s32 dump_check_single_reset_by_modid(u32 modid)
{
    
    if ((RDR_MODEM_AP_MOD_ID == modid)
        || (RDR_MODEM_CP_RESET_REBOOT_REQ_MOD_ID == modid))
    {
        
        dump_fetal("need reset system: 0x%x\n", modid);
        return BSP_ERROR;
    }
    else if((( modid >= RDR_MODEM_CP_DRV_MOD_ID) 
            &&( modid <= RDR_MODEM_CP_IMS_MOD_ID))    
            || (RDR_MODEM_CP_RESET_SIM_SWITCH_MOD_ID == modid)
            || (RDR_MODEM_CP_RESET_RILD_MOD_ID == modid)
            || (RDR_MODEM_CP_RESET_3RD_MOD_ID == modid)
            || (RDR_MODEM_CP_RESET_USER_RESET_MOD_ID == modid)
            || (RDR_MODEM_CP_RESET_DLOCK_MOD_ID == modid)
            || (RDR_MODEM_CP_WDT_MOD_ID == modid)
            || (RDR_MODEM_CP_NOC_MOD_ID == modid)
            || (RDR_MODEM_AP_DRV_MOD_ID == modid))  
    {
        dump_fetal("go to modem reset\n");
        return BSP_OK;
    }
    else
    { 
        dump_fetal("invalid mod id: 0x%x\n", modid);
        return BSP_ERROR;
    }

}
/*****************************************************************************
* �� �� ��  : dump_check_cp_reset
* ��������  : �ж��Ƿ�Ҫִ�е�����λ����
*
* �������  :
* �������  :

* �� �� ֵ  :

*
* �޸ļ�¼  : 2016��1��4��17:05:33   lixiaofan  creat
*
*****************************************************************************/

s32 dump_check_cp_reset(u32 modid)
{
    if(DUMP_PHONE != dump_get_product_type())
    {
        dump_fetal("mbb not support cp_reset\n");
        return BSP_ERROR;
    }
    if(BSP_ERROR == dump_check_single_reset_by_modid(modid))
    {   
        dump_fetal("modid not support cp_reset\n");
        return BSP_ERROR;
    }
    if(BSP_ERROR == dump_check_single_reset_by_nv())
    {
        dump_fetal("dump_check_single_reset_by_nv retun not support\n");                
        dump_set_exc_flag(false);
        return BSP_ERROR;
    }

    return BSP_OK;
}
/*****************************************************************************
* �� �� ��  : dump_reset
* ��������  : modem ��λ������
*
* �������  :
* �������  :

* �� �� ֵ  :

*
* �޸ļ�¼  : 2016��1��4��17:05:33   lixiaofan  creat
*
*****************************************************************************/
void dump_reset(u32 modid, u32 etype, u64 coreid)
{
    s32 ret;
    dump_fetal("enter dump reset, mod id:0x%x\n", modid);

    dump_clear_cpboot_area();
    if(BSP_OK == dump_check_cp_reset(modid))
    {
        ret = bsp_cp_reset();
        if(ret == -1)
        {
            dump_reset_fail_proc(RDR_MODEM_CP_RESET_REBOOT_REQ_MOD_ID);
            return;
        }
        

        if(!bsp_reset_is_successful(RDR_MODEM_CP_RESET_TIMEOUT))
        {
            dump_reset_fail_proc(RDR_MODEM_CP_RESET_FAIL_MOD_ID);
        }
        else
        {
            dump_reset_success_proc();            
        }
    }


}


/*****************************************************************************
* �� �� ��  : dump_register_rdr_exc
* ��������  : modem dump��ʼ����һ�׶�
*
* �������  :
* �������  :

* �� �� ֵ  :

*
* �޸ļ�¼  : 2016��1��4��17:05:33   lixiaofan  creat
*
*****************************************************************************/
s32 dump_register_rdr_exc(void)
{
    u32 i = 0;
    struct rdr_module_ops_pub   soc_ops = {NULL,NULL};

    if(DUMP_PHONE == dump_get_product_type())
    {
        for(i=0; i<sizeof(g_phone_exc_info)/sizeof(struct rdr_exception_info_s); i++)
        {

            if(rdr_register_exception(&g_phone_exc_info[i]) == 0)
            {
                dump_fetal("dump init: rdr_register_exception 0x%x fail\n", g_phone_exc_info[i].e_modid);
                return BSP_ERROR;
            }
        }
    }
    else
    {
        for(i=0; i<sizeof(g_mbb_exc_info)/sizeof(struct rdr_exception_info_s); i++)
        {
            if(rdr_register_exception(&g_mbb_exc_info[i]) == 0)
            {
                dump_fetal("dump init: rdr_register_exception 0x%x fail\n", g_mbb_exc_info[i].e_modid);
                return BSP_ERROR;
            }
        }
    }


    soc_ops.ops_dump  = (pfn_dump)dump_callback;
    soc_ops.ops_reset = (pfn_reset)dump_reset;

    if(rdr_register_module_ops(RDR_CP, &soc_ops, &(g_rdr_exc_info.soc_rst)) != BSP_OK)
    {
        dump_fetal("dump init: rdr_register_soc_ops fail\n");
        return BSP_ERROR;
    }

    g_rdr_exc_info.dump_done = NULL;

    return BSP_OK;

}
/*****************************************************************************
* �� �� ��  : dump_get_rdr_exc_info
* ��������  : ��ȡrdr���쳣������ַ
*
* �������  :
* �������  :

* �� �� ֵ  :

*
* �޸ļ�¼  : 2016��1��4��17:05:33   lixiaofan  creat
*
*****************************************************************************/

rdr_exc_info_s* dump_get_rdr_exc_info(void)
{
    return &g_rdr_exc_info;
}

/*****************************************************************************
* �� �� ��  : dump_match_rdr_mod_id
* ��������  : ��drv�Ĵ�����ת��Ϊrdr�Ĵ�����
*
* �������  :
* �������  :

* �� �� ֵ  :

*
* �޸ļ�¼  : 2016��1��4��17:05:33   lixiaofan  creat
*
*****************************************************************************/
u32 dump_match_rdr_mod_id(u32 drv_mod_id)
{
    u32 rdr_mod_id = RDR_MODEM_AP_MOD_ID;

    if(DUMP_PHONE == dump_get_product_type())
    {
        switch(drv_mod_id)
        {
        case DRV_ERRNO_RESET_SIM_SWITCH:
            rdr_mod_id = RDR_MODEM_CP_RESET_SIM_SWITCH_MOD_ID;
            return rdr_mod_id;
        case NAS_REBOOT_MOD_ID_RILD:
            rdr_mod_id = RDR_MODEM_CP_RESET_RILD_MOD_ID;
            return rdr_mod_id;
        case DRV_ERRNO_RESET_3RD_MODEM:
            rdr_mod_id = RDR_MODEM_CP_RESET_3RD_MOD_ID;
            return rdr_mod_id;
        case DRV_ERRNO_RESET_REBOOT_REQ:
            rdr_mod_id = RDR_MODEM_CP_RESET_REBOOT_REQ_MOD_ID;
            return rdr_mod_id;
        case DRV_ERROR_USER_RESET:
            rdr_mod_id = RDR_MODEM_CP_RESET_USER_RESET_MOD_ID;
            return rdr_mod_id;
        case DRV_ERRNO_DLOCK:
            rdr_mod_id = RDR_MODEM_CP_RESET_DLOCK_MOD_ID;
            return rdr_mod_id;
        default:
            break;
        }
    }

    if((DRV_ERRNO_DUMP_CP_WDT == drv_mod_id))
    {
        rdr_mod_id = RDR_MODEM_CP_WDT_MOD_ID;
    }
    else if((DRV_ERRNO_DLOCK == drv_mod_id))
    {
        rdr_mod_id = RDR_MODEM_CP_RESET_DLOCK_MOD_ID;
    }
    else if((drv_mod_id <= (u32)RDR_MODEM_CP_DRV_MOD_ID_END) 
        && EDITION_INTERNAL_BETA !=dump_get_edition_type())
    {
        rdr_mod_id = RDR_MODEM_AP_DRV_MOD_ID;
    }
    else
    {
        rdr_mod_id = RDR_MODEM_AP_MOD_ID;
    }

    return rdr_mod_id;

}

