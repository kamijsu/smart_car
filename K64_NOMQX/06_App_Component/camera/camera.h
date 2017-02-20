//==========================================================================
//�ļ����ƣ�camera.h
//���ܸ�Ҫ������ͷӦ����������ͷ�ļ�
//==========================================================================

#ifndef _CAMERA_H
#define _CAMERA_H

#include "common.h"
#include "gpio.h"
#include "i2c.h"

//��������ͷ�������ź����ţ�D0-D7����Ϊ������8�����ţ���D0�����źű���Ϊ8�ı�����
//�����ſ���ʹ��DMA�ɼ�ͼ������
#define CAMERA_D0		(COM_PORTC|8)	//C8
#define CAMERA_D1		(COM_PORTC|9)	//C9
#define CAMERA_D2		(COM_PORTC|10)	//C10
#define CAMERA_D3		(COM_PORTC|11)	//C11
#define CAMERA_D4		(COM_PORTC|12)	//C12
#define CAMERA_D5		(COM_PORTC|13)	//C13
#define CAMERA_D6		(COM_PORTC|14)	//C14
#define CAMERA_D7		(COM_PORTC|15)	//C15
//��������ͷ�ĳ��ж��ź�����
#define CAMERA_VSYNC	(COM_PORTC|16)	//C16
//��������ͷ��ʱ���ź�����
#define CAMERA_PCLK		(COM_PORTC|7)	//C7

//��������ͷ��ʹ�õ�I2Cģ��
#define CAMERA_I2C_MOD		I2C_MOD0	//I2C0��SCL:B2��SDA:B3

//��������ͷ�ĵ�ַ
#define CAMERA_I2C_ADDR		(0x42)

#endif
