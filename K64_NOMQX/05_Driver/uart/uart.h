//==========================================================================
//文件名称：uart.h
//功能概要：K64 UART底层驱动程序头文件
//==========================================================================

#ifndef _UART_H
#define _UART_H

//包含公共头文件
#include "common.h"

//定义串口模块号（MOD为module简称）
#define UART_MOD0	(0)
#define UART_MOD1	(1)
#define UART_MOD2	(2)
#define UART_MOD3	(3)
#define UART_MOD4	(4)
#define UART_MOD5	(5)

//设置各串口模块所用引脚组，通过更改宏定义的值以选择引脚组，引脚组前者为TX，后者为RX
//UART0:  0=PTA2-PTA1;  1=PTA14-PTA15;  2=PTB17-PTB16;  3=PTD7-PTD6;
#define UART_MOD0_SETUP		(0)
//UART1:  0=PTC4-PTC3;  1=PTE0-PTE1;
#define UART_MOD1_SETUP		(1)
//UART2:  0=PTD3-PTD2;
#define UART_MOD2_SETUP		(0)
//UART3:  0=PTB11-PTB10;  1=PTC17-PTC16;  2=PTE4-PTE5;
#define UART_MOD3_SETUP		(0)
//UART4:  0=PTC15-PTC14;  1=PTE24-PTE25;
#define UART_MOD4_SETUP		(0)
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

//==========================================================================
//函数名称: uart_init
//函数返回: 无
//参数说明: mod:UART模块号，UART_MODx，x为模块号
//         baud:波特率:(600) | 1200 | 2400 | 4800 | 9600 | 14400 | 19200 |
//                     38400 | 56000 | 57600 | 115200 | 128000 | 256000
//         parity_mode:校验模式，UART_PARITY_DISABLED:不启用校验;
//                     UART_PARITY_ODD:奇校验; UART_PARITY_EVEN:偶校验
//         stop_bit:停止位，UART_STOP_BIT_1:1位停止位; UART_STOP_BIT_2:2位停止位
//功能概要: 初始化UART模块
//备注: 波特率为600时，UART0与UART1无法使用
//==========================================================================
void uart_init(uint8 mod, uint32 baud, uint8 parity_mode, uint8 stop_bit);

//=========================================================================
//函数名称：uart_re1
//参数说明：uartNo: 串口号
//          fp:接收成功标志的指针:*p=0，成功接收；*p=1，接收失败
//函数返回：接收返回字节
//功能概要：串行接收1个字节
//=========================================================================
uint8_t uart_re1(uint8_t uartNo, uint8_t *fp);

//=========================================================================
//函数名称：uart_send1
//参数说明：uartNo: 串口号
//          ch:要发送的字节
//函数返回：函数执行状态：0=正常；非0=异常。
//功能概要：串行发送1个字节
//=========================================================================
uint8_t uart_send1(uint8_t uartNo, uint8_t ch);

//=========================================================================
//函数名称：uart_reN                                                         
//功能概要：串行 接收n个字节                                                 
//参数说明：uartNo: 串口号
//          buff: 接收缓冲区                                                 
//		   len:接收长度                                                     
//函数返回： 1:成功;0:失败                                                    
//=========================================================================
uint8_t uart_reN(uint8_t uartNo, uint16_t len, uint8_t* buff);

//=========================================================================
//函数名称：uart_sendN
//参数说明：uartNo: 串口号
//          buff: 发送缓冲区
//          len:发送长度
//函数返回：函数执行状态：0=正常；1=异常。
//功能概要：串行接收n个字节   
//=========================================================================
uint8_t uart_sendN(uint8_t uartNo, uint16_t len, uint8_t* buff);

//=========================================================================
//函数名称：uart_send_string
//参数说明：uartNo:UART模块号
//          buff:要发送的字符串的首地址
//函数返回：函数执行状态：0=正常；非0=异常。
//功能概要：从指定UART端口发送一个以'\0'结束的字符串
//=========================================================================
uint8_t uart_send_string(uint8_t uartNo, void *buff);

//=========================================================================
//函数名称：uart_enable_re_int
//参数说明：uartNo: 串口号
//函数返回：无
//功能概要：开串口接收中断
//=========================================================================
void uart_enable_re_int(uint8_t uartNo);

//=========================================================================
//函数名称：uart_disable_re_int
//参数说明：uartNo: 串口号
//函数返回：无
//功能概要：关串口接收中断
//=========================================================================
void uart_disable_re_int(uint8_t uartNo);

//根据各模块所设置的引脚组，定义其使用的引脚控制寄存器
#ifdef UART_MOD0_SETUP
#if(UART_MOD0_SETUP == 0)
#define UART_MOD0_TX_PCR	PORTA_PCR2
#define UART_MOD0_RX_PCR	PORTA_PCR1
#endif
#if(UART_MOD0_SETUP == 1)
#define UART_MOD0_TX_PCR	PORTA_PCR14
#define UART_MOD0_RX_PCR	PORTA_PCR15
#endif
#if(UART_MOD0_SETUP == 2)
#define UART_MOD0_TX_PCR	PORTB_PCR17
#define UART_MOD0_RX_PCR	PORTB_PCR16
#endif
#if(UART_MOD0_SETUP == 3)
#define UART_MOD0_TX_PCR	PORTD_PCR7
#define UART_MOD0_RX_PCR	PORTD_PCR6
#endif
#endif

#ifdef UART_MOD1_SETUP
#if(UART_MOD1_SETUP == 0)
#define UART_MOD1_TX_PCR	PORTC_PCR4
#define UART_MOD1_RX_PCR	PORTC_PCR3
#endif
#if(UART_MOD1_SETUP == 1)
#define UART_MOD1_TX_PCR	PORTE_PCR0
#define UART_MOD1_RX_PCR	PORTE_PCR1
#endif
#endif

#ifdef UART_MOD2_SETUP
#if(UART_MOD2_SETUP == 0)
#define UART_MOD2_TX_PCR	PORTD_PCR3
#define UART_MOD2_RX_PCR	PORTD_PCR2
#endif
#endif

#ifdef UART_MOD3_SETUP
#if(UART_MOD3_SETUP == 0)
#define UART_MOD3_TX_PCR	PORTB_PCR11
#define UART_MOD3_RX_PCR	PORTB_PCR10
#endif
#if(UART_MOD3_SETUP == 1)
#define UART_MOD3_TX_PCR	PORTC_PCR17
#define UART_MOD3_RX_PCR	PORTC_PCR16
#endif
#if(UART_MOD3_SETUP == 2)
#define UART_MOD3_TX_PCR	PORTE_PCR4
#define UART_MOD3_RX_PCR	PORTE_PCR5
#endif
#endif

#ifdef UART_MOD4_SETUP
#if(UART_MOD4_SETUP == 0)
#define UART_MOD4_TX_PCR	PORTC_PCR15
#define UART_MOD4_RX_PCR	PORTC_PCR14
#endif
#if(UART_MOD4_SETUP == 1)
#define UART_MOD4_TX_PCR	PORTE_PCR24
#define UART_MOD4_RX_PCR	PORTE_PCR25
#endif
#endif

#ifdef UART_MOD5_SETUP
#if(UART_MOD5_SETUP == 0)
#define UART_MOD5_TX_PCR	PORTD_PCR9
#define UART_MOD5_RX_PCR	PORTD_PCR8
#endif
#if(UART_MOD5_SETUP == 1)
#define UART_MOD5_TX_PCR	PORTE_PCR8
#define UART_MOD5_RX_PCR	PORTE_PCR9
#endif
#endif

#endif
