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

#include "AtCheckFunc.h"
#include "ATCmdProc.h"
#include "AtDataProc.h"
#include "Taf_Tafm_Remote.h"
#include "AtTestParaCmd.h"

#include "TafAppMma.h"


#define    THIS_FILE_ID        PS_FILE_ID_AT_TESTPARACMD_C


/*****************************************************************************
 �� �� ��  : At_TestCgdcont
 ��������  : cgdcont���Ժ���
 �������  : VOS_UINT8 ucIndex
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��10��18��
    ��    ��   : c00184452
    �޸�����   : AT��ֲ��Ŀ
  2.��    ��   : 2011��12��19��
    ��    ��   : c00173809
    �޸�����   : PS�ں���Ŀ
  3.��    ��   : 2012��04��10��
    ��    ��   : s62952
    �޸�����   : ���ⵥ��:DTS2012040902843
  3.��    ��   : 2012��05��03��
    ��    ��   : w00199382
    �޸�����   : ���ⵥ��:DTS2012042602550
  4.��    ��   : 2012��5��26��
    ��    ��   : A00165503
    �޸�����   : DTS2012052404203: +CGDCONT�������Ӷ�<CID 0>��֧��
  5.��    ��   : 2012��8��10��
    ��    ��   : z60575
    �޸�����   : DTS2012081006711: ��Ӷ�IPV6�������ж�
  6.��    ��   : 2013��05��25��
    ��    ��   : f00179208
    �޸�����   : V3R3 PPP PROJECT
  7.��    ��   : 2013��07��08��
    ��    ��   : Y00213812
    �޸�����   : VoLTE_PhaseI ��Ŀ��������
*****************************************************************************/
VOS_UINT32 At_TestCgdcont(VOS_UINT8 ucIndex)
{
    VOS_UINT16 usLength = 0;

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr, (VOS_CHAR *)pgucAtSndCodeAddr,
                                       "%s: (0-31),\"IP\",,,(0-2),(0-3),(0,1),(0,1),(0-2),(0,1)\r\n",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName);


    if (AT_IPV6_CAPABILITY_IPV4_ONLY != AT_GetIpv6Capability())
    {
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR*)pgucAtSndCodeAddr,(TAF_CHAR*)pgucAtSndCodeAddr + usLength,
                                           "%s: (0-31),\"IPV6\",,,(0-2),(0-3),(0,1),(0,1),(0-2),(0,1)\r\n",
                                           g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR*)pgucAtSndCodeAddr,(TAF_CHAR*)pgucAtSndCodeAddr + usLength,
                                           "%s: (0-31),\"IPV4V6\",,,(0-2),(0-3),(0,1),(0,1),(0-2),(0,1)\r\n",
                                           g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
    }

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr, (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s: (0-31),\"PPP\",,,(0-2),(0-3),(0,1),(0,1),(0-2),(0,1)",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName);


    gstAtSendData.usBufLen = usLength;

    return AT_OK;
}

/*****************************************************************************
 �� �� ��  : At_TestCgdscont
 ��������  : cgdscont���Ժ���
 �������  : VOS_UINT8 ucIndex
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��10��18��
    ��    ��   : c00184452
    �޸�����   : AT��ֲ��Ŀ
  2.��    ��   : 2011��12��19��
    ��    ��   : c00173809
    �޸�����   : PS�ں���Ŀ
  3.��    ��   : 2012��3��5��
    ��    ��   : c00173809
    �޸�����   : DTS2012010604900,Cgdscont��������GU��Lģʽ��Ҫ����CID�����ȡֵ��Χ
  4.��    ��   : 2011��5��2��
    ��    ��   : w00199382
    �޸�����   : DTS2012042803139ȥ���ַ�������ය��
  5.��    ��   : 2013��07��08��
    ��    ��   : Y00213812
    �޸�����   : VoLTE_PhaseI ��Ŀ,�޸���PDP������Χ
*****************************************************************************/
VOS_UINT32 At_TestCgdscont(VOS_UINT8 ucIndex)
{
    VOS_UINT16 usLength = 0;

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr, (VOS_CHAR *)pgucAtSndCodeAddr,
                                        "%s: (1-31),(0-31),(0-2),(0-3)",
                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    gstAtSendData.usBufLen = usLength;

    return AT_OK;
}


/*****************************************************************************
 �� �� ��  : At_TestCgtft
 ��������  : cgtft���Ժ���
 �������  : VOS_UINT8 ucIndex
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��10��18��
    ��    ��   : c00184452
    �޸�����   : AT��ֲ��Ŀ
  2.��    ��   : 2012��3��24��
    ��    ��   : c00173809
    �޸�����   : DTS2012031901946,�ڲ���������ɾ��PPPЭ��
  3.��    ��   : 2012��8��10��
    ��    ��   : z60575
    �޸�����   : DTS2012081006711: ��Ӷ�IPV6�������ж�
  4.��    ��   : 2015��9��28��
    ��    ��   : W00316404
    �޸�����   : R11 TFT Э������
*****************************************************************************/
VOS_UINT32 At_TestCgtft(VOS_UINT8 ucIndex)
{
    VOS_UINT16 usLength = 0;

    if (AT_IsSupportReleaseRst(AT_ACCESS_STRATUM_REL11))
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr, (VOS_CHAR *)pgucAtSndCodeAddr,
                                           "%s: \"IP\",(1-16),(0-255),,(0-255),,,(00000000-FFFFFFFF),,(00000-FFFFF),(0-3),\r\n",
                                           g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
    }
    else
    {
        usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr, (VOS_CHAR *)pgucAtSndCodeAddr,
                                           "%s: \"IP\",(1-16),(0-255),,(0-255),,,(00000000-FFFFFFFF),,(00000-FFFFF),(0-3)\r\n",
                                           g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
    }

    if (AT_IPV6_CAPABILITY_IPV4_ONLY != AT_GetIpv6Capability())
    {
        if (AT_IsSupportReleaseRst(AT_ACCESS_STRATUM_REL11))
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR*)pgucAtSndCodeAddr,(TAF_CHAR*)pgucAtSndCodeAddr + usLength,
                                               "%s: \"IPV6\",(1-16),(0-255),,(0-255),,,(00000000-FFFFFFFF),,(00000-FFFFF),(0-3),\r\n",
                                               g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR*)pgucAtSndCodeAddr,(TAF_CHAR*)pgucAtSndCodeAddr + usLength,
                                               "%s: \"IPV4V6\",(1-16),(0-255),,(0-255),,,(00000000-FFFFFFFF),,(00000-FFFFF),(0-3),",
                                               g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        }
        else
        {
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR*)pgucAtSndCodeAddr,(TAF_CHAR*)pgucAtSndCodeAddr + usLength,
                                               "%s: \"IPV6\",(1-16),(0-255),,(0-255),,,(00000000-FFFFFFFF),,(00000-FFFFF),(0-3)\r\n",
                                               g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
            usLength += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,(TAF_CHAR*)pgucAtSndCodeAddr,(TAF_CHAR*)pgucAtSndCodeAddr + usLength,
                                               "%s: \"IPV4V6\",(1-16),(0-255),,(0-255),,,(00000000-FFFFFFFF),,(00000-FFFFF),(0-3)",
                                               g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
        }
    }

    gstAtSendData.usBufLen = usLength;

    return AT_OK;
}

/*****************************************************************************
 �� �� ��  : At_TestCgact
 ��������  : cgact���Ժ���
 �������  : VOS_UINT8 ucIndex
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��10��18��
    ��    ��   : c00184452
    �޸�����   : AT��ֲ��Ŀ
*****************************************************************************/
VOS_UINT32 At_TestCgact(VOS_UINT8 ucIndex)
{
    VOS_UINT16 usLength = 0;

    usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr, (VOS_CHAR *)pgucAtSndCodeAddr,
                                       "%s: (0,1)",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
    gstAtSendData.usBufLen = usLength;

    return AT_OK;
}

/*****************************************************************************
 �� �� ��  : At_TestCgcmod
 ��������  : cgcmod���Ժ���
 �������  : VOS_UINT8 ucIndex
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��10��18��
    ��    ��   : c00184452
    �޸�����   : AT��ֲ��Ŀ
  2.��    ��   : 2011��12��19��
    ��    ��   : �����/00130025
    �޸�����   : ���ⵥDTS2011120604361��CGEQNEG��������ش���,CGCMOD���ش���
*****************************************************************************/
VOS_UINT32 At_TestCgcmod(VOS_UINT8 ucIndex)
{


    /* CGEQNEG�Ĳ��������CGCMODE�ķ���ֵ��ͬ��ʹ����ͬ�ĺ��� */
   return At_TestCgeqnegPara(ucIndex);


}

/*****************************************************************************
 �� �� ��  : At_TestCgpaddr
 ��������  : CGPADDR���Ժ���
 �������  : VOS_UINT8 ucIndex
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��3��5��
    ��    ��   : c00173809
    �޸�����   : DTS2012010604900
  2.��    ��   : 2013��07��08��
    ��    ��   : Y00213812
    �޸�����   : VoLTE_PhaseI ��Ŀ��ɾ����״̬�ж�
*****************************************************************************/
VOS_UINT32 At_TestCgpaddr(VOS_UINT8 ucIndex)
{
    /* ִ��������� */
    if ( VOS_OK != TAF_PS_GetPdpContextInfo(WUEPS_PID_AT,
                                            AT_PS_BuildExClientId(gastAtClientTab[ucIndex].usClientId),
                                            0) )
    {
        return AT_ERROR;
    }

    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CGPADDR_TEST;

    return AT_WAIT_ASYNC_RETURN;
}

/*****************************************************************************
 �� �� ��  : At_TestCgautoPara
 ��������  : +CGAUTO���Ժ���
 �������  : VOS_UINT8 ucIndex
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��01��09��
    ��    ��   : l00198894
    �޸�����   : ��������
*****************************************************************************/
VOS_UINT32 At_TestCgautoPara(VOS_UINT8 ucIndex)
{
    VOS_UINT16      usLength;

    usLength = 0;

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s: (0,1)",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    gstAtSendData.usBufLen = usLength;

    return AT_OK;
}

/*****************************************************************************
 �� �� ��  : At_TestAuhtdata
 ��������  : Auhtdata���Ժ���
 �������  : VOS_UINT8 ucIndex
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��10��18��
    ��    ��   : c00184452
    �޸�����   : AT��ֲ��Ŀ

  2.��    ��   : 2012��3��5��
    ��    ��   : c00173809
    �޸�����   : DTS2012010604900,authdata��������GU��Lģʽ��Ҫ����CID�����ȡֵ��Χ

  3.��    ��   : 2013��12��25��
    ��    ��   : A00165503
    �޸�����   : DTS2013122403650: ^AUTHDATA����֧��CID0
*****************************************************************************/
VOS_UINT32 At_TestAuhtdata(VOS_UINT8 ucIndex)
{
    VOS_UINT16 usLength = 0;

    usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr, (VOS_CHAR *)pgucAtSndCodeAddr,
                                   "%s: (0-31),(0-2),,",
                                   g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    gstAtSendData.usBufLen = usLength;

    return AT_OK;
}

/*****************************************************************************
 �� �� ��  : At_TestNdisconn
 ��������  : Ndisconn���Ժ���
 �������  : VOS_UINT8 ucIndex
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��10��18��
    ��    ��   : c00184452
    �޸�����   : AT��ֲ��Ŀ
  2.��    ��   : 2012��5��7��
    ��    ��   : l60609
    �޸�����   : DTS2012050501603:CID��Χ����
*****************************************************************************/
VOS_UINT32 At_TestNdisconn(VOS_UINT8 ucIndex)
{
    VOS_UINT16                          usLength;

    usLength = 0;

    usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr, (VOS_CHAR *)pgucAtSndCodeAddr,
                                       "%s: (1-20),(0,1),,,,(0,1,2,3)",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    gstAtSendData.usBufLen = usLength;

    return AT_OK;
}

/*****************************************************************************
 �� �� ��  : At_TestNdisDup
 ��������  : NdisDup���Ժ���
 �������  : VOS_UINT8 ucIndex
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��10��18��
    ��    ��   : c00184452
    �޸�����   : AT��ֲ��Ŀ
  2.��    ��   : 2012��5��7��
    ��    ��   : l60609
    �޸�����   : DTS2012050501603:CID��Χ����
*****************************************************************************/
VOS_UINT32 At_TestNdisDup(VOS_UINT8 ucIndex)
{
    VOS_UINT16                          usLength;

    usLength = 0;

    usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr, (VOS_CHAR *)pgucAtSndCodeAddr,
                                       "%s: (1-20),(0-1)",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    gstAtSendData.usBufLen = usLength;

    return AT_OK;
}


/*****************************************************************************
 �� �� ��  : At_TestCgeqos
 ��������  : cgeqos���Ժ���
 �������  : VOS_UINT8 ucIndex
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��1��8��
    ��    ��   : c00184452
    �޸�����   : PS�ں���Ŀ

  2.��    ��   : 2015��10��22��
    ��    ��   : W00316404
    �޸�����   : R11 ����
*****************************************************************************/
VOS_UINT32 At_TestCgeqos(VOS_UINT8 ucIndex)
{
    VOS_UINT16 usLength = 0;

    usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr, (VOS_CHAR *)pgucAtSndCodeAddr,
                                       "%s: (1-31),(0-9),(0-10485760),(0-10485760),(0-10485760),(0-10485760)",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
    gstAtSendData.usBufLen = usLength;

    return AT_OK;
}


/*****************************************************************************
 �� �� ��  : At_TestCgeqosrdp
 ��������  : cgeqosrdp���Ժ���
 �������  : VOS_UINT8 ucIndex
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��1��8��
    ��    ��   : c00184452
    �޸�����   : PS�ں���Ŀ
*****************************************************************************/
VOS_UINT32 At_TestCgeqosrdp(VOS_UINT8 ucIndex)
{
    VOS_UINT16 usLength = 0;

    usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr, (VOS_CHAR *)pgucAtSndCodeAddr,
                                       "%s: (1-31)",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
    gstAtSendData.usBufLen = usLength;

    return AT_OK;
}


/*****************************************************************************
 �� �� ��  : At_TestCgcontrdp
 ��������  : cgcontrdp���Ժ���
 �������  : VOS_UINT8 ucIndex
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��1��8��
    ��    ��   : c00184452
    �޸�����   : PS�ں���Ŀ
*****************************************************************************/
VOS_UINT32 At_TestCgcontrdp(VOS_UINT8 ucIndex)
{
    VOS_UINT16 usLength = 0;

    usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr, (VOS_CHAR *)pgucAtSndCodeAddr,
                                       "%s: (1-31)",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
    gstAtSendData.usBufLen = usLength;

    return AT_OK;
}


/*****************************************************************************
 �� �� ��  : At_TestCgscontrdp
 ��������  : cgscontrdp���Ժ���
 �������  : VOS_UINT8 ucIndex
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��1��8��
    ��    ��   : c00184452
    �޸�����   : PS�ں���Ŀ
*****************************************************************************/
VOS_UINT32 At_TestCgscontrdp(VOS_UINT8 ucIndex)
{
    VOS_UINT16 usLength = 0;

    usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr, (VOS_CHAR *)pgucAtSndCodeAddr,
                                       "%s: (1-31)",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
    gstAtSendData.usBufLen = usLength;

    return AT_OK;
}


/*****************************************************************************
 �� �� ��  : At_TestCgtftrdp
 ��������  : cgtftrdp���Ժ���
 �������  : VOS_UINT8 ucIndex
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��1��8��
    ��    ��   : c00184452
    �޸�����   : PS�ں���Ŀ
*****************************************************************************/
VOS_UINT32 At_TestCgtftrdp(VOS_UINT8 ucIndex)
{
    VOS_UINT16 usLength = 0;

    usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)pgucAtSndCodeAddr, (VOS_CHAR *)pgucAtSndCodeAddr,
                                       "%s: (1-31)",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
    gstAtSendData.usBufLen = usLength;

    return AT_OK;
}

/*DTS2012041102190 : h00135900 start in 2011-04-11 AT�����ں�*/
/*****************************************************************************
 �� �� ��  : AT_TestChrgEnablePara
 ��������  : ^TCHRENABLE=? ��ѯ�Ƿ���Ҫ����
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��3��16��
    ��    ��   : �޾�ǿ
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 AT_TestChrgEnablePara(VOS_UINT8 ucIndex)
{
    /* DTS2012041102190 : h00135900 start in 2011-04-11 AT�����ں� */
    VOS_INT32                           lChargeEnable = 0;

    lChargeEnable = mdrv_misc_get_charge_state(); /* BSP_TBAT_CHRStGet()) */
    /* ֻ��TRUE/FLASE�ķ���ֵ����Ч�� */
    if((lChargeEnable == TRUE)||( lChargeEnable == FALSE))
    {
        gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                  (TAF_CHAR *)pgucAtSndCodeAddr,
                                                  (TAF_CHAR *)pgucAtSndCodeAddr,
                                                  "%s:%d",
                                                  "^TCHRENABLE",
                                                  lChargeEnable);
        return AT_OK;
    }
    /* DTS2012041102190 : h00135900 end in 2011-04-11 AT�����ں� */
    else
    {
        return AT_ERROR;
    }
}
/* DTS2012041102190 : h00135900 start in 2011-04-11 AT�����ں� */


/*****************************************************************************
 �� �� ��  : AT_TestCmutPara
 ��������  : +CMUT=? ��������
 �������  : ucIndex --- �û�����
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��9��12��
    ��    ��   : A00165503
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 AT_TestCmutPara(VOS_UINT8 ucIndex)
{
    VOS_UINT16                          usLength = 0;

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s: (0-1)",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    gstAtSendData.usBufLen = usLength;

    return AT_OK;
}



/* ���º������Ǵ���Ŀ������, ֻ�Ǵ�AtExtendCmd.c�ļ����ƶ������ļ����� */

/*****************************************************************************
 �� �� ��  : At_TestVtsPara
 ��������  : VTS���Ժ���
 �������  : VOS_UINT8 ucIndex
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��10��18��
    ��    ��   : f00179208
    �޸�����   : AT��ֲ��Ŀ
*****************************************************************************/
VOS_UINT32 At_TestVtsPara(VOS_UINT8 ucIndex)
{
    VOS_UINT16      usLength;

    usLength = 0;

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "+VTS: (0-9,A-D,*,#)");
    gstAtSendData.usBufLen = usLength;

    return AT_OK;
}

/*****************************************************************************
 �� �� ��  : At_TestCuus1Para
 ��������  : CUUS1���Ժ���
 �������  : VOS_UINT8 ucIndex
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��10��18��
    ��    ��   : f00179208
    �޸�����   : AT��ֲ��Ŀ
*****************************************************************************/
VOS_UINT32 At_TestCuus1Para(VOS_UINT8 ucIndex)
{
    VOS_UINT16      usLength;

    usLength = 0;

    usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                      (VOS_CHAR*)pgucAtSndCodeAddr,
                                      (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                      "+CUUS1:(0,1),(0,1),(0-6),(0-4),(0-3)");

    gstAtSendData.usBufLen = usLength;

    return AT_OK;
}

/*****************************************************************************
 �� �� ��  : At_TestCcwaPara
 ��������  : CCWA���Ժ���
 �������  : VOS_UINT8 ucIndex
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��10��18��
    ��    ��   : f00179208
    �޸�����   : AT��ֲ��Ŀ
*****************************************************************************/
VOS_UINT32 At_TestCcwaPara(VOS_UINT8 ucIndex)
{
    VOS_UINT16      usLength;

    usLength = 0;

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s: (0,1)",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    gstAtSendData.usBufLen = usLength;

    return AT_OK;
}

/*****************************************************************************
 �� �� ��  : At_TestCcfcPara
 ��������  : CCFC���Ժ���
 �������  : VOS_UINT8 ucIndex
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��10��18��
    ��    ��   : f00179208
    �޸�����   : AT��ֲ��Ŀ
*****************************************************************************/
VOS_UINT32 At_TestCcfcPara(VOS_UINT8 ucIndex)
{
    VOS_UINT16      usLength;

    usLength = 0;

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s: (0-5)",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    gstAtSendData.usBufLen = usLength;

    return AT_OK;
}

/*****************************************************************************
 �� �� ��  : At_TestCusdPara
 ��������  : CUSD���Ժ���
 �������  : VOS_UINT8 ucIndex
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��10��18��
    ��    ��   : f00179208
    �޸�����   : AT��ֲ��Ŀ
*****************************************************************************/
VOS_UINT32 At_TestCusdPara(VOS_UINT8 ucIndex)
{
    VOS_UINT16      usLength;

    usLength = 0;

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s: (0-2)",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    gstAtSendData.usBufLen = usLength;

    return AT_OK;
}

/*****************************************************************************
 �� �� ��  : At_TestCpwdPara
 ��������  : CPWD���Ժ���
 �������  : VOS_UINT8 ucIndex
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��10��18��
    ��    ��   : f00179208
    �޸�����   : AT��ֲ��Ŀ

  2.��    ��   : 2012��5��7��
    ��    ��   : A00165503
    �޸�����   : DTS2012050405948: +CPWD���������޶�
*****************************************************************************/
VOS_UINT32 At_TestCpwdPara(VOS_UINT8 ucIndex)
{
    VOS_UINT16      usLength;

    usLength = 0;

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s: (\"P2\",8),(\"SC\",8),(\"AO\",4),(\"OI\",4),(\"OX\",4),(\"AI\",4),(\"IR\",4),(\"AB\",4),(\"AG\",4),(\"AC\",4)",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    gstAtSendData.usBufLen = usLength;

    return AT_OK;
}

/*****************************************************************************
 �� �� ��  : At_TestClckPara
 ��������  : CLCK���Ժ���
 �������  : VOS_UINT8 ucIndex
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��10��18��
    ��    ��   : f00179208
    �޸�����   : AT��ֲ��Ŀ

  2.��    ��   : 2012��5��7��
    ��    ��   : A00165503
    �޸�����   : DTS2012050405948: +CLCK���������޶�
  3.��    ��   : 2015��4��7��
    ��    ��   : w00316404
    �޸�����   : M project A characeristic AT part
*****************************************************************************/
VOS_UINT32 At_TestClckPara(VOS_UINT8 ucIndex)
{
    VOS_UINT16      usLength;

    usLength = 0;

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s: (\"SC\",\"AO\",\"OI\",\"OX\",\"AI\",\"IR\",\"AB\",\"AG\",\"AC\",\"FD\",\"PN\",\"PU\",\"PP\")",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    gstAtSendData.usBufLen = usLength;

    return AT_OK;
}

/*****************************************************************************
 �� �� ��  : AT_TestCpbsPara
 ��������  : CPBS���Ժ���
 �������  : VOS_UINT8 ucIndex
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��10��28��
    ��    ��   : f62575
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 AT_TestCpbsPara(VOS_UINT8 ucIndex)
{
    if (TAF_SUCCESS == SI_PB_Query(gastAtClientTab[ucIndex].usClientId, 1))
    {
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }
}

/*****************************************************************************
 �� �� ��  : AT_TestCpbrPara
 ��������  : CPBR���Ժ���
 �������  : VOS_UINT8 ucIndex
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��10��28��
    ��    ��   : f62575
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 AT_TestCpbrPara(VOS_UINT8 ucIndex)
{
    if (TAF_SUCCESS == SI_PB_Query(gastAtClientTab[ucIndex].usClientId, 0))
    {
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }
}
/* Added by f62575 for AT Project, 2011-10-28, begin */
/*****************************************************************************
 �� �� ��  : AT_TestCnmaPara
 ��������  : +CNMA����Ĳ�������
 �������  : VOS_UINT8 ucIndex  �û�����
 �������  : ��
 �� �� ֵ  : VOS_UINT32 ATC������
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��10��18��
    ��    ��   : ��ӳ��/f62575
    �޸�����   : �����ɺ���
  2.��    ��   : 2013��2��20��
    ��    ��   : l60609
    �޸�����   : DSDA PHASE III
*****************************************************************************/
VOS_UINT32 AT_TestCnmaPara(VOS_UINT8 ucIndex)
{
    /* Modified by l60609 for DSDA Phase III, 2013-2-20, Begin */
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    pstSmsCtx = AT_GetModemSmsCtxAddrFromClientId(ucIndex);

    if(AT_CMGF_MSG_FORMAT_PDU == pstSmsCtx->enCmgfMsgFormat)    /* TEXT��ʽ������� */
    {
        gstAtSendData.usBufLen = (TAF_UINT16)VOS_sprintf_s((TAF_CHAR*)pgucAtSndCodeAddr,
                                          AT_CMD_MAX_LEN + 20 - 3,
                                          "%s: (0-2)",
                                          g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
    }
    else
    {
        gstAtSendData.usBufLen = 0;
    }
    /* Modified by l60609 for DSDA Phase III, 2013-2-20, End */

    return AT_OK;
}

/*****************************************************************************
 �� �� ��  : AT_TestCpasPara
 ��������  : CPAS��������
 �������  : VOS_UINT8 ucIndex
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��10��28��
    ��    ��   : f62575
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 AT_TestCpasPara(VOS_UINT8 ucIndex)
{
    gstAtSendData.usBufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                    (TAF_CHAR *)pgucAtSndCodeAddr,
                                                    (TAF_CHAR *)pgucAtSndCodeAddr,
                                                    "%s: (0-5)",
                                                    g_stParseContext[ucIndex].pstCmdElement->pszCmdName);


    return AT_OK;
}
/*****************************************************************************
 �� �� ��  : AT_TestCscbPara
 ��������  : +CSCB����Ĳ�������
 �������  : VOS_UINT8 ucIndex  �û�����
 �������  : ��
 �� �� ֵ  : VOS_UINT32 ATC������
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��10��18��
    ��    ��   : ��ӳ��/f62575
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 AT_TestCscbPara(VOS_UINT8 ucIndex)
{
    gstAtSendData.usBufLen = (VOS_UINT16)VOS_sprintf_s((VOS_CHAR*)pgucAtSndCodeAddr,
                                        AT_CMD_MAX_LEN + 20 - 3,
                                        "%s:(0,1)",
                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
    return AT_OK;
}

/*****************************************************************************
 �� �� ��  : AT_TestCpmsPara
 ��������  : +CPMS����Ĳ�������
 �������  : VOS_UINT8 ucIndex
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��10��18��
    ��    ��   : ��ӳ��/f62575
    �޸�����   : �����ɺ���
  2.��    ��   : 2013��2��20��
    ��    ��   : l60609
    �޸�����   : DSDA PHASE III
*****************************************************************************/
VOS_UINT32 AT_TestCpmsPara(VOS_UINT8 ucIndex)
{
    VOS_UINT16                          usLength;
    /* Modified by l60609 for DSDA Phase III, 2013-2-20, Begin */
    AT_MODEM_SMS_CTX_STRU              *pstSmsCtx = VOS_NULL_PTR;

    usLength  = 0;
    pstSmsCtx = AT_GetModemSmsCtxAddrFromClientId(ucIndex);

    if (MN_MSG_ME_STORAGE_ENABLE == pstSmsCtx->enMsgMeStorageStatus)
    {
        usLength = (VOS_UINT16)VOS_sprintf_s((VOS_CHAR*)pgucAtSndCodeAddr,
                    AT_CMD_MAX_LEN + 20 - 3,
                    "%s: (\"SM\",\"ME\"),(\"SM\",\"ME\"),(\"SM\",\"ME\")",
                    g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
    }
    else
    {
        usLength = (VOS_UINT16)VOS_sprintf_s((VOS_CHAR*)pgucAtSndCodeAddr,
                    AT_CMD_MAX_LEN + 20 - 3,
                    "%s: (\"SM\"),(\"SM\"),(\"SM\")",
                    g_stParseContext[ucIndex].pstCmdElement->pszCmdName);
    }

    gstAtSendData.usBufLen = usLength;
    /* Modified by l60609 for DSDA Phase III, 2013-2-20, End */

    return AT_OK;
}

/* Added by f62575 for AT Project, 2011-10-28, end */

/*****************************************************************************
 �� �� ��  : AT_TestCsgIdSearchPara
 ��������  : csg�б�������ѯ����
 �������  : ucIndex - ����
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��9��6��
    ��    ��   : z00161729
    �޸�����   : ֧��LTE CSG��������
*****************************************************************************/
VOS_UINT32 AT_TestCsgIdSearchPara(VOS_UINT8 ucIndex)
{
    /* �˼�ͨ����󻺴�4K,��Ҫ�ֶβ�ѯ�ֶ��ϱ���� */
    TAF_MMA_PLMN_LIST_PARA_STRU         stPlmnListPara;

    stPlmnListPara.usQryNum    = TAF_MMA_MAX_CSG_ID_LIST_NUM;
    stPlmnListPara.usCurrIndex = 0;

    if (VOS_TRUE == TAF_MMA_CsgListSearchReq(WUEPS_PID_AT, gastAtClientTab[ucIndex].usClientId, 0, &stPlmnListPara))
    {
        /* ���õ�ǰ�������� */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CSG_LIST_SEARCH;

        /* ������������״̬ */
        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;
}


/*****************************************************************************
 �� �� ��  : At_TestCopsPara
 ��������  : cops���Ժ���
 �������  : VOS_UINT8 ucIndex
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��10��18��
    ��    ��   : c00173809
    �޸�����   : AT��ֲ��Ŀ
  2.��    ��   : 2011��10��24��
    ��    ��   : f00179208
    �޸�����   : AT��ֲ��Ŀ:�޸��б��ѵĶ�ʱ������ʱ��
  3.��    ��   : 2015��3��4��
    ��    ��   : b00269685
    �޸�����   : ��Ϊ�ֶ��ϱ�
*****************************************************************************/
VOS_UINT32 At_TestCopsPara(TAF_UINT8 ucIndex)
{
    TAF_MMA_PLMN_LIST_PARA_STRU             stPlmnListPara;

    stPlmnListPara.usQryNum    = TAF_MMA_MAX_PLMN_NAME_LIST_NUM;
    stPlmnListPara.usCurrIndex = 0;

    if (VOS_TRUE == At_CheckCurrRatModeIsCL((VOS_UINT8)(gastAtClientTab[ucIndex].usClientId)))
    {
        return AT_CME_OPERATION_NOT_ALLOWED_IN_CL_MODE;
    }

    if (VOS_TRUE == Taf_PhonePlmnList(WUEPS_PID_AT, gastAtClientTab[ucIndex].usClientId, 0, &stPlmnListPara))
    {
        /* ���õ�ǰ�������� */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_COPS_TEST;

        /* ������������״̬ */
        return AT_WAIT_ASYNC_RETURN;
    }
    else
    {
        return AT_ERROR;
    }

}
/*****************************************************************************
 �� �� ��  : At_TestCpolPara
 ��������  : cpol���Ժ���
 �������  : VOS_UINT8 ucIndex
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��10��18��
    ��    ��   : c00173809
    �޸�����   : AT��ֲ��Ŀ
  2.��    ��   : 2013��2��22��
    ��    ��   : l60609
    �޸�����   : DSDA PHASE III
  3.��    ��   : 2015��02��05��
    ��    ��   : y00307564
    �޸�����   : Itertion 8 �޸�
*****************************************************************************/
VOS_UINT32 At_TestCpolPara(VOS_UINT8 ucIndex)
{
    VOS_UINT32                          ulRst;

    /* Modified by l60609 for DSDA Phase III, 2013-2-22, Begin */
    AT_MODEM_NET_CTX_STRU              *pstNetCtx = VOS_NULL_PTR;

    /* ������� */
    if (AT_CMD_OPT_TEST_CMD != g_stATParseCmd.ucCmdOptType)
    {
        return AT_ERROR;
    }

    pstNetCtx = AT_GetModemNetCtxAddrFromClientId(ucIndex);

    ulRst = TAF_MMA_TestCpolReq(WUEPS_PID_AT,
                                gastAtClientTab[ucIndex].usClientId,
                                0,
                                pstNetCtx->enPrefPlmnType);
    /* Modified by l60609 for DSDA Phase III, 2013-2-22, End */

    if (VOS_TRUE == ulRst)
    {
        /* ����ATģ��ʵ���״̬Ϊ�ȴ��첽���� */
        gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CPOL_TEST;

        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;
}

/*****************************************************************************
 �� �� ��  : At_TestCgeqnegPara
 ��������  : +CGEQNEQ����Ĳ�������
 �������  : VOS_UINT8 ucIndex
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��10��24��
    ��    ��   : ����/00179208
    �޸�����   : �����ɺ���
  2.��    ��   : 2011��12��19��
    ��    ��   : �����/00130025
    �޸�����   : ���ⵥDTS2011120604361��CGEQNEG��������ش���
*****************************************************************************/
VOS_UINT32 At_TestCgeqnegPara(VOS_UINT8 ucIndex)
{

    /*CGEQNEG�Ĳ��������CGACT�Ĳ�ѯ����Ĺ�������,��Ҫ��ȡ��ǰ��CID�ļ���
    ���,��TAF��ѯ��ǰ��CID�ļ������*/
    if ( VOS_OK != TAF_PS_GetPdpContextState(WUEPS_PID_AT,
                                             AT_PS_BuildExClientId(gastAtClientTab[ucIndex].usClientId),
                                             0) )
    {
        return AT_ERROR;
    }

    /*���Ͳ�ѯ��Ϣ�ɹ���,������״̬��ΪCGEQNEG*/
    if(AT_SUCCESS != At_StartTimer(AT_QRY_PARA_TIME,ucIndex))
    {
        AT_ERR_LOG("At_TestParaCmd:ERROR:Start Timer");
        return AT_ERROR;
    }

    /* ���õ�ǰ�������� */
    gastAtClientTab[ucIndex].CmdCurrentOpt = AT_CMD_CGEQNEG_TEST;
    return AT_WAIT_ASYNC_RETURN;    /* ������������״̬ */

    /*��������TAF�¼��ϱ��ĵط�,��ȡ��ǰ��ЩCID������,����ӡ��ǰ��ЩCID������.*/


}


/*****************************************************************************
 �� �� ��  : At_TestCgeqreqPara
 ��������  : +CGEQREQ����Ĳ�������
 �������  : VOS_UINT8 ucIndex
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��10��24��
    ��    ��   : ����/00179208
    �޸�����   : �����ɺ���
  2.��    ��   : 2011��11��24��
    ��    ��   : f62575
    �޸�����   : DTS2011112307607,����AT�ںϷ�����֧��42M��������
  3.��    ��   : 2011��12��19��
    ��    ��   : c00173809
    �޸�����   : PS�ں���Ŀ
  4.��    ��   : 2012��03��17��
    ��    ��   : w00199382
    �޸�����   : DTS2012031603201������������
*****************************************************************************/
VOS_UINT32 At_TestCgeqreqPara(VOS_UINT8 ucIndex)
{
    VOS_UINT16      usLength;

    usLength = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                        (VOS_CHAR *)pgucAtSndCodeAddr,(VOS_CHAR *)pgucAtSndCodeAddr,
                                        "%s: \"IP\",%s%s",
                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                        CGEQREQ_TEST_CMD_PARA_STRING,
                                        gaucAtCrLf);


    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                        (VOS_CHAR *)pgucAtSndCodeAddr,(VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                        "%s: \"IPV6\",%s%s",
                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                        CGEQREQ_TEST_CMD_PARA_STRING,
                                        gaucAtCrLf);


    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                        (VOS_CHAR *)pgucAtSndCodeAddr,(VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                        "%s: \"IPV4V6\",%s",
                                        g_stParseContext[ucIndex].pstCmdElement->pszCmdName,
                                        CGEQREQ_TEST_CMD_PARA_STRING);

    gstAtSendData.usBufLen = usLength;

    return AT_OK;
}

/*****************************************************************************
 �� �� ��  : At_TestCgansPara
 ��������  : +CGANS���Ժ���
 �������  : VOS_UINT8 ucIndex
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��10��24��
    ��    ��   : f00179208
    �޸�����   : AT��ֲ��Ŀ
*****************************************************************************/
VOS_UINT32 At_TestCgansPara(VOS_UINT8 ucIndex)
{
    VOS_UINT16      usLength;

    usLength = 0;

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s: (0,1),(\"NULL\")",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    gstAtSendData.usBufLen = usLength;

    return AT_OK;
}

/* Added by l60609 for V3R3 PPP RPOJECT 2013-06-07, Begin */
/*****************************************************************************
 �� �� ��  : At_TestCgansExtPara
 ��������  : ^CGANS����Ĳ��Ժ���
 �������  : VOS_UINT8 ucIndex
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��6��7��
    ��    ��   : l60609
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 At_TestCgansExtPara(VOS_UINT8 ucIndex)
{
    VOS_UINT16      usLength;

    usLength = 0;

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s: (0,1)",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    gstAtSendData.usBufLen = usLength;

    return AT_OK;
}
/* Added by l60609 for V3R3 PPP RPOJECT 2013-06-07, End*/

/*****************************************************************************
 �� �� ��  : At_TestCgdnsPara
 ��������  : +CGDNS���Ժ���
 �������  : VOS_UINT8 ucIndex
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��10��24��
    ��    ��   : f00179208
    �޸�����   : AT��ֲ��Ŀ
  2.��    ��   : 2012��4��30��
    ��    ��   : z60575
    �޸�����   : DTS2012031400889,CGDNS������Ҫ���ؼ����CID
*****************************************************************************/
VOS_UINT32 At_TestCgdnsPara(VOS_UINT8 ucIndex)
{



    /* CGDNS�Ĳ��������CGCMODE�ķ���ֵ��ͬ��ʹ����ͬ�ĺ��� */
    return At_TestCgeqnegPara(ucIndex);

}


/*****************************************************************************
 �� �� ��  : At_TestCgdataPara
 ��������  : +CGDATA���Ժ���
 �������  : VOS_UINT8 ucIndex
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��10��25��
    ��    ��   : f00179208
    �޸�����   : AT��ֲ��Ŀ
*****************************************************************************/
VOS_UINT32 At_TestCgdataPara(VOS_UINT8 ucIndex)
{
    VOS_UINT16      usLength;

    usLength = 0;

    usLength += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr + usLength,
                                       "%s: (\"NULL\")",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    gstAtSendData.usBufLen = usLength;

    return AT_OK;
}


/*****************************************************************************
 �� �� ��  : AT_TestCbgPara
 ��������  : �����������
 �������  : VOS_UINT8 ucIndex
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��12��28��
    ��    ��   : z00214637
    �޸�����   : AT^CBG �����������
*****************************************************************************/
VOS_UINT32 AT_TestCbgPara( VOS_UINT8 ucIndex )
{
    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       (VOS_CHAR *)pgucAtSndCodeAddr,
                                       "%s: (0,1)",
                                       g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    return AT_OK;
}

/*****************************************************************************
 �� �� ��  : AT_TestNCellMonitorPara
 ��������  : ^NCELLMONITOR�����������
 �������  : VOS_UINT8 ucIndex
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��06��04��
    ��    ��   : s00217060
    �޸�����   : V9R1_SVLTE: ��������
*****************************************************************************/
VOS_UINT32 AT_TestNCellMonitorPara(VOS_UINT8 ucIndex)
{

    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    "%s: (0,1)",
                                                    g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    return AT_OK;
}

/*****************************************************************************
 �� �� ��  : AT_TestRefclkfreqPara
 ��������  : AT^REFCLKFREQ�����������
 �������  : ucIndex
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��07��22��
    ��    ��   : l00198894
    �޸�����   : V9R1 AGPS
*****************************************************************************/
VOS_UINT32 AT_TestRefclkfreqPara(VOS_UINT8 ucIndex)
{
    /* ͨ����� */
    if (VOS_FALSE == AT_IsApPort(ucIndex))
    {
        return AT_ERROR;
    }

    /* ��ӡ���^REFCLKFREQ֧�ֵĲ������÷�Χ */
    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                   (VOS_CHAR*)pgucAtSndCodeAddr,
                                                   (VOS_CHAR*)pgucAtSndCodeAddr,
                                                   "%s: (0,1)",
                                                   g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    return AT_OK;
}

/*****************************************************************************
 �� �� ��  : AT_TestCiregPara
 ��������  : +CIREG=?�����������
 �������  : VOS_UINT8 ucIndex
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��09��18��
    ��    ��   : Y00213812
    �޸�����   : ��������

*****************************************************************************/
VOS_UINT32 AT_TestCiregPara(VOS_UINT8 ucIndex)
{

    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    "%s: (0-2)",
                                                    g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    return AT_OK;
}

/*****************************************************************************
 �� �� ��  : AT_TestCirepPara
 ��������  : +CIREP=?�����������
 �������  : VOS_UINT8 ucIndex
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2013��09��18��
    ��    ��   : Y00213812
    �޸�����   : ��������

*****************************************************************************/
VOS_UINT32 AT_TestCirepPara(VOS_UINT8 ucIndex)
{

    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    "%s: (0,1)",
                                                    g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    return AT_OK;
}


/*****************************************************************************
 �� �� ��  : AT_TestUECenterPara
 ��������  : +CEUS=?
 �������  : ucIndex --- �û�����
 �������  : ��
 �� �� ֵ  : AT_XXX  --- ATC������
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2015��09��07��
    ��    ��   : lwx277467
    �޸�����   : �����ɺ���
*****************************************************************************/
VOS_UINT32 AT_TestUECenterPara(VOS_UINT8 ucIndex)
{
    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    "%s: (0,1)",
                                                    g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    return AT_OK;
}

/*****************************************************************************
 �� �� ��  : At_TestFclass
 ��������  : +FCLASS=?
 �������  : ucIndex --- �˿�����
 �������  : ��
 �� �� ֵ  : AT_XXX  --- ATC������
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��7��11��
    ��    ��   : l00373346
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 At_TestFclass(VOS_UINT8 ucIndex)
{
    AT_WARN_LOG("At_TestFclass: Not support this command!");
    (VOS_VOID)ucIndex;
    return AT_ERROR;
}

/*****************************************************************************
 �� �� ��  : At_TestGci
 ��������  : +GCI=?
 �������  : ucIndex --- �˿�����
 �������  : ��
 �� �� ֵ  : AT_XXX  --- ATC������
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��7��11��
    ��    ��   : l00373346
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 At_TestGci(VOS_UINT8 ucIndex)
{
    AT_WARN_LOG("At_TestGci: Not support this command!");
    (VOS_VOID)ucIndex;
    return AT_ERROR;
}


/* Added by Y00213812 for Spirnt ����, 2017-3-25, begin */
/*****************************************************************************
 �� �� ��  : AT_TestMipPara
 ��������  : ����^CMIIP��������
 �������  : VOS_UINT8 ucIndex
 �������  : ��
 �� �� ֵ  : VOS_UINT32
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2017��3��25��
    ��    ��   : Y00213812
    �޸�����   : �����ɺ���

*****************************************************************************/
VOS_UINT32 AT_TestMipPara(VOS_UINT8 ucIndex)
{
    gstAtSendData.usBufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    (VOS_CHAR *)pgucAtSndCodeAddr,
                                                    "%s: (0-2)",
                                                    g_stParseContext[ucIndex].pstCmdElement->pszCmdName);

    return AT_OK;
}

/* Added by Y00213812 for Spirnt ����, 2017-3-25, end */


