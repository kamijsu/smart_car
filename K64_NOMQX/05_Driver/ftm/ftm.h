//==========================================================================
//文件名称：ftm.h
//功能概要：K64 FTM底层驱动程序头文件
//==========================================================================

#ifndef _FTM_H
#define _FTM_H

//包含公共头文件
#include "common.h"

//定义FTM模块号
#define FTM_MOD0	(0)
#define FTM_MOD1	(1)
#define FTM_MOD2	(2)
#define FTM_MOD3	(3)

//定义FTM模块通道号
#define FTM_CH0		(0)
#define FTM_CH1		(1)
#define FTM_CH2		(2)
#define FTM_CH3		(3)
#define FTM_CH4		(4)
#define FTM_CH5		(5)
#define FTM_CH6		(6)
#define FTM_CH7		(7)

//FTM各模块各通道的引脚设置，通过更改COM_PORTx|x的x以选择引脚，
//可选择的引脚注释在相应通道后方。注意：B12|B13不要重复定义
#define FTM_MOD0_CH0_PIN	(COM_PORTA|3)	//A3   C1
#define FTM_MOD0_CH1_PIN	(COM_PORTA|4)	//A2   C2
#define FTM_MOD0_CH2_PIN	(COM_PORTA|5)	//A5   C3   C5
#define FTM_MOD0_CH3_PIN	(COM_PORTA|6)	//A6   C4
#define FTM_MOD0_CH4_PIN	(COM_PORTA|7)	//A7   B12  D4
#define FTM_MOD0_CH5_PIN	(COM_PORTA|0)	//A0   B13  D5
#define FTM_MOD0_CH6_PIN	(COM_PORTA|1)	//A1        D6
#define FTM_MOD0_CH7_PIN	(COM_PORTA|2)	//A2        D7

#define FTM_MOD1_CH0_PIN	(COM_PORTA|8)	//A8   A12  B0   B12
#define FTM_MOD1_CH1_PIN	(COM_PORTA|9)	//A9   A13  B1   B13

#define FTM_MOD2_CH0_PIN	(COM_PORTA|10)	//A10  B18
#define FTM_MOD2_CH1_PIN	(COM_PORTA|11)	//A11  B19

#define FTM_MOD3_CH0_PIN	(COM_PORTD|0)	//D0   E5
#define FTM_MOD3_CH1_PIN	(COM_PORTD|1)	//D1   E6
#define FTM_MOD3_CH2_PIN	(COM_PORTD|2)	//D2   E7
#define FTM_MOD3_CH3_PIN	(COM_PORTD|3)	//D3   E8
#define FTM_MOD3_CH4_PIN	(COM_PORTC|8)	//C8   E9
#define FTM_MOD3_CH5_PIN	(COM_PORTC|9)	//C9   E10
#define FTM_MOD3_CH6_PIN	(COM_PORTC|10)	//C10  E11
#define FTM_MOD3_CH7_PIN	(COM_PORTC|11)	//C11  E12

//FTM模块所用时钟频率，这里使用的是系统时钟，单位kHz
#define FTM_CLK_FREQ	SYSTEM_CLK_KHZ

//FTM模块时钟分频因子，DIV后的数字代表分频因子，其工作频率=时钟频率/分频因子
#define FTM_CLK_DIV_1		(0)
#define FTM_CLK_DIV_2		(1)
#define FTM_CLK_DIV_4		(2)
#define FTM_CLK_DIV_8		(3)
#define FTM_CLK_DIV_16		(4)
#define FTM_CLK_DIV_32		(5)
#define FTM_CLK_DIV_64		(6)
#define FTM_CLK_DIV_128		(7)

#ifdef FTM_MOD0_CH0_PIN
#if(FTM_MOD0_CH0_PIN == (COM_PORTA|3))
#define FTM_MOD0_CH0_PCR	PORTA_PCR3
#define FTM_MOD0_CH0_MUX	(3)
#elif(FTM_MOD0_CH0_PIN == (COM_PORTC|1))
#define FTM_MOD0_CH0_PCR	PORTC_PCR1
#define FTM_MOD0_CH0_MUX	(4)
#endif
#endif

#ifdef FTM_MOD0_CH1_PIN
#if(FTM_MOD0_CH1_PIN == (COM_PORTA|4))
#define FTM_MOD0_CH1_PCR	PORTA_PCR4
#define FTM_MOD0_CH1_MUX	(3)
#elif(FTM_MOD0_CH1_PIN == (COM_PORTC|2))
#define FTM_MOD0_CH1_PCR	PORTC_PCR2
#define FTM_MOD0_CH1_MUX	(4)
#endif
#endif

#ifdef FTM_MOD0_CH2_PIN
#if(FTM_MOD0_CH2_PIN == (COM_PORTA|5))
#define FTM_MOD0_CH2_PCR	PORTA_PCR5
#define FTM_MOD0_CH2_MUX	(3)
#elif(FTM_MOD0_CH2_PIN == (COM_PORTC|3))
#define FTM_MOD0_CH2_PCR	PORTC_PCR3
#define FTM_MOD0_CH2_MUX	(4)
#elif(FTM_MOD0_CH2_PIN == (COM_PORTC|5))
#define FTM_MOD0_CH2_PCR	PORTC_PCR5
#define FTM_MOD0_CH2_MUX	(7)
#endif
#endif

#ifdef FTM_MOD0_CH3_PIN
#if(FTM_MOD0_CH3_PIN == (COM_PORTA|6))
#define FTM_MOD0_CH3_PCR	PORTA_PCR6
#define FTM_MOD0_CH3_MUX	(3)
#elif(FTM_MOD0_CH3_PIN == (COM_PORTC|4))
#define FTM_MOD0_CH3_PCR	PORTC_PCR4
#define FTM_MOD0_CH3_MUX	(4)
#endif
#endif

#ifdef FTM_MOD0_CH4_PIN
#if(FTM_MOD0_CH4_PIN == (COM_PORTA|7))
#define FTM_MOD0_CH4_PCR	PORTA_PCR7
#define FTM_MOD0_CH4_MUX	(3)
#elif(FTM_MOD0_CH4_PIN == (COM_PORTB|12))
#define FTM_MOD0_CH4_PCR	PORTB_PCR12
#define FTM_MOD0_CH4_MUX	(4)
#elif(FTM_MOD0_CH4_PIN == (COM_PORTD|4))
#define FTM_MOD0_CH4_PCR	PORTD_PCR4
#define FTM_MOD0_CH4_MUX	(4)
#endif
#endif

#ifdef FTM_MOD0_CH5_PIN
#if(FTM_MOD0_CH5_PIN == (COM_PORTA|0))
#define FTM_MOD0_CH5_PCR	PORTA_PCR0
#define FTM_MOD0_CH5_MUX	(3)
#elif(FTM_MOD0_CH5_PIN == (COM_PORTB|13))
#define FTM_MOD0_CH5_PCR	PORTB_PCR13
#define FTM_MOD0_CH5_MUX	(4)
#elif(FTM_MOD0_CH5_PIN == (COM_PORTD|5))
#define FTM_MOD0_CH5_PCR	PORTD_PCR5
#define FTM_MOD0_CH5_MUX	(4)
#endif
#endif

#ifdef FTM_MOD0_CH6_PIN
#if(FTM_MOD0_CH6_PIN == (COM_PORTA|1))
#define FTM_MOD0_CH6_PCR	PORTA_PCR1
#define FTM_MOD0_CH6_MUX	(3)
#elif(FTM_MOD0_CH6_PIN == (COM_PORTD|6))
#define FTM_MOD0_CH6_PCR	PORTD_PCR6
#define FTM_MOD0_CH6_MUX	(4)
#endif
#endif

#ifdef FTM_MOD0_CH7_PIN
#if(FTM_MOD0_CH7_PIN == (COM_PORTA|2))
#define FTM_MOD0_CH7_PCR	PORTA_PCR2
#define FTM_MOD0_CH7_MUX	(3)
#elif(FTM_MOD0_CH7_PIN == (COM_PORTD|7))
#define FTM_MOD0_CH7_PCR	PORTD_PCR7
#define FTM_MOD0_CH7_MUX	(4)
#endif
#endif

#ifdef FTM_MOD1_CH0_PIN
#if(FTM_MOD1_CH0_PIN == (COM_PORTA|8))
#define FTM_MOD1_CH0_PCR	PORTA_PCR8
#elif(FTM_MOD1_CH0_PIN == (COM_PORTA|12))
#define FTM_MOD1_CH0_PCR	PORTA_PCR12
#elif(FTM_MOD1_CH0_PIN == (COM_PORTB|0))
#define FTM_MOD1_CH0_PCR	PORTB_PCR0
#elif(FTM_MOD1_CH0_PIN == (COM_PORTB|12))
#define FTM_MOD1_CH0_PCR	PORTB_PCR12
#endif
#define FTM_MOD1_CH0_MUX	(3)
#endif

#ifdef FTM_MOD1_CH1_PIN
#if(FTM_MOD1_CH1_PIN == (COM_PORTA|9))
#define FTM_MOD1_CH1_PCR	PORTA_PCR9
#elif(FTM_MOD1_CH1_PIN == (COM_PORTA|13))
#define FTM_MOD1_CH1_PCR	PORTA_PCR13
#elif(FTM_MOD1_CH1_PIN == (COM_PORTB|1))
#define FTM_MOD1_CH1_PCR	PORTB_PCR1
#elif(FTM_MOD1_CH1_PIN == (COM_PORTB|13))
#define FTM_MOD1_CH1_PCR	PORTB_PCR13
#endif
#define FTM_MOD1_CH1_MUX	(3)
#endif

#ifdef FTM_MOD2_CH0_PIN
#if(FTM_MOD2_CH0_PIN == (COM_PORTA|10))
#define FTM_MOD2_CH0_PCR	PORTA_PCR10
#elif(FTM_MOD2_CH0_PIN == (COM_PORTB|18))
#define FTM_MOD2_CH0_PCR	PORTB_PCR18
#endif
#define FTM_MOD2_CH0_MUX	(3)
#endif

#ifdef FTM_MOD2_CH1_PIN
#if(FTM_MOD2_CH1_PIN == (COM_PORTA|11))
#define FTM_MOD2_CH1_PCR	PORTA_PCR11
#elif(FTM_MOD2_CH1_PIN == (COM_PORTB|19))
#define FTM_MOD2_CH1_PCR	PORTB_PCR19
#endif
#define FTM_MOD2_CH1_MUX	(3)
#endif

#ifdef FTM_MOD3_CH0_PIN
#if(FTM_MOD3_CH0_PIN == (COM_PORTD|0))
#define FTM_MOD3_CH0_PCR	PORTD_PCR0
#define FTM_MOD3_CH0_MUX	(4)
#elif(FTM_MOD3_CH0_PIN == (COM_PORTE|5))
#define FTM_MOD3_CH0_PCR	PORTE_PCR5
#define FTM_MOD3_CH0_MUX	(6)
#endif
#endif

#ifdef FTM_MOD3_CH1_PIN
#if(FTM_MOD3_CH1_PIN == (COM_PORTD|1))
#define FTM_MOD3_CH1_PCR	PORTD_PCR1
#define FTM_MOD3_CH1_MUX	(4)
#elif(FTM_MOD3_CH1_PIN == (COM_PORTE|6))
#define FTM_MOD3_CH1_PCR	PORTE_PCR6
#define FTM_MOD3_CH1_MUX	(6)
#endif
#endif

#ifdef FTM_MOD3_CH2_PIN
#if(FTM_MOD3_CH2_PIN == (COM_PORTD|2))
#define FTM_MOD3_CH2_PCR	PORTD_PCR2
#define FTM_MOD3_CH2_MUX	(4)
#elif(FTM_MOD3_CH2_PIN == (COM_PORTE|7))
#define FTM_MOD3_CH2_PCR	PORTE_PCR7
#define FTM_MOD3_CH2_MUX	(6)
#endif
#endif

#ifdef FTM_MOD3_CH3_PIN
#if(FTM_MOD3_CH3_PIN == (COM_PORTD|3))
#define FTM_MOD3_CH3_PCR	PORTD_PCR3
#define FTM_MOD3_CH3_MUX	(4)
#elif(FTM_MOD3_CH3_PIN == (COM_PORTE|8))
#define FTM_MOD3_CH3_PCR	PORTE_PCR8
#define FTM_MOD3_CH3_MUX	(6)
#endif
#endif

#ifdef FTM_MOD3_CH4_PIN
#if(FTM_MOD3_CH4_PIN == (COM_PORTC|8))
#define FTM_MOD3_CH4_PCR	PORTC_PCR8
#define FTM_MOD3_CH4_MUX	(3)
#elif(FTM_MOD3_CH4_PIN == (COM_PORTE|9))
#define FTM_MOD3_CH4_PCR	PORTE_PCR9
#define FTM_MOD3_CH4_MUX	(6)
#endif
#endif

#ifdef FTM_MOD3_CH5_PIN
#if(FTM_MOD3_CH5_PIN == (COM_PORTC|9))
#define FTM_MOD3_CH5_PCR	PORTC_PCR9
#define FTM_MOD3_CH5_MUX	(3)
#elif(FTM_MOD3_CH5_PIN == (COM_PORTE|10))
#define FTM_MOD3_CH5_PCR	PORTE_PCR10
#define FTM_MOD3_CH5_MUX	(6)
#endif
#endif

#ifdef FTM_MOD3_CH6_PIN
#if(FTM_MOD3_CH6_PIN == (COM_PORTC|10))
#define FTM_MOD3_CH6_PCR	PORTC_PCR10
#define FTM_MOD3_CH6_MUX	(3)
#elif(FTM_MOD3_CH6_PIN == (COM_PORTE|11))
#define FTM_MOD3_CH6_PCR	PORTE_PCR11
#define FTM_MOD3_CH6_MUX	(6)
#endif
#endif

#ifdef FTM_MOD3_CH7_PIN
#if(FTM_MOD3_CH7_PIN == (COM_PORTC|11))
#define FTM_MOD3_CH7_PCR	PORTC_PCR11
#define FTM_MOD3_CH7_MUX	(3)
#elif(FTM_MOD3_CH7_PIN == (COM_PORTE|12))
#define FTM_MOD3_CH7_PCR	PORTE_PCR12
#define FTM_MOD3_CH7_MUX	(6)
#endif
#endif

#endif
