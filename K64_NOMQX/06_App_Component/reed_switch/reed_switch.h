//==========================================================================
//文件名称：reed_switch.h
//功能概要：干簧管应用驱动程序头文件
//==========================================================================

#ifndef _REED_SWITCH_H
#define _REED_SWITCH_H

#include "common.h"
#include "gpio.h"

//定义干簧管输入输出引脚
#define REED_SWITCH_INPUT	(COM_PORTD|2)	//D2，输入引脚
#define REED_SWITCH_OUTPUT	(COM_PORTD|3)	//D3，输出引脚

//定义干簧管输入引脚上下拉状态，可以更改该配置
#define REED_SWITCH_INPUT_PULL	GPIO_LEVEL_LOW	//拉低电平

//根据干簧管输入引脚上下拉状态决定输出引脚电平状态以及中断类型
#ifdef REED_SWITCH_INPUT_PULL
#if(REED_SWITCH_INPUT_PULL == GPIO_LEVEL_LOW)				//输入拉低电平
#define REED_SWITCH_OUTPUT_LEVEL	GPIO_LEVEL_HIGH			//输出高电平
#define REED_SWITCH_INT_TYPE		GPIO_INT_RISING_EDGE	//上升沿触发
#elif(REED_SWITCH_INPUT_PULL == GPIO_LEVEL_HIGH)			//输入拉高电平
#define REED_SWITCH_OUTPUT_LEVEL	GPIO_LEVEL_LOW			//输出低电平
#define REED_SWITCH_INT_TYPE		GPIO_INT_FALLING_EDGE	//下降沿触发
#endif
#endif

//==========================================================================
//函数名称: reed_switch_init
//函数返回: 无
//参数说明: 无
//功能概要: 初始化干簧管，相应配置在reed_switch.h中
//==========================================================================
void reed_switch_init();

//==========================================================================
//函数名称: reed_switch_enable_int
//函数返回: 无
//参数说明: 无
//功能概要: 使能干簧管输入中断，当有磁铁靠近干簧管时会产生中断，
//         相应的中断服务程序由输入引脚的端口决定，相应配置在reed_switch.h中
//==========================================================================
void reed_switch_enable_int();

//==========================================================================
//函数名称: reed_switch_disable_int
//函数返回: 无
//参数说明: 无
//功能概要: 关闭干簧管输入中断
//==========================================================================
void reed_switch_disable_int();

//==========================================================================
//函数名称: reed_switch_get_int
//函数返回: true:产生中断; false:未产生中断
//参数说明: 无
//功能概要: 获取干簧管输入中断标志
//==========================================================================
bool reed_switch_get_int();

//==========================================================================
//函数名称: reed_switch_clear_int
//函数返回: 无
//参数说明: 无
//功能概要: 清除干簧管输入中断标志
//==========================================================================
void reed_switch_clear_int();

#endif
