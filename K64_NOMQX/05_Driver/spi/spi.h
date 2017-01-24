//==========================================================================
//文件名称：spi.h
//功能概要：K64 SPI底层驱动程序头文件
//==========================================================================

#ifndef _SPI_H
#define _SPI_H

//包含公共头文件
#include "common.h"

//定义SPI模块号
#define SPI_MOD0	(0)
#define SPI_MOD1	(1)
#define SPI_MOD2	(2)

//定义SPI片选号
#define SPI_CS0		(0)
#define SPI_CS1		(1)
#define SPI_CS2		(2)
#define SPI_CS3		(3)
#define SPI_CS4		(4)
#define SPI_CS5		(5)

//定义SPI各模块的引脚设置，通过更改COM_PORTx|x的x以选择引脚，
//SCK、SOUT、SIN、CSx分别为时钟信号、输出信号、输入信号、片选信号引脚
#define SPI_MOD0_SCK_PIN	(COM_PORTA|15)	//A15 C5  D1
#define SPI_MOD0_SOUT_PIN	(COM_PORTA|16)	//A16 C6  D2
#define SPI_MOD0_SIN_PIN	(COM_PORTA|17)	//A17 C7  D3
#define SPI_MOD0_CS0_PIN	(COM_PORTA|14)	//A14 C4  D0
#define SPI_MOD0_CS1_PIN	(COM_PORTC|3)	//    C3  D4
#define SPI_MOD0_CS2_PIN	(COM_PORTC|2)	//    C2  D5
#define SPI_MOD0_CS3_PIN	(COM_PORTC|1)	//    C1  D6
#define SPI_MOD0_CS4_PIN	(COM_PORTC|0)	//    C0
#define SPI_MOD0_CS5_PIN	(COM_PORTB|23)	//    B23

#define SPI_MOD1_SCK_PIN	(COM_PORTE|2)	//E2      B11 D5
#define SPI_MOD1_SOUT_PIN	(COM_PORTE|1)	//E1  E3  B16 D6
#define SPI_MOD1_SIN_PIN	(COM_PORTE|3)	//E3  E1  B17 D7
#define SPI_MOD1_CS0_PIN	(COM_PORTE|4)	//E4      B10 D4
#define SPI_MOD1_CS1_PIN	(COM_PORTE|0)	//E0      B9
#define SPI_MOD1_CS2_PIN	(COM_PORTE|5)	//E5
#define SPI_MOD1_CS3_PIN	(COM_PORTE|6)	//E6

#define SPI_MOD2_SCK_PIN	(COM_PORTB|21)	//B21 D12
#define SPI_MOD2_SOUT_PIN	(COM_PORTB|22)	//B22 D13
#define SPI_MOD2_SIN_PIN	(COM_PORTB|23)	//B23 D14
#define SPI_MOD2_CS0_PIN	(COM_PORTB|20)	//B20 D11
#define SPI_MOD2_CS1_PIN	(COM_PORTD|15)	//    D15

//根据SPI模块所设置的引脚号，定义相应的PCR的MUX值
#ifdef SPI_MOD0_SCK_PIN
#define SPI_MOD0_SCK_PCR_MUX	(2)
#endif

#ifdef SPI_MOD0_SOUT_PIN
#define SPI_MOD0_SOUT_PCR_MUX	(2)
#endif

#ifdef SPI_MOD0_SIN_PIN
#define SPI_MOD0_SIN_PCR_MUX	(2)
#endif

#ifdef SPI_MOD0_CS0_PIN
#define SPI_MOD0_CS0_PCR_MUX	(2)
#endif

#ifdef SPI_MOD0_CS1_PIN
#define SPI_MOD0_CS1_PCR_MUX	(2)
#endif

#ifdef SPI_MOD0_CS2_PIN
#define SPI_MOD0_CS2_PCR_MUX	(2)
#endif

#ifdef SPI_MOD0_CS3_PIN
#define SPI_MOD0_CS3_PCR_MUX	(2)
#endif

#ifdef SPI_MOD0_CS4_PIN
#define SPI_MOD0_CS4_PCR_MUX	(2)
#endif

#ifdef SPI_MOD0_CS5_PIN
#define SPI_MOD0_CS5_PCR_MUX	(3)
#endif

#ifdef SPI_MOD1_SCK_PIN
#if(SPI_MOD1_SCK_PIN == (COM_PORTD|5))
#define SPI_MOD1_SCK_PCR_MUX	(7)
#else
#define SPI_MOD1_SCK_PCR_MUX	(2)
#endif
#endif

#ifdef SPI_MOD1_SOUT_PIN
#if(SPI_MOD1_SOUT_PIN == (COM_PORTE|1) || SPI_MOD1_SOUT_PIN == (COM_PORTB|16))
#define SPI_MOD1_SOUT_PCR_MUX	(2)
#else
#define SPI_MOD1_SOUT_PCR_MUX	(7)
#endif
#endif

#ifdef SPI_MOD1_SIN_PIN
#if(SPI_MOD1_SIN_PIN == (COM_PORTE|3) || SPI_MOD1_SIN_PIN == (COM_PORTB|17))
#define SPI_MOD1_SIN_PCR_MUX	(2)
#else
#define SPI_MOD1_SIN_PCR_MUX	(7)
#endif
#endif

#ifdef SPI_MOD1_CS0_PIN
#if(SPI_MOD1_CS0_PIN == (COM_PORTD|4))
#define SPI_MOD1_CS0_PCR_MUX	(7)
#else
#define SPI_MOD1_CS0_PCR_MUX	(2)
#endif
#endif

#ifdef SPI_MOD1_CS1_PIN
#define SPI_MOD1_CS1_PCR_MUX	(2)
#endif

#ifdef SPI_MOD1_CS2_PIN
#define SPI_MOD1_CS2_PCR_MUX	(2)
#endif

#ifdef SPI_MOD1_CS3_PIN
#define SPI_MOD1_CS3_PCR_MUX	(2)
#endif

#ifdef SPI_MOD2_SCK_PIN
#define SPI_MOD2_SCK_PCR_MUX	(2)
#endif

#ifdef SPI_MOD2_SOUT_PIN
#define SPI_MOD2_SOUT_PCR_MUX	(2)
#endif

#ifdef SPI_MOD2_SIN_PIN
#define SPI_MOD2_SIN_PCR_MUX	(2)
#endif

#ifdef SPI_MOD2_CS0_PIN
#define SPI_MOD2_CS0_PCR_MUX	(2)
#endif

#ifdef SPI_MOD2_CS1_PIN
#define SPI_MOD2_CS1_PCR_MUX	(2)
#endif

#endif
