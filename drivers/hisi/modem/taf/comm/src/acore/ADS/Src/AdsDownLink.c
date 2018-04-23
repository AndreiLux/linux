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
#include "AdsDownLink.h"
#include "AdsDebug.h"
#include "mdrv.h"
#include "AcpuReset.h"
#include "AdsFilter.h"
#include "AdsDhcpInterface.h"
#include "AdsMntn.h"
/* Added by Y00213812 for Spirnt ����, 2017-4-2, begin */
#include "ads_mip_pif.h"

/* Added by Y00213812 for Spirnt ����, 2017-4-2, end */


/*****************************************************************************
    Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
*****************************************************************************/
#define    THIS_FILE_ID                 PS_FILE_ID_ADS_DOWNLINK_C


/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/
VOS_UINT32                              g_ulAdsDlDiscardPktFlag  = VOS_FALSE;   /* ADS����ֱ�Ӷ������� */

/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/

/*****************************************************************************
 �� �� ��  : ADS_DL_CCpuResetCallback
 ��������  : C�˵�����λʱADS DL�Ļص�������
 �������  : enParam   -- 0��ʾ��λǰ�� ������ʾ��λ��
             lUserData -- �û�����
 �������  : ��
 �� �� ֵ  : VOS_INT
 ���ú���  :
 ��������  :

 �޸���ʷ     :
 1.��    ��   : 2013��04��10��
   ��    ��   : f00179208
   �޸�����   : �����ɺ���
*****************************************************************************/
VOS_INT ADS_DL_CCpuResetCallback(
    DRV_RESET_CB_MOMENT_E               enParam,
    VOS_INT                             lUserData
)
{
    ADS_CCPU_RESET_IND_STRU                *pstMsg = VOS_NULL_PTR;

    /* ����Ϊ0��ʾ��λǰ���� */
    if (MDRV_RESET_CB_BEFORE == enParam)
    {
        ADS_TRACE_HIGH("before reset enter.\n");

        /* ������Ϣ */
        pstMsg = (ADS_CCPU_RESET_IND_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(ACPU_PID_ADS_DL,
                                                                        sizeof(ADS_CCPU_RESET_IND_STRU));
        if (VOS_NULL_PTR == pstMsg)
        {
            ADS_TRACE_HIGH("before reset: alloc msg failed.\n");
            return VOS_ERROR;
        }

        /* ��д��Ϣͷ */
        pstMsg->ulReceiverPid               = ACPU_PID_ADS_DL;
        pstMsg->enMsgId                     = ID_ADS_CCPU_RESET_START_IND;

        /* ����Ϣ */
        if (VOS_OK != PS_SEND_MSG(ACPU_PID_ADS_DL, pstMsg))
        {
            ADS_TRACE_HIGH("before reset: send msg failed.\n");
            return VOS_ERROR;
        }

        /* �ȴ��ظ��ź�����ʼΪ��״̬���ȴ���Ϣ��������ź��������� */
        if (VOS_OK != VOS_SmP(ADS_GetDLResetSem(), ADS_RESET_TIMEOUT_LEN))
        {
            ADS_TRACE_HIGH("before reset VOS_SmP failed.\n");
            ADS_DBG_DL_RESET_LOCK_FAIL_NUM(1);
            return VOS_ERROR;
        }

        ADS_TRACE_HIGH("before reset succ.\n");
        return VOS_OK;
    }
    /* ��λ�� */
    else if (MDRV_RESET_CB_AFTER == enParam)
    {
        ADS_TRACE_HIGH("after reset enter.\n");

        /* ������Ϣ */
        pstMsg = (ADS_CCPU_RESET_IND_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(ACPU_PID_ADS_DL,
                                                                        sizeof(ADS_CCPU_RESET_IND_STRU));
        if (VOS_NULL_PTR == pstMsg)
        {
            ADS_TRACE_HIGH("after reset: alloc msg failed.\n");
            return VOS_ERROR;
        }

        /* ��д��Ϣͷ */
        pstMsg->ulReceiverPid               = ACPU_PID_ADS_DL;
        pstMsg->enMsgId                     = ID_ADS_CCPU_RESET_END_IND;

        /* ����Ϣ */
        if (VOS_OK != PS_SEND_MSG(ACPU_PID_ADS_DL, pstMsg))
        {
            ADS_TRACE_HIGH("after reset: send msg failed.\n");
            return VOS_ERROR;
        }

        ADS_TRACE_HIGH("after reset: succ.\n");
        ADS_DBG_DL_RESET_SUCC_NUM(1);
        return VOS_OK;
    }
    else
    {
        return VOS_ERROR;
    }
}

/*****************************************************************************
 �� �� ��  : ADS_DL_IpfIntCB
 ��������  : ADS�������ݴ���ģ����IPFע����жϻص�����
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : VOS_INT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��12��7��
    ��    ��   : ³��/l60609
    �޸�����   : �����ɺ���

  2.��    ��   : 2012��4��12��
    ��    ��   : A00165503
    �޸�����   : DTS2012020708222: ADS�����Ż�
*****************************************************************************/
VOS_INT32 ADS_DL_IpfIntCB(VOS_VOID)
{
    if (ADS_IPF_MODE_INT == ADS_GET_IPF_MODE())
    {
        ADS_DL_WakeLock();
        ADS_DL_ProcIpfResult();
        ADS_DL_WakeUnLock();
    }
    else
    {
        ADS_DL_SndEvent(ADS_DL_EVENT_IPF_RD_INT);
    }
    ADS_DBG_DL_RCV_IPF_RD_INT_NUM(1);

    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : ADS_DL_IpfAdqEmptyCB
 ��������  : ADS��IPFע�������ADQ���жϴ�����
 �������  : IPF_ADQ_EMPTY_E eAdqEmpty
 �������  : ��
 �� �� ֵ  : VOS_INT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��12��19��
    ��    ��   : l60609
    �޸�����   : �����ɺ���
*****************************************************************************/
VOS_INT32 ADS_DL_IpfAdqEmptyCB(IPF_ADQ_EMPTY_E eAdqEmpty)
{
    if (IPF_EMPTY_ADQ0 == eAdqEmpty)
    {
        ADS_DBG_DL_ADQ_RCV_AD0_EMPTY_INT_NUM(1);
    }
    else if (IPF_EMPTY_ADQ1 == eAdqEmpty)
    {
        ADS_DBG_DL_ADQ_RCV_AD1_EMPTY_INT_NUM(1);
    }
    else
    {
        ADS_DBG_DL_ADQ_RCV_AD0_EMPTY_INT_NUM(1);
        ADS_DBG_DL_ADQ_RCV_AD1_EMPTY_INT_NUM(1);
    }

    ADS_DL_SndEvent(ADS_DL_EVENT_IPF_ADQ_EMPTY_INT);
    ADS_DBG_DL_RCV_IPF_ADQ_EMPTY_INT_NUM(1);
    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : ADS_DL_RcvTiAdqEmptyExpired
 ��������  : TI_ADS_DL_ADQ_EMPTY��ʱ������
 �������  : VOS_UINT32                          ulParam,
             VOS_UINT32                          ulTimerName
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��12��19��
    ��    ��   : l60609
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_VOID ADS_DL_RcvTiAdqEmptyExpired(
    VOS_UINT32                          ulParam,
    VOS_UINT32                          ulTimerName
)
{
    /* ��������ADQ���жϴ����¼� */
    ADS_DL_SndEvent(ADS_DL_EVENT_IPF_ADQ_EMPTY_INT);
    ADS_DBG_DL_ADQ_EMPTY_TMR_TIMEOUT_NUM(1);
    return;
}

/*****************************************************************************
 �� �� ��  : ADS_DL_RcvTiCcoreResetDelayExpired
 ��������  : TI_ADS_DL_CCORE_RESET_DELAY��ʱ������
 �������  : VOS_UINT32                          ulParam,
             VOS_UINT32                          ulTimerName
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2017��07��10��
    ��    ��   : l00373346
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_VOID ADS_DL_RcvTiCcoreResetDelayExpired(
    VOS_UINT32                          ulParam,
    VOS_UINT32                          ulTimerName
)
{
    ADS_DL_RcvCcpuResetStartInd();
    return;
}

/*****************************************************************************
 �� �� ��  : ADS_DL_ConfigAdq
 ��������  : ����AD
 �������  : IPF_AD_TYPE_E                       enAdType
             VOS_UINT                            ulIpfAdNum
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��12��21��
    ��    ��   : l60609
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 ADS_DL_ConfigAdq(
    IPF_AD_TYPE_E                       enAdType,
    VOS_UINT                            ulIpfAdNum
)
{
    IPF_AD_DESC_S                      *pstAdDesc = VOS_NULL_PTR;
    IMM_ZC_STRU                        *pstImmZc  = VOS_NULL_PTR;
    VOS_UINT32                          ulPoolId;
    VOS_UINT32                          ulTmp;
    VOS_UINT32                          ulCnt;
    VOS_UINT32                          ulLen;
    VOS_INT32                           lRslt;

    if (IPF_AD_0 == enAdType)
    {
        ulPoolId = ADS_IPF_MEM_POOL_ID_AD0;
        ulLen    = ADS_IPF_AD0_MEM_BLK_SIZE;
    }
    else
    {
        ulPoolId = ADS_IPF_MEM_POOL_ID_AD1;
        ulLen    = ADS_IPF_AD1_MEM_BLK_SIZE;
    }

    for (ulCnt = 0; ulCnt < ulIpfAdNum; ulCnt++)
    {
        pstImmZc = ADS_IPF_AllocMem(ulPoolId, ulLen, IMM_MAC_HEADER_RES_LEN);
        if (VOS_NULL_PTR == pstImmZc)
        {
            ADS_DBG_DL_ADQ_ALLOC_MEM_FAIL_NUM(1);
            break;
        }

        ADS_DBG_DL_ADQ_ALLOC_MEM_SUCC_NUM(1);

        /* ��дAD������: OUTPUT0 ---> Ŀ�ĵ�ַ; OUTPUT1 ---> SKBUFF */
        pstAdDesc = ADS_DL_GET_IPF_AD_DESC_PTR(enAdType, ulCnt);
        pstAdDesc->OutPtr0 = (modem_phy_addr)virt_to_phys((VOS_VOID *)pstImmZc->data);
        pstAdDesc->OutPtr1 = (modem_phy_addr)virt_to_phys((VOS_VOID *)pstImmZc);
    }

    if (0 == ulCnt)
    {
        return 0;
    }

    /* ���뵽AD����Ҫ����ADQ */
    lRslt = mdrv_ipf_config_dlad(enAdType, ulCnt, ADS_DL_GET_IPF_AD_DESC_PTR(enAdType, 0));
    if (IPF_SUCCESS != lRslt)
    {
        /* ����ʧ�ܣ��ͷ��ڴ� */
        ulTmp = ulCnt;
        for (ulCnt = 0; ulCnt < ulTmp; ulCnt++)
        {
            pstAdDesc = ADS_DL_GET_IPF_AD_DESC_PTR(enAdType, ulCnt);
            pstImmZc  = (IMM_ZC_STRU *)phys_to_virt((unsigned long)pstAdDesc->OutPtr1);
            IMM_ZcFreeAny(pstImmZc);
            ADS_DBG_DL_ADQ_FREE_MEM_NUM(1);
        }

        ADS_DBG_DL_ADQ_CFG_IPF_FAIL_NUM(1);
        return 0;
    }

    /* ���ʵ�����õ�AD��Ŀ */
    ADS_DBG_DL_ADQ_CFG_AD_NUM(ulCnt);
    ADS_DBG_DL_ADQ_CFG_IPF_SUCC_NUM(1);
    return ulCnt;
}

/*****************************************************************************
 �� �� ��  : ADS_DL_AllocMemForAdq
 ��������  : ����ADQ
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��12��19��
    ��    ��   : l60609
    �޸�����   : �����ɺ���

  2.��    ��   : 2015��4��24��
    ��    ��   : A00165503
    �޸�����   : DTS2015042201072: �Ż�ADQ���жϴ���, ��ֹ�ڴ��޷���ʱ����

*****************************************************************************/
VOS_VOID ADS_DL_AllocMemForAdq(VOS_VOID)
{
    VOS_INT32                           lRslt;
    VOS_UINT32                          ulIpfAd0Num;
    VOS_UINT32                          ulIpfAd1Num;
    VOS_UINT32                          ulActAd0Num;
    VOS_UINT32                          ulActAd1Num;

    ulIpfAd0Num = 0;
    ulIpfAd1Num = 0;
    ulActAd0Num = 0;
    ulActAd1Num = 0;

    /* ��ȡ����ADQ�Ŀ��е�AD���� */
    lRslt = mdrv_ipf_get_dlad_num(&ulIpfAd0Num, &ulIpfAd1Num);
    if (IPF_SUCCESS != lRslt)
    {
        ADS_DBG_DL_ADQ_GET_FREE_AD_FAIL_NUM(1);
        return;
    }

    ADS_DBG_DL_ADQ_GET_FREE_AD_SUCC_NUM(1);

    /* �������ô��ڴ��ADQ1 */
    if (0 != ulIpfAd1Num)
    {
        ulActAd1Num = ADS_DL_ConfigAdq(IPF_AD_1, ulIpfAd1Num);
        ADS_DBG_DL_ADQ_CFG_AD1_NUM(ulActAd1Num);
    }

    /* ������С�ڴ��ADQ0 */
    if (0 != ulIpfAd0Num)
    {
        ulActAd0Num = ADS_DL_ConfigAdq(IPF_AD_0, ulIpfAd0Num);
        ADS_DBG_DL_ADQ_CFG_AD0_NUM(ulActAd0Num);
    }

    /* AD0Ϊ�ջ���AD1Ϊ����Ҫ����������ʱ�� */
    if ( ((0 == ulActAd0Num) && (ADS_IPF_DLAD_START_TMR_THRESHOLD < ulIpfAd0Num))
      || ((0 == ulActAd1Num) && (ADS_IPF_DLAD_START_TMR_THRESHOLD < ulIpfAd1Num)) )
    {
        /* �������ADQ�κ�һ���������벻���ڴ棬����ʱ�� */
        ADS_StartTimer(TI_ADS_DL_ADQ_EMPTY, TI_ADS_DL_ADQ_EMPTY_LEN);
    }

    return;
}

/*****************************************************************************
 �� �� ��  : ADS_DL_SendNdClientDataInd
 ��������  : RA����ECHO REQUEST�ȱ���ͨ��VOS����NDCLIENT
 �������  : VOS_VOID *pData
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��12��7��
    ��    ��   : ³��/l60609
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_VOID ADS_DL_SendNdClientDataInd(IMM_ZC_STRU *pstImmZc)
{
    ADS_NDCLIENT_DATA_IND_STRU         *pstMsg  = VOS_NULL_PTR;
    VOS_UINT8                          *pucData = VOS_NULL_PTR;
    VOS_UINT32                          ulDataLen;
    VOS_UINT32                          ulResult;

    ulDataLen = IMM_ZcGetUsedLen(pstImmZc);

    /* ������Ϣ */
    pstMsg = (ADS_NDCLIENT_DATA_IND_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                               ACPU_PID_ADS_DL,
                                               sizeof(ADS_NDCLIENT_DATA_IND_STRU) + ulDataLen - 2);
    if (VOS_NULL_PTR == pstMsg)
    {
        ADS_ERROR_LOG(ACPU_PID_ADS_DL, "ADS_DL_SendNdClientDataInd: pstMsg is NULL!");
        return;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
               (VOS_SIZE_T)(sizeof(ADS_NDCLIENT_DATA_IND_STRU) + ulDataLen - 2 - VOS_MSG_HEAD_LENGTH),
               0x00,
               (VOS_SIZE_T)(sizeof(ADS_NDCLIENT_DATA_IND_STRU) + ulDataLen - 2 - VOS_MSG_HEAD_LENGTH));

    /* ��д��Ϣ���� */
    pstMsg->ulReceiverPid = UEPS_PID_NDCLIENT;
    pstMsg->enMsgId       = ID_ADS_NDCLIENT_DATA_IND;
    pstMsg->enModemId     = ADS_DL_GET_MODEMID_FROM_IMM(pstImmZc);
    pstMsg->ucRabId       = ADS_DL_GET_RABID_FROM_IMM(pstImmZc);;
    pstMsg->usLen         = (VOS_UINT16)ulDataLen;

    /* ������������ */
    pucData = IMM_ZcGetDataPtr(pstImmZc);
    TAF_MEM_CPY_S(pstMsg->aucData, ulDataLen, pucData, ulDataLen);

    /* ������Ϣ */
    ulResult = PS_SEND_MSG(ACPU_PID_ADS_DL, pstMsg);
    if (VOS_OK != ulResult)
    {
        ADS_ERROR_LOG(ACPU_PID_ADS_DL, "ADS_DL_SendNdClientDataInd: Send msg fail!");
    }

    return;
}

/*****************************************************************************
 �� �� ��  : ADS_DL_SendDhcpClientDataInd
 ��������  : DHCPv6����ͨ��VOS����DHCP CLIENT
 �������  : IMM_ZC_STRU *pstImmZc
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��05��18��
    ��    ��   : w00316385
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_VOID ADS_DL_SendDhcpClientDataInd(IMM_ZC_STRU *pstImmZc)
{
    ADS_DHCP_DATA_IND_STRU             *pstMsg  = VOS_NULL_PTR;
    VOS_UINT8                          *pucData = VOS_NULL_PTR;
    VOS_UINT32                          ulDataLen;
    VOS_UINT32                          ulResult;

    ulDataLen = IMM_ZcGetUsedLen(pstImmZc);

    /* ������Ϣ */
    pstMsg = (ADS_DHCP_DATA_IND_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                               ACPU_PID_ADS_DL,
                                               sizeof(ADS_DHCP_DATA_IND_STRU) + ulDataLen);

    /* �ڴ�����ʧ�ܣ����� */
    if( VOS_NULL_PTR == pstMsg )
    {
        ADS_ERROR_LOG(ACPU_PID_ADS_DL, "ADS_DL_SendDhcpClientDataInd: pstMsg is NULL!");
        return;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
               (VOS_SIZE_T)(sizeof(ADS_DHCP_DATA_IND_STRU) + ulDataLen - VOS_MSG_HEAD_LENGTH),
               0x00,
               (VOS_SIZE_T)(sizeof(ADS_DHCP_DATA_IND_STRU) + ulDataLen - VOS_MSG_HEAD_LENGTH));

    /* ��д��Ϣ���� */
    pstMsg->ulReceiverPid = UEPS_PID_DHCP;
    pstMsg->enMsgId       = ID_ADS_DHCP_DATA_IND;
    pstMsg->enModemId     = ADS_DL_GET_MODEMID_FROM_IMM(pstImmZc);
    pstMsg->ucRabId       = ADS_DL_GET_RABID_FROM_IMM(pstImmZc);
    pstMsg->ulLen         = ulDataLen;

    /* �������� */
    pucData = IMM_ZcGetDataPtr(pstImmZc);
    TAF_MEM_CPY_S(pstMsg->aucData, ulDataLen, pucData, ulDataLen);

    /* ����VOS����ԭ�� */
    ulResult = PS_SEND_MSG(ACPU_PID_ADS_DL, pstMsg);
    if (VOS_OK != ulResult)
    {
        ADS_ERROR_LOG(ACPU_PID_ADS_DL, "ADS_DL_SendDhcpClientDataInd: Send msg fail!");
    }

    return;
}

/* Added by Y00213812 for Spirnt ����, 2017-3-31, begin */
/*****************************************************************************
 �� �� ��  : ADS_DL_SendMipDataInd
 ��������  : ICMPV4��MIPV4����ͨ��VOS����MIP
 �������  : IMM_ZC_STRU *pstImmZc
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2017��3��31��
    ��    ��   : Y00213812
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_VOID ADS_DL_SendMipDataInd(IMM_ZC_STRU *pstImmZc)
{
    ADS_MIP_DATA_IND_STRU              *pstMsg  = VOS_NULL_PTR;
    VOS_UINT8                          *pucData = VOS_NULL_PTR;
    VOS_UINT32                          ulDataLen;

    ulDataLen = IMM_ZcGetUsedLen(pstImmZc);

    /* ������Ϣ */
    pstMsg = (ADS_MIP_DATA_IND_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                               ACPU_PID_ADS_DL,
                                               sizeof(ADS_MIP_DATA_IND_STRU) + ulDataLen - 4);

    /* �ڴ�����ʧ�ܣ����� */
    if( VOS_NULL_PTR == pstMsg )
    {
        ADS_ERROR_LOG(ACPU_PID_ADS_DL, "ADS_DL_SendMipDataInd: pstMsg is NULL!");
        return;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
               (VOS_SIZE_T)(sizeof(ADS_MIP_DATA_IND_STRU) + ulDataLen - 4 - VOS_MSG_HEAD_LENGTH),
               0x00,
               (VOS_SIZE_T)(sizeof(ADS_MIP_DATA_IND_STRU) + ulDataLen - 4 - VOS_MSG_HEAD_LENGTH));

    /* ��д��Ϣ���� */
    pstMsg->ulReceiverPid = MSPS_PID_MIP;
    pstMsg->enMsgId       = ID_ADS_MIP_DATA_IND;
    pstMsg->enModemId     = ADS_DL_GET_MODEMID_FROM_IMM(pstImmZc);
    pstMsg->ucRabId       = ADS_DL_GET_RABID_FROM_IMM(pstImmZc);
    pstMsg->usLen         = (VOS_UINT16)ulDataLen;

    /* �������� */
    pucData = IMM_ZcGetDataPtr(pstImmZc);
    TAF_MEM_CPY_S(pstMsg->aucData, ulDataLen, pucData, ulDataLen);

    /* ����VOS����ԭ�� */
    (VOS_VOID)PS_SEND_MSG(ACPU_PID_ADS_DL, pstMsg);

    return;
}

/* Added by Y00213812 for Spirnt ����, 2017-3-31, end */
/*****************************************************************************
 �� �� ��  : ADS_DL_FreeIpfUsedAd0
 ��������  : �ͷ�IPF��ʹ�õ�AD0
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��03��18��
    ��    ��   : f00179208
    �޸�����   : �����ɺ���
*****************************************************************************/
VOS_VOID ADS_DL_FreeIpfUsedAd0(VOS_VOID)
{
    VOS_UINT32                          i;
    VOS_UINT32                          ulAdNum;

    IPF_AD_DESC_S                      *pstAdDesc = VOS_NULL_PTR;

    ulAdNum = 0;

    pstAdDesc = (IPF_AD_DESC_S*)PS_MEM_ALLOC(ACPU_PID_ADS_DL,
                                             sizeof(IPF_AD_DESC_S) * IPF_DLAD0_DESC_SIZE);

    if ( VOS_NULL_PTR == pstAdDesc )
    {
        ADS_ERROR_LOG(ACPU_PID_ADS_DL, "ADS_DL_FreeIpfUsedAd0: pstAdDesc is null");
        return;
    }

    TAF_MEM_SET_S(pstAdDesc, (VOS_SIZE_T)(sizeof(IPF_AD_DESC_S) * IPF_DLAD0_DESC_SIZE), 0x00, (VOS_SIZE_T)(sizeof(IPF_AD_DESC_S) * IPF_DLAD0_DESC_SIZE));

    if (IPF_SUCCESS == mdrv_ipf_get_used_dlad(IPF_AD_0, (VOS_UINT32 *)&ulAdNum, pstAdDesc))
    {
        /* �ͷ�ADQ0���ڴ� */
        for (i = 0; i < PS_MIN(ulAdNum, IPF_DLAD0_DESC_SIZE); i++)
        {
            IMM_ZcFreeAny((IMM_ZC_STRU *)phys_to_virt((unsigned long)pstAdDesc[i].OutPtr1));
        }
    }
    else
    {
        ADS_DBG_DL_ADQ_GET_IPF_AD0_FAIL_NUM(1);
    }

    ADS_DBG_DL_ADQ_FREE_AD0_NUM(PS_MIN(ulAdNum, IPF_DLAD0_DESC_SIZE));

    PS_MEM_FREE(ACPU_PID_ADS_DL, pstAdDesc);

    return;
}

/*****************************************************************************
 �� �� ��  : ADS_DL_FreeIpfUsedAd1
 ��������  : �ͷ�IPF��ʹ�õ�AD1
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��03��18��
    ��    ��   : f00179208
    �޸�����   : �����ɺ���
*****************************************************************************/
VOS_VOID ADS_DL_FreeIpfUsedAd1(VOS_VOID)
{
    VOS_UINT32                          i;
    VOS_UINT32                          ulAdNum;

    IPF_AD_DESC_S                      *pstAdDesc = VOS_NULL_PTR;

    ulAdNum = 0;

    pstAdDesc = (IPF_AD_DESC_S*)PS_MEM_ALLOC(ACPU_PID_ADS_DL,
                                             sizeof(IPF_AD_DESC_S) * IPF_DLAD1_DESC_SIZE);

    if ( VOS_NULL_PTR == pstAdDesc )
    {
        ADS_ERROR_LOG(ACPU_PID_ADS_DL, "ADS_DL_FreeIpfUsedAd1: pstAdDesc is null");
        return;
    }

    TAF_MEM_SET_S(pstAdDesc, (VOS_SIZE_T)(sizeof(IPF_AD_DESC_S) * IPF_DLAD1_DESC_SIZE), 0x00, (VOS_SIZE_T)(sizeof(IPF_AD_DESC_S) * IPF_DLAD1_DESC_SIZE));

    if (IPF_SUCCESS == mdrv_ipf_get_used_dlad(IPF_AD_1, (VOS_UINT32 *)&ulAdNum, pstAdDesc))
    {
        /* �ͷ�ADQ1���ڴ� */
        for (i = 0; i < PS_MIN(ulAdNum, IPF_DLAD1_DESC_SIZE); i++)
        {
            IMM_ZcFreeAny((IMM_ZC_STRU *)phys_to_virt((unsigned long)pstAdDesc[i].OutPtr1));
        }
    }
    else
    {
        ADS_DBG_DL_ADQ_GET_IPF_AD1_FAIL_NUM(1);
    }

    ADS_DBG_DL_ADQ_FREE_AD1_NUM(PS_MIN(ulAdNum, IPF_DLAD1_DESC_SIZE));

    PS_MEM_FREE(ACPU_PID_ADS_DL, pstAdDesc);

    return;
}

/*****************************************************************************
 �� �� ��  : ADS_DL_SndCdsErrorInd
 ��������  : ����ID_ADS_CDS_ERR_IND��CDS
 �������  : usModemId --- MODEMID
             ucRabId   --- RABID [5,15]
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2017��7��11��
    ��    ��   : A00165503
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_VOID ADS_DL_SndCdsErrorInd(
    VOS_UINT16                          usModemId,
    VOS_UINT32                           ulRabId
)
{
    ADS_CDS_ERR_IND_STRU               *pstMsg = VOS_NULL_PTR;

    /* ����OSA��Ϣ */
    pstMsg = (ADS_CDS_ERR_IND_STRU *)ADS_DL_ALLOC_MSG_WITH_HDR(
                                            sizeof(ADS_CDS_ERR_IND_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        ADS_ERROR_LOG(ACPU_PID_ADS_DL, "ADS_DL_SndCdsErrorInd: Alloc MSG failed.\n");\
        return;
    }

    /* �����Ϣ���� */
    TAF_MEM_SET_S(ADS_DL_GET_MSG_ENTITY(pstMsg), ADS_DL_GET_MSG_LENGTH(pstMsg),
                  0x00, ADS_DL_GET_MSG_LENGTH(pstMsg));

    /* ��д��Ϣͷ */
    ADS_DL_CFG_CDS_MSG_HDR(pstMsg, ID_ADS_CDS_ERR_IND);

    /* ��д��Ϣ���� */
    pstMsg->enModemId = usModemId;
    pstMsg->ucRabId   = ulRabId;

    /* ������Ϣ */
    ADS_DL_SEND_MSG(pstMsg);
    return;
}

/*****************************************************************************
 �� �� ��  : ADS_DL_SndIntraPacketErrorInd
 ��������  : �����ڲ���ϢID_ADS_PACKET_ERROR_IND
 �������  : ucInstance   --- MODEMID
             usBearerMask --- Bearer Mask
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2017��7��11��
    ��    ��   : A00165503
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_VOID ADS_DL_SndIntraPacketErrorInd(
    VOS_UINT8                           ucInstance,
    VOS_UINT16                          usBearerMask
)
{
    ADS_PACKET_ERROR_IND_STRU          *pstMsg = VOS_NULL_PTR;

    /* ����OSA��Ϣ */
    pstMsg = (ADS_PACKET_ERROR_IND_STRU *)ADS_DL_ALLOC_MSG_WITH_HDR(
                                            sizeof(ADS_PACKET_ERROR_IND_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        ADS_ERROR_LOG(ACPU_PID_ADS_DL,
            "ADS_DL_SndIntraPacketErrorInd: Alloc MSG failed.\n");
        return;
    }

    /* ��д��Ϣͷ */
    ADS_DL_CFG_INTRA_MSG_HDR(pstMsg, ID_ADS_PACKET_ERROR_IND);

    /* ��д��Ϣ���� */
    pstMsg->usModemId    = ucInstance;
    pstMsg->usBearerMask = usBearerMask;

    /* ������Ϣ */
    ADS_DL_SEND_MSG(pstMsg);
    return;
}

/*****************************************************************************
 �� �� ��  : ADS_DL_RecordPacketErrorStats
 ��������  : ��¼���г����ϵĴ��ͳ��
 �������  : pstImmZc --- imm memory
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2017��7��11��
    ��    ��   : A00165503
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_VOID ADS_DL_RecordPacketErrorStats(IMM_ZC_STRU *pstImmZc)
{
    ADS_PACKET_ERROR_FEEDBACK_CFG_STRU *pstFeedbackCfg = VOS_NULL_PTR;
    ADS_BEARER_PACKET_ERROR_STATS_STRU *pstPktErrStats = VOS_NULL_PTR;
    ADS_DL_IMM_PRIV_CB_STRU               *pstImmPriv     = VOS_NULL_PTR;

    pstFeedbackCfg = ADS_DL_GET_PKT_ERR_FEEDBACK_CFG_PTR();
    if (VOS_FALSE == pstFeedbackCfg->ulEnabled)
    {
        return;
    }

    pstImmPriv = ADS_DL_IMM_PRIV_CB(pstImmZc);
    if (ADS_CDS_IPF_PKT_TYPE_IP == ADS_DL_GET_PKT_TYPE(pstImmPriv->ucModemId,
                                                       pstImmPriv->ucRabId))
    {
        pstPktErrStats = ADS_DL_GET_PKT_ERR_STATS_PTR(pstImmPriv->ucModemId,
                                                      pstImmPriv->ucRabId);
        if (pstImmPriv->u.usIpfResult & ADS_DL_IPF_RESULT_PKT_ERR_FEEDBACK_MASK)
        {
            if (0 == pstPktErrStats->ulErrorPktNum)
            {
                pstPktErrStats->ulMinDetectExpires =
                    ADS_CURRENT_TICK + pstFeedbackCfg->ulMinDetectDuration;
                pstPktErrStats->ulMaxDetectExpires =
                    ADS_CURRENT_TICK + pstFeedbackCfg->ulMaxDetectDuration;
            }

            pstPktErrStats->ulErrorPktNum++;
        }

        if (0 != pstPktErrStats->ulErrorPktNum)
        {
            pstPktErrStats->ulTotalPktNum++;
        }
    }
}

/*****************************************************************************
 �� �� ��  : ADS_DL_CalcPacketErrorRate
 ��������  : ��������
 �������  : ulErrorPktNum --- �������
             ulTotalPktNum --- ���ݰ�����
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2017��7��11��
    ��    ��   : A00165503
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 ADS_DL_CalcPacketErrorRate(
    VOS_UINT32                          ulErrorPktNum,
    VOS_UINT32                          ulTotalPktNum
)
{
    VOS_UINT32                          ulPktErrRate;

    if ((0 == ulTotalPktNum) || (ulErrorPktNum > ulTotalPktNum))
    {
        ulPktErrRate = 100;
    }
    else if (ulErrorPktNum <= ADS_DL_ERROR_PACKET_NUM_THRESHOLD)
    {
        ulPktErrRate = ulErrorPktNum * 100 / ulTotalPktNum;
    }
    else
    {
        ulPktErrRate = ulErrorPktNum / (ulTotalPktNum / 100);
    }

    return ulPktErrRate;
}

/*****************************************************************************
 �� �� ��  : ADS_DL_FeedBackPacketErrorIfNeeded
 ��������  : ���ش���ʴﵽ��ֵ��, ���������Э��ջ
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2017��7��11��
    ��    ��   : A00165503
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_VOID ADS_DL_FeedBackPacketErrorIfNeeded(VOS_VOID)
{
    ADS_PACKET_ERROR_FEEDBACK_CFG_STRU *pstFeedbackCfg = VOS_NULL_PTR;
    ADS_BEARER_PACKET_ERROR_STATS_STRU *pstPktErrStats = VOS_NULL_PTR;
    VOS_UINT32                          ulPktErrRate;
    VOS_UINT16                          usBearerMask;
    VOS_UINT32                           ulInstance;
    VOS_UINT32                           ulRabId;

    pstFeedbackCfg = ADS_DL_GET_PKT_ERR_FEEDBACK_CFG_PTR();
    if (VOS_FALSE == pstFeedbackCfg->ulEnabled)
    {
        return;
    }

    for (ulInstance = 0; ulInstance < ADS_INSTANCE_MAX_NUM; ulInstance++)
    {
        usBearerMask = 0;

        for (ulRabId = ADS_RAB_ID_MIN; ulRabId <= ADS_RAB_ID_MAX; ulRabId++)
        {
            pstPktErrStats = ADS_DL_GET_PKT_ERR_STATS_PTR(ulInstance, ulRabId);
            if (0 != pstPktErrStats->ulErrorPktNum)
            {
                if (ADS_TIME_IN_RANGE(ADS_CURRENT_TICK,
                        pstPktErrStats->ulMinDetectExpires,
                        pstPktErrStats->ulMaxDetectExpires))
                {
                    ulPktErrRate = ADS_DL_CalcPacketErrorRate(
                                        pstPktErrStats->ulErrorPktNum,
                                        pstPktErrStats->ulTotalPktNum);

                    if (ulPktErrRate >= pstFeedbackCfg->ulPktErrRateThres)
                    {
                        usBearerMask = ADS_BIT16_SET(usBearerMask, ulRabId);
                    }

                    pstPktErrStats->ulErrorPktNum = 0;
                    pstPktErrStats->ulTotalPktNum = 0;
                }

                if (ADS_TIME_AFTER(ADS_CURRENT_TICK,
                        pstPktErrStats->ulMaxDetectExpires))
                {
                    pstPktErrStats->ulErrorPktNum = 0;
                    pstPktErrStats->ulTotalPktNum = 0;
                    continue;
                }
            }
        }

        if (0 != usBearerMask)
        {
            ADS_DL_SndIntraPacketErrorInd(ulInstance, usBearerMask);
        }
    }

    return;
}

/*****************************************************************************
 �� �� ��  : ADS_DL_ValidateImmMem
 ��������  : ���IMM�ڴ�����Ч��
 �������  : pstImmZc --- imm memory
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2017��7��11��
    ��    ��   : A00165503
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 ADS_DL_ValidateImmMem(IMM_ZC_STRU *pstImmZc)
{
    ADS_DL_IMM_PRIV_CB_STRU            *pstImmPriv = VOS_NULL_PTR;

    pstImmPriv = ADS_DL_IMM_PRIV_CB(pstImmZc);

    /* ���MODEMID */
    if (!ADS_IS_MODEMID_VALID(pstImmPriv->ucModemId))
    {
        ADS_DBG_DL_RMNET_MODEMID_ERR_NUM(1);
        return VOS_ERR;
    }

    /* ���RABID */
    if (!ADS_IS_RABID_VALID(pstImmPriv->ucRabId))
    {
        ADS_DBG_DL_RMNET_RABID_ERR_NUM(1);
        return VOS_ERR;
    }

    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : ADS_DL_Xmit
 ��������  : �ַ����ݰ�, ����ÿ�������ϵ����һ�����������
 �������  : ucInstance --- modem ID
             ulRabId    --- RABID [5, 15]
             pstImmZc   --- IP buff
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��6��29��
    ��    ��   : A00165503
    �޸�����   : �����ɺ���

  2.��    ��   : 2017��3��10��
    ��    ��   : l00373346
    �޸�����   : ����NAPI+GRO����֧��

  3.��    ��   : 2017��7��11��
    ��    ��   : A00165503
    �޸�����   : DTS2017081104985: �Ż������߼�

*****************************************************************************/
VOS_VOID ADS_DL_Xmit(
    IMM_ZC_STRU                         *pstImmZc,
    VOS_UINT32                           ulInstance,
    VOS_UINT32                           ulRabId
)
{
    RCV_DL_DATA_FUNC                    pRcvDlDataFunc    = VOS_NULL_PTR;
    RCV_RD_LAST_DATA_FUNC               pRcvRdLstDataFunc = VOS_NULL_PTR;
    IMM_ZC_STRU                        *pstLstImmZc       = VOS_NULL_PTR;
    VOS_UINT32                          ulExParam;
    VOS_UINT16                          usIpfResult;
    ADS_PKT_TYPE_ENUM_UINT8             enIpType;
    VOS_UINT32                          ulExRabId;

    pRcvDlDataFunc    = ADS_DL_GET_DATA_CALLBACK_FUNC(ulInstance, ulRabId);
    pRcvRdLstDataFunc = ADS_DL_GET_RD_LST_DATA_CALLBACK_FUNC(ulInstance, ulRabId);

    /* ��ȡ��������� */
    pstLstImmZc = ADS_DL_GET_LST_DATA_PTR(ulInstance, ulRabId);
    if (VOS_NULL_PTR != pstLstImmZc)
    {
        usIpfResult = ADS_DL_GET_IPF_RESULT_FORM_IMM(pstLstImmZc);
        enIpType    = ADS_DL_GET_IPTYPE_FROM_IPF_RESULT(usIpfResult);
        ulExParam   = ADS_DL_GET_DATA_EXPARAM(ulInstance, ulRabId);
        ulExRabId   = ADS_BUILD_EXRABID(ulInstance, ulRabId);

        pRcvDlDataFunc = ADS_DL_GET_DATA_CALLBACK_FUNC(ulInstance, ulRabId);
        if (VOS_NULL_PTR != pRcvDlDataFunc)
        {
            (VOS_VOID)pRcvDlDataFunc(ulExRabId, pstLstImmZc, enIpType, ulExParam);

            /* ���һ������ */
            pRcvRdLstDataFunc = ADS_DL_GET_RD_LST_DATA_CALLBACK_FUNC(ulInstance, ulRabId);
            if ((VOS_NULL_PTR == pstImmZc) && (VOS_NULL_PTR != pRcvRdLstDataFunc))
            {
                pRcvRdLstDataFunc(ulExParam);
            }
            ADS_DBG_DL_RMNET_TX_PKT_NUM(1);
        }
        else
        {
            IMM_ZcFreeAny(pstLstImmZc);
            ADS_DBG_DL_RMNET_NO_FUNC_FREE_PKT_NUM(1);
        }
    }

    ADS_DL_SET_LST_DATA_PTR(ulInstance, ulRabId, pstImmZc);

    return;
}

/*****************************************************************************
 �� �� ��  : ADS_DL_ProcTxData
 ��������  : ��������RD
 �������  : pstImmZc --- �ǿմ�����������, ��ָ�����Ҫ�������һ����
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��3��1��
    ��    ��   : A00165503
    �޸�����   : �����ɺ���

  2.��    ��   : 2017��7��11��
    ��    ��   : A00165503
    �޸�����   : DTS2017081104985: �Ż������߼�

*****************************************************************************/
VOS_VOID ADS_DL_ProcTxData(IMM_ZC_STRU *pstImmZc)
{
    VOS_UINT32                           ulInstance;
    VOS_UINT32                           ulRabId;

    if (VOS_NULL_PTR != pstImmZc)
    {
        ulInstance = ADS_DL_GET_MODEMID_FROM_IMM(pstImmZc);
        ulRabId    = ADS_DL_GET_RABID_FROM_IMM(pstImmZc);

        ADS_DL_Xmit(pstImmZc, ulInstance, ulRabId);
    }
    else
    {
        for (ulInstance = 0; ulInstance < ADS_INSTANCE_MAX_NUM; ulInstance++)
        {
            for (ulRabId = ADS_RAB_ID_MIN; ulRabId <= ADS_RAB_ID_MAX; ulRabId++)
            {
                ADS_DL_Xmit(VOS_NULL_PTR, ulInstance, ulRabId);
            }
        }
    }

    return;
}

/*****************************************************************************
 �� �� ��  : ADS_DL_RdDescTransImmMem
 ��������  : ��RD������ת����IMM�ڴ��ʽ, ���������������Ϣ
 �������  : pstRdDesc --- RD������
 �������  : ��
 �� �� ֵ  : IMM_ZC_STRU*
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��10��06��
    ��    ��   : A00165503
    �޸�����   : �����ɺ���

  2.��    ��   : 2017��7��24��
    ��    ��   : A00165503
    �޸�����   : DTS2017072014516: ��¼��������Ϣ

*****************************************************************************/
IMM_ZC_STRU* ADS_DL_RdDescTransImmMem(const IPF_RD_DESC_S *pstRdDesc)
{
    ADS_IPF_RD_RECORD_STRU             *pstRdRecord = VOS_NULL_PTR;
    IMM_ZC_STRU                        *pstImmZc = VOS_NULL_PTR;
    VOS_UINT32                          ulCacheLen;

    /* ��OUTָ��ת��ΪIMM�ڴ�ָ�� */
    pstImmZc = (IMM_ZC_STRU *)phys_to_virt((VOS_ULONG)ADS_DL_GET_IPF_RD_DESC_OUT_PTR(pstRdDesc));
    if (VOS_NULL_PTR == pstImmZc)
    {
        return VOS_NULL_PTR;
    }

    /* ��¼��������Ϣ */
    pstRdRecord = ADS_DL_GET_IPF_RD_RECORD_PTR();
    if (pstRdRecord->ulPos >= ADS_IPF_RD_BUFF_RECORD_NUM)
    {
        pstRdRecord->ulPos = 0;
    }

    pstRdRecord->astRdBuff[pstRdRecord->ulPos].ulSlice  = VOS_GetSlice();
    pstRdRecord->astRdBuff[pstRdRecord->ulPos].usAttr   = pstRdDesc->u16Attribute;
    pstRdRecord->astRdBuff[pstRdRecord->ulPos].usPktLen = pstRdDesc->u16PktLen;
    pstRdRecord->astRdBuff[pstRdRecord->ulPos].pstImmZc = pstImmZc;
    pstRdRecord->astRdBuff[pstRdRecord->ulPos].pOutPtr  = (VOS_VOID *)ADS_DL_GET_IPF_RD_DESC_OUT_PTR(pstRdDesc);
    pstRdRecord->ulPos++;

    /* ͳһˢCACHE */
    ulCacheLen = pstRdDesc->u16PktLen + IMM_MAC_HEADER_RES_LEN;
    ADS_IPF_DL_MEM_UNMAP(pstImmZc, ulCacheLen);

    /* ����������ʵ���� */
    IMM_ZcPut(pstImmZc, pstRdDesc->u16PktLen);

    /* ������������Ϣ: u16Result/u16UsrField1 */
    ADS_DL_SAVE_RD_DESC_TO_IMM(pstImmZc, pstRdDesc);

    return pstImmZc;
}

/*****************************************************************************
 �� �� ��  : ADS_DL_ProcIpfFilterData
 ��������  : ����IPF filterƥ�䱨��
 �������  : pstImmZc --- skb����
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��11��11��
    ��    ��   : l00373346
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_VOID ADS_DL_ProcIpfFilterData(
    IMM_ZC_STRU                        *pstImmZc
)
{
    VOS_UINT32                          ulCacheLen;
    VOS_UINT16                          usIpfResult;

    /* ͳһˢCACHE */
    usIpfResult = ADS_DL_GET_IPF_RESULT_FORM_IMM(pstImmZc);
    ulCacheLen  = IMM_ZcGetUsedLen(pstImmZc) + IMM_MAC_HEADER_RES_LEN;

    ADS_IPF_SPE_MEM_UNMAP(pstImmZc, ulCacheLen);

    /* ƥ�����й��˹��������
     * BearId 19: NDClient������Ҫת����NDClient
     * BearId 17: DHCPv6������Ҫת����DHCP
     * [0, 15]����Ϊ�Ƿ����ݰ�;
     * [16, 18, 20, 21]Ŀǰֱ���ͷ�
     */
    if (CDS_ADS_DL_IPF_BEARER_ID_ICMPV6 == ADS_DL_GET_BEAREDID_FROM_IPF_RESULT(usIpfResult))
    {
        ADS_DL_SendNdClientDataInd(pstImmZc);
        ADS_DBG_DL_RDQ_RX_ND_PKT_NUM(1);
    }
    else if (CDS_ADS_DL_IPF_BEARER_ID_DHCPV6 == ADS_DL_GET_BEAREDID_FROM_IPF_RESULT(usIpfResult))
    {
        ADS_DL_SendDhcpClientDataInd(pstImmZc);
        ADS_DBG_DL_RDQ_RX_DHCP_PKT_NUM(1);
    }
    /* Added by Y00213812 for Spirnt ����, 2017-3-31, begin */
    else if (CDS_ADS_DL_IPF_BEARER_ID_MIP_ADV == ADS_DL_GET_BEAREDID_FROM_IPF_RESULT(usIpfResult))
    {
        ADS_DL_SendMipDataInd(pstImmZc);
        ADS_DBG_DL_RDQ_RX_ND_PKT_NUM(1);
    }
    else if (CDS_ADS_DL_IPF_BEARER_ID_MIP_REG_REPLY == ADS_DL_GET_BEAREDID_FROM_IPF_RESULT(usIpfResult))
    {
        ADS_DL_SendMipDataInd(pstImmZc);
        ADS_DBG_DL_RDQ_RX_ND_PKT_NUM(1);
    }
    /* Added by Y00213812 for Spirnt ����, 2017-3-31, end */
    else
    {
        ADS_DBG_DL_RDQ_FILTER_ERR_PKT_NUM(1);
    }

    ADS_IPF_SPE_MEM_MAP(pstImmZc, ulCacheLen);
    IMM_ZcFreeAny(pstImmZc);

    return;

}

/*****************************************************************************
 �� �� ��  : ADS_DL_ProcIpfFilterQue
 ��������  : ����IPF filterƥ�䱨�Ķ���
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��11��11��
    ��    ��   : l00373346
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_VOID ADS_DL_ProcIpfFilterQue(VOS_VOID)
{
    IMM_ZC_STRU                        *pstImmZc = VOS_NULL_PTR;

    for ( ; ; )
    {
        pstImmZc = IMM_ZcDequeueHead(ADS_GET_IPF_FILTER_QUE());
        if (VOS_NULL_PTR == pstImmZc)
        {
            break;
        }

        ADS_DL_ProcIpfFilterData(pstImmZc);
    }

    return;
}

/*****************************************************************************
 �� �� ��  : ADS_DL_ProcIpfResult
 ��������  : ADS����IPF�¼�����
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :


 �޸���ʷ      :
  1.��    ��   : 2012��4��12��
    ��    ��   : A00165503
    �޸�����   : �����ɺ���

  2.��    ��   : 2015��9��22��
    ��    ��   : A00165503
    �޸�����   : DTS2015081008249: ADS���������г���, ��ֹϵͳ��������

  3.��    ��   : 2017��7��11��
    ��    ��   : A00165503
    �޸�����   : DTS2017081104985: ���Ӵ����������

***************************************************************************/
VOS_VOID ADS_DL_ProcIpfResult(VOS_VOID)
{
    ADS_DL_IPF_RESULT_STRU             *pstIpfResult = VOS_NULL_PTR;
    IPF_RD_DESC_S                      *pstRdDesc = VOS_NULL_PTR;
    IMM_ZC_STRU                        *pstImmZc = VOS_NULL_PTR;
    VOS_UINT32                          ulRdNum = IPF_DLRD_DESC_SIZE;
    VOS_UINT32                          ulTxTimeout = 0;
    VOS_UINT32                          ulCnt;
    VOS_ULONG                           ulLockLevel;
    VOS_UINT32                          ulExParam;
    VOS_UINT32                          ulInstance;
    VOS_UINT32                          ulRabId;
    VOS_UINT32                          ulRnicNapiPollQueLen = 0;

    /*
     * IPF_RD_DESC_S��u16Result����
     * [15]pkt_len_err     IP�����������÷�Χ��
     * [14]bd_cd_noeq      BD��len��CD�ĳ��Ȳ��ȴ�����ʾ
     * [13]pkt_parse_err   ���ݽ�������ָʾ
     * [12]bd_pkt_noeq     BD��len��IP��ͷָʾ��len���ȴ���ָʾ
     * [11]head_len_err    IPV4���ȴ���ָʾ�źţ�IPV6����鳤��
     * [10]version_err     �汾�Ŵ���ָʾ
     * [9]ip_type          IP�����ͣ�
     *                     0 ��ʾIPV4
     *                     1 ��ʾIPV6
     * [8]ff_type          ��Ƭ����һ����Ƭ�Ƿ�����ϲ�ͷָʾ
     *                     0 ��ʾ��Ƭ����һ����Ƭ�����ϲ�ͷ(IP��δ��ƬʱҲΪ0)
     *                     1 ��ʾ��Ƭ����һ����Ƭ�����ϲ�ͷ
     * [7:6]pf_type        IP����Ƭָʾ����
     *                     0 ��ʾIP��δ��Ƭ
     *                     1 ��ʾIP����Ƭ����Ϊ��һ����Ƭ��
     *                     2 ��ʾ��Ƭ����Ϊ���һ����Ƭ��
     *                     3 ��ʾ��Ƭ����Ϊ�м��Ƭ
     * [0:5]bear_id        ���غţ����Ϊ0x3F�������й�������ƥ��
     */

    /* ��ȡRD DESC */
    pstRdDesc = ADS_DL_GET_IPF_RD_DESC_PTR(0);
    mdrv_ipf_get_dlrd(&ulRdNum, pstRdDesc);

    /* ��ȡ��RDΪ0 */
    if (0 == ulRdNum)
    {
        ADS_DBG_DL_RDQ_GET_RD0_NUM(1);
        return;
    }


    /* ����RDͳ�Ƹ��� */
    ADS_DBG_DL_RDQ_RX_RD_NUM(ulRdNum);

    /* ������AD���ٴ���RD */
    /*lint -e571*/
    VOS_SpinLockIntLock(&(g_stAdsCtx.stAdsIpfCtx.stAdqSpinLock), ulLockLevel);
    /*lint +e571*/
    ADS_DL_AllocMemForAdq();
    VOS_SpinUnlockIntUnlock(&(g_stAdsCtx.stAdsIpfCtx.stAdqSpinLock), ulLockLevel);

    for (ulCnt = 0; ulCnt < ulRdNum; ulCnt++)
    {
        pstRdDesc = ADS_DL_GET_IPF_RD_DESC_PTR(ulCnt);

        /* ת��ΪIMM�ڴ� */
        pstImmZc = ADS_DL_RdDescTransImmMem(pstRdDesc);
        if (VOS_NULL_PTR == pstImmZc)
        {
            ADS_DBG_DL_RDQ_TRANS_MEM_FAIL_NUM(1);
            continue;
        }

        /* У��IMM�ڴ� */
        if (VOS_OK != ADS_DL_ValidateImmMem(pstImmZc))
        {
            IMM_ZcFreeAny(pstImmZc);
            continue;
        }

        /* ͳ���쳣���� */
        ADS_DL_RecordPacketErrorStats(pstImmZc);

        /* ͳ�������������յ��������ֽ���������������ѯ */
        ADS_RECV_DL_PERIOD_PKT_NUM(pstRdDesc->u16PktLen);

        ulInstance           = ADS_DL_GET_MODEMID_FROM_IMM(pstImmZc);
        ulRabId              = ADS_DL_GET_RABID_FROM_IMM(pstImmZc);
        ulExParam            = ADS_DL_GET_DATA_EXPARAM(ulInstance, ulRabId);
        ulRnicNapiPollQueLen = ADS_DL_GET_RNIC_NAPI_POLL_QUE_LEN(ulExParam);

        ADS_MNTN_RecDLIpPktInfo(pstImmZc,
                                ADS_DL_GET_IPF_RSLT_USR_FIELD1_FROM_IMM(pstImmZc),
                                pstRdDesc->u32UsrField2,
                                pstRdDesc->u32UsrField3,
                                ulRnicNapiPollQueLen);

        /* ��ȡIPF RESULT */
        pstIpfResult = (ADS_DL_IPF_RESULT_STRU *)&(pstRdDesc->u16Result);

        /* BearId 0x3F: �����������ݰ���Ҫת����NDIS/PPP/RNIC*/
        if (CDS_ADS_DL_IPF_BEARER_ID_INVALID == pstIpfResult->usBearedId)
        {
            if (pstRdDesc->u16Result & ADS_DL_IPF_RESULT_PKT_ERR_MASK)
            {
                ADS_DBG_DL_RDQ_RX_ERR_PKT_NUM(1);
            }

            ulTxTimeout = ADS_DL_TX_WAKE_LOCK_TMR_LEN;

            if (VOS_TRUE == g_ulAdsDlDiscardPktFlag)
            {
                IMM_ZcFreeAny(pstImmZc);
                continue;
            }

            ADS_DL_ProcTxData(pstImmZc);
            ADS_DBG_DL_RDQ_RX_NORM_PKT_NUM(1);
        }
        else
        {
            if (VOS_FALSE != VOS_CheckInterrupt())
            {
                /* ������л��� */
                IMM_ZcQueueTail(ADS_GET_IPF_FILTER_QUE(), pstImmZc);
            }
            else
            {
                ADS_DL_ProcIpfFilterData(pstImmZc);
            }
        }
    }

    /* �������һ������ */
    ADS_DL_ProcTxData(VOS_NULL_PTR);

    if (0 != IMM_ZcQueueLen(ADS_GET_IPF_FILTER_QUE()))
    {
        ADS_DL_SndEvent(ADS_DL_EVENT_IPF_FILTER_DATA_PROC);
    }

    /* ����������ʾ��Э��ջ */
    ADS_DL_FeedBackPacketErrorIfNeeded();

    ADS_MNTN_ReportDLPktInfo();

    ADS_DL_EnableTxWakeLockTimeout(ulTxTimeout);
    return;
}

/*****************************************************************************
 �� �� ��  : ADS_DL_IsFcAssemTuneNeeded
 ��������  : ����Ƿ���Ҫ���������������
 �������  : ulRdNum   - ���ݰ�����
 �������  : ��
 �� �� ֵ  : VOS_TRUE  - ��Ҫ����
             VOS_FALSE - ����Ҫ����
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��11��19��
    ��    ��   : A00165503
    �޸�����   : �����ɺ���
*****************************************************************************/
VOS_UINT32 ADS_DL_IsFcAssemTuneNeeded(VOS_UINT32 ulRdNum)
{
    ADS_DL_FC_ASSEM_STRU               *pstFcAssemInfo;
    VOS_UINT32                          ulRslt = VOS_FALSE;

    /* ��ȡ������ֵ���� */
    pstFcAssemInfo = ADS_DL_GET_FC_ASSEM_INFO_PTR(ADS_INSTANCE_INDEX_0);

    if (0 != pstFcAssemInfo->ulEnableMask)
    {
        pstFcAssemInfo->ulRdCnt += ulRdNum;

        if (ADS_TIME_AFTER_EQ(ADS_CURRENT_TICK, (pstFcAssemInfo->ulTmrCnt + pstFcAssemInfo->ulExpireTmrLen)))
        {
            if (VOS_FALSE == pstFcAssemInfo->ulFcActiveFlg)
            {
                if (pstFcAssemInfo->ulRdCnt >= pstFcAssemInfo->ulRateUpLev)
                {
                    ADS_DBG_DL_FC_ACTIVATE_NUM(1);
                    pstFcAssemInfo->ulFcActiveFlg = VOS_TRUE;
                    ulRslt = VOS_TRUE;
                }
            }
            else
            {
                if (pstFcAssemInfo->ulRdCnt <= pstFcAssemInfo->ulRateDownLev)
                {
                    pstFcAssemInfo->ulFcActiveFlg = VOS_FALSE;
                }
            }

            ADS_DBG_DL_FC_TMR_TIMEOUT_NUM(1);
            pstFcAssemInfo->ulTmrCnt = ADS_CURRENT_TICK;
            pstFcAssemInfo->ulRdCnt = 0;
        }
    }

    return ulRslt;
}

/*****************************************************************************
 �� �� ��  : ADS_DL_RegDrvAssemFunc
 ��������  : ��������ע�ắ��, ���ڽ������ͻ������, FC�޷���ʱ�������̲���,
             FC��ADSע�����صĻص�����, �Լ�����������ֵ����
 �������  : pstDrvAssemParam - ����������ֵ����
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��11��19��
    ��    ��   : A00165503
    �޸�����   : �����ɺ���
*****************************************************************************/
VOS_VOID ADS_DL_RegDrvAssemFunc(FC_ADS_DRV_ASSEM_STRU *pstDrvAssemParam)
{
    ADS_DL_FC_ASSEM_STRU               *pstFcAssemParamInfo;

    pstFcAssemParamInfo = ADS_DL_GET_FC_ASSEM_INFO_PTR(ADS_INSTANCE_INDEX_0);

    pstFcAssemParamInfo->ulEnableMask     = pstDrvAssemParam->ucEnableMask;
    pstFcAssemParamInfo->ulRateUpLev      = pstDrvAssemParam->ulDlRateUpLev;
    pstFcAssemParamInfo->ulRateDownLev    = pstDrvAssemParam->ulDlRateDownLev;
    pstFcAssemParamInfo->ulExpireTmrLen   = pstDrvAssemParam->ulExpireTmrLen;
    pstFcAssemParamInfo->pFcAssemTuneFunc = pstDrvAssemParam->pDrvAssemFunc;

    return;
}

/*****************************************************************************
 �� �� ��  : ADS_DL_RegDlDataCallback
 ��������  : ADS�������ݴ���ģ��Ϊ�ϲ�ģ���ṩ��ע���������ݽ��պ����ӿ�
 �������  : ucExRabId --- ��չRABID
             pFunc     --- ���ݽ��ջص�
             ulExParam --- ��չ����
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��12��7��
    ��    ��   : ³��/l60609
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 ADS_DL_RegDlDataCallback(
    VOS_UINT8                           ucExRabId,
    RCV_DL_DATA_FUNC                    pFunc,
    VOS_UINT32                          ulExParam
)
{
    ADS_DL_RAB_INFO_STRU               *pstDlRabInfo = VOS_NULL_PTR;
    VOS_UINT32                          ulInstance;
    VOS_UINT32                          ulRabId;

    /* ���MODEMID */
    ulInstance = ADS_GET_MODEMID_FROM_EXRABID(ucExRabId);
    if (!ADS_IS_MODEMID_VALID(ulInstance))
    {
        ADS_ERROR_LOG1(ACPU_PID_ADS_DL,
            "ADS_DL_RegDlDataCallback: ModemId is invalid! <ModemId>", ulInstance);
        return VOS_ERR;
    }

    /* ���RABID */
    ulRabId = ADS_GET_RABID_FROM_EXRABID(ucExRabId);
    if (!ADS_IS_RABID_VALID(ulRabId))
    {
        ADS_ERROR_LOG2(ACPU_PID_ADS_DL,
            "ADS_DL_RegDlDataCallback: RabId is invalid! <ModemId>,<RabId>", ulInstance, ulRabId);
        return VOS_ERR;
    }

    /* �����������ݻص����� */
    pstDlRabInfo = ADS_DL_GET_RAB_INFO_PTR(ulInstance, ulRabId);
    pstDlRabInfo->ulRabId           = ulRabId;
    pstDlRabInfo->ulExParam         = ulExParam;
    pstDlRabInfo->pRcvDlDataFunc    = pFunc;

    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : ADS_DL_RegFilterDataCallback
 ��������  : ADS�������ݴ���ģ��Ϊ�ϲ�ģ���ṩ��ע���������ݹ��˽��պ����ӿ�
 �������  : VOS_UINT8                           ulRabId
             ADS_FILTER_IP_ADDR_INFO_STRU       *pstFilterIpAddr,
             RCV_DL_DATA_FUNC                    pFunc
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��06��04��
    ��    ��   : l00198894
    �޸�����   : Share-PDP��Ŀ��������
*****************************************************************************/
VOS_UINT32 ADS_DL_RegFilterDataCallback(
    VOS_UINT8                           ucRabId,
    ADS_FILTER_IP_ADDR_INFO_STRU       *pstFilterIpAddr,
    RCV_DL_DATA_FUNC                    pFunc
)
{
    ADS_DL_RAB_INFO_STRU               *pstDlRabInfo        = VOS_NULL_PTR;
    VOS_UINT32                          ulInstanceIndex;
    VOS_UINT32                          ulRealRabId;

    /* ucRabId�ĸ�2��bit��ʾmodem id*/
    ulInstanceIndex = ADS_GET_MODEMID_FROM_EXRABID(ucRabId);
    ulRealRabId     = ADS_GET_RABID_FROM_EXRABID(ucRabId);

    /* RabId�Ϸ��Լ�� */
    if (!ADS_IS_RABID_VALID(ulRealRabId))
    {
        ADS_WARNING_LOG1(ACPU_PID_ADS_DL, "ADS_DL_RegFilterDataCallback: ucRabId is", ulRealRabId);
        return VOS_ERR;
    }

    pstDlRabInfo = ADS_DL_GET_RAB_INFO_PTR(ulInstanceIndex, ulRealRabId);

    /* ����ADS�������ݹ��˻ص����� */
    pstDlRabInfo->ulRabId               = ulRealRabId;
    pstDlRabInfo->pRcvDlFilterDataFunc  = pFunc;

    /* ������˵�ַ��Ϣ */
    ADS_FILTER_SaveIPAddrInfo(pstFilterIpAddr);

    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : ADS_DL_DeregFilterDataCallback
 ��������  : ADS�������ݴ���ģ��Ϊ�ϲ�ģ���ṩ��ȥע���������ݹ��˽��պ����ӿ�
 �������  : VOS_UINT8                           ulRabId
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��06��04��
    ��    ��   : l00198894
    �޸�����   : Share-PDP��Ŀ��������
*****************************************************************************/
VOS_UINT32 ADS_DL_DeregFilterDataCallback(VOS_UINT32 ulRabId)
{
    ADS_DL_RAB_INFO_STRU                 *pstDlRabInfo        = VOS_NULL_PTR;
    VOS_UINT32                           ulInstanceIndex;
    VOS_UINT32                           ulRealRabId;

    /* ucRabId�ĸ�2��bit��ʾmodem id*/
    ulInstanceIndex = ADS_GET_MODEMID_FROM_EXRABID(ulRabId);
    ulRealRabId     = ADS_GET_RABID_FROM_EXRABID(ulRabId);

    /* RabId�Ϸ��Լ�� */
    if (!ADS_IS_RABID_VALID(ulRealRabId))
    {
        ADS_WARNING_LOG1(ACPU_PID_ADS_DL, "ADS_DL_DeregFilterDataCallback: ulRabId is", ulRealRabId);
        return VOS_ERR;
    }

    pstDlRabInfo = ADS_DL_GET_RAB_INFO_PTR(ulInstanceIndex, ulRealRabId);

    /* ȥע��ADS�������ݹ��˻ص����� */
    pstDlRabInfo->pRcvDlFilterDataFunc  = VOS_NULL_PTR;

    /* ���������Ϣ */
    ADS_FILTER_Reset();

    return VOS_OK;
}

/*********************************************************************************
 �� �� ��  : ADS_DL_RegNapiCallback
 ��������  : ADS�������ݴ���ģ��Ϊ�ϲ�ģ���ṩ��ע������RD���һ�����ݴ���ص�����
 �������  : ucExRabId          --- ��չRABID
             pLastDataFunc      --- ���ݽ��ջص�
             pAdjNapiWeightFunc --- ��̬����NAPI Weight�ص�����ָ��
             ulExParam          --- ��չ����
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2017��03��10��
    ��    ��   : l00373346
    �޸�����   : �����ɺ���

*********************************************************************************/
VOS_UINT32 ADS_DL_RegNapiCallback(
    VOS_UINT8                          ulExRabId,
    RCV_RD_LAST_DATA_FUNC               pLastDataFunc,
    ADJUST_NAPI_WEIGHT_FUNC             pAdjNapiWeightFunc,
    VOS_UINT32                          ulExParam
)
{
    ADS_DL_RAB_INFO_STRU                *pstDlRabInfo = VOS_NULL_PTR;
    VOS_UINT32                          ulInstance;
    VOS_UINT32                          ulRabId;

    /* ���MODEMID */
    ulInstance = ADS_GET_MODEMID_FROM_EXRABID(ulExRabId);
    if (!ADS_IS_MODEMID_VALID(ulInstance))
    {
        ADS_ERROR_LOG1(ACPU_PID_ADS_DL,
            "ADS_DL_RegRdLastDataCallback: ModemId is invalid! <ModemId>", ulInstance);
        return VOS_ERR;
    }

    /* ���RABID */
    ulRabId = ADS_GET_RABID_FROM_EXRABID(ulExRabId);
    if (!ADS_IS_RABID_VALID(ulRabId))
    {
        ADS_ERROR_LOG2(ACPU_PID_ADS_DL,
            "ADS_DL_RegRdLastDataCallback: RabId is invalid! <ModemId>,<RabId>", ulInstance, ulRabId);
        return VOS_ERR;
    }

    /* �����������ݻص����� */
    pstDlRabInfo = ADS_DL_GET_RAB_INFO_PTR(ulInstance, ulRabId);
    pstDlRabInfo->ulRabId            = ulRabId;
    pstDlRabInfo->ulExParam          = ulExParam;
    pstDlRabInfo->pRcvRdLstDataFunc  = pLastDataFunc;
    pstDlRabInfo->pAdjNapiWeightFunc = pAdjNapiWeightFunc;

    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : ADS_DL_RcvTafPdpStatusInd
 ��������  : ����ID_APS_ADS_PDP_STATUS_IND��Ϣ
 �������  : MsgBlock *pMsg
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��12��6��
    ��    ��   : ³��/l60609
    �޸�����   : �����ɺ���

  2.��    ��   : 2012��4��12��
    ��    ��   : A00165503
    �޸�����   : DTS2012020708222: ADS�����Ż�
  3.��    ��   : 2012��11��24��
    ��    ��   : l60609
    �޸�����   : DSDA Phase II:˫ʵ��
  4.��    ��   : 2016��8��16��
    ��    ��   : l00373346
    �޸�����   : DTS2016081611130: VT��Ƶ�޻���
*****************************************************************************/
VOS_UINT32 ADS_DL_RcvTafPdpStatusInd(MsgBlock *pMsg)
{
    ADS_PDP_STATUS_IND_STRU            *pstPdpStatusInd;
    ADS_DL_RAB_INFO_STRU               *pstDlRabInfo;
    VOS_UINT32                          ulInstanceIndex;
    ADS_CDS_IPF_PKT_TYPE_ENUM_UINT8     enPktType;

    pstPdpStatusInd = (ADS_PDP_STATUS_IND_STRU *)pMsg;

    enPktType       = ADS_CDS_IPF_PKT_TYPE_IP;
    ulInstanceIndex = pstPdpStatusInd->enModemId;

    /* RabId�Ϸ��Լ�� */
    if (!ADS_IS_RABID_VALID(pstPdpStatusInd->ucRabId))
    {
        ADS_ERROR_LOG(ACPU_PID_ADS_DL, "ADS_DL_RcvTafPdpStatusInd: Rab Id is invalid");
        return VOS_ERR;
    }

    if (ADS_PDP_PPP == pstPdpStatusInd->enPdpType)
    {
        enPktType = ADS_CDS_IPF_PKT_TYPE_PPP;
    }

    pstDlRabInfo = ADS_DL_GET_RAB_INFO_PTR(ulInstanceIndex, pstPdpStatusInd->ucRabId);

    /* PDP���� */
    if (ADS_PDP_STATUS_ACT == pstPdpStatusInd->enPdpStatus)
    {
        /* ����ADS�������ݻص���RABID */
        pstDlRabInfo->ulRabId           = pstPdpStatusInd->ucRabId;
        pstDlRabInfo->enPktType         = enPktType;
    }
    /* PDPȥ����  */
    else if (ADS_PDP_STATUS_DEACT == pstPdpStatusInd->enPdpStatus)
    {
        /* ���ADS�������ݻص����� */
        pstDlRabInfo->ulRabId           = ADS_RAB_ID_INVALID;
        pstDlRabInfo->enPktType         = ADS_CDS_IPF_PKT_TYPE_IP;
        pstDlRabInfo->ulExParam         = 0;

        if (ADS_CLEAN_RCV_CB_FUNC_TURE == pstPdpStatusInd->enCleanRcvCbFlag)
        {
            pstDlRabInfo->pRcvDlDataFunc    = VOS_NULL_PTR;
            pstDlRabInfo->pRcvRdLstDataFunc  = VOS_NULL_PTR;
            pstDlRabInfo->pAdjNapiWeightFunc = VOS_NULL_PTR;
        }
        else
        {
            ADS_NORMAL_LOG(ACPU_PID_ADS_DL, "ADS_DL_RcvTafPdpStatusInd: Not clean ADS DL data call back func");
        }
    }
    else
    {
        ADS_ERROR_LOG(ACPU_PID_ADS_DL, "ADS_DL_RcvTafPdpStatusInd: Pdp Status is invalid");
    }

    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : ADS_DL_RcvCcpuResetStartInd
 ��������  : ADS_DL�յ�ADS_DL��ϢID_CCPU_ADS_DL_START_RESET_IND�Ĵ���
 �������  : pMsg:��Ϣ�׵�ַ
 �������  : ��
 �� �� ֵ  : VOS_UINT32:VOS_OK
 ���ú���  :
 ��������  :

 �޸���ʷ     :
 1.��    ��   : 2013��04��15��
   ��    ��   : f00179208
   �޸�����   : �����ɺ���

  2.��    ��   : 2015��9��1��
    ��    ��   : A00165503
    �޸�����   : DTS2015082300106: ���й�С��, A�˴���C�˵�����λ����������

  3.��    ��   : 2015��9��22��
    ��    ��   : A00165503
    �޸�����   : DTS2015081008249: ADS���������г���, ��ֹϵͳ��������

*****************************************************************************/
VOS_UINT32 ADS_DL_RcvCcpuResetStartInd(VOS_VOID)
{
    VOS_UINT32                           ulIndex;

    ADS_TRACE_HIGH("proc reset msg: enter.\n");

    /* ֹͣ���������Ķ�ʱ�� */
    for (ulIndex = 0; ulIndex < ADS_MAX_TIMER_NUM; ulIndex++)
    {
        ADS_StopTimer(ACPU_PID_ADS_DL, ulIndex, ADS_TIMER_STOP_CAUSE_USER);
    }

    /* �������������� */
    ADS_ResetDlCtx();

    /* �ͷ�IPF��AD */
    ADS_DL_FreeIpfUsedAd1();
    ADS_DL_FreeIpfUsedAd0();

    /* �ͷ��ź�����ʹ�õ���API����������� */
    VOS_SmV(ADS_GetDLResetSem());

    ADS_TRACE_HIGH("proc reset msg: leave.\n");
    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : ADS_DL_RcvCcpuResetEndInd
 ��������  : ADS_DL�յ�ADS_DL��ϢID_CCPU_ADS_DL_RESET_END_IND�Ĵ���
 �������  : pMsg:��Ϣ�׵�ַ
 �������  : ��
 �� �� ֵ  : VOS_UINT32:VOS_OK
 ���ú���  :
 ��������  :

 �޸���ʷ     :
 1.��    ��   : 2013��09��10��
   ��    ��   : f00179208
   �޸�����   : �����ɺ���

  2.��    ��   : 2015��9��1��
    ��    ��   : A00165503
    �޸�����   : DTS2015082300106: ���й�С��, A�˴���C�˵�����λ����������
*****************************************************************************/
VOS_UINT32 ADS_DL_RcvCcpuResetEndInd(
    MsgBlock                           *pstMsg
)
{
    VOS_ULONG                           ulLockLevel;
    ADS_TRACE_HIGH("proc reset msg: enter.\n");

    TAF_MEM_SET_S(ADS_DL_GET_IPF_RD_RECORD_PTR(),
                  sizeof(ADS_IPF_RD_RECORD_STRU),
                  0x00,
                  sizeof(ADS_IPF_RD_RECORD_STRU));

    /* ��λIPF */
    mdrv_ipf_reinit_dlreg();

    /* ���³�ʼ��ADQ */
    /*lint -e571*/
    VOS_SpinLockIntLock(&(g_stAdsCtx.stAdsIpfCtx.stAdqSpinLock), ulLockLevel);
    /*lint +e571*/
    ADS_DL_AllocMemForAdq();
    VOS_SpinUnlockIntUnlock(&(g_stAdsCtx.stAdsIpfCtx.stAdqSpinLock), ulLockLevel);

    ADS_TRACE_HIGH("proc reset msg: leave.\n");
    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : ADS_DL_RcvTafMsg
 ��������  : ��������TAF����Ϣ
 �������  : MsgBlock *pMsg
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��12��6��
    ��    ��   : ³��/l60609
    �޸�����   : �����ɺ���
 2.��    ��   : 2014��02��14��
   ��    ��   : m00217266
   �޸�����   : pdp status ind ��Ϊtaf aps����
*****************************************************************************/
VOS_UINT32 ADS_DL_RcvTafMsg(MsgBlock* pMsg)
{
    MSG_HEADER_STRU                    *pstMsg;

    pstMsg = (MSG_HEADER_STRU*)pMsg;

    switch(pstMsg->ulMsgName)
    {
        case ID_APS_ADS_PDP_STATUS_IND:
            ADS_DL_RcvTafPdpStatusInd(pMsg);
            break;

        default:
            break;
    }

    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : ADS_DL_RcvCdsMsg
 ��������  : ��������CDS����Ϣ
             ��ʱû��Ҫ�������Ϣ��Ԥ�����ں�����չ
 �������  : MsgBlock *pMsg
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��12��6��
    ��    ��   : ³��/l60609
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 ADS_DL_RcvCdsMsg(MsgBlock *pMsg)
{
    /* ��ʱû��Ҫ�������Ϣ������յ���Ϣ�����д��� */
    return VOS_ERR;
}

/*****************************************************************************
 �� �� ��  : ADS_DL_ProcIntraPacketErrorInd
 ��������  : ����ID_ADS_PACKET_ERROR_IND��Ϣ
 �������  : MsgBlock* pMsg
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2017��7��11��
    ��    ��   : A00165503
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_VOID ADS_DL_ProcIntraPacketErrorInd(MsgBlock* pMsg)
{
    ADS_PACKET_ERROR_IND_STRU          *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulRabId;

    pstMsg = (ADS_PACKET_ERROR_IND_STRU *)pMsg;

    for (ulRabId = ADS_RAB_ID_MIN; ulRabId <= ADS_RAB_ID_MAX; ulRabId++)
    {
        if (ADS_BIT16_IS_SET(pstMsg->usBearerMask, ulRabId))
        {
            ADS_DL_SndCdsErrorInd(pstMsg->usModemId, ulRabId);
        }
    }

    return;
}

/*****************************************************************************
 �� �� ��  : ADS_DL_RcvAdsDlMsg
 ��������  : ��������ADS DL����Ϣ
 �������  : MsgBlock *pMsg
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��04��15��
    ��    ��   : f00179208
    �޸�����   : �����ɺ���

  2.��    ��   : 2017��7��11��
    ��    ��   : A00165503
    �޸�����   : DTS2017081104985: ����ID_ADS_PACKET_ERROR_IND��Ϣ����

*****************************************************************************/
VOS_UINT32 ADS_DL_RcvAdsDlMsg(MsgBlock *pMsg)
{
    MSG_HEADER_STRU                    *pstMsg;

    pstMsg = (MSG_HEADER_STRU*)pMsg;

    switch(pstMsg->ulMsgName)
    {
        case ID_ADS_CCPU_RESET_START_IND:
            /* �ȴ�IPF���������ݰ�գ�ADS_DL�ٸ�λ���ȴ�ʱ��1�� */
            ADS_StartTimer(TI_ADS_DL_CCORE_RESET_DELAY, ADS_DL_GET_CCORE_RESET_DELAY_TIMER_LEN());
            break;

        case ID_ADS_CCPU_RESET_END_IND:
            ADS_DL_RcvCcpuResetEndInd(pMsg);
            break;

        case ID_ADS_PACKET_ERROR_IND:
            ADS_DL_ProcIntraPacketErrorInd(pMsg);
            break;

        default:
            ADS_NORMAL_LOG1(ACPU_PID_ADS_DL, "ADS_DL_RcvAdsDlMsg: rcv error msg id %d\r\n", pstMsg->ulMsgName);
            break;
    }

    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : ADS_DL_ProcMsg
 ��������  : ADS������Ϣ������
 �������  : MsgBlock* pMsg
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��12��6��
    ��    ��   : ³��/l60609
    �޸�����   : �����ɺ���
  2.��    ��   : 2013��4��15��
    ��    ��   : f00179208
    �޸�����   : C�˵�����λ��Ŀ
  3.��    ��   : 2014��02��14��
    ��    ��   : m00217266
    �޸�����   : pdp status ind ��Ϊtaf aps����
  4.��    ��   : 2015��6��25��
    ��    ��   : l00198894
    �޸�����   : TSTS
*****************************************************************************/
VOS_VOID ADS_DL_ProcMsg(MsgBlock* pMsg)
{
    if (VOS_NULL_PTR == pMsg)
    {
        return;
    }

    /* ��Ϣ�ķַ����� */
    switch ( pMsg->ulSenderPid )
    {
        /* ����APS����Ϣ */
        case I0_WUEPS_PID_TAF:
        case I1_WUEPS_PID_TAF:
        case I2_WUEPS_PID_TAF:
            ADS_DL_RcvTafMsg(pMsg);
            return;

        /* ����CDS����Ϣ */
        case UEPS_PID_CDS:
            ADS_DL_RcvCdsMsg(pMsg);
            return;

        /* ����ADS DL����Ϣ */
        case ACPU_PID_ADS_DL:
            ADS_DL_RcvAdsDlMsg(pMsg);
            return;

        default:
            return;
    }
}





