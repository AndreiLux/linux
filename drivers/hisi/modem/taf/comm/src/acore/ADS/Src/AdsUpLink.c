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
#include "AdsUpLink.h"
#include "AcpuReset.h"
#include "AdsFilter.h"
#include "AdsDebug.h"
#include "AdsMntn.h"
#include <net/inet_sock.h>
#include <linux/version.h>

/*****************************************************************************
    Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
*****************************************************************************/
#define    THIS_FILE_ID                 PS_FILE_ID_ADS_UPLINK_C


/*****************************************************************************
  2 ȫ�ֱ�������
*****************************************************************************/

/*****************************************************************************
  3 ����ʵ��
*****************************************************************************/

/*****************************************************************************
 �� �� ��  : ADS_UL_CCpuResetCallback
 ��������  : C�˵�����λʱADS UL�Ļص�������
 �������  : enParam   -- 0��ʾ��λǰ�� ������ʾ��λ��
             iUserData -- �û�����
 �������  : ��
 �� �� ֵ  : VOS_INT
 ���ú���  :
 ��������  :

 �޸���ʷ     :
 1.��    ��   : 2013��04��10��
   ��    ��   : f00179208
   �޸�����   : �����ɺ���
*****************************************************************************/
VOS_INT ADS_UL_CCpuResetCallback(
    DRV_RESET_CB_MOMENT_E               enParam,
    VOS_INT                             iUserData
)
{
    ADS_CCPU_RESET_IND_STRU                 *pstMsg = VOS_NULL_PTR;

    /* ����Ϊ0��ʾ��λǰ���� */
    if (MDRV_RESET_CB_BEFORE == enParam)
    {
        ADS_TRACE_HIGH("before reset: enter.\n");

        ADS_SetUlResetFlag(VOS_TRUE);

        /* ������Ϣ */
        pstMsg = (ADS_CCPU_RESET_IND_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(ACPU_PID_ADS_UL,
                                                                        sizeof(ADS_CCPU_RESET_IND_STRU));
        if (VOS_NULL_PTR == pstMsg)
        {
            ADS_TRACE_HIGH("before reset: alloc msg failed.\n");
            return VOS_ERROR;
        }

        /* ��д��Ϣͷ */
        pstMsg->ulReceiverPid               = ACPU_PID_ADS_UL;
        pstMsg->enMsgId                     = ID_ADS_CCPU_RESET_START_IND;

        /* ����Ϣ */
        if (VOS_OK != PS_SEND_MSG(ACPU_PID_ADS_UL, pstMsg))
        {
            ADS_TRACE_HIGH("before reset: send msg failed.\n");
            return VOS_ERROR;
        }

        /* �ȴ��ظ��ź�����ʼΪ��״̬���ȴ���Ϣ��������ź��������� */
        if (VOS_OK != VOS_SmP(ADS_GetULResetSem(), ADS_RESET_TIMEOUT_LEN))
        {
            ADS_TRACE_HIGH("before reset: VOS_SmP failed.\n");
            ADS_DBG_UL_RESET_LOCK_FAIL_NUM(1);
            return VOS_ERROR;
        }

        ADS_TRACE_HIGH("before reset: succ.\n");
        return VOS_OK;
    }
    /* ��λ�� */
    else if (MDRV_RESET_CB_AFTER == enParam)
    {
        ADS_TRACE_HIGH("after reset enter.\n");

        ADS_SetUlResetFlag(VOS_FALSE);

        ADS_TRACE_HIGH("after reset: succ.\n");
        ADS_DBG_UL_RESET_SUCC_NUM(1);
        return VOS_OK;
    }
    else
    {
        return VOS_ERROR;
    }
}

/*****************************************************************************
 �� �� ��  : ADS_UL_StartDsFlowStats
 ��������  : ��������ͳ��
 �������  : ucInstance - ʵ������
             ulRabId    - RABID [5,15]
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��8��22��
    ��    ��   : A00165503
    �޸�����   : �����ɺ���
*****************************************************************************/
VOS_VOID ADS_UL_StartDsFlowStats(
    VOS_UINT32                           ulInstance,
    VOS_UINT32                           ulRabId
)
{
    /* ������ж��д���, ����������ͳ�ƶ�ʱ�� */
    if (VOS_OK == ADS_UL_IsQueueExistent(ulInstance, ulRabId))
    {
        ADS_StartTimer(TI_ADS_DSFLOW_STATS, TI_ADS_DSFLOW_STATS_LEN);
    }

    return;
}

/*****************************************************************************
 �� �� ��  : ADS_UL_StopDsFlowStats
 ��������  : ֹͣ����ͳ��
 �������  : ucInstance - ʵ������
             ulRabId    - RABID [5,15]
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��8��22��
    ��    ��   : A00165503
    �޸�����   : �����ɺ���
*****************************************************************************/
VOS_VOID ADS_UL_StopDsFlowStats(
    VOS_UINT32                           ulInstance,
    VOS_UINT32                           ulRabId
)
{
    /* ����������ж����Ѳ����ڣ���ֹͣ����ͳ�ƶ�ʱ�����������ͳ����Ϣ */
    if (VOS_FALSE == ADS_UL_IsAnyQueueExist())
    {
        ADS_StopTimer(ACPU_PID_ADS_UL, TI_ADS_DSFLOW_STATS, ADS_TIMER_STOP_CAUSE_USER);
        ADS_InitStatsInfoCtx();
    }

    return;
}

/*****************************************************************************
 �� �� ��  : ADS_UL_SendPacket
 ��������  : ADS����Ϊ�ϲ�ģ���ṩ�����ݷ��ͺ��������ӿڲ��ͷ��ڴ档
             �ϲ�ģ����ݷ���ֵ�ж��Ƿ���Ҫ�ͷ��ڴ�
 �������  : pstImmZc  --- IMM����
             ucExRabId --- RABID [5, 15]
 �������  : ��
 �� �� ֵ  : VOS_OK/VOS_ERR
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��12��7��
    ��    ��   : ³��/l60609
    �޸�����   : �����ɺ���

  2.��    ��   : 2016��6��17��
    ��    ��   : A00165503
    �޸�����   : DTS2016061504494: ���Ӱ���Ϊ0�ļ��

*****************************************************************************/
VOS_UINT32 ADS_UL_SendPacket(
    IMM_ZC_STRU                        *pstImmZc,
    VOS_UINT8                           ucExRabId
)
{
    VOS_UINT32                           ulInstance;
    VOS_UINT32                           ulRabId;

    /* �ж��Ƿ�Ϊ�����ݰ� */
    if (VOS_NULL_PTR == pstImmZc)
    {
        ADS_WARNING_LOG(ACPU_PID_ADS_UL, "ADS_UL_SendPacket: pstImmZc is null!");
        return VOS_ERR;
    }

    if (0 == IMM_ZcGetUsedLen(pstImmZc))
    {
        ADS_WARNING_LOG(ACPU_PID_ADS_UL, "ADS_UL_SendPacket: len is 0!");
        return VOS_ERR;
    }

    /* �������н�������ͳ�Ƹ��� */
    ADS_DBG_UL_RMNET_RX_PKT_NUM(1);

    /* ͳ�������������յ��������ֽ���������������ѯ */
    ADS_RECV_UL_PERIOD_PKT_NUM(pstImmZc->len);

    /* ���MODEMID */
    ulInstance = ADS_GET_MODEMID_FROM_EXRABID(ucExRabId);
    if (ulInstance >= ADS_INSTANCE_MAX_NUM)
    {
        ADS_DBG_UL_RMNET_MODEMID_ERR_NUM(1);
        return VOS_ERR;
    }

    /* ���RABID */
    ulRabId = ADS_GET_RABID_FROM_EXRABID(ucExRabId);
    if (!ADS_IS_RABID_VALID(ulRabId))
    {
        ADS_DBG_UL_RMNET_RABID_NUM(1);
        return VOS_ERR;
    }

    /* ��pstData���뵽ucRabId��Ӧ�Ļ�������� */
    if (VOS_OK != ADS_UL_InsertQueue(ulInstance, pstImmZc, ulRabId))
    {
        ADS_DBG_UL_RMNET_ENQUE_FAIL_NUM(1);
        return VOS_ERR;
    }

    ADS_DBG_UL_RMNET_ENQUE_SUCC_NUM(1);
    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : ADS_UL_SendPacketEx
 ��������  : ���з���������չ�ӿ�, ʹ�øýӿڷ��͵�������ADS��¼����Ϣ, ����
             �������ݹ���ƥ��, �ýӿڱ����ADS_DL_RegFilterDataCallback���
             ʹ��, ֻ��ʹ��ADS_DL_RegFilterDataCallbackע������й��˻ص���,
             �������ݲ���Ҫ����ʹ�øýӿڼ�¼����Ϣ���й���
 �������  : pstImmZc  --- IMM����
             enIpType  --- IP����
             ucExRabId --- RABID [5, 15]
 �������  : ��
 �� �� ֵ  : VOS_OK/VOS_ERR
 ���ú���  :
 ��������  :
 �޸���ʷ      :
  1.��    ��   : 2013��6��3��
    ��    ��   : L47619
    �޸�����   : V3R3 Share-PDP��Ŀ�޸�

  2.��    ��   : 2016��6��17��
    ��    ��   : A00165503
    �޸�����   : DTS2016061504494: ���Ӱ���Ϊ0�ļ��

*****************************************************************************/
VOS_UINT32 ADS_UL_SendPacketEx(
    IMM_ZC_STRU                        *pstImmZc,
    ADS_PKT_TYPE_ENUM_UINT8             enIpType,
    VOS_UINT8                          ucExRabId
)
{
    ADS_DL_RAB_INFO_STRU                *pstDlRabInfo;
    VOS_UINT32                           ulInstance;
    VOS_UINT32                           ulRabId;

    /* �ж��Ƿ�Ϊ�����ݰ� */
    if (VOS_NULL_PTR == pstImmZc)
    {
        ADS_WARNING_LOG(ACPU_PID_ADS_UL, "ADS_UL_SendPacketEx: pstImmZc is null!");
        return VOS_ERR;
    }

    if (0 == IMM_ZcGetUsedLen(pstImmZc))
    {
        ADS_WARNING_LOG(ACPU_PID_ADS_UL, "ADS_UL_SendPacketEx: len is 0!");
        return VOS_ERR;
    }

    /* �������н�������ͳ�Ƹ��� */
    ADS_DBG_UL_RMNET_RX_PKT_NUM(1);

    /* ͳ�������������յ��������ֽ���������������ѯ */
    ADS_RECV_UL_PERIOD_PKT_NUM(pstImmZc->len);

    /* ���MODEMID */
    ulInstance = ADS_GET_MODEMID_FROM_EXRABID(ucExRabId);
    if (ulInstance >= ADS_INSTANCE_MAX_NUM)
    {
        ADS_DBG_UL_RMNET_MODEMID_ERR_NUM(1);
        return VOS_ERR;
    }

    /* ���RABID */
    ulRabId = ADS_GET_RABID_FROM_EXRABID(ucExRabId);
    if (!ADS_IS_RABID_VALID(ulRabId))
    {
        ADS_DBG_UL_RMNET_RABID_NUM(1);
        return VOS_ERR;
    }

    /* �ж��Ƿ��Ѿ�ע������й��˻ص���������ע���������Ҫ���й�����Ϣ����ȡ������ֱ�ӷ��ͱ��� */
    pstDlRabInfo = ADS_DL_GET_RAB_INFO_PTR(ulInstance, ulRabId);
    if (VOS_NULL_PTR != pstDlRabInfo->pRcvDlFilterDataFunc)
    {
        /* ���ù����������ݰ������� */
        ADS_FILTER_ProcUlPacket(pstImmZc, enIpType);
    }

    /* ��pstData���뵽ucRabId��Ӧ�Ļ�������� */
    if (VOS_OK != ADS_UL_InsertQueue(ulInstance, pstImmZc, ulRabId))
    {
        ADS_DBG_UL_RMNET_ENQUE_FAIL_NUM(1);
        return VOS_ERR;
    }

    ADS_DBG_UL_RMNET_ENQUE_SUCC_NUM(1);
    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : ADS_UL_GetInstanceNextQueueNode
 ��������  : ��ȡ����һ��ʵ���Ľ��
 �������  : VOS_UINT8                           ucInstanceIndex,
 �������  : VOS_UINT8                          *pucRabId
 �� �� ֵ  : IMM_ZC_STRU                        *pstNode
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��12��20��
    ��    ��   : ³��/l60609
    �޸�����   : �����ɺ���
  2.��    ��   : 2012��02��21��
    ��    ��   : f00179208
    �޸�����   : V7R1C50��Ŀ���������ȼ���������
  3.��    ��   : 2012��11��24��
    ��    ��   : l60609
    �޸�����   : DSDA Phase II:˫ʵ��
*****************************************************************************/
IMM_ZC_STRU* ADS_UL_GetInstanceNextQueueNode(
    VOS_UINT32                           ulInstanceIndex,
    VOS_UINT32                          *pulRabId,
    VOS_UINT8                           *puc1XorHrpdUlIpfFlag
)
{
    VOS_UINT32                          i;
    VOS_UINT32                         *pulCurIndex;
    ADS_UL_CTX_STRU                    *pstAdsUlCtx;
    IMM_ZC_STRU                        *pstNode;

    pstAdsUlCtx = ADS_GetUlCtx(ulInstanceIndex);

    pulCurIndex = &pstAdsUlCtx->ulAdsUlCurIndex;

    pstNode     = VOS_NULL_PTR;

    /* ���ȼ���ʽ */
    for (i = 0; i < ADS_RAB_NUM_MAX; i++)
    {
        /* ��Ϊ�����Ѿ����򣬵�ǰ������Ч�����������ж��ж���Ч
           ����������������Ч���У�������ͷ���� */
        if (VOS_NULL_PTR == ADS_UL_GET_QUEUE_LINK_INFO(ulInstanceIndex, *pulCurIndex))
        {
            i += ADS_RAB_NUM_MAX - (*pulCurIndex + 1U);

            *pulCurIndex = 0;

            continue;
        }

        /* ����Ϊ��Ч���е�������ʱ������������ */
        if (0 == ADS_UL_GET_QUEUE_LINK_INFO(ulInstanceIndex, *pulCurIndex)->qlen)
        {
            /* ������һ�����е�����ʱ���������м�¼����� */
            ADS_UL_CLR_RECORD_NUM_IN_WEIGHTED(ulInstanceIndex, *pulCurIndex);

            *pulCurIndex = (*pulCurIndex + 1) % ADS_RAB_NUM_MAX;

            continue;
        }

        /* �������ȼ��ȼ���Ӧ�ļ�Ȩ���������� */
        /* ���ȼ��ߵ��ȷ��� */
        if (ADS_UL_GET_RECORD_NUM_IN_WEIGHTED(ulInstanceIndex, *pulCurIndex) < ADS_UL_GET_QUEUE_PRI_WEIGHTED_NUM(ulInstanceIndex, *pulCurIndex))
        {
            /* ��ȡ����ͷ��� */
            pstNode = IMM_ZcDequeueHead(ADS_UL_GET_QUEUE_LINK_INFO(ulInstanceIndex, *pulCurIndex));

            /* ��ȡ�ý���RabId */
            *pulRabId = ADS_UL_GET_PRIO_QUEUE_INDEX(ulInstanceIndex, *pulCurIndex);

            /* ��ȡ�ýڵ��Ƿ���1X����HRPD�İ� */
            *puc1XorHrpdUlIpfFlag = ADS_UL_GET_1X_OR_HRPD_UL_IPF_FLAG(ulInstanceIndex, *pulCurIndex);

            /* �����м�¼������ 1*/
            ADS_UL_SET_RECORD_NUM_IN_WEIGHTED(ulInstanceIndex, *pulCurIndex, 1);

            /* ����Ѿ������걾���е����ȼ���Ȩ�����������ݣ��������¸����з������� */
            if (ADS_UL_GET_RECORD_NUM_IN_WEIGHTED(ulInstanceIndex, *pulCurIndex) == ADS_UL_GET_QUEUE_PRI_WEIGHTED_NUM(ulInstanceIndex, *pulCurIndex))
            {
                /* ������һ�����е�����ʱ���������м�¼����� */
                ADS_UL_CLR_RECORD_NUM_IN_WEIGHTED(ulInstanceIndex, *pulCurIndex);

                *pulCurIndex = (*pulCurIndex + 1) % ADS_RAB_NUM_MAX;
            }

            break;
        }
    }


    return pstNode;
}

/*****************************************************************************
 �� �� ��  : ADS_UL_GetNextQueueNode
 ��������  : DSDA Phase II:˫ʵ��
 �������  : VOS_UINT8 *pucRabId
 �������  : ��
 �� �� ֵ  : IMM_ZC_STRU*
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��11��24��
    ��    ��   : l60609
    �޸�����   : �����ɺ���

*****************************************************************************/
IMM_ZC_STRU* ADS_UL_GetNextQueueNode(
    VOS_UINT32                          *pulRabId,
    VOS_UINT32                          *pulInstanceIndex,
    VOS_UINT8                           *puc1XorHrpdUlIpfFlag
)
{
    ADS_CTX_STRU                        *pstAdsCtx = VOS_NULL_PTR;
    IMM_ZC_STRU                         *pstNode = VOS_NULL_PTR;
    VOS_UINT32                           i;
    VOS_UINT32                           ulCurInstanceIndex;

    pstAdsCtx = ADS_GetAllCtx();

    ulCurInstanceIndex = pstAdsCtx->ulAdsCurInstanceIndex;

    for (i = 0; i < ADS_INSTANCE_MAX_NUM; i++)
    {
        ulCurInstanceIndex = ulCurInstanceIndex % ADS_INSTANCE_MAX_NUM;

        pstNode = ADS_UL_GetInstanceNextQueueNode(ulCurInstanceIndex, pulRabId, puc1XorHrpdUlIpfFlag);

        if (VOS_NULL_PTR != pstNode)
        {
            break;
        }

        ulCurInstanceIndex++;
    }

    /* ����ʵ������������BDʱ��modem id */
    *pulInstanceIndex = ulCurInstanceIndex;

    /* ��¼�´δ��ĸ�ʵ����ȥ���� */
    pstAdsCtx->ulAdsCurInstanceIndex = (ulCurInstanceIndex + 1) % ADS_INSTANCE_MAX_NUM;

    return pstNode;
}

/*****************************************************************************
 �� �� ��  : ADS_UL_SaveIpfSrcMem
 ��������  : ������������IPF BD�󣬱�����Ҫ�ͷŵ�Դ�ڴ�
 �������  : const ADS_IPF_BD_BUFF_STRU         *pstIpfUlBdBuff
             VOS_UINT32                          ulSaveNum
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��10��06��
    ��    ��   : A00165503
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_VOID ADS_UL_SaveIpfSrcMem(
    const ADS_IPF_BD_BUFF_STRU         *pstIpfUlBdBuff,
    VOS_UINT32                          ulSaveNum
)
{
    VOS_UINT32                          ulCnt;

    if (ulSaveNum > IPF_ULBD_DESC_SIZE)
    {
        return;
    }

    for (ulCnt = 0; ulCnt < ulSaveNum; ulCnt++)
    {
        IMM_ZcQueueTail(ADS_UL_IPF_SRCMEM_FREE_QUE(), pstIpfUlBdBuff[ulCnt].pstPkt);
        ADS_DBG_UL_BDQ_SAVE_SRC_MEM_NUM(1);
    }

    return;
}

/*****************************************************************************
 �� �� ��  : ADS_UL_FreeIpfSrcMem
 ��������  : �ͷ�Դ�ڴ�����п��ͷŵ��Ѿ����óɹ���Դϵͳ�ڴ�
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��8��30��
    ��    ��   : l60609
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_VOID ADS_UL_FreeIpfSrcMem(VOS_VOID)
{
    IMM_ZC_STRU                        *pstImmZc = VOS_NULL_PTR;
    VOS_UINT32                          ulIdleBD;
    VOS_UINT32                          ulBusyBD;
    VOS_UINT32                          ulCanFree;
    VOS_UINT32                          ulQueCnt;
    VOS_UINT32                          ulCnt;

    /* que is empty */
    ulQueCnt = IMM_ZcQueueLen(ADS_UL_IPF_SRCMEM_FREE_QUE());
    if (0 == ulQueCnt)
    {
        return;
    }

    /* get busy bd num */
    ulIdleBD = mdrv_ipf_get_uldesc_num();
    ulBusyBD = IPF_ULBD_DESC_SIZE - ulIdleBD;
    if (ulQueCnt >= ulBusyBD)
    {
        ulCanFree = ulQueCnt - ulBusyBD;
    }
    else
    {
        ADS_ERROR_LOG3(ACPU_PID_ADS_UL, "ADS_UL_FreeIpfUlSrcMem: Buff Num Less IPF Busy BD Num. ulQueCnt, ulBusyBD, ulIdleBD", ulQueCnt, ulBusyBD, ulIdleBD);
        ADS_DBG_UL_BDQ_FREE_SRC_MEM_ERR(1);
        return;
    }

    /* free src mem */
    for (ulCnt = 0; ulCnt < ulCanFree; ulCnt++)
    {
        pstImmZc = IMM_ZcDequeueHead(ADS_UL_IPF_SRCMEM_FREE_QUE());
        if (VOS_NULL_PTR == pstImmZc)
        {
            break;
        }

        /* ��ˢCACHE, ���ͷŻ��� */
        ADS_IPF_UL_MEM_UNMAP(pstImmZc, IMM_ZcGetUsedLen(pstImmZc));
        IMM_ZcFreeAny(pstImmZc);
        ADS_DBG_UL_BDQ_FREE_SRC_MEM_NUM(1);
    }

    return;
}

/*****************************************************************************
 �� �� ��  : ADS_UL_ClearIpfSrcMem
 ��������  : �������IPF Src Mem
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��8��31��
    ��    ��   : l60609
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_VOID ADS_UL_ClearIpfSrcMem(VOS_VOID)
{
    IMM_ZC_STRU                      *pstImmZc = VOS_NULL_PTR;
    VOS_UINT32                        ulQueCnt;
    VOS_UINT32                        ulCnt;
    VOS_UINT32                        ulIpfUlBdNum;
    VOS_UINT32                        i;

    ulQueCnt = IMM_ZcQueueLen(ADS_UL_IPF_SRCMEM_FREE_QUE());
    if (0 == ulQueCnt)
    {
        return;
    }

    /* ���е�PDP��ȥ����󣬲���BD�Ѿ�ȫ�����У�����������ȫ�����꣬���������Դ�ڴ���� */
    for (i = 0; i < ADS_INSTANCE_MAX_NUM; i++)
    {
        if (VOS_FALSE == ADS_UL_CheckAllQueueEmpty(i))
        {
            return;
        }
    }

    ulIpfUlBdNum = mdrv_ipf_get_uldesc_num();
    /* ����BD���63�� */
    if (IPF_ULBD_DESC_SIZE != ulIpfUlBdNum)
    {
        return;
    }

    /*free src mem*/
    for (ulCnt = 0; ulCnt < ulQueCnt; ulCnt++)
    {
        pstImmZc = IMM_ZcDequeueHead(ADS_UL_IPF_SRCMEM_FREE_QUE());
        if (VOS_NULL_PTR == pstImmZc)
        {
            break;
        }

        /* ��ˢCACHE, ���ͷŻ��� */
        ADS_IPF_UL_MEM_UNMAP(pstImmZc, pstImmZc->len);
        IMM_ZcFreeAny(pstImmZc);
        ADS_DBG_UL_BDQ_FREE_SRC_MEM_NUM(1);
    }

    return;
}

/*****************************************************************************
 �� �� ��  : ADS_UL_GetBdFcHead
 ��������  : ��ȡIPF������
 �������  : VOS_UINT8                           ucInstanceIndex
             VOS_UINT8                           uc1XorHrpdUlIpfFlag
 �������  : ��
 �� �� ֵ  : VOS_UINT8
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��12��19��
    ��    ��   : Y00213812
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT8 ADS_UL_GetBdFcHead(
    VOS_UINT32                          ulInstanceIndex,
    VOS_UINT8                           uc1XorHrpdUlIpfFlag
)
{
    VOS_UINT8                           ucIpfFlterCh;
    if (VOS_TRUE == uc1XorHrpdUlIpfFlag)
    {
        ucIpfFlterCh = ADS_UL_IPF_1XHRPD;
    }
    else
    {
        ucIpfFlterCh = ulInstanceIndex;
    }

    return ucIpfFlterCh;
}

/*****************************************************************************
 �� �� ��  : ADS_UL_CalcBuffTime
 ��������  : �������ݻ���ʱ��
 �������  : ulBeginSlice --- ��ʼʱ��
             ulEndSlice   --- ����ʱ��
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��10��06��
    ��    ��   : A00165503
    �޸�����   : �����ɺ���
  2.��    ��   : 2016��08��26��
    ��    ��   : l00373346
    �޸�����   : �޸ļ��㷽��

*****************************************************************************/
VOS_UINT32 ADS_UL_CalcBuffTime(VOS_UINT32 ulBeginSlice, VOS_UINT32 ulEndSlice)
{
    if (ulEndSlice > ulBeginSlice)
    {
        return (ulEndSlice - ulBeginSlice);
    }
    else
    {
        return (VOS_NULL_DWORD - ulBeginSlice + ulEndSlice + 1);
    }
}


/*****************************************************************************
 �� �� ��  : ADS_UL_GetAccState
 ��������  : ADS��ȡ���ݰ��Ƿ��Ǹ����ȼ�����
 �������  : IMM_ZC_STRU *pstImmZc ��Ҫ���͵����ݰ�ָ��
 �������  : ��
 �� �� ֵ  : VOS_UINT8 �� 0 �����٣�����Ϊ����(Ŀǰ������Ϊ1)
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2017��07��12��
    ��    ��   : ���Ĺ� 00371705
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT8 ADS_UL_GetAccState(IMM_ZC_STRU *pstImmZc)
{
    struct sock                        *pstSk;
    if( VOS_NULL_PTR == pstImmZc )
    {
        return 0;
    }
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 23))
    pstSk = skb_to_full_sk(pstImmZc);
#else
    pstSk = pstImmZc->sk;
#endif
    if( VOS_NULL_PTR != pstSk )
    {
         return pstSk->acc_state;
    }
    return 0;

}

/*****************************************************************************
 �� �� ��  : ADS_UL_ConfigBD
 ��������  : ADS��������BDд��IPF
 �������  : VOS_UINT32 ulBdNum
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��12��7��
    ��    ��   : ³��/l60609
    �޸�����   : �����ɺ���

  2.��    ��   : 2015��9��22��
    ��    ��   : A00165503
    �޸�����   : DTS2015081008249: ADS���������г���, ��ֹϵͳ��������

*****************************************************************************/
VOS_VOID ADS_UL_ConfigBD(VOS_UINT32 ulBdNum)
{
    IPF_CONFIG_ULPARAM_S               *pstUlCfgParam = VOS_NULL_PTR;
    ADS_IPF_BD_BUFF_STRU               *pstUlBdBuff   = VOS_NULL_PTR;
    IMM_ZC_STRU                        *pstImmZc      = VOS_NULL_PTR;
    VOS_UINT32                          ulBeginSlice;
    VOS_UINT32                          ulEndSlice;
    VOS_UINT32                          ulTmp;
    VOS_UINT32                          ulCnt;
    VOS_INT32                           lRslt;
    VOS_UINT32                          ulInstance;
    VOS_UINT32                          ulRabId;
    VOS_UINT8                           uc1XorHrpdUlIpfFlag;

    ulEndSlice = VOS_GetSlice();

    for (ulCnt = 0; ulCnt < ulBdNum; ulCnt++)
    {
        pstImmZc = ADS_UL_GetNextQueueNode(&ulRabId, &ulInstance, &uc1XorHrpdUlIpfFlag);
        if (VOS_NULL_PTR == pstImmZc)
        {
            break;
        }

        pstUlBdBuff = ADS_UL_GET_BD_BUFF_PTR(ulCnt);
        pstUlBdBuff->pstPkt = pstImmZc;

        ulBeginSlice  = ADS_UL_GET_SLICE_FROM_IMM(pstImmZc);
        pstUlCfgParam = ADS_UL_GET_BD_CFG_PARA_PTR(ulCnt);
        pstUlCfgParam->Data    = (modem_phy_addr)virt_to_phys((VOS_VOID *)pstImmZc->data);
        pstUlCfgParam->u16Len       = (VOS_UINT16)pstImmZc->len;
        pstUlCfgParam->u16UsrField1 = (VOS_UINT16)ADS_UL_BUILD_BD_USER_FIELD_1(ulInstance, ulRabId);
        pstUlCfgParam->u32UsrField3 = (VOS_UINT32)ADS_UL_CalcBuffTime(ulBeginSlice, ulEndSlice);

        pstUlCfgParam->u32UsrField2 |= ADS_UL_GetAccState( pstImmZc );

        /* Attribute: �ж�ʹ�ܣ����˼Ӱ��ƣ����������modem0��0��modem1��1 */
        pstUlCfgParam->mode    = IPF_MODE_FILTERANDTRANS;
        pstUlCfgParam->fc_head = ADS_UL_GetBdFcHead(ulInstance, uc1XorHrpdUlIpfFlag);
        ADS_MNTN_RecULIpPktInfo(pstImmZc,
                                pstUlCfgParam->u16UsrField1,
                                pstUlCfgParam->u32UsrField2,
                                pstUlCfgParam->u32UsrField3
                               );

        /* ˢCAHCE */
        ADS_IPF_UL_MEM_MAP(pstImmZc, pstImmZc->len);
    }

    /* �����������ݸ���Ϊ0 */
    if (0 == ulCnt)
    {
        return;
    }

    /* ���һ��BD�����ж�ʹ�� */
    pstUlCfgParam = ADS_UL_GET_BD_CFG_PARA_PTR(ulCnt - 1);
    pstUlCfgParam->int_en = 1;
    /* ����IPF����BD */
    lRslt = mdrv_ipf_config_ulbd(ulCnt, ADS_UL_GET_BD_CFG_PARA_PTR(0));
    if (IPF_SUCCESS != lRslt)
    {
        /* IPF����ʧ��, ��Ҫ�ͷ�Դ�ڴ� */
        ulTmp = ulCnt;
        for (ulCnt = 0; ulCnt < ulTmp; ulCnt++)
        {
            pstUlBdBuff = ADS_UL_GET_BD_BUFF_PTR(ulCnt);
            IMM_ZcFreeAny(pstUlBdBuff->pstPkt);
            ADS_DBG_UL_BDQ_CFG_BD_FAIL_NUM(1);
        }

        ADS_DBG_UL_BDQ_CFG_IPF_FAIL_NUM(1);
        return;
    }

    /* �������õ�BDԴ�ڴ汣�浽Դ�ڴ���� */
    ADS_UL_SaveIpfSrcMem(ADS_UL_GET_BD_BUFF_PTR(0), ulCnt);
    ADS_DBG_UL_BDQ_CFG_BD_SUCC_NUM(ulCnt);
    ADS_DBG_UL_BDQ_CFG_IPF_SUCC_NUM(1);

    ADS_MNTN_ReportULPktInfo();

    ADS_UL_EnableRxWakeLockTimeout(ADS_UL_RX_WAKE_LOCK_TMR_LEN);
    return;
}

/*****************************************************************************
 �� �� ��  : ADS_UL_ProcLinkData
 ��������  : �ӻ��������ȡ���ݰ������������д��IPF
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��12��7��
    ��    ��   : ³��/l60609
    �޸�����   : �����ɺ���

  2.��    ��   : 2012��3��30��
    ��    ��   : l60609
    �޸�����   : DTS2012031206512��ʮ�λ�ȡ����BD����ʱ��

  3.��    ��   : 2012��4��12��
    ��    ��   : A00165503
    �޸�����   : DTS2012020708222: ADS�����Ż�

  4.��    ��   : 2012��4��27��
    ��    ��   : l60609
    �޸�����   : DTS2012041802250: ADS����10ms��ʱ

  5.��    ��   : 2012��8��31��
    ��    ��   : l60609
    �޸�����   : AP������Ŀ������BDǰ���ͷ�Դ�ڴ�����п��ͷŵ��ڴ�

*****************************************************************************/
VOS_VOID ADS_UL_ProcLinkData(VOS_VOID)
{
    VOS_UINT32                          ulAllUlQueueDataNum;
    VOS_UINT32                          ulIpfUlBdNum;
    VOS_UINT32                          ulSndBdNum;

    /* �������ʱ�е����� */
    for (;;)
    {
        /* ���ڸ�λ������ֱ���˳� */
        if (VOS_TRUE == ADS_GetUlResetFlag())
        {
            printk(KERN_ERR "\n ADS_UL_ProcLinkData in ccore reset \n");
            return;
        }


        /* ��ȡ���п��Է��͵�BD���� */
        ulIpfUlBdNum = mdrv_ipf_get_uldesc_num();
        if (0 == ulIpfUlBdNum)
        {
            ADS_DBG_UL_BDQ_CFG_IPF_HAVE_NO_BD_NUM(1);

            /* ���÷��ͽ�����־ */
            ADS_UL_SET_SENDING_FLAG(VOS_FALSE);

            /* ������ʱ���˳� */
            ADS_StartTimer(TI_ADS_UL_SEND, ADS_UL_GET_PROTECT_TIMER_LEN());
            break;
        }


        /* �������ڷ��ͱ�־ */
        ADS_UL_SET_SENDING_FLAG(VOS_TRUE);

        /* ��ȡ��ǰ���ж����е����ݰ����� */
        ulAllUlQueueDataNum = ADS_UL_GetAllQueueDataNum();

        /* ���㵱ǰ�ɷ��͵�BD��Ŀ */
        ulSndBdNum = PS_MIN(ulIpfUlBdNum, ulAllUlQueueDataNum);

        /* �ͷű����Դ�ڴ� */
        ADS_UL_FreeIpfSrcMem();

        /* ����BD��д��IPF */
        ADS_UL_ConfigBD(ulSndBdNum);

        /* ��ȡ��ǰ���ж����е����ݰ����� */
        ulAllUlQueueDataNum = ADS_UL_GetAllQueueDataNum();

        /* ��ǰ������û�����ݣ��˳����ȴ��´ζ����ɿձ�Ϊ�ǿմ��� */
        if (0 == ulAllUlQueueDataNum)
        {
            /* ���÷��ͽ�����־ */
            ADS_UL_SET_SENDING_FLAG(VOS_FALSE);
            break;
        }
        /* ��ǰ�����������ݣ�������Ҫ�����ܰ� */
        else if (ulAllUlQueueDataNum <= ADS_UL_SEND_DATA_NUM_THREDHOLD)
        {
            ADS_StartTimer(TI_ADS_UL_SEND, ADS_UL_GET_PROTECT_TIMER_LEN());

            /* ���÷��ͽ�����־ */
            ADS_UL_SET_SENDING_FLAG(VOS_FALSE);
            break;
        }
        else
        {
            continue;
        }
    }

    return;
}

/*****************************************************************************
 �� �� ��  : ADS_UL_ProcPdpStatusInd
 ��������  : ����PDP״̬�仯����
 �������  : MsgBlock *pMsg
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��1��7��
    ��    ��   : l60609
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 ADS_UL_ProcPdpStatusInd(
    ADS_PDP_STATUS_IND_STRU            *pstStatusInd
)
{
    VOS_UINT32                          ulInstanceIndex;
    ADS_CDS_IPF_PKT_TYPE_ENUM_UINT8     enPktType;
    ADS_QCI_TYPE_ENUM_UINT8             enPrio;

    enPktType                           = ADS_CDS_IPF_PKT_TYPE_IP;

    ulInstanceIndex                     = pstStatusInd->enModemId;

    enPrio                              = pstStatusInd->enQciType;

    /* RabId�Ϸ��Լ�� */
    if (!ADS_IS_RABID_VALID(pstStatusInd->ucRabId))
    {
        ADS_WARNING_LOG1(ACPU_PID_ADS_UL, "ADS_UL_ProcPdpStatusInd: ulRabId is ", pstStatusInd->ucRabId);
        return VOS_ERR;
    }

    /* ������������ȼ������޸�����PDP��QCIΪ��ͬ���ȼ������������㷨��������ʹ�ȼ����PDP���ȴ��� */
    if (VOS_FALSE == g_stAdsCtx.astAdsSpecCtx[ulInstanceIndex].stAdsUlCtx.stQueuePriNv.ulStatus)
    {
        enPrio = ADS_QCI_TYPE_QCI9_NONGBR;
    }

    if (ADS_PDP_PPP == pstStatusInd->enPdpType)
    {
        enPktType = ADS_CDS_IPF_PKT_TYPE_PPP;
    }

    /* ����PDP״̬�ֱ���д��� */
    switch(pstStatusInd->enPdpStatus)
    {
        /* PDP���� */
        case ADS_PDP_STATUS_ACT:

            /* ����������� */
            ADS_UL_CreateQueue(ulInstanceIndex, pstStatusInd->ucRabId, enPrio, enPktType, pstStatusInd->uc1XorHrpdUlIpfFlag);

            /* ��������ͳ�� */
            ADS_UL_StartDsFlowStats(ulInstanceIndex, pstStatusInd->ucRabId);

            break;

        /* PDP�޸� */
        case ADS_PDP_STATUS_MODIFY:

            /* ���޸ĵĶ�����Ϣ���µ����ж��й����� */
            ADS_UL_UpdateQueueInPdpModified(ulInstanceIndex, enPrio, pstStatusInd->ucRabId);

            break;

        /* PDPȥ���� */
        case ADS_PDP_STATUS_DEACT:

            /* ���ٻ������ */
            ADS_UL_DestroyQueue(ulInstanceIndex, pstStatusInd->ucRabId);

            /* �����е�PDP��ȥ��������Դ�ڴ���� */
            ADS_UL_ClearIpfSrcMem();

            /* ֹͣ����ͳ�� */
            ADS_UL_StopDsFlowStats(ulInstanceIndex, pstStatusInd->ucRabId);

            break;

        default:
            break;
    }

    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : ADS_UL_RcvAtPdpStatusInd
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
  2.��    ��   : 2012��02��20��
    ��    ��   : f00179208
    �޸�����   : V7R1C50��Ŀ������PDP״̬
  3.��    ��   : 2012��8��31��
    ��    ��   : l60609
    �޸�����   : AP������Ŀ���ͷ�Դ�ڴ����
  4.��    ��   : 2012��11��24��
    ��    ��   : l60609
    �޸�����   : DSDA Phase II:˫ʵ��
  5.��    ��   : 2013��05��22��
    ��    ��   : f00179208
    �޸�����   : V3R3 PPP PROJECT

  6.��    ��   : 2013��9��22��
    ��    ��   : A00165503
    �޸�����   : DTS2013092200927: ��ʱ��ʹ���Ż�
*****************************************************************************/
VOS_UINT32 ADS_UL_RcvTafPdpStatusInd(MsgBlock *pMsg)
{
    ADS_PDP_STATUS_IND_STRU            *pstPdpStatusInd;
    VOS_UINT32                          ulRslt;

    pstPdpStatusInd = (ADS_PDP_STATUS_IND_STRU *)pMsg;

    ulRslt = ADS_UL_ProcPdpStatusInd(pstPdpStatusInd);

    return ulRslt;
}

/*****************************************************************************
 �� �� ��  : ADS_UL_RcvCdsIpPacketMsg
 ��������  : ����ID_CDS_ADS_IP_PACKET_IND��Ϣ
 �������  : MsgBlock *pMsg
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��12��7��
    ��    ��   : ³��/l60609
    �޸�����   : �����ɺ���

  2.��    ��   : 2012��8��31��
    ��    ��   : l60609
    �޸�����   : AP������Ŀ��IMM�ӿ��޸�

  3.��    ��   : 2012��12��21��
    ��    ��   : l60609
    �޸�����   : DSDA Phase II
*****************************************************************************/
VOS_UINT32 ADS_UL_RcvCdsIpPacketMsg(MsgBlock *pMsg)
{
    VOS_UINT32                          ulResult;
    ADS_NDIS_DATA_IND_STRU             *pstAdsNdisDataInd;
    IMM_ZC_STRU                        *pstZcData;
    CDS_ADS_DATA_IND_STRU              *pstDataInd;
    VOS_CHAR                           *pstZcPutData;

    pstDataInd = (CDS_ADS_DATA_IND_STRU *)pMsg;

    /* ������Ϣ  */
    pstAdsNdisDataInd = (ADS_NDIS_DATA_IND_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                             ACPU_PID_ADS_UL,
                                             sizeof(ADS_NDIS_DATA_IND_STRU));

    if (VOS_NULL_PTR == pstAdsNdisDataInd)
    {
        return VOS_ERR;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstAdsNdisDataInd + VOS_MSG_HEAD_LENGTH,
               (VOS_SIZE_T)(sizeof(ADS_NDIS_DATA_IND_STRU) - VOS_MSG_HEAD_LENGTH),
               0x00,
               (VOS_SIZE_T)(sizeof(ADS_NDIS_DATA_IND_STRU) - VOS_MSG_HEAD_LENGTH));

    /* ��д��Ϣ���� */
    pstAdsNdisDataInd->ulReceiverPid  = PS_PID_APP_NDIS;
    pstAdsNdisDataInd->enMsgId        = ID_ADS_NDIS_DATA_IND;
    pstAdsNdisDataInd->enModemId      = pstDataInd->enModemId;
    pstAdsNdisDataInd->ucRabId        = pstDataInd->ucRabId;
    pstAdsNdisDataInd->enIpPacketType = pstDataInd->enIpPacketType;

    pstZcData = (IMM_ZC_STRU *)IMM_ZcStaticAlloc((VOS_UINT32)pstDataInd->usLen);

    if (VOS_NULL_PTR == pstZcData)
    {
        PS_MEM_FREE(ACPU_PID_ADS_UL, pstAdsNdisDataInd);

        return VOS_ERR;
    }

    /*�˲��費���٣�����ƫ������βָ��*/
    pstZcPutData = (VOS_CHAR *)IMM_ZcPut(pstZcData, pstDataInd->usLen);

    TAF_MEM_CPY_S(pstZcPutData, pstDataInd->usLen, pstDataInd->aucData, pstDataInd->usLen);

    pstAdsNdisDataInd->pstSkBuff = pstZcData;

    /* ����VOS����ԭ�� */
    ulResult = PS_SEND_MSG(ACPU_PID_ADS_UL, pstAdsNdisDataInd);

    if(VOS_OK != ulResult)
    {
        ADS_ERROR_LOG(ACPU_PID_ADS_UL, "ADS_UL_RcvCdsIpPacketMsg: Send Msg Fail!");

        IMM_ZcFreeAny(pstZcData);

        return VOS_ERR;
    }

    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : ADS_UL_RcvCcpuResetStartInd
 ��������  : ADS_UL�յ�ADS_UL��ϢID_CCPU_RESET_START_ADS_UL_TO_ADS_UL_NOTIFY�Ĵ���
 �������  : pMsg:��Ϣ�׵�ַ
 �������  : ��
 �� �� ֵ  : VOS_UINT32:VOS_OK
 ���ú���  :
 ��������  :

 �޸���ʷ     :
 1.��    ��   : 2013��04��15��
   ��    ��   : f00179208
   �޸�����   : �����ɺ���

 2.��    ��   : 2013��06��18��
   ��    ��   : l00198894
   �޸�����   : V3R3 Share-PDP��Ŀ��������

  3.��    ��   : 2015��9��1��
    ��    ��   : A00165503
    �޸�����   : DTS2015082300106: ���й�С��, A�˴���C�˵�����λ����������
*****************************************************************************/
VOS_UINT32 ADS_UL_RcvCcpuResetStartInd(
    MsgBlock                           *pstMsg
)
{
    VOS_UINT32                           ulInsIndex;
    VOS_UINT32                           ulRabIndex;
    VOS_UINT32                           ulTiIndex;
    ADS_CTX_STRU                        *pstAdsCtx = VOS_NULL_PTR;

    ADS_TRACE_HIGH("enter.\n");

    pstAdsCtx = ADS_GetAllCtx();

    /* ����������л������ */
    for (ulInsIndex = 0; ulInsIndex < ADS_INSTANCE_MAX_NUM; ulInsIndex++)
    {
        for (ulRabIndex = 0; ulRabIndex < ADS_RAB_ID_MAX + 1; ulRabIndex++)
        {
            ADS_UL_DestroyQueue(ulInsIndex, ulRabIndex);
        }
    }

    /* ���Դ�ڴ���� */
    ADS_UL_ClearIpfSrcMem();

    /* ֹͣ���������Ķ�ʱ�� */
    for (ulTiIndex = 0; ulTiIndex < ADS_MAX_TIMER_NUM; ulTiIndex++)
    {
        ADS_StopTimer(ACPU_PID_ADS_UL, ulTiIndex, ADS_TIMER_STOP_CAUSE_USER);
    }

    /* �������������� */
    ADS_ResetUlCtx();

    /* ����IPF��ص������� */
    ADS_ResetIpfCtx();

    /* ���õ�ǰʵ������ֵ */
    pstAdsCtx->ulAdsCurInstanceIndex = ADS_INSTANCE_INDEX_0;

    /* ����ADS Filter���������� */
    ADS_FILTER_Reset();

    /* �ͷ��ź�����ʹ�õ���API����������� */
    VOS_SmV(ADS_GetULResetSem());

    ADS_TRACE_HIGH("leave.\n");
    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : ADS_UL_RcvTiSendExpired
 ��������  : TI_ADS_UL_SEND��ʱ������
 �������  : VOS_UINT32                          ulParam,
             VOS_UINT32                          ulTimerName
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��01��25��
    ��    ��   : w00316404
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_VOID ADS_UL_RcvTiSendExpired(
    VOS_UINT32                          ulParam,
    VOS_UINT32                          ulTimerName
)
{
    ADS_UL_SndEvent(ADS_UL_EVENT_DATA_PROC);
    ADS_DBG_UL_10MS_TMR_TRIG_EVENT(1);
    return;
}

/*****************************************************************************
 �� �� ��  : ADS_UL_RcvTiDsFlowStatsExpired
 ��������  : ����ͳ�ƶ�ʱ����ʱ��������ADS�����е�ǰ������
 �������  : ulTimerName
             ulParam
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ     :
 1.��    ��   : 2012��1��10��
   ��    ��   : l60609
   �޸�����   : �����ɺ���

  2.��    ��   : 2012��4��12��
    ��    ��   : A00165503
    �޸�����   : DTS2012020708222: ADS�����Ż�

  3.��    ��   : 2012��11��24��
    ��    ��   : l60609
    �޸�����   : DSDA Phase II:˫ʵ��

  4.��    ��   : 2013��9��22��
    ��    ��   : A00165503
    �޸�����   : DTS2013092200927: ��ʱ��ʹ���Ż�

  5.��    ��   : 2015��5��21��
    ��    ��   : l00198894
    �޸�����   : TSTS
*****************************************************************************/
VOS_VOID ADS_UL_RcvTiDsFlowStatsExpired(
    VOS_UINT32                          ulTimerName,
    VOS_UINT32                          ulParam
)
{
    VOS_UINT32                          ulTaBytes;
    VOS_UINT32                          ulRate;

    /* ���û�����ж��д���, ����ͳ������ */
    if (VOS_FALSE == ADS_UL_IsAnyQueueExist())
    {
        ADS_NORMAL_LOG(ACPU_PID_ADS_DL, "ADS_UL_RcvTiDsFlowStatsExpired: no queue is exist!");
        return;
    }

    /* ��ȡ1����������ݸ��� */
    ulTaBytes = ADS_GET_DL_PERIOD_PKT_NUM();

    /* ÿ1���Ӽ���һ��,��λΪ:byte/s */
    ulRate = ulTaBytes;
    ADS_SET_CURRENT_DL_RATE(ulRate);

    /* ��ȡ1����������� */
    ulTaBytes = ADS_GET_UL_PERIOD_PKT_NUM();

    /* ÿ1���Ӽ���һ��,��λΪ:byte/s */
    ulRate = ulTaBytes;
    ADS_SET_CURRENT_UL_RATE(ulRate);

    /* ÿ������ͳ�����ڽ�������Ҫ������ͳ��Byte����� */
    ADS_CLEAR_UL_PERIOD_PKT_NUM();
    ADS_CLEAR_DL_PERIOD_PKT_NUM();

    /* ��������ͳ�ƶ�ʱ��*/
    ADS_StartTimer(TI_ADS_DSFLOW_STATS, TI_ADS_DSFLOW_STATS_LEN);

    return ;
}

/*****************************************************************************
 �� �� ��  : ADS_UL_RcvTiStatExpired
 ��������  : ����ͳ�ƶ�ʱ����ʱ����
 �������  : VOS_UINT32    ulParam,
             VOS_UINT32    ulTimerName
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ     :
 1.��    ��   : 2014��06��03��
   ��    ��   : f00179208
   �޸�����   : �����ɺ���
*****************************************************************************/
VOS_VOID ADS_UL_RcvTiDataStatExpired(
    VOS_UINT32                          ulTimerName,
    VOS_UINT32                          ulParam
)
{
    VOS_UINT32                          ulStatPktNum;

    ulStatPktNum = ADS_UL_GET_STAT_PKT_NUM();

    /* �������ݰ����������ް����� */
    if (ulStatPktNum < ADS_UL_GET_WATER_LEVEL_ONE())
    {
        ADS_UL_SET_SEND_DATA_NUM_THREDHOLD(ADS_UL_DATA_THRESHOLD_ONE);
        ADS_DBG_UL_WM_LEVEL_1_HIT_NUM(1);
    }
    else if (ulStatPktNum <  ADS_UL_GET_WATER_LEVEL_TWO())
    {
        ADS_UL_SET_SEND_DATA_NUM_THREDHOLD(ADS_UL_DATA_THRESHOLD_TWO);
        ADS_DBG_UL_WM_LEVEL_2_HIT_NUM(1);
    }
    else if (ulStatPktNum <  ADS_UL_GET_WATER_LEVEL_THREE())
    {
        ADS_UL_SET_SEND_DATA_NUM_THREDHOLD(ADS_UL_DATA_THRESHOLD_THREE);
        ADS_DBG_UL_WM_LEVEL_3_HIT_NUM(1);
    }
    else
    {
        ADS_UL_SET_SEND_DATA_NUM_THREDHOLD(ADS_UL_DATA_THRESHOLD_FOUR);
        ADS_DBG_UL_WM_LEVEL_4_HIT_NUM(1);
    }

    /* 100ms��û�����ݰ���ö�ʱ���������� */
    if (0 != ulStatPktNum)
    {
        /* ������������ͳ�ƶ�ʱ�� */
        ADS_StartTimer(TI_ADS_UL_DATA_STAT, ADS_UL_GET_STAT_TIMER_LEN());
    }

    /* ���ͳ�ư��ĸ��� */
    ADS_UL_CLR_STAT_PKT_NUM();

    return;
}

/*****************************************************************************
 �� �� ��  : ADS_UL_RcvTafMsg
 ��������  : ��������AT����Ϣ
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
VOS_UINT32 ADS_UL_RcvTafMsg(MsgBlock *pMsg)
{
    MSG_HEADER_STRU                    *pstMsg;

    pstMsg = (MSG_HEADER_STRU*)pMsg;


    switch(pstMsg->ulMsgName)
    {
        case ID_APS_ADS_PDP_STATUS_IND:
            ADS_UL_RcvTafPdpStatusInd(pMsg);
            break;

        default:
            break;
    }

    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : ADS_UL_RcvCdsMsg
 ��������  : ��������CDS����Ϣ
 �������  : MsgBlock *pMsg
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��12��6��
    ��    ��   : ³��/l60609
    �޸�����   : �����ɺ���

  2.��    ��   : 2015��6��16��
    ��    ��   : l00198894
    �޸�����   : TSTS
*****************************************************************************/
VOS_UINT32 ADS_UL_RcvCdsMsg(MsgBlock *pMsg)
{
    MSG_HEADER_STRU                    *pstMsg;

    pstMsg = (MSG_HEADER_STRU*)pMsg;

    switch(pstMsg->ulMsgName)
    {
        case ID_CDS_ADS_IP_PACKET_IND:
            ADS_UL_RcvCdsIpPacketMsg(pMsg);
            break;

        default:
            break;
    }

    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : ADS_UL_RcvTimerMsg
 ��������  : ����ʱ����ʱ��Ϣ
 �������  : MsgBlock *pMsg
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��12��7��
    ��    ��   : ³��/l60609
    �޸�����   : �����ɺ���

  2.��    ��   : 2013��9��22��
    ��    ��   : A00165503
    �޸�����   : DTS2013092200927: ��ʱ��ʹ���Ż�

  3.��    ��   : 2014��06��03��
    ��    ��   : f00179208
    �޸�����   : CCPU LOAD�Ż�

  4.��    ��   : 2015��5��21��
    ��    ��   : l00198894
    �޸�����   : TSTS

  5.��    ��   : 2015��9��22��
    ��    ��   : A00165503
    �޸�����   : DTS2015081008249: ADS���������г���, ��ֹϵͳ��������

*****************************************************************************/
VOS_UINT32 ADS_UL_RcvTimerMsg(MsgBlock *pMsg)
{
    REL_TIMER_MSG                      *pstTimerMsg;

    pstTimerMsg = (REL_TIMER_MSG *)pMsg;

    /* ֹͣ�ö�ʱ�� */
    ADS_StopTimer(ACPU_PID_ADS_UL, pstTimerMsg->ulName, ADS_TIMER_STOP_CAUSE_TIMEOUT);

    switch (pstTimerMsg->ulName)
    {
        case TI_ADS_DSFLOW_STATS:
            ADS_UL_RcvTiDsFlowStatsExpired(pstTimerMsg->ulName, pstTimerMsg->ulPara);
            ADS_MNTN_ReportAllStatsInfo();
            ADS_RNIC_AdjNapiWeight();
            break;

        case TI_ADS_UL_DATA_STAT:
            ADS_UL_RcvTiDataStatExpired(pstTimerMsg->ulName, pstTimerMsg->ulPara);
            break;

        default:
            break;
    }

    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : ADS_UL_RcvAdsUlMsg
 ��������  : ��������ADS UL����Ϣ
 �������  : MsgBlock *pMsg
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��04��15��
    ��    ��   : f00179208
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 ADS_UL_RcvAdsUlMsg(MsgBlock *pMsg)
{
    MSG_HEADER_STRU                    *pstMsg;

    pstMsg = (MSG_HEADER_STRU*)pMsg;

    switch(pstMsg->ulMsgName)
    {
        case ID_ADS_CCPU_RESET_START_IND:
            ADS_UL_RcvCcpuResetStartInd(pMsg);
            break;

        case ID_ADS_CCPU_RESET_END_IND:
            /* do nothing */
            ADS_NORMAL_LOG(ACPU_PID_ADS_UL, "ADS_DL_RcvAdsDlMsg: rcv ID_CCPU_ADS_UL_RESET_END_IND");
            break;

        default:
            ADS_NORMAL_LOG1(ACPU_PID_ADS_UL, "ADS_UL_RcvAdsUlMsg: rcv error msg id %d\r\n", pstMsg->ulMsgName);
            break;
    }

    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : ADS_UL_ProcMsg
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
  2.��    ��   : 2012��4��12��
    ��    ��   : A00165503
    �޸�����   : DTS2012020708222: ADS�����Ż�
  3.��    ��   : 2013��4��15��
    ��    ��   : f00179208
    �޸�����   : C�˵�����λ��Ŀ
  4.��    ��   : 2014��02��14��
    ��    ��   : m00217266
    �޸�����   : pdp status ind ��Ϊtaf aps����
  5.��    ��   : 2015��5��21��
    ��    ��   : l00198894
    �޸�����   : TSTS
*****************************************************************************/
VOS_VOID ADS_UL_ProcMsg(MsgBlock* pMsg)
{
    if (VOS_NULL_PTR == pMsg)
    {
        return;
    }

    /* ��Ϣ�ķַ����� */
    switch ( pMsg->ulSenderPid )
    {
        /* ����Timer����Ϣ */
        case VOS_PID_TIMER:
            ADS_UL_RcvTimerMsg(pMsg);
            return;

        /* ����TAF����Ϣ */
        case I0_WUEPS_PID_TAF:
        case I1_WUEPS_PID_TAF:
        case I2_WUEPS_PID_TAF:
            ADS_UL_RcvTafMsg(pMsg);
            return;

        /* ����CDS����Ϣ */
        case UEPS_PID_CDS:
            ADS_UL_RcvCdsMsg(pMsg);
            return;

        /* ����ADS UL����Ϣ */
        case ACPU_PID_ADS_UL:
            ADS_UL_RcvAdsUlMsg(pMsg);
            return;

        default:
            return;
    }
}

