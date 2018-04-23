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
#include "bsp_dump_mem.h"
#include "dump_modem_field.h"
#include "dump_modem_area.h"
#include "dump_print.h"

struct dump_field_ctrl_info_s   g_st_field_ctrl;
u32    g_dump_mem_init = 0;

/*****************************************************************************
* �� �� ��  : dump_get_area_info
* ��������  : ��֤field_id�Ƿ�Ϸ����쳣id����BSP_ERROR������id����area id
*
* �������  :
* �������  :

* �� �� ֵ  :

*
* �޸ļ�¼  : 2016��1��4��17:05:33   lixiaofan  creat
*
*****************************************************************************/
static u32 dump_get_areaid_by_fieldid(u32 field_id)
{
    u32 ret;

    if((field_id>=DUMP_MODEMAP_FIELD_BEGIN) && (field_id<DUMP_MODEMAP_FIELD_END))
    {
        ret = DUMP_AREA_MDMAP;
    }
    else if((field_id>=DUMP_CP_FIELD_BEGIN) && (field_id<DUMP_CP_FIELD_END))
    {
        ret = DUMP_AREA_CP;
    }
    else
    {
        (void)printk("%s:invalid field id 0x%x\n", __FUNCTION__, (u32)field_id);
        ret = (u32)(-1);
    }

    return ret;
}
/*****************************************************************************
* �� �� ��  : dump_get_cp_save_done
* ��������  : �ж�c��log�Ƿ񱣴����
*
* �������  :
* �������  :

* �� �� ֵ  :

*
* �޸ļ�¼  : 2016��1��4��17:05:33   lixiaofan  creat
*
*****************************************************************************/
s32 dump_get_cp_save_done(void)
{
    s32  flag = 0;
    struct dump_area_mntn_addr_info_s area_info= {NULL,};

    flag = dump_get_area_info(DUMP_AREA_CP,&area_info);
    if(flag == BSP_ERROR)
    {
        dump_fetal("find cp area info fail\n");
        return BSP_ERROR;
    }

    if(area_info.vaddr == NULL)
    {
        return BSP_ERROR;
    }
    /*coverity[secure_coding]*/
    memcpy(&flag,(u32*)(((dump_area_head_t*)(area_info.vaddr))->version),sizeof(u32));

    /*lint -e650 -esym(650,*)*/
    if((flag == DUMP_SAVE_SUCCESS ))
    {
        return BSP_OK;
    }
    /*lint -e650 +esym(650,*)*/    

    return BSP_ERROR;
}

/*****************************************************************************
* �� �� ��  : bsp_dump_get_field_addr
* ��������  : ��ȡfield�ĵ�ַ
*
* �������  :field_id
* �������  :

* �� �� ֵ  :

*
* �޸ļ�¼  : 2016��1��4��17:05:33   lixiaofan  creat
*
*****************************************************************************/
u8 * bsp_dump_get_field_addr(u32 field_id)
{
    u32 i = 0;
    u8 * addr = 0;
    dump_area_t * parea = NULL;
    s32    area_id = DUMP_AREA_BUTT;
    struct dump_area_mntn_addr_info_s area_info;

    /*����field���Ҷ�Ӧ��area id*/
    area_id = dump_get_areaid_by_fieldid(field_id);
    if(area_id >= DUMP_AREA_BUTT)
    {
        dump_fetal("not find area\n");
        return NULL;
    }
    /*����area id���Ҷ�Ӧarea ����ַ*/
    /*coverity[secure_coding]*/
    memset(&area_info,0,sizeof(area_info));
    if(dump_get_area_info((DUMP_AREA_ID)area_id,&area_info))
    {
        dump_fetal("get area info error\n");

        return NULL;
    }

    if((!area_info.vaddr) || (!area_info.len) ||(!area_info.paddr))
    {
        dump_fetal("get area info error\n");

        return NULL;
    }
    parea = (dump_area_t *)area_info.vaddr;
    /* search field addr by field id */
    for(i=0; i<parea->area_head.field_num; i++)
    {
        if(field_id == parea->fields[i].field_id)
        {
            addr = (u8 *)parea + parea->fields[i].offset_addr;
            return addr;
        }
    }

    return NULL;
}
/*****************************************************************************
* �� �� ��  : bsp_dump_get_field_map
* ��������  : ��ȡfield��field��Ϣ
*
* �������  :field_id
* �������  :

* �� �� ֵ  :

*
* �޸ļ�¼  : 2016��1��4��17:05:33   lixiaofan  creat
*
*****************************************************************************/
u8 * bsp_dump_get_field_map(u32 field_id)
{
    u32 i = 0 ;
    u32 areaid = DUMP_AREA_BUTT;
    dump_area_t * parea = NULL;
    struct dump_area_mntn_addr_info_s area_info;


    areaid = dump_get_areaid_by_fieldid(field_id);
    if(areaid >= DUMP_AREA_BUTT)
    {
        dump_fetal("not find area\n");

        return NULL;
    }

    if(dump_get_area_info((DUMP_AREA_ID)areaid,&area_info))
    {
        dump_fetal("get area info error\n");

        return NULL;
    }
    parea = (dump_area_t*)area_info.vaddr;

    /* search field map by field id */
    for(i=0; i<parea->area_head.field_num; i++)
    {
        if(field_id == parea->fields[i].field_id)
        {
            return (u8 * )&parea->fields[i];
        }
    }

    return NULL;
}

/*****************************************************************************
* �� �� ��  : bsp_dump_register_field
* ��������  : ����field��Ϣ
            1. ������ַע�ᣬ�������ʱvirt_addr,phy_addr���봫0���ɹ�����dumpע���ַ
            2. �Դ���ַע�ᣬ�������ʱphy_addrΪ�Դ������ַ��virt_addrΪ�����ַ��ͬʱ��dump�ڴ��з�����ͬ��С�ڴ棬�ɹ�������virt_addr

            3. ����ע�᷽ʽ��������dump�����ڴ棬�����Դ���ַ��ע�᷽ʽ����ϵͳ�쳣ʱ����dumpģ�������ݿ���
            4. ÿ��ע��������Ҫ��ʹ���ߴ����Ӧ�İ汾�ţ���8λΪ���汾�ţ���8λΪ�ΰ汾��
* �������  :field_id
* �������  :

* �� �� ֵ  :

*
* �޸ļ�¼  : 2016��1��4��17:05:33   lixiaofan  creat
*
*****************************************************************************/
u8 * bsp_dump_register_field(u32 field_id, char * name, void * virt_addr, void * phy_addr, u32 length, u16 version)
{
    dump_area_t* area_info = NULL;
    u8*         ret = NULL;
    u32         index = 0;
    u32         i = 0;
    unsigned long flags = 0;
    struct dump_field_self_info_s*  self = NULL;
    dump_field_map_t* pfiled = NULL;

    if(!g_st_field_ctrl.ulInitflag)
    {
        bsp_dump_mem_init();
    }

    /*ע��Ĳ��ǵ�ǰ��ϵͳ��Χ��field*/
    if((field_id < CURRENT_FIELD_ID_START)||(field_id > CURRENT_FIELD_ID_END))
    {
        dump_fetal("field error field = %d\n",field_id);
        return NULL;
    }

    if(!name)
    {
        dump_fetal("name is null\n");

        return NULL;
    }

    spin_lock_irqsave(&g_st_field_ctrl.lock, flags);

    area_info = (dump_area_t*)g_st_field_ctrl.virt_area_addr;
    /*ע��field�����������Χ*/
    index = area_info->area_head.field_num;
    /*����Ƿ����ظ�ע��*/
    for(i=0;i<index;i++)
    {
        if(area_info->fields[i].field_id == field_id)
        {
            spin_unlock_irqrestore(&g_st_field_ctrl.lock, flags);
            return (u8*)area_info + area_info->fields[i].offset_addr;
        }
    }

    if(index >= DUMP_FIELD_MAX_NUM)
    {
        spin_unlock_irqrestore(&g_st_field_ctrl.lock, flags);

        dump_fetal("not enough id\n");

        return NULL;
    }

    /*ʣ��ռ䲻��*/
    if(g_st_field_ctrl.free_length < length)
    {
        spin_unlock_irqrestore(&g_st_field_ctrl.lock, flags);

        dump_fetal("not enough space\n");

        return NULL;
    }

    ret = (u8*)area_info + g_st_field_ctrl.free_offset;
    DUMP_FIXED_FIELD(((void*)&(area_info->fields[index])),(field_id),(name),(g_st_field_ctrl.free_offset),(length));
    area_info->area_head.field_num ++;
    pfiled = &(area_info->fields[index]);

    g_st_field_ctrl.free_length -= length;
    g_st_field_ctrl.free_offset += length;
    g_st_field_ctrl.field_num ++;

    area_info->area_head.field_num = g_st_field_ctrl.field_num;

    /*�Դ���ַע��*/
    if( virt_addr || phy_addr )
    {
        self = (struct dump_field_self_info_s*)ret;
        self->magic_num = DUMP_FIELD_SELF_MAGICNUM;
        self->phy_addr  = (uintptr_t)phy_addr;
        self->virt_addr  = virt_addr;
        self->reserved  = 0;
    }
    spin_unlock_irqrestore(&g_st_field_ctrl.lock, flags);

    return ret;
}

/*****************************************************************************
* �� �� ��  : bsp_dump_save_self_addr
* ��������  : ������ע��Ŀռ�
*
* �������  :field_id
* �������  :

* �� �� ֵ  :

*
* �޸ļ�¼  : 2016��1��4��17:05:33   lixiaofan  creat
*
*****************************************************************************/
void bsp_dump_save_self_addr(void)
{
    dump_area_t* area_info;
    u32         index;
    u32         i;
    unsigned long flags;
    struct dump_field_self_info_s*  self;
    void* self_virt;


    spin_lock_irqsave(&g_st_field_ctrl.lock, flags);

    area_info = (dump_area_t*)g_st_field_ctrl.virt_area_addr;
    index     = g_st_field_ctrl.field_num;

    for(i=0;i<index;i++)
    {
        self = (struct dump_field_self_info_s*)((u8*)area_info+area_info->fields[i].offset_addr);

        if((DUMP_FIELD_SELF_MAGICNUM == self->magic_num)&&(self->virt_addr))
        {
            self_virt = self->virt_addr;
            /*coverity[secure_coding]*/
            memcpy(self,self_virt,area_info->fields[i].length);
        }
    }
    spin_unlock_irqrestore(&g_st_field_ctrl.lock, flags);

    dump_fetal("bsp_dump_save_self_addr finish\n");

    return ;
}

/*****************************************************************************
* �� �� ��  : bsp_dump_field_init
* ��������  : cp dump fileld��ʼ��
*
* �������  :field_id
* �������  :

* �� �� ֵ  :

*
* �޸ļ�¼  : 2016��1��4��17:05:33   lixiaofan  creat
*
*****************************************************************************/
s32 bsp_dump_field_init(void)
{
    struct dump_area_mntn_addr_info_s area_info;

    if(true == g_st_field_ctrl.ulInitflag)
    {
        return BSP_OK;
    }

    spin_lock_init(&g_st_field_ctrl.lock);

    memset(&area_info,0,sizeof(struct dump_area_mntn_addr_info_s));

    if(dump_get_area_info(CURRENT_AREA,&area_info))
    {
        dump_fetal("get area error area_id = %d\n",CURRENT_AREA);

        return BSP_ERROR;
    }

    if((!area_info.vaddr) || (!area_info.len) ||(!area_info.paddr))
    {
        dump_fetal("area_info error\n");

        return BSP_ERROR;
    }

    g_st_field_ctrl.virt_area_addr = area_info.vaddr;
    g_st_field_ctrl.phy_area_addr  = area_info.paddr;
    g_st_field_ctrl.total_length   = area_info.len;
    g_st_field_ctrl.free_offset    = sizeof(dump_area_t);
    g_st_field_ctrl.free_length    = g_st_field_ctrl.total_length - g_st_field_ctrl.free_offset;
    /*coverity[secure_coding]*/
    memset(g_st_field_ctrl.virt_area_addr,0,g_st_field_ctrl.total_length);
    g_st_field_ctrl.virt_area_addr->area_head.magic_num = DUMP_AREA_MAGICNUM;
    /*coverity[secure_coding]*/
    memcpy(g_st_field_ctrl.virt_area_addr->area_head.name,CURRENT_AREA_NAME,strlen(CURRENT_AREA_NAME));
    g_st_field_ctrl.virt_area_addr->area_head.field_num = 0;

    dump_fetal("virt_area_addr = 0x%p phy_area_addr = 0x%p total_length = 0x%x free_offset = 0x%x free_length=0x%x\n", \
        g_st_field_ctrl.virt_area_addr,g_st_field_ctrl.phy_area_addr,g_st_field_ctrl.total_length,g_st_field_ctrl.free_offset,g_st_field_ctrl.free_length);

    g_st_field_ctrl.ulInitflag = true;

    dump_fetal("bsp_dump_field_init finish\n");
    return BSP_OK;
}

/*****************************************************************************
* �� �� ��  : bsp_dump_get_field_phy_addr
* ��������  : ��ȡfield�������ַ
*
* �������  :field_id
* �������  :

* �� �� ֵ  :

*
* �޸ļ�¼  : 2016��1��4��17:05:33   lixiaofan  creat
*
*****************************************************************************/
u8 * bsp_dump_get_field_phy_addr(u32 field_id)
{
    u8 * addr = 0;

    addr = bsp_dump_get_field_addr(field_id);
    if(NULL == addr)
    {
        return NULL;
    }

    /*coverity[overflow] */
    return (u8 *)((unsigned long)addr - (unsigned long)g_st_field_ctrl.virt_area_addr+ (unsigned long)g_st_field_ctrl.phy_area_addr);
}



/*****************************************************************************
* �� �� ��  : modem_dump_field_init
* ��������  : ��ʼ��modem ap��Ҫʹ��filed �ռ�
*
* �������  :
* �������  :

* �� �� ֵ  :

*
* �޸ļ�¼  : 2016��1��4��17:05:33   lixiaofan  creat
*
*****************************************************************************/

void dump_mdmap_field_init(void)
{
    void* addr = NULL;
    addr = bsp_dump_register_field(DUMP_MODEMAP_BASE_INFO_SMP,"MDMAP_BASE_INFO",NULL,NULL,DUMP_MODEMAP_BASE_INFO_SIZE,0);
    if(addr == NULL)
    {
        dump_fetal("DUMP_MODEMAP_BASE_INFO get addr fail\n");
    }

    addr = bsp_dump_register_field(DUMP_MODEMAP_USER_DATA,  "USER_DATA",  NULL,NULL,DUMP_MODEMAP_USER_DATA_SIZE,0);
    if(addr == NULL)
    {
        dump_fetal("DUMP_MODEMAP_USER_DATA get addr fail\n");
    }

    dump_fetal("dump_mdmap_field_init finish\n");

}

/*****************************************************************************
* �� �� ��  : bsp_dump_mem_init
* ��������  : modem ap ��ά�ɲ�ռ��ʼ��
*
* �������  :
* �������  :

* �� �� ֵ  :

*
* �޸ļ�¼  : 2016��1��4��17:05:33   lixiaofan  creat
*
*****************************************************************************/
s32 bsp_dump_mem_init(void)
{
    if(g_dump_mem_init == 1)
    {
        return BSP_OK;
    }
    if(dump_area_init())
    {
        return BSP_ERROR;
    }

    if(bsp_dump_field_init())
    {
        return BSP_ERROR;
    }

    dump_fetal("bsp_dump_mem_init finish\n");
    g_dump_mem_init = 1;

    return BSP_OK;
}

/*****************************************************************************
* �� �� ��  : dump_show_field
* ��������  : ���Խӿڣ���ʾ��ǰ�Ѿ�ע��fieldʹ�����
*
* �������  :
* �������  :

* �� �� ֵ  :

*
* �޸ļ�¼  : 2016��1��4��17:05:33   lixiaofan  creat
*
*****************************************************************************/
void dump_show_field(void)
{
    u32 i = 0,j = 0;
    dump_area_t* parea = NULL;
    struct dump_area_mntn_addr_info_s area_info = {NULL,};
    u32 used_size = 0;

    for(i=0;i<DUMP_AREA_BUTT;i++)
    {
        used_size = 0;
        /*coverity[secure_coding]*/
        memset(&area_info,0,sizeof(struct dump_area_mntn_addr_info_s));
        if(dump_get_area_info((DUMP_AREA_ID)i,&area_info))
        {
            continue;
        }
        if((!area_info.vaddr) || (!area_info.len) ||(!area_info.paddr))
        {
            continue;
        }
        /*coverity[tainted_data_downcast] */
        parea = (dump_area_t*)area_info.vaddr;
        if(parea->area_head.magic_num != DUMP_AREA_MAGICNUM)
        {
            continue;
        }
        printk("-----------------------------------------\n");
        printk("[%s] AREA INFO(VIRT ADDR %pK)(PHY ADDR %pK)\n", parea->area_head.name,area_info.vaddr,area_info.paddr);
        for(j=0;j<parea->area_head.field_num;j++)
        {
            used_size += parea->fields[j].length;
            dump_fetal("FILED %-16s: id=0x%-8x offset=0x%-8x size=0x%x\n", \
                parea->fields[j].field_name, parea->fields[j].field_id, parea->fields[j].offset_addr, parea->fields[j].length);
        }
        dump_fetal("[%s] total len 0x%x,used len = 0x%x,free len = 0x%x\n",parea->area_head.name,area_info.len,used_size,area_info.len-used_size);

    }
    return;
}

arch_initcall(bsp_dump_mem_init);

