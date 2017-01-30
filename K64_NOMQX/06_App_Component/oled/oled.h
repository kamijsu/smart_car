//==========================================================================
//文件名称：oled.h
//功能概要：OLED应用驱动程序头文件
//==========================================================================

#ifndef _OLED_H
#define _OLED_H

#include "common.h"
#include "gpio.h"
#include "spi.h"

//定义OLED所使用的复位信号引脚
#define OLED_RST		(COM_PORTC|5)	//C5
//定义OLED所使用的数据/命令选择信号引脚
#define OLED_DC			(COM_PORTC|6)	//C6

//定义OLED所使用的SPI模块
#define OLED_SPI_MOD	SPI_MOD2	//SPI2，SCK:B21，SOUT:B22，SIN:B23(未用)
//定义OLED所使用的片选信号
#define OLED_SPI_CS		SPI_CS0		//SPI2，CS0:B20
//定义所使用SPI模块的配置号(注意:若相同SPI模块的相同配置号被用于不同应用构件，请保证它们的配置相同)
#define OLED_SPI_CONFIG			SPI_CONFIG0		//0号配置号
//定义所使用SPI模块的波特率分频因子
#define OLED_SPI_BAUD_SCALER	SPI_BAUD_SCALER_2	//总线时钟频率为48MHz时，波特率为24Mbps
//定义所使用SPI模块的帧格式
#define OLED_SPI_FORMAT		SPI_FORMAT_CPOL0_CPHA0	//平时低电平，第一跳变取数，第二跳变数据上线
//定义所使用SPI模块的帧数据位数
#define OLED_SPI_DATA_SIZE		(8)					//8位，即每帧发送一个字节数据
//定义所使用SPI模块的位传输顺序
#define OLED_SPI_BIT_ORDER		SPI_BIT_ORDER_MSB	//最高有效位，即先发送最高位
//定义所使用SPI模块的CSC延时时间
#define OLED_SPI_CSC_SCALER		SPI_DELAY_SCALER_2	//总线时钟频率为48MHz时，延时时间为41.67ns
//定义所使用SPI模块的ASC延时时间
#define OLED_SPI_ASC_SCALER		SPI_DELAY_SCALER_2	//总线时钟频率为48MHz时，延时时间为41.67ns
//定义所使用SPI模块的DT延时时间
#define OLED_SPI_DT_SCALER		SPI_DELAY_SCALER_2	//总线时钟频率为48MHz时，延时时间为41.67ns

//定义复位信号有效电平
#define OLED_RST_VALID_LEVEL	GPIO_LEVEL_LOW		//有效电平为低
//根据复位信号有效电平定义无效电平
#if(OLED_RST_VALID_LEVEL == GPIO_LEVEL_LOW)
#define OLED_RST_INVALID_LEVEL	GPIO_LEVEL_HIGH		//无效电平为高
#else
#define OLED_RST_INVALID_LEVEL	GPIO_LEVEL_LOW		//无效电平为低
#endif

//定义数据/命令选择信号的数据选择电平
#define OLED_DC_DATA_LEVEL		GPIO_LEVEL_HIGH		//数据选择电平为高
//根据数据/命令选择信号的数据选择电平定义命令选择电平
#if(OLED_DC_DATA_LEVEL == GPIO_LEVEL_HIGH)
#define OLED_DC_CMD_LEVEL	GPIO_LEVEL_LOW		//命令选择电平为低
#else
#define OLED_DC_CMD_LEVEL	GPIO_LEVEL_HIGH		//命令选择电平为高
#endif

//定义OLED分辨率
#define OLED_WIDTH				(128)				//宽度为128个像素
#define OLED_HEIGHT				(64)				//高度为64个像素

#define OLED_PAGE_NUM			(OLED_HEIGHT/8)

#define OLED_SCROLL_DIR_LEFT	(1)
#define OLED_SCROLL_DIR_RIGHT	(0)

#define OLED_SCROLL_INTERVAL_FRAMES_2	(7)
#define OLED_SCROLL_INTERVAL_FRAMES_3	(4)
#define OLED_SCROLL_INTERVAL_FRAMES_4	(5)
#define OLED_SCROLL_INTERVAL_FRAMES_5	(0)
#define OLED_SCROLL_INTERVAL_FRAMES_25	(6)
#define OLED_SCROLL_INTERVAL_FRAMES_64	(1)
#define OLED_SCROLL_INTERVAL_FRAMES_128	(2)
#define OLED_SCROLL_INTERVAL_FRAMES_256	(3)

//定义寻址方式
#define OLED_ADDRESSING_MODE_HORIZONTAL	(0)
#define OLED_ADDRESSING_MODE_VERTICAL	(1)
#define OLED_ADDRESSING_MODE_PAGE		(2)

//定义引脚配置COM0-COM31和COM32-COM63交换映射是否使能
#define OLED_COM_PIN_CONFIG_REMAP_ENABLE	(0x20)	//使能交换映射
#define OLED_COM_PIN_CONFIG_REMAP_DISABLE	(0x00)	//关闭交换映射

//定义引脚配置的顺序
#define OLED_COM_PIN_CONFIG_SEQUENTIAL		(0x00)	//按顺序映射
#define OLED_COM_PIN_CONFIG_ALTERNATIVE		(0x10)	//交替映射

//定义VCOMH未选中时的电压
#define OLED_VCOMH_065		(0x00)		//0.65*VCC
#define OLED_VCOMH_077		(0x20)		//0.77*VCC
#define OLED_VCOMH_083		(0x30)		//0.83*VCC

void oled_init();

void oled_set_scroll(uint8 dir, uint8 start_page, uint8 end_page,
		uint8 interval);

void oled_set_scroll_with_vertical(uint8 dir, uint8 start_page, uint8 end_page,
		uint8 interval, uint8 fixed_row_num, uint8 scroll_row_num,
		uint8 offset_row_num);

void oled_scroll_start();

void oled_scroll_stop();

void oled_set_start_line(uint8 row);

void oled_set_display_offset(uint8 row);

void oled_set_displayable_rows(uint8 num);

void oled_fill(uint8 data);

void oled_display_str(uint8 col, uint8 page, uint8* str);

#endif
