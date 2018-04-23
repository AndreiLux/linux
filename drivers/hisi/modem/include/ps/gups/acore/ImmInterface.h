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


#ifndef __IMMINTERFACE_H__
#define __IMMINTERFACE_H__


/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "vos.h"
#include "product_config.h"
#include "IMMmem_PS.h"

#if (defined(CONFIG_BALONG_SPE) && (VOS_LINUX == VOS_OS_VER))
#include "mdrv_spe_wport.h"
#endif /* CONFIG_BALONG_SPE */

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#pragma pack(4)


/*****************************************************************************
  2 �궨��
*****************************************************************************/
#define IMM_MAX_ETH_FRAME_LEN           (1536)
#define IMM_MAC_HEADER_RES_LEN          (14)
#define IMM_INVALID_VALUE               (0xFFFFFFFF)


/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/


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
  8 UNION����
*****************************************************************************/


/*****************************************************************************
  9 OTHERS����
*****************************************************************************/


/*****************************************************************************
  10 ��������
*****************************************************************************/


/*****************************************************************************
 �� �� ��  : IMM_ZcStaticAlloc
 ��������  : �˷��亯��������ACPU����������ʹ�õ��ڴ�
 �������  : ulLen - ������ֽ���
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  : �ɹ�������ָ��IMM_ZC_STRU��ָ��
             ʧ�ܣ�����NULL��
 ��������  :
 ˵    ��  :

 �޸���ʷ      :
  1.��    ��   : 2011��12��1��
    ��    ��   : y00171741
    �޸�����   : �����ɺ���

*****************************************************************************/
extern IMM_ZC_STRU* IMM_ZcStaticAlloc_Debug(unsigned short usFileID, unsigned short usLineNums,
            unsigned int ulLen);

#define    IMM_ZcStaticAlloc(ulLen)\
    IMM_ZcStaticAlloc_Debug(THIS_FILE_ID, __LINE__, (ulLen))



/*****************************************************************************
 �� �� ��  : IMM_ZcLargeMemAlloc
 ��������  : ��Linuxϵͳ�������ڴ�ķ��亯��
 �������  : ulLen - ������ֽ���
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  : �ɹ�������ָ��IMM_ZC_STRU��ָ��
             ʧ�ܣ�����NULL��
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��12��1��
    ��    ��   : y00171741
    �޸�����   : �����ɺ���

*****************************************************************************/
#define IMM_ZcLargeMemAlloc(ulLen)  dev_alloc_skb((ulLen))

/*****************************************************************************
 �� �� ��  : IMM_DataTransformImmZc
 ��������  : ����IMM_ZC_STRU����ͷ����C�˿�˹����ڴ�ҽӵ�IMM_ZC_STRU��
 �������  : pucData - ���ݿ��׵�ַ
             ulLen - ���ݿ鳤��(byte)
             pstTtfMem - ����C�˵ĵ�ַ��C��ָ��TTF_MEM�ĵ�ַ
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  : �ɹ�������ָ��IMM_ZC_STRU��ָ��
             ʧ�ܣ�����NULL��
 ��������  :
 ˵    ��  : �˺������ֲ�Ʒ����̬, ��MBB�ϴ˽ӿ���Ч, ���ܻ��Ϸ��ؿ�ָ��

 �޸���ʷ      :
  1.��    ��   : 2011��12��1��
    ��    ��   : y00171741
    �޸�����   : �����ɺ���

*****************************************************************************/
extern IMM_ZC_STRU * IMM_ZcDataTransformImmZc_Debug(unsigned short usFileID,
            unsigned short usLineNum, const unsigned char *pucData, unsigned int ulLen, void *pstTtfMem);


#define IMM_DataTransformImmZc(pucData, ulLen, pstTtfMem)\
    IMM_ZcDataTransformImmZc_Debug(THIS_FILE_ID, __LINE__, (pucData), (ulLen), (pstTtfMem))


/*****************************************************************************
 �� �� ��  : IMM_ZcStaticCopy
 ��������  : IMM_ZC�����ݽṹ�Ŀ��������ƽڵ�����ݿ飩�������ݿ��Linux�Դ����ڴ濽����A�˹����ڴ�
 �������  : pstImmZc - Դ���ݽṹ
 �������  : ��
 �� �� ֵ  : �ɹ�������Ŀ��IMM_ZC_STRU��ָ��
             ʧ�ܣ�����NULL��
 ���ú���  :
 ��������  :
 ˵    ��  : �˺������ֲ�Ʒ����̬, ��MBB�ϴ˽ӿ���Ч, ���ܻ��Ϸ��ؿ�ָ��
             MBB��, A����������ΪMEM_TYPE_SYS_DEFINED�����ݿ�, ���ݴ���C��ǰ,
             һ��Ҫ���ñ��ӿ�, �����ݿ�����A�˹����ڴ档
 �޸���ʷ      :
  1.��    ��   : 2011��12��1��
    ��    ��   : y00171741
    �޸�����   : �����ɺ���

*****************************************************************************/
extern  IMM_ZC_STRU* IMM_ZcStaticCopy_Debug(VOS_UINT16 usFileID, VOS_UINT16 usLineNums, IMM_ZC_STRU* pstImmZc);

#define IMM_ZcStaticCopy(pstImmZc)\
    IMM_ZcStaticCopy_Debug(THIS_FILE_ID, __LINE__, (pstImmZc))

/*****************************************************************************
 �� �� ��  : IMM_ZcFree
 ��������  : �ͷ�IMM_ZC_STRU�ڴ�
 �������  : pstImmZc - ָ��IMM_ZC_STRU��ָ��
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :
 �޸���ʷ      :
  1.��    ��   : 2011��12��1��
    ��    ��   : y00171741
    �޸�����   : �����ɺ���

*****************************************************************************/
#if (defined(CONFIG_BALONG_SPE) && (VOS_LINUX == VOS_OS_VER))
#define IMM_ZcFree( pstImmZc )              mdrv_spe_wport_recycle((pstImmZc))
#else
#define IMM_ZcFree( pstImmZc )              kfree_skb((pstImmZc))
#endif

/*****************************************************************************
 �� �� ��  : IMM_ZcFreeAny
 ��������  : �ͷ�IMM_ZC_STRU�ڴ�
 �������  : pstImmZc - ָ��IMM_ZC_STRU��ָ��
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :
 �޸���ʷ      :
  1.��    ��   : 2017��1��22��
    ��    ��   : l00373346
    �޸�����   : �����ɺ���

*****************************************************************************/
#if (defined(CONFIG_BALONG_SPE) && (VOS_LINUX == VOS_OS_VER))
#define IMM_ZcFreeAny( pstImmZc )              mdrv_spe_wport_recycle((pstImmZc))
#else
#define IMM_ZcFreeAny( pstImmZc )              dev_kfree_skb_any((pstImmZc))
#endif

/*****************************************************************************
 �� �� ��  : IMM_ZcHeadFree
 ��������  : �ͷ�IMM_ZC_STRU���ƽڵ㣬���ݿ鲻�ͷš�
 �������  : pstImmZc - ָ��IMM_ZC_STRU��ָ��
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :
 �޸���ʷ      :
  1.��    ��   : 2011��12��1��
    ��    ��   : y00171741
    �޸�����   : �����ɺ���

*****************************************************************************/
extern void IMM_ZcHeadFree(IMM_ZC_STRU* pstImmZc);

/*****************************************************************************
 �� �� ��  : IMM_RemoteFreeTtfMem
 ��������  : �ͷ�Զ��TTF_Mem��
 �������  : pucAddr - ָ��C�˵�TTF_MEM��ָ��
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :
 �޸���ʷ      :
  1.��    ��   : 2011��12��1��
    ��    ��   : y00171741
    �޸�����   : �����ɺ���

*****************************************************************************/
extern VOS_VOID IMM_RbRemoteFreeMem(VOS_VOID *pucAddr);

#define IMM_RemoteFreeTtfMem(pucAddr)     IMM_RbRemoteFreeMem((pucAddr))


/*****************************************************************************
 �� �� ��  : IMM_ZcAddMacHead
 ��������  : ���MACͷ��
 �������  : pstImmZc - ָ��IMM_ZC_STRU��ָ��
             pucAddData - ��ӵ�MACͷ�׵�ַ
             usLen - MACͷ�ĳ���
 �������  : ��
 �� �� ֵ  : �����ɹ����, VOS_OK - �ɹ�, VOS_ERR - ʧ��
 ���ú���  :
 ��������  :
 ˵    ��  : �˺������ֲ�Ʒ����̬, ��MBB�ϴ˽ӿ���Ч, ���ܻ��Ϸ���ERR

 �޸���ʷ      :
  1.��    ��   : 2011��12��1��
    ��    ��   : y00171741
    �޸�����   : �����ɺ���

*****************************************************************************/
extern unsigned int IMM_ZcAddMacHead (IMM_ZC_STRU *pstImmZc, const unsigned char * pucAddData);

/*****************************************************************************
 �� �� ��  : IMM_ZcRemoveMacHead
 ��������  : ����̫֡�а�ȥMACͷ��
 �������  : pstImmZc - ָ��IMM_ZC_STRU��ָ��
 �������  : ��
 �� �� ֵ  : �����ɹ����, VOS_OK - �ɹ�, VOS_ERR - ʧ��
 ���ú���  :
 ��������  :
 ˵    ��  : �˺������ֲ�Ʒ����̬, ��MBB�ϴ˽ӿ���Ч, ���ܻ��Ϸ���ERR

 �޸���ʷ      :
  1.��    ��   : 2011��12��1��
    ��    ��   : y00171741
    �޸�����   : �����ɺ���

*****************************************************************************/
extern unsigned int IMM_ZcRemoveMacHead (IMM_ZC_STRU *pstImmZc);

/*****************************************************************************
 �� �� ��  : IMM_ZcMapToImmMem
 ��������  : A�˰�IMM_ZC�㿽�����ƽڵ�ת����IMM_Mem���ƽڵ㡣
 �������  : pstImmZc - ָ��IMM_ZC_STRU��ָ��
 �������  : ��
 �� �� ֵ  : ����IMM_MEM_STRU��ָ�롣
 ���ú���  :
 ��������  :
 ˵    ��  : �˺������ֲ�Ʒ����̬, ��MBB�ϴ˽ӿ���Ч, ���ܻ��Ϸ��ؿ�ָ��

 �޸���ʷ      :
  1.��    ��   : 2011��12��1��
    ��    ��   : y00171741
    �޸�����   : �����ɺ���

*****************************************************************************/
extern IMM_MEM_STRU *IMM_ZcMapToImmMem_Debug(unsigned short usFileID,
            unsigned short usLineNum, IMM_ZC_STRU *pstImmZc);


#define IMM_ZcMapToImmMem(pstImmZc)\
    IMM_ZcMapToImmMem_Debug(THIS_FILE_ID, __LINE__, (pstImmZc))


/*****************************************************************************
 �� �� ��  : IMM_ZcPush
 ��������  : ������ӵ���Ч���ݿ��ͷ����
 �������  : pstImmZc - ָ��IMM_ZC_STRU��ָ��
             ulLen - ������ݵĳ���
 �������  : ��
 �� �� ֵ  : ���ص����ݿ��׵�ַ���������������֮������ݿ��ַ��
 ���ú���  :
 ��������  :
 ����      : ���ӿ�ֻ�ƶ�ָ�룻
             ������ӵ���Ч���ݿ��ͷ��֮ǰ,���ñ��ӿ�
 �޸���ʷ      :
  1.��    ��   : 2011��12��1��
    ��    ��   : y00171741
    �޸�����   : �����ɺ���

*****************************************************************************/
#define IMM_ZcPush(pstImmZc,ulLen)          skb_push((pstImmZc),(ulLen))


/*****************************************************************************
 �� �� ��  : IMM_ZcPull
 ��������  : ��IMM_ZCָ������ݿ��ͷ��ȡ�����ݡ�
 �������  : pstImmZc - ָ��IMM_ZC_STRU��ָ��
             ulLen - ȡ�����ݵĳ���
 �������  : ��
 �� �� ֵ  : ���ص����ݿ��׵�ַ��������ȡ������֮��ĵ�ַ��
 ���ú���  :
 ��������  :
 ����      : ���ӿ�ֻ�ƶ�ָ�룻
 �޸���ʷ      :
  1.��    ��   : 2011��12��1��
    ��    ��   : y00171741
    �޸�����   : �����ɺ���

*****************************************************************************/
#define IMM_ZcPull(pstImmZc,ulLen)          skb_pull((pstImmZc),(ulLen))


/*****************************************************************************
 �� �� ��  : IMM_ZcPut
 ��������  : ���������IMM_ZCָ������ݿ��β����
 �������  : pstImmZc - ָ��IMM_ZC_STRU��ָ��
             ulLen - ������ݵĳ���
 �������  : ��
 �� �� ֵ  : ���ص����ݿ�β����ַ���������������֮ǰ�����ݿ�β����ַ��
 ���ú���  :
 ��������  :
 ����      : ���ӿ�ֻ�ƶ�ָ�룻
 �޸���ʷ      :
  1.��    ��   : 2011��12��1��
    ��    ��   : y00171741
    �޸�����   : �����ɺ���

*****************************************************************************/
#define IMM_ZcPut(pstImmZc,ulLen)           skb_put((pstImmZc),(ulLen))


/*****************************************************************************
 �� �� ��  : IMM_ZcReserve
 ��������  : Ԥ��IMM_ZCָ������ݿ�ͷ���ռ䡣
 �������  : pstImmZc - ָ��IMM_ZC_STRU��ָ��
             ulLen - Ԥ������ͷ���Ŀռ�(byte)
 �������  : ��
 �� �� ֵ  : �ޡ�
 ���ú���  :
 ��������  :
 ˵    ��  : ���ӿ�ֻ�ƶ�ָ�룬Ϊͷ��Ԥ���ռ䡣
             ֻ���ڸշ����IMM_ZC,IMM_ZCָ������ݿ黹û��ʹ�ã�
 �޸���ʷ      :
  1.��    ��   : 2011��12��1��
    ��    ��   : y00171741
    �޸�����   : �����ɺ���

*****************************************************************************/
#define IMM_ZcReserve(pstImmZc,ulLen)       skb_reserve((pstImmZc),(int)(ulLen))


/*****************************************************************************
 �� �� ��  : IMM_ZcResetTailPoint
 ��������  : ����IMM_ZCβ����ַ
 �������  : pstImmZc --- ָ��IMM_ZC_STRU��ָ��
 �������  : ��
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��10��06��
    ��    ��   : A00165503
    �޸�����   : �����ɺ���

*****************************************************************************/
#define IMM_ZcResetTailPointer(pstImmZc)    skb_reset_tail_pointer((pstImmZc))


/*****************************************************************************
 �� �� ��  : IMM_ZcHeadRoom
 ��������  : �õ�����ͷ��Ԥ���ռ��ֽ�����
 �������  : pstImmZc - ָ��IMM_ZC_STRU��ָ��
 �������  : ��
 �� �� ֵ  : Ԥ���ռ��ֽ�����
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��12��1��
    ��    ��   : y00171741
    �޸�����   : �����ɺ���

*****************************************************************************/
#define IMM_ZcHeadRoom(pstImmZc)            skb_headroom((pstImmZc))


/*****************************************************************************
 �� �� ��  : IMM_ZcTailRoom
 ��������  : �õ�����β��Ԥ���ռ��ֽ�����
 �������  : pstImmZc - ָ��IMM_ZC_STRU��ָ��
 �������  : ��
 �� �� ֵ  : Ԥ���ռ��ֽ�����
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��12��1��
    ��    ��   : y00171741
    �޸�����   : �����ɺ���

*****************************************************************************/
#define IMM_ZcTailRoom(pstImmZc)            skb_tailroom((pstImmZc))


/*****************************************************************************
 �� �� ��  : IMM_ZcGetDataPtr
 ��������  : �õ����ݿ��׵�ַ��
 �������  : pstImmZc - ָ��IMM_ZC_STRU��ָ��
 �������  : ��
 �� �� ֵ  : ���ݿ��׵�ַ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��12��1��
    ��    ��   : y00171741
    �޸�����   : �����ɺ���

*****************************************************************************/
#define IMM_ZcGetDataPtr(pstImmZc)          ((pstImmZc)->data)


/*****************************************************************************
 �� �� ��  : IMM_ZcGetUsedLen
 ��������  : �õ����ݿ�ʹ�õ��ֽ�����
 �������  : pstImmZc - ָ��IMM_ZC_STRU��ָ��
 �������  : ��
 �� �� ֵ  : ���ݿ�ʹ�õ��ֽ�����
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��12��1��
    ��    ��   : y00171741
    �޸�����   : �����ɺ���

*****************************************************************************/
#define IMM_ZcGetUsedLen(pstImmZc)          ((pstImmZc)->len)


/*****************************************************************************
 �� �� ��  : IMM_ZcGetUserApp
 ��������  : �õ�UserApp��
 �������  : pstImmZc - ָ��IMM_ZC_STRU��ָ��
 �������  : ��
 �� �� ֵ  : �õ�UserApp��
 ���ú���  :
 ��������  :
 ˵    ��  : ����MBB�����ܻ���̬, MBB�ϴ˽ӿ���Ч, ���ܻ�����Ч, ֱ�ӷ���0
             ���Ҫ���ⲿʹ���ߴ������

 �޸���ʷ      :
  1.��    ��   : 2011��12��1��
    ��    ��   : y00171741
    �޸�����   : �����ɺ���

*****************************************************************************/
extern unsigned short IMM_ZcGetUserApp(IMM_ZC_STRU *pstImmZc);


/*****************************************************************************
 �� �� ��  : IMM_ZcSetUserApp
 ��������  : ����UserApp��
 �������  : pstImmZc - ָ��IMM_ZC_STRU��ָ��
             usApp - �û��Զ���
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :
 ˵    ��  : ����MBB�����ܻ���̬, MBB�ϴ˽ӿ���Ч, ���ܻ�����Ч, Ϊ�պ���
             ���Ҫ���ⲿʹ���ߴ������

 �޸���ʷ      :
  1.��    ��   : 2011��12��1��
    ��    ��   : y00171741
    �޸�����   : �����ɺ���

*****************************************************************************/
extern void IMM_ZcSetUserApp (IMM_ZC_STRU *pstImmZc, unsigned short usApp);


/*****************************************************************************
 �� �� ��  : IMM_ZcQueueHeadInit
 ��������  : ���г�ʼ����
 �������  : pstList - ָ��IMM_ZC_HEAD_STRU��ָ��
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��12��1��
    ��    ��   : y00171741
    �޸�����   : �����ɺ���

*****************************************************************************/
#define IMM_ZcQueueHeadInit(pstList)        skb_queue_head_init((pstList))


/*****************************************************************************
 �� �� ��  : IMM_ZcQueueHead
 ��������  : Ԫ�ز������ͷ����
 �������  : pstList - ָ��IMM_ZC_HEAD_STRU��ָ��
             pstNew  - �����Ԫ�أ���ָ��IMM_ZC_STRU��ָ��
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��12��1��
    ��    ��   : y00171741
    �޸�����   : �����ɺ���

*****************************************************************************/
#define IMM_ZcQueueHead(pstList,pstNew)     skb_queue_head((pstList),(pstNew))


/*****************************************************************************
 �� �� ��  : IMM_ZcQueueTail
 ��������  : Ԫ�ز������β�� ��
 �������  : pstList - ָ��IMM_ZC_HEAD_STRU��ָ��
             pstNew  - �����Ԫ�أ���ָ��IMM_ZC_STRU��ָ��
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��12��1��
    ��    ��   : y00171741
    �޸�����   : �����ɺ���

*****************************************************************************/
#define IMM_ZcQueueTail(pstList,pstNew)     skb_queue_tail((pstList),(pstNew))


/*****************************************************************************
 �� �� ��  : IMM_ZcDequeueHead
 ��������  : �Ӷ���ͷ�� ȡԪ�� ��
 �������  : pstList - ָ��IMM_ZC_HEAD_STRU��ָ��
 �������  : ��
 �� �� ֵ  : ָ��IMM_ZC_STRU��ָ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��12��1��
    ��    ��   : y00171741
    �޸�����   : �����ɺ���

*****************************************************************************/
#define IMM_ZcDequeueHead(pstList)          skb_dequeue((pstList))


/*****************************************************************************
 �� �� ��  : IMM_ZcDequeueTail
 ��������  : �Ӷ���β��ȡԪ�� ��
 �������  : pstList - ָ��IMM_ZC_HEAD_STRU��ָ��
 �������  : ��
 �� �� ֵ  : ָ��IMM_ZC_STRU��ָ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��12��1��
    ��    ��   : y00171741
    �޸�����   : �����ɺ���

*****************************************************************************/
#define IMM_ZcDequeueTail(pstList)          skb_dequeue_tail((pstList))


/*****************************************************************************
 �� �� ��  : IMM_ZcQueueLen
 ��������  : �õ������е�Ԫ�ص���Ŀ��
 �������  : pstList - ָ��IMM_ZC_HEAD_STRU��ָ��
 �������  : ��
 �� �� ֵ  : �õ������е�Ԫ�ص���Ŀ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��12��1��
    ��    ��   : y00171741
    �޸�����   : �����ɺ���

*****************************************************************************/
#define IMM_ZcQueueLen(pstList)             skb_queue_len((pstList))


/*****************************************************************************
 �� �� ��  : IMM_ZcQueuePeek
 ��������  : �õ������еĶ���Ԫ�ص�ָ��
 �������  : pstList - ָ��IMM_ZC_HEAD_STRU��ָ��
 �������  : ��
 �� �� ֵ  : ָ�����Ԫ�ص�ָ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��8��11��
    ��    ��   : liukai
    �޸�����   : �����ɺ���

*****************************************************************************/
#define IMM_ZcQueuePeek(pstList)            skb_peek((pstList))


/*****************************************************************************
 �� �� ��  : IMM_ZcQueuePeekTail
 ��������  : �õ������еĶ�βԪ�ص�ָ��
 �������  : pstList - ָ��IMM_ZC_HEAD_STRU��ָ��
 �������  : ��
 �� �� ֵ  : ָ���βԪ�ص�ָ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��8��11��
    ��    ��   : liukai
    �޸�����   : �����ɺ���

*****************************************************************************/
#define IMM_ZcQueuePeekTail(pstList)        skb_peek_tail((pstList))


/*****************************************************************************
 �� �� ��  : IMM_MntnAcpuCheckMemPoolLeak
 ��������  : �ṩ���ⲿģ����ü��A���ڴ��Ƿ�й¶
 �������  : VOS_VOID
 �������  : ��
 �� �� ֵ  : VOS_VOID
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��1��13��
    ��    ��   : s00164817
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_VOID IMM_MntnAcpuCheckPoolLeak( VOS_VOID );


#pragma pack()




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of ImmInterface.h */

