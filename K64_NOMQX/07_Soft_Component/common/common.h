//==========================================================================
//文件名称：common.h
//功能概要：通用文件的头文件
//==========================================================================

#ifndef _COMMON_H
#define _COMMON_H

//芯片寄存器映射文件及处理器内核属性文件
#include "core_cmFunc.h"
#include "core_cmInstr.h"
#include "core_cm4.h"
#include "core_cmSimd.h"
#include "MK64F12.h"           //包含芯片头文件
#include "system_MK64F12.h"    //包含芯片系统初始化文件

//定义开关中断
#define ENABLE_INTERRUPTS   __enable_irq()   //开总中断
#define DISABLE_INTERRUPTS  __disable_irq()  //关总中断
#define ENABLE_IRQ(IRQn)	NVIC_EnableIRQ(IRQn)	//允许接收中断请求号为IRQn的中断，中断请求号的定义在"MK64F12.h"的171行
#define DISABLE_IRQ(IRQn)	NVIC_DisableIRQ(IRQn)	//禁止接收中断请求号为IRQn的中断

//寄存器操作宏函数
//根据偏移进行操作
#define REG_SET_SHIFT(reg,shift)	((reg) |= (0x1 << (shift)))		//置寄存器的一位
#define REG_CLR_SHIFT(reg,shift)	((reg) &= ~(0x1 << (shift)))	//清寄存器的一位
#define REG_GET_SHIFT(reg,shift)	(((reg) >> (shift)) & 0x1)		//获取寄存器一位的状态
//根据掩码进行操作
#define REG_SET_MASK(reg,mask)		((reg) |= (mask))				//置寄存器的掩码位
#define REG_CLR_MASK(reg,mask)		((reg) &= ~(mask))				//清寄存器的掩码位
#define REG_GET_MASK(reg,mask)		((reg) & (mask))				//获取寄存器掩码位的值
//设置寄存器的值
#define REG_SET_VAL(reg,val)		((reg) = (val))

//重定义基本数据类型（类型别名宏定义）
typedef unsigned char uint8;		//无符号8位数，字节
typedef unsigned short int uint16;	//无符号16位数，字
typedef unsigned long int uint32;	//无符号32位数，长字
typedef unsigned long long uint64;	//无符号64位数，长长字

typedef signed char int8;			//有符号的8位数，字节
typedef signed short int int16;		//有符号的16位数，字
typedef signed long int int32;		//有符号的32位数，长字
typedef signed long long int64;		//有符号的64位数，长长字

typedef volatile uint8 vuint8;		//不优化无符号8位数，字节
typedef volatile uint16 vuint16;	//不优化无符号16位数，字
typedef volatile uint32 vuint32;	//不优化无符号32位数，长字
typedef volatile uint64 vuint64;	//不优化无符号64位数，长长字

typedef volatile int8 vint8;		//不优化有符号8位数，字节
typedef volatile int16 vint16;		//不优化有符号16位数，字
typedef volatile int32 vint32;		//不优化有符号32位数，长字
typedef volatile int64 vint64;		//不优化有符号64位数，长长字

typedef uint8 bool;					//bool型，值为true或false
//定义bool型的值
#define true	(1)					//真
#define false	(0)					//假

//定义系统使用的时钟频率
#define  SYSTEM_CLK_KHZ   (SystemCoreClock/1000)	//芯片系统时钟频率(KHz)
#define  BUS_CLK_KHZ      (SYSTEM_CLK_KHZ>>1)		//芯片总线时钟频率(KHz)，这里设置的为系统时钟2分频

//端口号地址偏移量宏定义，即形如COM_PORTA|0的值表示A端口0引脚；
//为了防止宏定义重名，请加上相应文件的名称，这里的COM为common的简写
#define COM_PORTA     (0<<5)
#define COM_PORTB     (1<<5)
#define COM_PORTC     (2<<5)
#define COM_PORTD     (3<<5)
#define COM_PORTE     (4<<5)

//各端口基地址，这是个全局常量，通过PORT_PCR_REG(port_table[port],pin)可获得该引脚的引脚控制寄存器
static PORT_Type * const port_table[] = { PORTA, PORTB, PORTC, PORTD, PORTE };

//===========================================================================
//函数名称: com_port_pin_resolution
//函数返回: 无
//参数说明: port_pin:(端口号)|(引脚号):
//                  COM_PORTx|p，x为端口号，p为引脚号，具体见common.h中宏定义;
//         port:获得端口号的变量
//	       pin:获得引脚号的变量(0~31，实际取值由芯片的物理引脚决定)
//功能概要: 将传进参数port_pin进行解析，得出具体端口号与引脚号
//===========================================================================
void com_port_pin_resolution(uint8 port_pin, uint8* port, uint8* pin);

#endif
