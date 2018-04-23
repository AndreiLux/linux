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



/*****************************************************************************
  1 ͷ�ļ�����
**************************************************************************** */
#include <osl_sem.h>
#include "diag_port_manager.h"
#include "OmCommonPpm.h"
#include "OmHsicPpm.h"


/* ****************************************************************************
  2 ȫ�ֱ�������
**************************************************************************** */
/* HSIC��OM����״̬��Ĭ��Ϊ������ */
OM_HSIC_PORT_STATUS_ENUM_UINT32         g_ulOmHsicConnectStatus = OM_HSIC_PORT_STATUS_OFF;

/*�����ź�������������OM HSIC���� */
osl_sem_id                                 g_ulOmAcpuHsicSem;

/*****************************************************************************
  3 �ⲿ��������
*****************************************************************************/


/*****************************************************************************
  4 ����ʵ��
*****************************************************************************/

/*****************************************************************************
 �� �� ��  :
 ��������  :
 �������  :
 �������  :
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��5��31��
    ��    ��   : XXXXXXXX
    �޸�����   : V8R1 OM_Optimize��Ŀ����

*****************************************************************************/


/*****************************************************************************
 �� �� ��  : PPM_GetHsicPortStatus
 ��������  : OM�ں�HSIC����״̬
 �������  : ��
 �������  : ��
 �� �� ֵ  : ״̬״̬
 �޸���ʷ      :
  1.��    ��   : 2014��5��31��
    ��    ��   : h59254
    �޸�����   : V8R1 OM_Optimize��Ŀ����
*****************************************************************************/
OM_HSIC_PORT_STATUS_ENUM_UINT32 PPM_GetHsicPortStatus(void)
{
    /* ����OM�ں�HSIC����״̬ */
    return g_ulOmHsicConnectStatus;
}

/*****************************************************************************
 �� �� ��  : PPM_HsicIndWriteDataCB
 ��������  : ���ڴ���Hsic �ڵ��첽�������ݵĻص�
 �������  : pucData:   ��Ҫ���͵���������
             ulDataLen: ���ݳ���
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :
 �޸���ʷ  :
   1.��    ��  : 2012��2��2��
     ��    ��  : zhuli
     �޸�����  : Creat Function
*****************************************************************************/
void PPM_HsicIndWriteDataCB(u8* pucVirData, u8* pucPhyData, s32 lLen)
{
    /*��ǰֻ����OM����*/
    PPM_PortWriteAsyCB(OM_HSIC_IND_PORT_HANDLE, pucVirData, lLen);

    return;
}


/*****************************************************************************
 �� �� ��  : PPM_HsicCfgReadDataCB
 ��������  : ����ACPU��������HSIC CFG������ͨ��ICC���͸�OMģ��
 �������  : ��
 �������  : ��
 �� �� ֵ  : BSP_ERROR/BSP_OK
 �޸���ʷ  :
  1.��    ��   : 2014��5��31��
    ��    ��   : h59254
    �޸�����   : V8R1 OM_Optimize��Ŀ����
*****************************************************************************/
s32 PPM_HsicCfgReadDataCB(void)
{
    if (OM_HSIC_PORT_STATUS_OFF == PPM_GetHsicPortStatus())
    {
        return BSP_OK;
    }

    return PPM_ReadPortData(CPM_HSIC_CFG_PORT, g_astOMPortUDIHandle[OM_HSIC_CFG_PORT_HANDLE], OM_HSIC_CFG_PORT_HANDLE);
}

/*****************************************************************************
 �� �� ��  : PPM_HsicCfgWriteDataCB
 ��������  : ���ڴ���Hsic CFG�ڵ��첽�������ݵĻص�
 �������  : pucData:   ��Ҫ���͵���������
             ulDataLen: ���ݳ���
 �������  : ��
 �� �� ֵ  : ��
 �޸���ʷ  :
  1.��    ��   : 2014��5��31��
    ��    ��   : h59254
    �޸�����   : V8R1 OM_Optimize��Ŀ����
*****************************************************************************/
void PPM_HsicCfgWriteDataCB(u8* pucVirData, u8* pucPhyData, s32 lLen)
{
    /*��ǰֻ����OM����*/
    PPM_PortWriteAsyCB(OM_HSIC_CFG_PORT_HANDLE, pucVirData, lLen);

    return;
}

/*****************************************************************************
 �� �� ��  : PPM_HsicCfgPortOpen
 ��������  : ���ڳ�ʼ��OMʹ�õ�Hsic CFG�˿�
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
  1.��    ��   : 2014��5��31��
    ��    ��   : h59254
    �޸�����   : V8R1 OM_Optimize��Ŀ����
*****************************************************************************/
void PPM_HsicCfgPortOpen(void)
{

    PPM_ReadPortDataInit(CPM_HSIC_CFG_PORT,
                           OM_HSIC_CFG_PORT_HANDLE,
                           PPM_HsicCfgReadDataCB,
                           PPM_HsicCfgWriteDataCB,
                           NULL);

    return;
}

/*****************************************************************************
 �� �� ��  : PPM_HsicIndPortOpen
 ��������  : ���ڳ�ʼ��OMʹ�õ�Hsic IND�˿�
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��
 �޸���ʷ      :
  1.��    ��   : 2014��5��31��
    ��    ��   : h59254
    �޸�����   : V8R1 OM_Optimize��Ŀ����
*****************************************************************************/
void PPM_HsicIndPortOpen(void)
{
    /* HSIC IND �˿ڲ��������ݣ�û�жϿ����� */
    PPM_ReadPortDataInit(CPM_HSIC_IND_PORT,
                           OM_HSIC_IND_PORT_HANDLE,
                           NULL,
                           PPM_HsicIndWriteDataCB,
                           NULL);

    return;
}

/*****************************************************************************
 �� �� ��  : PPM_HsicIndPortClose
 ��������  : Hsic IND�˿��Ѿ���ʧ����Ҫ�ر�USB�˿�
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :
 �޸���ʷ  :
  1.��    ��   : 2014��5��31��
    ��    ��   : h59254
    �޸�����   : V8R1 OM_Optimize��Ŀ����
*****************************************************************************/
void PPM_HsicIndPortClose(void)
{
    PPM_PortCloseProc(OM_HSIC_IND_PORT_HANDLE, CPM_HSIC_IND_PORT);

    return;
}

/*****************************************************************************
 �� �� ��  : PPM_HsicCfgPortClose
 ��������  : Hsic IND�˿��Ѿ���ʧ����Ҫ�ر�USB�˿�
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��
 �޸���ʷ  :
  1.��    ��   : 2014��5��31��
    ��    ��   : h59254
    �޸�����   : V8R1 OM_Optimize��Ŀ����
*****************************************************************************/
void PPM_HsicCfgPortClose(void)
{
    PPM_PortCloseProc(OM_HSIC_CFG_PORT_HANDLE, CPM_HSIC_CFG_PORT);

    return;
}

/*****************************************************************************
 �� �� ��  : GU_OamHsicIndSendData
 ��������  : �����������ͨ��Hsic IND���͸�PC��
 �������  : pucVirAddr:   �������ַ
             pucPhyAddr:   ����ʵ��ַ
             ulDataLen: ���ݳ���
 �������  : ��
 �� �� ֵ  : VOS_ERROR/BSP_OK
 �޸���ʷ  :
  1.��    ��   : 2014��5��31��
    ��    ��   : h59254
    �޸�����   : V8R1 OM_Optimize��Ŀ����
*****************************************************************************/
u32 PPM_HsicIndSendData(u8 *pucVirAddr, u8 *pucPhyAddr, u32 ulDataLen)
{
    return PPM_PortSend(OM_HSIC_IND_PORT_HANDLE, pucVirAddr, pucPhyAddr, ulDataLen);
}

/*****************************************************************************
 �� �� ��  : PPM_HsicCfgSendData
 ��������  : �����������ͨ��Hsic CFG���͸�PC��
 �������  : pucVirAddr:   �������ַ
             pucPhyAddr:   ����ʵ��ַ
             ulDataLen: ���ݳ���
 �������  : ��
 �� �� ֵ  : VOS_ERROR/BSP_OK
 �޸���ʷ  :
  1.��    ��   : 2014��5��31��
    ��    ��   : h59254
    �޸�����   : V8R1 OM_Optimize��Ŀ����
*****************************************************************************/
u32 PPM_HsicCfgSendData(u8 *pucVirAddr, u8 *pucPhyAddr, u32 ulDataLen)
{
    return PPM_PortSend(OM_HSIC_CFG_PORT_HANDLE, pucVirAddr, pucPhyAddr, ulDataLen);
}

/*****************************************************************************
 �� �� ��  : PPM_HsicIndStatusCB
 ��������  : ����ACPU���洦��HSIC IND�˿ڶϿ����OM���ӶϿ�
 �������  : enPortState:   �˿�״̬
 �������  : ��
 �� �� ֵ  : ��
 �޸���ʷ  :
  1.��    ��   : 2014��5��31��
    ��    ��   : h59254
    �޸�����   : V8R1 OM_Optimize��Ŀ����
*****************************************************************************/
void PPM_HsicIndStatusCB(ACM_EVT_E enPortState)
{
    PPM_PortStatus(OM_HSIC_IND_PORT_HANDLE, CPM_HSIC_IND_PORT, enPortState);

    return;
}

/*****************************************************************************
 �� �� ��  : PPM_HsicIndStatusCB
 ��������  : ����ACPU���洦��HSIC CFG�˿ڶϿ����OM���ӶϿ�
 �������  : enPortState:   �˿�״̬
 �������  : ��
 �� �� ֵ  : ��
 �޸���ʷ  :
  1.��    ��   : 2014��5��31��
    ��    ��   : h59254
    �޸�����   : V8R1 OM_Optimize��Ŀ����
*****************************************************************************/
void PPM_HsicCfgStatusCB(ACM_EVT_E enPortState)
{
    PPM_PortStatus(OM_HSIC_CFG_PORT_HANDLE, CPM_HSIC_CFG_PORT, enPortState);

    return;
}

/*****************************************************************************
 �� �� ��  : PPM_HsicPortInit
 ��������  : ����HSIC��ͨ���ĳ�ʼ��
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��
 �޸���ʷ  :
  1.��    ��   : 2014��5��31��
    ��    ��   : h59254
    �޸�����   : V8R1 OM_Optimize��Ŀ����
*****************************************************************************/
void PPM_HsicPortInit(void)
{
    /* ��Ʒ��֧��HSIC���ԣ�ֱ�ӷ��� */
    if (BSP_MODULE_SUPPORT != mdrv_misc_support_check(BSP_MODULE_TYPE_HSIC))
    {
        return;
    }

    /* ���HSICͨ���Ѿ�ö�ٳɹ�������Э��ջִ�г�ʼ�����������򽫳�ʼ������ע��������
        �ɵ�����HSICö�ٳɹ�������Խ��г�ʼ��*/
    if (true == DRV_GET_HSIC_ENUM_STATUS())
    {
        PPM_HsicIndPortOpen();
        PPM_HsicCfgPortOpen();
    }
    else
    {
        DRV_HSIC_REGUDI_ENABLECB((HSIC_UDI_ENABLE_CB_T)PPM_HsicIndPortOpen);
        DRV_HSIC_REGUDI_ENABLECB((HSIC_UDI_ENABLE_CB_T)PPM_HsicCfgPortOpen);
    }

    DRV_HSIC_REGUDI_DISABLECB((HSIC_UDI_DISABLE_CB_T)PPM_HsicIndPortClose);
    DRV_HSIC_REGUDI_DISABLECB((HSIC_UDI_DISABLE_CB_T)PPM_HsicCfgPortClose);

    CPM_PhySendReg(CPM_HSIC_IND_PORT,  PPM_HsicIndSendData);
    CPM_PhySendReg(CPM_HSIC_CFG_PORT,  PPM_HsicCfgSendData);

    return;
}

/*****************************************************************************
 Prototype       : PPM_HsicConnectProc
 Description     : OM�ں�HSIC����
 Input           : None
 Output          : None
 Return Value    : u32

 History         : ---
    Date         : 2012-04-09
    Author       : h59254
    Modification : AP-Modem����������Ŀ��������
 *****************************************************************************/
void PPM_HsicConnectProc(void)
{
    osl_sem_down(&g_ulOmAcpuHsicSem);

    /* ��Ʒ��֧��HSIC���ԣ�ֱ�ӳ�ʼ���ɹ� */
    if (BSP_MODULE_SUPPORT != mdrv_misc_support_check(BSP_MODULE_TYPE_HSIC))
    {
        (void)osl_sem_up(&g_ulOmAcpuHsicSem);

        return;
    }

    /* ����Ѿ������ϲ������� */
    if (OM_HSIC_PORT_STATUS_ON == g_ulOmHsicConnectStatus)
    {
        (void)osl_sem_up(&g_ulOmAcpuHsicSem);

        return;
    }

    /* ��ȫ�ֱ�������Ϊ�ѹ����� */
    g_ulOmHsicConnectStatus = OM_HSIC_PORT_STATUS_ON;

    (void)osl_sem_up(&g_ulOmAcpuHsicSem);

    return;
}

/*****************************************************************************
 Prototype       : PPM_HsicDisconnectProc
 Description     : OM�ں�HSIC�������
 Input           : None
 Output          : None
 Return Value    : u32

 History         : ---
    Date         : 2012-04-09
    Author       : h59254
    Modification : AP-Modem����������Ŀ��������
 *****************************************************************************/
void PPM_HsicDisconnectProc(void)
{
    /* ��Ʒ��֧��HSIC���ԣ�ֱ�ӳ�ʼ���ɹ� */
    if (BSP_MODULE_SUPPORT != mdrv_misc_support_check(BSP_MODULE_TYPE_HSIC))
    {
        return;
    }

    /* ����Ѿ���δ����״̬��������������� */
    if (OM_HSIC_PORT_STATUS_OFF == g_ulOmHsicConnectStatus)
    {
        return;
    }

    /* ��ȫ�ֱ�������Ϊ�ѽ�������� */
    g_ulOmHsicConnectStatus = OM_HSIC_PORT_STATUS_OFF;

    return;
}





