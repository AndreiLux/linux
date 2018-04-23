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
#include <linux/module.h>
#include <bsp_socp.h>
#include <bsp_nvim.h>
#include <bsp_slice.h>
#include <nv_stru_drv.h>
#include <acore_nv_stru_drv.h>
#include "diag_port_manager.h"
#include "scm_ind_src.h"
#include "scm_ind_dst.h"
#include "scm_cnf_src.h"
#include "scm_cnf_dst.h"
#include "diag_system_debug.h"
#include "diag_port_manager.h"
#include "scm_common.h"
#include "OmCommonPpm.h"


/* ****************************************************************************
  2 ȫ�ֱ�������
**************************************************************************** */
/* ����ACPU��USB�豸��UDI��� */
UDI_HANDLE                              g_astOMPortUDIHandle[OM_PORT_HANDLE_BUTT];

/* USB���ص�OM IND�˿��У�α��Ϊͬ���ӿ�ʹ�õ����ݽṹ�� */
OM_PSEUDO_SYNC_STRU                     g_stUsbIndPseudoSync;

/* USB���ص�OM CNF�˿��У�α��Ϊͬ���ӿ�ʹ�õ����ݽṹ�� */
OM_PSEUDO_SYNC_STRU                     g_stUsbCfgPseudoSync;

u32                              g_ulUSBSendErrCnt   = 0;

/* �˿��л���Ϣ�����ݽṹ�� */
PPM_PORT_CFG_INFO_STRU                  g_stPpmPortSwitchInfo;

/* ��������������AT����˿��л����ٽ���Դ���� */
spinlock_t                            g_stPpmPortSwitchSpinLock;



OM_ACPU_DEBUG_INFO                      g_stAcpuDebugInfo;

u32                              g_ulOmAcpuDbgFlag = false;
/*****************************************************************************
  3 �ⲿ��������
*****************************************************************************/
extern DIAG_CHANNLE_PORT_CFG_STRU         g_stPortCfg;
extern spinlock_t                     g_stScmSoftDecodeDataRcvSpinLock;
extern spinlock_t                     g_stCbtScmDataRcvSpinLock;
PPM_DisconnectTLPortFuc g_disconnectCb = NULL;

/*****************************************************************************
  4 ����ʵ��
*****************************************************************************/


void PPM_RegDisconnectCb(PPM_DisconnectTLPortFuc cb)
{
    g_disconnectCb = cb;
}
/*****************************************************************************
 �� �� ��  : PPM_DisconnectAllPort
 ��������  : GUTL�Ͽ�OM�˿�
 �������  : ��
 �������  : ��
 �� �� ֵ  : BSP_ERROR/BSP_OK
 �޸���ʷ  :
   1.��    ��  : 2014��05��26��
     ��    ��  : h59254
     �޸�����  : Creat Function
*****************************************************************************/
void PPM_DisconnectAllPort(OM_LOGIC_CHANNEL_ENUM_UINT32 enChannel)
{
    if(g_disconnectCb)
    {
        if(g_disconnectCb())
        {
            ppm_printf("disconnect error\n");
        }
    }
    else
    {
        ppm_printf("disconnect func is null\n");
    }
    return;
}

/*****************************************************************************
 �� �� ��  : PPM_GetSendDataLen
 ��������  : PPM�����ݷ��ͳ��Ƚ��д���
 �������  : ulDataLen: SOCPͨ�������ݳ���
             enChanID:  SOCP channel ID
 �������  : pulSendDataLen:ʵ�ʷ��ͳ�ȥ�����ݳ���
             penPhyport:ָ������˿ں�
 �� �� ֵ  : void

 �޸���ʷ      :
  1.��    ��   : 2014��5��25��
    ��    ��   : h59254
    �޸�����   : V8R1 OM_Optimize��Ŀ����
*****************************************************************************/
void PPM_GetSendDataLen(SOCP_CODER_DST_ENUM_U32 enChanID, u32 ulDataLen, u32 *pulSendDataLen, CPM_PHY_PORT_ENUM_UINT32 *penPhyport)
{
    CPM_PHY_PORT_ENUM_UINT32    enPhyport;

    if (SOCP_CODER_DST_OM_CNF == enChanID)
    {
        enPhyport = CPM_QueryPhyPort(CPM_OM_CFG_COMM);
    }
    else
    {
        enPhyport = CPM_QueryPhyPort(CPM_OM_IND_COMM);
    }


    /*��������ͨ��USB���ҷ��ͳ��ȴ���60k��ʱ����Ҫ���Ʒ��ͳ���*/
    if (((CPM_IND_PORT == enPhyport) || (CPM_CFG_PORT == enPhyport))
        &&(ulDataLen > USB_MAX_DATA_LEN))
    {

        *pulSendDataLen = USB_MAX_DATA_LEN;
    }
    else
    {
        *pulSendDataLen = ulDataLen;  /*��������²���Ҫ������ǰ�Ĵ�С������sd��wifi*/
    }

    *penPhyport = enPhyport;
    return;
}

/*****************************************************************************
 �� �� ��  : PPM_PortStatus
 ��������  : ����ACPU���洦������˿ڶϿ����OM���ӶϿ�
 �������  : enHandle:   UDI�豸���
             enPhyPort:  ����˿�
             enPortState:�˿�״̬
 �������  : ��
 �� �� ֵ  : ��

 �޸���ʷ      :
  1.��    ��   : 2014��5��25��
    ��    ��   : h59254
    �޸�����   : V8R1 OM_Optimize��Ŀ����
*****************************************************************************/
void PPM_PortStatus(OM_PROT_HANDLE_ENUM_UINT32 enHandle, CPM_PHY_PORT_ENUM_UINT32 enPhyPort,ACM_EVT_E enPortState)
{
    unsigned long                           ulLockLevel;
    OM_LOGIC_CHANNEL_ENUM_UINT32            enChannel;
    bool                                ulSndMsg;
    u32 ulSlicehigh, ulSlicelow;


    if (ACM_EVT_DEV_SUSPEND == enPortState)
    {
        (void)mdrv_timer_get_accuracy_timestamp(&ulSlicehigh, &ulSlicelow);

        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOutNum++;

        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOutTime = ulSlicelow;

        ppm_printf("Receive USB disconnect (chan %d) callback at slice 0x%x %08x!\n",enHandle, ulSlicehigh, ulSlicelow);

        scm_SpinLockIntLock(&g_stPpmPortSwitchSpinLock, ulLockLevel);

        ulSndMsg  = false;
        enChannel = OM_LOGIC_CHANNEL_BUTT;

        /* CFG�˿ڴ���GU��TL�Ķ˿ڶϿ�������Ϣ��GU��TLȥ���������Ͽ�CPM�Ĺ��� */
        if (OM_USB_CFG_PORT_HANDLE == enHandle)
        {
            if (enPhyPort == CPM_QueryPhyPort(CPM_OM_CFG_COMM))
            {
                ulSndMsg  = true;
                enChannel = OM_LOGIC_CHANNEL_CNF;
            }
        }
        /* IND�˿ڶϿ�ʱ����Ϣ��GU��TLȥ���� */
        else if (OM_USB_IND_PORT_HANDLE == enHandle)
        {
            if (enPhyPort == CPM_QueryPhyPort(CPM_OM_IND_COMM))
            {
                ulSndMsg  = true;
                enChannel = OM_LOGIC_CHANNEL_IND;
            }
        }
        else
        {

        }

        scm_SpinUnlockIntUnlock(&g_stPpmPortSwitchSpinLock, ulLockLevel);

        if (true == ulSndMsg)
        {
            PPM_DisconnectAllPort(enChannel);
        }
    }
    else if(ACM_EVT_DEV_READY == enPortState)
    {
        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBINNum++;

        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBINTime = mdrv_timer_get_normal_timestamp();
    }
    else
    {
        ppm_printf("PPM_PortStatus: The USB Port %d State %d is Unknow", (s32)enPhyPort, (s32)enPortState);

        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBStateErrNum++;

        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBStateErrTime = mdrv_timer_get_normal_timestamp();
    }

    return;
}

/*****************************************************************************
 �� �� ��  : PPM_PortCloseProc
 ��������  : OM�˿��Ѿ���ʧ����Ҫ�ر�USB�˿�
 �������  : enHandle: �˿ھ��ö�ٶ��壬�رպ�˿�����ΪVOS_ERROR
             enPhyPort:����˿�
 �������  : ��
 �� �� ֵ  : ��

 �޸���ʷ      :
  1.��    ��   : 2014��5��25��
    ��    ��   : h59254
    �޸�����   : V8R1 OM_Optimize��Ŀ����
*****************************************************************************/
void PPM_PortCloseProc(OM_PROT_HANDLE_ENUM_UINT32  enHandle, CPM_PHY_PORT_ENUM_UINT32 enPhyPort)
{
    unsigned long                           ulLockLevel;
    OM_LOGIC_CHANNEL_ENUM_UINT32        enChannel;
    bool                            ulSndMsg;

    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCloseNum++;
    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCloseSlice = mdrv_timer_get_normal_timestamp();

    if (BSP_ERROR == g_astOMPortUDIHandle[enHandle])
    {
        return;
    }

    mdrv_udi_close(g_astOMPortUDIHandle[enHandle]);

    g_astOMPortUDIHandle[enHandle] = BSP_ERROR;

    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCloseOkNum++;
    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCloseOkSlice = mdrv_timer_get_normal_timestamp();

    scm_SpinLockIntLock(&g_stPpmPortSwitchSpinLock, ulLockLevel);

    ulSndMsg  = false;
    enChannel = OM_LOGIC_CHANNEL_BUTT;

    /* CFG�˿ڴ���GU��TL�Ķ˿ڶϿ�������Ϣ��GU��TLȥ���������Ͽ�CPM�Ĺ��� */
    if (OM_USB_CFG_PORT_HANDLE == enHandle)
    {
        if (enPhyPort == CPM_QueryPhyPort(CPM_OM_CFG_COMM))
        {
            ulSndMsg  = true;
            enChannel = OM_LOGIC_CHANNEL_CNF;
        }
    }
    /* IND�˿ڶϿ�ʱ����Ϣ��GU��TLȥ���������Ͽ�CPM�Ĺ��� */
    else if (OM_USB_IND_PORT_HANDLE == enHandle)
    {
        if (enPhyPort == CPM_QueryPhyPort(CPM_OM_IND_COMM))
        {
            ulSndMsg  = true;
            enChannel = OM_LOGIC_CHANNEL_IND;
        }
    }
    else
    {

    }

    scm_SpinUnlockIntUnlock(&g_stPpmPortSwitchSpinLock, ulLockLevel);

    if (true == ulSndMsg)
    {
        PPM_DisconnectAllPort(enChannel);
    }

    return;
}

/*****************************************************************************
 �� �� ��  : GU_OamUSBReadData
 ��������  : ����ACPU������������ͨ��ICC���͸�OMģ��
 �������  : enPhyPort: ����˿�
             UdiHandle:�豸���
             enHandle: UDI�豸�������ƫ��
 �������  : ��
 �� �� ֵ  : BSP_ERROR/BSP_OK

 �޸���ʷ      :
  1.��    ��   : 2014��5��25��
    ��    ��   : h59254
    �޸�����   : V8R1 OM_Optimize��Ŀ����
*****************************************************************************/
u32 PPM_ReadPortData(CPM_PHY_PORT_ENUM_UINT32 enPhyPort, UDI_HANDLE UdiHandle, OM_PROT_HANDLE_ENUM_UINT32 enHandle)
{
    ACM_WR_ASYNC_INFO                   stInfo={NULL,NULL,0,NULL};
    u32 ret = 0xFFFFFFFF;

    diag_PTR(EN_DIAG_PTR_PPM_READDATA, 0, 0, 0);

    if (BSP_ERROR == UdiHandle)
    {
        ppm_printf(" PPM_PortCloseProc: Input HANDLE  is err. \n");

        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBUdiHandleErr++;

        diag_PTR(EN_DIAG_PTR_PPM_ERR1, 0, 0, 0);

        return (u32)BSP_ERROR;
    }
    /* coverity[secure_coding] */
    (void)memset(&stInfo, 0, sizeof(ACM_WR_ASYNC_INFO));

    /* ��ȡUSB��IO CTRL�ڵĶ����� */
    if (BSP_OK != mdrv_udi_ioctl(UdiHandle, UDI_ACM_IOCTL_GET_READ_BUFFER_CB, &stInfo))
    {
        ppm_printf(" PPM_ReadPortData:Call mdrv_udi_ioctl is Failed\n");

        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBUdiHandleReadGetBufferErr++;
        diag_PTR(EN_DIAG_PTR_PPM_ERR2, 0, 0, 0);

        return (u32)BSP_ERROR;
    }

    OM_ACPU_DEBUG_CHANNEL_TRACE(enPhyPort, (u8*)stInfo.pVirAddr, stInfo.u32Size,OM_ACPU_USB_CB,OM_ACPU_DATA);

    ret = CPM_ComRcv(enPhyPort, (u8*)stInfo.pVirAddr, stInfo.u32Size);
    if(BSP_OK != ret)
    {
        ppm_printf("CPM_ComRcv return error, PhyPort is %d, ret = 0x%x\n", (s32)enPhyPort, ret);

        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBUdiCommRcvNullPtrErr++;
    }

    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBRcvPktNum++;
    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBRcvPktByte += stInfo.u32Size;

    if(BSP_OK != mdrv_udi_ioctl(UdiHandle, UDI_ACM_IOCTL_RETUR_BUFFER_CB, &stInfo))
    {
        ppm_printf(" PPM_ReadPortData:Call mdrv_udi_ioctl UDI_ACM_IOCTL_RETUR_BUFFER_CB is Failed\n");

        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBUdiHandleReadBufferFreeErr++;
    }

    return BSP_OK;
}


/*****************************************************************************
 �� �� ��  : PPM_PortPseudoSyncGetSmp
 ��������  : αͬ���ӿڣ���Ҫ�ڵ��õ����첽д�����󣬻�ȡ�ź���
 �������  : enHandle:UDI�豸���
 �������  : void
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��5��25��
    ��    ��   : L00256032
    �޸�����   : V8R1 OM_Optimize��Ŀ����
*****************************************************************************/
void PPM_PortPseudoSyncGetSmp(OM_PROT_HANDLE_ENUM_UINT32 enHandle)
{
    return;
}

/*****************************************************************************
 �� �� ��  : PPM_QueryLogPort
 ��������  : �ṩ��NAS����Log�˿ڲ�ѯ
 �������  : ��
 �������  : pulLogPort��ǰLog����˿�

 �� �� ֵ  : BSP_ERROR/BSP_OK

 �޸���ʷ  :
   1.��    ��  : 2014��06��3��
     ��    ��  : h59254
     �޸�����  : Creat Function
*****************************************************************************/
u32 PPM_QueryLogPort(u32  *pulLogPort)
{
    if (NULL == pulLogPort)
    {
        (void)ppm_printf(" PPM_QueryLogPort: para is NULL %d %d %d!!\n", 0,0,0);
        return (u32)BSP_ERROR;
    }

    *pulLogPort = g_stPortCfg.enPortNum;

    if ((CPM_OM_PORT_TYPE_USB != *pulLogPort) && (CPM_OM_PORT_TYPE_VCOM != *pulLogPort))
    {
        return (u32)BSP_ERROR;
    }

    return BSP_OK;
}

/*****************************************************************************
 �� �� ��  : PPM_LogPortSwitch
 ��������  : �ṩ��NAS���ж˿��л�
 �������  : enPhyPort: ���л�����˿�ö��ֵ
             ulEffect:�Ƿ�������Ч
 �������  : ��
 �� �� ֵ  : BSP_ERROR/BSP_OK

 �޸���ʷ  :
   1.��    ��  : 2014��6��3��
     ��    ��  : h59254
     �޸�����  : Creat Function
*****************************************************************************/
u32 PPM_LogPortSwitch(u32  ulPhyPort, bool ulEffect)
{
    CPM_PHY_PORT_ENUM_UINT32            enPhyCfgPort;
    CPM_PHY_PORT_ENUM_UINT32            enPhyIndPort;
    unsigned long                           ulLockLevel;
    bool                            ulSndMsg;
    u32                           ret;
    u32                           ulCompressState;

    if ((CPM_OM_PORT_TYPE_USB != ulPhyPort) && (CPM_OM_PORT_TYPE_VCOM != ulPhyPort))
    {
        (void)ppm_printf(" PPM_LogPortSwitch: enPhyPort is error.port %d\n", ulPhyPort);

        g_stPpmPortSwitchInfo.ulPortTypeErr++;

        return (u32)BSP_ERROR;
    }

    (void)ppm_printf("PPM_LogPortSwitch ulPhyPort:%s ulEffect:%s.\n",
                     (ulPhyPort == CPM_OM_PORT_TYPE_USB) ? "USB"  : "VCOM",
                     (ulEffect  == true)             ? "TRUE" : "FALSE");

    /* �л��Ķ˿��뵱ǰ�˿�һ�²��л� */
    if (ulPhyPort == g_stPortCfg.enPortNum)
    {
        /* Ϊ�˹��USB���ʱ��������ʱд���޷����ӹ���,�л���USB���ʱ��Ҫ��������SOCP�ĳ�ʱ�жϵ�Ĭ��ֵ */
        if (CPM_OM_PORT_TYPE_USB == g_stPortCfg.enPortNum)
        {
            ret = bsp_socp_set_ind_mode(SOCP_IND_MODE_DIRECT);
            if(BSP_OK != ret)
            {
                (void)ppm_printf("PPM_LogPortSwitch: set socp ind mode failed 0x%x %d %d\n", ret, __LINE__, 0);
                return (u32)ret;
            }
        }
        (void)ppm_printf("PPM_LogPortSwitch Set same port, don't need to do anything.\n");

        return BSP_OK;
    }

    g_stPpmPortSwitchInfo.ulStartSlice = mdrv_timer_get_normal_timestamp();

    enPhyCfgPort = CPM_QueryPhyPort(CPM_OM_CFG_COMM);
    enPhyIndPort = CPM_QueryPhyPort(CPM_OM_IND_COMM);

    ulSndMsg = false;

    scm_SpinLockIntLock(&g_stPpmPortSwitchSpinLock, ulLockLevel);

    /* �л���VCOM��� */
    if (CPM_OM_PORT_TYPE_VCOM == ulPhyPort)
    {
        /* ��ǰ��USB��� */
        if ((CPM_CFG_PORT == enPhyCfgPort) && (CPM_IND_PORT == enPhyIndPort))
        {
            /* ��Ҫ�Ͽ����� */
            ulSndMsg = true;

            CPM_DisconnectPorts(CPM_CFG_PORT, CPM_OM_CFG_COMM);
            CPM_DisconnectPorts(CPM_IND_PORT, CPM_OM_IND_COMM);
        }

        /* ��ǰOM��VCOM�ϱ� */
        CPM_ConnectPorts(CPM_VCOM_CFG_PORT, CPM_OM_CFG_COMM);
        CPM_ConnectPorts(CPM_VCOM_IND_PORT, CPM_OM_IND_COMM);

        g_stPortCfg.enPortNum = CPM_OM_PORT_TYPE_VCOM;
    }
    /* �л���USB��� */
    else
    {
        /* ��ǰ��VCOM��� */
        if ((CPM_VCOM_CFG_PORT == enPhyCfgPort) && (CPM_VCOM_IND_PORT == enPhyIndPort))
        {
            /* �Ͽ����� */
            ulSndMsg = true;

            CPM_DisconnectPorts(CPM_VCOM_CFG_PORT, CPM_OM_CFG_COMM);
            CPM_DisconnectPorts(CPM_VCOM_IND_PORT, CPM_OM_IND_COMM);
        }

        /* OM��USB�ϱ� */
        CPM_ConnectPorts(CPM_CFG_PORT, CPM_OM_CFG_COMM);
        CPM_ConnectPorts(CPM_IND_PORT, CPM_OM_IND_COMM);

        g_stPortCfg.enPortNum = CPM_OM_PORT_TYPE_USB;
    }

    scm_SpinUnlockIntUnlock(&g_stPpmPortSwitchSpinLock, ulLockLevel);

    if (true == ulSndMsg)
    {
        PPM_DisconnectAllPort(OM_LOGIC_CHANNEL_CNF);
    }

    /* Ϊ�˹��USB���ʱ��������ʱд���޷����ӹ���,�л���USB���ʱ��Ҫ��������SOCP�ĳ�ʱ�жϵ�Ĭ��ֵ */
    if (CPM_OM_PORT_TYPE_USB == g_stPortCfg.enPortNum)
    {

        ulCompressState = mdrv_deflate_read_cur_mode();
        if(COMPRESS_ENABLE_STATE==ulCompressState)

        {
            ret= mdrv_socp_compress_disable(SOCP_CODER_DST_OM_IND);
            if(BSP_OK != ret)
            {
                (void)ppm_printf("PPM_LogPortSwitch: deflate disable failed 0x%x %d\n", ret, __LINE__, 0);
                return (u32)ret;
            }
        }
            ret = bsp_socp_set_ind_mode(SOCP_IND_MODE_DIRECT);
            if(BSP_OK != ret)
            {
                (void)ppm_printf("PPM_LogPortSwitch: set socp ind mode failed 0x%x %d\n", ret, __LINE__, 0);
                return (u32)ret;

            }
    }
    g_stPpmPortSwitchInfo.ulSwitchSucc++;
    g_stPpmPortSwitchInfo.ulEndSlice = mdrv_timer_get_normal_timestamp();

    if (true == ulEffect)
    {
        if (BSP_OK != bsp_nvm_write(NV_ID_DRV_DIAG_PORT, (u8 *)&g_stPortCfg, sizeof(DIAG_CHANNLE_PORT_CFG_STRU)))
        {
            (void)ppm_printf("PPM_LogPortSwitch: NV_Write fail!\n");
        }
        else
        {
            (void)ppm_printf("PPM_LogPortSwitch: NV_Write success!\n");
        }
    }

    (void)ppm_printf("PPM_LogPortSwitch: Set port success!\n");

    return BSP_OK;
}


/*****************************************************************************
 �� �� ��  : PPM_UdiRegCallBackFun
 ��������  : ����ע��UDI�豸�Ļص�����
 �������  : enHandle: �˿ڵľ��
             ulCmdType: ��������
             pFunc: ע��Ļص�����ָ��
 �������  : ��
 �� �� ֵ  : BSP_OK/BSP_ERROR
 �޸���ʷ      :
  1.��    ��   : 2014��5��31��
    ��    ��   : h59254
    �޸�����   : V8R1 OM_Optimize��Ŀ����
*****************************************************************************/
u32 PPM_UdiRegCallBackFun(UDI_HANDLE enHandle, u32 ulCmdType, void* pFunc)
{
    if (NULL == pFunc)
    {
        return BSP_OK;
    }

    if (BSP_OK != mdrv_udi_ioctl(enHandle, ulCmdType, pFunc))
    {
        ppm_printf(" PPM_UdiRegCallBackFun, mdrv_udi_ioctl Failed\n");

        return (u32)BSP_ERROR;
    }

    return BSP_OK;
}


#define OM_SOCP_CNF_BUFFER_SIZE          (8*1024)
#define OM_SOCP_CNF_BUFFER_NUM           (8)

#define OM_SOCP_IND_BUFFER_SIZE          (2*1024)
#define OM_SOCP_IND_BUFFER_NUM           (2)


/*****************************************************************************
 �� �� ��  : PPM_ReadPortDataInit
 ��������  : ���ڳ�ʼ��OMʹ�õ��豸
 �������  : enPhyPort: ����˿ں�
             enHandle: �˿ڵľ��
             pReadCB: �ö˿�����Ķ�ȡ�ص�����
             pWriteCB: �ö˿�������첽д�ص�����
             pStateCB: �ö˿������״̬�ص�����
 �������  : ��
 �� �� ֵ  : BSP_OK/BSP_ERROR
 �޸���ʷ      :
  1.��    ��   : 2014��5��31��
    ��    ��   : h59254
    �޸�����   : V8R1 OM_Optimize��Ŀ����
*****************************************************************************/
void PPM_ReadPortDataInit(CPM_PHY_PORT_ENUM_UINT32        enPhyPort,
                                    OM_PROT_HANDLE_ENUM_UINT32          enHandle,
                                    void                            *pReadCB,
                                    void                            *pWriteCB,
                                    void                            *pStateCB)
{
    UDI_OPEN_PARAM_S                    stUdiPara;
    ACM_READ_BUFF_INFO                  stReadBuffInfo;
    bool                            isCnf = true;

    /*��ʼ����ǰʹ�õ�USBͨ��*/
    /* Add by h59254 for V8R1 OM begin */
    if (CPM_IND_PORT == enPhyPort)
    {
        isCnf                      = false;
        stUdiPara.devid            = UDI_ACM_LTE_DIAG_ID;
    }
    else if (CPM_CFG_PORT == enPhyPort)
    {
        isCnf                      = true;
        stUdiPara.devid            = UDI_ACM_GPS_ID;
    }
    else if (CPM_HSIC_IND_PORT == enPhyPort)
    {
        isCnf                      = false;
        stUdiPara.devid            = UDI_ACM_HSIC_ACM7_ID;
    }
    else if (CPM_HSIC_CFG_PORT == enPhyPort)
    {
        isCnf                      = true;
        stUdiPara.devid            = UDI_ACM_HSIC_ACM8_ID;
    }
    /* Add by h59254 for V8R1 OM end */
    else
    {
        ppm_printf("PPM_ReadPortDataInit: Open Wrong Port %d!", (s32)enPhyPort);

        return;
    }

    if(isCnf == true)
    {
        stReadBuffInfo.u32BuffSize = OM_SOCP_CNF_BUFFER_SIZE;
        stReadBuffInfo.u32BuffNum  = OM_SOCP_CNF_BUFFER_NUM;
    }
    else
    {
        stReadBuffInfo.u32BuffSize = OM_SOCP_IND_BUFFER_SIZE;
        stReadBuffInfo.u32BuffNum  = OM_SOCP_IND_BUFFER_NUM;
    }

    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenNum++;
    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenSlice = mdrv_timer_get_normal_timestamp();

    if (BSP_ERROR != g_astOMPortUDIHandle[enHandle])
    {
        ppm_printf("PPM_ReadPortDataInit: The UDI Handle is not Null !");

        return;
    }

    /* ��OMʹ�õ�USBͨ�� */
    g_astOMPortUDIHandle[enHandle] = mdrv_udi_open(&stUdiPara);

    if (BSP_ERROR == g_astOMPortUDIHandle[enHandle])
    {
        ppm_printf("PPM_ReadPortDataInit: Open UDI ACM failed!");

        return;
    }

    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenOkNum++;
    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenOkSlice = mdrv_timer_get_normal_timestamp();

    /* ����OMʹ�õ�USBͨ������ */
    if (BSP_OK != mdrv_udi_ioctl(g_astOMPortUDIHandle[enHandle], ACM_IOCTL_RELLOC_READ_BUFF, &stReadBuffInfo))
    {
        ppm_printf(" PPM_ReadPortDataInit, mdrv_udi_ioctl ACM_IOCTL_RELLOC_READ_BUFF Failed\r\n");

        return;
    }

    /* ע��OMʹ�õ�USB�����ݻص����� */
    if (BSP_OK != PPM_UdiRegCallBackFun(g_astOMPortUDIHandle[enHandle], UDI_ACM_IOCTL_SET_READ_CB, pReadCB))
    {
        ppm_printf("PPM_ReadPortDataInit, mdrv_udi_ioctl UDI_ACM_IOCTL_SET_READ_CB Failed\r\n");

        return;
    }

    if(BSP_OK != PPM_UdiRegCallBackFun(g_astOMPortUDIHandle[enHandle], ACM_IOCTL_SET_WRITE_CB, pWriteCB))
    {
        ppm_printf("PPM_ReadPortDataInit, mdrv_udi_ioctl ACM_IOCTL_SET_WRITE_CB Failed\r\n");

        return;
    }

    if(BSP_OK != PPM_UdiRegCallBackFun(g_astOMPortUDIHandle[enHandle], ACM_IOCTL_SET_EVT_CB, pStateCB))
    {
        ppm_printf("PPM_ReadPortDataInit, mdrv_udi_ioctl ACM_IOCTL_SET_WRITE_CB Failed\r\n");

        return;
    }

    if (BSP_OK != mdrv_udi_ioctl(g_astOMPortUDIHandle[enHandle], ACM_IOCTL_WRITE_DO_COPY, NULL))
    {
        ppm_printf("PPM_ReadPortDataInit, mdrv_udi_ioctl ACM_IOCTL_WRITE_DO_COPY Failed\r\n");

        return;
    }

    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenOk2Num++;
    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenOk2Slice = mdrv_timer_get_normal_timestamp();

    return;
}

/*****************************************************************************
 �� �� ��  : PPM_PortSend
 ��������  : �����������ͨ���첽���͸�PC��
 �������  :  enHandle: ���Ͷ˿ھ����ö�ٶ���
              pucVirAddr:   �������ַ
              pucPhyAddr:   ����ʵ��ַ
              ulDataLen: ���ݳ���
 �������  : ��
 �� �� ֵ  : BSP_ERROR/BSP_OK
 �޸���ʷ      :
  1.��    ��   : 2014��5��31��
    ��    ��   : h59254
    �޸�����   : V8R1 OM_Optimize��Ŀ����
*****************************************************************************/
u32 PPM_PortSend(OM_PROT_HANDLE_ENUM_UINT32 enHandle, u8 *pucVirAddr, u8 *pucPhyAddr, u32 ulDataLen)
{
    s32           lRet;
    ACM_WR_ASYNC_INFO   stVcom;
    u32          ulInSlice;
    u32          ulOutSlice;
    u32          ulWriteSlice;


    if((OM_USB_CFG_PORT_HANDLE == enHandle)
        || (OM_HSIC_CFG_PORT_HANDLE == enHandle))
    {
        diag_PTR(EN_DIAG_PTR_PPM_PORTSEND, 1, enHandle, ulDataLen);
    }

    if ((NULL == pucVirAddr) || (NULL == pucPhyAddr))
    {
        /* ��ӡ���� */
        ppm_printf("PPM_PortSend: Vir or Phy Addr is Null \n");

        return CPM_SEND_PARA_ERR;
    }

    stVcom.pVirAddr = (char*)pucVirAddr;
    stVcom.pPhyAddr = (char*)pucPhyAddr;
    stVcom.u32Size  = ulDataLen;
    stVcom.pDrvPriv = NULL;

    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteNum1++;

    if (BSP_ERROR == g_astOMPortUDIHandle[enHandle])
    {
        /*ppm_printf("warning PPM_PortSend: USB HANDLE  enHandle = 0x%x\n", enHandle);*/
        g_stAcpuDebugInfo.ulInvaldChannel++;
        return CPM_SEND_ERR;
    }

    ulInSlice = bsp_get_slice_value();

    if(OM_USB_IND_PORT_HANDLE == enHandle)
    {
        g_stAcpuDebugInfo.stIndDebugInfo.ulUSBSendLen += ulDataLen;
        diag_system_debug_usb_len(ulDataLen);
        diag_system_debug_send_usb_start();
    }
    else
    {
        g_stAcpuDebugInfo.stCnfDebugInfo.ulUSBSendLen += ulDataLen;
    }

    /* ����д�����ݳ��ȴ���д�����ɹ� */
    lRet = (s32)mdrv_udi_ioctl(g_astOMPortUDIHandle[enHandle], ACM_IOCTL_WRITE_ASYNC, &stVcom);

    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteNum2++;

    ulOutSlice = bsp_get_slice_value();

    if(ulInSlice > ulOutSlice)
    {
        ulWriteSlice = (0xffffffff - ulInSlice) + ulOutSlice;
    }
    else
    {
        ulWriteSlice = ulOutSlice - ulInSlice;
    }

    if(ulWriteSlice > g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteMaxTime)
    {
        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteMaxTime = ulWriteSlice;
    }


    if (MDRV_OK == lRet)     /*��ǰ���ͳɹ�*/
    {
        /* αͬ���ӿڣ���ȡ�ź��� */
        PPM_PortPseudoSyncGetSmp(enHandle);
        if(OM_USB_IND_PORT_HANDLE == enHandle)
        {
            diag_system_debug_usb_ok_len(ulDataLen);
        }
        return CPM_SEND_AYNC;

    }
    else if(MDRV_OK > lRet)    /*��ʱ����*/
    {
        /*��ӡ��Ϣ������UDI�ӿڵĴ�����Ϣ*/
        ppm_printf("PPM_PortSend: mdrv_udi_ioctl Send Data return Error %d\n", lRet);
        if(OM_USB_IND_PORT_HANDLE == enHandle)
        {
            diag_system_debug_usb_fail_len(ulDataLen);
        }
        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteErrNum++;
        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteErrLen    += ulDataLen;
        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteErrValue  = (u32)lRet;
        g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteErrTime   = bsp_get_slice_value();

        g_ulUSBSendErrCnt++;

        return CPM_SEND_FUNC_NULL; /*������ʱ������Ҫ����NULL��������*/
    }
    else    /*����������Ҫ��λ����*/
    {
        if(OM_USB_IND_PORT_HANDLE == enHandle)
        {
            diag_system_debug_usb_fail_len(ulDataLen);
        }
        /*��ӡ��Ϣ������UDI�ӿ�*/
        ppm_printf("PPM_PortSend: mdrv_udi_ioctl Send Data return Error %d\n", lRet);

        /*system_error(DRV_ERRNO_USB_SEND_ERROR, lRet, (s32)enHandle,
                             (s8 *)&g_stAcpuDebugInfo, (s32)sizeof(OM_ACPU_DEBUG_INFO));*/

        return CPM_SEND_ERR;
    }
}

/*****************************************************************************
 �� �� ��  :GU_OamPortWriteAsyCB
 ��������  :OM�첽���ͻص�����
 �������  :enHandle:UDI���
            pucData:��Ҫ���͵���������
            lLen:���ݳ���
 �������  :��
 �� �� ֵ  :��
 �޸���ʷ      :
  1.��    ��   : 2014��5��31��
    ��    ��   : h59254
    �޸�����   : V8R1 OM_Optimize��Ŀ����
*****************************************************************************/
void PPM_PortWriteAsyCB(OM_PROT_HANDLE_ENUM_UINT32 enHandle, u8* pucData, s32 lLen)
{
    u32      ulRlsLen;


    if(lLen < 0)
    {
        ulRlsLen = 0;
    }
    else
    {
        ulRlsLen = (u32)lLen;
    }

    /* ͳ������ͨ���������� */
    if(OM_USB_IND_PORT_HANDLE == enHandle)
    {
        diag_ThroughputSave(EN_DIAG_THRPUT_DATA_CHN_CB, lLen);
        diag_system_debug_usb_free_len(ulRlsLen);
        diag_system_debug_send_data_end();
        diag_system_debug_send_usb_end();
    }

    /* αͬ���ӿڣ��ͷ��ź��� */
    if (OM_USB_IND_PORT_HANDLE == enHandle)
    {
        g_stUsbIndPseudoSync.ulLen          = ulRlsLen;
        g_stUsbIndPseudoSync.pucAsyncCBData = pucData;

        scm_rls_ind_dst_buff(ulRlsLen);
    }
    else if (OM_USB_CFG_PORT_HANDLE == enHandle)
    {
        g_stUsbCfgPseudoSync.ulLen          = ulRlsLen;
        g_stUsbCfgPseudoSync.pucAsyncCBData = pucData;

        scm_rls_cnf_dst_buff(ulRlsLen);
    }
    else
    {
        ;
    }

    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteCBNum++;

    return;
}


OM_PSEUDO_SYNC_STRU * PPM_ComPpmGetSyncInfo(void)
{
    return &g_stUsbIndPseudoSync;
}

OM_ACPU_DEBUG_INFO * PPM_ComPpmGetDebugInfo(void)
{
    return &g_stAcpuDebugInfo;
}


/*****************************************************************************
 �� �� ��  : PPM_InitPhyPort
 ��������  : ��ʼ������ͨ��
 �������  : void
 �������  : ��
 �� �� ֵ  : BSP_OK:�ɹ�������Ϊʧ��
 �޸���ʷ:
     1. ����:2012-01-21
         �޸���:s00207770
         �޸�ԭ��:���SOCKET�˿ڳ�ʼ��
     2. ����:2013-12-29
         �޸���:s00207770
         �޸�ԭ��:�޸�LOG�����ʼ���ӿ�
*****************************************************************************/
int PPM_InitPhyPort(void)
{
    if (BSP_OK != PPM_PortInit())
    {
        ppm_printf("PPM_InitPhyPort: PPM_PortInit failed.\n");
        return BSP_ERROR;
    }


    ppm_printf("diag ppm init ok\n");
    return BSP_OK;
}
module_init(PPM_InitPhyPort);

/*****************************************************************************
 �� �� ��  : PPM_PortInit
 ��������  : �˿�ͨ���ĳ�ʼ��
 �������  : ��
 �������  : ��
 �� �� ֵ  : BSP_OK   - ��ʼ���ɹ�
             BSP_ERROR  - ��ʼ��ʧ��

 �޸���ʷ      :
  1.��    ��   : 2014��5��31��
    ��    ��   : h59254
    �޸�����   : V8R1 OM_Optimize��Ŀ����
*****************************************************************************/
u32 PPM_PortInit(void)
{
    /* coverity[secure_coding] */
    (void)memset(&g_stAcpuDebugInfo, 0, sizeof(OM_ACPU_DEBUG_INFO));
    /* coverity[secure_coding] */
    (void)memset(g_astOMPortUDIHandle, BSP_ERROR, OM_PORT_HANDLE_BUTT*sizeof(UDI_HANDLE));
    /* coverity[secure_coding] */
    (void)memset(&g_stPpmPortSwitchInfo, 0, sizeof(PPM_PORT_CFG_INFO_STRU));

    scm_SpinLockInit(&g_stPpmPortSwitchSpinLock);

    /* USB���ص�����˿�ͨ���ĳ�ʼ�� */
    PPM_UsbPortInit();

    /* Hsic���ص�����˿�ͨ���ĳ�ʼ�� */
    PPM_HsicPortInit();

    /* Vcom���ص�����˿�ͨ���ĳ�ʼ�� */
    PPM_VComPortInit();

    return BSP_OK;
}


/*****************************************************************************
 �� �� ��  : OmOpenLog
 ��������  : ��ӡ��ǰOMͨ����״̬
 �������  :
 �������  :
 �� �� ֵ  :
 ���ú���  :
 ��������  :
 �޸���ʷ  :
   1.��    ��  : 2009��5��18��
     ��    ��  : g47350
     �޸�����  : Creat Function
*****************************************************************************/
void OmOpenLog(u32 ulFlag)
{
    g_ulOmAcpuDbgFlag = ulFlag;

    return;
}


/*****************************************************************************
 �� �� ��  : PPM_OmPortInfoShow
 ��������  : ���ڴ�ӡ��ǰͨ����ͳ����Ϣ
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :
 �޸���ʷ  :
   1.��    ��  : 2011��3��10��
     ��    ��  : l46160
     �޸�����  : Creat Function
*****************************************************************************/
void PPM_OmPortInfoShow(OM_PROT_HANDLE_ENUM_UINT32  enHandle)
{
    (void)ppm_printf("Invalidchanel num is             %d\n",   g_stAcpuDebugInfo.ulInvaldChannel);

    (void)ppm_printf("The Port Write num 1 is          %d\n",   g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteNum1);
    (void)ppm_printf("The Port Write num 2 is          %d\n",   g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteNum2);
    (void)ppm_printf("The Port Write Max Time is       0x%x\n", g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteMaxTime);

    (void)ppm_printf("The Port Write CB Num is         %d\n",   g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteCBNum);

    (void)ppm_printf("The Port Write Err Time is       %d\n",   g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteErrTime);
    (void)ppm_printf("The Port Write Err Num  is       %d\n",   g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteErrNum);
    (void)ppm_printf("The Port Write Err Value is      0x%x\n", g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteErrValue);
    (void)ppm_printf("The Port Write Err Len is        0x%x\n", g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBWriteErrLen);

    (void)ppm_printf("The Port In CB Num is            %d\n",   g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBINNum);
    (void)ppm_printf("The Port In CB Time is           0x%x\n", g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBINTime);
    (void)ppm_printf("The Port Out CB Num is           %d\n",   g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOutNum);
    (void)ppm_printf("The Port Out CB Time is          0x%x\n", g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOutTime);
    (void)ppm_printf("The Port State CB Err Num is     %d\n",   g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBStateErrNum);
    (void)ppm_printf("The Port State CB Err Time is    0x%x\n", g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBStateErrTime);

    (void)ppm_printf("The Port Open num is            %d\n",    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenNum);
    (void)ppm_printf("The Port Open slice is          0x%x\n",  g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenSlice);

    (void)ppm_printf("The Port Open OK num is         %d\n",    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenOkNum);
    (void)ppm_printf("The Port Open OK slice is       0x%x\n",  g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenOkSlice);

    (void)ppm_printf("The Port Open OK2 num is        %d\n",    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenOk2Num);
    (void)ppm_printf("The Port Open OK2 slice is      0x%x\n",  g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBOpenOk2Slice);

    (void)ppm_printf("The Port Close num is           %d\n",    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCloseNum);
    (void)ppm_printf("The Port Close slice is         0x%x\n",  g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCloseSlice);

    (void)ppm_printf("The Port Close OK num is        %d\n",    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCloseOkNum);
    (void)ppm_printf("The Port Close OK slice is      0x%x",  g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCloseOkSlice);

    (void)ppm_printf("USB IND Pseudo sync fail num is   %d\n",    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBIndPseudoSyncFailNum);
    (void)ppm_printf("USB IND Pseudo sync fail slice is 0x%x\n",  g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBIndPseudoSyncFailSlice);
    (void)ppm_printf("USB CFG Pseudo sync fail num is   %d\n",    g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCnfPseudoSyncFailNum);
    (void)ppm_printf("USB CFG Pseudo sync fail slice is 0x%x\n",  g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBCnfPseudoSyncFailSlice);

    (void)ppm_printf("The Port UDI Handle Err num is %d\n",                 g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBUdiHandleErr);
    (void)ppm_printf("The Port UDI Handle Get Buffer Err num is %d\n",      g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBUdiHandleReadGetBufferErr);
    (void)ppm_printf("The Port UDI Handle Comm Rcv Null Ptr num is %d\n",   g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBUdiCommRcvNullPtrErr);
    (void)ppm_printf("The Port UDI Handle Read Buffer Free Err num is %d\n",g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBUdiHandleReadBufferFreeErr);

    (void)ppm_printf("The Port UDI Handle Total Rcv Pkt num is %d\n",       g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBRcvPktNum);
    (void)ppm_printf("The Port UDI Handle Total Rcv Byte is %d.\n",          g_stAcpuDebugInfo.astPortInfo[enHandle].ulUSBRcvPktByte);

    return;
}


/*****************************************************************************
 �� �� ��  : PPM_OmPortDebugInfoShow
 ��������  : ���ڴ�ӡCNF\INDͨ����ͳ����Ϣ
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :
 �޸���ʷ  :
   1.��    ��  : 2015��11��2��
     ��    ��  : c00326366
     �޸�����  : Creat Function
*****************************************************************************/
void PPM_OmPortDebugInfoShow(void)
{
    (void)ppm_printf("IND ulOmDiscardNum %d, len %d; ulUSBSendErrNum %d, Len %d; ulUSBSendNum %d, len %d, reallen %d.\n",
    g_stAcpuDebugInfo.stIndDebugInfo.ulOmDiscardNum, g_stAcpuDebugInfo.stIndDebugInfo.ulOmDiscardLen,
    g_stAcpuDebugInfo.stIndDebugInfo.ulUSBSendErrNum, g_stAcpuDebugInfo.stIndDebugInfo.ulUSBSendErrLen,
    g_stAcpuDebugInfo.stIndDebugInfo.ulUSBSendNum, g_stAcpuDebugInfo.stIndDebugInfo.ulUSBSendLen,
    g_stAcpuDebugInfo.stIndDebugInfo.ulUSBSendRealLen);

    (void)ppm_printf("CNF ulOmDiscardNum %d, len %d; ulUSBSendErrNum %d, Len %d; ulUSBSendNum %d, len %d, Reallen %d.\n",
    g_stAcpuDebugInfo.stCnfDebugInfo.ulOmDiscardNum, g_stAcpuDebugInfo.stCnfDebugInfo.ulOmDiscardLen,
    g_stAcpuDebugInfo.stCnfDebugInfo.ulUSBSendErrNum, g_stAcpuDebugInfo.stCnfDebugInfo.ulUSBSendErrLen,
    g_stAcpuDebugInfo.stCnfDebugInfo.ulUSBSendNum, g_stAcpuDebugInfo.stCnfDebugInfo.ulUSBSendLen,
    g_stAcpuDebugInfo.stCnfDebugInfo.ulUSBSendRealLen);
}

/*****************************************************************************
 �� �� ��  : PPM_PortSwitchInfoShow
 ��������  : ���ڴ�ӡ�˿��л���ͳ����Ϣ
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :
 �޸���ʷ  :
   1.��    ��  : 2014��6��10��
     ��    ��  : h59254
     �޸�����  : Creat Function
*****************************************************************************/
void PPM_PortSwitchInfoShow(void)
{
    (void)ppm_printf("Port Type Err num is %d\n", g_stPpmPortSwitchInfo.ulPortTypeErr);

    (void)ppm_printf("Port Switch Fail time is %d\n", g_stPpmPortSwitchInfo.ulSwitchFail);

    (void)ppm_printf("Port Switch Success time is %d\n", g_stPpmPortSwitchInfo.ulSwitchSucc);

    (void)ppm_printf("Port Switch Start slice is 0x%x\n", g_stPpmPortSwitchInfo.ulStartSlice);

    (void)ppm_printf("Port Switch End slice is 0x%x.\n", g_stPpmPortSwitchInfo.ulEndSlice);

    return;
}







