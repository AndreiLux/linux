/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��Ϊ�������޹�˾

 ******************************************************************************
  �� �� ��   : Hi6210-pcm.h
  �� �� ��   : ����
  ��    ��   : ʯ���� s00212991
  ��������   : 2012��7��31��
  ����޸�   :
  ��������   : Hi6210-pcm.h ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2012��7��31��
    ��    ��   : ʯ���� s00212991
    �޸�����   : �����ļ�

******************************************************************************/

#ifndef HI6210_PCM_H
#define HI6210_PCM_H


/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
#include "hifi_lpp.h"


/*****************************************************************************
  2 �궨��
*****************************************************************************/
#define HI_CHN_COMMON   \
	unsigned short  msg_type;   \
	unsigned short  pcm_device;   \
	unsigned short  pcm_mode;   \
	unsigned short  reserved;


#define INT_TO_ADDR(low,high) (void*) (unsigned long)((unsigned long long)(low) | ((unsigned long long)(high)<<32))
#define GET_LOW32(x) (unsigned int)(((unsigned long long)(unsigned long)(x))&0xffffffffULL)
#define GET_HIG32(x) (unsigned int)((((unsigned long long)(unsigned long)(x))>>32)&0xffffffffULL)
/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/

/* HI6210 PCM����״̬ */
enum HI6210_STATUS
{
	STATUS_STOP = 0,
	STATUS_RUNNING = 1,
	STATUS_STOPPING = 2
};

enum HI6210_PCM_FORMATS
{
	PCM_FORMAT_S16_LE = 0,
	PCM_FORMAT_S24_LE_LA,       /* left alignment */
	PCM_FORMAT_S24_LE_RA,       /* right alignment */
	PCM_FORMAT_S32_LE,
	PCM_FORMAT_MAX,
};

/* AP��HIFI��Ƶ���ݴ�����Ϣ���� */
enum AUDIO_MSG_ENUM
{
	/* AP DSP������HIFI������ͨ��ģ�齻����ϢID */
	ID_AP_AUDIO_PLAY_DECODE_REQ         = 0xDD33,
	ID_AUDIO_AP_PLAY_DECODE_RSP         = 0xDD34,

	ID_AP_AUDIO_RECORD_START_REQ        = 0xDD40,
	ID_AP_AUDIO_RECORD_STOP_REQ         = 0xDD41,

	/* AP��Ƶ������HIFI��Ƶͨ��ģ�齻����ϢID */
	ID_AP_AUDIO_PCM_OPEN_REQ            = 0xDD25,
	ID_AP_AUDIO_PCM_CLOSE_REQ           = 0xDD26,
	ID_AP_AUDIO_PCM_HW_PARA_REQ         = 0xDD27,
	ID_AP_AUDIO_PCM_HW_FREE_REQ         = 0xDD28,   /* ��δʹ�ã����� */
	ID_AP_AUDIO_PCM_PREPARE_REQ         = 0xDD29,   /* ��δʹ�ã����� */
	ID_AP_AUDIO_PCM_TRIGGER_REQ         = 0xDD2A,
	ID_AP_AUDIO_PCM_POINTER_REQ         = 0xDD2B,   /* ��δʹ�ã����� */
	ID_AP_AUDIO_PCM_SET_BUF_CMD         = 0xDD2C,
	ID_AUDIO_AP_PCM_PERIOD_ELAPSED_CMD  = 0xDD2D,

	/* HIFI��Ƶͨ��ģ���ڲ�������ϢID */
	ID_AUDIO_UPDATE_PLAY_BUFF_CMD       = 0xDD2E,
	ID_AUDIO_UPDATE_CAPTURE_BUFF_CMD    = 0xDD2F,
	ID_AUDIO_AP_PCM_TRIGGER_CNF         = 0xDDA0,
	ID_AUDIO_AP_PCM_XRUN = 0xDD2E,
};
typedef unsigned short  AUDIO_MSG_ENUM_UINT16;

enum HIFI_CHN_MSG_TYPE
{
	HI_CHN_MSG_PCM_OPEN             = ID_AP_AUDIO_PCM_OPEN_REQ,
	HI_CHN_MSG_PCM_CLOSE            = ID_AP_AUDIO_PCM_CLOSE_REQ,
	HI_CHN_MSG_PCM_HW_PARAMS        = ID_AP_AUDIO_PCM_HW_PARA_REQ,
	HI_CHN_MSG_PCM_HW_FREE          = ID_AP_AUDIO_PCM_HW_FREE_REQ,
	HI_CHN_MSG_PCM_PREPARE          = ID_AP_AUDIO_PCM_PREPARE_REQ,
	HI_CHN_MSG_PCM_TRIGGER          = ID_AP_AUDIO_PCM_TRIGGER_REQ,
	HI_CHN_MSG_PCM_POINTER          = ID_AP_AUDIO_PCM_POINTER_REQ,
	/* ���ݰ��� */
	HI_CHN_MSG_PCM_SET_BUF          = ID_AP_AUDIO_PCM_SET_BUF_CMD,
	/* HIFI���ݰ������*/
	HI_CHN_MSG_PCM_PERIOD_ELAPSED   = ID_AUDIO_AP_PCM_PERIOD_ELAPSED_CMD,
	HI_CHN_MSG_PCM_PERIOD_STOP      = ID_AUDIO_AP_PCM_TRIGGER_CNF,
	HI_CHN_MSG_PCM_XRUN      = ID_AUDIO_AP_PCM_XRUN,
};

enum IRQ_RT
{
	/* IRQ Not Handled as Other problem */
	IRQ_NH_OTHERS    = -5,
	/* IRQ Not Handled as Mailbox problem */
	IRQ_NH_MB     = -4,
	/* IRQ Not Handled as pcm MODE problem */
	IRQ_NH_MODE     = -3,
	/* IRQ Not Handled as TYPE problem */
	IRQ_NH_TYPE     = -2,
	/* IRQ Not Handled */
	IRQ_NH          = -1,
	/* IRQ HanDleD */
	IRQ_HDD         = 0,
	/* IRQ HanDleD related to PoinTeR */
	IRQ_HDD_PTR     = 1,
	/* IRQ HanDleD related to STATUS */
	IRQ_HDD_STATUS,
	/* IRQ HanDleD related to SIZE */
	IRQ_HDD_SIZE,
	/* IRQ HanDleD related to PoinTeR of Substream */
	IRQ_HDD_PTRS,
	/* IRQ HanDleD Error */
	IRQ_HDD_ERROR,
};
typedef enum IRQ_RT irq_rt_t;

typedef irq_rt_t (*irq_hdl_t)(void *, unsigned int);

/*****************************************************************************
  4 ��Ϣͷ����
*****************************************************************************/

/*****************************************************************************
  5 ��Ϣ����
*****************************************************************************/

/*****************************************************************************s
  6 STRUCT����
*****************************************************************************/

#ifdef __DRV_AUDIO_MAILBOX_WORK__
struct hi6210_pcm_mailbox_data
{
	struct list_head node;
	unsigned short   msg_type;   /* ��Ϣ���ͣ�HIFI_CHN_MSG_TYPE */
	unsigned short   pcm_mode;   /* PLAYBACK �� CAPTURE */
	void *           substream;  /* ͨ��SubStream����ĵ�ַ */
};
#endif

/* runtime data������PLAYBACK��CAPTURE */
struct hi6210_runtime_data
{
	spinlock_t          lock;         /* protect hi6210_runtime_data */
	unsigned int        period_next;  /* record which period to fix dma next time */
	unsigned int        period_cur;   /* record which period using now */
	unsigned int        period_size;  /* DMA SIZE */
	enum HI6210_STATUS  status;       /* pcm status running or stop */
	struct snd_pcm_substream * substream;
#ifdef __DRV_AUDIO_MAILBOX_WORK__
	struct hi6210_pcm_mailbox_data hi6210_pcm_mailbox;
#endif
	long snd_pre_time;
};

struct hifi_pcm_config
{
	unsigned int channels;
	unsigned int rate;
	unsigned int period_size;
	unsigned int period_count;
	unsigned int format;
};

/* AP�˼�ͨ�Ŵ��ݸ�HIFI�����ݣ���PCM OPENʱʹ�õĽṹ����Ϣ */
struct hifi_chn_pcm_open
{
	HI_CHN_COMMON
	struct hifi_pcm_config config;
};

/* AP�˼�ͨ�Ŵ��ݸ�HIFI�����ݣ���PCM OPENʱʹ�õĽṹ����Ϣ */
struct hifi_chn_pcm_close
{
	HI_CHN_COMMON
};

/* AP�˼�ͨ�Ŵ��ݸ�HIFI�����ݣ���HW PARAMSʱʹ�õĽṹ����Ϣ */
struct hifi_chn_pcm_hw_params
{
	HI_CHN_COMMON
	unsigned int    channel_num;    /* ͨ���������� */
	unsigned int    sample_rate;    /* ͨ�������� */
	unsigned int    format;         /* ��Ƶ��ʽ */
};

/* AP�˼�ͨ�Ŵ��ݸ�HIFI�����ݣ���HW FREEʱʹ�õĽṹ����Ϣ
struct hifi_chn_pcm_hw_free
{
    HI_CHN_COMMON
};*/

/* AP�˼�ͨ�Ŵ��ݸ�HIFI�����ݣ���PCM PREPAREʱʹ�õĽṹ����Ϣ
struct hifi_chn_pcm_prepare
{
    HI_CHN_COMMON
};*/

/* AP�˼�ͨ�Ŵ��ݸ�HIFI�����ݣ���PCM TRIGGERʱʹ�õĽṹ����Ϣ */
struct hifi_chn_pcm_trigger
{
	HI_CHN_COMMON
	unsigned short  tg_cmd;         /* ��������,����SNDRV_PCM_TRIGGER_START */
	unsigned short  reserved1;      /* reserved for aligned */
	unsigned int    substream_l32;  /* ͨ��SubStream����ĵ�ַ */
	unsigned int    substream_h32;  /* ͨ��SubStream����ĵ�ַ */
	unsigned int    data_addr;      /* DMA���ݰ��˵�����ָ�룬�����ַ */
	unsigned int    data_len;       /* ���ݳ��ȣ���λByte */
};

/* AP�˼�ͨ�Ŵ��ݸ�HIFI�����ݣ�DMA Buffer���ݴ���ʱʹ�� */
struct hifi_channel_set_buffer
{
	HI_CHN_COMMON
	unsigned int    data_addr;  /* DMA���ݰ��˵�����ָ�룬�����ַ
									   PLAYBACKʱ��ΪDMA���ݰ���Դ
									   CAPTURE ʱ��ΪDMA���ݰ���Ŀ�ĵ� */
	unsigned int    data_len;   /* ���ݳ��ȣ���λByte */
};

/* HIFI�˼�ͨ�Ŵ��ݸ�AP�����ݣ���DMA���ݰ�����ɺ󴫵� */
struct hifi_chn_pcm_period_elapsed
{
	HI_CHN_COMMON
	unsigned int    substream_l32;  /* ͨ��SubStream����ĵ�ַ */
	unsigned int    substream_h32;  /* ͨ��SubStream����ĵ�ַ */
	unsigned int    msg_timestamp;
};

#define PCM_DMA_BUF_PLAYBACK_PRIMARY_LEN (48*20*2*2*4)

#define PCM_DMA_BUF_PLAYBACK_DIRECT_LEN (384*20*4*2*4)

#define PCM_DMA_BUF_PLAYBACK_LEN    (0x00030000)
#define PCM_DMA_BUF_0_PLAYBACK_BASE (PCM_PLAY_BUFF_LOCATION)
#define PCM_DMA_BUF_0_PLAYBACK_LEN  (PCM_DMA_BUF_PLAYBACK_PRIMARY_LEN)
#define PCM_DMA_BUF_0_CAPTURE_BASE  (PCM_DMA_BUF_0_PLAYBACK_BASE+PCM_DMA_BUF_0_PLAYBACK_LEN)
#define PCM_DMA_BUF_0_CAPTURE_LEN   (PCM_DMA_BUF_PLAYBACK_LEN)

#define PCM_DMA_BUF_1_PLAYBACK_BASE (PCM_DMA_BUF_0_CAPTURE_BASE+PCM_DMA_BUF_0_CAPTURE_LEN)
#define PCM_DMA_BUF_1_PLAYBACK_LEN  (PCM_DMA_BUF_PLAYBACK_DIRECT_LEN)
#define PCM_DMA_BUF_1_CAPTURE_BASE  (PCM_DMA_BUF_1_PLAYBACK_BASE+PCM_DMA_BUF_1_PLAYBACK_LEN)
#define PCM_DMA_BUF_1_CAPTURE_LEN   (193*1024)

#define PCM_DMA_BUF_2_PLAYBACK_BASE (PCM_DMA_BUF_1_CAPTURE_BASE+PCM_DMA_BUF_1_CAPTURE_LEN)
#define PCM_DMA_BUF_2_PLAYBACK_LEN  (PCM_DMA_BUF_PLAYBACK_LEN)
#define PCM_DMA_BUF_2_CAPTURE_BASE  (PCM_DMA_BUF_2_PLAYBACK_BASE+PCM_DMA_BUF_2_PLAYBACK_LEN)
#define PCM_DMA_BUF_2_CAPTURE_LEN   (PCM_DMA_BUF_PLAYBACK_LEN)

#define PCM_DMA_BUF_3_PLAYBACK_BASE (PCM_DMA_BUF_2_CAPTURE_BASE+PCM_DMA_BUF_2_CAPTURE_LEN)
#define PCM_DMA_BUF_3_PLAYBACK_LEN  (0)
#define PCM_DMA_BUF_3_CAPTURE_BASE  (PCM_DMA_BUF_3_PLAYBACK_BASE+PCM_DMA_BUF_3_PLAYBACK_LEN)
#define PCM_DMA_BUF_3_CAPTURE_LEN   (0)

#define PCM_STREAM_MAX              (2)
#define PCM_DEVICE_MAX              (7)

typedef struct pcm_dma_buf_config
{
	u64       pcm_dma_buf_base;
	u64       pcm_dma_buf_len;
}PCM_DMA_BUF_CONFIG;

/*****************************************************************************
  7 UNION����
*****************************************************************************/


/*****************************************************************************
  8 OTHERS����
*****************************************************************************/


/*****************************************************************************
  9 ȫ�ֱ�������
*****************************************************************************/


/*****************************************************************************
  10 ��������
*****************************************************************************/
void snd_pcm_print_timeout(struct snd_pcm_substream *substream, unsigned int timeout_type);
void snd_pcm_reset_pre_time(struct snd_pcm_substream *substream);

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /*HI6210_PCM_H*/

