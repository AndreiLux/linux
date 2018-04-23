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
#include "vos.h"
#include "PsCommonDef.h"
#include "AtMnInterface.h"
#include "MnCallApi.h"
#include  "product_config.h"
#include "MnErrorCode.h"
#include "AtParse.h"
#include "ATCmdProc.h"

#include "TafAppCall.h"


#define    THIS_FILE_ID        PS_FILE_ID_MNCALL_API_C

/*****************************************************************************
   2 ����ʵ��
*****************************************************************************/

/*****************************************************************************
 �� �� ��  : MN_CALL_SendAppRequest
 ��������  : ��APP���첽�����͵�CCA����������
 �������  : enReq    - ���������
              clientId - Client ID
              opId     - ���β����ı�ʶ
              callId   - ���е�ID
              punParam - ������Я���Ĳ���
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2007��9��20��
    ��    ��   : ���� 49431
    �޸�����   : �����ɺ���
  2.��    ��   : 2011��11��07��
    ��    ��   : f00179208
    �޸�����   : AT Project, �޸�AT�����WUEPS_PID_VCΪWUEPS_PID_AT
  3.��    ��   : 2012��9��27��
    ��    ��   : A00165503
    �޸�����   : STK&DCM��Ŀ: CS��������ϱ�
  4.��    ��   : 2012��12��22��
    ��    ��   : l00227485
    �޸�����   : DSDA phaseII

*****************************************************************************/
VOS_UINT32  MN_CALL_SendAppRequest(
    MN_CALL_APP_REQ_ENUM_UINT32         enReq,
    MN_CLIENT_ID_T                      clientId,
    MN_OPERATION_ID_T                   opId,
    MN_CALL_ID_T                        callId,
    const MN_CALL_APP_REQ_PARM_UNION   *punParam
)
{
    MN_CALL_APP_REQ_MSG_STRU *pstMsg =
        (MN_CALL_APP_REQ_MSG_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(WUEPS_PID_AT,
                                              sizeof(MN_CALL_APP_REQ_MSG_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        AT_ERR_LOG("MN_CALL_SendAppRequest: Failed to alloc VOS message.");
        return VOS_ERR;
    }

    TAF_MEM_SET_S((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH,
               (VOS_SIZE_T)(sizeof(MN_CALL_APP_REQ_MSG_STRU) - VOS_MSG_HEAD_LENGTH),
                0x00,
               (VOS_SIZE_T)(sizeof(MN_CALL_APP_REQ_MSG_STRU) - VOS_MSG_HEAD_LENGTH));

    /* ��дVOS��Ϣͷ */
    pstMsg->ulSenderCpuId               = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid                 = WUEPS_PID_AT;
    pstMsg->ulReceiverCpuId             = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid               = AT_GetDestPid(clientId, I0_WUEPS_PID_TAF);

    /* ��дԭ���ײ� */
    pstMsg->enReq = enReq;
    pstMsg->clientId = clientId;
    pstMsg->opId = opId;
    pstMsg->callId = callId;

    if (TAF_NULL_PTR != punParam)
    {
        TAF_MEM_CPY_S(&pstMsg->unParm, sizeof(pstMsg->unParm), punParam, sizeof(pstMsg->unParm));
    }

    /* ����VOS��Ϣ */
    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_AT, pstMsg))
    {
        AT_ERR_LOG1("MN_CALL_SendAppRequest: Send Message Fail. reqtype:", (VOS_INT32)enReq);
        return VOS_ERR;
    }

    return VOS_OK;
}


/*****************************************************************************
 �� �� ��  : MN_CALL_Orig
 ��������  : ����һ������
 �������  : clientId   - Client ID
             opId       - ���β����ı�ʶ
             pstOrigParam - ���в�����Ҫ�Ĳ���
 �������  : pCallId    - ���κ��е�ID, ����Ψһ�ı�ʶ�������
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2007��9��20��
    ��    ��   : ���� 49431
    �޸�����   : �����ɺ���
  2.��    ��   : 2011��10��06��
    ��    ��   : f00179208
    �޸�����   : AT��ֲ��Ŀ, ����CALLId�ŵ�C��
  3.��    ��   : 2012��9��25��
    ��    ��   : A00165503
    �޸�����   : STK&DCM��Ŀ: CS��������ϱ�
*****************************************************************************/
VOS_UINT32  MN_CALL_Orig(
    MN_CLIENT_ID_T                      clientId,
    MN_OPERATION_ID_T                   opId,
    MN_CALL_ID_T                       *pCallId,
    const MN_CALL_ORIG_PARAM_STRU      *pstOrigParam
)
{
    VOS_UINT32                          ulResult;
    MN_CALL_ID_T                        callId;
    MN_CALL_APP_REQ_PARM_UNION          stAppReq;

    /* �ڸô����ڷ���CallId��ֱ�ӽ�callId��ֵΪ0
       CallId�ķ���ŵ�MN CALLģ�鴦�������ĺ����� */
    callId = 0;

    TAF_MEM_SET_S(&stAppReq, (VOS_UINT32)sizeof(stAppReq), 0x00, (VOS_UINT32)sizeof(stAppReq));
    TAF_MEM_CPY_S(&(stAppReq.stOrig), (VOS_UINT32)sizeof(stAppReq.stOrig), pstOrigParam, (VOS_UINT32)sizeof(MN_CALL_ORIG_PARAM_STRU));

    /* �����첽Ӧ������ */
    ulResult = MN_CALL_SendAppRequest(MN_CALL_APP_ORIG_REQ, clientId,
                                      opId, callId,
                                      &stAppReq);

    *pCallId = callId;

    return ulResult;

}


/*****************************************************************************
 �� �� ��  : MN_CALL_End
 ��������  : �Ҷ�һ������
 �������  : clientId   - Client ID
             opId       - ���β����ı�ʶ
             callId     - ��Ҫ�Ҷϵĺ��е�ID
             pstEndParam  - �Ҷϲ�����Ҫ�Ĳ���, �ò�����ѡ, NULL��ʾʹ��Ĭ�ϲ���
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2007��9��20��
    ��    ��   : ���� 49431
    �޸�����   : �����ɺ���
  2.��    ��   : 2012��09��18��
    ��    ��   : y00213812
    �޸�����   : STK&DCM ��ĿCS��������ϱ����ϱ����ඨ��Ĵ�����

*****************************************************************************/
VOS_UINT32  MN_CALL_End(
    MN_CLIENT_ID_T                      clientId,
    MN_OPERATION_ID_T                   opId,
    MN_CALL_ID_T                        callId,
    const MN_CALL_END_PARAM_STRU       *pstEndParam
)
{
    MN_CALL_APP_REQ_PARM_UNION          stAppReq;
    VOS_UINT32                          ulResult;

    TAF_MEM_SET_S(&stAppReq, (VOS_UINT32)sizeof(stAppReq), 0x00, (VOS_UINT32)sizeof(stAppReq));

    if ( TAF_NULL_PTR == pstEndParam)
    {
        /* ���ع���һ��MN_CALL_END_REQ_PARAM_STRU�ṹ, ��дԭ��ֵΪ255 */
        stAppReq.stEnd.enEndCause = MN_CALL_INTERWORKING_UNSPECIFIED;
    }
    else
    {
        stAppReq.stEnd.enEndCause = pstEndParam->enEndCause;
    }

    /* �����첽Ӧ������ */
    ulResult = MN_CALL_SendAppRequest(MN_CALL_APP_END_REQ, clientId,
                                      opId, callId,
                                      &stAppReq);

    return ulResult;
}

/* Added by f62575 for AT Project, 2011-10-04,  Begin */
/*****************************************************************************
 �� �� ��  : MN_CALL_QryCdur
 ��������  : ��ѯͨ��ʱ��
 �������  : MN_CLIENT_ID_T                      clientId  Client ID
             MN_OPERATION_ID_T                   opId      ���β����ı�ʶ
             MN_CALL_ID_T                        callId    ���е�ID
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��10��6��
    ��    ��   : f62575
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32  MN_CALL_QryCdur(
    MN_CLIENT_ID_T                      clientId,
    MN_OPERATION_ID_T                   opId,
    MN_CALL_ID_T                        callId
)
{
    VOS_UINT32                          ulResult;

    /* �����첽Ӧ������ */
    ulResult = MN_CALL_SendAppRequest(MN_CALL_APP_GET_CDUR_REQ, clientId,
                                      opId, callId,
                                      VOS_NULL_PTR);

    return ulResult;
}
/* Added by f62575 for AT Project, 2011-10-04,  End */

/*****************************************************************************
 �� �� ��  : TAF_CALL_SendDtmf
 ��������  : APP��TAF����DTMF
 �������  : enMsgType      - DTMF������Ϣ����
             clientId       - Client ID
             opId           - ���β����ı�ʶ
             pstDtmfParam   - ����DTMF������Ҫ�Ĳ���
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��07��11��
    ��    ��   : l00198894
    �޸�����   : V9R1 STK������Ŀ
*****************************************************************************/
VOS_UINT32  TAF_CALL_SendDtmf(
    AT_MN_MSGTYPE_ENUM_UINT16           enMsgType,
    MN_CLIENT_ID_T                      clientId,
    MN_OPERATION_ID_T                   opId,
    const TAF_CALL_DTMF_PARAM_STRU     *pstDtmfParam
)
{
    VOS_UINT32                          ulResult;
    MN_CALL_APP_REQ_PARM_UNION          stAppPara;

    /* ��ʼ���ֲ����� */
    TAF_MEM_SET_S(&stAppPara, sizeof(stAppPara), 0x00, sizeof(stAppPara));
    TAF_MEM_CPY_S(&stAppPara.stDtmf, sizeof(stAppPara.stDtmf), pstDtmfParam, sizeof(TAF_CALL_DTMF_PARAM_STRU));

    /* �����첽Ӧ������ */
    ulResult = MN_CALL_SendAppRequest(enMsgType, clientId, opId,
                                      pstDtmfParam->CallId,
                                      &stAppPara);

    return ulResult;
}

/*****************************************************************************
 �� �� ��  : MN_CALL_Sups
 ��������  : ����һ�κ�����ز���ҵ��
 �������  : clientId   - Client ID
             opId       - ���β����ı�ʶ
             pstCallSupsParam   - ���ͺ�����ز���ҵ����Ҫ�Ĳ���
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2007��9��20��
    ��    ��   : ���� 49431
    �޸�����   : �����ɺ���
  2.��    ��   : 2012��9��25��
    ��    ��   : A00165503
    �޸�����   : STK&DCM��Ŀ: CS��������ϱ�
  3.��    ��   : 2013��09��30��
    ��    ��   : s00217060
    �޸�����   : VoLTE_PhaseII��Ŀ����������CallId���һ��
*****************************************************************************/
VOS_UINT32  MN_CALL_Sups(
    MN_CLIENT_ID_T                      clientId,
    MN_OPERATION_ID_T                   opId,
    const MN_CALL_SUPS_PARAM_STRU       *pstCallSupsParam
)
{
    VOS_UINT32                          ulResult;
    MN_CALL_APP_REQ_PARM_UNION          stAppPara;

    TAF_MEM_SET_S(&stAppPara, (VOS_UINT32)sizeof(stAppPara), 0x00, (VOS_UINT32)sizeof(stAppPara));
    TAF_MEM_CPY_S(&stAppPara.stCallMgmtCmd, (VOS_UINT32)sizeof(stAppPara.stCallMgmtCmd), pstCallSupsParam, (VOS_UINT32)sizeof(MN_CALL_SUPS_PARAM_STRU));

    /* �����첽Ӧ������ */
    /* Modified by s00217060 for VoLTE_PhaseII  ��Ŀ, 2013-09-30, begin */
    /* ��������CallId���һ�� */
    ulResult = MN_CALL_SendAppRequest(MN_CALL_APP_SUPS_CMD_REQ, clientId,
                                      opId, pstCallSupsParam->callId,
                                      &stAppPara);
    /* Modified by s00217060 for VoLTE_PhaseII  ��Ŀ, 2013-09-30, end */

    return ulResult;
}


/*****************************************************************************
 �� �� ��  : MN_CALL_GetInfoList
 ��������  : ��ȡ��ǰ����״̬��ΪIDLE�ĺ�����Ϣ
 �������  : pNumOfCalls - �����������ܴ洢�ĺ�����Ϣ����
 �������  : pNumOfCalls - ʵ�������(״̬��ΪIDLE��)������Ϣ����
              pstCallInfos  - ����ĺ�����Ϣ
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2007��9��20��
    ��    ��   : ���� 49431
    �޸�����   : �����ɺ���

  2.��    ��   : 2010��3��1��
    ��    ��   : zhoujun /z40661
    �޸�����   : �޸Ľӿڻ�ȡ��ǰ������Ϣ
  3.��    ��   : 2011��10��15��
    ��    ��   : f00179208
    �޸�����   : AT��ֲ��Ŀ�������첽��Ϣ��C�˻�ȡCALL Info
  4.��    ��   : 2012��9��25��
    ��    ��   : A00165503
    �޸�����   : STK&DCM��Ŀ: CS��������ϱ�
*****************************************************************************/
VOS_UINT32  MN_CALL_GetCallInfos(
    MN_CLIENT_ID_T                      clientId,
    MN_OPERATION_ID_T                   opId,
    MN_CALL_ID_T                        callId
)
{
    VOS_UINT32                          ulResult;

    /* �����첽Ӧ������ */
    ulResult = MN_CALL_SendAppRequest(MN_CALL_APP_GET_INFO_REQ, clientId,
                                      opId, callId,
                                      VOS_NULL_PTR);

    return ulResult;
}

/*****************************************************************************
 �� �� ��  : MN_CALL_SetAlsLineNo
 ��������  : �������õ���·�Ÿ���NV�͵�ǰʹ�õ���·��
 �������  : enAlsLine : ѡ�����·��
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2010��1��22��
    ��    ��   : z40661
    �޸�����   : �����ɺ���
  2.��    ��   : 2011��10��24��
    ��    ��   : c00173809
    �޸�����   : AT�ں���Ŀ��ͨ���˼�ͨ�ŷ�ʽ����ֱ�ӵ���MN_CALL_UpdateAlsLineInfo������

  3.��    ��   : 2011��10��24��
    ��    ��   : o00132663
    �޸�����   : AT�ں���Ŀ�� CC API����MN_CALL_UpdateAlsLineInfo��Ϊ��Ϣ��
                 ��
  4.��    ��   : 2012��9��25��
    ��    ��   : A00165503
    �޸�����   : STK&DCM��Ŀ: CS��������ϱ�
*****************************************************************************/
VOS_UINT32 MN_CALL_SetAlsLineNo(
    TAF_UINT8                           ucIndex,
    MN_CALL_ALS_LINE_NO_ENUM_U8         enAlsLine
)
{
    VOS_UINT32                          ulRst;
    MN_CALL_APP_REQ_PARM_UNION          stAppReq;

    stAppReq.stSetAls.enAlsLine = enAlsLine;

    /*1.ͨ��TAF_MSG_ALS_LINE_NO_SET��Ϣ�������ṹMN_CALL_ALS_PARAM_STRU
        ֪ͨTAF��ALS�������á�*/
    ulRst = MN_CALL_SendAppRequest(MN_CALL_APP_SET_ALS_REQ,
                                   gastAtClientTab[ucIndex].usClientId,
                                   At_GetOpId(),
                                   0,
                                   &stAppReq);
    return ulRst;
}

/*****************************************************************************
 �� �� ��  : MN_CALL_CheckUus1ParmValid
 ��������  : �������UUS1��������Ƿ�Ϸ�
 �������  : enSetType      :�����ȥ����UUS1
             pstUus1Info    :UUS1�����Ϣ
 �������  : ��
 �� �� ֵ  : VOS_UINT32     :�������ɹ���ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2010��7��27��
    ��    ��   : zhoujun /40661
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 MN_CALL_CheckUus1ParmValid(
    MN_CALL_SET_UUS1_TYPE_ENUM_U32      enSetType,
    MN_CALL_UUS1_INFO_STRU              *pstUus1Info
)
{
    if ( VOS_NULL_PTR == pstUus1Info )
    {
        return MN_ERR_INVALIDPARM;
    }

    /*  У������ĺϷ���,�Ƿ�ֱ�ӷ��� */
    if ( ( enSetType >= MN_CALL_SET_UUS1_BUTT )
      || ( pstUus1Info->enMsgType > MN_CALL_UUS1_MSG_RELEASE_COMPLETE ))
    {
        return MN_ERR_INVALIDPARM;
    }


    /* ����UUIE�ļ�������һ���Ƿ���UUIE,�����ĳ��Ⱥ�PD�����м��,
       ��Ӧ�ñ�֤,������ڼ���UUS1ʱ��Ҫ���,ȥ������ĸ���  */
    if ( ( MN_CALL_SET_UUS1_ACT == enSetType)
      && ( MN_CALL_UUS_IEI != pstUus1Info->aucUuie[MN_CALL_IEI_POS]))
    {
        return MN_ERR_INVALIDPARM;
    }

    return MN_ERR_NO_ERROR;
}

/*****************************************************************************
 �� �� ��  : TAF_XCALL_SendFlashReq
 ��������  : APP��TAF����Flash����
 �������  : clientId       - Client ID
             opId           - ���β����ı�ʶ
             pstFlashPara   - ����Flash������Ҫ�Ĳ���
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��11��8��
    ��    ��   : L00256032
    �޸�����   : 1X SS Project�޸�
*****************************************************************************/
VOS_UINT32  TAF_XCALL_SendFlashReq(
    MN_CLIENT_ID_T                      clientId,
    MN_OPERATION_ID_T                   opId,
    TAF_CALL_FLASH_PARA_STRU           *pstFlashPara
)
{
    TAF_CALL_APP_SEND_FLASH_REQ_STRU *pstMsg =
        (TAF_CALL_APP_SEND_FLASH_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(WUEPS_PID_AT,
                                              sizeof(TAF_CALL_APP_SEND_FLASH_REQ_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        AT_ERR_LOG("TAF_XCALL_SendFlashReq: Failed to alloc VOS message.");
        return VOS_ERR;
    }

    TAF_MEM_SET_S((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH,
               (VOS_SIZE_T)(sizeof(TAF_CALL_APP_SEND_FLASH_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
                0x00,
               (VOS_SIZE_T)(sizeof(TAF_CALL_APP_SEND_FLASH_REQ_STRU) - VOS_MSG_HEAD_LENGTH));

    /* ��дVOS��Ϣͷ */
    pstMsg->ulSenderCpuId               = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid                 = WUEPS_PID_AT;
    pstMsg->ulReceiverCpuId             = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid               = AT_GetDestPid(clientId, I0_WUEPS_PID_TAF);

    /* ��д��Ϣ���� */
    pstMsg->usMsgId    = TAF_CALL_APP_SEND_FLASH_REQ;
    pstMsg->usClientId = clientId;
    pstMsg->ucOpId     = opId;
    TAF_MEM_CPY_S(&(pstMsg->stFlashPara), sizeof(pstMsg->stFlashPara), pstFlashPara, sizeof(TAF_CALL_FLASH_PARA_STRU));

    /* ����VOS��Ϣ */
    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_AT, pstMsg))
    {
        AT_ERR_LOG("TAF_XCALL_SendFlashReq: Send TAF_CALL_APP_SEND_FLASH_REQ Message Fail");
        return VOS_ERR;
    }

    return VOS_OK;
}

/* Added by f279542 for CDMA 1X Iteration 4, 2014-11-10, begin */
/*****************************************************************************
 �� �� ��  : TAF_XCALL_SendBurstDtmf
 ��������  : APP��TAF����Flash����
 �������  : clientId              - Client ID
             opId                  - ���β����ı�ʶ
             pstSndBurstDTMFPara   - ����Send Burst Dtmf������Ҫ�Ĳ���
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��11��10��
    ��    ��   : f279542
    �޸�����   : �����ɺ���
*****************************************************************************/
VOS_UINT32  TAF_XCALL_SendBurstDtmf(
    MN_CLIENT_ID_T                      clientId,
    MN_OPERATION_ID_T                   opId,
    TAF_CALL_BURST_DTMF_PARA_STRU      *pstSndBurstDTMFPara
)
{
    TAF_CALL_BURST_DTMF_REQ_MSG_STRU *pstMsg =
        (TAF_CALL_BURST_DTMF_REQ_MSG_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(WUEPS_PID_AT,
                                              sizeof(TAF_CALL_BURST_DTMF_REQ_MSG_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        AT_ERR_LOG("TAF_XCALL_SendBurstDtmf: Failed to alloc VOS message.");
        return VOS_ERR;
    }

    TAF_MEM_SET_S((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH,
               (VOS_SIZE_T)(sizeof(TAF_CALL_BURST_DTMF_REQ_MSG_STRU) - VOS_MSG_HEAD_LENGTH),
                0x00,
               (VOS_SIZE_T)(sizeof(TAF_CALL_BURST_DTMF_REQ_MSG_STRU) - VOS_MSG_HEAD_LENGTH));

    /* ��дVOS��Ϣͷ */
    pstMsg->ulSenderCpuId               = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid                 = WUEPS_PID_AT;
    pstMsg->ulReceiverCpuId             = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid               = AT_GetDestPid(clientId, I0_WUEPS_PID_TAF);

    /* ��д��Ϣ���� */
    pstMsg->usMsgId    = TAF_CALL_APP_SEND_BURST_DTMF_REQ;
    pstMsg->usClientId = clientId;
    pstMsg->ucOpId     = opId;
    TAF_MEM_CPY_S(&(pstMsg->stBurstDTMFPara), sizeof(pstMsg->stBurstDTMFPara), pstSndBurstDTMFPara, sizeof(TAF_CALL_BURST_DTMF_PARA_STRU));

    /* ����VOS��Ϣ */
    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_AT, pstMsg))
    {
        AT_ERR_LOG("TAF_XCALL_SendBurstDtmf: Send TAF_CALL_APP_SEND_BURST_DTMF_REQ Message Fail");
        return VOS_ERR;
    }

    return VOS_OK;
}
/* Added by f279542 for CDMA 1X Iteration 4, 2014-11-10, end */

/*****************************************************************************
 �� �� ��  : TAF_XCALL_SendCustomDialReq
 ��������  : APP��TAF����Flash����
 �������  : clientId       - Client ID
             opId           - ���β����ı�ʶ
             pstFlashPara   - ����Flash������Ҫ�Ĳ���
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��4��8��
    ��    ��   : w00242748
    �޸�����   : ��������
*****************************************************************************/
VOS_UINT32  TAF_XCALL_SendCustomDialReq(
    MN_CLIENT_ID_T                      clientId,
    MN_OPERATION_ID_T                   opId,
    TAF_CALL_CUSTOM_DIAL_PARA_STRU     *pstCustomDialPara
)
{
    TAF_CALL_APP_SEND_CUSTOM_DIAL_REQ_STRU *pstMsg =
        (TAF_CALL_APP_SEND_CUSTOM_DIAL_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(WUEPS_PID_AT,
                                              sizeof(TAF_CALL_APP_SEND_CUSTOM_DIAL_REQ_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        AT_ERR_LOG("TAF_XCALL_SendCustomDialReq: Failed to alloc VOS message.");
        return VOS_FALSE;
    }

    TAF_MEM_SET_S((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH,
               (VOS_SIZE_T)(sizeof(TAF_CALL_APP_SEND_CUSTOM_DIAL_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
                0x00,
               (VOS_SIZE_T)(sizeof(TAF_CALL_APP_SEND_CUSTOM_DIAL_REQ_STRU) - VOS_MSG_HEAD_LENGTH));

    /* ��дVOS��Ϣͷ */
    pstMsg->ulSenderCpuId               = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid                 = WUEPS_PID_AT;
    pstMsg->ulReceiverCpuId             = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid               = AT_GetDestPid(clientId, I0_WUEPS_PID_TAF);

    /* ��д��Ϣ���� */
    pstMsg->usMsgId    = TAF_CALL_APP_SEND_CUSTOM_DIAL_REQ;
    pstMsg->usClientId = clientId;
    pstMsg->ucOpId     = opId;
    TAF_MEM_CPY_S(&(pstMsg->stCustomDialPara), sizeof(pstMsg->stCustomDialPara), pstCustomDialPara, sizeof(pstMsg->stCustomDialPara));

    /* ����VOS��Ϣ */
    (VOS_VOID)PS_SEND_MSG(WUEPS_PID_AT, pstMsg);

    return VOS_TRUE;
}



VOS_UINT32  TAF_XCALL_SendContinuousDtmf(
    MN_CLIENT_ID_T                      clientId,
    MN_OPERATION_ID_T                   opId,
    TAF_CALL_CONT_DTMF_PARA_STRU       *pstSndContDTMFPara
)
{
    TAF_CALL_CONT_DTMF_REQ_MSG_STRU *pstMsg = VOS_NULL_PTR;
    pstMsg = (TAF_CALL_CONT_DTMF_REQ_MSG_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(WUEPS_PID_AT,
                                              sizeof(TAF_CALL_CONT_DTMF_REQ_MSG_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        AT_ERR_LOG("TAF_XCALL_SendContinuousDtmf: Failed to alloc VOS message.");
        return VOS_ERR;
    }

    TAF_MEM_SET_S((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH,
               (VOS_SIZE_T)(sizeof(TAF_CALL_CONT_DTMF_REQ_MSG_STRU) - VOS_MSG_HEAD_LENGTH),
                0x00,
               (VOS_SIZE_T)(sizeof(TAF_CALL_CONT_DTMF_REQ_MSG_STRU) - VOS_MSG_HEAD_LENGTH));

    /* fill in VOS_MSG_HEAD */
    pstMsg->ulSenderCpuId               = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid                 = WUEPS_PID_AT;
    pstMsg->ulReceiverCpuId             = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid               = AT_GetDestPid(clientId, I0_WUEPS_PID_TAF);

    /* fill in message content */
    pstMsg->usMsgId    = TAF_CALL_APP_SEND_CONT_DTMF_REQ;
    pstMsg->usClientId = clientId;
    pstMsg->ucOpId     = opId;
    TAF_MEM_CPY_S(&(pstMsg->stContDTMFPara), sizeof(pstMsg->stContDTMFPara), pstSndContDTMFPara, sizeof(TAF_CALL_CONT_DTMF_PARA_STRU));

    /* Send VOS message */
    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_AT, pstMsg))
    {
        AT_ERR_LOG("TAF_XCALL_SendContinuousDtmf: Send TAF_CALL_APP_SEND_CONT_DTMF_REQ Message Fail");
        return VOS_ERR;
    }

    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : TAF_XCALL_SendCclpr
 ��������  : APP��TAF����Clpr����
 �������  : clientId              - Client ID
             opId                  - ���β����ı�ʶ
             ucCallId               - ����id
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��8��20��
    ��    ��   : f279542
    �޸�����   : �����ɺ���
*****************************************************************************/
VOS_UINT32  TAF_XCALL_SendCclpr(
    MN_CLIENT_ID_T                      clientId,
    MN_OPERATION_ID_T                   opId,
    VOS_UINT8                           ucCallId
)
{
    TAF_CALL_SND_CCLPR_REQ_MSG_STRU *pstMsg =
        (TAF_CALL_SND_CCLPR_REQ_MSG_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(WUEPS_PID_AT,
                                              sizeof(TAF_CALL_SND_CCLPR_REQ_MSG_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        AT_ERR_LOG("TAF_XCALL_SendCclpr: Failed to alloc VOS message.");
        return VOS_ERR;
    }

    TAF_MEM_SET_S((VOS_INT8*)pstMsg + VOS_MSG_HEAD_LENGTH,
               (VOS_SIZE_T)(sizeof(TAF_CALL_SND_CCLPR_REQ_MSG_STRU) - VOS_MSG_HEAD_LENGTH),
                0x00,
               (VOS_SIZE_T)(sizeof(TAF_CALL_SND_CCLPR_REQ_MSG_STRU) - VOS_MSG_HEAD_LENGTH));

    /* ��дVOS��Ϣͷ */
    pstMsg->ulSenderCpuId               = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid                 = WUEPS_PID_AT;
    pstMsg->ulReceiverCpuId             = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid               = AT_GetDestPid(clientId, I0_WUEPS_PID_TAF);

    /* ��д��Ϣ���� */
    pstMsg->usMsgId    = TAF_CALL_APP_SEND_CCLPR_REQ;
    pstMsg->usClientId = clientId;
    pstMsg->ucOpId     = opId;
    pstMsg->ucCallId   = ucCallId;

    /* ����VOS��ϢTAF_CALL_APP_SEND_CCLPR_REQ */
    (VOS_VOID)PS_SEND_MSG(WUEPS_PID_AT, pstMsg);

    return VOS_OK;
}


/*****************************************************************************
 �� �� ��  : TAF_XCALL_SetPrivacyModePreferred
 ��������  : ����Privacy mode����ģʽ
 �������  : VOS_UINT32                                              ulModuleId,
             MN_CLIENT_ID_T                                          usClientId,
             MN_OPERATION_ID_T                                       opId,
             VOS_UINT32                                              ulPrivacyMode

 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��12��22��
    ��    ��   : y00245242
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_XCALL_SetPrivacyModePreferred(
    VOS_UINT32                                              ulModuleId,
    MN_CLIENT_ID_T                                          usClientId,
    MN_OPERATION_ID_T                                       opId,
    VOS_UINT32                                              ulPrivacyMode
)
{
    TAF_CALL_APP_PRIVACY_MODE_SET_REQ_STRU                 *pstPrivacyModeReq = VOS_NULL_PTR;
    VOS_UINT32                                              ulReceiverPid;
    VOS_UINT32                                              ulSenderPid;

    ulReceiverPid        = AT_GetDestPid(usClientId, WUEPS_PID_TAF);
    ulSenderPid          = AT_GetDestPid(usClientId, WUEPS_PID_TAF);


    pstPrivacyModeReq = (TAF_CALL_APP_PRIVACY_MODE_SET_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(ulSenderPid,
                                                                                               sizeof(TAF_CALL_APP_PRIVACY_MODE_SET_REQ_STRU));
    if (VOS_NULL_PTR == pstPrivacyModeReq)
    {
        AT_ERR_LOG("TAF_XCALL_SetPrivacyModePreferred: Failed to alloc VOS message.");
        return VOS_ERR;
    }

    TAF_MEM_SET_S((VOS_INT8*)pstPrivacyModeReq + VOS_MSG_HEAD_LENGTH,
               (VOS_SIZE_T)(sizeof(TAF_CALL_APP_PRIVACY_MODE_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
                0x00,
               (VOS_SIZE_T)(sizeof(TAF_CALL_APP_PRIVACY_MODE_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH));


    /* ��дVOS��Ϣͷ */
    pstPrivacyModeReq->ulSenderCpuId                        = VOS_LOCAL_CPUID;
    pstPrivacyModeReq->ulSenderPid                          = ulSenderPid;
    pstPrivacyModeReq->ulReceiverCpuId                      = VOS_LOCAL_CPUID;
    pstPrivacyModeReq->ulReceiverPid                        = ulReceiverPid;
    pstPrivacyModeReq->ulLength                             = sizeof(TAF_CALL_APP_PRIVACY_MODE_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH;

    /* ��д��Ϣ���� */
    pstPrivacyModeReq->enMsgName                            = ID_TAF_CALL_APP_PRIVACY_MODE_SET_REQ;
    pstPrivacyModeReq->stCtrl.usClientId                    = usClientId;
    pstPrivacyModeReq->stCtrl.ucOpId                        = opId;
    pstPrivacyModeReq->stCtrl.ulModuleId                    = ulModuleId;

    pstPrivacyModeReq->enPrivacyMode                        = (TAF_CALL_PRIVACY_MODE_ENUM_UINT8)ulPrivacyMode;

    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstPrivacyModeReq);

    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : TAF_XCALL_QryPrivacyModePreferred
 ��������  : ��ѯPrivacy mode����ģʽ
 �������  : VOS_UINT32                                              ulModuleId,
             MN_CLIENT_ID_T                                          usClientId,
             MN_OPERATION_ID_T                                       opId

 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��12��22��
    ��    ��   : y00245242
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_XCALL_QryPrivacyModePreferred(
    VOS_UINT32                                              ulModuleId,
    MN_CLIENT_ID_T                                          usClientId,
    MN_OPERATION_ID_T                                       opId
)
{
    TAF_CALL_APP_PRIVACY_MODE_QRY_REQ_STRU                 *pstPrivacyModeReq = VOS_NULL_PTR;
    VOS_UINT32                                              ulReceiverPid;
    VOS_UINT32                                              ulSenderPid;

    ulReceiverPid        = AT_GetDestPid(usClientId, WUEPS_PID_TAF);
    ulSenderPid          = AT_GetDestPid(usClientId, WUEPS_PID_TAF);


    pstPrivacyModeReq = (TAF_CALL_APP_PRIVACY_MODE_QRY_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(ulSenderPid,
                                                                                               sizeof(TAF_CALL_APP_PRIVACY_MODE_QRY_REQ_STRU));
    if (VOS_NULL_PTR == pstPrivacyModeReq)
    {
        AT_ERR_LOG("TAF_XCALL_QryPrivacyModePreferred: Failed to alloc VOS message.");
        return VOS_ERR;
    }

    TAF_MEM_SET_S((VOS_INT8*)pstPrivacyModeReq + VOS_MSG_HEAD_LENGTH,
               (VOS_SIZE_T)(sizeof(TAF_CALL_APP_PRIVACY_MODE_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
                0x00,
               (VOS_SIZE_T)(sizeof(TAF_CALL_APP_PRIVACY_MODE_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH));


    /* ��дVOS��Ϣͷ */
    pstPrivacyModeReq->ulSenderCpuId                        = VOS_LOCAL_CPUID;
    pstPrivacyModeReq->ulSenderPid                          = ulSenderPid;
    pstPrivacyModeReq->ulReceiverCpuId                      = VOS_LOCAL_CPUID;
    pstPrivacyModeReq->ulReceiverPid                        = ulReceiverPid;
    pstPrivacyModeReq->ulLength                             = sizeof(TAF_CALL_APP_PRIVACY_MODE_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH;

    /* ��д��Ϣ���� */
    pstPrivacyModeReq->enMsgName                            = ID_TAF_CALL_APP_PRIVACY_MODE_QRY_REQ;
    pstPrivacyModeReq->stCtrl.usClientId                    = usClientId;
    pstPrivacyModeReq->stCtrl.ucOpId                        = opId;
    pstPrivacyModeReq->stCtrl.ulModuleId                    = ulModuleId;

    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstPrivacyModeReq);

    return VOS_OK;
}

/*****************************************************************************
 �� �� ��  : TAF_CALL_QryCnap
 ��������  : ^CNAP��ѯ��Ϣ������
 �������  : VOS_UINT32                              ulModuleId
             MN_CLIENT_ID_T                          usClientId
             MN_OPERATION_ID_T                       opId
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��11��16��
    ��    ��   : c00380008
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_CALL_QryCnap(
    VOS_UINT32                              ulModuleId,
    MN_CLIENT_ID_T                          usClientId,
    MN_OPERATION_ID_T                       opId
)
{
    TAF_CALL_APP_CNAP_QRY_REQ_STRU         *pstCnapQryReq;
    VOS_UINT32                              ulReceiverPid;
    VOS_UINT32                              ulSenderPid;

    ulReceiverPid     = AT_GetDestPid(usClientId, WUEPS_PID_TAF);
    ulSenderPid       = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    pstCnapQryReq = (TAF_CALL_APP_CNAP_QRY_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(ulSenderPid,
                                sizeof(TAF_CALL_APP_CNAP_QRY_REQ_STRU));

    if (VOS_NULL_PTR == pstCnapQryReq)
    {
        AT_ERR_LOG("TAF_CALL_QryCnapPara: Failed to alloc VOS message.");
        return VOS_ERR;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstCnapQryReq + VOS_MSG_HEAD_LENGTH,
                  (VOS_SIZE_T)(sizeof(TAF_CALL_APP_CNAP_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
                  0x00,
                  (VOS_SIZE_T)(sizeof(TAF_CALL_APP_CNAP_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH));

    /* ��дVOS��Ϣͷ */
    pstCnapQryReq->ulSenderCpuId    = VOS_LOCAL_CPUID;
    pstCnapQryReq->ulSenderPid      = ulSenderPid;
    pstCnapQryReq->ulReceiverCpuId  = VOS_LOCAL_CPUID;
    pstCnapQryReq->ulReceiverPid    = ulReceiverPid;
    pstCnapQryReq->ulLength         = sizeof(TAF_CALL_APP_CNAP_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH;

    /* ��д��Ϣ���� */
    pstCnapQryReq->enMsgName            = ID_TAF_CALL_APP_CNAP_QRY_REQ;
    pstCnapQryReq->stCtrl.usClientId    = usClientId;
    pstCnapQryReq->stCtrl.ucOpId        = opId;
    pstCnapQryReq->stCtrl.ulModuleId    = ulModuleId;

    /* ����VOS��ϢID_TAF_CALL_APP_CNAP_QRY_REQ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstCnapQryReq);

    return VOS_OK;
}

