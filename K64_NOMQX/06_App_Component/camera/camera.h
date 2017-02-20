//==========================================================================
//文件名称：camera.h
//功能概要：摄像头应用驱动程序头文件
//==========================================================================

#ifndef _CAMERA_H
#define _CAMERA_H

#include "common.h"
#include "gpio.h"
#include "i2c.h"

//定义摄像头的数据信号引脚，D0-D7必须为连续的8个引脚，且D0的引脚号必须为8的倍数，
//这样才可以使用DMA采集图像数据
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

//定义摄像头所使用的I2C模块
#define CAMERA_I2C_MOD		I2C_MOD0	//I2C0，SCL:B2，SDA:B3

//定义摄像头的地址
#define CAMERA_I2C_ADDR		(0x42)

#endif
