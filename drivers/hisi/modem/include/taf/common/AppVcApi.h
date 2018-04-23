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
#ifndef  APP_VC_API_H
#define  APP_VC_API_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "TafTypeDef.h"
#include "vos.h"

/*****************************************************************************
  2 ��������
*****************************************************************************/
/* Added by j00174725 for V3R3C60_eCall��Ŀ, 2014-3-29, begin */
#define APP_VC_MSD_DATA_LEN             (140)                                   /* ��λ:btye */
/* Added by j00174725 for V3R3C60_eCall��Ŀ, 2014-3-29, End */



/*****************************************************************************
  3���Ͷ���
*****************************************************************************/
/* �����豸��0 �ֳ֣�1 �ֳ����᣻2 �������᣻3 ������4 ������7 �������᣻8 ����ͨ�� */
enum VC_PHY_DEVICE_MODE_ENUM
{
    VC_PHY_DEVICE_MODE_HANDSET,                                                 /* �ֳ� */
    VC_PHY_DEVICE_MODE_HANDS_FREE,                                              /* �ֳ����� */
    VC_PHY_DEVICE_MODE_CAR_FREE,                                                /* �������� */
    VC_PHY_DEVICE_MODE_EARPHONE,                                                /* ���� */
    VC_PHY_DEVICE_MODE_BLUETOOTH,                                               /* ���� */
    VC_PHY_DEVICE_MODE_PCVOICE,                                                 /* PC����ģʽ */
    VC_PHY_DEVICE_MODE_HEADPHONE,                                               /* ��������MIC�Ķ���ͨ�� */
    VC_PHY_DEVICE_MODE_SUPER_HANDFREE,                                              /* �������� */
    VC_PHY_DEVICE_MODE_SMART_TALK,                                              /* ����ͨ�� */
    VC_PHY_DEVICE_MODEM_USBVOICE,                                               /* USB�豸 */
    VC_PHY_DEVICE_MODE_BUTT                                                     /* invalid value */
};
typedef VOS_UINT16  VC_PHY_DEVICE_MODE_ENUM_U16;

/*****************************************************************************
 ö����    : APP_VC_VOICE_MODE_ENUM
 �ṹ˵��  : STICK��̬�£��û�ͨ��^CVOICE˽���������õ�����ģʽö��
*****************************************************************************/
enum APP_VC_VOICE_MODE_ENUM
{
    APP_VC_VOICE_MODE_PCVOICE           = 0,
    APP_VC_VOICE_MODE_EARPHONE          = 1,
    APP_VC_VOICE_MODE_HANDSET           = 2,
    APP_VC_VOICE_MODE_HANDS_FREE        = 3,
    APP_VC_VOICE_MODE_BUTT
};
typedef VOS_UINT16 APP_VC_VOICE_MODE_ENUM_U16;

/*****************************************************************************
 ö����    : APP_VC_VOICE_PORT_ENUM
 �ṹ˵��  : ��������˿�ö��
*****************************************************************************/
enum APP_VC_VOICE_PORT_ENUM
{
    APP_VC_VOICE_PORT_MODEM             = 1,
    APP_VC_VOICE_PORT_DIAG              = 2,
    APP_VC_VOICE_PORT_PCUI              = 3,
    APP_VC_VOICE_PORT_PCVOICE           = 4,
    APP_VC_VOICE_PORT_PCSC              = 5,
    APP_VC_VOICE_PORT_BUTT
};
typedef VOS_UINT8 APP_VC_VOICE_PORT_ENUM_U8;

enum APP_VC_EVENT_ENUM
{
    APP_VC_EVT_PARM_CHANGED,                                                /*service para changed*/
    APP_VC_EVT_SET_VOLUME,
    APP_VC_EVT_SET_VOICE_MODE,
    APP_VC_EVT_GET_VOLUME,
    APP_VC_EVT_SET_MUTE_STATUS,
    APP_VC_EVT_GET_MUTE_STATUS,

    /* Added by j00174725 for V3R3C60_eCall��Ŀ, 2014-3-29, begin */
    APP_VC_EVT_SET_ECALL_CFG,
    APP_VC_EVT_ECALL_TRANS_STATUS,
    /* Added by j00174725 for V3R3C60_eCall��Ŀ, 2014-3-29, end */
    APP_VC_EVT_BUTT
};
typedef VOS_UINT32 APP_VC_EVENT_ENUM_U32;

enum APP_VC_MUTE_STATUS_ENUM
{
    APP_VC_MUTE_STATUS_UNMUTED          = 0,
    APP_VC_MUTE_STATUS_MUTED            = 1,
    APP_VC_MUTE_STATUS_BUTT
};
typedef VOS_UINT8 APP_VC_MUTE_STATUS_ENUM_UINT8;

/*****************************************************************************
 ö����    : TAF_VC_TTYMODE_ENUM
 �ṹ˵��  : TTYģʽ״̬ö��
  1.��    ��   : 2015��02��07��
    ��    ��   : w00316404
    �޸�����   : �½�
*****************************************************************************/
enum TAF_VC_TTYMODE_ENUM
{
    TAF_VC_TTY_OFF                      = 0,
    TAF_VC_TTY_FULL                     = 1,
    TAF_VC_TTY_VCO                      = 2,
    TAF_VC_TTY_HCO                      = 3,
    TAF_VC_TTYMODE_BUTT
};
typedef VOS_UINT8 TAF_VC_TTYMODE_ENUM_UINT8;

/*****************************************************************************
 ö����    : APP_VC_MSG_REQ_ENUM
 ö��˵��  : AT��VCģ��䷢����Ϣ����Ϣ��

1.��    ��   : 2011��10��15��
  ��    ��   : f00179209
  �޸�����   : AT��ֲ��Ŀ
*****************************************************************************/
enum APP_VC_MSG_ENUM
{
    APP_VC_MSG_REQ_SET_VOLUME   = 0,                                            /* _H2ASN_MsgChoice APP_VC_REQ_MSG_ASN_STRU *//* ��������������Ϣ */
    APP_VC_MSG_REQ_SET_MODE,                                                    /* _H2ASN_MsgChoice APP_VC_REQ_MSG_ASN_STRU *//* ����ͨ��ģʽ������Ϣ */

    APP_VC_MSG_REQ_QRY_MODE,                                                    /* _H2ASN_MsgChoice APP_VC_REQ_MSG_ASN_STRU *//* ��ѯͨ��ģʽ������Ϣ */
    APP_VC_MSG_CNF_QRY_MODE,                                                    /* _H2ASN_MsgChoice APP_VC_EVENT_INFO_STRU *//* ��ѯͨ��ģʽ�ظ���Ϣ */

    APP_VC_MSG_REQ_SET_PORT,                                                    /* _H2ASN_MsgChoice APP_VC_REQ_MSG_ASN_STRU *//* ���������˿�������Ϣ */
    APP_VC_MSG_CNF_SET_PORT,                                                    /* _H2ASN_MsgChoice APP_VC_EVENT_INFO_STRU *//* ���������˿ڻظ���Ϣ */

    APP_VC_MSG_REQ_QRY_PORT,                                                    /* _H2ASN_MsgChoice APP_VC_REQ_MSG_ASN_STRU *//* ��ѯ�����˿�������Ϣ */
    APP_VC_MSG_CNF_QRY_PORT,                                                    /* _H2ASN_MsgChoice APP_VC_EVENT_INFO_STRU *//* ��ѯ�����˿ڻظ���Ϣ */

    APP_VC_MSG_REQ_QRY_VOLUME,                                                  /* _H2ASN_MsgChoice APP_VC_REQ_MSG_ASN_STRU */
    APP_VC_MSG_CNF_QRY_VOLUME,

    APP_VC_MSG_SET_MUTE_STATUS_REQ,                                             /* _H2ASN_MsgChoice APP_VC_REQ_MSG_ASN_STRU *//* ���þ���״̬������Ϣ */
    APP_VC_MSG_GET_MUTE_STATUS_REQ,                                             /* _H2ASN_MsgChoice APP_VC_REQ_MSG_ASN_STRU *//* ��ȡ����״̬������Ϣ */

    APP_VC_MSG_SET_FOREGROUND_REQ,                                              /* _H2ASN_MsgChoice APP_VC_REQ_MSG_ASN_STRU *//* ����ǰ̨ģʽ������Ϣ */
    APP_VC_MSG_SET_FOREGROUND_CNF,                                              /* _H2ASN_MsgChoice APP_VC_MSG_CNF_ASN_STRU *//* ����ǰ̨ģʽ�ظ���Ϣ */

    APP_VC_MSG_SET_BACKGROUND_REQ,                                              /* _H2ASN_MsgChoice APP_VC_REQ_MSG_ASN_STRU *//* ���ú�̨ģʽ������Ϣ */
    APP_VC_MSG_SET_BACKGROUND_CNF,                                              /* _H2ASN_MsgChoice APP_VC_MSG_CNF_ASN_STRU *//* ���ú�̨ģʽ�ظ���Ϣ */

    APP_VC_MSG_FOREGROUND_QRY,                                                  /* _H2ASN_MsgChoice APP_VC_REQ_MSG_ASN_STRU *//* ģʽ��ѯ������Ϣ */
    APP_VC_MSG_FOREGROUND_RSP,                                                  /* _H2ASN_MsgChoice APP_VC_QRY_GROUNG_RSP_STRU *//* ģʽ��ѯ�ظ���Ϣ */

    APP_VC_MSG_SET_MODEMLOOP_REQ,                                               /* _H2ASN_MsgChoice APP_VC_REQ_MSG_ASN_STRU *//* ������������ģʽ��Ϣ */

    /* Added by j00174725 for V3R3C60_eCall��Ŀ, 2014-3-29, begin */
    APP_VC_MSG_SET_MSD_REQ,                                                     /* _H2ASN_MsgChoice APP_VC_MSG_SET_MSD_REQ_STRU *//* ����MSD�������� */
    APP_VC_MSG_SET_MSD_CNF,                                                     /* _H2ASN_MsgChoice APP_VC_MSG_CNF_ASN_STRU *//* ����MSD���ݻظ� */

    APP_VC_MSG_QRY_MSD_REQ,                                                     /* _H2ASN_MsgChoice APP_VC_REQ_MSG_ASN_STRU *//* ��ѯMSD�������� */
    APP_VC_MSG_QRY_MSD_CNF,                                                     /* _H2ASN_MsgChoice APP_VC_MSG_QRY_MSD_CNF_STRU *//* ��ѯMSD���ݻظ� */

    APP_VC_MSG_SET_ECALL_CFG_REQ,                                               /* _H2ASN_MsgChoice APP_VC_MSG_SET_ECALL_CFG_REQ_STRU *//* ����ecall�������� */

    APP_VC_MSG_QRY_ECALL_CFG_REQ,                                               /* _H2ASN_MsgChoice APP_VC_REQ_MSG_ASN_STRU *//* ��ѯecall������Ϣ���� */
    APP_VC_MSG_QRY_ECALL_CFG_CNF,                                               /* _H2ASN_MsgChoice APP_VC_MSG_QRY_ECALL_CFG_CNF_STRU *//* ��ѯecall������Ϣ�ظ� */
    /* Added by j00174725 for V3R3C60_eCall��Ŀ, 2014-3-29, end */

    APP_VC_MSG_DTMF_DECODER_IND,                                                /* _H2ASN_MsgChoice APP_VC_DTMF_DECODER_IND_STRU *//* DTMF Decoder Ind�ϱ���Ϣ */

    APP_VC_MSG_SET_TTYMODE_REQ,                                                 /* _H2ASN_MsgChoice APP_VC_SET_TTYMODE_REQ_STRU *//* ����TTYģʽ������Ϣ */
    APP_VC_MSG_SET_TTYMODE_CNF,                                                 /* _H2ASN_MsgChoice APP_VC_MSG_CNF_ASN_STRU *//* ����TTYģʽ�ظ���Ϣ */
    APP_VC_MSG_QRY_TTYMODE_REQ,                                                 /* _H2ASN_MsgChoice APP_VC_REQ_MSG_ASN_STRU *//* ��ѯTTYģʽ������Ϣ */
    APP_VC_MSG_QRY_TTYMODE_CNF,                                                 /* _H2ASN_MsgChoice APP_VC_QRY_TTYMODE_CNF_STRU *//* ��ѯTTYģʽ�ظ���Ϣ */

    APP_VC_MSG_BUTT
};
typedef VOS_UINT16  APP_VC_MSG_ENUM_U16;

/*****************************************************************************
 ö����    : APP_VC_QRY_GROUND_ENUM
 �ṹ˵��  : VC����״̬ö��
  1.��    ��   : 2013��01��18��
    ��    ��   : z00214637
    �޸�����   : �½�
*****************************************************************************/
enum APP_VC_QRY_GROUND_ENUM
{
    APP_VC_FOREGROUND                   = 0,
    APP_VC_BACKGROUND                   = 1,
    APP_VC_GROUND_INVALID               = 2,
    APP_VC_GROUND_BUTT
};
typedef VOS_UINT16 APP_VC_QRY_GROUND_ENUM_U16;

/* Added by s00217060 for VoLTE_PhaseI  ��Ŀ, 2013-07-11, begin */
/*****************************************************************************
 ö����    : APP_VC_START_HIFI_ORIG_ENUM_UINT8
 �ṹ˵��  : ������IMSA����CALLҪ����HIFI
  1.��    ��   : 2013��07��11��
    ��    ��   : s00217060
    �޸�����   : �½�
*****************************************************************************/
enum APP_VC_START_HIFI_ORIG_ENUM
{
    APP_VC_START_HIFI_ORIG_CALL                   = 0,
    APP_VC_START_HIFI_ORIG_IMSA                   = 1,
    APP_VC_START_HIFI_ORIG_BUTT
};
typedef VOS_UINT8 APP_VC_START_HIFI_ORIG_ENUM_UINT8;
/* Added by s00217060 for VoLTE_PhaseI  ��Ŀ, 2013-07-11, end */

/* Added by j00174725 for V3R3C60_eCall��Ŀ, 2014-3-29, begin */
/*****************************************************************************
 ö����    : APP_VC_ECALL_TRANS_STATUS_ENUM
 ö��˵��  : ECALL��ǰ�Ĵ���״̬
1.��    ��   : 2014��3��27��
  ��    ��   : h00246512
  �޸�����   : V3R3C60_eCall��Ŀ
*****************************************************************************/
enum APP_VC_ECALL_TRANS_STATUS_ENUM
{
    APP_VC_ECALL_MSD_TRANSMITTING_START     = 0,                                /* ��ǰMSD�����Ѿ���ʼ���� */
    APP_VC_ECALL_MSD_TRANSMITTING_SUCCESS,                                      /* ��ǰMSD�����Ѿ����ͳɹ� */
    APP_VC_ECALL_MSD_TRANSMITTING_FAIL,                                         /* ��ǰMSD�����Ѿ�����ʧ�� */
    APP_VC_ECALL_PSAP_MSD_REQUIRETRANSMITTING,                                  /* PSAPҪ�����MSD���� */
    APP_VC_ECALL_TRANS_STATUS_BUTT
};
typedef VOS_UINT8  APP_VC_ECALL_TRANS_STATUS_ENUM_UINT8;

/*****************************************************************************
 ö����    : APP_VC_ECALL_TRANS_FAIL_CAUSE_ENUM
 ö��˵��  : ECALL�Ĵ���ʧ��ԭ��
1.��    ��   : 2014��3��27��
  ��    ��   : h00246512
  �޸�����   : V3R3C60_eCall��Ŀ
*****************************************************************************/
enum APP_VC_ECALL_TRANS_FAIL_CAUSE_ENUM
{
    APP_VC_ECALL_CAUSE_WAIT_PSAP_TIME_OUT   = 0,                                /* �ȴ�PSAP�Ĵ���ָʾ��ʱ */
    APP_VC_ECALL_CAUSE_MSD_TRANSMITTING_TIME_OUT,                               /* MSD���ݴ��䳬ʱ */
    APP_VC_ECALL_CAUSE_WAIT_AL_ACK_TIME_OUT,                                    /* �ȴ�Ӧ�ò��ȷ�ϳ�ʱ */
    APP_VC_ECALL_CAUSE_UNSPECIFIC_ERROR,                                        /* �����쳣���� */
    APP_VC_ECALL_CAUSE_BUTT
};
typedef VOS_UINT8  APP_VC_ECALL_TRANS_FAIL_CAUSE_ENUM_UINT8;

/*****************************************************************************
 ö����    : APP_VC_ECALL_MSD_MODE_ENUM
 ö��˵��  : �������õ����ȡMSD���ݵ�ģʽ
1.��    ��   : 2014��3��27��
  ��    ��   : j00174725
  �޸�����   : V3R3C60_eCall��Ŀ
*****************************************************************************/
enum APP_VC_ECALL_MSD_MODE_ENUM
{
    APP_VC_ECALL_MSD_MODE_TRANSPARENT   = 0,                                    /* ͸��ģʽ */
    APP_VC_ECALL_MSD_MODE_BUTT
};
typedef VOS_UINT16  APP_VC_ECALL_MSD_MODE_ENUM_UINT16;

/*****************************************************************************
 ö����    : APP_VC_ECALL_VOC_CONFIG_ENUM
 ö��˵��  : ��MSD����������Ƿ��ֹ�������ݵĴ���
1.��    ��   : 2014��3��27��
  ��    ��   : j00174725
  �޸�����   : V3R3C60_eCall��Ŀ
*****************************************************************************/
enum APP_VC_ECALL_VOC_CONFIG_ENUM
{
    APP_VC_ECALL_VOC_CONFIG_NOT_ABANDON = 0,                                    /* MSD���ݴ�����̴�����*/
    APP_VC_ECALL_VOC_CONFIG_ABANDON,                                            /* MSD���ݴ�����̲�������*/
    APP_VC_ECALL_VOC_CONFIG_BUTT
};
typedef VOS_UINT16  APP_VC_ECALL_VOC_CONFIG_ENUM_UINT16;

/* Added by j00174725 for V3R3C60_eCall��Ŀ, 2014-3-29, end */

/*****************************************************************************
 �ṹ��    : APP_VC_CNF_MSG_STRU
 �ṹ˵��  : AT��VC����Ϣ�ṹ, AT��VC����REQ��Ϣ

  1.��    ��   : 2011��10��15��
    ��    ��   : f00179209
    �޸�����   : ����
*****************************************************************************/
typedef struct
{
    VOS_MSG_HEADER                                                              /* _H2ASN_Skip */
    APP_VC_MSG_ENUM_U16                 enMsgName;                              /* _H2ASN_Skip *//* ��Ϣ�� */
    VOS_UINT8                           aucReserve1[2];                         /* ���� */
    MN_CLIENT_ID_T                      clientId;                               /* Client ID */
    MN_OPERATION_ID_T                   opId;                                   /* Operation ID */
    VOS_UINT8                           aucReserve2;                            /* ���� */
    VOS_UINT8                           aucContent[4];                          /* ��Ϣ���� */
} APP_VC_REQ_MSG_STRU;


/*****************************************************************************
 �ṹ��    : APP_VC_EVENT_INFO_STRU
 �ṹ˵��  : AT��VCģ������Ϣ�ṹ

  1.��    ��   : 2011��10��15��
    ��    ��   : f00179209
    �޸�����   : AT��ֲ��Ŀ

  2.��    ��   : 2012��9��13��
    ��    ��   : A00165503
    �޸�����   : DTS2012091405101: ^CMUT����ʵ��
*****************************************************************************/
typedef struct
{
    VOS_BOOL                            bSuccess;                               /*set(get) success or failure */
    VOS_UINT32                          ulErrCode;
    MN_CLIENT_ID_T                      clientId;
    MN_OPERATION_ID_T                   opId;
    APP_VC_VOICE_PORT_ENUM_U8           enVoicePort;
    VC_PHY_DEVICE_MODE_ENUM_U16         enDevMode;
    VOS_UINT16                          usVolume;
    APP_VC_MUTE_STATUS_ENUM_UINT8       enMuteStatus;
    VOS_UINT8                           aucRsv[2];

    /* Added by j00174725 for V3R3C60_eCall��Ŀ, 2014-3-29, begin */
    APP_VC_ECALL_TRANS_STATUS_ENUM_UINT8 enEcallState;
    VOS_UINT32                          ulEcallDescription;
    /* Added by j00174725 for V3R3C60_eCall��Ŀ, 2014-3-29, End */
}APP_VC_EVENT_INFO_STRU;

/*****************************************************************************
 �ṹ��    : APP_VC_QRY_GROUNG_RSP_STRU
 �ṹ˵��  : en_APP_VC_QRY_GROUNG_RSP_STRU�Ľṹ

  1.��    ��   : 2013��01��06��
    ��    ��   : z00214637
    �޸�����   : ����en_APP_VC_QRY_GROUNG_RSP_STRU�Ľṹ
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucQryRslt;                              /* ��ѯ success or failure */
    VOS_UINT8                           aucReserved;
    APP_VC_QRY_GROUND_ENUM_U16          enGround;                               /* ��ѯ��� */
}APP_VC_QRY_GROUNG_RSP_STRU;

/* Added by j00174725 for V3R3C60_eCall��Ŀ, 2014-3-29, begin */
/*****************************************************************************
 �ṹ��    : APP_VC_MSG_SET_MSD_REQ_STRU
 �ṹ˵��  : APP_VC_MSG_SET_MSD_REQ�Ľṹ

  1.��    ��   : 2014��03��27��
    ��    ��   : j00174725
    �޸�����   : ����APP_VC_MSG_SET_MSD_REQ�Ľṹ
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           aucMsdData[APP_VC_MSD_DATA_LEN];        /* MSD�������� */
}APP_VC_MSG_SET_MSD_REQ_STRU;

/*****************************************************************************
 �ṹ��    : APP_VC_MSG_QRY_MSD_CNF_STRU
 �ṹ˵��  : APP_VC_MSG_QRY_MSD_CNF�Ľṹ

  1.��    ��   : 2014��03��27��
    ��    ��   : j00174725
    �޸�����   : ����APP_VC_MSG_QRY_MSD_CNF�Ľṹ
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucQryRslt;                              /* ��ѯ��� */
    VOS_UINT8                           aucReserved[3];
    VOS_UINT8                           aucMsdData[APP_VC_MSD_DATA_LEN];        /* MSD�������� */
}APP_VC_MSG_QRY_MSD_CNF_STRU;

/*****************************************************************************
 �ṹ��    : APP_VC_MSG_SET_ECALL_CFG_REQ_STRU
 �ṹ˵��  : APP_VC_MSG_SET_ECALL_CFG_REQ�Ľṹ

  1.��    ��   : 2014��03��27��
    ��    ��   : j00174725
    �޸�����   : ����APP_VC_MSG_SET_ECALL_CFG_REQ�Ľṹ
*****************************************************************************/
typedef struct
{
    APP_VC_ECALL_MSD_MODE_ENUM_UINT16   enMode;                                 /* ���ݴ���ģʽ */
    APP_VC_ECALL_VOC_CONFIG_ENUM_UINT16 enVocConfig;                            /* ��������ʱ�Ƿ�������� */
}APP_VC_MSG_SET_ECALL_CFG_REQ_STRU;


/*****************************************************************************
 �ṹ��    : APP_VC_MSG_QRY_ECALL_CFG_CNF_STRU
 �ṹ˵��  : APP_VC_MSG_QRY_ECALL_CFG_CNF�Ľṹ

  1.��    ��   : 2014��03��27��
    ��    ��   : j00174725
    �޸�����   : ����APP_VC_MSG_QRY_ECALL_CFG_CNF�Ľṹ
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucQryRslt;                              /* ��ѯ��� */
    VOS_UINT8                           aucReserved[3];
    APP_VC_ECALL_MSD_MODE_ENUM_UINT16   enMode;                                 /* MSD���ݴ���ģʽ */
    APP_VC_ECALL_VOC_CONFIG_ENUM_UINT16 enVocConfig;                            /* ��������ʱ�Ƿ�������� */
}APP_VC_MSG_QRY_ECALL_CFG_CNF_STRU;

/*****************************************************************************
 �ṹ��    : APP_VC_SET_MSD_CNF_STRU
 �ṹ˵��  : ����MSD��Ϣ�ظ�

  1.��    ��   : 2014��03��27��
    ��    ��   : j00174725
    �޸�����   :
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucRslt;                                  /* ��� */
    VOS_UINT8                           aucReserved[3];
}APP_VC_SET_MSD_CNF_STRU;
/* Added by j00174725 for V3R3C60_eCall��Ŀ, 2014-3-29, end */

/*****************************************************************************
 �ṹ��    : APP_VC_QRY_TTYMODE_CNF_STRU
 �ṹ˵��  : APP_VC_MSG_QRY_TTYMODE_CNF�Ľṹ

  1.��    ��   : 2015��02��07��
    ��    ��   : w00316404
    �޸�����   : ����en_APP_VC_QRY_TTYMODE_STRU�Ľṹ
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucQryRslt;                              /* ��ѯ success or failure */
    TAF_VC_TTYMODE_ENUM_UINT8           enTTYMode;                              /* ��ѯ��� */
    VOS_UINT8                           aucReserved[2];
}APP_VC_QRY_TTYMODE_CNF_STRU;

/*****************************************************************************
 �ṹ��    : APP_VC_SET_TTYMODE_REQ_STRU
 �ṹ˵��  : APP_VC_MSG_SET_TTYMODE_REQ�Ľṹ

  1.��    ��   : 2015��02��07��
    ��    ��   : w00316404
    �޸�����   : ����APP_VC_Set_TTYMODE_STRU�Ľṹ
*****************************************************************************/
typedef struct
{
    TAF_VC_TTYMODE_ENUM_UINT8           enTTYMode;                              /* ����TTY MODE*/
    VOS_UINT8                           aucReserved[3];
}APP_VC_SET_TTYMODE_REQ_STRU;

/* ���½ṹֻΪתASN���� j00174725 2014-05-08 begin */
/*****************************************************************************
 �ṹ��    : APP_VC_MSG_CNF_ASN_STRU
 �ṹ˵��  : VC��AT��Ϣ�ظ�

  1.��    ��   : 2014��03��27��
    ��    ��   : j00174725
    �޸�����   :
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucRslt;
    VOS_UINT8                           aucReserved[3];
}APP_VC_MSG_CNF_ASN_STRU;

/*****************************************************************************
 �ṹ��    : APP_VC_MSG_REQ_STRU
 �ṹ˵��  : AT��VC��Ϣ

  1.��    ��   : 2014��03��27��
    ��    ��   : j00174725
    �޸�����   :
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           aucContent[4];
}APP_VC_REQ_MSG_ASN_STRU;
/* ���½ṹֻΪתASN���� j00174725 2014-05-08 end */


/*****************************************************************************
  4 �궨��
*****************************************************************************/


/*****************************************************************************
  5 ȫ�ֱ�������
*****************************************************************************/


/*****************************************************************************
  6 �ӿں�������
*****************************************************************************/

/*****************************************************************************
�� �� ��  : VCI_SetVoiceVolume
��������  : �����������
�������  : ��
�������  : ��
�� �� ֵ  : VOS_OK
���ú���  : SI_InitGlobeVariable
��������  :
�޶���¼  :
1.  ��    ��   : 2009��07��05��
    ��    ��   : h44270
    �޸�����   : Creat
*****************************************************************************/
VOS_UINT32  APP_VC_SetVoiceVolume(
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId,
    VOS_UINT8                           ucVoiceVolume
);

/*****************************************************************************
�� �� ��  : APP_VC_SetVoiceMode
��������  : �����������
�������  : ��
�������  : ��
�� �� ֵ  : VOS_OK
���ú���  : SI_InitGlobeVariable
��������  :
�޶���¼  :
1.  ��    ��   : 2009��07��05��
    ��    ��   : h44270
    �޸�����   : Creat
*****************************************************************************/
VOS_UINT32  APP_VC_SetVoiceMode(
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId,
    VOS_UINT8                           ucVoiceMode,
    VOS_UINT32                          ulRcvPid
);

/*****************************************************************************
�� �� ��  : APP_VC_SetVoicePort
��������  : ���������˿�
�������  : APP_VC_VOICE_PORT_ENUM_U8   ucVoicePort - ���õ������˿�
�������  : ��
�� �� ֵ  : ��ǰ�������˿�
���ú���  :
��������  :
�޶���¼  :
1.  ��    ��   : 2010��04��16��
    ��    ��   : o00132663
    �޸�����   : Creat
*****************************************************************************/
VOS_UINT32 APP_VC_SetVoicePort(
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId,
    APP_VC_VOICE_PORT_ENUM_U8           ucVoicePort
);

/*****************************************************************************
�� �� ��  : APP_VC_GetVoicePort
��������  : ��ȡ��ǰ�������˿�
�������  : ��
�������  : ��
�� �� ֵ  : APP_VC_VOICE_PORT_ENUM_U8 - ��ǰ�������˿�
���ú���  :
��������  :
�޶���¼  :
1.  ��    ��   : 2010��04��16��
    ��    ��   : o00132663
    �޸�����   : Creat
*****************************************************************************/
VOS_UINT32 APP_VC_GetVoicePort(
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId
);

/*****************************************************************************
�� �� ��  : APP_VC_GetVoiceMode
��������  : ��ȡ��ǰ�������˿�
�������  : ��
�������  : ��
�� �� ֵ  : VC_PHY_DEVICE_MODE_ENUM_U16 - ��ǰ������ģʽ
���ú���  :
��������  :
�޶���¼  :
1.  ��    ��   : 2011��10��05��
    ��    ��   : f00179208
    �޸�����   : Creat
*****************************************************************************/
VOS_UINT32 APP_VC_GetVoiceMode(
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId
);

/*****************************************************************************
�� �� ��  : APP_VC_AppVcVoiceMode2VcPhyVoiceMode
��������  : Ӧ����VCģ������ģʽ����ת��ΪVCģ�������������ģʽ�ӿڶ���
�������  : APP_VC_VOICE_MODE_ENUM_U16  usVoiceMode - Ӧ����VC�ӿ�����ģʽ����
�������  : ��
�� �� ֵ  : VC_PHY_DEVICE_MODE_ENUM_U16 - VC�����������ģʽ����
���ú���  :
��������  :
�޶���¼  :
1.  ��    ��   : 2021��04��17��
    ��    ��   : o00132663
    �޸�����   : Creat
*****************************************************************************/
extern  VC_PHY_DEVICE_MODE_ENUM_U16  APP_VC_AppVcVoiceMode2VcPhyVoiceMode(
    APP_VC_VOICE_MODE_ENUM_U16          usVoiceMode
);

/*****************************************************************************
�� �� ��  : APP_VC_VcPhyVoiceMode2AppVcVoiceMode
��������  : VCģ�������������ģʽ�ӿڶ���ת��ΪӦ����VCģ������ģʽ����
�������  : VC_PHY_DEVICE_MODE_ENUM_U16  usVoiceMode - VC�����������ģʽ����
�������  : ��
�� �� ֵ  : APP_VC_VOICE_MODE_ENUM_U16 - Ӧ����VC�ӿ�����ģʽ����
���ú���  :
��������  :
�޶���¼  :
1.  ��    ��   : 2021��04��17��
    ��    ��   : o00132663
    �޸�����   : Creat
*****************************************************************************/
extern  APP_VC_VOICE_MODE_ENUM_U16  APP_VC_VcPhyVoiceMode2AppVcVoiceMode(
    VC_PHY_DEVICE_MODE_ENUM_U16         usVoiceMode
);

extern VOS_UINT32  APP_VC_GetVoiceVolume(
    MN_CLIENT_ID_T                      ClientId,
    MN_OPERATION_ID_T                   OpId
);

/*****************************************************************************
 �� �� ��  : APP_VC_SetMuteStatus
 ��������  : ���þ���״̬
 �������  : usClientId   - �ͻ���ID
             ucOpId       - ������ID
             enMuteStatus - ����״̬
 �������  : ��
 �� �� ֵ  : VOS_OK       - �ɹ�
             VOS_ERR      - ʧ��
 ���ú���  :
 ��������  :
*****************************************************************************/
VOS_UINT32  APP_VC_SetMuteStatus(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    APP_VC_MUTE_STATUS_ENUM_UINT8       enMuteStatus
);


/*****************************************************************************
 �� �� ��  : APP_VC_GetMuteStatus
 ��������  : ��ȡ����״̬
 �������  : usClientId   - �ͻ���ID
             ucOpId       - ������ID
 �������  : ��
 �� �� ֵ  : VOS_OK       - �ɹ�
             VOS_ERR      - ʧ��
 ���ú���  :
 ��������  :
*****************************************************************************/
VOS_UINT32 APP_VC_GetMuteStatus(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId
);

/* Added by L47619 for VOICE_LOOP, 2013/07/05, begin */
/*****************************************************************************
 �� �� ��  : APP_VC_SetModemLoop
 ��������  : ���ý�����˳���������
 �������  : usClientId   - �ͻ���ID
             ucOpId       - ������ID
             ucModemLoop  - ������˳���������״̬
 �������  : ��
 �� �� ֵ  : VOS_OK       - �ɹ�
             VOS_ERR      - ʧ��
 ���ú���  :
 ��������  :
*****************************************************************************/
VOS_UINT32 APP_VC_SetModemLoop(
    VOS_UINT16                          usClientId,
    VOS_UINT8                           ucOpId,
    VOS_UINT8                           ucModemLoop
);
/* Added by L47619 for VOICE_LOOP, 2013/07/05, end */

/*****************************************************************************
 �ṹ��    : APP_VC_DTMF_DECODER_IND_STRU
 �ṹ˵��  : APP_VC_DTMF_DECODER_IND_STRU�Ľṹ

  1.��    ��   : 2014��05��09��
    ��    ��   : g00261581
    �޸�����   : ����
*****************************************************************************/
typedef struct
{
    VOS_CHAR                            ucDtmfCode;
    VOS_UINT8                           ucReserved;
    VOS_UINT16                          usOntime;
}APP_VC_DTMF_DECODER_IND_STRU;



#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
#pragma pack()
#else
#pragma pack(0)
#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* APP_VC_API_H */

