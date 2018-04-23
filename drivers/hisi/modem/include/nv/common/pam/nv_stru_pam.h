/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��Ϊ�������޹�˾

 ******************************************************************************
  �� �� ��      : nv_stru_pam.h
  �� �� ��      : ����
  ��    ��      : ��͢��
  ��������      : 2017��4��20��
  ����޸�      :
  ��������      : ��ͷ�ļ�ΪOM��NV�ṹͷ�ļ�����Ҫ��������NV bin�ļ�
  �����б�      :
  �޸���ʷ      :
  1.��    ��    : 2017��4��20��
    ��    ��    : d00212987
    �޸�����    : �����ļ�
******************************************************************************/
#ifndef __NV_STRU_PAM_H__
#define __NV_STRU_PAM_H__

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "vos.h"
#include "nv_id_pam.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#if (VOS_OS_VER != VOS_WIN32)
#pragma pack(4)
#else
#pragma pack(push, 4)
#endif
/*****************************************************************************
  2 �궨��
*****************************************************************************/
#define ESN_NV_DATA_LEN                             (8)
#define MEID_NV_DATA_LEN                            (8)

/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/

/*****************************************************************************
 ö����    : AT_PHY_PORT_ENUM
 ö��˵��  : ����˿ں�ö��ֵ
*****************************************************************************/
enum AT_PHY_PORT_ENUM
{
    AT_UART_PORT = 0,
    AT_PCUI_PORT,
    AT_CTRL_PORT,
    AT_HSUART_PORT,
    AT_PCUI2_PORT,
    AT_PORT_BUTT
};
typedef VOS_UINT32  AT_PHY_PORT_ENUM_UINT32;

enum
{
    CPM_IND_PORT = AT_PORT_BUTT,    /* OM�����ϱ��˿� */
    CPM_CFG_PORT,                   /* OM���ö˿� */
    CPM_SD_PORT,
    CPM_WIFI_OM_IND_PORT,           /* WIFI��OM�����ϱ��˿� */
    CPM_WIFI_OM_CFG_PORT,           /* WIFI��OM�����·��˿� */
    CPM_WIFI_AT_PORT,               /* WIFI��AT�˿� */
    CPM_HSIC_IND_PORT,
    CPM_HSIC_CFG_PORT,
    CPM_VCOM_IND_PORT,              /* VCOM��OM�����ϱ��ӿ� */
    CPM_VCOM_CFG_PORT,              /* VCOM��OM���ýӿ� */
    CPM_FS_PORT,
    CPM_PORT_BUTT
};
typedef VOS_UINT32  CPM_PHY_PORT_ENUM_UINT32;

/*****************************************************************************
�ṹ��    : CPM_CBT_PORT_ENUM_UINT32
�ṹ˵��  : CBT������˿ڳ���
��ӦNVID: 45 en_NV_Item_Om_Port_Type

  1.��    ��   : 2014��6��3��
    ��    ��   : h59254
    �޸�����   : ��ʼ����
*****************************************************************************/
enum
{
    CPM_CBT_PORT_USB,
    CPM_CBT_PORT_VCOM,
    CPM_CBT_PORT_SOCKET,
    CBP_CBT_PORT_BUTT
};
typedef VOS_UINT32 CPM_CBT_PORT_ENUM_UINT32;

enum
{
    CPM_OM_PORT_TYPE_USB,
    CPM_OM_PORT_TYPE_VCOM,
    CPM_OM_PORT_TYPE_WIFI,
    CPM_OM_PORT_TYPE_SD,
    CPM_OM_PORT_TYPE_FS,
    CPM_OM_PORT_TYPE_HSIC,
    CBP_OM_PORT_TYPE_BUTT
};
typedef VOS_UINT32 CPM_OM_PORT_ENUM_UINT32;

/*****************************************************************************
 ö����    : ESN_MEID_FLAG_ENUM
 ö��˵��  : �¼������ط����

  1.��    ��   : 2014��07��3��
    ��    ��   : zhuli
    �޸�����   : V9R1 C+L ��Ŀ����
*****************************************************************************/
enum ESN_MEID_FLAG_ENUM
{
    ESN_MEID_DISABLE        = 0,
    ESN_ME_ENABLE_FLAG      = 1,
    MEID_ME_ENABLE_FLAG     = 2,
    ESN_MEID_ENABLE_BOTH    = 3,
    ESN_MEID_ENABLE_FLAG_BUTT
};
typedef VOS_UINT8 ESN_MEID_FLAG_ENUM_UINT8;

/*****************************************************************************
  4 ȫ�ֱ�������
*****************************************************************************/

/*****************************************************************************
  5 ��Ϣͷ����
*****************************************************************************/

/*****************************************************************************
  6 ��Ϣ����
*****************************************************************************/


/*****************************************************************************
  7 STRUCT����
*****************************************************************************/
/*****************************************************************************
�ṹ��    : OM_MONITOR_PARA_CTRL_STRU
�ṹ˵��  :
��ӦNVID: 40 MonitorParaCtrl

  1.��    ��   : 2013��5��16��
    ��    ��   : j00174725
    �޸�����   : ��ʼ����
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucTempOverCount;     /*������ģʽ���¶����������澯���޻�͹������޵ļ��޴���*/
    VOS_UINT8                           ucTempRsumeCount;    /*�ڵ͹���ģʽ��澯ģʽ���¶��������ڸ澯�ŵļ��޴���*/
    VOS_UINT8                           ucTempCloseCount;    /*�ڽ���ģʽ���¶����������澯���޵ļ��޴���*/
    VOS_UINT8                           ucTempPowerOffCount; /*�����µ����޵ļ��޴���*/
    VOS_UINT32                          ulWaitTimer;         /*�ȴ���ʱ��ʱ��*/
}TEMP_PARA_CTRL_STRU;

/*****************************************************************************
�ṹ��    : OM_USIM_TEMP_PROTECT_NEW_STRU
�ṹ˵��  :
��ӦNVID: 40 UsimTempProtect_New

  1.��    ��   : 2013��5��16��
    ��    ��   : j00174725
    �޸�����   : ��ʼ����
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulIsEnable;           /*�Ƿ���Ҫ������������, 0Ϊ�رգ�1Ϊ����*/
    VOS_UINT32                          ulLocation;           /*ʹ��������¶���Ϊ�±������룬�ò���ֱ����Ϊ��ν������ĺ���,Ŀǰ��ȡֵ��Χ��0��6 */
    VOS_INT32                           lPowerOffThreshold;   /*�����µ�ģʽ������*/
    VOS_INT32                           lCloseAdcThreshold;   /*����͹���ģʽ���¶�����*/
    VOS_INT32                           lAlarmAdcThreshold;   /*��Ҫ���и����������¶�����*/
    VOS_INT32                           lResumeAdcThreshold;  /*�ָ�������ģʽ���¶�����*/
    VOS_INT32                           lSIMPowerOffThreshold;  /*E5��̬��SIM���±�������*/
    TEMP_PARA_CTRL_STRU                 stTempCtrlPara;
}SPY_TEMP_PROTECT_NV_STRU;

/*****************************************************************************
�ṹ��    : OM_PORT_CFG_STRU
�ṹ˵��  :
��ӦNVID: 45 NV_OM_Port_Type

  1.��    ��   : 2013��5��16��
    ��    ��   : j00174725
    �޸�����   : ��ʼ����
*****************************************************************************/
typedef struct
{
    CPM_OM_PORT_ENUM_UINT32             enPortNum;         /* ����ͨ������ */
    CPM_CBT_PORT_ENUM_UINT32            enCbtPortNum;      /* У׼ͨ������ */
}OM_CHANNLE_PORT_CFG_STRU;

/*****************************************************************************
 ö����    : USIMM_NV_CARDAPP_ENUM
 ö��˵��  : USIMM������ö��

  1.��    ��   : 2015��03��23��
    ��    ��   : ganlan
    �޸�����   : V8R2 UICC MutliApp PhaseI��������
*****************************************************************************/
enum USIMM_NV_CARDAPP_ENUM
{
    USIMM_NV_GUTL_APP          = 0x00000000,
    USIMM_NV_IMS_APP           = 0x00000001,
    USIMM_NV_CDMA_APP          = 0x00000002,
    USIMM_NV_CARDAPP_BUTT      = 0x00000003
};
typedef VOS_UINT32 USIMM_NV_CARDAPP_ENUM_UINT32;

/*****************************************************************************
 �ṹ��    : USIMM_APP_PRIORITY_CONFIG_STRU
 �ṹ˵��  : USIMMӦ�ó�ʼ��˳������

 ��Ӧnvid  : 4001

  1.��    ��   : 2015��03��23��
    ��    ��   : ganlan
    �޸�����   : V8R2 UICC MutliApp PhaseI��������
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucAppNum;
    VOS_UINT8                           aucRsv[3];
    USIMM_NV_CARDAPP_ENUM_UINT32        aenAppList[5];
}USIMM_APP_PRIORITY_CONFIG_STRU;

/*****************************************************************************
 �ṹ��    : NV_SC_PERS_CTRL_STRU
 �ṹ˵��  : SC �����������ƿ������

 ��Ӧnv id 4008

  1.��    ��   : 2015��11��30��
    ��    ��   : d00212987
    �޸�����   : SC���Կ�������
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          usImei0ReplaseImei2;
    VOS_UINT16                          usRsv;
}NV_SC_PERS_CTRL_STRU;

/*****************************************************************************
�ṹ��    : USIMM_CMD_DEBUG_MODE_ST
�ṹ˵��  :
��ӦNVID: 4043 UsimDebugMode

  1.��    ��   : 2013��5��16��
    ��    ��   : j00174725
    �޸�����   : ��ʼ����
*****************************************************************************/
typedef struct
{
    VOS_UINT16                          usFlag;
    VOS_UINT16                          usTimerValue;
}USIMM_CMD_DEBUG_MODE_ST;

/*****************************************************************************
 �ṹ��    : NV_ESN_MEID_STRU
 �ṹ˵��  : NV_ESN_MEID_STRU 9283
 ��Ӧnv id en_NV_Item_ESN_MEID = 9283

  1.��    ��   : 2014��07��3��
    ��    ��   : zhuli
    �޸�����   : X��Ŀ����
*****************************************************************************/
typedef struct
{
    ESN_MEID_FLAG_ENUM_UINT8        enEsnMeIDFlag;
    VOS_UINT8                       ucRSv;
    VOS_UINT8                       aucEsn[ESN_NV_DATA_LEN];  /*Len+Data*/
    VOS_UINT8                       aucMeID[MEID_NV_DATA_LEN]; /*Len+Data*/
    VOS_UINT8                       aucRfu[6];
}NV_ESN_MEID_STRU;

/*****************************************************************************
�ṹ��    : SI_PB_CTRL_INFO_ST
�ṹ˵��  :
��ӦNVID: 8244 UsimPBCtrlInfo

  1.��    ��   : 2013��5��16��
    ��    ��   : j00174725
    �޸�����   : ��ʼ����
*****************************************************************************/
typedef VOS_UINT32 SI_PB_STORATE_TYPE;
typedef struct
{
    SI_PB_STORATE_TYPE                  enCurMaterial;
    VOS_UINT8                           aucPasswords[8];
    VOS_UINT16                          usUsed;
    VOS_UINT16                          usTotal;
    VOS_UINT8                           ucAnrMaxNum;        /*Balong֧�ֵ�ANR�����ֵ*/
    VOS_UINT8                           ucEmailFlag;        /*Balong֧��Email���*/
    VOS_UINT8                           ucSPBFlag;         /*���ϵ绰��֧�ֱ�ǣ�Ϊ1ʱ��ʾ֧��*/
    VOS_UINT8                           ucRsv;
}SI_PB_CTRL_INFO_ST;

/*****************************************************************************
�ṹ��    : USIMM_STK_CFG_STRU
�ṹ˵��  :
��ӦNVID: 8261 TerminalProfile_Set

  1.��    ��   : 2013��5��16��
    ��    ��   : j00174725
    �޸�����   : ��ʼ����
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucProfileLen;
    VOS_UINT8                           aucProfile[47];
}USIMM_USIM_PROFILE_STRU;


typedef struct
{
    VOS_UINT8                           ucProfileLen;
    VOS_UINT8                           aucProfile[31];
}USIMM_SIM_PROFILE_STRU;

typedef struct
{
    VOS_UINT8                           ucFuncEnable;
    VOS_UINT8                           ucTimer;
    VOS_UINT8                           ucTerminalType;
    VOS_UINT8                           ucRsv;
    USIMM_USIM_PROFILE_STRU             stUsimProfile;
    USIMM_SIM_PROFILE_STRU              stSimProfile;
}USIMM_STK_CFG_STRU;

/*****************************************************************************
  8 UNION����
*****************************************************************************/


/*****************************************************************************
  9 OTHERS����
*****************************************************************************/

#if (VOS_OS_VER != VOS_WIN32)
#pragma pack()
#else
#pragma pack(pop)
#endif



#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif
