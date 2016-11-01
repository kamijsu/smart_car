//==========================================================================
//文件名称：uart.h
//功能概要：K64 UART底层驱动程序头文件
//==========================================================================

#ifndef _UART_H
#define _UART_H

//包含公共头文件
#include "common.h"

//定义串口模块号（MOD为module简写）
#define UART_MOD0	(0)
#define UART_MOD1	(1)
#define UART_MOD2	(2)
#define UART_MOD3	(3)
#define UART_MOD4	(4)
#define UART_MOD5	(5)

//设置各串口模块所用引脚组，通过更改宏定义的值以选择引脚组，引脚组前者为TX，后者为RX
//UART0:  0=PTA2-PTA1;  1=PTA14-PTA15;  2=PTB17-PTB16;  3=PTD7-PTD6;
#define UART_MOD0_SETUP		(1)
//UART1:  0=PTC4-PTC3;  1=PTE0-PTE1;
#define UART_MOD1_SETUP		(1)
//UART2:  0=PTD3-PTD2;
#define UART_MOD2_SETUP		(0)
//UART3:  0=PTB11-PTB10;  1=PTC17-PTC16;  2=PTE4-PTE5;
#define UART_MOD3_SETUP		(0)
//UART4:  0=PTC15-PTC14;  1=PTE24-PTE25;
#define UART_MOD4_SETUP		(1)
//UART5:  0=PTD9-PTD8;  1=PTE8-PTE9;
#define UART_MOD5_SETUP		(0)

//定义UART各模块工作时钟频率，单位Hz
#define UART_WORK_FREQ1		(SYSTEM_CLK_KHZ * 1000)	//UART0和UART1使用系统时钟
#define UART_WORK_FREQ2		(BUS_CLK_KHZ * 1000)	//其余UART模块使用总线时钟

//定义UART模块校验模式
#define UART_PARITY_DISABLED	(0)		//不启用校验
#define UART_PARITY_ODD			(1)		//奇校验
#define UART_PARITY_EVEN		(2)		//偶校验

//定义UART模块停止位
#define UART_STOP_BIT_1			(1)		//1位停止位
#define UART_STOP_BIT_2			(2)		//2位停止位

//定义发送与接收时轮询上限次数，最大32位（RP为roll poling简写）
#define UART_RP_TIME_SEND		(0xFBBBu)	//发送轮询上限次数
#define UART_RP_TIME_RECEIVE	(0xFBBBu)	//接收轮询上限次数

//==========================================================================
//函数名称: uart_init
//函数返回: 无
//参数说明: mod:UART模块号:
//             UART_MODx，x为模块号;
//         baud:波特率:
//              (600) | 1200  | 2400  | 4800 | 9600 | 14400 | 19200 |
//              38400 | 56000 | 57600 | 115200
//         parity_mode:校验模式:
//                     UART_PARITY_DISABLED:不启用校验;
//                     UART_PARITY_ODD:奇校验;
//                     UART_PARITY_EVEN:偶校验;
//         stop_bit:停止位:
//                  UART_STOP_BIT_1:1位停止位;
//                  UART_STOP_BIT_2:2位停止位;
//功能概要: 初始化UART模块
//备注: 波特率为600时，UART0与UART1无法使用
//==========================================================================
void uart_init(uint8 mod, uint32 baud, uint8 parity_mode, uint8 stop_bit);

//==========================================================================
//函数名称: uart_send1
//函数返回: true:发送成功; false:发送失败
//参数说明: mod:UART模块号:
//             UART_MODx，x为模块号;
//         byte:想要发送的字节数据
//功能概要: 发送1个字节数据
//==========================================================================
bool uart_send1(uint8 mod, uint8 byte);

//==========================================================================
//函数名称: uart_sendN
//函数返回: true:发送成功; false:发送失败
//参数说明: mod:UART模块号:
//             UART_MODx，x为模块号;
//         len:发送的字节数
//         buff:发送缓冲区
//功能概要: 发送N个字节数据
//==========================================================================
bool uart_sendN(uint8 mod, uint32 len, uint8* buff);

//==========================================================================
//函数名称: uart_send_string
//函数返回: true:发送成功; false:发送失败
//参数说明: mod:UART模块号:
//             UART_MODx，x为模块号;
//         str:发送字符串的首地址
//功能概要: 发送一个以'\0'结束的字符串，不会发送'\0'
//==========================================================================
bool uart_send_string(uint8 mod, uint8* str);

//==========================================================================
//函数名称: uart_re1
//函数返回: true:接收成功; false:接收失败
//参数说明: mod:UART模块号:
//             UART_MODx，x为模块号;
//         byte:想要接收字节数据的地址
//功能概要: 接收1个字节数据
//==========================================================================
bool uart_re1(uint8 mod, uint8* byte);

//==========================================================================
//函数名称: uart_re1_parity
//函数返回: true:接收成功; false:接收失败
//参数说明: mod:UART模块号:
//             UART_MODx，x为模块号;
//         byte:想要接收字节数据的地址
//         err:接收成功时，奇偶校验有无错误，若未开启校验，则始终无错误
//功能概要: 接收1个字节数据，并判断校验位有无错误
//==========================================================================
bool uart_re1_parity(uint8 mod, uint8* byte, bool* err);

//==========================================================================
//函数名称: uart_enable_re_int
//函数返回: 无
//参数说明: mod:UART模块号:
//             UART_MODx，x为模块号;
//功能概要: 使能串口模块接收中断，通过接收数据可以清除中断标志
//==========================================================================
void uart_enable_re_int(uint8 mod);

//==========================================================================
//函数名称: uart_disable_re_int
//函数返回: 无
//参数说明: mod:UART模块号:
//             UART_MODx，x为模块号;
//功能概要: 关闭串口模块接收中断
//==========================================================================
void uart_disable_re_int(uint8 mod);

//根据各模块所设置的引脚组，定义相应模块的RX与TX引脚号，以及相应的PCR的MUX值
#ifdef UART_MOD0_SETUP
#if(UART_MOD0_SETUP == 0)
#define UART_MOD0_TX_PIN	(COM_PORTA|2)
#define UART_MOD0_RX_PIN	(COM_PORTA|1)
#define UART_MOD0_PCR_MUX	(2)
#else
#if(UART_MOD0_SETUP == 1)
#define UART_MOD0_TX_PIN	(COM_PORTA|14)
#define UART_MOD0_RX_PIN	(COM_PORTA|15)
#elif(UART_MOD0_SETUP == 2)
#define UART_MOD0_TX_PIN	(COM_PORTB|17)
#define UART_MOD0_RX_PIN	(COM_PORTB|16)
#elif(UART_MOD0_SETUP == 3)
#define UART_MOD0_TX_PIN	(COM_PORTD|7)
#define UART_MOD0_RX_PIN	(COM_PORTD|6)
#endif
#define UART_MOD0_PCR_MUX	(3)
#endif
#endif

#ifdef UART_MOD1_SETUP
#if(UART_MOD1_SETUP == 0)
#define UART_MOD1_TX_PIN	(COM_PORTC|4)
#define UART_MOD1_RX_PIN	(COM_PORTC|3)
#elif(UART_MOD1_SETUP == 1)
#define UART_MOD1_TX_PIN	(COM_PORTE|0)
#define UART_MOD1_RX_PIN	(COM_PORTE|1)
#endif
#define UART_MOD1_PCR_MUX	(3)
#endif

#ifdef UART_MOD2_SETUP
#if(UART_MOD2_SETUP == 0)
#define UART_MOD2_TX_PIN	(COM_PORTD|3)
#define UART_MOD2_RX_PIN	(COM_PORTD|2)
#endif
#define UART_MOD2_PCR_MUX	(3)
#endif

#ifdef UART_MOD3_SETUP
#if(UART_MOD3_SETUP == 0)
#define UART_MOD3_TX_PIN	(COM_PORTB|11)
#define UART_MOD3_RX_PIN	(COM_PORTB|10)
#elif(UART_MOD3_SETUP == 1)
#define UART_MOD3_TX_PIN	(COM_PORTC|17)
#define UART_MOD3_RX_PIN	(COM_PORTC|16)
#elif(UART_MOD3_SETUP == 2)
#define UART_MOD3_TX_PIN	(COM_PORTE|4)
#define UART_MOD3_RX_PIN	(COM_PORTE|5)
#endif
#define UART_MOD3_PCR_MUX	(3)
#endif

#ifdef UART_MOD4_SETUP
#if(UART_MOD4_SETUP == 0)
#define UART_MOD4_TX_PIN	(COM_PORTC|15)
#define UART_MOD4_RX_PIN	(COM_PORTC|14)
#elif(UART_MOD4_SETUP == 1)
#define UART_MOD4_TX_PIN	(COM_PORTE|24)
#define UART_MOD4_RX_PIN	(COM_PORTE|25)
#endif
#define UART_MOD4_PCR_MUX	(3)
#endif

#ifdef UART_MOD5_SETUP
#if(UART_MOD5_SETUP == 0)
#define UART_MOD5_TX_PIN	(COM_PORTD|9)
#define UART_MOD5_RX_PIN	(COM_PORTD|8)
#elif(UART_MOD5_SETUP == 1)
#define UART_MOD5_TX_PIN	(COM_PORTE|8)
#define UART_MOD5_RX_PIN	(COM_PORTE|9)
#endif
#define UART_MOD5_PCR_MUX	(3)
#endif

#endif
