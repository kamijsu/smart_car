//==========================================================================
//文件名称：dac.h
//功能概要：K64 DAC底层驱动程序头文件
//==========================================================================

#ifndef _DAC_H
#define _DAC_H

//包含公共头文件
#include "common.h"

//定义DAC模块号
#define DAC_MOD0	(0)		//引脚号:27
#define DAC_MOD1	(1)		//K64 100引脚LQFP封装无该模块

//定义DAC模块参考电压源
#define DAC_REF_VTG_VREFOUT		(0)		//VREF_OUT
#define DAC_REF_VTG_VDDA		(1)		//VDDA

void dac_init(uint8 mod, uint8 ref_vtg);

void dac_set_buffer_val(uint8 mod, uint8 no, uint16 val);

#endif
