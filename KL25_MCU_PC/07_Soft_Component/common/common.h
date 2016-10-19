//=====================================================================
//文件名称：common.h
//功能概要：公共要素头文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//更新记录：2012-10-12   V1.0
//         2013-02-02 V2.0(WYH)
//=====================================================================
#ifndef _COMMON_H    //防止重复定义（_COMMON_H  开头)
#define _COMMON_H

//1．芯片寄存器映射文件及处理器内核属性文件
#include "MKL25Z4.h"      // 包含芯片头文件
#include "core_cm0plus.h"
#include "core_cmFunc.h"
#include "core_cmInstr.h"
#include "system_MKL25Z4.h"

//2．定义开关总中断
#define ENABLE_INTERRUPTS   __enable_irq()  //开总中断
#define DISABLE_INTERRUPTS  __disable_irq()  //关总中断


//3．位操作宏函数（置位、清位、获得寄存器一位的状态）
#define BSET(bit,Register)  ((Register)|= (1<<(bit)))    //置寄存器的一位
#define BCLR(bit,Register)  ((Register) &= ~(1<<(bit)))  //清寄存器的一位
#define BGET(bit,Register)  (((Register) >> (bit)) & 1)  //获得寄存器一位的状态

//4．重定义基本数据类型（类型别名宏定义）
typedef unsigned char        uint_8;   // 无符号8位数，字节
typedef unsigned short int   uint_16;  // 无符号16位数，字
typedef unsigned long int    uint_32;  // 无符号32位数，长字
typedef unsigned long long   uint_64;
typedef char                 int_8;    // 有符号8位数
typedef short int            int_16;   // 有符号16位数
typedef int                  int_32;   // 有符号32位数
typedef long long            int_64;
//不优化类型
typedef volatile uint_8      vuint_8;  // 不优化无符号8位数，字节
typedef volatile uint_16     vuint_16; // 不优化无符号16位数，字
typedef volatile uint_32     vuint_32; // 不优化无符号32位数，长字
typedef volatile int_8       vint_8;   // 不优化有符号8位数
typedef volatile int_16      vint_16;  // 不优化有符号16位数
typedef volatile int_32      vint_32;  // 不优化有符号32位数

// 端口号地址偏移量宏定义
#define PORT_A    (0<<8)
#define PORT_B    (1<<8)
#define PORT_C    (2<<8)
#define PORT_D    (3<<8)
#define PORT_E    (4<<8)

//喂狗
#define WDI_FEED() {SIM_SRVCOP = 0x55;    SIM_SRVCOP = 0xAA;}

//5. 定义系统使用的时钟频率
#define  SYSTEM_CLK_KHZ   SystemCoreClock/1000     // 芯片系统时钟频率(KHz)
#define  BUS_CLK_KHZ      SYSTEM_CLK_KHZ/2         // 芯片总线时钟频率(KHz)

// 6．临界区访问函数

//===================================================================
//函数名称：init_critical
//参数说明：无
//函数返回：无
//功能概要：初始化临界区访问控制
//===================================================================
void  init_critical(void);

//===================================================================
//函数名称：enter_critical
//参数说明：无
//函数返回：无
//功能概要：进入临界区
//===================================================================
void  enter_critical(void);

//===================================================================
//函数名称：exit_critical
//参数说明：无
//函数返回：无
//功能概要：离开临界区
//===================================================================
void  exit_critical(void);


//======================================================================
//函数名称：enable_irq
//函数返回：无
//参数说明：irq：irq号, 各个模块的中断号定义在MKL25Z.h中
//功能概要：使能irq中断 
//======================================================================
void enable_irq (IRQn_Type irq);

//======================================================================
//函数名称：disable_irq
//函数返回：无      
//参数说明：irq：irq号, 各个模块的中断号定义在MKL25Z.h中
//功能概要：禁止irq中断 
//======================================================================
void disable_irq (IRQn_Type irq);

//======================================================================
//函数名称：set_irq_priority
//函数返回：无      
//参数说明：irq：irq号,各个模块的中断号定义在MKL25Z.h中
//          prio：优先级
//功能概要：设置irq中断和优先级 
//======================================================================
void set_irq_priority (IRQn_Type irq, uint_16 prio);

#endif //防止重复定义（_COMMON_H  结尾)
