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


#ifndef __ACORE_NV_STRU_GUCNAS_H__
#define __ACORE_NV_STRU_GUCNAS_H__

#include "vos.h"
#include "acore_nv_id_gucnas.h"

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
#define AT_MDATE_STRING_LENGTH                                  (20)
#define TAF_AT_NVIM_CLIENT_CFG_LEN                              (128)
#define AT_DISSD_PWD_LEN                                        (16)
#define AT_OPWORD_PWD_LEN                                       (16)
#define AT_FACINFO_INFO1_LENGTH                                 (128)
#define AT_FACINFO_INFO2_LENGTH                                 (128)
#define AT_FACINFO_STRING_LENGTH                                ((AT_FACINFO_INFO1_LENGTH + 1) + (AT_FACINFO_INFO2_LENGTH + 1))
#define AT_FACINFO_INFO1_STR_LENGTH                             (AT_FACINFO_INFO1_LENGTH + 1)
#define AT_FACINFO_INFO2_STR_LENGTH                             (AT_FACINFO_INFO2_LENGTH + 1)
#define AT_NVIM_SETZ_LEN                                        (16)
#define AT_NOTSUPPORT_STR_LEN                                   (16)
#define AT_NVIM_RIGHT_PWD_LEN                                   (16)
#define ADS_UL_QUEUE_SCHEDULER_PRI_MAX                          (9)             /* ���ȼ���������� */
#define AT_WIFI_SSID_LEN_MAX                                    (33)            /* WIFI SSID KEY��󳤶� */
#define AT_WIFI_KEY_LEN_MAX                                     (27)
#define AT_WIFI_WLAUTHMODE_LEN                                  (16)            /* ��Ȩģʽ�ַ������� */
#define AT_WIFI_ENCRYPTIONMODES_LEN                             (5)             /* ����ģʽ�ַ������� */
#define AT_WIFI_WLWPAPSK_LEN                                    (65)            /* WPA�������ַ������� */
#define AT_WIFI_MAX_SSID_NUM                                    (4)             /* ���֧��4��SSID */
#define AT_WIFI_KEY_NUM                                         (AT_WIFI_MAX_SSID_NUM)
#define AT_MAX_ABORT_CMD_STR_LEN                                (16)
#define AT_MAX_ABORT_RSP_STR_LEN                                (16)
#define AT_PRODUCT_NAME_MAX_NUM                                 (29)
#define AT_PRODUCT_NAME_LENGHT                                  (AT_PRODUCT_NAME_MAX_NUM + 1)
#define AT_WEBUI_PWD_MAX                                        (16)            /*WEB UI ������󳤶�*/
#define AT_AP_XML_RPT_SRV_URL_LEN                               (127)
#define AT_AP_XML_RPT_SRV_URL_STR_LEN                           (AT_AP_XML_RPT_SRV_URL_LEN + 1)
#define AT_AP_NVIM_XML_RPT_INFO_TYPE_LEN                        (127)
#define AT_AP_NVIM_XML_RPT_INFO_TYPE_STR_LEN                    (AT_AP_NVIM_XML_RPT_INFO_TYPE_LEN + 1)


/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/
enum AT_UART_LINK_TYPE_ENUM
{
    AT_UART_LINK_TYPE_OM        = 1,          /* OMģʽ */
    AT_UART_LINK_TYPE_AT        = 2,          /* ATģʽ */
    AT_UART_LINK_TYPE_BUTT
};
typedef VOS_UINT16 AT_UART_LINK_TYPE_ENUM_UINT16;

/*****************************************************************************
 ö����    : NAPI_WEIGHT_ADJ_MODE_ENUM
 �ṹ˵��  : NAPI Weight����ģʽö��
 1.��    ��   : 2017��06��05��
   ��    ��   : l00373346
   �޸�����   : �½�
*****************************************************************************/
enum NAPI_WEIGHT_ADJ_MODE_ENUM
{
    NAPI_WEIGHT_ADJ_STATIC_MODE           = 0x00,
    NAPI_WEIGHT_ADJ_DYNAMIC_MODE          = 0x01,

    NAPI_WEIGHT_ADJ_MODE_BUTT
};
typedef VOS_UINT8 NAPI_WEIGHT_ADJ_MODE_ENUM_UINT8;

/*****************************************************************************
  4 STRUCT����
*****************************************************************************/
/*****************************************************************************
 �ṹ����  : USB_ENUM_STATUS_STRU
 �ṹ˵��  : en_NV_Item_USB_Enum_Status(21)�ṹ
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          status;
    VOS_UINT32                          value;
    VOS_UINT32                          reserve1;
    VOS_UINT32                          reserve2;
}USB_ENUM_STATUS_STRU;

/*****************************************************************************
 �ṹ��    : TAF_AT_NVIM_COMMDEGBUG_CFG_STRU
 �ṹ˵��  : en_NV_Item_COMMDEGBUG_CFG(27)�ṹ

  1.��    ��   : 2013��5��22��
    ��    ��   : m00217266
    �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulCommDebugFlag;
}TAF_AT_NVIM_COMMDEGBUG_CFG_STRU;

/*****************************************************************************
 �ṹ��    : nv_pid_enable_type
 �ṹ˵��  : en_NV_Item_PID_Enable_Type(31)�ṹ
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulPidEnabled;
}nv_pid_enable_type;

/*****************************************************************************
 �ṹ��    : OM_SW_VER_FLAG_STRU
 �ṹ˵��  : NV_SW_VER_Flag(36)�ṹ

  1.��    ��   : 2013��16��
    ��    ��   : j00174725
    �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulNvSwVerFlag;                          /*[0,1]*/
}OM_SW_VER_FLAG_STRU;

/*****************************************************************************
 �ṹ��    : TAF_AT_NVIM_DISSD_PWD_STRU
 �ṹ˵��  : en_NV_Item_AT_DISSD_PWD(43)�ṹ

  1.��    ��   : 2013��5��22��
    ��    ��   : m00217266
    �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    VOS_INT8                           acATE5DissdPwd[AT_DISSD_PWD_LEN];
}TAF_AT_NVIM_DISSD_PWD_STRU;

/*****************************************************************************
 �ṹ��    : TAF_AT_NVIM_DISLOG_PWD_NEW_STRU
 �ṹ˵��  : en_NV_Item_AT_DISLOG_PWD_NEW(48)�ṹ

  1.��    ��   : 2013��5��22��
    ��    ��   : m00217266
    �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    VOS_INT8                           acATOpwordPwd[AT_OPWORD_PWD_LEN];
}TAF_AT_NVIM_DISLOG_PWD_NEW_STRU;

/*****************************************************************************
 �ṹ��    : TAF_AT_NVIM_DEFAULT_LINK_OF_UART_STRU
 �ṹ˵��  : en_NV_Item_DEFAULT_LINK_OF_UART(53)�ṹ
  1.��    ��   : 2013��5��22��
    ��    ��   : m00217266
    �޸�����   : �½�
  2.��    ��   : 2015��3��2��
    ��    ��   : w00316404
    �޸�����   : ���ֽڶ��룬����reserveλ
*****************************************************************************/
typedef struct
{
    AT_UART_LINK_TYPE_ENUM_UINT16       enUartLinkType;
    VOS_UINT8                           ucReserve1;
    VOS_UINT8                           ucReserve2;
}TAF_AT_NVIM_DEFAULT_LINK_OF_UART_STRU;

/*****************************************************************************
 �ṹ��    : VBAT_CALIBART_TYPE
 �ṹ˵��  : en_NV_Item_BATTERY_ADC(90)�ṹ ���У׼�������ݽṹ
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          min_value;
    VOS_UINT16                          max_value;
}VBAT_CALIBART_TYPE;

/*****************************************************************************
 �ṹ��    : TAF_AT_NVIM_FACTORY_INFO_STRU
 �ṹ˵��  : en_NV_Item_AT_FACTORY_INFO(115)�ṹ
  1.��    ��   : 2013��5��22��
    ��    ��   : m00217266
    �޸�����   : �½�
  2.��    ��   : 2015��3��2��
    ��    ��   : w00316404
    �޸�����   : ���ֽڶ��룬����reserveλ
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           aucFactInfo1[AT_FACINFO_INFO1_STR_LENGTH];
    VOS_UINT8                           aucFactInfo2[AT_FACINFO_INFO2_STR_LENGTH];
    VOS_UINT8                           ucReserve1;
    VOS_UINT8                           ucReserve2;
}TAF_AT_NVIM_FACTORY_INFO_STRU;

/*****************************************************************************
�ṹ��    : TAF_AT_NVIM_MANUFACTURE_DATE_STRU
�ṹ˵��  : en_NV_Item_AT_MANUFACTURE_DATE(116)�ṹ
1.��    ��   : 2013��5��22��
  ��    ��   : m00217266
  �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           aucMDate[AT_MDATE_STRING_LENGTH];
}TAF_AT_NVIM_MANUFACTURE_DATE_STRU;

/*****************************************************************************
 �ṹ��    : TAF_NV_UART_RI_CFG_STRU
 �ṹ˵��  : UART RI�źŲ��νṹ

  1.��    ��   : 2013��09��18��
    ��    ��   : A00165503
    �޸�����   : �����ṹ
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulSmsRiOnInterval;      /* ����RI�ߵ�ƽ����ʱ��(ms) */
    VOS_UINT32                          ulSmsRiOffInterval;     /* ����RI�͵�ƽ����ʱ��(ms) */
    VOS_UINT32                          ulVoiceRiOnInterval;    /* ����RI�ߵ�ƽ����ʱ��(ms) */
    VOS_UINT32                          ulVoiceRiOffInterval;   /* ����RI�͵�ƽ����ʱ��(ms) */
    VOS_UINT8                           ucVoiceRiCycleTimes;    /* ����RI�������ڴ���     */
    VOS_UINT8                           aucReserved[3];
} TAF_NV_UART_RI_STRU;

/*****************************************************************************
 �ṹ��    : TAF_NV_UART_FRAME_STRU
 �ṹ˵��  : UART ֡�ṹ

  1.��    ��   : 2013��09��18��
    ��    ��   : j00174725
    �޸�����   : �����ṹ
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucFormat;               /* UART ����λֹͣλλ�� */
    VOS_UINT8                           ucParity;               /* UARTУ�鷽ʽ */
    VOS_UINT8                           aucReserved[2];

} TAF_NV_UART_FRAME_STRU;

/*****************************************************************************
 �ṹ��    : TAF_NV_UART_CFG_STRU
 �ṹ˵��  : en_NV_Item_UART_CFG(141)�ṹ   UART �������

  1.��    ��   : 2013��09��18��
    ��    ��   : j00174725
    �޸�����   : �����ṹ
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulBaudRate;             /* UART������ */
    TAF_NV_UART_FRAME_STRU              stFrame;                /* UART֡��ʽ */
    TAF_NV_UART_RI_STRU                 stRiConfig;             /* UART Ring������ */
} TAF_NV_UART_CFG_STRU;

/*****************************************************************************
 �ṹ��    : TAF_NV_PORT_BUFF_CFG_STRU
 �ṹ˵��  : en_NV_Item_PORT_BUFF_CFG(144)�ṹ   �������

  1.��    ��   : 2013��12��28��
    ��    ��   : j00174725
    �޸�����   : �����ṹ
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucEnableFlg;
    VOS_UINT8                           aucReserved[3];
} TAF_NV_PORT_BUFF_CFG_STRU;

/*****************************************************************************
 �ṹ��    : TAF_NVIM_CDMAMODEMSWITCH_NOT_RESET_CFG_STRU
 �ṹ˵��  : en_NV_Item_CDMAMODEMSWITCH_NOT_RESET_CFG(2102)�ṹ  CDMAMODEMSWITCH���������Կ���NV

  1.��    ��   : 2016��10��26��
    ��    ��   : h00313353
    �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucEnableFlg;                            /* CDMAMODEMSWITCH����������NV����, 1: �򿪣�0���ر� */
    VOS_UINT8                           aucReversed[7];                         /* ���ֽڶ��� */
} TAF_NVIM_CDMAMODEMSWITCH_NOT_RESET_CFG_STRU;

/*****************************************************************************
 �ṹ��    : TAF_NVIM_CCPU_RESET_RECORD_STRU
 �ṹ˵��  : en_NV_Item_Ipv6_Address_Test_Mode_Cfg(2336) NV��ṹ
  1.��    ��   : 2015��7��29��
    ��    ��   : n00269697
    �޸�����   : DTS2015072804293 �½�
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulIpv6AddrTestModeCfg;                  /* 0x55aa55aa�ǲ���ģʽ������ֵΪ����ģʽ */
}TAF_NVIM_IPV6_ADDR_TEST_MODE_CFG_STRU;

/*****************************************************************************
 �ṹ��    : TAF_NV_ADS_IPF_MODE_CFG_STRU
 �ṹ˵��  : en_NV_Item_ADS_IPF_MODE_CFG(2337)  IPF����ADS�������ݵ�ģʽ

 1.��    ��   : 2016��11��3��
   ��    ��   : l00373346
   �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucIpfMode;                              /* IPF����ADS�������ݵ�ģʽ, 0: �ж�������(Ĭ��)��1���߳������� */
    VOS_UINT8                           ucReserved0;
    VOS_UINT8                           ucReserved1;
    VOS_UINT8                           ucReserved2;
} TAF_NV_ADS_IPF_MODE_CFG_STRU;

/*****************************************************************************
 �ṹ��    : OM_MANUFACTUREINFO_STRU
 �ṹ˵��  : ��ӦNVID: 2351 ManufactureInfo old: 32768

  1.��    ��   : 2013��5��16��
    ��    ��   : j00174725
    �޸�����   : ��ʼ����
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           aucInfo[128];
}OM_MANUFACTUREINFO_STRU;

/*****************************************************************************
 �ṹ����  : TAF_NV_ADS_WAKE_LOCK_CFG_STRU
 �ṹ˵��  : en_NV_Item_ADS_WAKE_LOCK_CFG(2354) ADS WALE LOCK���ýṹ

  1.��    ��   : 2015��09��23��
    ��    ��   : A00165503
    �޸�����   : �����ṹ
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulEnable;                               /* ʹ�ܱ�ʶ */
    VOS_UINT32                          ulTxWakeTimeout;                        /* ���з��ͻ��ѳ�ʱ���� */
    VOS_UINT32                          ulRxWakeTimeout;                        /* ���н��ջ��ѳ�ʱ���� */
    VOS_UINT32                          ulReserved;
} TAF_NV_ADS_WAKE_LOCK_CFG_STRU;

/*****************************************************************************
 �ṹ����  : TAF_NV_PRINT_MODEM_LOG_TYPE_STRU
 �ṹ˵��  : en_NV_Item_Print_Modem_Log_Type(2361)  �����Ƿ����modem log������

  1.��    ��   : 2015��10��17��
    ��    ��   : w00316404
    �޸�����   : �����ṹ
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucPrintModemLogType;                    /* 0:���modem log��1:�����modem log��default:0 */
    VOS_UINT8                           ucReserved0;
    VOS_UINT8                           ucReserved1;
    VOS_UINT8                           ucReserved2;
} TAF_NV_PRINT_MODEM_LOG_TYPE_STRU;

/*****************************************************************************
 �ṹ����  : TAF_NV_ADS_MEM_CFG_STRU
 �ṹ˵��  : �ڴ�������Ϣ

  1.��    ��   : 2015��10��06��
    ��    ��   : A00165503
    �޸�����   : �����ṹ
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          usBlkNum;
    VOS_UINT16                          usReserved0;
    VOS_UINT16                          usReserved1;
    VOS_UINT16                          usReserved2;

} TAF_NV_ADS_MEM_CFG_STRU;

/*****************************************************************************
 �ṹ����  : TAF_NV_ADS_MEM_POOL_CFG_STRU
 �ṹ˵��  : en_NV_Item_ADS_MEM_POOL_CFG(2369) �ڴ�����ýṹ

  1.��    ��   : 2015��10��06��
    ��    ��   : A00165503
    �޸�����   : �����ṹ
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulEnable;
    TAF_NV_ADS_MEM_CFG_STRU             astMemCfg[2];
} TAF_NV_ADS_MEM_POOL_CFG_STRU;

/*****************************************************************************
 �ṹ����  : TAF_NV_ADS_ERROR_FEEDBACK_CFG_STRU
 �ṹ˵��  : en_NV_Item_ADS_PACKET_ERROR_FEEDBACK_CFG() ����������

  1.��    ��   : 2017��7��11��
    ��    ��   : A00165503
    �޸�����   : �����ṹ
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulEnabled;                              /* ʹ�ܱ�ʶ */
    VOS_UINT32                          ulErrorRateThreshold;                   /* ���ݴ���������(�ٷֱ�) */
    VOS_UINT32                          ulDetectDuration;                       /* �����(����) */
    VOS_UINT32                          ulReserved;                             /* ���� */

} TAF_NV_ADS_ERROR_FEEDBACK_CFG_STRU;

/*****************************************************************************
 �ṹ��    : TAF_NVIM_USER_REBOOT_SUPPORT_STRU
 �ṹ˵��  : en_NV_Item_User_Reboot_Support_Flg(2391)    �Ƿ�֧���û�����������λ
 1.��    ��   : 2016��05��30��
   ��    ��   : h00360002
   �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucUserRebootConfig;
    VOS_UINT8                           ucReserved1;
    VOS_UINT8                           ucReserved2;
    VOS_UINT8                           ucReserved3;
}TAF_NVIM_USER_REBOOT_SUPPORT_STRU;

/*****************************************************************************
 �ṹ��    : TAF_NV_PDP_REDIAL_FOR_NO_CAUSE_CFG_STRU
 �ṹ˵��  : en_NV_Item_PDP_REDIAL_FOR_NO_CAUSE_CFG(2413) ������ԭ��ֵ�ز����ܿ���

 1.��    ��   : 2016��08��31��
   ��    ��   : l00373346
   �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucEnable;                               /* ������ԭ��ֵ�ز����ܿ���, 1: �򿪣�0���ر� */
    VOS_UINT8                           aucReversed[3];                         /* ���ֽڶ��� */
} TAF_NV_PDP_REDIAL_FOR_NO_CAUSE_CFG_STRU;

/*****************************************************************************
 �ṹ��    : RNIC_NAPI_PKT_NUM_PER_SEC_LEVEL_STRU
 �ṹ˵��  : RNIC NAPIÿ���հ�����λ����

 1.��    ��   : 2017��03��10��
   ��    ��   : l00373346
   �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulDlPktNumPerSecLevel1;                 /* ÿ���հ�����λLevel1 */
    VOS_UINT32                          ulDlPktNumPerSecLevel2;                 /* ÿ���հ�����λLevel2 */
    VOS_UINT32                          ulDlPktNumPerSecLevel3;                 /* ÿ���հ�����λLevel3 */
    VOS_UINT32                          ulDlPktNumPerSecLevel4;                 /* ÿ���հ�����λLevel4 */
    VOS_UINT32                          ulReserved0;                            /* ����λ */
    VOS_UINT32                          ulReserved1;                            /* ����λ */
    VOS_UINT32                          ulReserved2;                            /* ����λ */
    VOS_UINT32                          ulReserved3;                            /* ����λ */

} RNIC_NAPI_DL_PKT_NUM_PER_SEC_LEVEL_STRU;

/*****************************************************************************
 �ṹ��    : RNIC_NAPI_WEIGHT_LEVEL_STRU
 �ṹ˵��  : RNIC NAPI Weight��λ����

 1.��    ��   : 2017��03��10��
   ��    ��   : l00373346
   �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucNapiWeightLevel1;                     /* NAPI Weightֵ��λLevel1 */
    VOS_UINT8                           ucNapiWeightLevel2;                     /* NAPI Weightֵ��λLevel2 */
    VOS_UINT8                           ucNapiWeightLevel3;                     /* NAPI Weightֵ��λLevel3 */
    VOS_UINT8                           ucNapiWeightLevel4;                     /* NAPI Weightֵ��λLevel4 */
    VOS_UINT8                           ucReserved0;                            /* ����λ */
    VOS_UINT8                           ucReserved1;                            /* ����λ */
    VOS_UINT8                           ucReserved2;                            /* ����λ */
    VOS_UINT8                           ucReserved3;                            /* ����λ */

} RNIC_NAPI_WEIGHT_LEVEL_STRU;

/*****************************************************************************
 �ṹ��    : RNIC_NAPI_WEIGHT_DYNAMIC_ADJ_CFG_STRU
 �ṹ˵��  : RNIC NAPI Weight��̬������������

 1.��    ��   : 2017��03��10��
   ��    ��   : l00373346
   �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    RNIC_NAPI_DL_PKT_NUM_PER_SEC_LEVEL_STRU   stDlPktNumPerSecLevel;            /* RNIC��������ÿ���հ�������λֵ */
    RNIC_NAPI_WEIGHT_LEVEL_STRU               stNapiWeightLevel;                /* NAPI Weightֵ��λ */

} RNIC_NAPI_WEIGHT_DYNAMIC_ADJ_CFG_STRU;

/*****************************************************************************
 �ṹ��    : TAF_NV_RNIC_NET_IF_CFG_STRU
 �ṹ˵��  : en_NV_Item_Rnic_Net_If_Cfg(2421)     RNIC�������ݵ�Linux����Э��ջ�Ľӿ�ģʽ����

 1.��    ��   : 2017��03��10��
   ��    ��   : l00373346
   �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    VOS_UINT8                                     ucNetInterfaceMode;           /* RNIC�����������ݵ�Linux����Э��ջ�Ľӿ�ģʽ, 0: Net_rx(Ĭ��)��1��NAPI�ӿ� */
    NAPI_WEIGHT_ADJ_MODE_ENUM_UINT8               enNapiWeightAdjMode;          /* NAPI Weight����ģʽ, 0: ��̬ģʽ��1����̬����ģʽ */
    VOS_UINT8                                     ucNapiPollWeight;             /* RNIC����NAPI��ʽһ��poll��������� */
    VOS_UINT8                                     ucReserved0;                  /* ����λ */
    VOS_UINT16                                    usNapiPollQueMaxLen;          /* Napi poll�������֧�ֳ��� */
    VOS_UINT8                                     ucReserved1;                  /* ����λ */
    VOS_UINT8                                     ucReserved2;                  /* ����λ */
    RNIC_NAPI_WEIGHT_DYNAMIC_ADJ_CFG_STRU         stNapiWeightDynamicAdjCfg;    /* Napi Weight��̬�������� */

} TAF_NV_RNIC_NET_IF_CFG_STRU;

/*****************************************************************************
 �ṹ��    : TAF_AT_NVIM_AT_CLIENT_CFG_STRU
 �ṹ˵��  : en_NV_Item_AT_CLIENT_CONFIG 2437

  1.��    ��   : 2017��02��20��
    ��    ��   : lwx331495
    �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    VOS_UINT8                          aucAtClientConfig[TAF_AT_NVIM_CLIENT_CFG_LEN];     /*������nvԤ����32���ֽ�*/
}TAF_AT_NVIM_AT_CLIENT_CFG_STRU;

/*****************************************************************************
 �ṹ��    : TAF_NV_PDP_REDIAL_FOR_NO_PDP_TYPE_CAUSE_CFG_STRU
 �ṹ˵��  : ����ܾ�28��ԭ��ֵ�ز����ܿ���

 1.��    ��   : 2017��05��20��
   ��    ��   : w00316404
   �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucEnablePolicy;                         /* ����ܾ�28��ԭ��ֵ�ز����ܿ���, 1: �򿪣�0���ر� */
    VOS_UINT8                           ucReserved1;
    VOS_UINT8                           ucReserved2;
    VOS_UINT8                           ucReserved3;
} TAF_NV_PDP_REDIAL_FOR_NO_PDP_TYPE_CAUSE_CFG_STRU;

/*****************************************************************************
�ṹ��    : TAF_AT_NV_GMM_INFO_STRU
�ṹ˵��  : en_NV_Item_GmmInfo 8200(�ѷ���)
1.��    ��   : 2013��5��22��
  ��    ��   : m00217266
  �޸�����   : �½�
2.��    ��   : 2015��3��2��
  ��    ��   : w00316404
  �޸�����   : ���ֽڶ��룬����reserveλ
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           aucGmmInfo[2];
    VOS_UINT8                           ucReserve1;
    VOS_UINT8                           ucReserve2;
}TAF_AT_NV_GMM_INFO_STRU;

/*****************************************************************************
 �ṹ��    : AT_NVIM_SEND_DOMAIN_STRU
 �ṹ˵��  : en_NV_Item_SMS_SEND_DOMAIN(8295)�ṹ#
  1.��    ��   : 2013��5��22��
    ��    ��   : m00217266
    �޸�����   : �½�
  2.��    ��   : 2015��3��2��
    ��    ��   : w00316404
    �޸�����   : ���ֽڶ��룬����reserveλ
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucActFlg;
    VOS_UINT8                           ucSendDomain;
    VOS_UINT8                           ucReserve1;
    VOS_UINT8                           ucReserve2;
}AT_NVIM_SEND_DOMAIN_STRU;/* NVIM���ж��ŷ�����Ľṹ */

/*****************************************************************************
 �ṹ��    : TAF_AT_NVIM_RXDIV_CONFIG_STRU
 �ṹ˵��  : en_NV_Item_RXDIV_CONFIG(8319)�ṹ
  1.��    ��   : 2013��5��22��
    ��    ��   : m00217266
    �޸�����   : �½�
  2.��    ��   : 2015��3��2��
    ��    ��   : w00316404
    �޸�����   : ���ֽڶ��룬����reserveλ
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucVaild;
    VOS_UINT8                           ucReserved1;
    VOS_UINT8                           ucReserved2;
    VOS_UINT8                           ucReserved3;
}TAF_AT_NVIM_RXDIV_CONFIG_STRU;

/*****************************************************************************
 �ṹ��    : TAF_AT_NVIM_REPORT_REG_ACT_FLG_STRU
 �ṹ˵��  : en_NV_Item_REPORT_REG_ACT_FLG(8340)�ṹ
  1.��    ��   : 2013��5��22��
    ��    ��   : m00217266
    �޸�����   : �½�
  2.��    ��   : 2015��3��2��
    ��    ��   : w00316404
    �޸�����   : ���ֽڶ��룬����reserveλ
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          usReportRegActFlg;
    VOS_UINT8                           ucReserve1;
    VOS_UINT8                           ucReserve2;
}TAF_AT_NVIM_REPORT_REG_ACT_FLG_STRU;

/*****************************************************************************
�ṹ��    : NDIS_NV_DHCP_LEASE_HOUR_STRU
�ṹ˵��  : en_NV_Item_NDIS_DHCP_DEF_LEASE_TIME 8344 �ṹ
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulDhcpLeaseHour;                        /* Range:[0x1,0x2250] */
}NDIS_NV_DHCP_LEASE_HOUR_STRU;

/*****************************************************************************
�ṹ��    : NAS_NV_CREG_CGREG_CI_FOUR_BYTE_RPT_STRU
�ṹ˵��  : en_NV_Item_CREG_CGREG_CI_Four_Byte_Rpt(8345)�ṹ#
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
    VOS_UINT8                           ucCiBytesRpt;                           /* <CI>���ϱ��ֽ�����־��0��2�ֽ��ϱ�, 1��4�ֽ��ϱ� */
    VOS_UINT8                           ucReserve1;
    VOS_UINT8                           ucReserve2;
}NAS_NV_CREG_CGREG_CI_FOUR_BYTE_RPT_STRU; /* VDF����: CREG/CGREG����<CI>���Ƿ���4�ֽ��ϱ���NV����ƽṹ�� */

/*****************************************************************************
 �ṹ��    : TAF_AT_TZ_STRU
 �ṹ˵��  : en_NV_Item_ATSETZ_RET_VALUE(8488) NV��ṹ
  1.��    ��   : 2011��7��25��
    ��    ��   : z00161729
    �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    VOS_INT8                            acTz[AT_NVIM_SETZ_LEN];
}TAF_AT_TZ_STRU;

/*****************************************************************************
 �ṹ��    : AT_NOT_SUPPORT_CMD_ERROR_TEXT_STRU
 �ṹ˵��  : en_NV_Item_NOT_SUPPORT_RET_VALUE(8489) NV��ṹ
  1.��    ��   : 2013��5��22��
    ��    ��   : l00167671
    �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    VOS_INT8                            acErrorText[AT_NOTSUPPORT_STR_LEN];
}TAF_AT_NOT_SUPPORT_CMD_ERROR_TEXT_STRU;

/*****************************************************************************
 �ṹ��    : TAF_AT_NVIM_RIGHT_OPEN_FLAG_STRU
 �ṹ˵��  : en_NV_Item_AT_RIGHT_PASSWORD(8492) NV��ṹ
  1.��    ��   : 2013��5��22��
    ��    ��   : l00167671
    �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          enRightOpenFlg;
    VOS_INT8                            acPassword[AT_NVIM_RIGHT_PWD_LEN];
}TAF_AT_NVIM_RIGHT_OPEN_FLAG_STRU;

/*****************************************************************************
�ṹ��    : TAF_NDIS_NV_IPV6_ROUTER_MTU_STRU
�ṹ˵��  : en_NV_Item_IPV6_ROUTER_MTU 8513(��nv�����ڱ���,�ýṹ����û��ʹ��)
1.��    ��   : 2013��5��22��
  ��    ��   : m00217266
  �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulIpv6RouterMtu;
}TAF_NDIS_NV_IPV6_ROUTER_MTU_STRU;

/*****************************************************************************
 �ṹ����   : AT_IPV6_CAPABILITY_STRU
 Э����   :
 ASN.1 ���� :
 �ṹ˵��   : en_NV_Item_IPV6_CAPABILITY(8514)  IPV6����NV����ƽṹ��
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucStatus;           /* NV��Ч��־, 1: ��Ч��0����Ч */
    VOS_UINT8                           ucIpv6Capablity;    /* IPV6���� */
    VOS_UINT8                           aucReversed[2];     /* ���ֽڶ��� */
} AT_NV_IPV6_CAPABILITY_STRU;

/*****************************************************************************
 �ṹ��    : NAS_PREVENT_TEST_IMSI_REG_STRU
 �ṹ˵��  : en_NV_Item_DIAL_CONNECT_DISPLAY_RATE(8516)
  1.��    ��   : 2013��5��22��
    ��    ��   : l00167671
    �޸�����   : ��ֲ
  2.��    ��   : 2015��3��2��
    ��    ��   : w00316404
    �޸�����   : ���ֽڶ��룬����reserveλ
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucGsmConnectRate;
    VOS_UINT8                           ucGprsConnectRate;
    VOS_UINT8                           ucEdgeConnectRate;
    VOS_UINT8                           ucWcdmaConnectRate;
    VOS_UINT8                           ucDpaConnectRate;
    VOS_UINT8                           ucReserve1;
    VOS_UINT8                           ucReserve2;
    VOS_UINT8                           ucReserve3;
}AT_NVIM_DIAL_CONNECT_DISPLAY_RATE_STRU;

/*****************************************************************************
 �ṹ��    : ADS_UL_QUEUE_SCHEDULER_PRI_NV_STRU
 �ṹ˵��  : en_NV_Item_ADS_Queue_Scheduler_Pri(9039)  ADS���ж������ȼ���NV�ṹ

  1.��    ��   : 2012��02��24��
    ��    ��   : f00179208
    �޸�����   : �����ṹ
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulStatus;                               /* ��NV���״̬ 0:δʹ�ܣ���ʾ���������ȼ����ȼ����ȴ��� 1:ʹ�ܣ��������ȼ��㷨 */
    VOS_UINT16                          ausPriWeightedNum[ADS_UL_QUEUE_SCHEDULER_PRI_MAX];  /* ���ж������ȼ��ļ�Ȩ�� */
    VOS_UINT8                           aucRsv[2];
}ADS_UL_QUEUE_SCHEDULER_PRI_NV_STRU;

/*****************************************************************************
 �ṹ��    : AT_WIFI_INFO_STRU (9040 ������ʵ��ʹ�õĶ���Ľṹ����)
 �ṹ˵��  : en_NV_Item_WIFI_INFO(9040)   ���ڴ�NV���ȡWIFI�����Ϣ
 �޸ļ�¼  :
  1.��    ��   : 2012��02��02��
    ��    ��   : l00198894
    �޸�����   : B050 Project, �����ṹ
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          bitOpbSupport       : 1;
    VOS_UINT32                          bitOpgSupport       : 1;
    VOS_UINT32                          bitOpnSupport       : 1;
    VOS_UINT32                          bitOpSpare          : 29;
    VOS_UINT8                           usbHighChannel;                         /* 802.11b��ʽ����ŵ��� */
    VOS_UINT8                           usbLowChannel;                          /* 802.11b��ʽ��С�ŵ��� */
    VOS_UINT8                           aucRsv1[2];
    VOS_UINT16                          ausbPower[2];                           /* 802.11b��ʽ���������� */
    VOS_UINT8                           usgHighChannel;                         /* 802.11g��ʽ����ŵ��� */
    VOS_UINT8                           usgLowChannel;                          /* 802.11g��ʽ��С�ŵ��� */
    VOS_UINT8                           aucRsv2[2];
    VOS_UINT16                          ausgPower[2];                           /* 802.11g��ʽ���������� */
    VOS_UINT8                           usnHighChannel;                         /* 802.11n��ʽ����ŵ��� */
    VOS_UINT8                           usnLowChannel;                          /* 802.11n��ʽ��С�ŵ��� */
    VOS_UINT8                           aucRsv3[2];
    VOS_UINT16                          ausnPower[2];                           /* 802.11n��ʽ���������� */
}AT_WIFI_INFO_STRU;

/*****************************************************************************
 �ṹ��    : nv_wifi_info (9040 XML�ж���Ľṹ����)
 �ṹ˵��  : en_NV_Item_WIFI_INFO(9040)   nv_wifi_info�ṹ
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulOpSupport;
    VOS_UINT8                           usbHighChannel;
    VOS_UINT8                           usbLowChannel;
    VOS_UINT8                           aucRsv1[2];
    VOS_UINT16                          ausbPower[2];
    VOS_UINT8                           usgHighsChannel;
    VOS_UINT8                           usgLowChannel;
    VOS_UINT8                           aucRsv2[2];
    VOS_UINT16                          ausgPower[2];
    VOS_UINT8                           usnHighsChannel;
    VOS_UINT8                           usnLowChannel;
    VOS_UINT8                           ausnRsv3[2];
    VOS_UINT16                          ausnPower[2];
}nv_wifi_info;

/*****************************************************************************
 �ṹ��    : NAS_PREVENT_TEST_IMSI_REG_STRU
 �ṹ˵��  : en_NV_Item_SubPlatFormInfo(9041) �ṹ
  1.��    ��   : 2013��5��22��
    ��    ��   : l00167671
    �޸�����   : ��ֲ
  2.��    ��   : 2015��3��2��
    ��    ��   : w00316404
    �޸�����   : ���ֽڶ��룬����reserveλ
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          usPlatform;
    VOS_UINT8                           ucReserve1;
    VOS_UINT8                           ucReserve2;
}NAS_NVIM_PLATFORM_STRU;

/*****************************************************************************
 �ṹ��    : TAF_AT_EQ_VER_STRU
 �ṹ˵��  : en_NV_Item_Eqver(9070) �ṹ
  1.��    ��   : 2013��5��22��
    ��    ��   : l00167671
    �޸�����   : ��ֲ
  2.��    ��   : 2015��3��2��
    ��    ��   : w00316404
    �޸�����   : ���ֽڶ��룬����reserveλ
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          usEqver;
    VOS_UINT8                           ucReserve1;
    VOS_UINT8                           ucReserve2;
}TAF_AT_EQ_VER_STRU;

/*****************************************************************************
 �ṹ��    : TAF_NVIM_CS_VER_STRU
 �ṹ˵��  : en_NV_Item_Csver(9071) �ṹ
  1.��    ��   : 2013��5��22��
    ��    ��   : l00167671
    �޸�����   : ��ֲ
  2.��    ��   : 2015��3��2��
    ��    ��   : w00316404
    �޸�����   : ���ֽڶ��룬����reserveλ
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          usCsver;
    VOS_UINT8                           ucReserve1;
    VOS_UINT8                           ucReserve2;
}TAF_NVIM_CS_VER_STRU;

/*****************************************************************************
 �ṹ��    : TAF_AT_NVIM_MUX_SUPPORT_FLG_STRU
 �ṹ˵��  : en_NV_Item_Mux_Support_Flg(9100) �ṹ
  1.��    ��   : 2013��5��22��
    ��    ��   : l00167671
    �޸�����   : ��ֲ
  2.��    ��   : 2015��3��2��
    ��    ��   : w00316404
    �޸�����   : ���ֽڶ��룬����reserveλ
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucMuxSupportFlg;
    VOS_UINT8                           ucReserved1;
    VOS_UINT8                           ucReserved2;
    VOS_UINT8                           ucReserved3;
}TAF_AT_NVIM_MUX_SUPPORT_FLG_STRU;

/*****************************************************************************
 �ṹ��    : TAF_AT_MULTI_WIFI_SEC_STRU
 Э����  :
 ASN.1���� :
 �ṹ˵��  : en_NV_Item_MULTI_WIFI_KEY(9110) NV���Ӧ�Ľṹ��
             ���ڱ���wifi ��ȫ��Ϣ
             nv_wifisec_type�ṹ�ĸ���������NV��50012�����ݽṹnv_wifisec_type����һ��
             aucwlAuthMode              ��Ȩģʽ
             aucBasicEncryptionModes    ��������ģʽ
             aucWPAEncryptionModes      WPA����ģʽ
             aucWifiWepKey1...aucWifiWepKey4               WIFI KEY
             ucWifiWepKeyIndex          ʹ�õ�WIFI KEY index,���磬1����ʹ��wlKeys1
             aucWpaPsk                  WPA������
             ucWpsEnable                wps�Ƿ�ʹ�ܿ���
             ucWpsCfg                   �Ƿ�����register���ı�enrollee�Ĳ���,  0:������(Ĭ��);  1:����

  1.��    ��   : 2012��9��15��
    ��    ��   : z60575
    �޸�����   : �����ṹ
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           aucWifiAuthmode[AT_WIFI_WLAUTHMODE_LEN];
    VOS_UINT8                           aucWifiBasicencryptionmodes[AT_WIFI_ENCRYPTIONMODES_LEN];
    VOS_UINT8                           aucWifiWpaencryptionmodes[AT_WIFI_ENCRYPTIONMODES_LEN];
    VOS_UINT8                           aucWifiWepKey1[AT_WIFI_MAX_SSID_NUM][AT_WIFI_KEY_LEN_MAX];
    VOS_UINT8                           aucWifiWepKey2[AT_WIFI_MAX_SSID_NUM][AT_WIFI_KEY_LEN_MAX];
    VOS_UINT8                           aucWifiWepKey3[AT_WIFI_MAX_SSID_NUM][AT_WIFI_KEY_LEN_MAX];
    VOS_UINT8                           aucWifiWepKey4[AT_WIFI_MAX_SSID_NUM][AT_WIFI_KEY_LEN_MAX];
    VOS_UINT8                           ucWifiWepKeyIndex[AT_WIFI_MAX_SSID_NUM];
    VOS_UINT8                           aucWifiWpapsk[AT_WIFI_MAX_SSID_NUM][AT_WIFI_WLWPAPSK_LEN];
    VOS_UINT8                           ucWifiWpsenbl;
    VOS_UINT8                           ucWifiWpscfg;
}TAF_AT_MULTI_WIFI_SEC_STRU;

/*****************************************************************************
 �ṹ��    : TAF_TAF_AT_MULTI_WIFI_SSID_STRU
 Э����  :
 ASN.1���� :
 �ṹ˵��  : en_NV_Item_MULTI_WIFI_STATUS_SSID(9111) NV���Ӧ�Ľṹ��
             ���ڱ���wifi SSID����NV�ṹ���£����ڳ���SSID����Ĳ��ֲ�����
             �κβ�����Ϊ�˱�������PADDING���ڶ���ʱֱ�Ӷ���ΪUINT8���͵�����
             aucwlSsid[AT_WIFI_MAX_SSID_NUM][AT_WIFI_SSID_LEN_MAX];
             ulwlChannel;
             ulwlHide;
             aucwlCountry[NV_WLCOUNTRY_CODE_LEN];
             aucwlMode[NV_WLMODE_LEN];
             ulwlRate;
             ulwlTxPwrPcnt;
             ulwlMaxAssoc;
             ucwlEnbl;
             ulwlFrgThrshld;
             ulwlRtsThrshld;
             ulwlDtmIntvl;
             ulwlBcnIntvl;
             ulwlWme;
             ulwlPamode;
             ulwlIsolate;
             ulwlProtectionmode;
             ulwloffenable;
             ucwlofftime;
             aucwlExtends[12];
             ucReserve;

  1.��    ��   : 2012��9��15��
    ��    ��   : z60575
    �޸�����   : �����ṹ
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           aucWifiSsid[AT_WIFI_MAX_SSID_NUM][AT_WIFI_SSID_LEN_MAX];
    VOS_UINT8                           aucReserved[84];
}TAF_AT_MULTI_WIFI_SSID_STRU;

/*****************************************************************************
 �ṹ��    : AT_NVIM_ABORT_CMD_PARA_STRU
 �ṹ˵��  : en_NV_Item_AT_ABORT_CMD_PARA(9113)
             ��������AT���������������ͷ��ؽ���Ľṹ, NV��ʹ��
             �������û�����"AT"��ʾ����������, ����"ABORT"��ʾ������. �����ִ�Сд

  1.��    ��   : 2012��09��21��
    ��    ��   : L00171473
    �޸�����   : for V7R1C50_At_Abort,  ����
  2.��    ��   : 2015��3��2��
    ��    ��   : w00316404
    �޸�����   : ���ֽڶ��룬����reserveλ
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucAbortEnableFlg;                           /* AT��Ͽ��ر�־ */
    VOS_UINT8                           ucReserve1;
    VOS_UINT8                           ucReserve2;
    VOS_UINT8                           ucReserve3;
    VOS_UINT8                           aucAbortAtCmdStr[AT_MAX_ABORT_CMD_STR_LEN]; /* ���AT������ */
    VOS_UINT8                           aucAbortAtRspStr[AT_MAX_ABORT_RSP_STR_LEN]; /* �������ķ��ؽ�� */
}AT_NVIM_ABORT_CMD_PARA_STRU;

/*****************************************************************************
 �ṹ��    : TAF_AT_NVIM_MUX_REPORT_CFG_STRU
 �ṹ˵��  : en_NV_Item_MUX_REPORT_CFG(9125) �ṹ
  1.��    ��   : 2013��5��22��
    ��    ��   : l00167671
    �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulMuxReportCfg;
}TAF_AT_NVIM_MUX_REPORT_CFG_STRU;

/*****************************************************************************
 �ṹ��    : TAF_AT_NVIM_CIMI_PORT_CFG_STRU
 �ṹ˵��  : en_NV_Item_CIMI_PORT_CFG(9128) �ṹ
  1.��    ��   : 2013��5��22��
    ��    ��   : l00167671
    �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulCimiPortCfg;
}TAF_AT_NVIM_CIMI_PORT_CFG_STRU;

/*****************************************************************************
 �ṹ��    : AT_SS_CUSTOMIZE_PARA_STRU
 �ṹ˵��  : en_NV_Item_SS_CUSTOMIZE_PARA(9132)
             SS���Ʋ���������
             NV����־ucStatus��0: �����1: ����
             SS��׼����Ʋ���
             BIT 0 ����+CCWA��ѯ�����Ƿ��������ҵ��״̬��0������� ��1: ���
             BIT 1 ����+CLCK��ѯ�����Ƿ��������ҵ��״̬��0������� ��1: ���
  1.��    ��  : 2013��01��24��
    ��    ��  : f62575
    �޸�����  : SS�������������ṹ
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucStatus;                               /* NV�Ƿ񼤻��־,  */
    VOS_UINT8                           ucSsCmdCustomize;
    VOS_UINT8                           aucReserved1[2];
} AT_SS_CUSTOMIZE_PARA_STRU;

/*****************************************************************************
 �ṹ��    : TAF_NVIM_SHARE_PDP_INFO_STRU
 �ṹ˵��  : en_NV_Item_SHARE_PDP_INFO(9218)  Share-PDP���Կ���NV
  1.��    ��   : 2013��06��03��
    ��    ��   : L47619
    �޸�����   : V3R3 Share-PDP��Ŀ����
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucEnableFlag;                           /* �Ƿ�ʹ��Share PDP����, 0 - ��ֹ;  1 - ʹ�� */
    VOS_UINT8                           ucReserved;                             /* ���� */
    VOS_UINT16                          usAgingTimeLen;                         /* �ϻ�ʱ�䣬��λΪ�� */
} TAF_NVIM_SHARE_PDP_INFO_STRU;

/*****************************************************************************
 �ṹ��    : ADS_UL_WATER_MARK_LEVEL_NV_STRU
 �ṹ˵��  : ADS ULˮ�߽��ṹ
   1.��    ��   : 2014��06��03��
     ��    ��   : f00179208
     �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulWaterLevel1;                          /* ˮ�߽��1 */
    VOS_UINT32                          ulWaterLevel2;                          /* ˮ�߽��2 */
    VOS_UINT32                          ulWaterLevel3;                          /* ˮ�߽��3 */
    VOS_UINT32                          ulWaterLevel4;                          /* ˮ�߽��4,Ԥ�� */
} ADS_UL_WATER_MARK_LEVEL_STRU;

/*****************************************************************************
 �ṹ��    : ADS_UL_WATER_MARK_LEVEL_NV_STRU
 �ṹ˵��  : ADS UL�ް����޽ṹ
   1.��    ��   : 2014��06��03��
     ��    ��   : f00179208
     �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulThreshold1;                           /* �ް�����1 */
    VOS_UINT32                          ulThreshold2;                           /* �ް�����2 */
    VOS_UINT32                          ulThreshold3;                           /* �ް�����3 */
    VOS_UINT32                          ulThreshold4;                           /* �ް�����4 */
} ADS_UL_THRESHOLD_LEVEL_STRU;

/*****************************************************************************
 �ṹ��    : ADS_UL_WATER_MARK_LEVEL_NV_STRU
 �ṹ˵��  : en_NV_Item_ADS_DYNAMIC_THRESHOLD_CFG(9268) ADS UL��̬�ް��ṹ
   1.��    ��   : 2014��06��03��
     ��    ��   : f00179208
     �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulActiveFlag;                           /* ʹ�ܱ�ʶ: 0��ʾȥʹ��,1��ʾʹ�� */
    VOS_UINT32                          ulProtectTmrExpCnt;                     /* ������ʱ����ʱ����ʱ�� */
    ADS_UL_WATER_MARK_LEVEL_STRU        stWaterMarkLevel;
    ADS_UL_THRESHOLD_LEVEL_STRU         stThresholdLevel;
    VOS_UINT32                          aulReserved[6];
} ADS_NV_DYNAMIC_THRESHOLD_STRU;

/*******************************************************************************
 �ṹ��    : TAF_NV_VCOM_DEBUG_CFG_STRU
 �ṹ˵��  : en_NV_Item_DMS_DEBUG_CFG(9269) VCOM DEBUG�������NV��
  1.��    ��   : 2014��06��03��
    ��    ��   : n00269697
    �޸�����   : VCOM DEBUG����
*******************************************************************************/
typedef struct
{
    VOS_UINT32                          ulDebugLevel;                           /* VCOM DEBUG����:ERR,NORMAL,INFO,DEBUG */
    VOS_UINT32                          ulAppVcomPortIdMask;                    /* VCOM�˿�ID���� */
    VOS_UINT32                          ulReserved[4];                          /* Ԥ�� */
} TAF_NV_PORT_DEBUG_CFG_STRU;

/*****************************************************************************
�ṹ��    : OM_MAC_ADDR_STRU
�ṹ˵��  :
��ӦNVID: 50014 en_NV_Item_WIFI_MAC_ADDR

  1.��    ��   : 2013��5��16��
    ��    ��   : j00174725
    �޸�����   : ��ʼ����
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           aucMacAddr[32];
}OM_MAC_ADDR_STRU;

/*****************************************************************************
 �ṹ��    : MMA_CUSTOM_CARDLOCK_OPERATOR_STRU
 �ṹ˵��  : en_NV_Item_PRODUCT_ID(50048) �ṹ
  1.��    ��   : 2013��5��22��
    ��    ��   : l00167671
    �޸�����   : �½�
  2.��    ��   : 2015��3��2��
    ��    ��   : w00316404
    �޸�����   : ���ֽڶ��룬����reserveλ
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulNvStatus;
    VOS_UINT8                           aucProductId[AT_PRODUCT_NAME_LENGHT];   /* product id */
    VOS_UINT8                           ucReserve1;
    VOS_UINT8                           ucReserve2;
}TAF_AT_PRODUCT_ID_STRU;

/*****************************************************************************
 �ṹ��    : NAS_NV_PPP_DIAL_ERR_CODE_STRU
 �ṹ˵��  : en_NV_Item_PPP_DIAL_ERR_CODE(50061)�ṹ
  1.��    ��   : 2013��5��22��
    ��    ��   : m00217266
    �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucStatus;                               /* 1: NV��Ч��־λ��0����Ч */
    VOS_UINT8                           ucErrCodeRpt;                           /*�������ϱ���־1: 0�����ϱ�,  1���ϱ�*/
}NAS_NV_PPP_DIAL_ERR_CODE_STRU;  /* ����TIM���Ŵ�����NV��ṹ�� */

/*****************************************************************************
 �ṹ��    : AT_DYNAMIC_PID_TYPE_STRU
 �ṹ˵��  : en_NV_Item_Huawei_Dynamic_PID_Type(50091)�ṹ
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulNvStatus;
    VOS_UINT8                           aucFirstPortStyle[17];                  /* �豸�л�ǰ�˿���̬ */
    VOS_UINT8                           aucRewindPortStyle[17];                 /* �豸�л���˿���̬ */
    VOS_UINT8                           aucReserved[22];                        /* ���� */
}AT_DYNAMIC_PID_TYPE_STRU;

/*****************************************************************************
 �ṹ��    : TAF_AT_NVIM_WEB_ADMIN_PASSWORD_STRU
 �ṹ˵��  : en_NV_Item_WEB_ADMIN_PASSWORD(52001)�ṹ
  1.��    ��   : 2013��5��22��
    ��    ��   : l00167671
    �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           aucWebPwd[AT_WEBUI_PWD_MAX];
}TAF_AT_NVIM_WEB_ADMIN_PASSWORD_STRU;

/*****************************************************************************
 �ṹ��    : TAF_AT_NVIM_AP_RPT_SRV_URL_STRU
 �ṹ˵��  : en_NV_Item_AP_RPT_SRV_URL(52002) �ṹ
  1.��    ��   : 2013��5��22��
    ��    ��   : l00167671
    �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           aucApRptSrvUrl[AT_AP_XML_RPT_SRV_URL_STR_LEN];
}TAF_AT_NVIM_AP_RPT_SRV_URL_STRU;

/*****************************************************************************
 �ṹ��    : TAF_AT_NVIM_AP_XML_INFO_TYPE_STRU
 �ṹ˵��  : en_NV_Item_AP_XML_INFO_TYPE(52003) �ṹ
  1.��    ��   : 2013��5��22��
    ��    ��   : l00167671
    �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           aucApXmlInfoType[AT_AP_NVIM_XML_RPT_INFO_TYPE_STR_LEN];
}TAF_AT_NVIM_AP_XML_INFO_TYPE_STRU;

/*****************************************************************************
 �ṹ��    : TAF_AT_NVIM_AP_XML_RPT_FLG_STRU
 �ṹ˵��  : en_NV_Item_AP_XML_RPT_FLAG(52004) �ṹ
  1.��    ��   : 2013��5��22��
    ��    ��   : l00167671
    �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucApXmlRptFlg;
    VOS_UINT8                           ucReserve[3];
}TAF_AT_NVIM_AP_XML_RPT_FLG_STRU;

/*****************************************************************************
 �ṹ��    : TAF_NV_IMS_SWITCH_RAT_RELATION_CFG_STRU
 �ṹ˵��  : en_NV_Item_IMS_SWITCH_RAT_RELATION_CFG(2478) �ṹ
  1.��    ��   : 2017��8��8��
    ��    ��   : l00198894
    �޸�����   : �½�
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucUtranRelationLteFlg;      /* ���Ƶ�^IMSSWITCH����δЯ��<utran_enable>����ʱ���˲���ֵ�Ƿ���<lte_enable>���� */
    VOS_UINT8                           ucGsmRelationLteFlg;        /* ���Ƶ�^IMSSWITCH����δЯ��<gsm_enable>����ʱ���˲���ֵ�Ƿ���<lte_enable>���� */
    VOS_UINT8                           ucReserved1;
    VOS_UINT8                           ucReserved2;
}TAF_NV_IMS_SWITCH_RAT_RELATION_CFG_STRU;


#pragma pack(pop)

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* __ACORE_NV_STRU_GUCNAS_H__ */


