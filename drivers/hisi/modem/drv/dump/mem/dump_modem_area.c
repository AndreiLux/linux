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


#include <product_config.h>
#include <bsp_ddr.h>
#include "bsp_dump_mem.h"
#include "dump_modem_area.h"
#include "dump_print.h"

struct dump_global_area_ctrl_s g_st_dump_area_ctrl;

/*****************************************************************************
* �� �� ��  : dump_get_load_info
* ��������  : ��ȡ��ǰload����Ϣ
*
* �������  :
* �������  :

* �� �� ֵ  :

*
* �޸ļ�¼  : 2016��1��4��17:05:33   lixiaofan  creat
*
*****************************************************************************/
s32 dump_get_load_info(dump_load_info_t* load_info)
{
    dump_load_info_t * dump_load = NULL;
    if(!g_st_dump_area_ctrl.ulInitFlag)
        return BSP_ERROR;
    if(!load_info)
        return BSP_ERROR;
    dump_load = (dump_load_info_t *)((u8*)g_st_dump_area_ctrl.virt_addr+(MNTN_AREA_RESERVE_ADDR-MNTN_BASE_ADDR));

    /* coverity[secure_coding] */
    (void)memcpy(load_info,dump_load,sizeof(dump_load_info_t));
    
    return BSP_OK;
}
/*****************************************************************************
* �� �� ��  : dump_get_area_info
* ��������  : ��ȡ��ǰarea����Ϣ
*
* �������  :
* �������  :

* �� �� ֵ  :

*
* �޸ļ�¼  : 2016��1��4��17:05:33   lixiaofan  creat
*
*****************************************************************************/
s32 dump_get_area_info(DUMP_AREA_ID areaid,struct dump_area_mntn_addr_info_s* area_info)
{
    if(!g_st_dump_area_ctrl.ulInitFlag)
    {
        dump_fetal("dump area has not init\n");
        return BSP_ERROR;
    }

    if((areaid >= DUMP_AREA_BUTT)||(NULL == area_info))
    {
        dump_fetal("dump area erro para areaid = %d\n",areaid);

        return BSP_ERROR;
    }

    area_info->vaddr    = (void*)((unsigned long)g_st_dump_area_ctrl.virt_addr+g_st_dump_area_ctrl.virt_addr->area_info[areaid].offset);
    area_info->paddr    = (void*)(g_st_dump_area_ctrl.phy_addr+g_st_dump_area_ctrl.virt_addr->area_info[areaid].offset);
    area_info->len      = g_st_dump_area_ctrl.virt_addr->area_info[areaid].length;

    return BSP_OK;
}

/*****************************************************************************
* �� �� ��  : dump_get_global_info
* ��������  : ��ȡrdr��globalͷ��Ϣ
*
* �������  :
* �������  :

* �� �� ֵ  :

*
* �޸ļ�¼  : 2016��1��4��17:05:33   lixiaofan  creat
*
*****************************************************************************/
s32 dump_get_global_info(struct dump_global_area_ctrl_s * global_area)
{
    if(0 == g_st_dump_area_ctrl.ulInitFlag || global_area == NULL)
    {
        return BSP_ERROR;
    }
    global_area->virt_addr = g_st_dump_area_ctrl.virt_addr;
    global_area->phy_addr = g_st_dump_area_ctrl.phy_addr;
    global_area->length = g_st_dump_area_ctrl.length;
    return BSP_OK;

}

/*****************************************************************************
* �� �� ��  : dump_get_global_baseinfo
* ��������  : ��ȡrdr��globalͷ��Ϣ�Ļ�����Ϣ
*
* �������  :
* �������  :

* �� �� ֵ  :

*
* �޸ļ�¼  : 2016��1��4��17:05:33   lixiaofan  creat
*
*****************************************************************************/
void* dump_get_global_baseinfo(void)
{
    if(!g_st_dump_area_ctrl.ulInitFlag)
    {
        dump_fetal("dump area has not init\n");

        return NULL;
    }

    return &(g_st_dump_area_ctrl.virt_addr->base_info);
}
/*****************************************************************************
* �� �� ��  : dump_clear_cpboot_area
* ��������  : ���c�������׶ε�����
*
* �������  :
* �������  :

* �� �� ֵ  :

*
* �޸ļ�¼  : 2016��1��4��17:05:33   lixiaofan  creat
*
*****************************************************************************/

void dump_clear_cpboot_area(void)
{
    unsigned long offset,base;
    offset = MNTN_AREA_CBOOT_ADDR - MNTN_BASE_ADDR;
    base = (unsigned long)g_st_dump_area_ctrl.virt_addr;
    
    /* coverity[secure_coding] */
    (void)memset( (void*)(base+offset), 0, MNTN_AREA_CBOOT_SIZE);
}
/*****************************************************************************
* �� �� ��  : dump_area_init
* ��������  : ��ʼ����ǰarea��Ϣ
*
* �������  :
* �������  :

* �� �� ֵ  :

*
* �޸ļ�¼  : 2016��1��4��17:05:33   lixiaofan  creat
*
*****************************************************************************/
s32 dump_area_init(void)
{
    dump_load_info_t * dump_load = NULL;
    s32 len ;

    if(g_st_dump_area_ctrl.ulInitFlag)
    {
        dump_fetal("dump area has init\n");
        return BSP_OK;
    }
    g_st_dump_area_ctrl.phy_addr = MDDR_FAMA(MNTN_BASE_ADDR);
    g_st_dump_area_ctrl.length   = MNTN_BASE_SIZE;
    g_st_dump_area_ctrl.virt_addr= (struct dump_global_struct_s*)ioremap_wc(g_st_dump_area_ctrl.phy_addr,g_st_dump_area_ctrl.length);

    if(NULL == g_st_dump_area_ctrl.virt_addr)
    {
        dump_error(" ioremap fail !\n");
        return BSP_ERROR;
    }

    memset(g_st_dump_area_ctrl.virt_addr,0,g_st_dump_area_ctrl.length);
    g_st_dump_area_ctrl.virt_addr->top_head.area_number     = DUMP_AREA_BUTT;
    g_st_dump_area_ctrl.virt_addr->top_head.magic           = DUMP_GLOBALE_TOP_HEAD_MAGIC;
    /*coverity[secure_coding]*/
    len = snprintf((char*)g_st_dump_area_ctrl.virt_addr->top_head.build_time,sizeof(g_st_dump_area_ctrl.virt_addr->top_head.build_time),"%s,%s", __DATE__,__TIME__);
    if(len > 32 || len < 0)
    {
        return BSP_ERROR;
    }
    /*coverity[secure_coding]*/
    len = snprintf((char*)g_st_dump_area_ctrl.virt_addr->top_head.product_name, sizeof(g_st_dump_area_ctrl.virt_addr->top_head.product_name),"%s",PRODUCT_NAME);
    if(len > 32 || len < 0)
    {
        return BSP_ERROR;
    }
    /*coverity[secure_coding]*/
    len = snprintf((char*)g_st_dump_area_ctrl.virt_addr->top_head.product_version,sizeof(g_st_dump_area_ctrl.virt_addr->top_head.product_version),"%s",PRODUCT_FULL_VERSION_STR);
    if(len > 32 || len < 0)
    {
        return BSP_ERROR;
    }

    g_st_dump_area_ctrl.virt_addr->area_info[DUMP_AREA_MDMAP].offset     = MNTN_AREA_MDMAP_ADDR- MNTN_BASE_ADDR;
    g_st_dump_area_ctrl.virt_addr->area_info[DUMP_AREA_MDMAP].length     = MNTN_AREA_MDMAP_SIZE;
    g_st_dump_area_ctrl.virt_addr->area_info[DUMP_AREA_CP].offset        = MNTN_AREA_CP_ADDR - MNTN_BASE_ADDR;
    g_st_dump_area_ctrl.virt_addr->area_info[DUMP_AREA_CP].length        = MNTN_AREA_CP_SIZE;

    dump_load = (dump_load_info_t *)((u8*)g_st_dump_area_ctrl.virt_addr+(MNTN_AREA_RESERVE_ADDR-MNTN_BASE_ADDR));
    dump_load->mdm_ddr  = DDR_MCORE_ADDR;
    dump_load->mdm_share= DDR_SHARED_MEM_ADDR;
    dump_load->mdm_dump = DDR_MNTN_ADDR;
    dump_load->mdm_sram = HI_SRAM_MEM_ADDR;
    dump_load->mdm_dts  = DDR_MCORE_DTS_ADDR;
    dump_load->mdm_ddr_saveoff = 0;
    dump_load->mdm_share_ddr_size = HI_SHARED_DDR_SIZE;
    dump_load->magic_num= DUMP_LOAD_MAGIC;
    g_st_dump_area_ctrl.ulInitFlag = true;

    dump_fetal("dump_area_init finish\n");

    return BSP_OK;
}




