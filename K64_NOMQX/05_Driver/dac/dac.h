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

//定义DAC模块缓冲区工作模式
#define DAC_BUFFER_MODE_NORMAL			(0)		//普通模式
#define DAC_BUFFER_MODE_SWING			(1)		//摇摆模式
#define DAC_BUFFER_MODE_ONE_TIME_SCAN	(2)		//单次扫描模式

//定义DAC模块缓冲区水印选择
#define DAC_WATERMARK_WORD_1	(0)		//水印距离缓冲区指针位置上限1个字
#define DAC_WATERMARK_WORD_2	(1)		//水印距离缓冲区指针位置上限2个字
#define DAC_WATERMARK_WORD_3	(2)		//水印距离缓冲区指针位置上限3个字
#define DAC_WATERMARK_WORD_4	(3)		//水印距离缓冲区指针位置上限4个字

//定义DAC模块触发转换方式
#define DAC_TRIGGER_HARDWARE	(0)		//硬件触发
#define DAC_TRIGGER_SOFTWARE	(1)		//软件触发

void dac_init(uint8 mod, uint8 ref_vtg);

void dac_set_buffer_val(uint8 mod, uint8 index, uint16 val);

uint16 dac_get_buffer_val(uint8 mod, uint8 index);

void dac_enable_buffer(uint8 mod, uint8 buffer_mode, uint8 max_index,
		uint8 watermark, uint8 trigger);

void dac_disable_buffer(uint8 mod);

void dac_software_trigger(uint8 mod);

void dac_set_buffer_index(uint8 mod, uint8 index);

uint8 dac_get_buffer_index(uint8 mod);

#endif
