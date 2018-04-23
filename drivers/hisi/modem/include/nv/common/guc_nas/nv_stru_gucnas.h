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


#ifndef __NV_STRU_GUCNAS_H__
#define __NV_STRU_GUCNAS_H__

#include "vos.h"
#include "nv_id_gucnas.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#pragma pack(push, 4)

/*****************************************************************************
  2 �궨��
*****************************************************************************/
#define TAF_NVIM_SN_LEN                                     (20)
#define TAF_NVIM_MAX_OPER_NAME_SERVICE_PRIO_NUM             (4)
#define TAF_NV_BLACK_LIST_MAX_NUM                           (51)
#define TAF_NVIM_MAX_USER_SYS_CFG_RAT_NUM                   (5)
#define TAF_MAX_MFR_ID_LEN                                  (31)
#define TAF_MAX_MFR_ID_STR_LEN                              (TAF_MAX_MFR_ID_LEN + 1)
#define TAF_PH_PRODUCT_NAME_LEN                             (15)
#define TAF_PH_PRODUCT_NAME_STR_LEN                         (TAF_PH_PRODUCT_NAME_LEN + 1)
#define MN_MSG_SRV_PARAM_LEN                                (8)                 /* ����ҵ������ṹ��NV���д洢�ĳ��� */
#define TAF_NVIM_MSG_ACTIVE_MESSAGE_MAX_URL_LEN             (160)
#define TAF_SVN_DATA_LENGTH                                 (2)                 /* SVN��Ч���ݳ��� */
#define TAF_PH_NVIM_MAX_GUL_RAT_NUM                         (3)                 /*AT^syscfgex��acqorder����Ľ��뼼������ */
#define TAF_NV_IPV6_FALLBACK_EXT_CAUSE_MAX_NUM              (20)                /* 9130��չIPv6���˴�����չԭ��ֵ������ */
#define PLATFORM_MAX_RAT_NUM                                (7)                 /* ���뼼�����ֵ */
#define MTA_BODY_SAR_WBAND_MAX_NUM                          (5)
#define MTA_BODY_SAR_GBAND_MAX_NUM                          (4)
#define TAF_PH_SIMLOCK_PLMN_STR_LEN                         (8)                 /* Plmn �Ŷγ��� */
#define TAF_MAX_SIM_LOCK_RANGE_NUM                          (20)

/* WINS������NV��Ľṹ�� */
#define WINS_CONFIG_DISABLE                                 (0)                 /* WINS��ʹ�� */
#define WINS_CONFIG_ENABLE                                  (1)                 /* WINSʹ�� */

typedef VOS_UINT32  MMA_QUICK_START_STA_UINT32;
/* ��NVIDö�ٵ�ת����(PS_NV_ID_ENUM, SYS_NV_ID_ENUM, RF_NV_ID_ENUM) */
typedef VOS_UINT16  NV_ID_ENUM_U16;

/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/
/*****************************************************************************
 ö����    : SYSTEM_APP_CONFIG_TYPE_ENUM
 �ṹ˵��  : �ϲ�Խ�Ӧ������ö��
*****************************************************************************/
enum SYSTEM_APP_CONFIG_TYPE_ENUM
{
    SYSTEM_APP_MP                       = 0,                /*�ϲ�Ӧ����Mobile Partner*/
    SYSTEM_APP_WEBUI,                                       /*�ϲ�Ӧ����Web UI*/
    SYSTEM_APP_ANDROID,                                     /*�ϲ�Ӧ����Android*/
    SYSTEM_APP_BUTT
};
typedef VOS_UINT16 SYSTEM_APP_CONFIG_TYPE_ENUM_UINT16;

/* Added by s00246516 for L-C��������Ŀ, 2014-01-23, Begin */
/*****************************************************************************
 ö����    : TAF_NVIM_LC_RAT_COMBINED_ENUM
 �ṹ˵��  : L-C��������������
 1.��    ��   : 2014��01��23��
   ��    ��   : s00246516
   �޸�����   : ����ö��
*****************************************************************************/
enum TAF_NVIM_LC_RAT_COMBINED_ENUM
{
    TAF_NVIM_LC_RAT_COMBINED_GUL  = 0x55,
    TAF_NVIM_LC_RAT_COMBINED_CL   = 0xAA,
    TAF_NVIM_LC_RAT_COMBINED_BUTT
};
typedef VOS_UINT8 TAF_NVIM_LC_RAT_COMBINED_ENUM_UINT8;

/*****************************************************************************
 ö����    : TAF_NVIM_LC_WORK_CFG_ENUM
 �ṹ˵��  : L-C��������������
 1.��    ��   : 2014��01��23��
   ��    ��   : s00246516
   �޸�����   : ����ö��
*****************************************************************************/
enum TAF_NVIM_LC_WORK_CFG_ENUM
{
    TAF_NVIM_LC_INDEPENT_WORK = 0,
    TAF_NVIM_LC_INTER_WORK    = 1,
    TAF_NVIM_LC_WORK_CFG_BUTT
};
typedef VOS_UINT8 TAF_NVIM_LC_WORK_CFG_ENUM_UINT8;
/* Added by s00246516 for L-C��������Ŀ, 2014-01-23, End */

/*****************************************************************************
 ö����    : TAF_MMA_CFREQ_LOCK_MODE_TYPE_ENUM
 ö��˵��  :
*****************************************************************************/
enum TAF_MMA_CFREQ_LOCK_MODE_TYPE_ENUM
{
    TAF_MMA_CFREQ_LOCK_MODE_OFF         = 0x00,     /* ��ֹ��Ƶ���� */
    TAF_MMA_CFREQ_LOCK_MODE_ON          = 0x01,     /* ������Ƶ���� */
    TAF_MMA_CFREQ_LOCK_MODE_BUTT        = 0x02
};
typedef VOS_UINT8 TAF_MMA_CFREQ_LOCK_MODE_TYPE_ENUM_UINT8;

/*****************************************************************************
 ö����    : TAF_LSMS_RESEND_FLAG_ENUM
 �ṹ˵��  : �ط�����ö��
  1.��    ��   : 2016��07��18��
    ��    ��   : z00316370
    �޸�����   : �½�
*****************************************************************************/
enum TAF_LSMS_RESEND_FLAG_ENUM
{
    TAF_LSMS_RESEND_FLAG_DISABLE          = 0,  /* ��ֹ�ط� */
    TAF_LSMS_RESEND_FLAG_ENABLE           = 1,  /* ʹ���ط� */

    TAF_LSMS_RESEND_FLAG_BUTT
};
typedef VOS_UINT8 TAF_LSMS_RESEND_FLAG_ENUM_UINT8;

/*****************************************************************************
 ö����    : TAF_NV_ACTIVE_MODEM_MODE_ENUM
 �ṹ˵��  : DSDS��Ʒ��Modemģʽö��
 1.��    ��   : 2014��09��21��
   ��    ��   : l00198894
   �޸�����   : ����DSDS��/˫������
*****************************************************************************/
enum TAF_NV_ACTIVE_MODEM_MODE_ENUM
{
    TAF_NV_ACTIVE_SINGLE_MODEM              = 0x00,
    TAF_NV_ACTIVE_MULTI_MODEM               = 0x01,

    TAF_NV_ACTIVE_MODEM_MODE_BUTT
};
typedef VOS_UINT8 TAF_NV_ACTIVE_MODEM_MODE_ENUM_UINT8;

/*****************************************************************************
 ö����    : TAF_NV_GPS_CHIP_TYPE_ENUM
 �ṹ˵��  : GPSоƬ����ö��
  1.��    ��   : 2016��7��9��
    ��    ��   : wx270776
    �޸�����   : �½�
*****************************************************************************/
enum TAF_NV_GPS_CHIP_TYPE_ENUM
{
    TAF_NV_GPS_CHIP_BROADCOM          = 0,
    TAF_NV_GPS_CHIP_HISI1102          = 1,

    TAF_NV_GPS_CHIP_BUTT
};
typedef VOS_UINT8 TAF_NV_GPS_CHIP_TYPE_ENUM_UINT8;

/* ME Storage Function On or Off*/
enum MN_MSG_ME_STORAGE_STATUS_ENUM
{
    MN_MSG_ME_STORAGE_DISABLE           = 0x00,
    MN_MSG_ME_STORAGE_ENABLE            = 0x01,
    MN_MSG_ME_STORAGE_BUTT
};
typedef VOS_UINT8 MN_MSG_ME_STORAGE_STATUS_ENUM_UINT8;

/*****************************************************************************
 �ṹ��    : PLATFORM_RAT_TYPE_ENUM
 �ṹ˵��  : ���뼼��

  1.��    ��   : 2012��12��25��
    ��    ��   : z00220246
    �޸�����   : ����
*******************************************************************************/
enum PLATFORM_RAT_TYPE_ENUM
{
    PLATFORM_RAT_GSM,                                                           /*GSM���뼼�� */
    PLATFORM_RAT_WCDMA,                                                         /* WCDMA���뼼�� */
    PLATFORM_RAT_LTE,                                                           /* LTE���뼼�� */
    PLATFORM_RAT_TDS,                                                           /* TDS���뼼�� */
    PLATFORM_RAT_1X,                                                            /* CDMA-1X���뼼�� */
    PLATFORM_RAT_HRPD,                                                          /* CDMA-EV_DO���뼼�� */
    PLATFORM_RAT_BUTT
};
typedef VOS_UINT16 PLATFORM_RAT_TYPE_ENUM_UINT16;

/*****************************************************************************
 ö����    : MTA_WCDMA_BAND_ENUM
 �ṹ˵��  : WCDMAƵ�ζ���
  1.��    ��   : 2013��02��27��
    ��    ��   : l00198894
    �޸�����   : ����ö��
*****************************************************************************/
enum MTA_WCDMA_BAND_ENUM
{
    MTA_WCDMA_I_2100                    = 0x0001,
    MTA_WCDMA_II_1900,
    MTA_WCDMA_III_1800,
    MTA_WCDMA_IV_1700,
    MTA_WCDMA_V_850,
    MTA_WCDMA_VI_800,
    MTA_WCDMA_VII_2600,
    MTA_WCDMA_VIII_900,
    MTA_WCDMA_IX_J1700,
    /* Ƶ���ݲ�֧��
    MTA_WCDMA_X,
    */
    MTA_WCDMA_XI_1500                   = 0x000B,
    /* ����Ƶ���ݲ�֧��
    MTA_WCDMA_XII,
    MTA_WCDMA_XIII,
    MTA_WCDMA_XIV,
    MTA_WCDMA_XV,
    MTA_WCDMA_XVI,
    MTA_WCDMA_XVII,
    MTA_WCDMA_XVIII,
    */
    MTA_WCDMA_XIX_850                   = 0x0013,

    MTA_WCDMA_BAND_BUTT
};
typedef VOS_UINT16 MTA_WCDMA_BAND_ENUM_UINT16;

/*****************************************************************************
 ö����    : TAF_NVIM_RAT_MODE_ENUM
 �ṹ˵��  : ��Ƶ�����еĽ���ģʽö��
 1.��    ��   : 2014��06��03��
   ��    ��   : g00261581
   �޸�����   : V711 Cell Lock��Ŀ
*****************************************************************************/
enum TAF_NVIM_RAT_MODE_ENUM
{
    TAF_NVIM_RAT_MODE_GSM               = 0x01,
    TAF_NVIM_RAT_MODE_WCDMA,
    TAF_NVIM_RAT_MODE_LTE,
    TAF_NVIM_RAT_MODE_CDMA1X,
    TAF_NVIM_RAT_MODE_TDSCDMA,
    TAF_NVIM_RAT_MODE_WIMAX,
    TAF_NVIM_RAT_MODE_EVDO,

    TAF_NVIM_RAT_MODE_BUTT
};
typedef VOS_UINT8 TAF_NVIM_RAT_MODE_ENUM_UINT8;

/*****************************************************************************
 ö����    : TAF_NVIM_GSM_BAND_ENUM
 �ṹ˵��  : GSMƵ�ζ���
 1.��    ��   : 2014��06��04��
   ��    ��   : g00261581
   �޸�����   : ����ö��
*****************************************************************************/
enum TAF_NVIM_GSM_BAND_ENUM
{
    TAF_NVIM_GSM_BAND_850               = 0,
    TAF_NVIM_GSM_BAND_900,
    TAF_NVIM_GSM_BAND_1800,
    TAF_NVIM_GSM_BAND_1900,

    TAF_NVIM_GSM_BAND_BUTT
};
typedef VOS_UINT16 TAF_NVIM_GSM_BAND_ENUM_UINT16;

/*****************************************************************************
 ö����    : NV_MS_MODE_ENUM_UINT8
 �ṹ˵��  :  NV�м�¼���ֻ�ģʽ
  1.��    ��   : 2011��8��18��
    ��    ��   : z00161729
    �޸�����   : ����
  2.��    ��   : 2012��4��23��
    ��    ��   : w00166186
    �޸�����   : DTS2012033104746,�ػ�״̬���÷�����ΪANY����Ч
*****************************************************************************/
enum NV_MS_MODE_ENUM
{
    NV_MS_MODE_CS_ONLY,                                                         /* ��֧��CS�� */
    NV_MS_MODE_PS_ONLY,                                                         /* ��֧��PS�� */
    NV_MS_MODE_CS_PS,                                                           /* CS��PS��֧�� */

    NV_MS_MODE_ANY,                                                             /* ANY,�൱�ڽ�֧��CS�� */

    NV_MS_MODE_BUTT
};
typedef VOS_UINT8 NV_MS_MODE_ENUM_UINT8;


/*****************************************************************************
  4 STRUCT����
*****************************************************************************/
/*****************************************************************************
 �ṹ��    : TAF_NVIM_SERIAL_NUM_STRU
 �ṹ˵��  : ��ӦNVID: 6 SerialNumber

  1.��    ��   : 2013��5��16��
    ��    ��   : j00174725
    �޸�����   : ��ʼ����
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           aucSerialNumber[TAF_NVIM_SN_LEN];
}TAF_NVIM_SERIAL_NUM_STRU;

/*****************************************************************************
 �ṹ��    : SVLTE_SUPPORT_FLAG_STRU
 �ṹ˵��  : en_NV_Item_SVLTE_FLAG NV��ṹ
  1.��    ��   : 2013��6��20��
    ��    ��   : z00161729
    �޸�����   : SVLTE�����Ƿ�֧��:0-��֧�֣�1-֧��
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucSvlteSupportFlag;                     /* SVLTE�����Ƿ�֧��:0-��֧�֣�1-֧�� */
    VOS_UINT8                           aucReserved[3];                         /* ���� */
}SVLTE_SUPPORT_FLAG_STRU;

/* Added by s00246516 for L-C��������Ŀ, 2014-01-23, Begin */
/*****************************************************************************
 �ṹ��    : TAF_NV_LC_CTRL_PARA_STRU
 �ṹ˵��  : en_NV_Item_LC_Ctrl_PARA(70)  ���ڼ�¼L+C��SIM�������Ƿ���ʹ��״̬

 �޸���ʷ      :
  1.��    ��   : 2014��01��23��
    ��    ��   : s00246516
    �޸�����   : L-C��������Ŀ����NV
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucLCEnableFlg;
    TAF_NVIM_LC_RAT_COMBINED_ENUM_UINT8 enRatCombined;
    TAF_NVIM_LC_WORK_CFG_ENUM_UINT8     enLCWorkCfg;
    VOS_UINT8                           aucReserved[1];
}TAF_NV_LC_CTRL_PARA_STRU;
/* Added by s00246516 for L-C��������Ŀ, 2014-01-23, End */

/*****************************************************************************
 �ṹ��    : NAS_NVIM_SYSTEM_APP_CONFIG_STRU
 �ṹ˵��  : en_NV_Item_System_APP_Config(121)�ṹ

 �޸���ʷ      :
  1.��    ��   : 2013��5��22��
    ��    ��   : m00217266
    �޸�����   : �½�
  2.��    ��   : 2015��3��2��
    ��    ��   : w00316404
    �޸�����   : ���ֽڶ��룬����reserveλ
*****************************************************************************/
typedef struct
{
    SYSTEM_APP_CONFIG_TYPE_ENUM_UINT16  usSysAppConfigType;
    VOS_UINT8                           ucReserve1;
    VOS_UINT8                           ucReserve2;
}NAS_NVIM_SYSTEM_APP_CONFIG_STRU;

/*****************************************************************************
�ṹ��    : TAF_NVIM_CFREQ_LOCK_CFG_STRU
�ṹ˵��  : 1X/EVDO��Ƶ���� en_NV_Item_CFreqLock_CFG 2003

  1.��    ��   : 2014��12��29��
    ��    ��   : y00307564
    �޸�����   : ����
*******************************************************************************/
typedef struct
{
    TAF_MMA_CFREQ_LOCK_MODE_TYPE_ENUM_UINT8                 enFreqLockMode;
    VOS_UINT8                                               aucReserve[3];
    VOS_UINT16                                              usSid;
    VOS_UINT16                                              usNid;
    VOS_UINT16                                              usCdmaBandClass;
    VOS_UINT16                                              usCdmaFreq;
    VOS_UINT16                                              usCdmaPn;
    VOS_UINT16                                              usEvdoBandClass;
    VOS_UINT16                                              usEvdoFreq;
    VOS_UINT16                                              usEvdoPn;
}TAF_NVIM_CFREQ_LOCK_CFG_STRU;

/*****************************************************************************
 �ṹ��    : TAF_NVIM_LTE_SMS_CFG_STRU
 �ṹ˵��  : lte����3gpp2�������ò���

 1.��    ��   : 2016��05��03��
   ��    ��   : y00245242
   �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucLteSmsEnable;                         /* LTE����3GPP2�������Կ��� VOS_TURE:ʹ��,VOS_FALSE:��ֹ */
    TAF_LSMS_RESEND_FLAG_ENUM_UINT8     enResendFlag;                           /* �ط�ʹ�ܱ�־ */
    VOS_UINT8                           ucResendMax;                            /* ������������Ĭ�����1�� */
    VOS_UINT8                           ucResendInterval;                       /* ���·��͵ȴ�ʱ�䣬��λ: �룬Ĭ��30�� */
}TAF_NVIM_LTE_SMS_CFG_STRU;

/*****************************************************************************
 �ṹ��    : TAF_NV_DSDS_ACTIVE_MODEM_MODE_STRU
 �ṹ˵��  : en_NV_Item_DSDS_Active_Modem_Mode(2300) ָʾDSDS��Ʒ����ʱ�Ķ�Modemģʽ
 1.��    ��   : 2014��09��21��
   ��    ��   : l00198894
   �޸�����   : ����DSDS��/˫������
*****************************************************************************/
typedef struct
{
    TAF_NV_ACTIVE_MODEM_MODE_ENUM_UINT8 enActiveModem;
    VOS_UINT8                           aucReserve[3];
}TAF_NV_DSDS_ACTIVE_MODEM_MODE_STRU;

/*****************************************************************************
 �ṹ��    : TAF_NVIM_ENHANCED_OPER_NAME_SERVICE_CFG_STRU
 �ṹ˵��  : en_NV_Item_ENHANCED_OPERATOR_NAME_SRV_CFG(2316) NV��ṹ
  1.��    ��   : 2015��2��9��
    ��    ��   : z00161729
    �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucOperNameServicePrioNum;

    /* 1����֧��PNN��cosp��^eonsucs2��ѯʱ���PNN�ļ�����ƥ�����Ӫ������;
       2����֧��CPHS,cops��^eonsucs2��ѯʱ���CPHS�ļ�����ƥ�����Ӫ������;
       3����֧��MM INFO,cops��^eonsucs2��ѯʱ��mm/gmm/emm information�в���ƥ�����Ӫ������;
       4����֧��SPN,cops��^eonsucs2��ѯʱ���SPN�ļ�����ƥ�����Ӫ������*/
    VOS_UINT8                           aucOperNameSerivcePrio[TAF_NVIM_MAX_OPER_NAME_SERVICE_PRIO_NUM];
    VOS_UINT8                           ucReserved1;
    VOS_UINT8                           ucReserved2;
    VOS_UINT8                           ucReserved3;
    VOS_UINT8                           ucPlmnCompareSupportWildCardFlag;       /* plmn�Ƚ�ʱ�Ƿ�֧��ͨ��� */
    VOS_UINT8                           ucWildCard;                             /* ͨ���,ȡֵa-f,�ɴ���0-9�������� */
    VOS_UINT8                           ucReserved4;
    VOS_UINT8                           ucReserved5;                            /* �����ڲ���������^usimstub��^refreshstub�����Ƿ����*/
}TAF_NVIM_ENHANCED_OPER_NAME_SERVICE_CFG_STRU;

/*****************************************************************************
 �ṹ����  : TAF_NV_NVWR_SEC_CTRL_STRU
 �ṹ˵��  : en_NV_Item_NVWR_SEC_CTRL(2321)   NVWR���ȫ����

  1.��    ��   : 2015��04��04��
    ��    ��   : l00198894
    �޸�����   : �����ṹ
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucSecType;
    VOS_UINT8                           ucBlackListNum;
    VOS_UINT16                          ausBlackList[TAF_NV_BLACK_LIST_MAX_NUM];
} TAF_NV_NVWR_SEC_CTRL_STRU;

/*****************************************************************************
 �ṹ��    : TAF_NVIM_CCPU_RESET_RECORD_STRU
 �ṹ˵��  : en_NV_Item_Ccpu_Reset_Record(2331) NV��ṹ
  1.��    ��   : 2015��5��25��
    ��    ��   : n00269697
    �޸�����   : ROAM_PLMN_SELECTION_OPTIMIZE_2.0 �½�
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucCcpuResetFlag;                        /* 0:����C�˵�����λ�������ϵ� */
                                                                                /* 1:��C�˵�����λ */
    VOS_UINT8                           ucReserved1;
    VOS_UINT8                           ucReserved2;
    VOS_UINT8                           ucReserved3;
}TAF_NVIM_CCPU_RESET_RECORD_STRU;

/*****************************************************************************
 �ṹ����  : NAS_NV_PRIVACY_FILTER_CFG_STRU
 �ṹ˵��  : en_NV_Item_Privacy_Log_Filter_Cfg(2352) ���Ź���������Ϣ

  1.��    ��   : 2015��09��17��
    ��    ��   : h00313353
    �޸�����   : �����ṹ
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucFilterEnableFlg;
    VOS_UINT8                           aucReserved[3];
} NAS_NV_PRIVACY_FILTER_CFG_STRU;

/*****************************************************************************
 �ṹ��    : TAF_NVIM_XCPOSRRPT_CFG_STRU
 �ṹ˵��  : en_NV_Item_XCPOSRRPT_CFG(2371)  NV��ṹ
 1.��    ��   : 2016��03��09��
   ��    ��   : h00360002
   �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucXcposrRptNvCfg;                       /* �Ƿ��ϱ����GPS����ĸ�����λ��Ϣ */
    VOS_UINT8                           ucCposrDefault;
    VOS_UINT8                           ucXcposrDefault;
    VOS_UINT8                           ucXcposrRptDefault;
}TAF_NVIM_XCPOSRRPT_CFG_STRU;

/*****************************************************************************
 �ṹ��    : TAF_NVIM_MULTIMODE_RAT_CFG_STRU
 �ṹ˵��  : en_NV_Item_USER_SYS_CFG_RAT_INFO(2387) �û���SYS CFG RAT������Ϣ

 1.��    ��   : 2016��05��03��
   ��    ��   : y00245242
   �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucRatOrderNum;                                      /* syscfgex�����õ�acqoder�е�ָʾ���� */
    VOS_UINT8                           aenRatOrder[TAF_NVIM_MAX_USER_SYS_CFG_RAT_NUM];     /* syscfgex�����õ�acqoder���� */
    VOS_UINT8                           aucReserved[2];
}TAF_NVIM_MULTIMODE_RAT_CFG_STRU;

/*****************************************************************************
 �ṹ��    : TAF_NVIM_GPS_CUST_CFG_STRU
 �ṹ˵��  : NV��en_NV_Item_Gps_Cust_CFG(2396) ���ݽṹ
             enGpsChipType      GPSоƬ����
  1.��    ��   : 2016��7��7��
    ��    ��   : wx270776
    �޸�����   : ����NV��

*****************************************************************************/
typedef struct
{
    TAF_NV_GPS_CHIP_TYPE_ENUM_UINT8     enGpsChipType;
    VOS_UINT8                           ucReserve1;
    VOS_UINT8                           ucReserve2;
    VOS_UINT8                           ucReserve3;
}TAF_NVIM_GPS_CUST_CFG_STRU;

/*****************************************************************************
 �ṹ��    : NAS_NVIM_FOLLOWON_OPENSPEED_FLAG_STRU
 �ṹ˵��  : en_NV_Item_FollowOn_OpenSpeed_Flag(6656)�ṹ
  1.��    ��   : 2013��5��22��
    ��    ��   : m00217266
    �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    MMA_QUICK_START_STA_UINT32          ulQuickStartSta;
}NAS_NVIM_FOLLOWON_OPENSPEED_FLAG_STRU;

/*****************************************************************************
�ṹ��    : NAS_NVIM_AUTOATTACH_STRU
�ṹ˵��  : en_NV_Item_FMRID(8203)�ṹ
1.��    ��   : 2013��5��22��
  ��    ��   : m00217266
  �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           aucMfrId[TAF_MAX_MFR_ID_STR_LEN];
}TAF_PH_FMR_ID_STRU;

/*****************************************************************************
 �ṹ��    : TAF_CCA_TELE_PARA_STRU
 �ṹ˵��  : en_NV_Item_CCA_TelePara(8230)�ṹ#
  1.��    ��   : 2013��5��22��
    ��    ��   : m00217266
    �޸�����   : �½�

  2.��    ��   : 2017��11��20��
    ��    ��   : f00367319
    �޸�����   : �ָ�Ϊ֮ǰ�Ĺ��ܣ�ɾ��T3247��ʱ��ʱ������
*****************************************************************************/
/*Ϊ����V1R1NV��ƥ��ǰ13���ֽڱ���*/
typedef struct
{
    VOS_UINT8                           aucRsv[15];                             /* NV��Ľṹ�У�4�ֽڶ��뷽ʽ���пն�Ҫ�ֶ����� */
    VOS_UINT8                           ucS0TimerLen;
} TAF_CCA_TELE_PARA_STRU;

/*****************************************************************************
�ṹ��    : TAF_PH_PRODUCT_NAME_STRU
�ṹ˵��  : en_NV_Item_ProductName(8205)�ṹ#2611
1.��    ��   : 2013��5��22��
  ��    ��   : m00217266
  �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           aucProductName[TAF_PH_PRODUCT_NAME_STR_LEN];
}TAF_PH_PRODUCT_NAME_STRU;

/*****************************************************************************
 �ṹ��    : NAS_MMA_NVIM_ACCESS_MODE_STRU
 �ṹ˵��  : en_NV_Item_MMA_AccessMode(8232)�ṹ
  1.��    ��   : 2013��5��22��
    ��    ��   : m00217266
    �޸�����   : �½�
  2.��    ��   : 2015��3��2��
    ��    ��   : w00316404
    �޸�����   : ���ֽڶ��룬����reserveλ
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           aucAccessMode[2];
    VOS_UINT8                           ucReserve1;
    VOS_UINT8                           ucReserve2;
}NAS_MMA_NVIM_ACCESS_MODE_STRU;

/*****************************************************************************
 �ṹ��    : NAS_NVIM_MS_CLASS_STRU
 �ṹ˵��  : en_NV_Item_MMA_MsClass(8233)�ṹ
  1.��    ��   : 2013��5��22��
    ��    ��   : m00217266
    �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucMsClass;
    VOS_UINT8                           ucReserved;
}NAS_NVIM_MS_CLASS_STRU;

/*****************************************************************************
 �ṹ��    : TAF_NVIM_SMS_SERVICE_PARA_STRU
 �ṹ˵��  : en_NV_Item_SMS_SERVICE_Para(8237)�ṹ
  1.��    ��   : 2013��5��22��
    ��    ��   : m00217266
    �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           aucSmsServicePara[MN_MSG_SRV_PARAM_LEN];
}TAF_NVIM_SMS_SERVICE_PARA_STRU;

/*****************************************************************************
 �ṹ��    : NAS_NVIM_ROAM_CFG_INFO_STRU
 �ṹ˵��  : en_NV_Item_Roam_Capa(8266) NV��ṹ#
  1.��    ��   : 2011��8��18��
    ��    ��   : z00161729
    �޸�����   : �½�
  2.��    ��   : 2015��3��2��
    ��    ��   : w00316404
    �޸�����   : ���ֽڶ��룬����reserveλ
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucRoamFeatureFlg;                       /*��¼���������Ƿ񼤻�,VOS_FALSE:������,VOS_TRUE:����*/
    VOS_UINT8                           ucRoamCapability;                       /*��¼�û����õ���������*/
    VOS_UINT8                           ucReserve1;
    VOS_UINT8                           ucReserve2;
}NAS_NVIM_ROAM_CFG_INFO_STRU;

/*en_NV_Item_CustomizeSimLockPlmnInfo 8267*/
/*****************************************************************************
�ṹ��    : TAF_CUSTOM_SIM_LOCK_PLMN_RANGE_STRU
�ṹ˵��  : en_NV_Item_CustomizeSimLockPlmnInfo(8267)�ṹǶ�׽ṹ#
            ���ڼ�¼SIM LOCKҪ���PLMN��Ϣ
1.��    ��   : 2013��5��22��
  ��    ��   : m00217266
  �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucMncNum;
    VOS_UINT8                           aucRangeBegin[TAF_PH_SIMLOCK_PLMN_STR_LEN];
    VOS_UINT8                           aucRangeEnd[TAF_PH_SIMLOCK_PLMN_STR_LEN];
}TAF_CUSTOM_SIM_LOCK_PLMN_RANGE_STRU;

/*****************************************************************************
 �ṹ��    : TAF_CUSTOM_SIM_LOCK_PLMN_INFO_STRU
 �ṹ˵��  : en_NV_Item_CustomizeSimLockPlmnInfo(8267)�ṹ#
  1.��    ��   : 2013��5��22��
    ��    ��   : m00217266
    �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulStatus;                               /*�Ƿ񼤻0�����1���� */
    TAF_CUSTOM_SIM_LOCK_PLMN_RANGE_STRU astSimLockPlmnRange[TAF_MAX_SIM_LOCK_RANGE_NUM];
}TAF_CUSTOM_SIM_LOCK_PLMN_INFO_STRU;

/* en_NV_Item_CardlockStatus 8268 */
/*****************************************************************************
 �ṹ��    : TAF_NVIM_CUSTOM_CARDLOCK_STATUS_STRU
 �ṹ˵��  : en_NV_Item_CardlockStatus(8268)�ṹ#
             ���ڱ���SIM LOCK��״̬�Լ�ʣ���������
  1.��    ��   : 2013��5��22��
    ��    ��   : m00217266
    �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulStatus;                               /*�Ƿ񼤻0�����1���� */
    VOS_UINT32                          ulCardlockStatus;
    VOS_UINT32                          ulRemainUnlockTimes;                    /*����ʣ�����*/
}TAF_NVIM_CUSTOM_CARDLOCK_STATUS_STRU;

/*en_NV_Item_CustomizeSimLockMaxTimes 8269*/
/*****************************************************************************
 �ṹ��    : TAF_CUSTOM_SIM_LOCK_MAX_TIMES_STRU
 �ṹ˵��  : en_NV_Item_CustomizeSimLockMaxTimes(8269)�ṹ#
             ���ڱ���SIM LOCKҪ�������������,Ĭ��Ϊδ���Ĭ������������Ϊ10��
  1.��    ��   : 2013��5��22��
    ��    ��   : m00217266
    �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulStatus;                               /*�Ƿ񼤻0�����1���� */
    VOS_UINT32                          ulLockMaxTimes;
}TAF_CUSTOM_SIM_LOCK_MAX_TIMES_STRU;

/*****************************************************************************
 �ṹ��    : NAS_NVIM_CUSTOMIZE_SERVICE_STRU
 �ṹ˵��  : en_NV_Item_CustomizeService(8271)�ṹ#
             ���ڱ�����Ӫ�̶���Ҫ��ҵ������
  1.��    ��   : 2013��5��22��
    ��    ��   : m00217266
    �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulStatus;                               /*�Ƿ񼤻0�����1���� */
    VOS_UINT32                          ulCustomizeService;                     /*�ն�˵������1��byte��Ϊ��û�пն��������4byte����3byte����*/
}NAS_NVIM_CUSTOMIZE_SERVICE_STRU;

/*****************************************************************************
 �ṹ��    : WINS_CONFIG_STRU
 �ṹ˵��  : en_NV_Item_WINS_Config(8297)�ṹ#
  1.��    ��   : 2013��5��22��
    ��    ��   : m00217266
    �޸�����   : �½�
  2.��    ��   : 2015��3��2��
    ��    ��   : w00316404
    �޸�����   : ���ֽڶ��룬����reserveλ
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucStatus;                               /* 1: NV��Ч��־λ��0����Ч */
    VOS_UINT8                           ucWins;                                 /* WINSʹ�ܱ��: 0��Disable,  1��Enable */
    VOS_UINT8                           ucReserve1;
    VOS_UINT8                           ucReserve2;
}WINS_CONFIG_STRU;

/*****************************************************************************
 �ṹ��    : TAF_AT_NVIM_SMS_ACTIVE_MESSAGE_STRU
 �ṹ˵��  : en_NV_Item_SMS_ActiveMessage_Para(8322)�ṹ
  1.��    ��   : 2013��5��22��
    ��    ��   : m00217266
    �޸�����   : �½�
  2.��    ��   : 2015��3��2��
    ��    ��   : w00316404
    �޸�����   : ���ֽڶ��룬����reserveλ
*****************************************************************************/
typedef struct
{
    VOS_INT8                            cStatus;
    VOS_UINT8                           ucEncodeType;
    VOS_UINT8                           ucReserved1;
    VOS_UINT8                           ucReserved2;
    VOS_UINT32                          ulLength;
    VOS_UINT8                           ucData[TAF_NVIM_MSG_ACTIVE_MESSAGE_MAX_URL_LEN];
}TAF_AT_NVIM_SMS_ACTIVE_MESSAGE_STRU;

/*****************************************************************************
 �ṹ��    : TAF_SVN_DATA_STRU
 �ṹ˵��  : en_NV_Item_Imei_Svn(8337)�ṹ#
  1.��    ��   : 2013��5��22��
    ��    ��   : m00217266
    �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucActiveFlag;
    VOS_UINT8                           aucSvn[TAF_SVN_DATA_LENGTH];
    VOS_UINT8                           aucReserve[1];
}TAF_SVN_DATA_STRU;

/*****************************************************************************
 �ṹ��    : APP_VC_NVIM_PC_VOICE_SUPPORT_FLAG_STRU
 �ṹ˵��  : en_NV_Item_PCVOICE_Support_Flg(8471) �Ľṹ

  1.��    ��   : 2013��5��17��
    ��    ��   : l00167671
    �޸�����   : �½�
  2.��    ��   : 2015��3��2��
    ��    ��   : w00316404
    �޸�����   : ���ֽڶ��룬����reserveλ
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          usPcVoiceSupportFlag;                   /* Range:[0,1] */
    VOS_UINT8                           ucReserve1;
    VOS_UINT8                           ucReserve2;
}APP_VC_NVIM_PC_VOICE_SUPPORT_FLAG_STRU;

/*****************************************************************************
 �ṹ��    : MN_MSG_ME_STORAGE_PARM_STRU
 �ṹ˵��  : en_NV_Item_Sms_Me_Storage_Info(8501) NV��ṹ
  1.��    ��   : 2013��5��22��
    ��    ��   : l00167671
    �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    MN_MSG_ME_STORAGE_STATUS_ENUM_UINT8 enMeStorageStatus;                      /* ME�洢���Ź����Ƿ����ñ�־*/
    VOS_UINT8                           aucReserve[1];                          /* NV����صĽṹ�壬��4�ֽڷ�ʽ�£����ֶ�����ն� */
    VOS_UINT16                          usMeStorageNum;                         /* ME�洢�������� */
}MN_MSG_ME_STORAGE_PARM_STRU;

/*****************************************************************************
 �ṹ��    : TAF_PH_RAT_STRU
 �ṹ˵��  : en_NV_Item_RAT_PRIO_LIST(9007) AT^syscfgex�����õ�acqorder�еĳ��Ⱥ���ֵ
  1.��    �� : 2011��06��9��
    ��    �� : l00130025
    �޸����� : �����ṹ
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucRatOrderNum;                          /* syscfgex�����õ�acqoder�е�ָʾ���� */
    VOS_UINT8                           aenRatOrder[TAF_PH_NVIM_MAX_GUL_RAT_NUM];    /* at^syscfgex�����õ�acqoder��˳�� */
}TAF_PH_NVIM_RAT_ORDER_STRU;

/*****************************************************************************
 �ṹ��    : NAS_MMC_NVIM_LTE_INTERNATIONAL_ROAM_CFG_STRU
 �ṹ˵��  : en_NV_Item_Lte_Internation_Roam_Config(9052) NV��ṹ
  1.��    ��   : 2012��3��14��
    ��    ��   : w00176964
    �޸�����   : �½�
  2.��    ��   : 2012��4��25��
    ��    ��   : w00176964
    �޸�����   : DTS2012042403564:�޸�NV��ȡ�Ľṹ��
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucLteRoamAllowedFlg;
    VOS_UINT8                           aucReserve[1];
    VOS_UINT8                           aucRoamEnabledMccList[20];              /* �������εĹ������б� */
}NAS_MMC_NVIM_LTE_INTERNATIONAL_ROAM_CFG_STRU;

/* Added by l60609 for B060 Project, 2012-2-20, Begin   */
/*****************************************************************************
 �ṹ��    : NAS_PREVENT_TEST_IMSI_REG_STRU
 �ṹ˵��  : en_NV_Item_PREVENT_TEST_IMSI_REG�ṹ
  1.��    ��   : 2012��2��22��
    ��    ��   : w00199382
    �޸�����   : ��ֲ
  2.��    ��   : 2015��3��2��
    ��    ��   : w00316404
    �޸�����   : ���ֽڶ��룬����reserveλ
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucStatus;                               /*�Ƿ񼤻0�����1���� */
    VOS_UINT8                           ucActFlg;
    VOS_UINT8                           ucReserve1;
    VOS_UINT8                           ucReserve2;
}NAS_PREVENT_TEST_IMSI_REG_STRU;
/* Added by l60609 for B060 Project, 2012-2-20, End   */

/*****************************************************************************
 �ṹ��    : MN_MSG_MT_CUSTOMIZE_INFO_STRU
 �ṹ˵��  : ���Ž��ն���NV��en_NV_Item_SMS_MT_CUSTOMIZE_INFO(9118)���ݽṹ
             ucActFlag      �����־
             enMtCustomize  ���Ž��ն�������
  1.��    ��   : 2012��11��10��
    ��    ��   : f62575
    �޸�����   : DCM&DEVICE
  2.��    ��   : 2017��10��12��
    ��    ��   : h00360002
    �޸�����   : ����download��������ʱstk���ŷ��Ͷ���
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucActFlag;
    VOS_UINT8                           enMtCustomize;
    VOS_UINT8                           aucMtCustomize[2];                      /* MT������������. [0]: download mt�������Ƿ���ʱ����stk���� */
}MN_MSG_MT_CUSTOMIZE_INFO_STRU;

/*****************************************************************************
 �ṹ��    : NAS_NVIM_CELL_SIGN_REPORT_CFG_STRU
 �ṹ˵��  : en_NV_Item_CELL_SIGN_REPORT_CFG(9123) �ź����������ϱ����������Ϣ
  1.��    ��  : 2012��11��21��
    ��    ��  : z00161729
    �޸�����  : ֧��^cerssi�����ṹ
  2.��    ��   : 2015��3��2��
    ��    ��   : w00316404
    �޸�����   : ���ֽڶ��룬����reserveλ
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucSignThreshold;                        /* �źű仯����,��RSSI�仯������ֵ���������Ҫ�����ϱ��ź�������ȡֵ0��ʾ����㰴Ĭ��ֵ���� */
    VOS_UINT8                           ucMinRptTimerInterval;                  /* ����ϱ���ʱ�� */
    VOS_UINT8                           ucRserved1;
    VOS_UINT8                           ucRserved2;
} NAS_NVIM_CELL_SIGN_REPORT_CFG_STRU;

/*****************************************************************************
 �ṹ��  : TAF_NV_IPV6_FALLBACK_EXT_CAUSE_STRU
 �ṹ˵��: en_NV_Item_IPV6_BACKPROC_EXT_CAUSE(9130) ��������IPv4v6˫ջ���Ż������̵���չԭ��ֵ(SM Cause)�ṹ��

  1.��    ��   : 2012��12��10��
    ��    ��   : Y00213812
    �޸�����   : C50 IPv6 ��Ŀ����
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulActiveFlag;
    VOS_UINT8                           aucSmCause[TAF_NV_IPV6_FALLBACK_EXT_CAUSE_MAX_NUM];
} TAF_NV_IPV6_FALLBACK_EXT_CAUSE_STRU;

/*******************************************************************************
 �ṹ��    : TAF_NVIM_SMS_CLOSE_PATH_CFG_STRU
 �ṹ˵��  : en_NV_Item_SMS_Close_Path(9136)
  1.��    ��   : 2013��7��17��
    ��    ��   : z00234330
    �޸�����   : �����ɺ���
*******************************************************************************/
typedef struct
{
    VOS_UINT8                           ucNvimValid;
    VOS_UINT8                           ucSmsClosePathFlg;
} TAF_NVIM_SMS_CLOSE_PATH_CFG_STRU;

/*****************************************************************************
�ṹ��    : PLATAFORM_RAT_CAPABILITY_STRU
�ṹ˵��  : en_NV_Item_Platform_RAT_CAP(9203) ƽ̨֧�ֵĽ��뼼��

  1.��    ��   : 2012��12��25��
    ��    ��   : z00220246
    �޸�����   : ����
*******************************************************************************/
typedef struct
{
    VOS_UINT16                           usRatNum;                              /* ���뼼������Ŀ*/
    PLATFORM_RAT_TYPE_ENUM_UINT16        aenRatList[PLATFORM_MAX_RAT_NUM];      /* ���뼼�� */
}PLATAFORM_RAT_CAPABILITY_STRU;

/*******************************************************************************
 �ṹ��    : MTA_BODY_SAR_G_PARA_STRU
 �ṹ˵��  : Body SAR G �����ṹ��
 1.��    ��   : 2013��02��27��
   ��    ��   : l00198894
   �޸�����   : �����ṹ��
*******************************************************************************/
typedef struct
{
    VOS_INT16                           sGPRSPower;                             /* GPRS��������ֵ */
    VOS_INT16                           sEDGEPower;                             /* EDGE��������ֵ */
}MTA_BODY_SAR_G_PARA_STRU;

/*******************************************************************************
 �ṹ��    : MTA_BODY_SAR_W_PARA_STRU
 �ṹ˵��  : Body SAR W �����ṹ��
 1.��    ��   : 2013��02��27��
   ��    ��   : l00198894
   �޸�����   : �����ṹ��
*******************************************************************************/
typedef struct
{
    MTA_WCDMA_BAND_ENUM_UINT16          enBand;                                 /* WCDMAƵ�� */
    VOS_INT16                           sPower;                                 /* ��������ֵ */
}MTA_BODY_SAR_W_PARA_STRU;

/*******************************************************************************
 �ṹ��    : MTA_BODY_SAR_PARA_STRU
 �ṹ˵��  : en_NV_Item_BODY_SAR_PARA(9208) Body SAR�������޲����ṹ��
 1.��    ��   : 2013��02��27��
   ��    ��   : l00198894
   �޸�����   : �����ṹ��
*******************************************************************************/
typedef struct
{
    VOS_UINT32                          ulGBandMask;                                /* GSM Band��Maskλ */
    VOS_UINT16                          usWBandNum;                                 /* WCDMA��Band���� */
    VOS_UINT16                          ausReserved1[1];                            /* ����λ */
    MTA_BODY_SAR_G_PARA_STRU            astGBandPara[MTA_BODY_SAR_GBAND_MAX_NUM];   /* GSM��������ֵ */
    MTA_BODY_SAR_W_PARA_STRU            astWBandPara[MTA_BODY_SAR_WBAND_MAX_NUM];   /* WCDMA��������ֵ */
}MTA_BODY_SAR_PARA_STRU;

/*******************************************************************************
 �ṹ��    : TAF_TEMP_PROTECT_CONFIG_STRU
 �ṹ˵��  : �¶ȱ���ʹ��NV��ṹ��(9212)
 1.��    ��   : 2013��04��17��
   ��    ��   : z00214637
   �޸�����   : �����ṹ��
*******************************************************************************/
typedef struct
{
    VOS_UINT8                           ucECallNotifySupport;                   /* �����������¶ȱ�������NV�� */
    VOS_UINT8                           ucSpyStatusIndSupport;                  /* �±�״̬�ϱ�ʹ��NV�� */
    VOS_UINT8                           aucReserved[2];                         /* ������ */
}TAF_TEMP_PROTECT_CONFIG_STRU;

/*******************************************************************************
 �ṹ��    : TAF_NVIM_SMS_DOMAIN_STRU
 �ṹ˵��  : en_NV_Item_SMS_DOMAIN(9237) �ṹ
  1.��    ��   : 2013��11��05��
    ��    ��   : s00217060
    �޸�����   : �����ɺ���
*******************************************************************************/
typedef struct
{
    VOS_UINT8                          ucSmsDomain;
    VOS_UINT8                          aucReserved[3];
} TAF_NVIM_SMS_DOMAIN_STRU;

/*****************************************************************************
�ṹ��    : NV_NAS_JAM_DETECT_CFG_STRU
�ṹ˵��  : en_NV_Item_JAM_DETECT_CFG(9267) �ṹ

1.��    ��   : 2014��4��25��
  ��    ��   : y00142674
  �޸�����   : ����

*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucMode;                                 /* JAM���õ�ģʽ��0:�ر�, 1:�� */
    VOS_UINT8                           ucMethod;                               /* JAM���ʹ�õķ�����1:����1��2:����2,Ŀǰֻ֧��2 */
    VOS_UINT8                           ucFreqNum;                              /* �����Ҫ�ﵽ��Ƶ�������ȡֵ��Χ:[0,255] */
    VOS_UINT8                           ucThreshold;                            /* �����Ҫ�ﵽ��Ƶ�����ֵ��ȡֵ��Χ:[0,70] */
    VOS_UINT8                           ucJamDetectingTmrLen;                   /* ǿ�ź�Ƶ������������޺󣬼��ͬ������Ķ�ʱ��ʱ��(s) */
    VOS_UINT8                           ucJamDetectedTmrLen;                    /* ��⵽���ź󣬵ȴ�����������ʱ��(s) */
    VOS_UINT8                           ucFastReportFlag;                       /* �����������Ƿ���Ҫ�����ϱ� */
    VOS_UINT8                           aucRsv[1];
}NV_NAS_JAM_DETECT_CFG_STRU;

/*******************************************************************************
 �ṹ��    : TAF_NVIM_FREQ_LOCK_CFG_STRU
 �ṹ˵��  : en_NV_Item_FREQ_LOCK_CFG(9273) ��Ƶ�������NV��
  1.��    ��   : 2014��06��03��
    ��    ��   : g00261581
    �޸�����   : V711 Cell Lock��Ŀ
*******************************************************************************/
typedef struct
{
    VOS_UINT8                           ucEnableFlg;                            /* 1:��Ƶ���ܴ򿪣�0:���ܹر� */
    TAF_NVIM_RAT_MODE_ENUM_UINT8        enRatMode;
    TAF_NVIM_GSM_BAND_ENUM_UINT16       enBand;
    VOS_UINT32                          ulLockedFreq;
} TAF_NVIM_FREQ_LOCK_CFG_STRU;

/*****************************************************************************
 �ṹ��    : AT_TRAFFIC_CLASS_CUSTOMIZE_STRU
 �ṹ˵��  : en_NV_Item_TRAFFIC_CLASS_Type(50009)  ���ڶ���PDP����������QoS�� Traffic Class��ֵ�Ľṹ��
*****************************************************************************/
typedef struct
{
    VOS_UINT8                          ucStatus;                                /* 1: NV��Ч��־λ��0����Ч */
    VOS_UINT8                          ucTrafficClass;                          /* Traffic Class��ֵ */
    VOS_UINT8                          ucReserve1;
    VOS_UINT8                          ucReserve2;
}AT_TRAFFIC_CLASS_CUSTOMIZE_STRU;

/*****************************************************************************
 �ṹ��    : TAF_USSD_NVIM_TRANS_MODE_STRU
 �ṹ˵��  : en_NV_Item_CUST_USSD_MODE(50052) �ṹ
  1.��    ��   : 2013��5��22��
    ��    ��   : l00167671
    �޸�����   : �½�
  2.��    ��   : 2015��3��2��
    ��    ��   : w00316404
    �޸�����   : ���ֽڶ��룬����reserveλ
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucStatus;                               /* 1: NV��Ч��־λ��0����Ч */
    VOS_UINT8                           ucUssdTransMode;
    VOS_UINT8                           ucReserve1;
    VOS_UINT8                           ucReserve2;
}TAF_USSD_NVIM_TRANS_MODE_STRU;

/*****************************************************************************
 �ṹ��    : MN_MSG_NVIM_CLASS0_TAILOR_STRU
 �ṹ˵��  : en_NV_Item_SMS_CLASS0_TAILOR(50056) �ṹ
  1.��    ��   : 2013��5��22��
    ��    ��   : l00167671
    �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucActFlg;                               /* NVIM�и����Ƿ񼤻� */
    VOS_UINT8                           enClass0Tailor;
}MN_MSG_NVIM_CLASS0_TAILOR_STRU;

/*******************************************************************************
 �ṹ��    : TAF_CALL_NVIM_CCWA_CTRL_MODE_STRU
 �ṹ˵��  : en_NV_Item_Ccwa_Ctrl_Mode(2340) ���еȴ�����ģʽNV��
  1.��    ��   : 2015��08��24��
    ��    ��   : n00269697
    �޸�����   : ims_call_wait
*******************************************************************************/
typedef struct
{
    /* ��NV �������ú��еȴ��Ŀ���ģʽ��ulCcwaCtrlModeΪ0������еȴ���3gpp������ƣ�
       ulCcwaCtrlModeΪ1������еȴ���UE���ƣ�����VOLTE�����硣
      ����VoLTE�������ϣ�AP����CCWA֧��ʱ��IMS��û�к����罻����VoLTE�ĵ绰��CCWA��UE���ƣ���
       ֧��VOLTE���նˣ�����VOLTE������û�д򿪣������·�CCWAI�����ú��еȴ���
    */
    VOS_UINT8                           enCcwaCtrlMode;                         /* TAF_CALL_CCWA_CTRL_MODE_ENUM_U8 */
    VOS_UINT8                           enCcwaiMode;                            /* TAF_CALL_CCWAI_MODE_ENUM_UINT8 */
    VOS_UINT8                           ucReserved1;
    VOS_UINT8                           ucReserved2;
} TAF_CALL_NVIM_CCWA_CTRL_MODE_STRU;


#pragma pack(pop)

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* __NV_STRU_GUCNAS_H__ */


