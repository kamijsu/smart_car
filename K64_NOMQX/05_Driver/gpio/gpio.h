//==========================================================================
//文件名称：gpio.h
//功能概要：K64 GPIO底层驱动程序头文件
//==========================================================================

#ifndef _GPIO_H
#define _GPIO_H

//包含公共头文件
#include "common.h"

// GPIO引脚方向宏定义
#define GPIO_IN     (0)
#define GPIO_OUTPUT (1)
// GPIO引脚中断类型宏定义
#define LOW_LEVEL    (8)      //低电平触发
#define HIGH_LEVEL   (12)     //高电平触发
#define RISING_EDGE  (9)      //上升沿触发
#define FALLING_EDGE (10)     //下降沿触发
#define DOUBLE_EDGE  (11)     //双边沿触发

//各端口基地址放入常数数据组PORT_ARR[0]~PORT_ARR[4]中
static const PORT_MemMapPtr PORT_ARR[] =
{ PORTA_BASE_PTR, PORTB_BASE_PTR,
PORTC_BASE_PTR, PORTD_BASE_PTR, PORTE_BASE_PTR };
//GPIO口基地址放入常数数据组GPIO_ARR[0]~GPIO_ARR[4]中
static const GPIO_MemMapPtr GPIO_ARR[] =
{ PTA_BASE_PTR, PTB_BASE_PTR,
PTC_BASE_PTR, PTD_BASE_PTR, PTE_BASE_PTR };

//===============================函数接口声明===================================

///==========================================================================================
//函数名称：gpio_init
//函数返回：无
//参数说明：port_pin：(端口号)|(引脚号)（例：U_PORTD|(3) 表示为D口3号脚）.
//          dir：引脚方向（0=输入，1=输出,可用引脚方向宏定义）.
//          state：端口引脚初始状态（0=低电平，1=高电平）.
//功能概要：初始化指定端口引脚作为GPIO引脚功能，并定义为输入或输出，若是输出，
//        还指定初始状态是低电平或高电平.
//===========================================================================
void gpio_init(uint16_t port_pin, uint8_t dir, uint8_t state);

//===========================================================================
//函数名称：gpio_set
//函数返回：无
//参数说明：port_pin：端口号|引脚号（例：U_PORTD|(3) 表示为D口3号脚）
//          state：引脚初始状态（0=低电平，1=高电平）
//功能概要：设定引脚状态为低电平或高电平
//===========================================================================
void gpio_set(uint16_t port_pin, uint8_t state);

//===========================================================================
//函数名称：gpio_get
//函数返回：指定引脚的状态（1或0）
//参数说明：port_pin：端口号|引脚号（例：U_PORTD|(3) 表示为D口3号脚）
//功能概要：获取指定引脚的状态（1或0）
//===========================================================================
uint8_t gpio_get(uint16_t port_pin);

//===========================================================================
//函数名称：gpio_reverse
//函数返回：无
//参数说明：port_pin：端口号|引脚号（例：U_PORTD|(3) 表示为D口3号脚）
//功能概要：反转指定引脚输出状态。
//===========================================================================
void gpio_reverse(uint16_t port_pin);

//===========================================================================
//函数名称：gpio_pull
//函数返回：无
//参数说明：port_pin：端口号|引脚号（例：U_PORTD|(3) 表示为D口3号脚）
//        pullselect：引脚拉高低电平（ 0=拉低电平，1=拉高电平）
//功能概要：使指定引脚上拉高电平或下拉低电平
//===========================================================================
void gpio_pull(uint16_t port_pin, uint8_t pullselect);

//===========================================================================
//函数名称：gpio_drive_strength
//函数返回：无
//参数说明：port_pin：(端口号)|(引脚号)（例：U_PORTD|(3) 表示为D口3号脚）
//        control：控制引脚的驱动能力，0=正常驱动能力(5mA),默认1=高驱动能力(18mA).
//功能概要： 指定端口引脚被定义为GPIO功能，设定引脚驱动能力
//===========================================================================
void gpio_drive_strength(uint16_t port_pin, uint8_t control);

//===========================================================================
//函数名称：gpio_enable_int
//函数返回：无
//参数说明：port_pin：(端口号)|(引脚号)（例：U_PORTD|(3) 表示为D口3号脚）
//       irqtype：引脚中断类型，由宏定义给出，再次列举如下：
//                  LOW_LEVEL    8      //低电平触发
//                  HIGH_LEVEL   12     //高电平触发
//                  RISING_EDGE  9      //上升沿触发
//                  FALLING_EDGE 10     //下降沿触发
//                  DOUBLE_EDGE  11     //双边沿触发
//功能概要：指定端口引脚被定义为GPIO功能且为输入时，本函数开启引脚中断，并设置中断触发条件。
//===========================================================================
void gpio_enable_int(uint16_t port_pin, uint8_t irqtype);

//===========================================================================
//函数名称：gpio_disable_int
//函数返回：无
//参数说明：port_pin：(端口号)|(引脚号)（例：U_PORTD|(3) 表示为D口3号脚）
//功能概要：当指定端口引脚被定义为GPIO功能且为输入时，本函数关闭引脚中断
//===========================================================================
void gpio_disable_int(uint16_t port_pin);

#endif
