//==========================================================================
//�ļ����ƣ�sdhc.h
//���ܸ�Ҫ��K64 SDHC�ײ���������ͷ�ļ�
//==========================================================================

#ifndef _SDHC_H
#define _SDHC_H

//��������ͷ�ļ�
#include "common.h"

//����SDHCģ������ŵ�����
#define SDHC_DCLK_PIN	(COM_PORTE|2)
#define SDHC_CMD_PIN	(COM_PORTE|3)
#define SDHC_D0_PIN		(COM_PORTE|1)
#define SDHC_D1_PIN		(COM_PORTE|0)
#define SDHC_D2_PIN		(COM_PORTE|5)
#define SDHC_D3_PIN		(COM_PORTE|4)

//����SDHCģ�������õ����źţ�������Ӧ��PCR��MUXֵ
#ifdef SDHC_DCLK_PIN
#define SDHC_DCLK_PCR_MUX	(4)
#endif
#ifdef SDHC_CMD_PIN
#define SDHC_CMD_PCR_MUX	(4)
#endif
#ifdef SDHC_D0_PIN
#define SDHC_D0_PCR_MUX		(4)
#endif
#ifdef SDHC_D1_PIN
#define SDHC_D1_PCR_MUX		(4)
#endif
#ifdef SDHC_D2_PIN
#define SDHC_D2_PCR_MUX		(4)
#endif
#ifdef SDHC_D3_PIN
#define SDHC_D3_PCR_MUX		(4)
#endif

#endif
