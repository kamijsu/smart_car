//==============================================================
//文件名称：reed_switch.h
//功能概要：干簧管头文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//==============================================================

#ifndef _REED_SWITCH_H
#define _REED_SWITCH_H

#include "common.h"    //包含公共要素头文件
#include "gpio.h"

//干簧管输入输出引脚定义
#define REED_SWITCH_INPUT  (COM_PORTD|2)
#define REED_SWITCH_OUTPUT (COM_PORTD|3)

//==============================================================
//函数名称：reed_switch_init
//函数返回：无
//参数说明：无
//功能概要：干簧管初始化。调用GPIO构件完成对干簧管的初始化，无需参数。
//==============================================================
void reed_switch_init(void);

//==============================================================
//函数名称：reed_switch_enable_int
//函数返回：无
//参数说明：无
//功能概要：干簧管输入中断使能。无需参数。
//==============================================================
void reed_switch_enable_int(void);

#endif /* _REED_SWITCH_H */
