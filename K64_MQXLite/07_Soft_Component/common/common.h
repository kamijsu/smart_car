//----------------------------------------------------------------------------
//common.h
//2015年4月3日
//Author: SUMCU
//----------------------------------------------------------------------------

#ifndef COMMON_H_
#define COMMON_H_

// 1．芯片寄存器映射文件及处理器内核属性文件
#include "core_cmFunc.h"
#include "core_cmInstr.h"
#include "core_cm4.h"
#include "MK64F12.h"           // 包含芯片头文件
#include "system_MK64F12.h"    // 包含芯片系统初始化文件

// 2．定义开关总中断
#define ENABLE_INTERRUPTS   __enable_irq   // 开总中断
#define DISABLE_INTERRUPTS  __disable_irq  // 关总中断


// 3．位操作宏函数（置位、清位、获得寄存器一位的状态）
#define BSET(bit,Register)  ((Register)|= (1<<(bit)))    // 置寄存器的一位
#define BCLR(bit,Register)  ((Register) &= ~(1<<(bit)))  // 清寄存器的一位
#define BGET(bit,Register)  (((Register) >> (bit)) & 1)  // 获得寄存器一位的状态


//4．重定义基本数据类型（类型别名宏定义）
typedef unsigned char        uint_8;   // 无符号8位数，字节
typedef unsigned short int   uint_16;  // 无符号16位数，字
typedef unsigned long int    uint_32;  // 无符号32位数，长字
typedef unsigned long long   uint_64;  // 无符号64位数，长长字

typedef signed char          int_8;    // 有符号的8位数，字节
typedef signed short int     int_16;   // 有符号的16位数，字
typedef signed long int      int_32;   // 有符号的32位数，长字
typedef signed long long     int_64;   // 有符号的64位数，长长字

//5. 定义系统使用的时钟频率
#define  SYSTEM_CLK_KHZ   SystemCoreClock/1000     // 芯片系统时钟频率(KHz)
#define  BUS_CLK_KHZ      SYSTEM_CLK_KHZ/2         // 芯片总线时钟频率(KHz)


#endif /* COMMON_H_ */
