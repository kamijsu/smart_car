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

//定义SPI配置号
#define SPI_CONFIG0		(0)
#define SPI_CONFIG1		(1)

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

//定义SPI各模块在主机模式下是否启用片选信号，true为启用，false为不启用，
//若无该片选信号，请设置false;从机模式下，无论如何设置，都会启用CS0
#define SPI_MOD0_CS0_ENABLE		true
#define SPI_MOD0_CS1_ENABLE		false
#define SPI_MOD0_CS2_ENABLE		false
#define SPI_MOD0_CS3_ENABLE		false
#define SPI_MOD0_CS4_ENABLE		false
#define SPI_MOD0_CS5_ENABLE		false

#define SPI_MOD1_CS0_ENABLE		true
#define SPI_MOD1_CS1_ENABLE		false
#define SPI_MOD1_CS2_ENABLE		false
#define SPI_MOD1_CS3_ENABLE		false
#define SPI_MOD1_CS4_ENABLE		false
#define SPI_MOD1_CS5_ENABLE		false

#define SPI_MOD2_CS0_ENABLE		true
#define SPI_MOD2_CS1_ENABLE		false
#define SPI_MOD2_CS2_ENABLE		false
#define SPI_MOD2_CS3_ENABLE		false
#define SPI_MOD2_CS4_ENABLE		false
#define SPI_MOD2_CS5_ENABLE		false

//定义SPI帧格式
#define SPI_FORMAT_CPOL0_CPHA0	(0)	//平时电平为低，第一个跳变取数，第二个跳变数据上线
#define SPI_FORMAT_CPOL0_CPHA1	(1)	//平时电平为低，第一个跳变数据上线，第二个跳变取数
#define SPI_FORMAT_CPOL1_CPHA0	(2)	//平时电平为高，第一个跳变取数，第二个跳变数据上线
#define SPI_FORMAT_CPOL1_CPHA1	(3)	//平时电平为高，第一个跳变数据上线，第二个跳变取数

//定义SPI帧数据位数
#define SPI_DATA_SIZE_8			(7)		//每帧8位，即1个字节
#define SPI_DATA_SIZE_16		(15)	//每帧16位，即2个字节

//定义SPI模块位传输顺序
#define SPI_BIT_ORDER_MSB		(0)		//最高有效位，即先发送最高位
#define SPI_BIT_ORDER_LSB		(1)		//最低有效位，即先发送最低位

//定义SPI模块工作频率，这里使用的是总线时钟，单位kHz
#define SPI_WORK_FREQ		BUS_CLK_KHZ

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
