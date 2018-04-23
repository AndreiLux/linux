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
#include "Taf_Tafm_Remote.h"
#include "PsCommonDef.h"
#include "NVIM_Interface.h"
#include "TafApi.h"

#include "MnCommApi.h"
#include "TafAppMma.h"




/*****************************************************************************
    Э��ջ��ӡ��㷽ʽ�µ�.C�ļ��궨��
*****************************************************************************/

#define    THIS_FILE_ID PS_FILE_ID_TAF_MMA_API_C

/*****************************************************************************
   2 ȫ�ֱ�������
*****************************************************************************/
    extern VOS_UINT32 AT_GetDestPid(
        MN_CLIENT_ID_T                      usClientId,
        VOS_UINT32                          ulRcvPid
    );

/*****************************************************************************
   3 ����ʵ��
*****************************************************************************/



/*****************************************************************************
 �� �� ��  : Taf_PhonePinHandle
 ��������  : +CPIN USIM�ṩ��PIN������API
             void Api_PIN_Handle(VOS_UINT8 ucClientId,VOS_UINT8 ucCmdType, VOS_UINT8
             ucPINType,\
             VOS_UINT8 *pucOldPIN, VOS_UINT8 *pucNewPIN)��
 �������  : ClientId - APP/AT��ʶ
                 OpId - ������ʶ
*pPinData - �ֻ�PIN��������ݽṹ
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2005��8��25��
    ��    ��   : liuyang
    �޸�����   : V200R001�汾���ɺ���
  2.��    ��   : 2007��10��16��
    ��    ��   : F62575
    �޸�����   : ���ⵥA32D13062(�ĵ�У�Է�������)
*****************************************************************************/
TAF_UINT32 Taf_PhonePinHandle ( MN_CLIENT_ID_T          ClientId,
                                MN_OPERATION_ID_T       OpId,
                                TAF_PH_PIN_DATA_STRU   *pPinData)
{
    return MN_FillAndSndAppReqMsg( ClientId,
                                   OpId,
                                   TAF_MSG_MMA_OP_PIN_REQ,
                                   pPinData,
                                   sizeof(TAF_PH_PIN_DATA_STRU),
                                   I0_WUEPS_PID_MMA);
}


/*****************************************************************************
 �� �� ��  : Taf_MePersonalisationHandle
 ��������  : ����״̬��ѯ
 �������  : pMsg  ��Ϣ��
 �������  : ClientId - APP/AT��ʶ
             OpId - ������ʶ
 �� �� ֵ  : �ɹ���ʧ��
 �޸���ʷ      :
  1.��    ��   : 2007��9��28��
    ��    ��   : h44270
    �޸�����   : �����ɺ���
*****************************************************************************/
VOS_UINT32 Taf_MePersonalisationHandle(MN_CLIENT_ID_T          ClientId,
                                       MN_OPERATION_ID_T                 OpId,
                                       TAF_ME_PERSONALISATION_DATA_STRU *pMePersonalData)
{
    return MN_FillAndSndAppReqMsg( ClientId,
                                   OpId,
                                   TAF_MSG_MMA_ME_PERSONAL_REQ,
                                   pMePersonalData,
                                   sizeof(TAF_ME_PERSONALISATION_DATA_STRU),
                                   I0_WUEPS_PID_MMA);
}


/* Taf_PhoneAttach */

/**********************************************************
 Function:       Taf_PhonePlmnList
 Description:    ����PLMN����
 Calls:          APP/AT
 Data Accessed:  2005-09-16
 Data Updated:
 Input:          ClientId - APP/AT��ʶ
                 OpId     - ������ʶ
                 ListType - ��������
 Output:
 Return:         TAF_SUCCESS - �ɹ�
                 TAF_FAILURE - ʧ��
 Others:
    1.Date        : 2007-10-16
    Author      : F62575
    Modification: ���ⵥA32D13062(�ĵ�У�Է�������)
    2.Date        : 2015-3-4
    Author      : b00269685
    Modification: ʹ���½ӿڸ�Ϊ�ֶ��ϱ�
**********************************************************/
TAF_UINT32 Taf_PhonePlmnList (
    VOS_UINT32                          ulModuleId,
    MN_CLIENT_ID_T                      usClientId,
    MN_OPERATION_ID_T                   ucOpId,
    TAF_MMA_PLMN_LIST_PARA_STRU            *pstPlmnListPara
)
{
    TAF_MMA_PLMN_LIST_REQ_STRU             *pstMsg  = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������� */
    if (VOS_NULL_PTR == pstPlmnListPara)
    {
        return VOS_FALSE;
    }

    /* ������Ϣ��TAF_MMA_DETACH_REQ_STRU */
    pstMsg = (TAF_MMA_PLMN_LIST_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                           ulSenderPid,
                                           sizeof(TAF_MMA_PLMN_LIST_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
            (VOS_SIZE_T)(sizeof(TAF_MMA_PLMN_LIST_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
            0x00,
            (VOS_SIZE_T)(sizeof(TAF_MMA_PLMN_LIST_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    /* ��������������TAF_PLMN_LIST_REQ_STRU */
    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid       = ulSenderPid;
    pstMsg->ulReceiverPid     = ulReceiverPid;
    pstMsg->ulMsgName         = ID_TAF_MMA_PLMN_LIST_REQ;
    pstMsg->stCtrl.ulModuleId = ulModuleId;
    pstMsg->stCtrl.usClientId = usClientId;
    pstMsg->stCtrl.ucOpId     = ucOpId;
    TAF_MEM_CPY_S(&(pstMsg->stPlmnListPara), sizeof(pstMsg->stPlmnListPara), pstPlmnListPara, sizeof(TAF_MMA_PLMN_LIST_PARA_STRU));

    /* ������Ϣ */
    if (VOS_OK != PS_SEND_MSG(ulSenderPid, pstMsg))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_PlmnAutoReselReq
 ��������  : �Զ�����
 �������  : ulModuleId             ---  �ⲿģ��PID
             usCliendId             ---  �ⲿģ��CliendId
             ucOpId                 ---  �ⲿģ��OpId
 �������  : ��
 �� �� ֵ  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��3��23��
    ��    ��   : f00279542
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_PlmnAutoReselReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_PLMN_AUTO_RESEL_REQ_STRU   *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ��TAF_MMA_PLMN_AUTO_RESEL_REQ_STRU */
    pstMsg = (TAF_MMA_PLMN_AUTO_RESEL_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                            ulSenderPid,
                                            sizeof(TAF_MMA_PLMN_AUTO_RESEL_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
                sizeof(TAF_MMA_PLMN_AUTO_RESEL_REQ_STRU) - VOS_MSG_HEAD_LENGTH,
                0x00,
                sizeof(TAF_MMA_PLMN_AUTO_RESEL_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_PLMN_AUTO_RESEL_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = ucOpId;

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_PlmnSpecialSelReq
 ��������  : �Զ�����
 �������  : ulModuleId             ---  �ⲿģ��PID
             usCliendId             ---  �ⲿģ��CliendId
             ucOpId                 ---  �ⲿģ��OpId
             ucPlmnReselModeType    ---  fplmn resel���ò���
 �������  : ��
 �� �� ֵ  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��3��23��
    ��    ��   : f00279542
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_PlmnSpecialSelReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_PLMN_USER_SEL_STRU             *pstPlmnUserSel
)
{
    TAF_MMA_PLMN_SPECIAL_SEL_REQ_STRU  *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ��TAF_MMA_PLMN_SPECIAL_SEL_REQ_STRU */
    pstMsg = (TAF_MMA_PLMN_SPECIAL_SEL_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                            ulSenderPid,
                                            sizeof(TAF_MMA_PLMN_SPECIAL_SEL_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
                sizeof(TAF_MMA_PLMN_SPECIAL_SEL_REQ_STRU) - VOS_MSG_HEAD_LENGTH,
                0x00,
                sizeof(TAF_MMA_PLMN_SPECIAL_SEL_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_PLMN_SPECIAL_SEL_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = ucOpId;
    TAF_MEM_CPY_S(&(pstMsg->stPlmnUserSel), sizeof(pstMsg->stPlmnUserSel), pstPlmnUserSel, sizeof(TAF_PLMN_USER_SEL_STRU));

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}
/*****************************************************************************
 �� �� ��  : TAF_MMA_AbortPlmnListReq
 ��������  : AbortReq
 �������  : ulModuleId             ---  �ⲿģ��PID
             usCliendId             ---  �ⲿģ��CliendId
             ucOpId                 ---  �ⲿģ��OpId
             ucPlmnReselModeType    ---  fplmn resel���ò���
 �������  : ��
 �� �� ֵ  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��3��23��
    ��    ��   : f00279542
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_AbortPlmnListReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_PLMN_LIST_ABORT_REQ_STRU   *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ��TAF_MMA_PLMN_LIST_ABORT_REQ_STRU */
    pstMsg = (TAF_MMA_PLMN_LIST_ABORT_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                            ulSenderPid,
                                            sizeof(TAF_MMA_PLMN_LIST_ABORT_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
                sizeof(TAF_MMA_PLMN_LIST_ABORT_REQ_STRU) - VOS_MSG_HEAD_LENGTH,
                0x00,
                sizeof(TAF_MMA_PLMN_LIST_ABORT_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_PLMN_LIST_ABORT_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = ucOpId;
    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_QryLocInfoReq
 ��������  : query loc info
 �������  : ulModuleId             ---  �ⲿģ��PID
             usCliendId             ---  �ⲿģ��CliendId
             ucOpId                 ---  �ⲿģ��OpId
 �������  : ��
 �� �� ֵ  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��3��23��
    ��    ��   : f00279542
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_QryLocInfoReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_LOCATION_INFO_QRY_REQ_STRU *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ��ID_TAF_MMA_LOCINFO_QRY_REQ */
    pstMsg = (TAF_MMA_LOCATION_INFO_QRY_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                                   ulSenderPid,
                                                   sizeof(TAF_MMA_LOCATION_INFO_QRY_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
                sizeof(TAF_MMA_LOCATION_INFO_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH,
                0x00,
                sizeof(TAF_MMA_LOCATION_INFO_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_LOCATION_INFO_QRY_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = ucOpId;
    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}
/*****************************************************************************
 �� �� ��  : TAF_MMA_QryCipherReq
 ��������  : query cipher info
 �������  : ulModuleId             ---  �ⲿģ��PID
             usCliendId             ---  �ⲿģ��CliendId
             ucOpId                 ---  �ⲿģ��OpId
 �������  : ��
 �� �� ֵ  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��3��23��
    ��    ��   : f00279542
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_QryCipherReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_CIPHER_QRY_REQ_STRU        *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ��ID_TAF_MMA_LOCINFO_QRY_REQ */
    pstMsg = (TAF_MMA_CIPHER_QRY_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                            ulSenderPid,
                                            sizeof(TAF_MMA_CIPHER_QRY_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
                sizeof(TAF_MMA_CIPHER_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH,
                0x00,
                sizeof(TAF_MMA_CIPHER_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_CIPHER_QRY_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = ucOpId;
    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}
/*****************************************************************************
 �� �� ��  : TAF_MMA_SetPrefPlmnTypeReq
 ��������  : set prefer plmn type
 �������  : ulModuleId             ---  �ⲿģ��PID
             usCliendId             ---  �ⲿģ��CliendId
             ucOpId                 ---  �ⲿģ��OpId
             ucPlmnReselModeType    ---  fplmn resel���ò���
 �������  : ��
 �� �� ֵ  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��3��23��
    ��    ��   : f00279542
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_SetPrefPlmnTypeReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    MN_PH_PREF_PLMN_TYPE_ENUM_U8       *penPrefPlmnType
)
{
    TAF_MMA_PREF_PLMN_TYPE_SET_REQ_STRU    *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                              ulReceiverPid;
    VOS_UINT32                              ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ��TAF_MMA_PREF_PLMN_TYPE_SET_REQ_STRU */
    pstMsg = (TAF_MMA_PREF_PLMN_TYPE_SET_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                                    ulSenderPid,
                                                    sizeof(TAF_MMA_PREF_PLMN_TYPE_SET_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
                sizeof(TAF_MMA_PREF_PLMN_TYPE_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH,
                0x00,
                sizeof(TAF_MMA_PREF_PLMN_TYPE_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_PREF_PLMN_TYPE_SET_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = ucOpId;
    pstMsg->enPrefPlmnType              = *penPrefPlmnType;
    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}
/*****************************************************************************
 �� �� ��  : TAF_MMA_MtPowerDownReq
 ��������  : �Զ�����
 �������  : ulModuleId             ---  �ⲿģ��PID
             usCliendId             ---  �ⲿģ��CliendId
             ucOpId                 ---  �ⲿģ��OpId
             ucPlmnReselModeType    ---  fplmn resel���ò���
 �������  : ��
 �� �� ֵ  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��3��23��
    ��    ��   : f00279542
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_MtPowerDownReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_MT_POWER_DOWN_REQ_STRU     *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ��TAF_MMA_MT_POWER_DOWN_REQ_STRU */
    pstMsg = (TAF_MMA_MT_POWER_DOWN_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                            ulSenderPid,
                                            sizeof(TAF_MMA_MT_POWER_DOWN_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
                sizeof(TAF_MMA_MT_POWER_DOWN_REQ_STRU) - VOS_MSG_HEAD_LENGTH,
                0x00,
                sizeof(TAF_MMA_MT_POWER_DOWN_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_MT_POWER_DOWN_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = ucOpId;
    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}


/*****************************************************************************
 �� �� ��  : TAF_MMA_SetQuickStartReq
 ��������  : AT^CQST=����
 �������  : ��
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��3��24��
    ��    ��   : y00322978
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_SetQuickStartReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    VOS_UINT32                          ulSetValue
)
{
    TAF_MMA_QUICKSTART_SET_REQ_STRU    *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ��TAF_MMA_QUICKSTART_SET_REQ_STRU */
    pstMsg = (TAF_MMA_QUICKSTART_SET_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                              ulSenderPid,
                                              sizeof(TAF_MMA_QUICKSTART_SET_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
            (VOS_SIZE_T)(sizeof(TAF_MMA_QUICKSTART_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
            0x00,
            (VOS_SIZE_T)(sizeof(TAF_MMA_QUICKSTART_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_QUICKSTART_SET_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = ucOpId;
    pstMsg->ulQuickStartMode            = ulSetValue;

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_QryQuickStartReq
 ��������  : CQST QRY����
 �������  : ��
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��3��27��
    ��    ��   : y00322978
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_QryQuickStartReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_QUICKSTART_QRY_REQ_STRU    *pstMsg;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ��TAF_MMA_QUICKSTART_QRY_REQ_STRU */
    pstMsg = (TAF_MMA_QUICKSTART_QRY_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                       ulSenderPid,
                                       sizeof(TAF_MMA_QUICKSTART_QRY_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
            (VOS_SIZE_T)(sizeof(TAF_MMA_QUICKSTART_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
            0x00,
            (VOS_SIZE_T)(sizeof(TAF_MMA_QUICKSTART_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_QUICKSTART_QRY_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = ucOpId;

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;

}


/*****************************************************************************
 �� �� ��  : Taf_UsimRestrictedAccessCommand
 ��������  : ����֧�������ƵĿ���������
 �������  :
 �������  :
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2006��10��25��
    ��    ��   : h59254
    �޸�����   : �����ɺ���
*****************************************************************************/
TAF_UINT32 Taf_UsimRestrictedAccessCommand(MN_CLIENT_ID_T               ClientId,
                                           MN_OPERATION_ID_T            OpId,
                                           USIMM_RACCESS_REQ_STRU      *pPara)
{
    return MN_FillAndSndAppReqMsg( ClientId,
                                   OpId,
                                   TAF_MSG_MMA_USIM_RESTRICTED_ACCESS,
                                   pPara,
                                   sizeof(USIMM_RACCESS_REQ_STRU),
                                   I0_WUEPS_PID_MMA);
}


/*****************************************************************************
 �� �� ��  : TAF_MMA_QrySyscfgReq
 ��������  : AT+SYSCFG QRY�߼�
 �������  : VOS_UINT32                          ulModuleId,
             VOS_UINT16                          usClientId,
             VOS_UINT8                           ucOpId
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��3��26��
    ��    ��   : y00322978
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_QrySyscfgReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_SYSCFG_QRY_REQ_STRU        *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ��TAF_MMA_SYSCFG_QRY_REQ_STRU */
    pstMsg = (TAF_MMA_SYSCFG_QRY_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                           ulSenderPid,
                                           sizeof(TAF_MMA_SYSCFG_QRY_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
       return VOS_FALSE;
    }

    TAF_MEM_SET_S( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
                (VOS_SIZE_T)(sizeof(TAF_MMA_SYSCFG_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
                0x00,
                (VOS_SIZE_T)(sizeof(TAF_MMA_SYSCFG_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_SYSCFG_QRY_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}


/*****************************************************************************
 �� �� ��  : Taf_SetCopsFormatTypeReq
 ��������  : ����cops������ʾ���͵�����
 �������  : TAF_CLIENT_ID        ClientId
             TAF_ID OpId
             TAF_MMA_NET_SCAN_REQ_STRU    pstNetScan
 �������  : ��
 �� �� ֵ  : TAF_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��3��9��
    ��    ��   : z00161729
    �޸�����   : AT&T ֧��EONS�����޸�
*****************************************************************************/
VOS_UINT32 Taf_SetCopsFormatTypeReq(
    MN_CLIENT_ID_T                                          usClientId,
    MN_OPERATION_ID_T                                       ucOpId,
    TAF_MMA_COPS_FORMAT_TYPE_SET_REQ_STRU                  *pstCopsFormatType
)
{
    return MN_FillAndSndAppReqMsg(usClientId,
                                  ucOpId,
                                  TAF_MSG_MMA_COPS_FORMAT_TYPE_SET_REQ,
                                  pstCopsFormatType,
                                  sizeof(TAF_MMA_COPS_FORMAT_TYPE_SET_REQ_STRU),
                                  I0_WUEPS_PID_MMA);
}


/*****************************************************************************
 �� �� ��  : TAF_SetUsimStub
 ��������  : ����ģ��usim�ظ����ļ��ظ���mma
 �������  : TAF_CLIENT_ID        ClientId
             TAF_ID OpId
             TAF_MMA_USIM_STUB_SET_REQ_STRU     *pstUsimStub
 �������  : ��
 �� �� ֵ  : TAF_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��3��31��
    ��    ��   : z00161729
    �޸�����   : AT&T ֧��EONS�����޸�
*****************************************************************************/
VOS_UINT32 TAF_SetUsimStub(
    MN_CLIENT_ID_T                      usClientId,
    MN_OPERATION_ID_T                   ucOpId,
    TAF_MMA_USIM_STUB_SET_REQ_STRU     *pstUsimStub
)
{
    return MN_FillAndSndAppReqMsg(usClientId,
                                  ucOpId,
                                  TAF_MSG_MMA_USIM_STUB_SET_REQ,
                                  pstUsimStub,
                                  sizeof(TAF_MMA_USIM_STUB_SET_REQ_STRU),
                                  I0_WUEPS_PID_MMA);
}


/*****************************************************************************
 �� �� ��  : TAF_SetRefreshStub
 ��������  : ģ��pih��ָ��ģ�鷢��refresh��Ϣ
 �������  : TAF_CLIENT_ID        ClientId
             TAF_ID OpId
             TAF_MMA_REFRESH_STUB_SET_REQ_STRU  *pstRefreshStub
 �������  : ��
 �� �� ֵ  : TAF_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��3��31��
    ��    ��   : z00161729
    �޸�����   : AT&T ֧��EONS�����޸�
*****************************************************************************/
VOS_UINT32 TAF_SetRefreshStub(
    MN_CLIENT_ID_T                      usClientId,
    MN_OPERATION_ID_T                   ucOpId,
    TAF_MMA_REFRESH_STUB_SET_REQ_STRU  *pstRefreshStub
)
{
    return MN_FillAndSndAppReqMsg(usClientId,
                                  ucOpId,
                                  TAF_MSG_MMA_REFRESH_STUB_SET_REQ,
                                  pstRefreshStub,
                                  sizeof(TAF_MMA_REFRESH_STUB_SET_REQ_STRU),
                                  I0_WUEPS_PID_MMA);
}


/*****************************************************************************
 �� �� ��  : TAF_SetAutoReselStub
 ��������  : auto resel
 �������  : TAF_CLIENT_ID        ClientId
             TAF_ID OpId
             TAF_MMA_AUTO_RESEL_STUB_SET_REQ_STRU       *pstAutoReselStub
 �������  : ��
 �� �� ֵ  : TAF_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��5��29��
    ��    ��   : b00269685
    �޸�����   : ����
*****************************************************************************/
VOS_UINT32 TAF_SetAutoReselStub(
    MN_CLIENT_ID_T                                          usClientId,
    MN_OPERATION_ID_T                                       ucOpId,
    TAF_MMA_AUTO_RESEL_STUB_SET_REQ_STRU                    *pstAutoReselStub
)
{
    return MN_FillAndSndAppReqMsg(usClientId,
                                  ucOpId,
                                  TAF_MSG_MMA_AUTO_RESEL_STUB_SET_REQ,
                                  pstAutoReselStub,
                                  sizeof(TAF_MMA_AUTO_RESEL_STUB_SET_REQ_STRU),
                                  I0_WUEPS_PID_MMA);
}

/* Deleted by k902809 for Iteration 11, 2015-3-25, begin */

/* Deleted by k902809 for Iteration 11, Iteration 11 2015-3-25, end */

/*****************************************************************************
 �� �� ��  : TAF_QryUsimInfo
 ��������  :
 �������  :
 �������  : Icctype
 �� �� ֵ  : TAF_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2009��9��19��
    ��    ��   : z40661
    �޸�����   : ��������,��ѯUSIM���������Ϣ

*****************************************************************************/
TAF_UINT32 TAF_QryUsimInfo(
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId,
    TAF_PH_QRY_USIM_INFO_STRU           *pstInfo
)
{
    if ( (TAF_PH_ICC_UNKNOW == pstInfo->Icctype)
      || (pstInfo->Icctype > TAF_PH_ICC_USIM))
    {
        return TAF_FAILURE;
    }

    if (pstInfo->enEfId > TAF_PH_OPL_FILE)
    {
        return TAF_FAILURE;
    }
    return MN_FillAndSndAppReqMsg( ClientId,
                             OpId,
                             TAF_MSG_MMA_USIM_INFO,
                             pstInfo,
                             sizeof(TAF_PH_QRY_USIM_INFO_STRU),
                             I0_WUEPS_PID_MMA);

}

/*****************************************************************************
 �� �� ��  : TAF_QryCpnnInfo
 ��������  :
 �������  :
 �������  :
 �� �� ֵ  : TAF_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2009��9��19��
    ��    ��   : z40661
    �޸�����   : ��������,��ѯUSIM���������Ϣ

*****************************************************************************/
TAF_UINT32 TAF_QryCpnnInfo(
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId,
    TAF_PH_ICC_TYPE                     IccType
)
{
    if ( (TAF_PH_ICC_UNKNOW == IccType)
      || (IccType > TAF_PH_ICC_USIM))
    {
        return TAF_FAILURE;
    }

    return MN_FillAndSndAppReqMsg( ClientId,
                             OpId,
                             TAF_MSG_MMA_CPNN_INFO,
                             &IccType,
                             sizeof(IccType),
                             I0_WUEPS_PID_MMA);
}

/* Deleted by k902809 for Iteration 11, 2015-3-24, begin */

/* Deleted by k902809 for Iteration 11, Iteration 11 2015-3-24, end */


/* Added by s00246516 for L-C��������Ŀ, 2014-01-29, Begin */
/*****************************************************************************
 �� �� ��  : TAF_MMA_PhoneModeSetReq
 ��������  : �ֻ�ģʽ���ýӿ�
 �������  : ulModuleId       ---  �ⲿģ��PID
             usCliendId       ---  �ⲿģ��CliendId
             ucOpId           ---  �ⲿģ��OpId
             pstPhoneModePara ---  Phone mode���ò���
 �������  : ��
 �� �� ֵ  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��1��27��
    ��    ��   : s00246516
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_PhoneModeSetReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_MMA_PHONE_MODE_PARA_STRU       *pstPhoneModePara
)
{
    TAF_MMA_PHONE_MODE_SET_REQ_STRU    *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������� */
    if (VOS_NULL_PTR == pstPhoneModePara)
    {
        return VOS_FALSE;
    }

    /* ������Ϣ��TAF_MMA_PHONE_MODE_SET_REQ_STRU */
    pstMsg = (TAF_MMA_PHONE_MODE_SET_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                       ulSenderPid,
                                       sizeof(TAF_MMA_PHONE_MODE_SET_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
            (VOS_SIZE_T)(sizeof(TAF_MMA_PHONE_MODE_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
            0x00,
            (VOS_SIZE_T)(sizeof(TAF_MMA_PHONE_MODE_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_PHONE_MODE_SET_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = ucOpId;
    TAF_MEM_CPY_S(&(pstMsg->stPhoneModePara), sizeof(pstMsg->stPhoneModePara), pstPhoneModePara, sizeof(TAF_MMA_PHONE_MODE_PARA_STRU));

    /* ������Ϣ */
    if (VOS_OK != PS_SEND_MSG(ulSenderPid, pstMsg))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}


/*****************************************************************************
 �� �� ��  : TAF_MMA_QryPhoneModeReq
 ��������  : �ֻ�ģʽ��ѯ�ӿ�
 �������  : ulModuleId       ---  �ⲿģ��PID
             usCliendId       ---  �ⲿģ��CliendId
             ucOpId           ---  �ⲿģ��OpId
 �������  : ��
 �� �� ֵ  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��3��23��
    ��    ��   : y00322978
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_QryPhoneModeReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_PHONE_MODE_QRY_REQ_STRU    *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ��TAF_MMA_PHONE_MODE_SET_REQ_STRU */
    pstMsg = (TAF_MMA_PHONE_MODE_QRY_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                       ulSenderPid,
                                       sizeof(TAF_MMA_PHONE_MODE_QRY_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
            (VOS_SIZE_T)(sizeof(TAF_MMA_PHONE_MODE_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
            0x00,
            (VOS_SIZE_T)(sizeof(TAF_MMA_PHONE_MODE_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_PHONE_MODE_QRY_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = ucOpId;

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_CsgListSearchReq
 ��������  : CSG�б��ѯ�ӿ�
 �������  : ulModuleId      -  �ⲿģ��PID
             usCliendId      -  �ⲿģ��CliendId
             ucOpId          -  �ⲿģ��OpId
             pstPlmnListPara -  csg list�ֶβ�ѯ����
 �������  : ��
 �� �� ֵ  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��9��28��
    ��    ��   : z00161729
    �޸�����   : ֧��LTE CSG��������

*****************************************************************************/
VOS_UINT32 TAF_MMA_CsgListSearchReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_MMA_PLMN_LIST_PARA_STRU        *pstPlmnListPara
)
{
    TAF_MMA_CSG_LIST_SEARCH_REQ_STRU   *pstMsg  = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������� */
    if (VOS_NULL_PTR == pstPlmnListPara)
    {
        return VOS_FALSE;
    }

    /* ������Ϣ�� */
    pstMsg = (TAF_MMA_CSG_LIST_SEARCH_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                             ulSenderPid,
                                             sizeof(TAF_MMA_CSG_LIST_SEARCH_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
            (VOS_SIZE_T)(sizeof(TAF_MMA_CSG_LIST_SEARCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
            0x00,
            (VOS_SIZE_T)(sizeof(TAF_MMA_CSG_LIST_SEARCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid       = ulSenderPid;
    pstMsg->ulReceiverPid     = ulReceiverPid;
    pstMsg->enMsgName         = ID_TAF_MMA_CSG_LIST_SEARCH_REQ;
    pstMsg->stCtrl.ulModuleId = ulModuleId;
    pstMsg->stCtrl.usClientId = usClientId;
    pstMsg->stCtrl.ucOpId     = ucOpId;

    TAF_MEM_CPY_S(&pstMsg->stPlmnListPara, sizeof(pstMsg->stPlmnListPara), pstPlmnListPara, sizeof(TAF_MMA_PLMN_LIST_PARA_STRU));

    /* ������Ϣ */
    if (VOS_OK != PS_SEND_MSG(ulSenderPid, pstMsg))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_AbortCsgListSearchReq
 ��������  : ��ֹcsg list�����ӿ�
 �������  : ulModuleId             ---  �ⲿģ��PID
             usCliendId             ---  �ⲿģ��CliendId
             ucOpId                 ---  �ⲿģ��OpId
 �������  : ��
 �� �� ֵ  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��9��28��
    ��    ��   : z00161729
    �޸�����   : ֧��LTE CSG��������

*****************************************************************************/
VOS_UINT32 TAF_MMA_AbortCsgListSearchReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_CSG_LIST_ABORT_REQ_STRU    *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ�� */
    pstMsg = (TAF_MMA_CSG_LIST_ABORT_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                            ulSenderPid,
                                            sizeof(TAF_MMA_CSG_LIST_ABORT_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
                sizeof(TAF_MMA_CSG_LIST_ABORT_REQ_STRU) - VOS_MSG_HEAD_LENGTH,
                0x00,
                sizeof(TAF_MMA_CSG_LIST_ABORT_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->enMsgName                   = ID_TAF_MMA_CSG_LIST_ABORT_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = ucOpId;

    /* ������Ϣ */
    if (VOS_OK != PS_SEND_MSG(ulSenderPid, pstMsg))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_SetCsgIdSearch
 ��������  : ����CSG ID�ӿ�
 �������  : ulModuleId       ---  �ⲿģ��PID
           : usCliendId       ---  �ⲿģ��CliendId
             ucOpId           ---  �ⲿģ��OpId
             pstUserSelCsgId  ---  USER���ò���
 �������  : ��
 �� �� ֵ  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��11��23��
    ��    ��   : s00193151
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_SetCsgIdSearch(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_MMA_CSG_SPEC_SEARCH_INFO_STRU  *pstUserSelCsgId
)
{
    TAF_MMA_CSG_SPEC_SEARCH_REQ_STRU   *pstMsg  = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������� */
    if (VOS_NULL_PTR == pstUserSelCsgId)
    {
        return VOS_FALSE;
    }

    /* ������Ϣ��TAF_MMA_CSG_SPEC_SEARCH_REQ_STRU */
    pstMsg = (TAF_MMA_CSG_SPEC_SEARCH_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                             ulSenderPid,
                                             sizeof(TAF_MMA_CSG_SPEC_SEARCH_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
            (VOS_SIZE_T)(sizeof(TAF_MMA_CSG_SPEC_SEARCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
            0x00,
            (VOS_SIZE_T)(sizeof(TAF_MMA_CSG_SPEC_SEARCH_REQ_STRU) - VOS_MSG_HEAD_LENGTH));

    /* ��������������TAF_MMA_CSG_SPEC_SEARCH_REQ_STRU, ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid       = ulSenderPid;
    pstMsg->ulReceiverPid     = ulReceiverPid;
    pstMsg->ulMsgName         = ID_TAF_MMA_CSG_SPEC_SEARCH_REQ;
    pstMsg->stCtrl.ulModuleId = ulModuleId;
    pstMsg->stCtrl.usClientId = usClientId;
    pstMsg->stCtrl.ucOpId     = ucOpId;

    TAF_MEM_CPY_S(&pstMsg->stCsgSpecSearchInfo, sizeof(pstMsg->stCsgSpecSearchInfo), pstUserSelCsgId, sizeof(pstMsg->stCsgSpecSearchInfo));

    /* ������Ϣ */
    if (VOS_OK != PS_SEND_MSG(ulSenderPid, pstMsg))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_QryCampCsgIdInfoReq
 ��������  : Csg Id Info qry req
 �������  : VOS_UINT32                          ulModuleId
             VOS_UINT16                          usClientId
             VOS_UINT8                           ucOpId
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��11��24��
    ��    ��   : s00193151
    �޸�����   : ֧��LTE CSG����
*****************************************************************************/
VOS_UINT32 TAF_MMA_QryCampCsgIdInfoReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_QRY_CAMP_CSG_ID_INFO_REQ_STRU        *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                                    ulReceiverPid;
    VOS_UINT32                                    ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ��TAF_MMA_QRY_CAMP_CSG_ID_INFO_REQ_STRU */
    pstMsg = (TAF_MMA_QRY_CAMP_CSG_ID_INFO_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                        ulSenderPid,
                                        sizeof(TAF_MMA_QRY_CAMP_CSG_ID_INFO_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
               (VOS_SIZE_T)(sizeof(TAF_MMA_QRY_CAMP_CSG_ID_INFO_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
               0x00,
               (VOS_SIZE_T)(sizeof(TAF_MMA_QRY_CAMP_CSG_ID_INFO_REQ_STRU) - VOS_MSG_HEAD_LENGTH));

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_QRY_CAMP_CSG_ID_INFO_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = ucOpId;

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}


/*****************************************************************************
 �� �� ��  : TAF_MMA_SetSysCfgReq
 ��������  : �ֻ�ϵͳ���ýӿ�
 �������  : ulModuleId      ---  �ⲿģ��PID
            : usCliendId     ---  �ⲿģ��CliendId
            ucOpId           ---  �ⲿģ��OpId
            pstSysCfgPara    ---  Sys Cfg���ò���
 �������  : ��
 �� �� ֵ  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��1��27��
    ��    ��   : s00246516
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_SetSysCfgReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_MMA_SYS_CFG_PARA_STRU          *pstSysCfgPara
)
{
    TAF_MMA_SYS_CFG_REQ_STRU           *pstMsg  = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������� */
    if (VOS_NULL_PTR == pstSysCfgPara)
    {
        return VOS_FALSE;
    }

    /* ������Ϣ��TAF_MMA_SYS_CFG_REQ_STRU */
    pstMsg = (TAF_MMA_SYS_CFG_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                             ulSenderPid,
                                             sizeof(TAF_MMA_SYS_CFG_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
            (VOS_SIZE_T)(sizeof(TAF_MMA_SYS_CFG_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
            0x00,
            (VOS_SIZE_T)(sizeof(TAF_MMA_SYS_CFG_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    /* ��������������TAF_MMA_SYS_CFG_REQ_STRU */
    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid       = ulSenderPid;
    pstMsg->ulReceiverPid     = ulReceiverPid;
    pstMsg->ulMsgName         = ID_TAF_MMA_SYS_CFG_SET_REQ;
    pstMsg->stCtrl.ulModuleId = ulModuleId;
    pstMsg->stCtrl.usClientId = usClientId;
    pstMsg->stCtrl.ucOpId     = ucOpId;

    TAF_MEM_CPY_S(&(pstMsg->stSysCfgPara), sizeof(pstMsg->stSysCfgPara), pstSysCfgPara, sizeof(TAF_MMA_SYS_CFG_PARA_STRU));

    /* ������Ϣ */
    if (VOS_OK != PS_SEND_MSG(ulSenderPid, pstMsg))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_QryEonsUcs2Req
 ��������  : �ֻ�ϵͳ���ýӿ�
 �������  : ulModuleId       ---  �ⲿģ��PID
             usCliendId       ---  �ⲿģ��CliendId
             ucOpId           ---  �ⲿģ��OpId
 �������  : ��
 �� �� ֵ  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��03��12��
    ��    ��   : c00318887
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_QryEonsUcs2Req(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_EONS_UCS2_REQ_STRU         *pstMsg  = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ��TAF_MMA_EONS_UCS2_REQ_STRU */
    pstMsg = (TAF_MMA_EONS_UCS2_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                        ulSenderPid,
                                        sizeof(TAF_MMA_EONS_UCS2_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
            (VOS_SIZE_T)(sizeof(TAF_MMA_EONS_UCS2_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
            0x00,
            (VOS_SIZE_T)(sizeof(TAF_MMA_EONS_UCS2_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    /* ��������������TAF_MMA_EONS_UCS2_REQ_STRU */
    pstMsg->ulSenderPid       = ulSenderPid;
    pstMsg->ulReceiverPid     = ulReceiverPid;
    pstMsg->ulMsgName         = ID_TAF_MSG_MMA_EONS_UCS2_REQ;
    pstMsg->stCtrl.ulModuleId = ulModuleId;
    pstMsg->stCtrl.usClientId = usClientId;
    pstMsg->stCtrl.ucOpId     = ucOpId;

    /* ������Ϣ */
    if (VOS_OK != PS_SEND_MSG(ulSenderPid, pstMsg))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_AcqBestNetworkReq
 ��������  : ��ȡ����ӿ�
 �������  : ulModuleId      ---  �ⲿģ��PID
            : usCliendId     ---  �ⲿģ��CliendId
            ucOpId           ---  �ⲿģ��OpId
            pstAcqPara       ---  Acq���ò���
 �������  : ��
 �� �� ֵ  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��1��27��
    ��    ��   : s00246516
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_AcqBestNetworkReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_MMA_ACQ_PARA_STRU              *pstAcqPara
)
{
    TAF_MMA_ACQ_REQ_STRU               *pstMsg  = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������� */
    if (VOS_NULL_PTR == pstAcqPara)
    {
        return VOS_FALSE;
    }

    /* ������Ϣ��TAF_MMA_ACQ_REQ_STRU */
    pstMsg = (TAF_MMA_ACQ_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                             ulSenderPid,
                                             sizeof(TAF_MMA_ACQ_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
            (VOS_SIZE_T)(sizeof(TAF_MMA_ACQ_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
            0x00,
            (VOS_SIZE_T)(sizeof(TAF_MMA_ACQ_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid       = ulSenderPid;
    pstMsg->ulReceiverPid     = ulReceiverPid;
    pstMsg->ulMsgName         = ID_TAF_MMA_ACQ_BEST_NETWORK_REQ;
    pstMsg->stCtrl.ulModuleId = ulModuleId;
    pstMsg->stCtrl.usClientId = usClientId;
    pstMsg->stCtrl.ucOpId     = ucOpId;
    TAF_MEM_CPY_S(&(pstMsg->stAcqPara), sizeof(pstMsg->stAcqPara), pstAcqPara, sizeof(TAF_MMA_ACQ_PARA_STRU));

    /* ������Ϣ */
    if (VOS_OK != PS_SEND_MSG(ulSenderPid, pstMsg))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_RegReq
 ��������  : ע������ӿ�
 �������  : ulModuleId      ---  �ⲿģ��PID
            : usCliendId     ---  �ⲿģ��CliendId
            ucOpId           ---  �ⲿģ��OpId
            pstRegPara       ---  REG���ò���
 �������  : ��
 �� �� ֵ  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��1��27��
    ��    ��   : s00246516
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_RegReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_MMA_REG_PARA_STRU              *pstRegPara
)
{
    TAF_MMA_REG_REQ_STRU               *pstMsg  = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������� */
    if (VOS_NULL_PTR == pstRegPara)
    {
        return VOS_FALSE;
    }

    /* ������Ϣ��TAF_MMA_REG_REQ_STRU */
    pstMsg = (TAF_MMA_REG_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                             ulSenderPid,
                                             sizeof(TAF_MMA_REG_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
            (VOS_SIZE_T)(sizeof(TAF_MMA_REG_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
            0x00,
            (VOS_SIZE_T)(sizeof(TAF_MMA_REG_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    /* ��������������TAF_MMA_REG_REQ_STRU */
    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid       = ulSenderPid;
    pstMsg->ulReceiverPid     = ulReceiverPid;
    pstMsg->ulMsgName         = ID_TAF_MMA_REG_REQ;
    pstMsg->stCtrl.ulModuleId = ulModuleId;
    pstMsg->stCtrl.usClientId = usClientId;
    pstMsg->stCtrl.ucOpId     = ucOpId;
    TAF_MEM_CPY_S(&(pstMsg->stRegPara), sizeof(pstMsg->stRegPara), pstRegPara, sizeof(TAF_MMA_REG_PARA_STRU));

    /* ������Ϣ */
    if (VOS_OK != PS_SEND_MSG(ulSenderPid, pstMsg))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_PowerSaveReq
 ��������  : Power Save�ӿ�
 �������  : ulModuleId      ---  �ⲿģ��PID
            : usCliendId     ---  �ⲿģ��CliendId
            ucOpId           ---  �ⲿģ��OpId
            pstPowerSavePara ---  Power Save���ò���
 �������  : ��
 �� �� ֵ  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��1��27��
    ��    ��   : s00246516
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_PowerSaveReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_MMA_POWER_SAVE_PARA_STRU       *pstPowerSavePara
)
{

    TAF_MMA_POWER_SAVE_REQ_STRU        *pstMsg  = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������� */
    if (VOS_NULL_PTR == pstPowerSavePara)
    {
        return VOS_FALSE;
    }

    /* ������Ϣ��TAF_MMA_POWER_SAVE_REQ_STRU */
    pstMsg = (TAF_MMA_POWER_SAVE_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                           ulSenderPid,
                                           sizeof(TAF_MMA_POWER_SAVE_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
            (VOS_SIZE_T)(sizeof(TAF_MMA_POWER_SAVE_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
            0x00,
            (VOS_SIZE_T)(sizeof(TAF_MMA_POWER_SAVE_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid       = ulSenderPid;
    pstMsg->ulReceiverPid     = ulReceiverPid;
    pstMsg->ulMsgName         = ID_TAF_MMA_POWER_SAVE_REQ;
    pstMsg->stCtrl.ulModuleId = ulModuleId;
    pstMsg->stCtrl.usClientId = usClientId;
    pstMsg->stCtrl.ucOpId     = ucOpId;
    TAF_MEM_CPY_S(&(pstMsg->stPowerSavePara), sizeof(pstMsg->stPowerSavePara), pstPowerSavePara, sizeof(TAF_MMA_POWER_SAVE_PARA_STRU));

    /* ������Ϣ */
    if (VOS_OK != PS_SEND_MSG(ulSenderPid, pstMsg))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_DetachReq
 ��������  : Detach�ӿ�
 �������  : ulModuleId      ---  �ⲿģ��PID
            : usCliendId     ---  �ⲿģ��CliendId
            ucOpId           ---  �ⲿģ��OpId
            pstDetachPara    ---  Detach���ò���
 �������  : ��
 �� �� ֵ  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��1��27��
    ��    ��   : s00246516
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_DetachReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_MMA_DETACH_PARA_STRU           *pstDetachPara
)
{
    TAF_MMA_DETACH_REQ_STRU            *pstMsg  = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������� */
    if (VOS_NULL_PTR == pstDetachPara)
    {
        return VOS_FALSE;
    }

    /* ������Ϣ��TAF_MMA_DETACH_REQ_STRU */
    pstMsg = (TAF_MMA_DETACH_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                           ulSenderPid,
                                           sizeof(TAF_MMA_DETACH_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
            (VOS_SIZE_T)(sizeof(TAF_MMA_DETACH_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
            0x00,
            (VOS_SIZE_T)(sizeof(TAF_MMA_DETACH_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    /* ��������������TAF_MMA_DETACH_REQ_STRU */
    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid       = ulSenderPid;
    pstMsg->ulReceiverPid     = ulReceiverPid;
    pstMsg->ulMsgName         = ID_TAF_MMA_DETACH_REQ;
    pstMsg->stCtrl.ulModuleId = ulModuleId;
    pstMsg->stCtrl.usClientId = usClientId;
    pstMsg->stCtrl.ucOpId     = ucOpId;
    TAF_MEM_CPY_S(&(pstMsg->stDetachPara), sizeof(pstMsg->stDetachPara), pstDetachPara, sizeof(TAF_MMA_DETACH_PARA_STRU));

    /* ������Ϣ */
    if (VOS_OK != PS_SEND_MSG(ulSenderPid, pstMsg))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}
/* Added by s00246516 for L-C��������Ŀ, 2014-01-29, End */

/*****************************************************************************
 �� �� ��  : TAF_MMA_SetImsSwitchReq
 ��������  : IMSSwitch�ӿ�
 �������  : ulModuleId         ---  �ⲿģ��PID
             usCliendId         ---  �ⲿģ��CliendId
             ucOpId             ---  �ⲿģ��OpId
             pstImsSwitchRat    ---  IMS����뼼������
 �������  : ��
 �� �� ֵ  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��02��02��
    ��    ��   : zwx247453
    �޸�����   : �����ɺ���

  2.��    ��   : 2017��2��24��
    ��    ��   : l00198894
    �޸�����   : IMS Over GU
*****************************************************************************/
VOS_UINT32 TAF_MMA_SetImsSwitchReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_MMA_IMS_SWITCH_INFO_STRU       *pstImsSwitchInfo
)
{
    TAF_MMA_IMS_SWITCH_SET_REQ_STRU    *pstMsg  = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������� */
    if (VOS_NULL_PTR == pstImsSwitchInfo)
    {
        return VOS_FALSE;
    }

    /* ������Ϣ��TAF_MMA_IMS_SWITCH_SET_REQ_STRU */
    pstMsg = (TAF_MMA_IMS_SWITCH_SET_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                           ulSenderPid,
                                           sizeof(TAF_MMA_IMS_SWITCH_SET_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
            (VOS_SIZE_T)(sizeof(TAF_MMA_IMS_SWITCH_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
            0x00,
            (VOS_SIZE_T)(sizeof(TAF_MMA_IMS_SWITCH_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    /* ��������������TAF_MMA_IMS_SWITCH_SET_REQ_STRU */
    pstMsg->ulSenderPid         = ulSenderPid;
    pstMsg->ulReceiverPid       = ulReceiverPid;
    pstMsg->ulMsgName           = ID_TAF_MMA_IMS_SWITCH_SET_REQ;
    pstMsg->stCtrl.ulModuleId   = ulModuleId;
    pstMsg->stCtrl.usClientId   = usClientId;
    pstMsg->stCtrl.ucOpId       = ucOpId;
    pstMsg->stImsSwitchInfo     = *pstImsSwitchInfo;

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_QryImsSwitchReq
 ��������  : IMSSwitch�ӿ�
 �������  : ulModuleId     ---  �ⲿģ��PID
             usCliendId     ---  �ⲿģ��CliendId
             ucOpId         ---  �ⲿģ��OpId
 �������  : ��
 �� �� ֵ  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��02��02��
    ��    ��   : zwx247453
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_QryImsSwitchReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_IMS_SWITCH_QRY_REQ_STRU    *pstMsg  = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);


    /* ������Ϣ��TAF_MMA_IMS_SWITCH_QRY_REQ_STRU */
    pstMsg = (TAF_MMA_IMS_SWITCH_QRY_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                           ulSenderPid,
                                           sizeof(TAF_MMA_IMS_SWITCH_QRY_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
            (VOS_SIZE_T)(sizeof(TAF_MMA_IMS_SWITCH_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
            0x00,
            (VOS_SIZE_T)(sizeof(TAF_MMA_IMS_SWITCH_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    /* ��������������TAF_MMA_IMS_SWITCH_QRY_REQ_STRU */
    pstMsg->ulSenderPid       = ulSenderPid;
    pstMsg->ulReceiverPid     = ulReceiverPid;
    pstMsg->ulMsgName         = ID_TAF_MMA_IMS_SWITCH_QRY_REQ;
    pstMsg->stCtrl.ulModuleId = ulModuleId;
    pstMsg->stCtrl.usClientId = usClientId;
    pstMsg->stCtrl.ucOpId     = ucOpId;

    /* ������Ϣ */
    if (VOS_OK != PS_SEND_MSG(ulSenderPid, pstMsg))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_QryRcsSwitchReq
 ��������  : RcsSwitch�ӿ�
 �������  : ulModuleId     ---  �ⲿģ��PID
             usCliendId     ---  �ⲿģ��CliendId
             ucOpId         ---  �ⲿģ��OpId
 �������  : ��
 �� �� ֵ  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2017��07��10��
    ��    ��   : m00217266
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_QryRcsSwitchReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_RCS_SWITCH_QRY_REQ_STRU    *pstMsg  = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);


    /* ������Ϣ��TAF_MMA_IMS_SWITCH_QRY_REQ_STRU */
    pstMsg = (TAF_MMA_RCS_SWITCH_QRY_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                           ulSenderPid,
                                           sizeof(TAF_MMA_RCS_SWITCH_QRY_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
            (VOS_SIZE_T)(sizeof(TAF_MMA_RCS_SWITCH_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
            0x00,
            (VOS_SIZE_T)(sizeof(TAF_MMA_RCS_SWITCH_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    /* ��������������TAF_MMA_RCS_SWITCH_QRY_REQ_STRU */
    pstMsg->ulSenderPid       = ulSenderPid;
    pstMsg->ulReceiverPid     = ulReceiverPid;
    pstMsg->ulMsgName         = ID_TAF_MMA_RCS_SWITCH_QRY_REQ;
    pstMsg->stCtrl.ulModuleId = ulModuleId;
    pstMsg->stCtrl.usClientId = usClientId;
    pstMsg->stCtrl.ucOpId     = ucOpId;

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_SetVoiceDomainReq
 ��������  : VoiceDomain�ӿ�
 �������  : ulModuleId       ---  �ⲿģ��PID
             usCliendId       ---  �ⲿģ��CliendId
             ucOpId           ---  �ⲿģ��OpId
             enVoiceDomain    ---  ���������ò���
 �������  : ��
 �� �� ֵ  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��02��02��
    ��    ��   : zwx247453
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_SetVoiceDomainReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_MMA_VOICE_DOMAIN_ENUM_UINT32    enVoiceDomain
)
{
    TAF_MMA_VOICE_DOMAIN_SET_REQ_STRU  *pstMsg  = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������� */
    if (enVoiceDomain >= TAF_MMA_VOICE_DOMAIN_BUTT)
    {
        return VOS_FALSE;
    }

    /* ������Ϣ��TAF_MMA_VOICE_DOMAIN_SET_REQ_STRU */
    pstMsg = (TAF_MMA_VOICE_DOMAIN_SET_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                           ulSenderPid,
                                           sizeof(TAF_MMA_VOICE_DOMAIN_SET_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
            (VOS_SIZE_T)(sizeof(TAF_MMA_VOICE_DOMAIN_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
            0x00,
            (VOS_SIZE_T)(sizeof(TAF_MMA_VOICE_DOMAIN_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    /* ��������������TAF_MMA_VOICE_DOMAIN_SET_REQ_STRU */
    pstMsg->ulSenderPid       = ulSenderPid;
    pstMsg->ulReceiverPid     = ulReceiverPid;
    pstMsg->ulMsgName         = ID_TAF_MMA_VOICE_DOMAIN_SET_REQ;
    pstMsg->stCtrl.ulModuleId = ulModuleId;
    pstMsg->stCtrl.usClientId = usClientId;
    pstMsg->stCtrl.ucOpId     = ucOpId;
    pstMsg->enVoiceDomain     = enVoiceDomain;

    /* ������Ϣ */
    if (VOS_OK != PS_SEND_MSG(ulSenderPid, pstMsg))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_QryVoiceDomainReq
 ��������  : VoiceDomain�ӿ�
 �������  : ulModuleId      ---  �ⲿģ��PID
             usCliendId      ---  �ⲿģ��CliendId
             ucOpId          ---  �ⲿģ��OpId
 �������  : ��
 �� �� ֵ  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��02��02��
    ��    ��   : zwx247453
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_QryVoiceDomainReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_VOICE_DOMAIN_QRY_REQ_STRU  *pstMsg  = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ��TAF_MMA_VOICE_DOMAIN_QRY_REQ_STRU */
    pstMsg = (TAF_MMA_VOICE_DOMAIN_QRY_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                           ulSenderPid,
                                           sizeof(TAF_MMA_VOICE_DOMAIN_QRY_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
            (VOS_SIZE_T)(sizeof(TAF_MMA_VOICE_DOMAIN_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
            0x00,
            (VOS_SIZE_T)(sizeof(TAF_MMA_VOICE_DOMAIN_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    /* ��������������TAF_MMA_VOICE_DOMAIN_QRY_REQ_STRU */
    pstMsg->ulSenderPid       = ulSenderPid;
    pstMsg->ulReceiverPid     = ulReceiverPid;
    pstMsg->ulMsgName         = ID_TAF_MMA_VOICE_DOMAIN_QRY_REQ;
    pstMsg->stCtrl.ulModuleId = ulModuleId;
    pstMsg->stCtrl.usClientId = usClientId;
    pstMsg->stCtrl.ucOpId     = ucOpId;

    /* ������Ϣ */
    if (VOS_OK != PS_SEND_MSG(ulSenderPid, pstMsg))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_SetRoamImsSupportReq
 ��������  : RoamImsSupport�ӿ�
 �������  : ulModuleId      ---  �ⲿģ��PID
             usCliendId      ---  �ⲿģ��CliendId
             ucOpId          ---  �ⲿģ��OpId
             pstRoamImsSupport    ---  RoamImsSupport���ò���
 �������  : ��
 �� �� ֵ  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��3��9��
    ��    ��   : w00316404
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_SetRoamImsSupportReq(
    VOS_UINT32                           ulModuleId,
    VOS_UINT16                           usClientId,
    VOS_UINT8                            ucOpId,
    TAF_MMA_ROAM_IMS_SUPPORT_ENUM_UINT32 enRoamImsSupport
)
{
    TAF_MMA_ROAM_IMS_SUPPORT_SET_REQ_STRU  *pstMsg  = VOS_NULL_PTR;
    VOS_UINT32                              ulReceiverPid;
    VOS_UINT32                              ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������� */
    if (TAF_MMA_ROAM_IMS_BUTT <= enRoamImsSupport)
    {
        return VOS_FALSE;
    }

    /* ������Ϣ��TAF_MMA_ROAM_IMS_SUPPORT_SET_REQ_STRU */
    pstMsg = (TAF_MMA_ROAM_IMS_SUPPORT_SET_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                           ulSenderPid,
                                           sizeof(TAF_MMA_ROAM_IMS_SUPPORT_SET_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
            (VOS_SIZE_T)(sizeof(TAF_MMA_ROAM_IMS_SUPPORT_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
            0x00,
            (VOS_SIZE_T)(sizeof(TAF_MMA_ROAM_IMS_SUPPORT_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    /* ��������������TAF_MMA_ROAM_IMS_SUPPORT_SET_REQ_STRU */
    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid       = ulSenderPid;
    pstMsg->ulReceiverPid     = ulReceiverPid;
    pstMsg->ulMsgName         = ID_TAF_MMA_ROAM_IMS_SUPPORT_SET_REQ;
    pstMsg->stCtrl.ulModuleId = ulModuleId;
    pstMsg->stCtrl.usClientId = usClientId;
    pstMsg->stCtrl.ucOpId     = ucOpId;
    TAF_MEM_CPY_S(&(pstMsg->enRoamingImsSupportFlag), sizeof(pstMsg->enRoamingImsSupportFlag), &enRoamImsSupport, sizeof(TAF_MMA_ROAM_IMS_SUPPORT_ENUM_UINT32));

    /* ������Ϣ */
    if (VOS_OK != PS_SEND_MSG(ulSenderPid, pstMsg))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_SetImsDomainCfgReq
 ��������  : AT��MMA����Ims��ѡ��ƫ������������Ϣ����
 �������  : ulModuleId       ---  �ⲿģ��PID
             usCliendId       ---  �ⲿģ��CliendId
             ucOpId           ---  �ⲿģ��OpId
             enImsDomainCfg   ---  IMS��ѡ��ƫ�ò���

 �������  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��10��26��
    ��    ��   : h00360002
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_SetImsDomainCfgReq(
    VOS_UINT32                                   ulModuleId,
    VOS_UINT16                                   usClientId,
    VOS_UINT8                                    ucOpId,
    TAF_MMA_IMS_DOMAIN_CFG_TYPE_ENUM_UINT32      enImsDomainCfg
)
{
    TAF_MMA_IMS_DOMAIN_CFG_SET_REQ_STRU     *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                               ulReceiverPid;
    VOS_UINT32                               ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    if (enImsDomainCfg >= TAF_MMA_IMS_DOMAIN_CFG_TYPE_BUTT)
    {
        return VOS_FALSE;
    }

    /* �ڴ����� */
    pstMsg = (TAF_MMA_IMS_DOMAIN_CFG_SET_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                           ulSenderPid,
                                           sizeof(TAF_MMA_IMS_DOMAIN_CFG_SET_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
          (VOS_SIZE_T)(sizeof(TAF_MMA_IMS_DOMAIN_CFG_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
          0x00,
          (VOS_SIZE_T)(sizeof(TAF_MMA_IMS_DOMAIN_CFG_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH));

    /* ��������������TAF_MMA_IMS_DOMAIN_CFG_SET_REQ_STRU */
    pstMsg->ulSenderPid       = ulSenderPid;
    pstMsg->ulReceiverPid     = ulReceiverPid;
    pstMsg->ulMsgName         = ID_TAF_MMA_IMS_DOMAIN_CFG_SET_REQ;
    pstMsg->stCtrl.ulModuleId = ulModuleId;
    pstMsg->stCtrl.usClientId = usClientId;
    pstMsg->stCtrl.ucOpId     = ucOpId;
    pstMsg->enImsDoaminCfg    = enImsDomainCfg;

    /* ������Ϣ */
    if (VOS_OK != PS_SEND_MSG(ulSenderPid, pstMsg))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_QryImsDomainCfgReq
 ��������  : AT��MMA����Ims��ѡ��ƫ�ò�ѯ������
 �������  : ulModuleId       ---  �ⲿģ��PID
             usCliendId       ---  �ⲿģ��CliendId
             ucOpId           ---  �ⲿģ��OpId

 �������  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��10��26��
    ��    ��   : h00360002
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_QryImsDomainCfgReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_IMS_DOMAIN_CFG_QRY_REQ_STRU      *pstMsg  = VOS_NULL_PTR;
    VOS_UINT32                                ulReceiverPid;
    VOS_UINT32                                ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* �ڴ�����*/
    pstMsg = (TAF_MMA_IMS_DOMAIN_CFG_QRY_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                           ulSenderPid,
                                           sizeof(TAF_MMA_IMS_DOMAIN_CFG_QRY_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
          (VOS_SIZE_T)(sizeof(TAF_MMA_IMS_DOMAIN_CFG_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
          0x00,
          (VOS_SIZE_T)(sizeof(TAF_MMA_IMS_DOMAIN_CFG_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH));

    /* ��������������TAF_MMA_IMS_DOMAIN_CFG_QRY_REQ_STRU */
    pstMsg->ulSenderPid       = ulSenderPid;
    pstMsg->ulReceiverPid     = ulReceiverPid;
    pstMsg->ulMsgName         = ID_TAF_MMA_IMS_DOMAIN_CFG_QRY_REQ;
    pstMsg->stCtrl.ulModuleId = ulModuleId;
    pstMsg->stCtrl.usClientId = usClientId;
    pstMsg->stCtrl.ucOpId     = ucOpId;

     /* ������Ϣ */
    if (VOS_OK != PS_SEND_MSG(ulSenderPid, pstMsg))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_SetImsVtCapCfgReq
 ��������  : AT��MMA����ImsVT��������������Ϣ����
 �������  : ulModuleId       ---  �ⲿģ��PID
             usCliendId       ---  �ⲿģ��CliendId
             ucOpId           ---  �ⲿģ��OpId
             pstImsVtCap      ---  IMS VT������Ϣ

 �������  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2017��05��10��
    ��    ��   : n00269697
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_SetImsVtCapCfgReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_MMA_IMS_VIDEO_CALL_CAP_STRU    *pstImsVtCap
)
{
    TAF_MMA_IMS_VIDEO_CALL_CAP_REQ_STRU                    *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                                              ulReceiverPid;
    VOS_UINT32                                              ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* �ڴ����� */
    pstMsg = (TAF_MMA_IMS_VIDEO_CALL_CAP_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                        ulSenderPid,
                                        sizeof(TAF_MMA_IMS_VIDEO_CALL_CAP_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
          (VOS_SIZE_T)(sizeof(TAF_MMA_IMS_VIDEO_CALL_CAP_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
          0x00,
          (VOS_SIZE_T)(sizeof(TAF_MMA_IMS_VIDEO_CALL_CAP_REQ_STRU) - VOS_MSG_HEAD_LENGTH));

    /* ��������������TAF_MMA_IMS_SMS_CFG_SET_REQ_STRU */
    pstMsg->ulSenderPid       = ulSenderPid;
    pstMsg->ulReceiverPid     = ulReceiverPid;
    pstMsg->ulMsgName         = ID_TAF_MMA_IMS_VIDEO_CALL_CAP_SET_REQ;
    pstMsg->stCtrl.ulModuleId = ulModuleId;
    pstMsg->stCtrl.usClientId = usClientId;
    pstMsg->stCtrl.ucOpId     = ucOpId;
    TAF_MEM_CPY_S(&pstMsg->stImsVtCap, sizeof(pstMsg->stImsVtCap), pstImsVtCap, sizeof(TAF_MMA_IMS_VIDEO_CALL_CAP_STRU));

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_SetImsSmsCfgReq
 ��������  : AT��MMA����Ims������������������Ϣ����
 �������  : ulModuleId       ---  �ⲿģ��PID
             usCliendId       ---  �ⲿģ��CliendId
             ucOpId           ---  �ⲿģ��OpId
             pstImsSmsCfg      ---  IMS ������������
 �������  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��10��31��
    ��    ��   : m00217266
    �޸�����   : �����ɺ���

  2.��    ��   : 2017��2��27��
    ��    ��   : l00198894
    �޸�����   : IMS Over GU
*****************************************************************************/
VOS_UINT32 TAF_MMA_SetImsSmsCfgReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_MMA_IMS_SMS_CFG_STRU           *pstImsSmsCfg
)
{
    TAF_MMA_IMS_SMS_CFG_SET_REQ_STRU   *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* �ڴ����� */
    pstMsg = (TAF_MMA_IMS_SMS_CFG_SET_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                        ulSenderPid,
                                        sizeof(TAF_MMA_IMS_SMS_CFG_SET_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
          (VOS_SIZE_T)(sizeof(TAF_MMA_IMS_SMS_CFG_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
          0x00,
          (VOS_SIZE_T)(sizeof(TAF_MMA_IMS_SMS_CFG_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH));

    /* ��������������TAF_MMA_IMS_SMS_CFG_SET_REQ_STRU */
    pstMsg->ulSenderPid       = ulSenderPid;
    pstMsg->ulReceiverPid     = ulReceiverPid;
    pstMsg->ulMsgName         = ID_TAF_MMA_IMS_SMS_CFG_SET_REQ;
    pstMsg->stCtrl.ulModuleId = ulModuleId;
    pstMsg->stCtrl.usClientId = usClientId;
    pstMsg->stCtrl.ucOpId     = ucOpId;
    pstMsg->stImsSmsCfg       = *pstImsSmsCfg;

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_QryImsSmsCfgReq
 ��������  : AT��MMA����Ims����������ѯ������
 �������  : ulModuleId       ---  �ⲿģ��PID
             usCliendId       ---  �ⲿģ��CliendId
             ucOpId           ---  �ⲿģ��OpId

 �������  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��10��31��
    ��    ��   : m00217266
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_QryImsSmsCfgReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_IMS_SMS_CFG_QRY_REQ_STRU   *pstMsg  = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* �ڴ�����*/
    pstMsg = (TAF_MMA_IMS_SMS_CFG_QRY_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                        ulSenderPid,
                                        sizeof(TAF_MMA_IMS_SMS_CFG_QRY_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
          (VOS_SIZE_T)(sizeof(TAF_MMA_IMS_SMS_CFG_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
          0x00,
          (VOS_SIZE_T)(sizeof(TAF_MMA_IMS_SMS_CFG_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH));

    /* ��������������TAF_MMA_IMS_DOMAIN_CFG_QRY_REQ_STRU */
    pstMsg->ulSenderPid       = ulSenderPid;
    pstMsg->ulReceiverPid     = ulReceiverPid;
    pstMsg->ulMsgName         = ID_TAF_MMA_IMS_SMS_CFG_QRY_REQ;
    pstMsg->stCtrl.ulModuleId = ulModuleId;
    pstMsg->stCtrl.usClientId = usClientId;
    pstMsg->stCtrl.ucOpId     = ucOpId;

     /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}


/*****************************************************************************
 �� �� ��  : TAF_MMA_AttachReq
 ��������  : AT��MMA����ATTACH������Ϣ����
 �������  : ulModuleId       ---  �ⲿģ��PID
             usCliendId       ---  �ⲿģ��CliendId
             ucOpId           ---  �ⲿģ��OpId
             ucAttachType     ---  Attach���ò���

 �������  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��4��9��
    ��    ��   : h00313353
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_AttachReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_MMA_ATTACH_TYPE_ENUM_UINT8      enAttachType
)
{
    TAF_MMA_ATTACH_REQ_STRU            *pstMsg  = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* �����ڴ���Ϣ����ʼ�� */
    pstMsg = (TAF_MMA_ATTACH_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                           ulSenderPid,
                                           sizeof(TAF_MMA_ATTACH_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
               (VOS_SIZE_T)(sizeof(TAF_MMA_ATTACH_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
               0x00,
               (VOS_SIZE_T)(sizeof(TAF_MMA_ATTACH_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    /* ��������������TAF_MMA_ATTACH_REQ_STRU */
    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid       = ulSenderPid;
    pstMsg->ulReceiverPid     = ulReceiverPid;
    pstMsg->enMsgName         = ID_TAF_MMA_ATTACH_REQ;
    pstMsg->stCtrl.ulModuleId = ulModuleId;
    pstMsg->stCtrl.usClientId = usClientId;
    pstMsg->stCtrl.ucOpId     = ucOpId;
    pstMsg->enAttachType      = enAttachType;

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_AttachStatusQryReq
 ��������  : AT��MMA������״̬��ѯ��Ϣ����Ϣ���ͺ���
 �������  : ulModuleId       ---  �ⲿģ��PID
             usCliendId       ---  �ⲿģ��CliendId
             ucOpId           ---  �ⲿģ��OpId
             enDomainType     ---  ��ѯ�ķ����������
 �������  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��4��9��
    ��    ��   : h00313353
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_AttachStatusQryReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_MMA_SERVICE_DOMAIN_ENUM_UINT8   enDomainType
)
{
    TAF_MMA_ATTACH_STATUS_QRY_REQ_STRU *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* �����ڴ���Ϣ����ʼ�� */
    pstMsg = (TAF_MMA_ATTACH_STATUS_QRY_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                           ulSenderPid,
                                           sizeof(TAF_MMA_ATTACH_STATUS_QRY_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
            (VOS_SIZE_T)(sizeof(TAF_MMA_ATTACH_STATUS_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
            0x00,
            (VOS_SIZE_T)(sizeof(TAF_MMA_ATTACH_STATUS_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    /* ��������������TAF_MMA_ATTACH_REQ_STRU */
    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid       = ulSenderPid;
    pstMsg->ulReceiverPid     = ulReceiverPid;
    pstMsg->enMsgName         = ID_TAF_MMA_ATTACH_STATUS_QRY_REQ;
    pstMsg->stCtrl.ulModuleId = ulModuleId;
    pstMsg->stCtrl.usClientId = usClientId;
    pstMsg->stCtrl.ucOpId     = ucOpId;
    pstMsg->enDomainType      = enDomainType;

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;

}


/*****************************************************************************
 �� �� ��  : TAF_MMA_SrvAcqReq
 ��������  : SPMģ���MMAģ�鷢������ָʾ
 �������  : enSrvType:��������
            pstRatList:��Ҫ������RAT�б�
 �������  : ��
 �� �� ֵ  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��6��17��
    ��    ��   : w00242748
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_VOID TAF_MMA_SrvAcqReq(
    TAF_MMA_SRV_TYPE_ENUM_UINT8         enSrvType,
    TAF_MMA_SRV_ACQ_RAT_LIST_STRU      *pstRatList,
    VOS_UINT32                          ulModuleId
)
{
    TAF_MMA_SRV_ACQ_REQ_STRU           *pstMsg  = VOS_NULL_PTR;

    /* ������Ϣ��TAF_MMA_SRV_ACQ_REQ_STRU */
    pstMsg = (TAF_MMA_SRV_ACQ_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                             WUEPS_PID_TAF,
                                             sizeof(TAF_MMA_SRV_ACQ_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {

        return;
    }

    TAF_MEM_SET_S( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
            (VOS_SIZE_T)(sizeof(TAF_MMA_SRV_ACQ_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
            0x00,
            (VOS_SIZE_T)(sizeof(TAF_MMA_SRV_ACQ_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid       = WUEPS_PID_TAF;
    pstMsg->ulReceiverPid     = WUEPS_PID_MMA;
    pstMsg->ulMsgName         = ID_TAF_MMA_SRV_ACQ_REQ;

    pstMsg->stCtrl.ulModuleId = ulModuleId;

    pstMsg->enSrvType         = enSrvType;
    pstMsg->stRatList         = *pstRatList;

    /* ������Ϣ */
    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_TAF, pstMsg))
    {
        return;
    }

    return;
}



/*****************************************************************************
 �� �� ��  : TAF_MMA_ProcCFreqLockSetReq
 ��������  : ����TAF_MMA_CFREQ_LOCK_SET_REQ
 �������  : ulModuleId        -  �ⲿģ��PID
             usClientId        - Client ID
             ucOpId            - ���β����ı�ʶ
             stCFreqLockPara   - ������Ƶ������Ҫ�Ĳ���
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��12��29��
    ��    ��   : y00307564
    �޸�����   : �����ɺ���
*****************************************************************************/
VOS_UINT32  TAF_MMA_ProcCFreqLockSetReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_MMA_CFREQ_LOCK_SET_PARA_STRU   *pstCFreqLockPara
)
{
    TAF_MMA_CFREQ_LOCK_SET_REQ_STRU    *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������� */
    if (VOS_NULL_PTR == pstCFreqLockPara)
    {
        return VOS_FALSE;
    }

    /* ������Ϣ��TAF_MMA_CFREQ_LOCK_SET_REQ_STRU */
    pstMsg = (TAF_MMA_CFREQ_LOCK_SET_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                       ulSenderPid,
                                       sizeof(TAF_MMA_CFREQ_LOCK_SET_REQ_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
            (VOS_SIZE_T)(sizeof(TAF_MMA_CFREQ_LOCK_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
            0x00,
            (VOS_SIZE_T)(sizeof(TAF_MMA_CFREQ_LOCK_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid       = ulSenderPid;
    pstMsg->ulReceiverPid     = ulReceiverPid;
    pstMsg->ulMsgName         = ID_TAF_MMA_CDMA_FREQ_LOCK_SET_REQ;
    pstMsg->ulModuleId        = ulModuleId;
    pstMsg->usClientId        = usClientId;
    pstMsg->ucOpId            = ucOpId;
    TAF_MEM_CPY_S(&pstMsg->stCFreqLockPara, sizeof(pstMsg->stCFreqLockPara), pstCFreqLockPara, sizeof(TAF_MMA_CFREQ_LOCK_SET_PARA_STRU));

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_ProcCFreqLockQryReq
 ��������  : �ֻ�ģʽ���ýӿ�
 �������  : ulModuleId       ---  �ⲿģ��PID
             usCliendId       ---  �ⲿģ��CliendId
             ucOpId           ---  �ⲿģ��OpId
 �������  : ��
 �� �� ֵ  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��12��25��
    ��    ��   : m00312079
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_ProcCFreqLockQryReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_CFREQ_LOCK_QUERY_REQ_STRU *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                         ulReceiverPid;
    VOS_UINT32                         ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ��TAF_MMA_CFREQ_LOCK_QRY_REQ_STRU */
    pstMsg = (TAF_MMA_CFREQ_LOCK_QUERY_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                                   ulSenderPid,
                                                   sizeof(TAF_MMA_CFREQ_LOCK_QUERY_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
                sizeof(TAF_MMA_CFREQ_LOCK_QUERY_REQ_STRU) - VOS_MSG_HEAD_LENGTH,
                0x00,
                sizeof(TAF_MMA_CFREQ_LOCK_QUERY_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_CDMA_FREQ_LOCK_QRY_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = ucOpId;

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_ProcCdmaCsqSetReq
 ��������  : �ֻ�ģʽ���ýӿ�
 �������  : ulModuleId       ---  �ⲿģ��PID
             usCliendId       ---  �ⲿģ��CliendId
             ucOpId           ---  �ⲿģ��OpId
             pstCdmaCsqPara   ---  CdmaCsq mode���ò���
 �������  : ��
 �� �� ֵ  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��12��25��
    ��    ��   : m00312079
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_ProcCdmaCsqSetReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_MMA_CDMACSQ_PARA_STRU          *pstCdmaCsqPara
)
{
    TAF_MMA_CDMACSQ_SET_REQ_STRU       *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������� */
    if (VOS_NULL_PTR == pstCdmaCsqPara)
    {
        return VOS_FALSE;
    }

    /* ������Ϣ��TAF_MMA_CDMA_CSQ_SET_REQ_STRU */
    pstMsg = (TAF_MMA_CDMACSQ_SET_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                       ulSenderPid,
                                       sizeof(TAF_MMA_CDMACSQ_SET_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
                sizeof(TAF_MMA_CDMACSQ_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH,
                0x00,
                sizeof(TAF_MMA_CDMACSQ_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_CDMACSQ_SET_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = ucOpId;
    TAF_MEM_CPY_S(&(pstMsg->stCdmaCsqPara), sizeof(pstMsg->stCdmaCsqPara), pstCdmaCsqPara, sizeof(TAF_MMA_CDMACSQ_PARA_STRU));

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_ProcCdmaCsqQryReq
 ��������  : �ֻ�ģʽ���ýӿ�
 �������  : ulModuleId       ---  �ⲿģ��PID
             usCliendId       ---  �ⲿģ��CliendId
             ucOpId           ---  �ⲿģ��OpId
 �������  : ��
 �� �� ֵ  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��12��25��
    ��    ��   : m00312079
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_ProcCdmaCsqQryReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_CDMACSQ_QRY_REQ_STRU       *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ��TAF_MMA_CDMACSQ_QRY_REQ_STRU */
    pstMsg = (TAF_MMA_CDMACSQ_QRY_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                       ulSenderPid,
                                       sizeof(TAF_MMA_CDMACSQ_QRY_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
                sizeof(TAF_MMA_CDMACSQ_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH,
                0x00,
                sizeof(TAF_MMA_CDMACSQ_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_CDMACSQ_QRY_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = ucOpId;

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;

}

/*****************************************************************************
 �� �� ��  : TAF_MMA_Proc1xChanSetReq
 ��������  : ����TAF_MMA_1XCHAN_SET_REQ
 �������  : ulModuleId        -  �ⲿģ��PID
             usClientId        - Client ID
             ucOpId            - ���β����ı�ʶ
             stCFreqLockPara   - ������Ƶ������Ҫ�Ĳ���
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��05��18��
    ��    ��   : z00316370
    �޸�����   : �����ɺ���
*****************************************************************************/
VOS_UINT32  TAF_MMA_Proc1xChanSetReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_MMA_CFREQ_LOCK_SET_PARA_STRU   *pstCFreqLockPara
)
{
    TAF_MMA_CFREQ_LOCK_SET_REQ_STRU    *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������� */
    if (VOS_NULL_PTR == pstCFreqLockPara)
    {
        return VOS_FALSE;
    }

    /* ������Ϣ��TAF_MMA_CFREQ_LOCK_SET_REQ_STRU */
    pstMsg = (TAF_MMA_CFREQ_LOCK_SET_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                       ulSenderPid,
                                       sizeof(TAF_MMA_CFREQ_LOCK_SET_REQ_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
            (VOS_SIZE_T)(sizeof(TAF_MMA_CFREQ_LOCK_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
            0x00,
            (VOS_SIZE_T)(sizeof(TAF_MMA_CFREQ_LOCK_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid       = ulSenderPid;
    pstMsg->ulReceiverPid     = ulReceiverPid;
    pstMsg->ulMsgName         = ID_TAF_MMA_1XCHAN_SET_REQ;
    pstMsg->ulModuleId        = ulModuleId;
    pstMsg->usClientId        = usClientId;
    pstMsg->ucOpId            = ucOpId;
    TAF_MEM_CPY_S(&pstMsg->stCFreqLockPara, sizeof(pstMsg->stCFreqLockPara), pstCFreqLockPara, sizeof(TAF_MMA_CFREQ_LOCK_SET_PARA_STRU));

    /* ������Ϣ */
    if (VOS_OK != PS_SEND_MSG(ulSenderPid, pstMsg))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_Proc1xChanQryReq
 ��������  : ��ȡ��ǰפ��channel����Ϣ�ӿ�
 �������  : ulModuleId       ---  �ⲿģ��PID
             usCliendId       ---  �ⲿģ��CliendId
             ucOpId           ---  �ⲿģ��OpId
 �������  : ��
 �� �� ֵ  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��05��18��
    ��    ��   : z00316370
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_Proc1xChanQryReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_1XCHAN_QUERY_REQ_STRU       *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                         ulReceiverPid;
    VOS_UINT32                         ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ��ID_TAF_MSG_MMA_CVER_QUERY_REQ */
    pstMsg = (TAF_MMA_1XCHAN_QUERY_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(ulSenderPid,
                                                sizeof(TAF_MMA_1XCHAN_QUERY_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
                sizeof(TAF_MMA_1XCHAN_QUERY_REQ_STRU) - VOS_MSG_HEAD_LENGTH,
                0x00,
                sizeof(TAF_MMA_1XCHAN_QUERY_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_1XCHAN_QUERY_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = ucOpId;

    /* ������Ϣ */
    if (VOS_OK != PS_SEND_MSG(ulSenderPid, pstMsg))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_ProcProRevInUseQryReq
 ��������  : ��ȡ��ǰʹ��Э��汾����Ϣ�ӿ�
 �������  : ulModuleId       ---  �ⲿģ��PID
             usCliendId       ---  �ⲿģ��CliendId
             ucOpId           ---  �ⲿģ��OpId
 �������  : ��
 �� �� ֵ  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��05��18��
    ��    ��   : z00316370
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_ProcProRevInUseQryReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_CVER_QUERY_REQ_STRU       *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                         ulReceiverPid;
    VOS_UINT32                         ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ��ID_TAF_MSG_MMA_CVER_QUERY_REQ */
    pstMsg = (TAF_MMA_CVER_QUERY_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(ulSenderPid,
                                                sizeof(TAF_MMA_CVER_QUERY_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
                sizeof(TAF_MMA_CVER_QUERY_REQ_STRU) - VOS_MSG_HEAD_LENGTH,
                0x00,
                sizeof(TAF_MMA_CVER_QUERY_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_CVER_QUERY_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = ucOpId;

    /* ������Ϣ */
    if (VOS_OK != PS_SEND_MSG(ulSenderPid, pstMsg))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_ProcStateQryReq
 ��������  : ��ȡ��ǰʹ��Э��汾����Ϣ�ӿ�
 �������  : ulModuleId       ---  �ⲿģ��PID
             usCliendId       ---  �ⲿģ��CliendId
             ucOpId           ---  �ⲿģ��OpId
 �������  : ��
 �� �� ֵ  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��05��18��
    ��    ��   : z00316370
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_ProcStateQryReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_STATE_QUERY_REQ_STRU       *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                         ulReceiverPid;
    VOS_UINT32                         ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ��ID_TAF_MSG_MMA_CVER_QUERY_REQ */
    pstMsg = (TAF_MMA_STATE_QUERY_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(ulSenderPid,
                                                sizeof(TAF_MMA_STATE_QUERY_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
                sizeof(TAF_MMA_STATE_QUERY_REQ_STRU) - VOS_MSG_HEAD_LENGTH,
                0x00,
                sizeof(TAF_MMA_STATE_QUERY_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_GETSTA_QUERY_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = ucOpId;

    /* ������Ϣ */
    if (VOS_OK != PS_SEND_MSG(ulSenderPid, pstMsg))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_ProcHPVerQryReq
 ��������  : ��ȡ��ǰʹ��Э��汾����Ϣ�ӿ�
 �������  : ulModuleId       ---  �ⲿģ��PID
             usCliendId       ---  �ⲿģ��CliendId
             ucOpId           ---  �ⲿģ��OpId
 �������  : ��
 �� �� ֵ  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��05��18��
    ��    ��   : z00316370
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_ProcCHVerQryReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_CHIGHVER_QUERY_REQ_STRU   *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                         ulReceiverPid;
    VOS_UINT32                         ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ��ID_TAF_MSG_MMA_CVER_QUERY_REQ */
    pstMsg = (TAF_MMA_CHIGHVER_QUERY_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(ulSenderPid,
                                                sizeof(TAF_MMA_CHIGHVER_QUERY_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
                sizeof(TAF_MMA_CHIGHVER_QUERY_REQ_STRU) - VOS_MSG_HEAD_LENGTH,
                0x00,
                sizeof(TAF_MMA_CHIGHVER_QUERY_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_CHIGHVER_QUERY_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = ucOpId;

    /* ������Ϣ */
    if (VOS_OK != PS_SEND_MSG(ulSenderPid, pstMsg))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}




/*****************************************************************************
 �� �� ��  : TAF_MMA_SetQuitCallBack
 ��������  : at QCCB�����
 �������  : ulModuleId       ---  �ⲿģ��PID
             usCliendId       ---  �ⲿģ��CliendId
             ucOpId           ---  �ⲿģ��OpId
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��7��9��
    ��    ��   : y00322978
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_SetQuitCallBack(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_QUIT_CALLBACK_SET_REQ_STRU *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ��ID_TAF_MMA_QUIT_CALLBACK_SET_REQ */
    pstMsg = (TAF_MMA_QUIT_CALLBACK_SET_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(ulSenderPid,
                                                sizeof(TAF_MMA_QUIT_CALLBACK_SET_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
                sizeof(TAF_MMA_QUIT_CALLBACK_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH,
                0x00,
                sizeof(TAF_MMA_QUIT_CALLBACK_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_QUIT_CALLBACK_SET_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = ucOpId;

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_SetCSidList
 ��������  : AT CMD CSIDLIST
 �������  : ulModuleId       ---  �ⲿģ��PID
             usCliendId       ---  �ⲿģ��CliendId
             ucOpId           ---  �ⲿģ��OpId
             pstWhiteSidList  ---  sid��������Ϣ
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��7��13��
    ��    ��   : y00322978
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_SetCSidList(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_MMA_OPER_LOCK_WHITE_SID_STRU   *pstWhiteSidList
)
{
    TAF_MMA_CSIDLIST_SET_REQ_STRU      *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ��ID_TAF_MSG_MMA_CVER_QUERY_REQ */
    pstMsg = (TAF_MMA_CSIDLIST_SET_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(ulSenderPid,
                                                sizeof(TAF_MMA_CSIDLIST_SET_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
                sizeof(TAF_MMA_CSIDLIST_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH,
                0x00,
                sizeof(TAF_MMA_CSIDLIST_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_CSIDLIST_SET_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = ucOpId;
    TAF_MEM_CPY_S(&pstMsg->stWhiteSidInfo, sizeof(pstMsg->stWhiteSidInfo), pstWhiteSidList, sizeof(TAF_MMA_OPER_LOCK_WHITE_SID_STRU));

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}
/*****************************************************************************
 �� �� ��  : TAF_MMA_QryCurrEmcCallBackMode
 ��������  : ��ѯ��ǰ�Ƿ��ڽ�����callbackģʽ
 �������  : ulModuleId       ---  �ⲿģ��PID
             usCliendId       ---  �ⲿģ��CliendId
             ucOpId           ---  �ⲿģ��OpId
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��7��14��
    ��    ��   : y00322978
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_QryCurrEmcCallBackMode(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_1X_EMC_CALL_BACK_QRY_REQ_STRU                  *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                                              ulReceiverPid;
    VOS_UINT32                                              ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* Allocating memory for message */
    pstMsg = (TAF_MMA_1X_EMC_CALL_BACK_QRY_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(ulSenderPid,
                                                                               sizeof(TAF_MMA_1X_EMC_CALL_BACK_QRY_REQ_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( ((VOS_UINT8 *)pstMsg + VOS_MSG_HEAD_LENGTH),
                (VOS_SIZE_T)(sizeof(TAF_MMA_1X_EMC_CALL_BACK_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
                0x00,
                (VOS_SIZE_T)(sizeof(TAF_MMA_1X_EMC_CALL_BACK_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    pstMsg->ulReceiverPid       = ulReceiverPid;
    pstMsg->ulSenderPid         = ulSenderPid;
    pstMsg->ulMsgName           = ID_TAF_MMA_1X_EMC_CALL_BACK_QRY_REQ;
    pstMsg->stCtrl.ulModuleId   = ulModuleId;
    pstMsg->stCtrl.usClientId   = usClientId;
    pstMsg->stCtrl.ucOpId       = ucOpId;

    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;

}


/*****************************************************************************
 �� �� ��  : TAF_MMA_ProcHdrCsqSetReq
 ��������  : �ֻ�ģʽ���ýӿ�
 �������  : ulModuleId       ---  �ⲿģ��PID
             usCliendId       ---  �ⲿģ��CliendId
             ucOpId           ---  �ⲿģ��OpId
             pstHDRCsqPara   ---  HDRCsq mode���ò���
 �������  : ��
 �� �� ֵ  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :2015��10��21��
    ��    ��   : C00299064
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_ProcHdrCsqSetReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_MMA_HDR_CSQ_PARA_STRU          *pstHdrCsqPara
)
{
    TAF_MMA_HDR_CSQ_SET_REQ_STRU       *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ��TAF_MMA_CDMA_CSQ_SET_REQ_STRU */
    pstMsg = (TAF_MMA_HDR_CSQ_SET_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                       ulSenderPid,
                                       sizeof(TAF_MMA_HDR_CSQ_SET_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
                sizeof(TAF_MMA_HDR_CSQ_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH,
                0x00,
                sizeof(TAF_MMA_HDR_CSQ_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_HDR_CSQ_SET_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = ucOpId;
    TAF_MEM_CPY_S(&(pstMsg->stHdrCsqSetting), sizeof(pstMsg->stHdrCsqSetting), pstHdrCsqPara, sizeof(TAF_MMA_HDR_CSQ_PARA_STRU));

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_ProcHdrCsqQryReq
 ��������  : �ֻ�ģʽ���ýӿ�
 �������  : ulModuleId       ---  �ⲿģ��PID
             usCliendId       ---  �ⲿģ��CliendId
             ucOpId           ---  �ⲿģ��OpId
 �������  : ��
 �� �� ֵ  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��10��20��
    ��    ��   : C00299064
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_ProcHdrCsqQryReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_HDR_CSQ_QRY_SETTING_REQ_STRU *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ��TAF_MMA_CDMACSQ_QRY_REQ_STRU */
    pstMsg = (TAF_MMA_HDR_CSQ_QRY_SETTING_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                       ulSenderPid,
                                       sizeof(TAF_MMA_HDR_CSQ_QRY_SETTING_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
                sizeof(TAF_MMA_HDR_CSQ_QRY_SETTING_REQ_STRU) - VOS_MSG_HEAD_LENGTH,
                0x00,
                sizeof(TAF_MMA_HDR_CSQ_QRY_SETTING_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_HDR_CSQ_QRY_SETTING_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = ucOpId;

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;

}


/*****************************************************************************
 �� �� ��  : TAF_MMA_QryCurrSidNid
 ��������  : ��ѯ��ǰϵͳפ����SID NID
 �������  : ulModuleId       ---  �ⲿģ��PID
             usCliendId       ---  �ⲿģ��CliendId
             ucOpId           ---  �ⲿģ��OpId
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��10��22��
    ��    ��   : f00279542
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_QryCurrSidNid(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_CURR_SID_NID_QRY_REQ_STRU  *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* Allocating memory for message */
    pstMsg = (TAF_MMA_CURR_SID_NID_QRY_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(ulSenderPid,
                                                                               sizeof(TAF_MMA_CURR_SID_NID_QRY_REQ_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( ((VOS_UINT8 *)pstMsg + VOS_MSG_HEAD_LENGTH),
                (VOS_SIZE_T)(sizeof(TAF_MMA_CURR_SID_NID_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
                0x00,
                (VOS_SIZE_T)(sizeof(TAF_MMA_CURR_SID_NID_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    pstMsg->ulReceiverPid       = ulReceiverPid;
    pstMsg->ulSenderPid         = ulSenderPid;
    pstMsg->ulMsgName           = ID_TAF_MMA_CURR_SID_NID_QRY_REQ;
    pstMsg->stCtrl.ulModuleId   = ulModuleId;
    pstMsg->stCtrl.usClientId   = usClientId;
    pstMsg->stCtrl.ucOpId       = ucOpId;

    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;

}

/*****************************************************************************
 �� �� ��  : TAF_MMA_QryMultiModeSidMcc
 ��������  : ��ѯ��ǰģʽ��SID/MCC
 �������  : ulModuleId       ---  �ⲿģ��PID
             usCliendId       ---  �ⲿģ��CliendId
             ucOpId           ---  �ⲿģ��OpId
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��1��20��
    ��    ��   : w00242748
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_QryCtRoamInfo(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_CTCC_ROAMING_NW_INFO_QRY_REQ_STRU              *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                                              ulReceiverPid;
    VOS_UINT32                                              ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* Allocating memory for message */
    pstMsg = (TAF_MMA_CTCC_ROAMING_NW_INFO_QRY_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(ulSenderPid,
                                                                               sizeof(TAF_MMA_CTCC_ROAMING_NW_INFO_QRY_REQ_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( ((VOS_UINT8 *)pstMsg + VOS_MSG_HEAD_LENGTH),
                (VOS_SIZE_T)(sizeof(TAF_MMA_CTCC_ROAMING_NW_INFO_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
                0x00,
                (VOS_SIZE_T)(sizeof(TAF_MMA_CTCC_ROAMING_NW_INFO_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    pstMsg->ulReceiverPid       = ulReceiverPid;
    pstMsg->ulSenderPid         = ulSenderPid;
    pstMsg->enMsgName           = ID_TAF_MMA_CTCC_ROAMING_NW_INFO_QRY_REQ;
    pstMsg->stCtrl.ulModuleId   = ulModuleId;
    pstMsg->stCtrl.usClientId   = usClientId;
    pstMsg->stCtrl.ucOpId       = ucOpId;

    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;

}

/*****************************************************************************
 �� �� ��  : TAF_MMA_SetCtOosCount
 ��������  : �����й�����OOS����
 �������  : ulModuleId       ---  �ⲿģ��PID
             usCliendId       ---  �ⲿģ��CliendId
             ucOpId           ---  �ⲿģ��OpId
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��7��9��
    ��    ��   : y00322978
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_SetCtOosCount(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    VOS_UINT32                          ulClOosCount,
    VOS_UINT32                          ulGulOosCount
)
{
    TAF_MMA_CTCC_OOS_COUNT_SET_REQ_STRU                    *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                                              ulReceiverPid;
    VOS_UINT32                                              ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ��ID_TAF_MMA_QUIT_CALLBACK_SET_REQ */
    pstMsg = (TAF_MMA_CTCC_OOS_COUNT_SET_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(ulSenderPid,
                                                sizeof(TAF_MMA_CTCC_OOS_COUNT_SET_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
                sizeof(TAF_MMA_CTCC_OOS_COUNT_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH,
                0x00,
                sizeof(TAF_MMA_CTCC_OOS_COUNT_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->enMsgName                   = ID_TAF_MMA_CTCC_OOS_COUNT_SET_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = ucOpId;
    pstMsg->usClOosCount                = (VOS_UINT16)ulClOosCount;
    pstMsg->usGulOosCount               = (VOS_UINT16)ulGulOosCount;

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_SetCtRoamInfo
 ��������  : ����CTROAMINFO�����Ƿ������ϱ�
 �������  : VOS_UINT32                          ulModuleId
             VOS_UINT16                          usClientId
             VOS_UINT8                           ucOpId
             VOS_UINT8                           ucCtRoamRtpFlag
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��3��11��
    ��    ��   : g00261581
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_SetCtRoamInfo(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    VOS_UINT8                           ucCtRoamRtpFlag
)
{
    TAF_MMA_CTCC_ROAMING_NW_INFO_RTP_CFG_SET_REQ_STRU      *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                                              ulReceiverPid;
    VOS_UINT32                                              ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ��ID_TAF_MMA_CTCC_ROAMING_NW_INFO_RTP_CFG_SET_REQ */
    pstMsg = (TAF_MMA_CTCC_ROAMING_NW_INFO_RTP_CFG_SET_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(ulSenderPid,
                                                                  sizeof(TAF_MMA_CTCC_ROAMING_NW_INFO_RTP_CFG_SET_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
                sizeof(TAF_MMA_CTCC_ROAMING_NW_INFO_RTP_CFG_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH,
                0x00,
                sizeof(TAF_MMA_CTCC_ROAMING_NW_INFO_RTP_CFG_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid       = ulSenderPid;
    pstMsg->ulReceiverPid     = ulReceiverPid;
    pstMsg->enMsgName         = ID_TAF_MMA_CTCC_ROAMING_NW_INFO_RTP_CFG_SET_REQ;
    pstMsg->stCtrl.ulModuleId = ulModuleId;
    pstMsg->stCtrl.usClientId = usClientId;
    pstMsg->stCtrl.ucOpId     = ucOpId;
    pstMsg->ucCtRoamRtpFlag   = ucCtRoamRtpFlag;

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}
/*****************************************************************************
 �� �� ��  : TAF_MMA_QryPrlIdInfo
 ��������  : ��ѯPRLID ,MLPL version id, MSPL version id
 �������  : ulModuleId       ---  �ⲿģ��PID
             usCliendId       ---  �ⲿģ��CliendId
             ucOpId           ---  �ⲿģ��OpId
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016-3-24
    ��    ��   : l00359089
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_QryPrlIdInfo(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_PRLID_QRY_REQ_STRU         *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* Allocating memory for message */
    pstMsg = (TAF_MMA_PRLID_QRY_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(ulSenderPid,
                                                                        sizeof(TAF_MMA_PRLID_QRY_REQ_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( ((VOS_UINT8 *)pstMsg + VOS_MSG_HEAD_LENGTH),
                sizeof(TAF_MMA_PRLID_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH,
                0x00,
                (VOS_SIZE_T)(sizeof(TAF_MMA_PRLID_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    pstMsg->ulReceiverPid       = ulReceiverPid;
    pstMsg->ulSenderPid         = ulSenderPid;
    pstMsg->enMsgName           = ID_TAF_MMA_PRLID_QRY_REQ;
    pstMsg->stCtrl.ulModuleId   = ulModuleId;
    pstMsg->stCtrl.usClientId   = usClientId;
    pstMsg->stCtrl.ucOpId       = ucOpId;

    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_QryRatCombinedMode
 ��������  : ��ѯ��ģ��ϵĵ�ǰģʽ
 �������  : VOS_UINT32                          ulModuleId
             VOS_UINT16                          usClientId
             VOS_UINT8                           ucOpId
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��7��18��
    ��    ��   : l00359089
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_QryRatCombinedMode(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_RAT_COMBINED_MODE_QRY_REQ_STRU                 *pstMsg;
    VOS_UINT32                                              ulReceiverPid;
    VOS_UINT32                                              ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* Allocating memory for message */
    pstMsg = (TAF_MMA_RAT_COMBINED_MODE_QRY_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(ulSenderPid,
                                                     sizeof(TAF_MMA_RAT_COMBINED_MODE_QRY_REQ_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( ((VOS_UINT8 *)pstMsg + VOS_MSG_HEAD_LENGTH),
                   sizeof(TAF_MMA_RAT_COMBINED_MODE_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH,
                   0x00,
                   (VOS_SIZE_T)(sizeof(TAF_MMA_RAT_COMBINED_MODE_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    pstMsg->ulReceiverPid       = ulReceiverPid;
    pstMsg->ulSenderPid         = ulSenderPid;
    pstMsg->enMsgName           = ID_TAF_MMA_RAT_COMBINED_MODE_QRY_REQ;
    pstMsg->stCtrl.ulModuleId   = ulModuleId;
    pstMsg->stCtrl.usClientId   = usClientId;
    pstMsg->stCtrl.ucOpId       = ucOpId;

    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}



/*****************************************************************************
 �� �� ��  : TAF_MMA_ProcResetNtf
 ��������  : ���͸�λ����
 �������  : ulModuleId        -  �ⲿģ��PID
             usClientId        - Client ID
             ucOpId            - ���β����ı�ʶ
             ucResetStep       - ������Ƶ������Ҫ�Ĳ���
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��8��13��
    ��    ��   : w00242748
    �޸�����   : �����ɺ���
*****************************************************************************/
VOS_UINT32  TAF_MMA_ProcResetNtf(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    VOS_UINT8                           ucResetStep
)
{
    TAF_MMA_RESET_NTF_STRU             *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ��TAF_MMA_CFREQ_LOCK_SET_REQ_STRU */
    pstMsg = (TAF_MMA_RESET_NTF_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                       ulSenderPid,
                                       sizeof(TAF_MMA_RESET_NTF_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
            (VOS_SIZE_T)(sizeof(TAF_MMA_RESET_NTF_STRU) - VOS_MSG_HEAD_LENGTH),
            0x00,
            (VOS_SIZE_T)(sizeof(TAF_MMA_RESET_NTF_STRU) - VOS_MSG_HEAD_LENGTH) );

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid       = ulSenderPid;
    pstMsg->ulReceiverPid     = ulReceiverPid;
    pstMsg->ulMsgName         = ID_TAF_MMA_RESET_NTF;
    pstMsg->stCtrl.ulModuleId = ulModuleId;
    pstMsg->stCtrl.usClientId = usClientId;
    pstMsg->stCtrl.ucOpId     = ucOpId;
    pstMsg->ucResetStep       = ucResetStep;

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}


/*****************************************************************************
 �� �� ��  : TAF_MMA_SetFPlmnInfo
 ��������  : �·�CFPLMN��������
 �������  : ulModuleId       ---  �ⲿģ��PID
             usCliendId       ---  �ⲿģ��CliendId
             ucOpId           ---  �ⲿģ��OpId
             pstCFPlmnPara    ---  fplmn mode���ò���
 �������  : ��
 �� �� ֵ  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��2��9��
    ��    ��   : f00279542
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_SetFPlmnInfo(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_PH_FPLMN_OPERATE_STRU          *pstCFPlmnPara
)
{
    TAF_MMA_CFPLMN_SET_REQ_STRU        *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������� */
    if (VOS_NULL_PTR == pstCFPlmnPara)
    {
        return VOS_FALSE;
    }

    /* ������Ϣ��TAF_MMA_CDMA_CSQ_SET_REQ_STRU */
    pstMsg = (TAF_MMA_CFPLMN_SET_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                            ulSenderPid,
                                            sizeof(TAF_MMA_CFPLMN_SET_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
                sizeof(TAF_MMA_CFPLMN_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH,
                0x00,
                sizeof(TAF_MMA_CFPLMN_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_CFPLMN_SET_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = ucOpId;
    TAF_MEM_CPY_S(&(pstMsg->stCFPlmnPara), sizeof(pstMsg->stCFPlmnPara), pstCFPlmnPara, sizeof(TAF_PH_FPLMN_OPERATE_STRU));

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_QryFPlmnInfo
 ��������  : fplmn ��ѯ����
 �������  : ulModuleId       ---  �ⲿģ��PID
             usCliendId       ---  �ⲿģ��CliendId
             ucOpId           ---  �ⲿģ��OpId
 �������  : ��
 �� �� ֵ  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��2��9��
    ��    ��   : f00279542
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_QryFPlmnInfo(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_CFPLMN_QUERY_REQ_STRU      *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ��TAF_MMA_CDMACSQ_QRY_REQ_STRU */
    pstMsg = (TAF_MMA_CFPLMN_QUERY_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                              ulSenderPid,
                                              sizeof(TAF_MMA_CFPLMN_QUERY_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
                sizeof(TAF_MMA_CFPLMN_QUERY_REQ_STRU) - VOS_MSG_HEAD_LENGTH,
                0x00,
                sizeof(TAF_MMA_CFPLMN_QUERY_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_CFPLMN_QUERY_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = ucOpId;

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);
    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_SetCpolReq
 ��������  : prefplmn������������ӿ�
 �������  : ulModuleId       ---  �ⲿģ��PID
             usCliendId       ---  �ⲿģ��CliendId
             ucOpId           ---  �ⲿģ��OpId
             pstPrefPlmn      ---  prefplmn ���ò���
 �������  : ��
 �� �� ֵ  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��2��3��
    ��    ��   : y00307564
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_SetCpolReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_PH_SET_PREFPLMN_STRU           *pstPrefPlmn
)
{
    TAF_MMA_PREF_PLMN_SET_REQ_STRU     *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������� */
    if (VOS_NULL_PTR == pstPrefPlmn)
    {
        return VOS_FALSE;
    }

    /* ������Ϣ��TAF_MMA_PREF_PLMN_SET_REQ_STRU */
    pstMsg = (TAF_MMA_PREF_PLMN_SET_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                       ulSenderPid,
                                       sizeof(TAF_MMA_PREF_PLMN_SET_REQ_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
            (VOS_SIZE_T)(sizeof(TAF_MMA_PREF_PLMN_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
            0x00,
            (VOS_SIZE_T)(sizeof(TAF_MMA_PREF_PLMN_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH));

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid       = ulSenderPid;
    pstMsg->ulReceiverPid     = ulReceiverPid;
    pstMsg->ulMsgName         = ID_TAF_MMA_PREF_PLMN_SET_REQ;
    pstMsg->stCtrl.ulModuleId = ulModuleId;
    pstMsg->stCtrl.usClientId = usClientId;
    pstMsg->stCtrl.ucOpId     = ucOpId;
    TAF_MEM_CPY_S(&pstMsg->stPrefPlmn, sizeof(pstMsg->stPrefPlmn), pstPrefPlmn, sizeof(TAF_PH_SET_PREFPLMN_STRU));

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_QueryCpolReq
 ��������  : prefplmn��ѯ��������ӿ�
 �������  : ulModuleId       ---  �ⲿģ��PID
             usCliendId       ---  �ⲿģ��CliendId
             ucOpId           ---  �ⲿģ��OpId
             pstCpolInfo      ---  pstCpolInfo��ѯcpol��Ϣ����
 �������  : ��
 �� �� ֵ  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��2��3��
    ��    ��   : y00307564
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_QueryCpolReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_MMA_CPOL_INFO_QUERY_REQ_STRU   *pstCpolInfo
)
{
    TAF_MMA_PREF_PLMN_QUERY_REQ_STRU   *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������� */
    if (VOS_NULL_PTR == pstCpolInfo)
    {
        return VOS_FALSE;
    }

    /* ������Ϣ��TAF_MMA_PREF_PLMN_QUERY_REQ_STRU */
    pstMsg = (TAF_MMA_PREF_PLMN_QUERY_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                       ulSenderPid,
                                       sizeof(TAF_MMA_PREF_PLMN_QUERY_REQ_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
            (VOS_SIZE_T)(sizeof(TAF_MMA_PREF_PLMN_QUERY_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
            0x00,
            (VOS_SIZE_T)(sizeof(TAF_MMA_PREF_PLMN_QUERY_REQ_STRU) - VOS_MSG_HEAD_LENGTH));

    /* ��д��Ϣͷ */
    pstMsg->ulSenderPid       = ulSenderPid;
    pstMsg->ulReceiverPid     = ulReceiverPid;
    pstMsg->ulMsgName         = ID_TAF_MMA_PREF_PLMN_QUERY_REQ;
    pstMsg->stCtrl.ulModuleId = ulModuleId;
    pstMsg->stCtrl.usClientId = usClientId;
    pstMsg->stCtrl.ucOpId     = ucOpId;

    TAF_MEM_CPY_S(&pstMsg->stCpolInfo, sizeof(pstMsg->stCpolInfo), pstCpolInfo, sizeof(TAF_MMA_CPOL_INFO_QUERY_REQ_STRU));

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_TestCpolReq
 ��������  : prefplmn������������ӿ�
 �������  : ulModuleId       ---  �ⲿģ��PID
             usCliendId       ---  �ⲿģ��CliendId
             ucOpId           ---  �ⲿģ��OpId
             enPrefPlmnType   ---  ����prefplmn����
 �������  : ��
 �� �� ֵ  : VOS_TRUE:�ɹ�,VOS_FALSE:ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2014��2��3��
    ��    ��   : y00307564
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_TestCpolReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    MN_PH_PREF_PLMN_TYPE_ENUM_U8        enPrefPlmnType
)
{
    TAF_MMA_PREF_PLMN_TEST_REQ_STRU    *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������� */
    if (MN_PH_PREF_PLMN_HPLMN < enPrefPlmnType)
    {
        return VOS_FALSE;
    }

    /* ������Ϣ��TAF_MMA_PREF_PLMN_TEST_REQ_STRU */
    pstMsg = (TAF_MMA_PREF_PLMN_TEST_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                       ulSenderPid,
                                       sizeof(TAF_MMA_PREF_PLMN_TEST_REQ_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
            (VOS_SIZE_T)(sizeof(TAF_MMA_PREF_PLMN_TEST_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
            0x00,
            (VOS_SIZE_T)(sizeof(TAF_MMA_PREF_PLMN_TEST_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid       = ulSenderPid;
    pstMsg->ulReceiverPid     = ulReceiverPid;
    pstMsg->ulMsgName         = ID_TAF_MMA_PREF_PLMN_TEST_REQ;
    pstMsg->stCtrl.ulModuleId = ulModuleId;
    pstMsg->stCtrl.usClientId = usClientId;
    pstMsg->stCtrl.ucOpId     = ucOpId;
    pstMsg->enPrefPlmnType    = enPrefPlmnType;

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_SetCerssiReq
 ��������  : Set Cerssi Cfg Req
 �������  : VOS_UINT32                          ulModuleId
             VOS_UINT16                          usClientId
             VOS_UINT8                           ucOpId
             TAF_START_INFO_IND_STRU            *pstStartInfoInd
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��3��23��
    ��    ��   : g00261581
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_SetCerssiReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_START_INFO_IND_STRU            *pstStartInfoInd
)
{
    TAF_MMA_CERSSI_SET_REQ_STRU        *pstCerssiCfg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������� */
    if (VOS_NULL_PTR == pstStartInfoInd)
    {
        return VOS_FALSE;
    }

    /* ������Ϣ��TAF_MMA_CDMA_CSQ_SET_REQ_STRU */
    pstCerssiCfg = (TAF_MMA_CERSSI_SET_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                            ulSenderPid,
                                            sizeof(TAF_MMA_CERSSI_SET_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstCerssiCfg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstCerssiCfg + VOS_MSG_HEAD_LENGTH,
                sizeof(TAF_MMA_CERSSI_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH,
                0x00,
                sizeof(TAF_MMA_CERSSI_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstCerssiCfg->ulSenderPid                 = ulSenderPid;
    pstCerssiCfg->ulReceiverPid               = ulReceiverPid;
    pstCerssiCfg->ulMsgName                   = ID_TAF_MMA_CERSSI_SET_REQ;

    pstCerssiCfg->stCtrl.ulModuleId           = ulModuleId;
    pstCerssiCfg->stCtrl.usClientId           = usClientId;
    pstCerssiCfg->stCtrl.ucOpId               = ucOpId;
    pstCerssiCfg->ucActionType                = pstStartInfoInd->ucActionType;
    pstCerssiCfg->ucRrcMsgType                = pstStartInfoInd->ucRrcMsgType;
    pstCerssiCfg->ucMinRptTimerInterval       = pstStartInfoInd->ucMinRptTimerInterval;
    pstCerssiCfg->ucSignThreshold             = pstStartInfoInd->ucSignThreshold;

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstCerssiCfg);

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_QryCerssiReq
 ��������  : Mma Proc Cerssi Qry Req
 �������  : VOS_UINT32                          ulModuleId
             VOS_UINT16                          usClientId
             VOS_UINT8                           ucOpId
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��3��24��
    ��    ��   : g00261581
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_QryCerssiReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_CERSSI_INFO_QRY_REQ_STRU   *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ��TAF_MMA_PHONE_MODE_SET_REQ_STRU */
    pstMsg = (TAF_MMA_CERSSI_INFO_QRY_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                                ulSenderPid,
                                                sizeof(TAF_MMA_CERSSI_INFO_QRY_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
                sizeof(TAF_MMA_CERSSI_INFO_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH,
                0x00,
                sizeof(TAF_MMA_CERSSI_INFO_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_CERSSI_QRY_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = ucOpId;

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}


/*****************************************************************************
 �� �� ��  : TAF_MMA_QryCrpnReq
 ��������  : AT^CRPN
 �������  : ��
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��3��24��
    ��    ��   : y00322978
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_QryCrpnReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_MMA_CRPN_QRY_PARA_STRU         *pstCrpnQryReq
)
{
    TAF_MMA_CRPN_QRY_REQ_STRU          *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ��TAF_MMA_CRPN_QRY_REQ_STRU */
    pstMsg = (TAF_MMA_CRPN_QRY_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                       ulSenderPid,
                                       sizeof(TAF_MMA_CRPN_QRY_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
       return VOS_FALSE;
    }

    TAF_MEM_SET_S( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
            (VOS_SIZE_T)(sizeof(TAF_MMA_CRPN_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
            0x00,
            (VOS_SIZE_T)(sizeof(TAF_MMA_CRPN_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_CRPN_QRY_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = ucOpId;

    if (VOS_NULL_PTR != pstCrpnQryReq)
    {
        TAF_MEM_CPY_S(&pstMsg->stCrpnQryPara, sizeof(pstMsg->stCrpnQryPara), pstCrpnQryReq, sizeof(TAF_MMA_CRPN_QRY_PARA_STRU));
    }

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_SetCmmReq
 ��������  : AT^CMM ����
 �������  : ulModuleId usClientId ucOpId *pstCrpnQryReq
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��3��24��
    ��    ��   : y00322978
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_SetCmmReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    MM_TEST_AT_CMD_STRU                *pstTestAtCmd
)
{
    TAF_MMA_CMM_SET_REQ_STRU           *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ��TAF_MMA_CMM_SET_REQ_STRU */
    pstMsg = (TAF_MMA_CMM_SET_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                       ulSenderPid,
                                       sizeof(TAF_MMA_CMM_SET_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
       return VOS_FALSE;
    }

    TAF_MEM_SET_S( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
            (VOS_SIZE_T)(sizeof(TAF_MMA_CMM_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
            0x00,
            (VOS_SIZE_T)(sizeof(TAF_MMA_CMM_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_CMM_SET_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = ucOpId;

    if (VOS_NULL_PTR != pstTestAtCmd )
    {
        TAF_MEM_CPY_S(&pstMsg->stCmmSetReq, sizeof(pstMsg->stCmmSetReq), pstTestAtCmd, sizeof(MM_TEST_AT_CMD_STRU));
    }

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}

/* Added by k902809 for Iteration 11, 2015-3-23, begin */
/*****************************************************************************
  Function Name     :   TAF_MMA_QryAcInfoReq
  Description       :   Allocate the memory for message.
                        Sends the message to MMA.
                        Returns VOS_FALSE on failure of senfing message.

  Input parameters  :   UINT32                              ulModuleId,
                        MN_CLIENT_ID_T                      ClientId,
                        MN_OPERATION_ID_T                   OpId
  Outout parameters :
  Return Value      :   VOS_FALSE or VOS_TRUE

  Modify History:
      1)  Date      :   2015-03-21
          Author    :   k902809
          Modify content :    Create
*****************************************************************************/
VOS_UINT32 TAF_MMA_QryAcInfoReq(
    VOS_UINT32                          ulModuleId,
    MN_CLIENT_ID_T                      usClientId,
    MN_OPERATION_ID_T                   ucOpId
)
{
    TAF_MMA_AC_INFO_QRY_REQ_STRU       *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* Allocating memory for message */
    pstMsg = (TAF_MMA_AC_INFO_QRY_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(ulSenderPid,
                                                                          sizeof(TAF_MMA_AC_INFO_QRY_REQ_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( ((VOS_UINT8 *)pstMsg + VOS_MSG_HEAD_LENGTH),
                (VOS_SIZE_T)(sizeof(TAF_MMA_AC_INFO_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
                0x00,
                (VOS_SIZE_T)(sizeof(TAF_MMA_AC_INFO_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    pstMsg->ulReceiverPid       = ulReceiverPid;
    pstMsg->ulSenderPid         = ulSenderPid;
    pstMsg->ulMsgName           = ID_TAF_MMA_AC_INFO_QRY_REQ;
    pstMsg->stCtrl.ulModuleId   = ulModuleId;
    pstMsg->stCtrl.usClientId   = usClientId;
    pstMsg->stCtrl.ucOpId       = ucOpId;

    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}

/*****************************************************************************
  Function Name     :   TAF_MMA_QryCopnInfoReq
  Description       :   Allocate the memory for message.
                        Sends the message to MMA
                        Returns AT_FAILURE on failure of senfing message.

  Input parameters  :   UINT32                              ulModuleId,
                        MN_CLIENT_ID_T                      ClientId,
                        VOS_UINT16                          usFromIndex,
                        MN_OPERATION_ID_T                   OpId
  Outout parameters :
  Return Value      :   VOS_FALSE or VOS_TRUE

  Modify History:
      1)  Date      :   2015-03-24
          Author    :   k902809
          Modify content :    Create
*****************************************************************************/
VOS_UINT32 TAF_MMA_QryCopnInfoReq(
    VOS_UINT32                          ulModuleId,
    MN_CLIENT_ID_T                      usClientId,
    VOS_UINT16                          usFromIndex,
    MN_OPERATION_ID_T                   ucOpId
)
{
    TAF_MMA_COPN_INFO_QRY_REQ_STRU     *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /*
    AT��MMA������Ӫ����Ϣ:
    ��Ϊ�˼���Ϣ���ƣ�����һ�λ�ȡ������Ӫ����Ϣ�����ﶨ��Ϊһ�λ�ȡ50����Ӫ����Ϣ
    ��һ��������Ϣ��������0��ʼҪ��������50����Ӫ����Ϣ
    */

    /* Allocating memory for message */
    pstMsg = (TAF_MMA_COPN_INFO_QRY_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(ulSenderPid,
                                                                            sizeof(TAF_MMA_COPN_INFO_QRY_REQ_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( ((VOS_UINT8 *)pstMsg + VOS_MSG_HEAD_LENGTH),
                (VOS_SIZE_T)(sizeof(TAF_MMA_COPN_INFO_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
                0x00,
                (VOS_SIZE_T)(sizeof(TAF_MMA_COPN_INFO_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    pstMsg->ulReceiverPid       = ulReceiverPid;
    pstMsg->ulSenderPid         = ulSenderPid;
    pstMsg->ulMsgName           = ID_TAF_MMA_COPN_INFO_QRY_REQ;
    pstMsg->stCtrl.ulModuleId   = ulModuleId;
    pstMsg->stCtrl.usClientId   = usClientId;
    pstMsg->stCtrl.ucOpId       = ucOpId;
    pstMsg->usFromIndex         = usFromIndex;
    pstMsg->usPlmnNum           = TAF_MMA_COPN_PLMN_MAX_NUM;

    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}

/*****************************************************************************
  Function Name     :   TAF_MMA_SimInsertReq
  Description       :   Allocate the memory for sim insert message.
                        Sends the message to MMA
                        Returns AT_FAILURE on failure of senfing message.

  Input parameters  :   UINT32                              ulModuleId
                        MN_CLIENT_ID_T                      ClientId
                        MN_OPERATION_ID_T                   OpId
                        TAF_SIM_INSERT_STATE_ENUM_UINT32    enSimInsertState
  Outout parameters :
  Return Value      :   VOS_FALSE or VOS_TRUE

  Modify History:
      1)  Date      :   2015-03-24
          Author    :   k902809
          Modify content :    Create
*****************************************************************************/
VOS_UINT32 TAF_MMA_SimInsertReq(
    VOS_UINT32                          ulModuleId,
    MN_CLIENT_ID_T                      usClientId,
    MN_OPERATION_ID_T                   ucOpId,
    TAF_SIM_INSERT_STATE_ENUM_UINT32    enSimInsertState)
{

    TAF_MMA_SIM_INSERT_REQ_STRU        *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* Allocating memory for message */
    pstMsg = (TAF_MMA_SIM_INSERT_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(ulSenderPid,
                                                                         sizeof(TAF_MMA_SIM_INSERT_REQ_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( ((VOS_UINT8 *)pstMsg + VOS_MSG_HEAD_LENGTH),
                (VOS_SIZE_T)(sizeof(TAF_MMA_SIM_INSERT_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
                0x00,
                (VOS_SIZE_T)(sizeof(TAF_MMA_SIM_INSERT_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    pstMsg->ulReceiverPid       = ulReceiverPid;
    pstMsg->ulSenderPid         = ulSenderPid;
    pstMsg->ulMsgName           = ID_TAF_MMA_SIM_INSERT_REQ;
    pstMsg->stCtrl.ulModuleId   = ulModuleId;
    pstMsg->stCtrl.usClientId   = usClientId;
    pstMsg->stCtrl.ucOpId       = ucOpId;
    pstMsg->enSimInsertState    = enSimInsertState;

    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}


/*****************************************************************************
  Function Name     :   TAF_MMA_SetEOPlmnReq
  Description       :   Allocate the memory for EOPlmn.
                        Sends the message to MMA
                        Returns AT_FAILURE on failure of senfing message.

  Input parameters  :   UINT32                              ulModuleId;
                        MN_CLIENT_ID_T                      ClientId,
                        MN_OPERATION_ID_T                   OpId
                        TAF_MMA_SET_EOPLMN_LIST_STRU       *pstEOPlmnCfg
  Outout parameters :
  Return Value      :   VOS_FALSE or VOS_TRUE

  Modify History:
      1)  Date      :   2015-03-24
          Author    :   k902809
          Modify content :    Create
*****************************************************************************/
VOS_UINT32 TAF_MMA_SetEOPlmnReq(
    VOS_UINT32                          ulModuleId,
    MN_CLIENT_ID_T                      usClientId,
    MN_OPERATION_ID_T                   ucOpId,
    TAF_MMA_SET_EOPLMN_LIST_STRU       *pstEOPlmnCfg
)
{
    TAF_MMA_EOPLMN_SET_REQ_STRU        *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    if (VOS_NULL_PTR == pstEOPlmnCfg)
    {
        return VOS_FALSE;
    }

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* Allocating memory for message */
    pstMsg = (TAF_MMA_EOPLMN_SET_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(ulSenderPid,
                                                                         sizeof(TAF_MMA_EOPLMN_SET_REQ_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( ((VOS_UINT8 *)pstMsg + VOS_MSG_HEAD_LENGTH),
                (VOS_SIZE_T)(sizeof(TAF_MMA_EOPLMN_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
                0x00,
                (VOS_SIZE_T)(sizeof(TAF_MMA_EOPLMN_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    pstMsg->ulReceiverPid       = ulReceiverPid;
    pstMsg->ulSenderPid         = ulSenderPid;
    pstMsg->ulMsgName           = ID_TAF_MMA_EOPLMN_SET_REQ;
    pstMsg->stCtrl.ulModuleId   = ulModuleId;
    pstMsg->stCtrl.usClientId   = usClientId;
    pstMsg->stCtrl.ucOpId       = ucOpId;

    TAF_MEM_CPY_S(&(pstMsg->stEOPlmnSetPara), sizeof(pstMsg->stEOPlmnSetPara), pstEOPlmnCfg, sizeof(TAF_MMA_SET_EOPLMN_LIST_STRU));

    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}


/*****************************************************************************
  Function Name     :   TAF_MMA_EOPlmnQryReq
  Description       :   Allocate the memory for EOPlmn.
                        Sends the message to MMA
                        Returns AT_FAILURE on failure of senfing message.

  Input parameters  :   UINT32                              ulModuleId;
                        MN_CLIENT_ID_T                      ClientId,
                        MN_OPERATION_ID_T                   OpId
                        TAF_MMA_SET_EOPLMN_LIST_STRU       *pstEOPlmnCfg
  Outout parameters :
  Return Value      :   VOS_FALSE or VOS_TRUE

  Modify History:
      1)  Date      :   2015-03-30
          Author    :   k902809
          Modify content :    Create
*****************************************************************************/
VOS_UINT32 TAF_MMA_QryEOPlmnReq(
    VOS_UINT32                          ulModuleId,
    MN_CLIENT_ID_T                      usClientId,
    MN_OPERATION_ID_T                   ucOpId
)
{
    TAF_MMA_EOPLMN_QRY_REQ_STRU        *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* Allocating memory for message */
    pstMsg = (TAF_MMA_EOPLMN_QRY_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(ulSenderPid,
                                                                         sizeof(TAF_MMA_EOPLMN_QRY_REQ_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( ((VOS_UINT8 *)pstMsg + VOS_MSG_HEAD_LENGTH),
                (VOS_SIZE_T)(sizeof(TAF_MMA_EOPLMN_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
                0x00,
                (VOS_SIZE_T)(sizeof(TAF_MMA_EOPLMN_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    pstMsg->ulReceiverPid       = ulReceiverPid;
    pstMsg->ulSenderPid         = ulSenderPid;
    pstMsg->ulMsgName           = ID_TAF_MMA_EOPLMN_QRY_REQ;
    pstMsg->stCtrl.ulModuleId   = ulModuleId;
    pstMsg->stCtrl.usClientId   = usClientId;
    pstMsg->stCtrl.ucOpId       = ucOpId;

    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}


/*****************************************************************************
  Function Name     :   TAF_MMA_QryCLocInfoReq
  Description       :   Allocate the memory for message.
                        Sends the message to MMA.
                        Returns VOS_FALSE on failure of senfing message.

  Input parameters  :   UINT32                              ulModuleId,
                        MN_CLIENT_ID_T                      ClientId,
                        MN_OPERATION_ID_T                   OpId
  Outout parameters :
  Return Value      :   VOS_FALSE or VOS_TRUE

  Modify History:
      1)  Date      :   2015-03-25
          Author    :   k902809
          Modify content :    Create
*****************************************************************************/
VOS_UINT32 TAF_MMA_QryCLocInfoReq(
    VOS_UINT32                          ulModuleId,
    MN_CLIENT_ID_T                      usClientId,
    MN_OPERATION_ID_T                   ucOpId
)
{
    TAF_MMA_CDMA_LOCINFO_QRY_REQ_STRU  *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* Allocating memory for message */
    pstMsg = (TAF_MMA_CDMA_LOCINFO_QRY_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(ulSenderPid,
                                                                               sizeof(TAF_MMA_CDMA_LOCINFO_QRY_REQ_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( ((VOS_UINT8 *)pstMsg + VOS_MSG_HEAD_LENGTH),
                (VOS_SIZE_T)(sizeof(TAF_MMA_CDMA_LOCINFO_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
                0x00,
                (VOS_SIZE_T)(sizeof(TAF_MMA_CDMA_LOCINFO_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    pstMsg->ulReceiverPid       = ulReceiverPid;
    pstMsg->ulSenderPid         = ulSenderPid;
    pstMsg->ulMsgName           = ID_TAF_MMA_CDMA_LOCINFO_QRY_REQ;
    pstMsg->stCtrl.ulModuleId   = ulModuleId;
    pstMsg->stCtrl.usClientId   = usClientId;
    pstMsg->stCtrl.ucOpId       = ucOpId;

    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}

/*****************************************************************************
  Function Name     :   TAF_MMA_NetScanReq
  Description       :   Allocate the memory for message.
                        Sends the message to MMA.
                        Returns VOS_FALSE on failure of senfing message.

  Input parameters  :   UINT32                              ulModuleId,
                        MN_CLIENT_ID_T                      ClientId,
                        MN_OPERATION_ID_T                   OpId,
                        TAF_MMA_NET_SCAN_REQ_STRU          *pstRecvNetScanSetPara
  Outout parameters :
  Return Value      :   VOS_FALSE or VOS_TRUE

  Modify History:
      1)  Date      :   2015-03-21
          Author    :   k902809
          Modify content :    Create
*****************************************************************************/
VOS_UINT32 TAF_MMA_NetScanReq(
    VOS_UINT32                          ulModuleId,
    MN_CLIENT_ID_T                      usClientId,
    MN_OPERATION_ID_T                   ucOpId,
    TAF_MMA_NET_SCAN_REQ_STRU          *pstRecvNetScanSetPara
)
{
    TAF_MMA_NET_SCAN_REQ_STRU          *pstNetScanSetPara = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    if (VOS_NULL_PTR == pstRecvNetScanSetPara)
    {
        return VOS_FALSE;
    }

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* Allocating memory for message */
    pstNetScanSetPara = (TAF_MMA_NET_SCAN_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(ulSenderPid,
                                                                                  sizeof(TAF_MMA_NET_SCAN_REQ_STRU));
    if (VOS_NULL_PTR == pstNetScanSetPara)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( ((VOS_UINT8 *)pstNetScanSetPara + VOS_MSG_HEAD_LENGTH),
                (VOS_SIZE_T)(sizeof(TAF_MMA_NET_SCAN_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
                0x00,
                (VOS_SIZE_T)(sizeof(TAF_MMA_NET_SCAN_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    pstNetScanSetPara->ulReceiverPid       = ulReceiverPid;
    pstNetScanSetPara->ulSenderPid         = ulSenderPid;
    pstNetScanSetPara->ulMsgName           = ID_TAF_MMA_NET_SCAN_REQ;
    pstNetScanSetPara->stCtrl.ulModuleId   = ulModuleId;
    pstNetScanSetPara->stCtrl.usClientId   = usClientId;
    pstNetScanSetPara->stCtrl.ucOpId       = ucOpId;
    pstNetScanSetPara->usCellNum           = pstRecvNetScanSetPara->usCellNum;
    pstNetScanSetPara->enRat               = pstRecvNetScanSetPara->enRat;
    pstNetScanSetPara->sCellPow            = pstRecvNetScanSetPara->sCellPow;
    TAF_MEM_CPY_S(&pstNetScanSetPara->stBand, sizeof(pstNetScanSetPara->stBand), &pstRecvNetScanSetPara->stBand, sizeof(TAF_USER_SET_PREF_BAND64));

    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstNetScanSetPara);

    return VOS_TRUE;
}

/*****************************************************************************
  Function Name     :   TAF_MMA_NetScanAbortReq
  Description       :   Allocate the memory for message.
                        Sends the message to MMA.
                        Returns VOS_FALSE on failure of senfing message.

  Input parameters  :   UINT32                              ulModuleId,
                        MN_CLIENT_ID_T                      ClientId,
                        MN_OPERATION_ID_T                   OpId
  Outout parameters :
  Return Value      :   VOS_FALSE or VOS_TRUE

  Modify History:
      1)  Date      :   2015-03-21
          Author    :   k902809
          Modify content :    Create
*****************************************************************************/
VOS_UINT32 TAF_MMA_NetScanAbortReq(
    VOS_UINT32                          ulModuleId,
    MN_CLIENT_ID_T                      usClientId,
    MN_OPERATION_ID_T                   ucOpId
)
{
    TAF_MMA_NET_SCAN_ABORT_REQ_STRU    *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* Allocating memory for message */
    pstMsg = (TAF_MMA_NET_SCAN_ABORT_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(ulSenderPid,
                                                                             sizeof(TAF_MMA_NET_SCAN_ABORT_REQ_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( ((VOS_UINT8 *)pstMsg + VOS_MSG_HEAD_LENGTH),
                (VOS_SIZE_T)(sizeof(TAF_MMA_NET_SCAN_ABORT_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
                0x00,
                (VOS_SIZE_T)(sizeof(TAF_MMA_NET_SCAN_ABORT_REQ_STRU) - VOS_MSG_HEAD_LENGTH));

    pstMsg->ulReceiverPid       = ulReceiverPid;
    pstMsg->ulSenderPid         = ulSenderPid;
    pstMsg->ulMsgName           = ID_TAF_MMA_NET_SCAN_ABORT_REQ;
    pstMsg->stCtrl.ulModuleId   = ulModuleId;
    pstMsg->stCtrl.usClientId   = usClientId;
    pstMsg->stCtrl.ucOpId       = ucOpId;

    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}


/*****************************************************************************
  Function Name     :   TAF_MMA_QrySpnReq
  Description       :   Allocate the memory for message.
                        Sends the message to MMA.
                        Returns VOS_FALSE on failure of senfing message.

  Input parameters  :   UINT32                              ulModuleId,
                        MN_CLIENT_ID_T                      ClientId,
                        MN_OPERATION_ID_T                   OpId
  Outout parameters :
  Return Value      :   VOS_FALSE or VOS_TRUE

  Modify History:
      1)  Date      :   2015-03-30
          Author    :   k902809
          Modify content :    Create
*****************************************************************************/
VOS_UINT32 TAF_MMA_QrySpnReq(
    VOS_UINT32                          ulModuleId,
    MN_CLIENT_ID_T                      usClientId,
    MN_OPERATION_ID_T                   ucOpId
)
{
    TAF_MMA_SPN_QRY_REQ_STRU           *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* Allocating memory for message */
    pstMsg = (TAF_MMA_SPN_QRY_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(ulSenderPid,
                                                                      sizeof(TAF_MMA_SPN_QRY_REQ_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( ((VOS_UINT8 *)pstMsg + VOS_MSG_HEAD_LENGTH),
                (VOS_SIZE_T)(sizeof(TAF_MMA_SPN_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
                0x00,
                (VOS_SIZE_T)(sizeof(TAF_MMA_SPN_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    pstMsg->ulReceiverPid       = ulReceiverPid;
    pstMsg->ulSenderPid         = ulSenderPid;
    pstMsg->ulMsgName           = ID_TAF_MMA_SPN_QRY_REQ;
    pstMsg->stCtrl.ulModuleId   = ulModuleId;
    pstMsg->stCtrl.usClientId   = usClientId;
    pstMsg->stCtrl.ucOpId       = ucOpId;

    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}



/*****************************************************************************
  Function Name     :   TAF_MMA_QryMMPlmnInfoReq
  Description       :   Allocate the memory for message.
                        Sends the message to MMA.
                        Returns VOS_FALSE on failure of senfing message.

  Input parameters  :   UINT32                              ulModuleId,
                        MN_CLIENT_ID_T                      ClientId,
                        MN_OPERATION_ID_T                   OpId
  Outout parameters :
  Return Value      :   VOS_FALSE or VOS_TRUE

  Modify History:
      1)  Date      :   2015-03-30
          Author    :   k902809
          Modify content :    Create
*****************************************************************************/
VOS_UINT32 TAF_MMA_QryMMPlmnInfoReq(
    VOS_UINT32                          ulModuleId,
    MN_CLIENT_ID_T                      usClientId,
    MN_OPERATION_ID_T                   ucOpId
)
{
    TAF_MMA_MMPLMNINFO_QRY_REQ_STRU    *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* Allocating memory for message */
    pstMsg = (TAF_MMA_MMPLMNINFO_QRY_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(ulSenderPid,
                                                                             sizeof(TAF_MMA_MMPLMNINFO_QRY_REQ_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( ((VOS_UINT8 *)pstMsg + VOS_MSG_HEAD_LENGTH),
                (VOS_SIZE_T)(sizeof(TAF_MMA_MMPLMNINFO_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
                0x00,
                (VOS_SIZE_T)(sizeof(TAF_MMA_MMPLMNINFO_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    pstMsg->ulReceiverPid       = ulReceiverPid;
    pstMsg->ulSenderPid         = ulSenderPid;
    pstMsg->ulMsgName           = ID_TAF_MMA_MMPLMNINFO_QRY_REQ;
    pstMsg->stCtrl.ulModuleId   = ulModuleId;
    pstMsg->stCtrl.usClientId   = usClientId;
    pstMsg->stCtrl.ucOpId       = ucOpId;

    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}

/*****************************************************************************
  Function Name     :   TAF_MMA_QryPlmnReq
  Description       :   Allocate the memory for message.
                        Sends the message to MMA.
                        Returns VOS_FALSE on failure of senfing message.

  Input parameters  :   UINT32                              ulModuleId,
                        MN_CLIENT_ID_T                      ClientId,
                        MN_OPERATION_ID_T                   OpId
  Outout parameters :
  Return Value      :   VOS_FALSE or VOS_TRUE

  Modify History:
      1)  Date      :   2015-03-30
          Author    :   k902809
          Modify content :    Create
*****************************************************************************/
VOS_UINT32 TAF_MMA_QryPlmnReq(
    VOS_UINT32                          ulModuleId,
    MN_CLIENT_ID_T                      usClientId,
    MN_OPERATION_ID_T                   ucOpId
)
{
    TAF_MMA_LAST_CAMP_PLMN_QRY_REQ_STRU    *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                              ulReceiverPid;
    VOS_UINT32                              ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* Allocating memory for message */
    pstMsg = (TAF_MMA_LAST_CAMP_PLMN_QRY_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(ulSenderPid,
                                                    sizeof(TAF_MMA_LAST_CAMP_PLMN_QRY_REQ_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( ((VOS_UINT8 *)pstMsg + VOS_MSG_HEAD_LENGTH),
                (VOS_SIZE_T)(sizeof(TAF_MMA_LAST_CAMP_PLMN_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
                0x00,
                (VOS_SIZE_T)(sizeof(TAF_MMA_LAST_CAMP_PLMN_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH));

    pstMsg->ulReceiverPid       = ulReceiverPid;
    pstMsg->ulSenderPid         = ulSenderPid;
    pstMsg->ulMsgName           = ID_TAF_MMA_LAST_CAMP_PLMN_QRY_REQ;
    pstMsg->stCtrl.ulModuleId   = ulModuleId;
    pstMsg->stCtrl.usClientId   = usClientId;
    pstMsg->stCtrl.ucOpId       = ucOpId;

    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}

/*****************************************************************************
  Function Name     :   TAF_MMA_QryUserSrvStateReq
  Description       :   Allocate the memory for message.
                        Sends the message to MMA.
                        Returns VOS_FALSE on failure of senfing message.

  Input parameters  :   UINT32                              ulModuleId,
                        MN_CLIENT_ID_T                      ClientId,
                        MN_OPERATION_ID_T                   OpId
  Outout parameters :
  Return Value      :   VOS_FALSE or VOS_TRUE

  Modify History:
      1)  Date      :   2015-03-27
          Author    :   k902809
          Modify content :    Create
*****************************************************************************/
VOS_UINT32 TAF_MMA_QryUserSrvStateReq(
    VOS_UINT32                          ulModuleId,
    MN_CLIENT_ID_T                      usClientId,
    MN_OPERATION_ID_T                   ucOpId
)
{
    TAF_MMA_USER_SRV_STATE_QRY_REQ_STRU    *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                              ulReceiverPid;
    VOS_UINT32                              ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* Allocating memory for message */
    pstMsg = (TAF_MMA_USER_SRV_STATE_QRY_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(ulSenderPid,
                                                    sizeof(TAF_MMA_USER_SRV_STATE_QRY_REQ_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( ((VOS_UINT8 *)pstMsg + VOS_MSG_HEAD_LENGTH),
                (VOS_SIZE_T)(sizeof(TAF_MMA_USER_SRV_STATE_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
                0x00,
                (VOS_SIZE_T)(sizeof(TAF_MMA_USER_SRV_STATE_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    pstMsg->ulReceiverPid       = ulReceiverPid;
    pstMsg->ulSenderPid         = ulSenderPid;
    pstMsg->ulMsgName           = ID_TAF_MMA_USER_SRV_STATE_QRY_REQ;
    pstMsg->stCtrl.ulModuleId   = ulModuleId;
    pstMsg->stCtrl.usClientId   = usClientId;
    pstMsg->stCtrl.ucOpId       = ucOpId;

    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}


/*****************************************************************************
  Function Name     :   TAF_MMA_QryApPwrOnAndRegTimeReq
  Description       :   Allocate the memory for message.
                        Sends the message to MMA.
                        Returns VOS_FALSE on failure of senfing message.

  Input parameters  :   UINT32                              ulModuleId,
                        MN_CLIENT_ID_T                      ClientId,
                        MN_OPERATION_ID_T                   OpId
  Outout parameters :
  Return Value      :   VOS_FALSE or VOS_TRUE

  Modify History:
      1)  Date      :   2015-03-27
          Author    :   k902809
          Modify content :    Create
*****************************************************************************/
VOS_UINT32 TAF_MMA_QryApPwrOnAndRegTimeReq(
    VOS_UINT32                          ulModuleId,
    MN_CLIENT_ID_T                      usClientId,
    MN_OPERATION_ID_T                   ucOpId
)
{
    TAF_MMA_POWER_ON_AND_REG_TIME_QRY_REQ_STRU *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                                  ulReceiverPid;
    VOS_UINT32                                  ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* Allocating memory for message */
    pstMsg = (TAF_MMA_POWER_ON_AND_REG_TIME_QRY_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(ulSenderPid,
                                                           sizeof(TAF_MMA_POWER_ON_AND_REG_TIME_QRY_REQ_STRU));
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( ((VOS_UINT8 *)pstMsg + VOS_MSG_HEAD_LENGTH),
                (VOS_SIZE_T)(sizeof(TAF_MMA_POWER_ON_AND_REG_TIME_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
                0x00,
                (VOS_SIZE_T)(sizeof(TAF_MMA_POWER_ON_AND_REG_TIME_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    pstMsg->ulReceiverPid       = ulReceiverPid;
    pstMsg->ulSenderPid         = ulSenderPid;
    pstMsg->ulMsgName           = ID_TAF_MMA_POWER_ON_AND_REG_TIME_QRY_REQ;
    pstMsg->stCtrl.ulModuleId   = ulModuleId;
    pstMsg->stCtrl.usClientId   = usClientId;
    pstMsg->stCtrl.ucOpId       = ucOpId;

    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}

/* Added by k902809 for Iteration 11, Iteration 11 2015-3-23, end */

/*****************************************************************************
 �� �� ��  : TAF_MMA_SetAutoAttachReq
 ��������  : AT^CAATT�����
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��3��25��
    ��    ��   : y00322978
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32  TAF_MMA_SetAutoAttachReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    VOS_UINT32                          ulSetValue
)
{
    TAF_MMA_AUTO_ATTACH_SET_REQ_STRU   *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ��TAF_MMA_AUTO_ATTACH_SET_REQ_STRU */
    pstMsg = (TAF_MMA_AUTO_ATTACH_SET_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                       ulSenderPid,
                                       sizeof(TAF_MMA_AUTO_ATTACH_SET_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
            (VOS_SIZE_T)(sizeof(TAF_MMA_AUTO_ATTACH_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
            0x00,
            (VOS_SIZE_T)(sizeof(TAF_MMA_AUTO_ATTACH_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_AUTO_ATTACH_SET_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = ucOpId;
    pstMsg->ulAutoAttachEnable          = ulSetValue;

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_TestSysCfgReq
 ��������  : TEST SYSCFG
 �������  : VOS_UINT32                          ulModuleId,
             VOS_UINT16                          usClientId,
             VOS_UINT8                           ucOpId
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��3��26��
    ��    ��   : y00322978
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_TestSysCfgReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_SYSCFG_TEST_REQ_STRU       *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ��TAF_MMA_SYSCFG_TEST_REQ_STRU */
    pstMsg = (TAF_MMA_SYSCFG_TEST_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                       ulSenderPid,
                                       sizeof(TAF_MMA_SYSCFG_TEST_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
            (VOS_SIZE_T)(sizeof(TAF_MMA_SYSCFG_TEST_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
            0x00,
            (VOS_SIZE_T)(sizeof(TAF_MMA_SYSCFG_TEST_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_SYSCFG_TEST_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = ucOpId;

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);
    return VOS_TRUE;
}


/*****************************************************************************
 �� �� ��  : TAF_MMA_QryAccessModeReq
 ��������  : Phone mode qry req
 �������  : VOS_UINT32                          ulModuleId
             VOS_UINT16                          usClientId
             VOS_UINT8                           ucOpId
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��3��27��
    ��    ��   : g00261581
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_QryAccessModeReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_ACCESS_MODE_QRY_REQ_STRU   *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ��TAF_MMA_ACCESS_MODE_QRY_REQ_STRU */
    pstMsg = (TAF_MMA_ACCESS_MODE_QRY_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                       ulSenderPid,
                                       sizeof(TAF_MMA_ACCESS_MODE_QRY_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
                (VOS_SIZE_T)(sizeof(TAF_MMA_ACCESS_MODE_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
                0x00,
                (VOS_SIZE_T)(sizeof(TAF_MMA_ACCESS_MODE_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_ACCESS_MODE_QRY_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = ucOpId;

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_QryCopsInfoReq
 ��������  : Cops Info qry req
 �������  : VOS_UINT32                          ulModuleId
             VOS_UINT16                          usClientId
             VOS_UINT8                           ucOpId
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��3��27��
    ��    ��   : g00261581
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_QryCopsInfoReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_COPS_QRY_REQ_STRU          *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ��TAF_MMA_COPS_QRY_REQ_STRU */
    pstMsg = (TAF_MMA_COPS_QRY_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                        ulSenderPid,
                                        sizeof(TAF_MMA_COPS_QRY_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
                (VOS_SIZE_T)(sizeof(TAF_MMA_COPS_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
                0x00,
                (VOS_SIZE_T)(sizeof(TAF_MMA_COPS_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_COPS_QRY_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = ucOpId;

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_SetEflociInfo
 ��������  : TAF��MMA����EflociInfo��������
 �������  :
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��07��18��
    ��    ��   : h00360002
    �޸�����   : ��������

*****************************************************************************/
VOS_UINT32 TAF_MMA_SetEflociInfo(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_MMA_EFLOCIINFO_STRU            *pstEfLociInfo
)
{
    TAF_MMA_EFLOCIINFO_SET_REQ_STRU    *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ�� */
    pstMsg = (TAF_MMA_EFLOCIINFO_SET_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(ulSenderPid, sizeof(TAF_MMA_EFLOCIINFO_SET_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
               (VOS_SIZE_T)(sizeof(TAF_MMA_EFLOCIINFO_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
               0x00,
               (VOS_SIZE_T)(sizeof(TAF_MMA_EFLOCIINFO_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH));

    pstMsg->ulSenderPid       = ulSenderPid;
    pstMsg->ulReceiverPid     = ulReceiverPid;
    pstMsg->enMsgName         = ID_TAF_MMA_EFLOCIINFO_SET_REQ;
    pstMsg->stCtrl.ucOpId     = ucOpId;
    pstMsg->stCtrl.ulModuleId = ulModuleId;
    pstMsg->stCtrl.usClientId = usClientId;

    TAF_MEM_CPY_S( &(pstMsg->stEflociInfo),
                   sizeof(TAF_MMA_EFLOCIINFO_STRU),
                   pstEfLociInfo,
                   sizeof(TAF_MMA_EFLOCIINFO_STRU));

    /* ��Ϣ����*/
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_QryEflociInfo
 ��������  : TAF��MMA����EflociInfo��ѯ����
 �������  :
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��07��18��
    ��    ��   : h00360002
    �޸�����   : ��������

*****************************************************************************/
VOS_UINT32 TAF_MMA_QryEflociInfo(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_EFLOCIINFO_QRY_REQ_STRU    *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ�� */
    pstMsg = (TAF_MMA_EFLOCIINFO_QRY_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(ulSenderPid, sizeof(TAF_MMA_EFLOCIINFO_QRY_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
               (VOS_SIZE_T)(sizeof(TAF_MMA_EFLOCIINFO_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
               0x00,
               (VOS_SIZE_T)(sizeof(TAF_MMA_EFLOCIINFO_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH));

    pstMsg->ulSenderPid       = ulSenderPid;
    pstMsg->ulReceiverPid     = ulReceiverPid;
    pstMsg->enMsgName         = ID_TAF_MMA_EFLOCIINFO_QRY_REQ;
    pstMsg->stCtrl.ucOpId     = ucOpId;
    pstMsg->stCtrl.ulModuleId = ulModuleId;
    pstMsg->stCtrl.usClientId = usClientId;

    /* ��Ϣ����*/
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_SetPsEflociInfo
 ��������  : TAF��MMA����PsEflociInfo��������
 �������  :
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��07��18��
    ��    ��   : h00360002
    �޸�����   : ��������

*****************************************************************************/
VOS_UINT32 TAF_MMA_SetPsEflociInfo(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_MMA_EFPSLOCIINFO_STRU          *pstPsefLociInfo
)
{
    TAF_MMA_EFPSLOCIINFO_SET_REQ_STRU  *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ�� */
    pstMsg = (TAF_MMA_EFPSLOCIINFO_SET_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(ulSenderPid, sizeof(TAF_MMA_EFPSLOCIINFO_SET_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
               (VOS_SIZE_T)(sizeof(TAF_MMA_EFPSLOCIINFO_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
               0x00,
               (VOS_SIZE_T)(sizeof(TAF_MMA_EFPSLOCIINFO_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH));

    pstMsg->ulSenderPid       = ulSenderPid;
    pstMsg->ulReceiverPid     = ulReceiverPid;
    pstMsg->enMsgName         = ID_TAF_MMA_EFPSLOCIINFO_SET_REQ;
    pstMsg->stCtrl.ucOpId     = ucOpId;
    pstMsg->stCtrl.ulModuleId = ulModuleId;
    pstMsg->stCtrl.usClientId = usClientId;

    TAF_MEM_CPY_S( &(pstMsg->stPsEflociInfo),
                   sizeof(TAF_MMA_EFPSLOCIINFO_STRU),
                   pstPsefLociInfo,
                   sizeof(TAF_MMA_EFPSLOCIINFO_STRU));

    /* ��Ϣ����*/
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_QryPsEflociInfo
 ��������  : TAF��MMA����PsEflociInfo��������
 �������  :
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��07��18��
    ��    ��   : h00360002
    �޸�����   : ��������

*****************************************************************************/
VOS_UINT32 TAF_MMA_QryPsEflociInfo(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_EFPSLOCIINFO_QRY_REQ_STRU  *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ�� */
    pstMsg = (TAF_MMA_EFPSLOCIINFO_QRY_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(ulSenderPid, sizeof(TAF_MMA_EFPSLOCIINFO_QRY_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
               (VOS_SIZE_T)(sizeof(TAF_MMA_EFPSLOCIINFO_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
               0x00,
               (VOS_SIZE_T)(sizeof(TAF_MMA_EFPSLOCIINFO_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH));

    pstMsg->ulSenderPid       = ulSenderPid;
    pstMsg->ulReceiverPid     = ulReceiverPid;
    pstMsg->enMsgName         = ID_TAF_MMA_EFPSLOCIINFO_QRY_REQ;
    pstMsg->stCtrl.ucOpId     = ucOpId;
    pstMsg->stCtrl.ulModuleId = ulModuleId;
    pstMsg->stCtrl.usClientId = usClientId;

    /* ��Ϣ����*/
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_QryDplmnListReq
 ��������  : TAF��MMA���� ��Ϣ
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��10��16��
    ��    ��   : n00355355
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_QryDplmnListReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_DPLMN_QRY_REQ_STRU         *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ�� */
    pstMsg = (TAF_MMA_DPLMN_QRY_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(ulSenderPid, sizeof(TAF_MMA_DPLMN_QRY_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
                (VOS_SIZE_T)(sizeof(TAF_MMA_DPLMN_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
                0x00,
                (VOS_SIZE_T)(sizeof(TAF_MMA_DPLMN_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    /* ��д��Ϣͷ */
    pstMsg->ulSenderCpuId               = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverCpuId             = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_DPLMN_QRY_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = ucOpId;

    /* ��Ϣ����*/
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_SetDplmnListReq
 ��������  : AT��MMA����SET DPLMN LIST REQ ��Ϣ
 �������  : VOS_UINT32                          ulModuleId,
             VOS_UINT16                          usClientId,
             VOS_UINT8                           ucSeq,
             VOS_UINT8                          *pucVersionId,
             VOS_UINT16                          usParaLen
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��10��13��
    ��    ��   : n00355355
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_SetDplmnListReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucSeq,
    VOS_UINT8                          *pucVersion,
    TAF_MMA_DPLMN_INFO_SET_STRU        *pstDplmnInfo
)
{
    TAF_MMA_DPLMN_SET_REQ_STRU         *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ�� */
    pstMsg = (TAF_MMA_DPLMN_SET_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                        WUEPS_PID_TAF,
                                        sizeof(TAF_MMA_DPLMN_SET_REQ_STRU));

    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    /* �����Ϣ���� */
    TAF_MEM_SET_S((VOS_UINT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
               (VOS_SIZE_T)sizeof(TAF_MMA_DPLMN_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH,
                0x00,
               (VOS_SIZE_T)sizeof(TAF_MMA_DPLMN_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    /* �����Ϣͷ */
    pstMsg->ulSenderCpuId               = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverCpuId             = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_DPLMN_SET_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = 0;

    /* �����ˮ�š��汾��*/
    pstMsg->ucSeq                       = ucSeq;
    TAF_MEM_CPY_S( pstMsg->aucVersionId,
                sizeof(pstMsg->aucVersionId),
                pucVersion,
                TAF_MMA_VERSION_INFO_LEN);

    if ( pstDplmnInfo->ucEhPlmnNum > TAF_MMA_MAX_EHPLMN_NUM )
    {
        pstDplmnInfo->ucEhPlmnNum = TAF_MMA_MAX_EHPLMN_NUM;
    }

    if ( pstDplmnInfo->usDplmnNum > TAF_MMA_MAX_DPLMN_NUM )
    {
        pstDplmnInfo->usDplmnNum = TAF_MMA_MAX_DPLMN_NUM;
    }

    /* ���HPLMN������HPLMN�б�Ԥ��DPLMN������DPLMN�б� */
    pstMsg->stDplmnInfo.ucEhPlmnNum     = pstDplmnInfo->ucEhPlmnNum;
    pstMsg->stDplmnInfo.usDplmnNum      = pstDplmnInfo->usDplmnNum;
    TAF_MEM_CPY_S(pstMsg->stDplmnInfo.astEhPlmnInfo,
               sizeof(pstMsg->stDplmnInfo.astEhPlmnInfo),
               pstDplmnInfo->astEhPlmnInfo,
               sizeof(TAF_PLMN_ID_STRU) * pstDplmnInfo->ucEhPlmnNum);
    TAF_MEM_CPY_S(pstMsg->stDplmnInfo.astDplmnList,
               sizeof(pstMsg->stDplmnInfo.astDplmnList),
               pstDplmnInfo->astDplmnList,
               sizeof(TAF_MMA_PLMN_WITH_SIM_RAT_STRU) * pstDplmnInfo->usDplmnNum );

    /* ������Ϣ */
    if (VOS_OK != PS_SEND_MSG(ulSenderPid, pstMsg))
    {
        return VOS_FALSE;
    }

    return VOS_TRUE;
}

/* Added by s00217060 for �߾������Ż�PhaseI, 2016-8-20, begin */
/*****************************************************************************
 �� �� ��  : TAF_MMA_SetBorderInfoReq
 ��������  : AT��MMA����SET_BORDER_INFO_REQ ��Ϣ
 �������  : VOS_UINT32                          ulModuleId
             VOS_UINT16                          usClientId
             VOS_UINT8                           ucSeq
             VOS_UINT8                          *pucVersion
             TAF_MMA_DPLMN_INFO_SET_STRU        *pstDplmnInfo
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��8��20��
    ��    ��   : s00217060
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_SetBorderInfoReq(
    VOS_UINT32                                              ulModuleId,
    VOS_UINT16                                              usClientId,
    TAF_MMA_SET_BORDER_INFO_OPERATE_TYPE_ENUM_UINT8         enOperateType,
    TAF_MMA_BORDER_INFO_STRU                               *pstBorderInfo
)
{
    TAF_MMA_BORDER_INFO_SET_REQ_STRU   *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ�� */
    pstMsg = (TAF_MMA_BORDER_INFO_SET_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                        WUEPS_PID_TAF,
                                        sizeof(TAF_MMA_BORDER_INFO_SET_REQ_STRU) + pstBorderInfo->ulBorderInfoLen - 4);

    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    /* �����Ϣ���� */
    TAF_MEM_SET_S((VOS_UINT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
               (VOS_SIZE_T)sizeof(TAF_MMA_BORDER_INFO_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH + pstBorderInfo->ulBorderInfoLen - 4,
                0x00,
               (VOS_SIZE_T)sizeof(TAF_MMA_BORDER_INFO_SET_REQ_STRU) - VOS_MSG_HEAD_LENGTH + pstBorderInfo->ulBorderInfoLen - 4);

    /* �����Ϣͷ */
    pstMsg->ulSenderCpuId               = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverCpuId             = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_BORDER_INFO_SET_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = 0;

    /* �����Ϣ���� */
    pstMsg->enOperateType               = enOperateType;

    TAF_MEM_CPY_S(&(pstMsg->stBorderInfo),
                  sizeof(TAF_MMA_BORDER_INFO_STRU) + pstBorderInfo->ulBorderInfoLen - 4,
                  pstBorderInfo,
                  sizeof(TAF_MMA_BORDER_INFO_STRU) + pstBorderInfo->ulBorderInfoLen - 4);

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_QryBorderInfoReq
 ��������  : ���Ͳ�ѯ��ϢID_TAF_MMA_BORDER_INFO_QRY_REQ
 �������  : VOS_UINT32                          ulModuleId
             VOS_UINT16                          usClientId
             VOS_UINT8                           ucOpId
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��8��24��
    ��    ��   : s00217060
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_QryBorderInfoReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_BORDER_INFO_QRY_REQ_STRU   *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ�� */
    pstMsg = (TAF_MMA_BORDER_INFO_QRY_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(ulSenderPid, sizeof(TAF_MMA_BORDER_INFO_QRY_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
                (VOS_SIZE_T)(sizeof(TAF_MMA_BORDER_INFO_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
                0x00,
                (VOS_SIZE_T)(sizeof(TAF_MMA_BORDER_INFO_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    /* ��д��Ϣͷ */
    pstMsg->ulSenderCpuId               = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverCpuId             = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_BORDER_INFO_QRY_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = ucOpId;

    /* ��Ϣ����*/
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}
/* Added by s00217060 for �߾������Ż�PhaseI, 2016-8-20, end */

/*****************************************************************************
 �� �� ��  : TAF_MMA_QryRegStateReq
 ��������  : Qry reg state info
 �������  : VOS_UINT32                          ulModuleId
             VOS_UINT16                          usClientId
             VOS_UINT8                           ucOpId
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��3��27��
    ��    ��   : g00261581
    �޸�����   : �����ɺ���
  2.��    ��   : 2015��8��21��
    ��    ��   : w00176964
    �޸�����   : DTS2015081907463
*****************************************************************************/
VOS_UINT32 TAF_MMA_QryRegStateReq(
    VOS_UINT32                                              ulModuleId,
    VOS_UINT16                                              usClientId,
    VOS_UINT8                                               ucOpId,
    TAF_MMA_QRY_REG_STATUS_TYPE_ENUM_UINT32                 enRegStaType
)
{
    TAF_MMA_REG_STATE_QRY_REQ_STRU     *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ��TAF_MMA_REG_STATE_QRY_REQ_STRU */
    pstMsg = (TAF_MMA_REG_STATE_QRY_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                              ulSenderPid,
                                              sizeof(TAF_MMA_REG_STATE_QRY_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
                (VOS_SIZE_T)(sizeof(TAF_MMA_REG_STATE_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
                0x00,
                (VOS_SIZE_T)(sizeof(TAF_MMA_REG_STATE_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_REG_STATE_QRY_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = ucOpId;

    pstMsg->enQryRegStaType = enRegStaType;

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_QryAutoAttachInfoReq
 ��������  : Mma Qry Auto Attach Info
 �������  : VOS_UINT32                          ulModuleId
             VOS_UINT16                          usClientId
             VOS_UINT8                           ucOpId
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��3��28��
    ��    ��   : g00261581
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_QryAutoAttachInfoReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_AUTO_ATTACH_QRY_REQ_STRU    *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ��TAF_MMA_AUTOATTACH_QRY_REQ_STRU */
    pstMsg = (TAF_MMA_AUTO_ATTACH_QRY_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                               ulSenderPid,
                                               sizeof(TAF_MMA_AUTO_ATTACH_QRY_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
                (VOS_SIZE_T)(sizeof(TAF_MMA_AUTO_ATTACH_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
                0x00,
                (VOS_SIZE_T)(sizeof(TAF_MMA_AUTO_ATTACH_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_AUTO_ATTACH_QRY_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = ucOpId;

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_QrySystemInfoReq
 ��������  : Mma Qry System Info
 �������  : VOS_UINT32                          ulModuleId
             VOS_UINT16                          usClientId
             VOS_UINT8                           ucOpId
             VOS_UINT32                          ulSysInfoExFlag
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��3��28��
    ��    ��   : g00261581
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_QrySystemInfoReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    VOS_UINT32                          ulSysInfoExFlag
)
{
    TAF_MMA_SYSINFO_QRY_REQ_STRU       *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ��TAF_MMA_SYSINFO_QRY_REQ_STRU */
    pstMsg = (TAF_MMA_SYSINFO_QRY_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                            ulSenderPid,
                                            sizeof(TAF_MMA_SYSINFO_QRY_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
                (VOS_SIZE_T)(sizeof(TAF_MMA_SYSINFO_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
                0x00,
                (VOS_SIZE_T)(sizeof(TAF_MMA_SYSINFO_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_SYSINFO_QRY_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = ucOpId;
    pstMsg->ulSysInfoExFlag             = ulSysInfoExFlag;

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}
/* QryAntennaInfoReq ����MTA���� */

/*****************************************************************************
 �� �� ��  : TAF_MMA_QryApHplmnInfoReq
 ��������  : Mma Qry ApHplmn Info
 �������  : VOS_UINT32                          ulModuleId
             VOS_UINT16                          usClientId
             VOS_UINT8                           ucOpId
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��3��30��
    ��    ��   : g00261581
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_QryApHplmnInfoReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_HOME_PLMN_QRY_REQ_STRU     *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ��TAF_MMA_APHPLMN_QRY_REQ_STRU */
    pstMsg = (TAF_MMA_HOME_PLMN_QRY_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                              ulSenderPid,
                                              sizeof(TAF_MMA_HOME_PLMN_QRY_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
                (VOS_SIZE_T)(sizeof(TAF_MMA_HOME_PLMN_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
                0x00,
                (VOS_SIZE_T)(sizeof(TAF_MMA_HOME_PLMN_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderCpuId               = VOS_LOCAL_CPUID;
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverCpuId             = VOS_LOCAL_CPUID;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_HOME_PLMN_QRY_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = ucOpId;

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}

/* QryCsnrReq����MTA���� */

/*****************************************************************************
 �� �� ��  : TAF_MMA_QryCsqReq
 ��������  : CSQ QRY��ѯ��Ϣ�·��ӿ�
 �������  : VOS_UINT32                          ulModuleId,
             VOS_UINT16                          usClientId,
             VOS_UINT8                           ucOpId
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��3��28��
    ��    ��   : y00322978
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_QryCsqReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_CSQ_QRY_REQ_STRU           *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ��TAF_MMA_CSQ_QRY_REQ_STRU */
    pstMsg = (TAF_MMA_CSQ_QRY_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                       ulSenderPid,
                                       sizeof(TAF_MMA_CSQ_QRY_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
            (VOS_SIZE_T)(sizeof(TAF_MMA_CSQ_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
            0x00,
            (VOS_SIZE_T)(sizeof(TAF_MMA_CSQ_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_CSQ_QRY_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = ucOpId;

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;

}

/* QryCsqlvlReq����MTA���� */



VOS_UINT32 TAF_MMA_QryBatteryCapacityReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_BATTERY_CAPACITY_QRY_REQ_STRU                  *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ��TAF_MMA_CBC_QRY_REQ_STRU */
    pstMsg = (TAF_MMA_BATTERY_CAPACITY_QRY_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                        ulSenderPid,
                                        sizeof(TAF_MMA_BATTERY_CAPACITY_QRY_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
                (VOS_SIZE_T)(sizeof(TAF_MMA_BATTERY_CAPACITY_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
                0x00,
                (VOS_SIZE_T)(sizeof(TAF_MMA_BATTERY_CAPACITY_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_BATTERY_CAPACITY_QRY_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = ucOpId;

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}


VOS_UINT32 TAF_MMA_QryHandShakeReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_HAND_SHAKE_QRY_REQ_STRU    *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ��TAF_MMA_HS_QRY_REQ_STRU */
    pstMsg = (TAF_MMA_HAND_SHAKE_QRY_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                       ulSenderPid,
                                       sizeof(TAF_MMA_HAND_SHAKE_QRY_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S( (VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
                (VOS_SIZE_T)(sizeof(TAF_MMA_HAND_SHAKE_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
                0x00,
                (VOS_SIZE_T)(sizeof(TAF_MMA_HAND_SHAKE_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH) );

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid                 = ulSenderPid;
    pstMsg->ulReceiverPid               = ulReceiverPid;
    pstMsg->ulMsgName                   = ID_TAF_MMA_HAND_SHAKE_QRY_REQ;
    pstMsg->stCtrl.ulModuleId           = ulModuleId;
    pstMsg->stCtrl.usClientId           = usClientId;
    pstMsg->stCtrl.ucOpId               = ucOpId;

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}



/*****************************************************************************
 �� �� ��  : TAF_MMA_ImsRegDomainNotify
 ��������  : SPMģ���MMAģ�鷢��IMSע����֪ͨ
 �������  : TAF_SDC_IMS_REG_DOMAIN_ENUM_UINT8           enImsRegDomain
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��5��4��
    ��    ��   : n00269697
    �޸�����   : �����ɺ���

  2.��    ��: 2017��7��6��
    ��    ��: l00359089
    �޸�����: �������TAF_SDC_IMS_REG_STATUS_ENUM_UINT8 enImsRegStatus
*****************************************************************************/
VOS_VOID TAF_MMA_ImsRegDomainNotify(
    TAF_MMA_IMS_REG_DOMAIN_ENUM_UINT8           enImsRegDomain,
    TAF_MMA_IMS_REG_STATUS_ENUM_UINT8           enImsRegStatus
)
{
    TAF_MMA_IMS_REG_DOMAIN_NOTIFY_STRU           *pstMsg  = VOS_NULL_PTR;

    /* ������Ϣ��TAF_MMA_SRV_ACQ_REQ_STRU */
    pstMsg = (TAF_MMA_IMS_REG_DOMAIN_NOTIFY_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                             WUEPS_PID_TAF,
                                             sizeof(TAF_MMA_IMS_REG_DOMAIN_NOTIFY_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
               sizeof(TAF_MMA_IMS_REG_DOMAIN_NOTIFY_STRU) - VOS_MSG_HEAD_LENGTH,
               0x00,
               (VOS_SIZE_T)(sizeof(TAF_MMA_IMS_REG_DOMAIN_NOTIFY_STRU) - VOS_MSG_HEAD_LENGTH));

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid       = WUEPS_PID_TAF;
    pstMsg->ulReceiverPid     = WUEPS_PID_MMA;
    pstMsg->enMsgName         = ID_TAF_MMA_IMS_REG_DOMAIN_NOTIFY;
    pstMsg->enImsRegDomain    = enImsRegDomain;

    pstMsg->enImsRegStatus    = enImsRegStatus;

    /* ������Ϣ */
    if (VOS_OK != PS_SEND_MSG(WUEPS_PID_TAF, pstMsg))
    {
        return;
    }

    return;
}
/*****************************************************************************
 �� �� ��  : TAF_MMA_QryPacspReq
 ��������  : ��ѯ����ѡ��˵������ԣ�����ѯENS�Ƿ�֧��
 �������  : VOS_UINT32                          ulModuleId
             VOS_UINT16                          usClientId
             VOS_UINT8                           ucOpId
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��10��9��
    ��    ��   : n00269697
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 TAF_MMA_QryPacspReq(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
)
{
    TAF_MMA_PACSP_QRY_REQ_STRU         *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ�� */
    pstMsg = (TAF_MMA_PACSP_QRY_REQ_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(ulSenderPid, sizeof(TAF_MMA_PACSP_QRY_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
               (VOS_SIZE_T)(sizeof(TAF_MMA_PACSP_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH),
               0x00,
               (VOS_SIZE_T)(sizeof(TAF_MMA_PACSP_QRY_REQ_STRU) - VOS_MSG_HEAD_LENGTH));

    pstMsg->ulSenderPid       = ulSenderPid;
    pstMsg->ulReceiverPid     = ulReceiverPid;
    pstMsg->enMsgName         = ID_TAF_MMA_PACSP_QRY_REQ;
    pstMsg->stCtrl.ucOpId     = ucOpId;
    pstMsg->stCtrl.ulModuleId = ulModuleId;
    pstMsg->stCtrl.usClientId = usClientId;

    /* ��Ϣ����*/
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}

/*****************************************************************************
 �� �� ��  : TAF_MMA_SendSmcNoEntityNtf
 ��������  : MSGģ���MMA֪ͨSMC��ǰû��ʵ��
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��10��15��
    ��    ��   : n00269697
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_VOID TAF_MMA_SendSmcNoEntityNtf(VOS_VOID)
{
    TAF_MMA_SMC_NO_ENTITY_NOTIFY_STRU           *pstMsg  = VOS_NULL_PTR;

    pstMsg = (TAF_MMA_SMC_NO_ENTITY_NOTIFY_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(
                                             WUEPS_PID_TAF,
                                             sizeof(TAF_MMA_SMC_NO_ENTITY_NOTIFY_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
               sizeof(TAF_MMA_SMC_NO_ENTITY_NOTIFY_STRU) - VOS_MSG_HEAD_LENGTH,
               0x00,
               (VOS_SIZE_T)(sizeof(TAF_MMA_SMC_NO_ENTITY_NOTIFY_STRU) - VOS_MSG_HEAD_LENGTH));

    /* ����PIDͳһ��дΪWUEPS_PID_TAF */
    pstMsg->ulSenderPid       = WUEPS_PID_TAF;
    pstMsg->ulReceiverPid     = WUEPS_PID_MMA;
    pstMsg->enMsgName         = ID_TAF_MMA_SMC_NO_ENTITY_NOTIFY;

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(WUEPS_PID_TAF, pstMsg);

    return;
}

/* Added by wx270776 for ����LNAS R13Э��������������, 2017-2-13, begin */
/*****************************************************************************
 �� �� ��  : TAF_MMA_AcdcAppNotify
 ��������  : AT��MMA����ID_TAF_MMA_ACDC_APP_NOTIFY��Ϣ
 �������  : VOS_UINT32                          ulModuleId,
             VOS_UINT16                          usClientId,
             VOS_UINT8                           ucOpId,
             TAF_MMA_ACDC_APP_INFO_STRU         *pstAcdcAppInfo
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2017��02��13��
    ��    ��   : wx270776
    �޸�����   : �����ɺ���

*****************************************************************************/
/*lint -save -e838 -specific(-e838)*/
VOS_UINT32 TAF_MMA_AcdcAppNotify(
    VOS_UINT32                          ulModuleId,
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    TAF_MMA_ACDC_APP_INFO_STRU         *pstAcdcAppInfo
)
{
    TAF_MMA_ACDC_APP_NOTIFY_STRU       *pstMsg = VOS_NULL_PTR;
    VOS_UINT32                          ulReceiverPid;
    VOS_UINT32                          ulSenderPid;

    ulReceiverPid = AT_GetDestPid(usClientId, WUEPS_PID_MMA);
    ulSenderPid   = AT_GetDestPid(usClientId, WUEPS_PID_TAF);

    /* ������Ϣ�� */
    pstMsg = (TAF_MMA_ACDC_APP_NOTIFY_STRU *)PS_ALLOC_MSG_WITH_HEADER_LEN(ulSenderPid, sizeof(TAF_MMA_ACDC_APP_NOTIFY_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return VOS_FALSE;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
               (VOS_SIZE_T)(sizeof(TAF_MMA_ACDC_APP_NOTIFY_STRU) - VOS_MSG_HEAD_LENGTH),
               0x00,
               (VOS_SIZE_T)(sizeof(TAF_MMA_ACDC_APP_NOTIFY_STRU) - VOS_MSG_HEAD_LENGTH));

    pstMsg->ulSenderPid       = ulSenderPid;
    pstMsg->ulReceiverPid     = ulReceiverPid;
    pstMsg->enMsgName         = ID_TAF_MMA_ACDC_APP_NOTIFY;
    pstMsg->stCtrl.ucOpId     = ucOpId;
    pstMsg->stCtrl.ulModuleId = ulModuleId;
    pstMsg->stCtrl.usClientId = usClientId;

    TAF_MEM_CPY_S(&(pstMsg->stAcdcAppInfo),
                  sizeof(TAF_MMA_ACDC_APP_INFO_STRU),
                  pstAcdcAppInfo,
                  sizeof(TAF_MMA_ACDC_APP_INFO_STRU));

    /* ��Ϣ����*/
    (VOS_VOID)PS_SEND_MSG(ulSenderPid, pstMsg);

    return VOS_TRUE;
}
/*lint -restore*/
/* Added by wx270776 for ����LNAS R13Э��������������, 2017-2-13, end */

/*****************************************************************************
 �� �� ��  : TAF_MMA_SndRestartReq
 ��������  : ����modem������Ϣ
 �������  : ��
 �������  : ��
 �� �� ֵ  : VOS_UOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2017��2��10��
    ��    ��   : c00299063
    �޸�����   : �����ɺ���
*****************************************************************************/
VOS_VOID TAF_MMA_SndRestartReq(
    VOS_UINT32                          ulModuleId,
    MODEM_ID_ENUM_UINT16                enModemId
)
{
    TAF_MMA_RESTART_REQ_STRU           *pstMsg;
    VOS_UINT32                          ulSenderPid;
    VOS_UINT32                          ulReceiverPid;

    if (MODEM_ID_0 == enModemId)
    {
        ulSenderPid      = I0_WUEPS_PID_TAF;
        ulReceiverPid    = I0_WUEPS_PID_MMA;
    }
    else if (MODEM_ID_1 == enModemId)
    {
        ulSenderPid      = I1_WUEPS_PID_TAF;
        ulReceiverPid    = I1_WUEPS_PID_MMA;
    }
    else if (MODEM_ID_2 == enModemId)
    {
        ulSenderPid      = I2_WUEPS_PID_TAF;
        ulReceiverPid    = I2_WUEPS_PID_MMA;
    }
    else
    {
        return;
    }

    pstMsg = (TAF_MMA_RESTART_REQ_STRU*)PS_ALLOC_MSG_WITH_HEADER_LEN(ulSenderPid, sizeof(TAF_MMA_RESTART_REQ_STRU));

    /* �ڴ�����ʧ�ܣ����� */
    if (VOS_NULL_PTR == pstMsg)
    {
        return;
    }

    TAF_MEM_SET_S((VOS_INT8 *)pstMsg + VOS_MSG_HEAD_LENGTH,
                  sizeof(TAF_MMA_RESTART_REQ_STRU) - VOS_MSG_HEAD_LENGTH,
                  0x00,
                  (VOS_SIZE_T)(sizeof(TAF_MMA_RESTART_REQ_STRU) - VOS_MSG_HEAD_LENGTH));

    pstMsg->ulSenderPid       = ulSenderPid;
    pstMsg->ulReceiverPid     = ulReceiverPid;
    pstMsg->enMsgName         = ID_TAF_MMA_RESTART_REQ;
    pstMsg->ulModuleId        = ulModuleId;

    /* ������Ϣ */
    (VOS_VOID)PS_SEND_MSG(WUEPS_PID_TAF, pstMsg);

    return;
}


