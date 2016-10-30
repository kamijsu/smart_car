//==========================================================================
//文件名称：ftm.c
//功能概要：K64 FTM底层驱动程序源文件
//==========================================================================

#include "ftm.h"

//FTM各模块基地址
static FTM_Type * const ftm_table[] = { FTM0, FTM1, FTM2, FTM3 };
//FTM各模块中断请求号
static const IRQn_Type ftm_irq_table[] = { FTM0_IRQn, FTM1_IRQn, FTM2_IRQn,
		FTM3_IRQn };

