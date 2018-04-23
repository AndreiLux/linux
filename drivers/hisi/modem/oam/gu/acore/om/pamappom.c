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

/************************************************************************
  Copyright    : 2005-2007, Huawei Tech. Co., Ltd.
  File name    : UsimmBase.c
  Author       : zhuli 00100318
  Version      : v00R002
  Date         : 2008-5-15
  Description  : ��C�ļ�������---Baseģ��ʵ��
  Function List:
  History      :
************************************************************************/
#include "pamappom.h"
#include "omprivate.h"
#include "NVIM_Interface.h"
#include "CbtPpm.h"
#include "msp_nvim.h"
#include "AtOamInterface.h"
#include "PsLogFilterInterface.h"


/*****************************************************************************
    Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
*****************************************************************************/
#define    THIS_FILE_ID PS_FILE_ID_PAM_APP_OM_C


/* ��¼�յ���Ϣ��Ϣ��buffer����ǰ���� */
OM_RECORD_BUF_STRU                      g_astAcpuRecordInfo[VOS_EXC_DUMP_MEM_NUM_BUTT];

VOS_UINT32                              g_ulAcpuOmFilterFlag;


/*****************************************************************************
 Prototype       : OM_RecordInfoEnd
 Description     : A�˱���׮����
 Input           : ulNumer -- �����
 Output          : None
 Return Value    : VOS_VOID

 History         : ---
    Date         : 2012-07-09
    Author       : s00207770
    Modification : Created function
 *****************************************************************************/
VOS_VOID OM_RecordInfoEnd(VOS_EXC_DUMP_MEM_NUM_ENUM_UINT32 enNumber)
{
    VOS_UINT32 *pulBuf;

    if(VOS_EXC_DUMP_MEM_NUM_BUTT <= enNumber)
    {
        return;
    }

    if(VOS_NULL_PTR == g_astAcpuRecordInfo[enNumber].pucBuf)
    {
        return;
    }

    if(0 == g_astAcpuRecordInfo[enNumber].ulLen)
    {
        return;
    }

    /* ��start���Ѿ�����˼�¼endslice�ĳ��ȣ���˴˴������ĸ��ֽ���дendslice��ֵ */
    pulBuf = (VOS_UINT32*)(g_astAcpuRecordInfo[enNumber].pucBuf + g_astAcpuRecordInfo[enNumber].ulLen - sizeof(VOS_UINT32));

    *pulBuf = VOS_GetSlice();

    return;
}

/*****************************************************************************
 Prototype       : OM_RecordInfoStart
 Description     : A�˱���׮����
 Input           : ulNumer -- �����
                   ulSendPid -- ����PID
                   ulRcvPid -- ����PID
                   ulMsgName -- ��Ϣ����
 Output          : None
 Return Value    : VOS_VOID

 History         : ---
    Date         : 2012-07-09
    Author       : s00702770
    Modification : Created function
 *****************************************************************************/
VOS_VOID OM_RecordInfoStart(VOS_EXC_DUMP_MEM_NUM_ENUM_UINT32 enNumber, VOS_UINT32 ulSendPid, VOS_UINT32 ulRcvPid, VOS_UINT32 ulMsgName)
{
    OM_RECORD_INFO_STRU     stRecordInfo;

    if(VOS_EXC_DUMP_MEM_NUM_BUTT <= enNumber)
    {
       return;
    }

    if(VOS_NULL_PTR == g_astAcpuRecordInfo[enNumber].pucBuf)
    {
       return;
    }

    g_astAcpuRecordInfo[enNumber].ulLen %= VOS_TASK_DUMP_INFO_SIZE;

    stRecordInfo.usSendPid      = (VOS_UINT16)ulSendPid;
    stRecordInfo.usRcvPid       = (VOS_UINT16)ulRcvPid;
    stRecordInfo.ulMsgName      = ulMsgName;
    stRecordInfo.ulSliceStart   = VOS_GetSlice();
    stRecordInfo.ulSliceEnd     = 0;

    PAM_MEM_CPY_S((g_astAcpuRecordInfo[enNumber].pucBuf + g_astAcpuRecordInfo[enNumber].ulLen),
                  sizeof(OM_RECORD_INFO_STRU),
                  &stRecordInfo,
                  sizeof(OM_RECORD_INFO_STRU));

    g_astAcpuRecordInfo[enNumber].ulLen += (VOS_UINT32)sizeof(OM_RECORD_INFO_STRU);

    return;
}

/*****************************************************************************
 Prototype       : OM_RecordMemInit
 Description     : ��ν�ɲ�ռ����
 Input           : None
 Output          : None
 Return Value    : VOS_VOID

 History         : ---
    Date         : 2012-06-28
    Author       : j00168360
    Modification : Created function
 *****************************************************************************/
VOS_VOID OM_RecordMemInit(VOS_VOID)
{
   VOS_UINT32 i;

   PAM_MEM_SET_S(g_astAcpuRecordInfo,
                 sizeof(g_astAcpuRecordInfo),
                 0,
                 sizeof(g_astAcpuRecordInfo));

   /* ����ÿ��ģ���¼��ν�ɲ���Ϣ�Ŀռ� */
   for(i = 0; i < VOS_EXC_DUMP_MEM_NUM_BUTT; i++)
   {
      g_astAcpuRecordInfo[i].pucBuf = (VOS_UINT8*)VOS_ExcDumpMemAlloc(i);

      if(VOS_NULL_PTR == g_astAcpuRecordInfo[i].pucBuf)
      {
          return;
      }
   }

   return;
}

VOS_VOID PAMOM_AcpuTimerMsgProc(MsgBlock* pMsg)
{
    return;
}

VOS_VOID PAMOM_QuereyPidInfo(VOS_VOID)
{
    PAM_VOS_QUEREY_PID_INFO_REQ_STRU    *pstMsg;

    pstMsg = (PAM_VOS_QUEREY_PID_INFO_REQ_STRU *)VOS_AllocMsg(ACPU_PID_PAM_OM,
                            sizeof(PAM_VOS_QUEREY_PID_INFO_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    /* ������Ϣʧ�� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return;
    }

    pstMsg->ulReceiverPid  = CCPU_PID_PAM_OM;
    pstMsg->usPrimId       = PAM_VOS_QUEREY_PID_INFO_REQ;

    (VOS_VOID)VOS_SendMsg(ACPU_PID_PAM_OM, pstMsg);

    return;
}


VOS_VOID PAMOM_QuereyPidInfoMsgProc(MsgBlock* pMsg)
{
    PAM_VOS_QUEREY_PID_INFO_REQ_STRU    *pstMsg;
    PAM_VOS_QUEREY_PID_INFO_CNF_STRU    *pstCnfMsg;
    VOS_UINT32                           ulLen;

    pstMsg = (PAM_VOS_QUEREY_PID_INFO_REQ_STRU *)pMsg;

    if (PAM_VOS_QUEREY_PID_INFO_REQ == pstMsg->usPrimId)
    {
        ulLen = VOS_QueryPidInfoBufSize();

        pstCnfMsg = (PAM_VOS_QUEREY_PID_INFO_CNF_STRU *)VOS_AllocMsg(ACPU_PID_PAM_OM,
                            sizeof(PAM_VOS_QUEREY_PID_INFO_CNF_STRU) - VOS_MSG_HEAD_LENGTH + ulLen);

        /* ������Ϣʧ�� */
        if (VOS_NULL_PTR == pstCnfMsg)
        {
            return;
        }

        pstCnfMsg->ulReceiverPid  = CCPU_PID_PAM_OM;
        pstCnfMsg->usPrimId       = PAM_VOS_QUEREY_PID_INFO_CNF;
        pstCnfMsg->usLen          = (VOS_UINT16)ulLen;
        VOS_QueryPidInfo((VOS_VOID *)pstCnfMsg->aucValue);

        (VOS_VOID)VOS_SendMsg(ACPU_PID_PAM_OM, pstCnfMsg);
    }
    else if (PAM_VOS_QUEREY_PID_INFO_CNF == pstMsg->usPrimId)
    {
        pstCnfMsg = (PAM_VOS_QUEREY_PID_INFO_CNF_STRU *)pMsg;

        VOS_SetPidInfo((VOS_VOID *)(pstCnfMsg->aucValue), pstCnfMsg->usLen);
    }
    else
    {
        /* blank */
    }

    return;
}

/*****************************************************************************
 �� �� ��  : PAM_OM_AcpuPihToAtMsgFilter
 ��������  : ����pih��at ��Ϣ�ĺ��� CGLA CRSM CRLA
 �������  : pstMsg -- ��Ϣ����
 �������  : NONE
 �� �� ֵ  : VOS_TRUE :��ʾ�����Ϣ�����˵��ˣ�����Ҫ�ϱ���OM
             VOS_FALSE:��ʾ�����Ϣ��Ҫ�ϱ�OM
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��08��29��
    ��    ��   : z00316370
    �޸�����   : �����ɺ���
*****************************************************************************/
VOS_UINT32 PAM_OM_AcpuPihToAtMsgFilter(
    const VOS_VOID                      *pMsg
)
{
    MN_APP_PIH_AT_CNF_STRU             *pstEventCnf;

    pstEventCnf = (MN_APP_PIH_AT_CNF_STRU *)pMsg;

    if (PIH_AT_EVENT_CNF != pstEventCnf->ulMsgId)
    {
        return VOS_FALSE;
    }

    switch (pstEventCnf->stPIHAtEvent.EventType)
    {
        case SI_PIH_EVENT_CGLA_SET_CNF:
        case SI_PIH_EVENT_CRSM_SET_CNF:
        case SI_PIH_EVENT_CRLA_SET_CNF:
            OM_NORMAL_LOG1("PAM_OM_AcpuAtToPihMsgFilter: The filter EventType is :", pstEventCnf->stPIHAtEvent.EventType);

            return VOS_TRUE;

        default:
            return VOS_FALSE;
    }
}

/*****************************************************************************
 �� �� ��  : PAM_OM_AcpuAtToPihMsgFilter
 ��������  : ����at��pih ��Ϣ�ĺ��� CGLA CRSM CRLA
 �������  : pstMsg -- ��Ϣ����
 �������  : NONE
 �� �� ֵ  : VOS_TRUE :��ʾ�����Ϣ�����˵��ˣ�����Ҫ�ϱ���OM
             VOS_FALSE:��ʾ�����Ϣ��Ҫ�ϱ�OM
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��08��29��
    ��    ��   : z00316370
    �޸�����   : �����ɺ���
*****************************************************************************/
VOS_UINT32 PAM_OM_AcpuAtToPihMsgFilter(
    const VOS_VOID                      *pMsg
)
{
    MSG_HEADER_STRU                     *pstMsg;

    pstMsg = (MSG_HEADER_STRU *)pMsg;

    switch (pstMsg->ulMsgName)
    {
        case OM_SI_PIH_CRSM_SET_REQ:
        case OM_SI_PIH_CRLA_SET_REQ:
        case OM_SI_PIH_CGLA_SET_REQ:
            OM_NORMAL_LOG1("PAM_OM_AcpuAtToPihMsgFilter: The filter ulMsgName is :", pstMsg->ulMsgName);

            return VOS_TRUE;

        default:
            break;
    }

    return VOS_FALSE;
}

/*****************************************************************************
 �� �� ��  : PAM_OM_AcpuLayerMsgFilterOptProc
 ��������  : ����PAM�е�������Ϣ
 �������  : const VOID *pMsg:��Ϣָ��
 �������  : ��
 �� �� ֵ  : VOS_UINT32:
                ����VOS_TRUE:  ��ʾ����Ϣ��Ҫ���й���
                ����VOS_FALSE: ��ʾ����Ϣ������й���
 ���ú���  :
 ��������  :
 �޸���ʷ      :
  1.��    ��   : 2015��10��28��
    ��    ��   : zhuli
    �޸�����   : �����ɺ���
*****************************************************************************/
VOS_UINT32 PAM_OM_AcpuLayerMsgFilterOptProc(
    const VOS_VOID                      *pMsg
)
{
    OM_FILTER_MSG_HEAD_STRU             *pstMsgHead;

    if (VOS_FALSE == g_ulAcpuOmFilterFlag)
    {
        return VOS_FALSE;
    }

    pstMsgHead = (OM_FILTER_MSG_HEAD_STRU*)pMsg;

     /* PIH ��Ϣ���� */
    if ( (I0_MAPS_PIH_PID == pstMsgHead->ulSenderPid)
      || (I1_MAPS_PIH_PID == pstMsgHead->ulSenderPid)
      || (I2_MAPS_PIH_PID == pstMsgHead->ulSenderPid))
    {
        return PAM_OM_AcpuPihToAtMsgFilter(pMsg);
    }

    if (WUEPS_PID_AT == pstMsgHead->ulSenderPid)
    {
        return PAM_OM_AcpuAtToPihMsgFilter(pMsg);
    }

    return VOS_FALSE;
}

/*****************************************************************************
 �� �� ��  : PAM_OM_LayerMsgFilter
 ��������  : PAM OM��Ϣ���˺���
 �������  : NONE
 �������  : NONE
 �� �� ֵ  : NA
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2017��04��06��
    ��    ��   : z00316370
    �޸�����   : �����ɺ���
*****************************************************************************/
VOS_VOID * PAM_OM_LayerMsgFilter(
    struct MsgCB                       *pMsg
)
{
    if (VOS_TRUE == PAM_OM_AcpuLayerMsgFilterOptProc(pMsg))
    {
        return VOS_NULL_PTR;
    }

    return pMsg;

}

/*****************************************************************************
 �� �� ��  : PAM_OM_LayerMsgReplaceCBReg
 ��������  : PAM OMΪ��pidע����˺���
 �������  : NONE
 �������  : NONE
 �� �� ֵ  : NA
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2017��04��06��
    ��    ��   : z00316370
    �޸�����   : �����ɺ���
*****************************************************************************/
VOS_VOID PAM_OM_LayerMsgReplaceCBReg(VOS_VOID)
{
    PS_OM_LayerMsgReplaceCBReg(WUEPS_PID_AT, PAM_OM_LayerMsgFilter);
    PS_OM_LayerMsgReplaceCBReg(I0_MAPS_PIH_PID, PAM_OM_LayerMsgFilter);

    PS_OM_LayerMsgReplaceCBReg(I1_MAPS_PIH_PID, PAM_OM_LayerMsgFilter);
    PS_OM_LayerMsgReplaceCBReg(I2_MAPS_PIH_PID, PAM_OM_LayerMsgFilter);

}



 VOS_VOID PAMOM_AcpuCcpuPamMsgProc(MsgBlock* pMsg)
 {
    VOS_UINT16                          usPrimId;

    usPrimId = *(VOS_UINT16 *)(pMsg->aucValue);

    if (PAM_VOS_QUEREY_PID_INFO_REQ == usPrimId)
    {
        PAMOM_QuereyPidInfoMsgProc(pMsg);
    }
    else if (PAM_VOS_QUEREY_PID_INFO_CNF == usPrimId)
    {
        PAMOM_QuereyPidInfoMsgProc(pMsg);
    }
    else
    {

    }

    return;
}

/*****************************************************************************
 Prototype       : CBT_AppMsgProc
 Description     : Handle all messages sent to OM.
 Input           : pMsg -- The pointer of the msg.
 Output          : None
 Return Value    : VOS_VOID

 History         : ---
    Date         : 2008-03-20
    Author       : g47350
    Modification : Created function
 *****************************************************************************/
VOS_VOID PAMOM_AppMsgProc(MsgBlock* pMsg)
{
    if (VOS_PID_TIMER == pMsg->ulSenderPid)
    {
        PAMOM_AcpuTimerMsgProc(pMsg);
    }
    else if (CCPU_PID_PAM_OM == pMsg->ulSenderPid)
    {
        PAMOM_AcpuCcpuPamMsgProc(pMsg);
    }
    else
    {
        /* blank */
    }

    return;
}

/*****************************************************************************
 Prototype       : OM_AcpuInit
 Description     : OM' initializtion function
 Input           : None
 Output          : None
 Return Value    : VOS_UINT32

 History         : ---
    Date         : 2011-07-01
    Author       : g47350
    Modification : Created function
 *****************************************************************************/
VOS_UINT32 PAMOM_AcpuInit(VOS_VOID)
{
    NAS_NV_PRIVACY_FILTER_CFG_STRU      stPrivacyFilterCfg  = {0};

    PAM_OM_LayerMsgReplaceCBReg();

    PAMOM_QuereyPidInfo();


    g_ulAcpuOmFilterFlag = VOS_TRUE;

    return VOS_OK;
}

/*****************************************************************************
 Prototype       : OM_AcpuPidInit
 Description     : ACPU OM PID' initializtion function
 Input           : None
 Output          : None
 Return Value    : VOS_UINT32

 History         : ---
    Date         : 2011-07-01
    Author       : g47350
    Modification : Created function
 *****************************************************************************/
VOS_UINT32 PAMOM_AppPidInit(enum VOS_INIT_PHASE_DEFINE ip)
{
    switch( ip )
    {
        case VOS_IP_LOAD_CONFIG:
            return PAMOM_AcpuInit();

        default:
            break;
    }

    return VOS_OK;
}

/*****************************************************************************
 Prototype       : OM_AcpuFidInit
 Description     : ACPU OM FID' initializtion function
 Input           : None
 Output          : None
 Return Value    : VOS_UINT32

 History         : ---
    Date         : 2011-07-01
    Author       : g47350
    Modification : Created function
 *****************************************************************************/
VOS_UINT32 PAMOM_APP_FID_Init(enum VOS_INIT_PHASE_DEFINE ip)
{
    VOS_UINT32                          ulRslt;

    switch( ip )
    {
        case VOS_IP_LOAD_CONFIG:
        {
            ulRslt = VOS_RegisterPIDInfo(ACPU_PID_PAM_OM,
                                        (Init_Fun_Type)PAMOM_AppPidInit,
                                        (Msg_Fun_Type)PAMOM_AppMsgProc);
            if( VOS_OK != ulRslt )
            {
                return VOS_ERR;
            }

            ulRslt = VOS_RegisterMsgTaskPrio(ACPU_FID_PAM_OM, VOS_PRIORITY_M2);
            if( VOS_OK != ulRslt )
            {
                return VOS_ERR;
            }

            /* ��Ŀ¼�������򴴽� */
            if (VOS_OK != mdrv_file_access(PAM_LOG_PARTH, PAM_FILE_EXIST))
            {
                (VOS_VOID)mdrv_file_mkdir(PAM_LOG_PARTH);
            }

            break;
        }

        default:
            break;
    }
    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : OM_OSAEvent
 ��������  : �ϱ�OSA�Ŀ�ά�ɲ���Ϣ
 �������  :
 �������  : ��
 �� �� ֵ  : ��

 �޸���ʷ      :
  1.��    ��   : 2013��7��27��
    ��    ��   : x51137
    �޸�����   : �����ɺ���
*****************************************************************************/
VOS_VOID OM_OSAEvent(VOS_VOID *pData, VOS_UINT32 ulLength)
{
    DIAG_EVENT_IND_STRU                 stEventInd;

    stEventInd.ulModule = DIAG_GEN_MODULE(DIAG_MODEM_0, DIAG_MODE_COMM);
    stEventInd.ulPid    = ACPU_PID_PAM_OM;
    stEventInd.ulEventId= OAM_EVENT_TIMER;
    stEventInd.ulLength = ulLength;
    stEventInd.pData    = pData;

    (VOS_VOID)DIAG_EventReport(&stEventInd);

    return;
}


/* AT<->AT�����δ����Ƶ�GuNasLogFilter.c */


