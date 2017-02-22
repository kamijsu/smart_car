//==========================================================================
//文件名称：camera.h
//功能概要：摄像头应用驱动程序头文件
//==========================================================================

#ifndef _CAMERA_H
#define _CAMERA_H

#include "common.h"
#include "gpio.h"
#include "i2c.h"
#include "dma.h"
#include "pit.h"

//需要提供一个可用的延时函数
#define CAMERA_DELAY_US(us)		pit_delay_us(PIT_CH1, us)

//定义摄像头的数据信号引脚，D0-D7必须为连续的8个引脚，且D0的引脚号必须为8的倍数，
//这样才可以使用DMA采集图像数据，数据信号引脚决定了DMA请求源和源地址
#define CAMERA_D0		(COM_PORTC|8)	//C8
#define CAMERA_D1		(COM_PORTC|9)	//C9
#define CAMERA_D2		(COM_PORTC|10)	//C10
#define CAMERA_D3		(COM_PORTC|11)	//C11
#define CAMERA_D4		(COM_PORTC|12)	//C12
#define CAMERA_D5		(COM_PORTC|13)	//C13
#define CAMERA_D6		(COM_PORTC|14)	//C14
#define CAMERA_D7		(COM_PORTC|15)	//C15
//定义摄像头的场中断信号引脚
#define CAMERA_VSYNC	(COM_PORTC|16)	//C16
//定义摄像头的时钟信号引脚
#define CAMERA_PCLK		(COM_PORTC|7)	//C7

//定义摄像头所使用的I2C模块配置，可选配置见i2c.h(注意:若相同I2C模块被用于不同应用构件，请保证它们的配置相同)
//定义所使用的I2C模块
#define CAMERA_I2C_MOD		I2C_MOD0	//I2C0，SCL:B2，SDA:B3
//定义I2C模块的乘数因子和时钟速率
#define CAMERA_I2C_MUL		I2C_MUL_4	//乘数因子为4
#define CAMERA_I2C_ICR		(0x05)		//总线时钟频率为48MHz，乘数因子为4时，波特率为400Kbps
//定义I2C模块的地址模式和地址
#define CAMERA_I2C_ADDR_MODE	I2C_ADDR_MODE_BITS_7	//7位地址模式
#define CAMERA_I2C_ADDR			(0x10)					//地址为0x10
//定义I2C模块是否使能广播呼叫地址
#define CAMERA_I2C_ENABLE_GENERAL_CALL_ADDR		false	//关闭广播呼叫地址

//定义摄像头的地址
#define CAMERA_ADDR		(0x21)	//若按照I2C的7位地址标准，地址为0x21，而按照SCCB的8位地址标准，地址为0x42，因为要左移一位

//定义摄像头所使用的DMA通道配置，可选配置见dma.h
//定义所使用的DMA通道
#define CAMERA_DMA_CH		DMA_CH0			//DMA0
//定义DMA通道的请求源，由数据信号引脚决定端口号
#define CAMERA_DMA_REQ		DMA_REQ_PORTC	//PORTC触发DMA请求
//定义DMA通道的源地址，由数据信号引脚决定数据输入寄存器的地址
#define CAMERA_DMA_SRC_ADDR	((uint32)&GPIOC_PDIR + 1)	//GPIOC的数据输入寄存器，并且偏移1个字节

//定义摄像头采集图像的宽度和高度，单位为像素，可选参数(宽*高)为80*60、160*120、240*180、320*240
//定义图像宽度
#define CAMERA_IMG_WIDTH	(80)
//定义图像高度
#define CAMERA_IMG_HEIGHT	(60)
//定义原始图像字节数
#define CAMERA_RAW_IMG_BYTES	(CAMERA_IMG_WIDTH * CAMERA_IMG_HEIGHT / 8)

void camera_init(uint8* raw_img);

void camera_enable_vsync_int();

void camera_disable_vsync_int();

bool camera_get_vsync_int();

void camera_clear_vsync_int();

void camera_start_collecting();

void camera_stop_collecting();

void camera_enable_collect_done_int();

void camera_disable_collect_done_int();

bool camera_get_collect_done_int();

void camera_clear_collect_done_int();

void vcan_sendimg(void *imgaddr, uint32_t imgsize);

#endif
