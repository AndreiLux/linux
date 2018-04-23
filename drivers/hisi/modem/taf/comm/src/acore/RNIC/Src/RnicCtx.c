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
*****************************************************************************/
#include "RnicCtx.h"
#include "RnicEntity.h"
#include "PsCommonDef.h"
#include "AtRnicInterface.h"
#include "RnicProcMsg.h"
#include "RnicDemandDialFileIO.h"
#include "RnicLog.h"
#include "RnicDebug.h"




/*****************************************************************************
    Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_RNIC_CTX_C


/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/

/* RNIC CTX,���ڱ���RNIC��ȫ�ֱ��� */
RNIC_CTX_STRU                           g_stRnicCtx;

extern RNIC_STATS_INFO_STRU                    g_astRnicStats[RNIC_NET_ID_MAX_NUM];


/*****************************************************************************
 �� �� ��  : RNIC_RnicCtxInit
 ��������  : ��ʼ��RNIC��CTX
 �������  : pstRnicCtx:RNIC�������׵�ַ
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ     :
 1.��    ��   : 2011��12��06��
   ��    ��   : f00179208
   �޸�����   : �����ɺ���
 2.��    ��   : 2012��6��05��
   ��    ��   : z60575
   �޸�����   : DTS2012060508406������Ͽ����������޸�
 3.��    ��   : 2012��11��23��
   ��    ��   : f00179208
   �޸�����   : DSDA Phase I: RNIC��ʵ��
 3.��    ��   : 2013��04��15��
   ��    ��   : f00179208
   �޸�����   : C�˵�����λ��Ŀ:��ʼ����λ�ź���
 4.��    ��   : 2013��07��222��
   ��    ��   : j00177245
   �޸�����   : �������warning
 5.��    ��   : 2014��02��14��
   ��    ��   : m00217266
   �޸�����   : ��ʼ��L-C��������Ŀ�����ӵ�ȫ�ֱ���
 6.��    ��   : 2014��6��3��
   ��    ��   : m00217266
   �޸�����   : DTS2014052902453
 7.��    ��   : 2015��07��31��
   ��    ��   : w00316404
   �޸�����   : DTS2015071401227
 8.��    ��   : 2016��11��08��
   ��    ��   : l00373346
   �޸�����   : DTS2016062107764
 9.��    ��   : 2017��03��21��
   ��    ��   : l00373346
   �޸�����   : RNIC����NAP+GRO����
*****************************************************************************/
VOS_VOID RNIC_InitCtx(
    RNIC_CTX_STRU                      *pstRnicCtx
)
{
    VOS_UINT8                           ucIndex;

    TAF_MEM_SET_S(g_astRnicStats, sizeof(g_astRnicStats), 0x00, sizeof(g_astRnicStats));

    /* ��ʼ��RNIC�������ݵ�Linux����Э��ջAPIģʽ��Ϣ */
    RNIC_InitRnicNetInterfaceCfg(pstRnicCtx);

    for (ucIndex = 0 ; ucIndex < RNIC_NET_ID_MAX_NUM ; ucIndex++)
    {
        /* ��ʼ��RNIC����ͳ�� */
        RNIC_ClearNetDsFlowStats(ucIndex);

        /* ��ʼ��RNIC PDP������ */
        RNIC_InitPdpCtx(&(pstRnicCtx->astSpecCtx[ucIndex].stPdpCtx), ucIndex);

        /* ��ʼ������״̬ */
        pstRnicCtx->astSpecCtx[ucIndex].enFlowCtrlStatus = RNIC_FLOW_CTRL_STATUS_STOP;

        /* ��ʼ��ģ��ID */
        /* ��ʼ����ʱ����Ѿ�ȷ��������������Ӧ�ĸ�MODEM */
        pstRnicCtx->astSpecCtx[ucIndex].enModemId        = RNIC_GET_MODEM_ID_BY_NET_ID(ucIndex);

        pstRnicCtx->astSpecCtx[ucIndex].enRmNetId        = ucIndex;
        pstRnicCtx->astSpecCtx[ucIndex].lSpePort         = RNIC_INVALID_SPE_PORT;
        pstRnicCtx->astSpecCtx[ucIndex].ulIpfPortFlg     = VOS_FALSE;

        pstRnicCtx->astSpecCtx[ucIndex].enRatType        = IMSA_RNIC_IMS_RAT_TYPE_BUTT;

        if (VOS_NULL_PTR != pstRnicCtx->astSpecCtx[ucIndex].pstPriv)
        {
            pstRnicCtx->astSpecCtx[ucIndex].pstPriv->stNapi.weight = RNIC_GET_NAPI_WEIGHT();
        }

        /* ��ʼ��RNIC����Poll������� */
        IMM_ZcQueueHeadInit(RNIC_GET_PollBuff_QUE(ucIndex));
    }

    /* ��ʼ��RNIC��ʱ�������� */
    RNIC_InitAllTimers(pstRnicCtx->astTimerCtx);

    /* ��ʼ������ģʽ��Ϣ */
    RNIC_InitDialMode(&(pstRnicCtx->stDialMode));

    /* ��ʼ�����ŶϿ���ʱ����ʱ��������ͳ�� */
    RNIC_ClearTiDialDownExpCount();

    RNIC_SetTimer4WakeFlg(VOS_FALSE);

    /* ��ʼ����λ�ź��� */
    RNIC_InitResetSem();

    /* ��ʼ��IPFģʽ��Ϣ */
    RNIC_InitIpfMode(pstRnicCtx);

    return;
}

/*****************************************************************************
 �� �� ��  : RNIC_InitPdpCtx
 ��������  : ��ʼ��ָ������PDP������
 �������  : pstPdpCtx:PDP�������׵�ַ
             ucRmNetId:����ID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ     :
 1.��    ��   : 2011��12��12��
   ��    ��   : f00179208
   �޸�����   : �����ɺ���
 2.��    ��   : 2012��11��23��
   ��    ��   : f00179208
   �޸�����   : DSDA Phase I: RNIC��ʵ��
*****************************************************************************/
VOS_VOID RNIC_InitPdpCtx(
    RNIC_PDP_CTX_STRU                  *pstPdpCtx,
    VOS_UINT8                           ucRmNetId
)
{
    RNIC_InitIpv4PdpCtx(&(pstPdpCtx->stIpv4PdpInfo));
    RNIC_InitIpv6PdpCtx(&(pstPdpCtx->stIpv6PdpInfo));
    RNIC_InitIpv4v6PdpCtx(&(pstPdpCtx->stIpv4v6PdpInfo), ucRmNetId);

    IMM_ZcQueueHeadInit(&pstPdpCtx->stImsQue);

    return;
}

/*****************************************************************************
 �� �� ��  : RNIC_ResetDialMode
 ��������  : ���ò���ģʽ��Ϣ
 �������  : pstDialMode:����ģʽ�������׵�ַ
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ     :
 1.��    ��   : 2013��04��15��
   ��    ��   : f00179208
   �޸�����   : �����ɺ���
*****************************************************************************/
VOS_VOID RNIC_ResetDialMode(
    RNIC_DIAL_MODE_STRU                *pstDialMode
)
{
    pstDialMode->enDialMode             = RNIC_DIAL_MODE_MANUAL;
    pstDialMode->ulIdleTime             = RNIC_DIAL_DEMA_IDLE_TIME;
    pstDialMode->enEventReportFlag      = RNIC_FORBID_EVENT_REPORT;

    return;
}

/*****************************************************************************
 �� �� ��  : RNIC_InitDialMode
 ��������  : ��ʼ������ģʽ��Ϣ
 �������  : pstDialMode:����ģʽ�������׵�ַ
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ     :
 1.��    ��   : 2011��12��12��
   ��    ��   : f00179208
   �޸�����   : �����ɺ���
 2.��    ��   : 2012��1��31��
   ��    ��   : w00199382
   �޸�����   : ��ʼ��/proc/dial�µ��ļ�
*****************************************************************************/
VOS_VOID RNIC_InitDialMode(
    RNIC_DIAL_MODE_STRU                *pstDialMode
)
{
    VOS_UINT32                          ulRlst;

    pstDialMode->enDialMode             = RNIC_DIAL_MODE_MANUAL;
    pstDialMode->ulIdleTime             = RNIC_DIAL_DEMA_IDLE_TIME;
    pstDialMode->enEventReportFlag      = RNIC_FORBID_EVENT_REPORT;

    ulRlst                              = RNIC_InitDemandDialFile();

    if ( VOS_ERR == ulRlst)
    {
       RNIC_ERROR_LOG(ACPU_PID_RNIC, "RNIC_InitDialMode:RNIC_InitDemandDialFile ERR!");
    }

    return;
}

/*****************************************************************************
 �� �� ��  : RNIC_InitIpv4PdpCtx
 ��������  : ��ʼ��IPV4 PDP������
 �������  : pstIpv4PdpCtx:IPV4 PDP�������׵�ַ
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ     :
 1.��    ��   : 2011��12��19��
   ��    ��   : f00179208
   �޸�����   : �����ɺ���

*****************************************************************************/
VOS_VOID RNIC_InitIpv4PdpCtx(
    RNIC_IPV4_PDP_INFO_STRU            *pstIpv4PdpCtx
)
{
    pstIpv4PdpCtx->enRegStatus = RNIC_PDP_REG_STATUS_DEACTIVE;
    /* Modified by l60609 for L-C��������Ŀ, 2014-1-14, begin */
    pstIpv4PdpCtx->ucRabId     = RNIC_RAB_ID_INVALID;
    /* Modified by l60609 for L-C��������Ŀ, 2014-1-14, end */
    pstIpv4PdpCtx->ulIpv4Addr  = 0;


    return;
}

/*****************************************************************************
 �� �� ��  : RNIC_InitIpv6PdpCtx
 ��������  : ��ʼ��IPV6 PDP������
 �������  : pstIpv4PdpCtx:IPV6 PDP�������׵�ַ
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ     :
 1.��    ��   : 2011��12��19��
   ��    ��   : f00179208
   �޸�����   : �����ɺ���

*****************************************************************************/
VOS_VOID RNIC_InitIpv6PdpCtx(
    RNIC_IPV6_PDP_INFO_STRU            *pstIpv6PdpCtx
)
{
    pstIpv6PdpCtx->enRegStatus = RNIC_PDP_REG_STATUS_DEACTIVE;
    /* Modified by l60609 for L-C��������Ŀ, 2014-1-14, begin */
    pstIpv6PdpCtx->ucRabId     = RNIC_RAB_ID_INVALID;
    /* Modified by l60609 for L-C��������Ŀ, 2014-1-14, end */

    TAF_MEM_SET_S(pstIpv6PdpCtx->aucIpv6Addr,
               sizeof(pstIpv6PdpCtx->aucIpv6Addr),
               0x00,
               RNIC_MAX_IPV6_ADDR_LEN);


    return;
}

/*****************************************************************************
 �� �� ��  : RNIC_InitIpv4v6PdpCtx
 ��������  : ��ʼ��ָ������IPV4V6 PDP������
 �������  : pstIpv4v6PdpCtx:IPV4V6 PDP�������׵�ַ
             ucRmNetId:����ID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ     :
 1.��    ��   : 2011��12��19��
   ��    ��   : f00179208
   �޸�����   : �����ɺ���
 2.��    ��   : 2012��11��23��
   ��    ��   : f00179208
   �޸�����   : DSDA Phase I: RNIC��ʵ��
*****************************************************************************/
VOS_VOID RNIC_InitIpv4v6PdpCtx(
    RNIC_IPV4V6_PDP_INFO_STRU          *pstIpv4v6PdpCtx,
    VOS_UINT8                           ucRmNetId
)
{
    RNIC_PDP_CTX_STRU                  *pstPdpAddr;

    /* ��ȡPDP�����ĵ�ַ */
    pstPdpAddr                              = RNIC_GetPdpCtxAddr(ucRmNetId);

    /* Modified by l60609 for L-C��������Ŀ, 2014-1-14, begin */
    pstIpv4v6PdpCtx->enRegStatus            = RNIC_PDP_REG_STATUS_DEACTIVE;
    pstIpv4v6PdpCtx->ucRabId                = RNIC_RAB_ID_INVALID;
    /* Modified by l60609 for L-C��������Ŀ, 2014-1-14, end */

    pstPdpAddr->stIpv4PdpInfo.ucRabId       = RNIC_RAB_ID_INVALID;
    pstPdpAddr->stIpv6PdpInfo.ucRabId       = RNIC_RAB_ID_INVALID;

    pstIpv4v6PdpCtx->ulIpv4Addr             = 0;

    TAF_MEM_SET_S(pstIpv4v6PdpCtx->aucIpv6Addr,
               sizeof(pstIpv4v6PdpCtx->aucIpv6Addr),
               0x00,
               RNIC_MAX_IPV6_ADDR_LEN);


}

/*****************************************************************************
 �� �� ��  : RNIC_InitResetSem
 ��������  : ��ʼ��RNIC��λ���ź���
 �������  : ��
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ     :
 1.��    ��   : 2013��04��15��
   ��    ��   : f00179208
   �޸�����   : �����ɺ���
*****************************************************************************/
VOS_VOID RNIC_InitResetSem(VOS_VOID)
{
    g_stRnicCtx.hResetSem  = VOS_NULL_PTR;

    /* ����������ź��� */
    if (VOS_OK != VOS_SmBCreate( "RNIC", 0, VOS_SEMA4_FIFO, &g_stRnicCtx.hResetSem))
    {
        PS_PRINTF("Create rnic acpu cnf sem failed!\r\n");
        RNIC_DBG_SET_SEM_INIT_FLAG(VOS_FALSE);
        RNIC_DBG_CREATE_BINARY_SEM_FAIL_NUM(1);

        return;
    }
    else
    {
        RNIC_DBG_SAVE_BINARY_SEM_ID(g_stRnicCtx.hResetSem);
    }

    RNIC_DBG_SET_SEM_INIT_FLAG(VOS_TRUE);

    return;
}

/*****************************************************************************
 �� �� ��  : RNIC_InitIpfMode
 ��������  : ��ʼ��IPFģʽ
 �������  : *pstRnicCtx --- RNC������
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ     :
 1.��    ��   : 201��11��08��
   ��    ��   : l00373346
   �޸�����   : �����ɺ���
*****************************************************************************/
VOS_VOID RNIC_InitIpfMode(
    RNIC_CTX_STRU                      *pstRnicCtx
)
{
    TAF_NV_ADS_IPF_MODE_CFG_STRU        stIpfMode;

    TAF_MEM_SET_S(&stIpfMode, (VOS_SIZE_T)sizeof(stIpfMode), 0x00, (VOS_SIZE_T)sizeof(TAF_NV_ADS_IPF_MODE_CFG_STRU));

    pstRnicCtx->ucIpfMode = 0;

    /* ��ȡNV�� */
    if (NV_OK == NV_ReadEx(MODEM_ID_0, en_NV_Item_ADS_IPF_MODE_CFG,
                      &stIpfMode, (VOS_UINT32)sizeof(TAF_NV_ADS_IPF_MODE_CFG_STRU)))
    {
        pstRnicCtx->ucIpfMode = stIpfMode.ucIpfMode;
    }

    return;
}

/*****************************************************************************
 �� �� ��  : RNIC_CheckNetIfCfgValid
 ��������  : �������Э��ջ�ӿ�������Ч��
 �������  : *pstRnicCtx --- RNC������
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ     :
 1.��    ��   : 2017��03��10��
   ��    ��   : l00373346
   �޸�����   : �����ɺ���
*****************************************************************************/
VOS_VOID RNIC_CheckNetIfCfgValid(
    RNIC_CTX_STRU                      *pstRnicCtx
)
{
    if (pstRnicCtx->stRnicNetIfCfg.ucNetInterfaceMode >= RNIC_NET_IF_BUTT)
    {
        pstRnicCtx->stRnicNetIfCfg.ucNetInterfaceMode = RNIC_NET_IF_NETRX;
    }

    if ((pstRnicCtx->stRnicNetIfCfg.ucNapiPollWeight > RNIC_NAPI_POLL_MAX_WEIGHT) ||
        (0 == pstRnicCtx->stRnicNetIfCfg.ucNapiPollWeight))
    {
        pstRnicCtx->stRnicNetIfCfg.ucNapiPollWeight   = RNIC_NAPI_POLL_DEFAULT_WEIGHT;
    }

    if ((pstRnicCtx->stRnicNetIfCfg.usNapiPollQueMaxLen > RNIC_POLL_QUEUE_DEFAULT_MAX_LEN) ||
        (0 == pstRnicCtx->stRnicNetIfCfg.usNapiPollQueMaxLen))
    {
        pstRnicCtx->stRnicNetIfCfg.usNapiPollQueMaxLen   = RNIC_POLL_QUEUE_DEFAULT_MAX_LEN;
    }

    if (pstRnicCtx->stRnicNetIfCfg.enNapiWeightAdjMode >= NAPI_WEIGHT_ADJ_MODE_BUTT)
    {
        pstRnicCtx->stRnicNetIfCfg.enNapiWeightAdjMode = NAPI_WEIGHT_ADJ_STATIC_MODE;
    }

    return;
}
/*****************************************************************************
 �� �� ��  : RNIC_InitRnicNetInterfaceMode
 ��������  : ��ʼ��RNIC�������ݵ�Linux����Э��ջ�Ľӿ�ģʽ
 �������  : *pstRnicCtx --- RNC������
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ     :
 1.��    ��   : 2017��03��10��
   ��    ��   : l00373346
   �޸�����   : �����ɺ���
*****************************************************************************/
VOS_VOID RNIC_InitRnicNetInterfaceCfg(
    RNIC_CTX_STRU                      *pstRnicCtx
)
{
    TAF_NV_RNIC_NET_IF_CFG_STRU        stRnicNetIfCfg;

    TAF_MEM_SET_S(&stRnicNetIfCfg, (VOS_SIZE_T)sizeof(stRnicNetIfCfg), 0x00, (VOS_SIZE_T)sizeof(TAF_NV_RNIC_NET_IF_CFG_STRU));

    /* ��ȡNV�� */
    if (NV_OK == NV_ReadEx(MODEM_ID_0, en_NV_Item_Rnic_Net_If_Cfg,
                      &stRnicNetIfCfg, (VOS_UINT32)sizeof(TAF_NV_RNIC_NET_IF_CFG_STRU)))
    {
        pstRnicCtx->stRnicNetIfCfg.ucNetInterfaceMode  = stRnicNetIfCfg.ucNetInterfaceMode;
        pstRnicCtx->stRnicNetIfCfg.enNapiWeightAdjMode = stRnicNetIfCfg.enNapiWeightAdjMode;
        pstRnicCtx->stRnicNetIfCfg.ucNapiPollWeight    = stRnicNetIfCfg.ucNapiPollWeight;
        pstRnicCtx->stRnicNetIfCfg.usNapiPollQueMaxLen = stRnicNetIfCfg.usNapiPollQueMaxLen;

        pstRnicCtx->stRnicNetIfCfg.stNapiWeightDynamicAdjCfg.stDlPktNumPerSecLevel.ulDlPktNumPerSecLevel1
                  = stRnicNetIfCfg.stNapiWeightDynamicAdjCfg.stDlPktNumPerSecLevel.ulDlPktNumPerSecLevel1;
        pstRnicCtx->stRnicNetIfCfg.stNapiWeightDynamicAdjCfg.stDlPktNumPerSecLevel.ulDlPktNumPerSecLevel2
                  = stRnicNetIfCfg.stNapiWeightDynamicAdjCfg.stDlPktNumPerSecLevel.ulDlPktNumPerSecLevel2;
        pstRnicCtx->stRnicNetIfCfg.stNapiWeightDynamicAdjCfg.stDlPktNumPerSecLevel.ulDlPktNumPerSecLevel3
                  = stRnicNetIfCfg.stNapiWeightDynamicAdjCfg.stDlPktNumPerSecLevel.ulDlPktNumPerSecLevel3;
        pstRnicCtx->stRnicNetIfCfg.stNapiWeightDynamicAdjCfg.stDlPktNumPerSecLevel.ulDlPktNumPerSecLevel4
                  = stRnicNetIfCfg.stNapiWeightDynamicAdjCfg.stDlPktNumPerSecLevel.ulDlPktNumPerSecLevel4;

        pstRnicCtx->stRnicNetIfCfg.stNapiWeightDynamicAdjCfg.stNapiWeightLevel.ucNapiWeightLevel1
                  = stRnicNetIfCfg.stNapiWeightDynamicAdjCfg.stNapiWeightLevel.ucNapiWeightLevel1;
        pstRnicCtx->stRnicNetIfCfg.stNapiWeightDynamicAdjCfg.stNapiWeightLevel.ucNapiWeightLevel2
                  = stRnicNetIfCfg.stNapiWeightDynamicAdjCfg.stNapiWeightLevel.ucNapiWeightLevel2;
        pstRnicCtx->stRnicNetIfCfg.stNapiWeightDynamicAdjCfg.stNapiWeightLevel.ucNapiWeightLevel3
                  = stRnicNetIfCfg.stNapiWeightDynamicAdjCfg.stNapiWeightLevel.ucNapiWeightLevel3;
        pstRnicCtx->stRnicNetIfCfg.stNapiWeightDynamicAdjCfg.stNapiWeightLevel.ucNapiWeightLevel4
                  = stRnicNetIfCfg.stNapiWeightDynamicAdjCfg.stNapiWeightLevel.ucNapiWeightLevel4;

    }
    else
    {
        pstRnicCtx->stRnicNetIfCfg.ucNetInterfaceMode     = RNIC_NET_IF_NETRX;
        pstRnicCtx->stRnicNetIfCfg.enNapiWeightAdjMode    = NAPI_WEIGHT_ADJ_STATIC_MODE;
        pstRnicCtx->stRnicNetIfCfg.ucNapiPollWeight       = RNIC_NAPI_POLL_DEFAULT_WEIGHT;
        pstRnicCtx->stRnicNetIfCfg.usNapiPollQueMaxLen    = RNIC_POLL_QUEUE_DEFAULT_MAX_LEN;
    }

    RNIC_CheckNetIfCfgValid(pstRnicCtx);

    return;
}

/*****************************************************************************
 �� �� ��  : RNIC_ClearNetDsFlowStats
 ��������  : ��ʼ������ͳ����Ϣ
 �������  : enRmNetId --- RMNET ID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��3��30��
    ��    ��   : A00165503
    �޸�����   : �����ɺ���
*****************************************************************************/
VOS_VOID RNIC_ClearNetDsFlowStats(RNIC_RMNET_ID_ENUM_UINT8 enRmNetId)
{
    RNIC_SPEC_CTX_STRU                 *pstNetCntxt = VOS_NULL_PTR;

    pstNetCntxt = RNIC_GET_SPEC_NET_CTX(enRmNetId);

    pstNetCntxt->stDsFlowStats.ulCurrentRecvRate   = 0;
    pstNetCntxt->stDsFlowStats.ulPeriodRecvPktNum  = 0;
    pstNetCntxt->stDsFlowStats.ulTotalRecvFluxLow  = 0;
    pstNetCntxt->stDsFlowStats.ulTotalRecvFluxHigh = 0;

    pstNetCntxt->stDsFlowStats.ulCurrentSendRate   = 0;
    pstNetCntxt->stDsFlowStats.ulPeriodSendPktNum  = 0;
    pstNetCntxt->stDsFlowStats.ulTotalSendFluxLow  = 0;
    pstNetCntxt->stDsFlowStats.ulTotalSendFluxHigh = 0;

    return;
}

/*****************************************************************************
 �� �� ��  : RNIC_GetRnicCtxAddr
 ��������  : ��ȡ��ǰRNIC��CTX
 �������  : ��
 �������  : ��
 �� �� ֵ  : ���ص�ǰRNIC��CTX��ַ
 ���ú���  :
 ��������  :

 �޸���ʷ     :
 1.��    ��   : 2011��12��06��
   ��    ��   : f00179208
   �޸�����   : �����ɺ���

*****************************************************************************/
RNIC_CTX_STRU* RNIC_GetRnicCtxAddr(VOS_VOID)
{
    return &(g_stRnicCtx);
}

/*****************************************************************************
 �� �� ��  : RNIC_GetRnicTiDialDownExpCount
 ��������  : ��ȡ����ģʽͳ�Ʋ��ŶϿ�������
 �������  : ��
 �������  : ��
 �� �� ֵ  : ����ģʽͳ�Ʋ��ŶϿ�������
 ���ú���  :
 ��������  :

 �޸���ʷ     :
 1.��    ��   : 2011��12��07��
   ��    ��   : f00179208
   �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 RNIC_GetTiDialDownExpCount( VOS_VOID)
{
    return (g_stRnicCtx.ulTiDialDownExpCount);
}


/*****************************************************************************
 �� �� ��  : RNIC_TiDialDownExpCountPlusOne
 ��������  : �ۼӲ���ģʽͳ�Ʋ��ŶϿ�������
 �������  : ��
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ     :
 1.��    ��   : 2011��12��07��
   ��    ��   : f00179208
   �޸�����   : �����ɺ���

*****************************************************************************/
VOS_VOID RNIC_IncTiDialDownExpCount( VOS_VOID)
{
    (g_stRnicCtx.ulTiDialDownExpCount)++;

    return;
}

/*****************************************************************************
 �� �� ��  : RNIC_ClearRnicTiDialDownExpCount
 ��������  : ����ģʽͳ�Ʋ��ŶϿ�����������
 �������  : ��
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ     :
 1.��    ��   : 2011��12��07��
   ��    ��   : f00179208
   �޸�����   : �����ɺ���

*****************************************************************************/
VOS_VOID RNIC_ClearTiDialDownExpCount( VOS_VOID)
{
    g_stRnicCtx.ulTiDialDownExpCount = 0;

    return;
}

/*****************************************************************************
 �� �� ��  : RNIC_GetCurrentUlRate
 ��������  : ��ȡָ����������������
 �������  : ucRmNetId: ����ID
 �������  : ��
 �� �� ֵ  : ��ǰRNIC���ʵĵ�ַ
 ���ú���  :
 ��������  :

 �޸���ʷ     :
 1.��    ��   : 2011��12��07��
   ��    ��   : f00179208
   �޸�����   : �����ɺ���
 2.��    ��   : 2012��11��23��
   ��    ��   : f00179208
   �޸�����   : DSDA Phase I: RNIC��ʵ��
*****************************************************************************/
VOS_UINT32 RNIC_GetCurrentUlRate(VOS_UINT8 ucRmNetId)
{
    return (g_stRnicCtx.astSpecCtx[ucRmNetId].stDsFlowStats.ulCurrentSendRate);
}

/*****************************************************************************
 �� �� ��  : RNIC_SetCurrentUlRate
 ��������  : ����ָ����������������
 �������  : ulULDataRate:��������
             ucRmNetId: ����ID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ     :
 1.��    ��   : 2011��12��13��
   ��    ��   : f00179208
   �޸�����   : �����ɺ���
 2.��    ��   : 2012��11��23��
   ��    ��   : f00179208
   �޸�����   : DSDA Phase I: RNIC��ʵ��
*****************************************************************************/
VOS_VOID RNIC_SetCurrentUlRate(
    VOS_UINT32                          ulULDataRate,
    VOS_UINT8                           ucRmNetId
)
{
    g_stRnicCtx.astSpecCtx[ucRmNetId].stDsFlowStats.ulCurrentSendRate = ulULDataRate;

    return;
}


/*****************************************************************************
 �� �� ��  : RNIC_GetCurrentDlRate
 ��������  : ��ȡָ����������������
 �������  : ucRmNetId: ����ID
 �������  : ��
 �� �� ֵ  : ��ǰRNIC���ʵĵ�ַ
 ���ú���  :
 ��������  :

 �޸���ʷ     :
 1.��    ��   : 2011��12��07��
   ��    ��   : f00179208
   �޸�����   : �����ɺ���
 2.��    ��   : 2012��11��23��
   ��    ��   : f00179208
   �޸�����   : DSDA Phase I: RNIC��ʵ��
*****************************************************************************/
VOS_UINT32 RNIC_GetCurrentDlRate(VOS_UINT8 ucRmNetId)
{
    return (g_stRnicCtx.astSpecCtx[ucRmNetId].stDsFlowStats.ulCurrentRecvRate);
}

/*****************************************************************************
 �� �� ��  : RNIC_SetCurrentDlRate
 ��������  : ����ָ����������������
 �������  : ulDLDataRate:��������
             ucRmNetId: ����ID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ     :
 1.��    ��   : 2011��12��07��
   ��    ��   : f00179208
   �޸�����   : �����ɺ���
 2.��    ��   : 2012��11��23��
   ��    ��   : f00179208
   �޸�����   : DSDA Phase I: RNIC��ʵ��
*****************************************************************************/
VOS_VOID RNIC_SetCurrentDlRate(
    VOS_UINT32                          ulDLDataRate,
    VOS_UINT8                           ucRmNetId
)
{
    g_stRnicCtx.astSpecCtx[ucRmNetId].stDsFlowStats.ulCurrentRecvRate = ulDLDataRate;

    return;
}

/*****************************************************************************
 �� �� ��  : RNIC_GetDialModeAddr
 ��������  : ��ȡ���貦�ŵ�ģʽ�Լ�ʱ���ĵ�ַ
 �������  : ��
 �������  : ��
 �� �� ֵ  : ���貦��ģʽ��ʱ���ĵ�ַ
 ���ú���  :
 ��������  :

 �޸���ʷ     :
 1.��    ��   : 2011��12��07��
   ��    ��   : f00179208
   �޸�����   : �����ɺ���
*****************************************************************************/
RNIC_DIAL_MODE_STRU* RNIC_GetDialModeAddr(VOS_VOID)
{
    return &(g_stRnicCtx.stDialMode);
}

/*****************************************************************************
 �� �� ��  : RNIC_GetPdpCtxAddr
 ��������  : ��ȡRNICģ��ָ������PDP�����ĵĵ�ַ
 �������  : ucRmNetId -- ��������
 �������  : ��
 �� �� ֵ  : RNICģ��ָ������PDP�����ĵĵ�ַ
 ���ú���  :
 ��������  :

 �޸���ʷ     :
 1.��    ��   : 2011��12��07��
   ��    ��   : f00179208
   �޸�����   : �����ɺ���
 2.��    ��   : 2012��11��23��
   ��    ��   : f00179208
   �޸�����   : DSDA Phase I: RNIC��ʵ��
*****************************************************************************/
RNIC_PDP_CTX_STRU* RNIC_GetPdpCtxAddr(VOS_UINT8 ucRmNetId)
{
    return &(g_stRnicCtx.astSpecCtx[ucRmNetId].stPdpCtx);
}

/*****************************************************************************
 �� �� ��  : RNIC_GetTimerAddr
 ��������  : ��ȡRNIC CTXģ���ж�ʱ����������
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��ʱ�������ĵ�ַ
 ���ú���  :
 ��������  :

 �޸���ʷ     :
 1.��    ��   : 2011��12��12��
   ��    ��   : f00179208
   �޸�����   : �����ɺ���

*****************************************************************************/
RNIC_TIMER_CTX_STRU*  RNIC_GetTimerAddr( VOS_VOID )
{
    return g_stRnicCtx.astTimerCtx;
}

/*****************************************************************************
 �� �� ��  : RNIC_GetTimer4WakeFlg
 ��������  : ��ȡRNIC CTXģ�����Ƿ�����Timer4���ѱ�־
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��ʱ�������ĵ�ַ
 ���ú���  :
 ��������  :

 �޸���ʷ     :
 1.��    ��   : 2012��6��5��
   ��    ��   : z60575
   �޸�����   : DTS2012060508406, �����ɺ���

*****************************************************************************/
VOS_UINT32 RNIC_GetTimer4WakeFlg(VOS_VOID)
{
    return g_stRnicCtx.ulSetTimer4WakeFlg;
}

/*****************************************************************************
 �� �� ��  : RNIC_SetTimer4WakeFlg
 ��������  : ����RNIC CTXģ�����Ƿ�����Timer4���ѱ�־
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��ʱ�������ĵ�ַ
 ���ú���  :
 ��������  :

 �޸���ʷ     :
 1.��    ��   : 2012��6��5��
   ��    ��   : z60575
   �޸�����   : DTS2012060508406, �����ɺ���

*****************************************************************************/
VOS_VOID RNIC_SetTimer4WakeFlg(VOS_UINT32 ulFlg)
{
    g_stRnicCtx.ulSetTimer4WakeFlg = ulFlg;
}

/*****************************************************************************
 �� �� ��  : RNIC_GetSpecNetCardCtxAddr
 ��������  : ��ȡRNICָ��������������
 �������  : ucIndex -- ��������
 �������  : ��
 �� �� ֵ  : RNICָ�������������ĵ�ַ
 ���ú���  :
 ��������  :

 �޸���ʷ     :
 1.��    ��   : 2012��11��24��
   ��    ��   : f00179208
   �޸�����   : �����ɺ���
*****************************************************************************/
RNIC_SPEC_CTX_STRU *RNIC_GetSpecNetCardCtxAddr(VOS_UINT8 ucRmNetId)
{
    return &(g_stRnicCtx.astSpecCtx[ucRmNetId]);
}

/*****************************************************************************
 �� �� ��  : RNIC_GetNetCntxtByRmNetId
 ��������  : ����RMNETID��ȡ����������ָ��
 �������  : enRmNetId --- RMNET ID
 �������  : ��
 �� �� ֵ  : RNIC_SPEC_CTX_STRU*
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��3��30��
    ��    ��   : A00165503
    �޸�����   : �����ɺ���
*****************************************************************************/
RNIC_SPEC_CTX_STRU* RNIC_GetNetCntxtByRmNetId(RNIC_RMNET_ID_ENUM_UINT8 enRmNetId)
{
    if (enRmNetId >= RNIC_RMNET_ID_BUTT)
    {
        return VOS_NULL_PTR;
    }

    return RNIC_GET_SPEC_NET_CTX(enRmNetId);
}


/*****************************************************************************
 �� �� ��  : RNIC_GetResetSem
 ��������  : ��ȡRNIC��λ�ź���
 �������  : ��
 �������  : ��
 �� �� ֵ  : RNIC��λ�ź���
 ���ú���  :
 ��������  :

 �޸���ʷ     :
 1.��    ��   : 2013��04��15��
   ��    ��   : f00179208
   �޸�����   : �����ɺ���
*****************************************************************************/
VOS_SEM RNIC_GetResetSem(VOS_VOID)
{
    return g_stRnicCtx.hResetSem;
}



