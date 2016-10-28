//==========================================================================
//文件名称：gpio.h
//功能概要：K64 GPIO底层驱动程序头文件
//==========================================================================

#ifndef _GPIO_H
#define _GPIO_H

//包含公共头文件
#include "common.h"

//定义GPIO模块电平状态
#define GPIO_LEVEL_LOW		(0)		//低电平
#define GPIO_LEVEL_HIGH		(1)		//高电平
#define GPIO_LEVEL_UNKNOWN	(2)		//未知电平，仅在配置为输入时有效

//定义GPIO模块引脚方向
#define GPIO_INPUT		(0)		//输入
#define GPIO_OUTPUT		(1)		//输出

//定义GPIO模块中断类型
#define GPIO_INT_LOW_LEVEL		(8)		//低电平触发
#define GPIO_INT_HIGH_LEVEL		(12)	//高电平触发
#define GPIO_INT_RISING_EDGE	(9)		//上升沿触发
#define GPIO_INT_FALLING_EDGE	(10)	//下降沿触发
#define GPIO_INT_DOUBLE_EDGE	(11)	//双边沿触发

//定义GPIO模块中断标志
#define GPIO_INT_TRUE	(1)		//产生中断
#define GPIO_INT_FALSE	(0)		//未产生中断

//定义GPIO模块驱动能力
#define GPIO_DRIVE_LOW	(0)		//正常驱动能力
#define GPIO_DRIVE_HIGH	(1)		//高驱动能力

//==========================================================================
//函数名称: gpio_init
//函数返回: 无
//参数说明: port_pin:(端口号)|(引脚号)，具体见common.h中宏定义
//         dir:引脚方向，GPIO_INPUT:配置为输入; GPIO_OUTPUT:配置为输出
//         status:输出时为引脚电平状态，输入时为引脚上下拉状态，GPIO_LEVEL_LOW:低电平;
//                GPIO_LEVEL_HIGH:高电平; GPIO_LEVEL_UNKNOWN:未知电平，仅在配置为输入时有效
//功能概要: 初始化指定端口引脚为GPIO功能
//==========================================================================
void gpio_init(uint16 port_pin, uint8 dir, uint8 status);

//==========================================================================
//函数名称: gpio_set
//函数返回: 无
//参数说明: port_pin:(端口号)|(引脚号)，具体见common.h中宏定义
//         status:引脚电平状态，GPIO_LEVEL_LOW:低电平; GPIO_LEVEL_HIGH:高电平
//功能概要: 当引脚配置为输出时，设定引脚状态为指定状态
//==========================================================================
void gpio_set(uint16 port_pin, uint8 status);

//==========================================================================
//函数名称: gpio_reverse
//函数返回: 无
//参数说明: port_pin:(端口号)|(引脚号)，具体见common.h中宏定义
//功能概要: 当引脚配置为输出时，反转其输出状态
//==========================================================================
void gpio_reverse(uint16 port_pin);

//==========================================================================
//函数名称: gpio_drive_strength
//函数返回: 无
//参数说明: port_pin:(端口号)|(引脚号)，具体见common.h中宏定义
//         status:引脚的驱动能力，GPIO_DRIVE_LOW:正常驱动能力(5mA);
//                GPIO_DRIVE_HIGH:高驱动能力(18mA)
//功能概要: 当引脚配置为输出时，设定其驱动能力
//备注:     K64芯片只有PTD7可以被设置为高驱动能力
//==========================================================================
void gpio_drive_strength(uint16 port_pin, uint8 status);

//==========================================================================
//函数名称: gpio_pull
//函数返回: 无
//参数说明: port_pin:(端口号)|(引脚号)，具体见common.h中宏定义
//         status:引脚上下拉状态，GPIO_LEVEL_LOW:低电平; GPIO_LEVEL_HIGH:高电平;
//                GPIO_LEVEL_UNKNOWN:未知电平，即关闭上下拉电阻
//功能概要: 当引脚配置为输入时，设定其上下拉状态
//==========================================================================
void gpio_pull(uint16 port_pin, uint8 status);

//==========================================================================
//函数名称: gpio_get
//函数返回: GPIO_LEVEL_LOW(0):低电平; GPIO_LEVEL_HIGH(1):高电平
//参数说明: port_pin:(端口号)|(引脚号)，具体见common.h中宏定义
//功能概要: 当引脚配置为输入时，获取其电平状态
//==========================================================================
uint8 gpio_get(uint16 port_pin);

//==========================================================================
//函数名称: gpio_enable_int
//函数返回: 无
//参数说明: port_pin:(端口号)|(引脚号)，具体见common.h中宏定义
//         int_type:引脚中断类型，具体如下：
//                  GPIO_INT_LOW_LEVEL		(8)		//低电平触发
//                  GPIO_INT_HIGH_LEVEL		(12)	//高电平触发
//                  GPIO_INT_RISING_EDGE	(9)		//上升沿触发
//                  GPIO_INT_FALLING_EDGE	(10)	//下降沿触发
//                  GPIO_INT_DOUBLE_EDGE	(11)	//双边沿触发
//功能概要: 当引脚配置为输入时，根据中断类型开启该引脚中断
//==========================================================================
void gpio_enable_int(uint16 port_pin, uint8 int_type);

//==========================================================================
//函数名称: gpio_disable_int
//函数返回: 无
//参数说明: port_pin:(端口号)|(引脚号)，具体见common.h中宏定义
//功能概要: 当引脚配置为输入时，关闭该引脚中断
//==========================================================================
void gpio_disable_int(uint16 port_pin);

//==========================================================================
//函数名称: gpio_get_int
//函数返回: GPIO_INT_TRUE(1):产生中断; GPIO_INT_FALSE(0):未产生中断
//参数说明: port_pin:(端口号)|(引脚号)，具体见common.h中宏定义
//功能概要: 当引脚配置为输入时，获取其中断标志
//==========================================================================
uint8 gpio_get_int(uint16 port_pin);

//==========================================================================
//函数名称: gpio_clear_int
//函数返回: 无
//参数说明: port_pin:(端口号)|(引脚号)，具体见common.h中宏定义
//功能概要: 当引脚配置为输入时，清除其中断标志
//==========================================================================
void gpio_clear_int(uint16 port_pin);

#endif
