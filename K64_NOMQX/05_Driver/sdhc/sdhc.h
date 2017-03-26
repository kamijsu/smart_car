//==========================================================================
//文件名称：sdhc.h
//功能概要：K64 SDHC底层驱动程序头文件
//==========================================================================

#ifndef _SDHC_H
#define _SDHC_H

//包含公共头文件
#include "common.h"

//定义SDHC模块各引脚的设置
#define SDHC_DCLK_PIN	(COM_PORTE|2)
#define SDHC_CMD_PIN	(COM_PORTE|3)
#define SDHC_D0_PIN		(COM_PORTE|1)
#define SDHC_D1_PIN		(COM_PORTE|0)
#define SDHC_D2_PIN		(COM_PORTE|5)
#define SDHC_D3_PIN		(COM_PORTE|4)

//根据SDHC模块所设置的引脚号，定义相应的PCR的MUX值
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
