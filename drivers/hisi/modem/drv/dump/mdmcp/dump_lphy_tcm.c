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

#include <linux/timer.h>
#include <linux/thread_info.h>
#include <linux/syslog.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <asm/string.h>
#include "osl_types.h"
#include "osl_bio.h"
#include "osl_io.h"
#include "bsp_rfile.h"
#include "bsp_dspload.h"
#include "bsp_dsp.h"
#include "bsp_dump.h"
#include "bsp_ddr.h"
#include "dump_file.h"
#include "dump_print.h"
#include "dump_config.h"


char * lphy_image_ddr_addr = NULL;
/*****************************************************************************
* �� �� ��  : dump_save_all_tcm
* ��������  : ����ȫ����tcm�ļ�
*
* �������  :
* �������  :

* �� �� ֵ  :

*
* �޸ļ�¼  : 2016��1��4��17:05:33   lixiaofan  creat
*
*****************************************************************************/
void dump_save_all_tcm(char* data,char* dst_path)
{
    char file_name[128] = {0}; 
    struct dsp_bin_header_stru *pheader;
    pheader = (struct dsp_bin_header_stru *)data;
    /* coverity[HUAWEI DEFECT] */
    memset(file_name, 0, sizeof(file_name));
    /* coverity[HUAWEI DEFECT] */
    snprintf(file_name, sizeof(file_name), "%slphy_dump.bin", dst_path);
    file_name[127]='\0';
    dump_save_file(file_name, data, pheader->ulFileSize);
    dump_fetal("[dump]: save %s finished\n", file_name);

}

/*****************************************************************************
* �� �� ��  : dump_save_some_tcm
* ��������  : ����ȫ����dtcm��itcm�ļ�
*
* �������  :
* �������  :

* �� �� ֵ  :

*
* �޸ļ�¼  : 2016��1��4��17:05:33   lixiaofan  creat
*
*****************************************************************************/
void dump_save_some_tcm(char* data,char* dst_path)
{
    char file_name[128] = {0};
    struct dsp_bin_header_stru *pheader;
    pheader = (struct dsp_bin_header_stru *)data;
    /* coverity[HUAWEI DEFECT] */
    memset(file_name, 0, sizeof(file_name));
    /* coverity[HUAWEI DEFECT] */
    snprintf(file_name, sizeof(file_name), "%slphy_dump.bin", dst_path);
    file_name[127]='\0';
    dump_save_file(file_name, data, pheader->ulFileSize);
    dump_fetal("[dump]: save %s finished\n", file_name);
    

}
/*****************************************************************************
* �� �� ��  : om_save_lphy_tcm
* ��������  : ����tldsp�ľ���
*
* �������  :
* �������  :

* �� �� ֵ  :

*
* �޸ļ�¼  : 2016��1��4��17:05:33   lixiaofan  creat
*
*****************************************************************************/

void dump_save_lphy_tcm(char * dst_path)
{
    bool flag = false;
    struct dsp_dump_proc_flag *tl_flag = NULL;
    DUMP_FILE_CFG_STRU* cfg = dump_get_file_cfg();

    if(cfg->file_list.file_bits.lphy_tcm == 0)
    {
        return;
    }

    lphy_image_ddr_addr = (char *)ioremap_wc(NXDSP_MDDR_FAMA(DDR_TLPHY_IMAGE_ADDR), DDR_TLPHY_IMAGE_SIZE);
    if(NULL == lphy_image_ddr_addr)
    {
        dump_fetal("ioremap DDR_TLPHY_IMAGE_ADDR fail\n");
        return;
    }

    tl_flag = (struct dsp_dump_proc_flag *)((unsigned long)SHM_BASE_ADDR + SHM_OFFSET_DSP_FLAG);
    if(SAVE_TCM_BEGIN == tl_flag->dsp_dump_flag)
    {
        dump_fetal("carry tldsp tcm to ddr not finished!\n");
        flag = true;
    }
    else if(SAVE_TCM_END == tl_flag->dsp_dump_flag)
    {
        flag = true;
    }
    else
    {
        flag = false;
    }

    /*DSP DDR�ڴ�ֲ��ο�hi_dsp.h*/
    if(flag == true)
    {
        dump_save_all_tcm(lphy_image_ddr_addr,dst_path);
    }
    else if(dump_get_edition_type() == EDITION_INTERNAL_BETA)
    {
        dump_save_some_tcm(lphy_image_ddr_addr,dst_path);
    }
    tl_flag->dsp_dump_flag = 0;

    iounmap(lphy_image_ddr_addr);
    return;
}

