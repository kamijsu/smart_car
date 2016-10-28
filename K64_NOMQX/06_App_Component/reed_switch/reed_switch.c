//==============================================================
//文件名称：reed_switch.c
//功能概要：干簧管源文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//==============================================================

#include "reed_switch.h"

//==============================================================
//函数名称：reed_switch_init
//函数返回：无
//参数说明：无
//功能概要：干簧管初始化。调用GPIO构件完成对干簧管的初始化，无需参数。
//==============================================================
void reed_switch_init(void)
{
	gpio_init(REED_SWITCH_OUTPUT,GPIO_OUTPUT,1);
	gpio_init(REED_SWITCH_INPUT,GPIO_INPUT,0);
	gpio_pull(REED_SWITCH_INPUT,0);
}

//==============================================================
//函数名称：reed_switch_enable_int
//函数返回：无
//参数说明：无
//功能概要：干簧管输入中断使能。无需参数。
//==============================================================
void reed_switch_enable_int(void)
{
	gpio_enable_int(REED_SWITCH_INPUT,GPIO_INT_RISING_EDGE);
}
