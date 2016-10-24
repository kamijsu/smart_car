/*
 * light.c
 *
 *  Created on: 2015年4月1日
 *      Author: SUMCU
 */

#include "light.h"

//============================================================================
//函数名称：light_init
//函数参数：port_pin：(端口号)|(引脚号)（如：(PORTB)|(5) 表示为B口5号脚）
//       state：设定小灯状态。由宏定义。
//函数返回：无
//功能概要：指示灯驱动初始化。
//============================================================================
void light_init(uint16_t port_pin, uint8_t state)
{
	gpio_init(port_pin, GPIO_OUTPUT, state);

}

//===========================================================================
//函数名称：light_control
//函数参数：port_pin：(端口号)|(引脚号)（如：(PORTB)|(5) 表示为B口5号脚）
//       state：设定小灯状态。由light.h中宏定义。
//函数返回：无
//功能概要：控制指示灯亮暗。
//===========================================================================
void light_control(uint16_t port_pin, uint8_t state)
{
	gpio_set(port_pin, state);
}

//===========================================================================
//函数名称：light_change
//函数参数：port_pin：(端口号)|(引脚号)（如：(PORTB)|(5) 表示为B口5号脚）
//函数返回：无
//功能概要：切换指示灯亮暗。
//===========================================================================
void light_change(uint16_t port_pin)
{
	gpio_reverse(port_pin);
}

