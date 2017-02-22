//==========================================================================
//�ļ����ƣ�camera.h
//���ܸ�Ҫ������ͷӦ����������ͷ�ļ�
//==========================================================================

#ifndef _CAMERA_H
#define _CAMERA_H

#include "common.h"
#include "gpio.h"
#include "i2c.h"
#include "dma.h"
#include "pit.h"

//��Ҫ�ṩһ�����õ���ʱ����
#define CAMERA_DELAY_US(us)		pit_delay_us(PIT_CH1, us)

//��������ͷ�������ź����ţ�D0-D7����Ϊ������8�����ţ���D0�����źű���Ϊ8�ı�����
//�����ſ���ʹ��DMA�ɼ�ͼ�����ݣ������ź����ž�����DMA����Դ��Դ��ַ
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

//��������ͷ��ʹ�õ�I2Cģ�����ã���ѡ���ü�i2c.h(ע��:����ͬI2Cģ�鱻���ڲ�ͬӦ�ù������뱣֤���ǵ�������ͬ)
//������ʹ�õ�I2Cģ��
#define CAMERA_I2C_MOD		I2C_MOD0	//I2C0��SCL:B2��SDA:B3
//����I2Cģ��ĳ������Ӻ�ʱ������
#define CAMERA_I2C_MUL		I2C_MUL_4	//��������Ϊ4
#define CAMERA_I2C_ICR		(0x05)		//����ʱ��Ƶ��Ϊ48MHz����������Ϊ4ʱ��������Ϊ400Kbps
//����I2Cģ��ĵ�ַģʽ�͵�ַ
#define CAMERA_I2C_ADDR_MODE	I2C_ADDR_MODE_BITS_7	//7λ��ַģʽ
#define CAMERA_I2C_ADDR			(0x10)					//��ַΪ0x10
//����I2Cģ���Ƿ�ʹ�ܹ㲥���е�ַ
#define CAMERA_I2C_ENABLE_GENERAL_CALL_ADDR		false	//�رչ㲥���е�ַ

//��������ͷ�ĵ�ַ
#define CAMERA_ADDR		(0x21)	//������I2C��7λ��ַ��׼����ַΪ0x21��������SCCB��8λ��ַ��׼����ַΪ0x42����ΪҪ����һλ

//��������ͷ��ʹ�õ�DMAͨ�����ã���ѡ���ü�dma.h
//������ʹ�õ�DMAͨ��
#define CAMERA_DMA_CH		DMA_CH0			//DMA0
//����DMAͨ��������Դ���������ź����ž����˿ں�
#define CAMERA_DMA_REQ		DMA_REQ_PORTC	//PORTC����DMA����
//����DMAͨ����Դ��ַ���������ź����ž�����������Ĵ����ĵ�ַ
#define CAMERA_DMA_SRC_ADDR	((uint32)&GPIOC_PDIR + 1)	//GPIOC����������Ĵ���������ƫ��1���ֽ�

//��������ͷ�ɼ�ͼ��Ŀ�Ⱥ͸߶ȣ���λΪ���أ���ѡ����(��*��)Ϊ80*60��160*120��240*180��320*240
//����ͼ����
#define CAMERA_IMG_WIDTH	(80)
//����ͼ��߶�
#define CAMERA_IMG_HEIGHT	(60)
//����ԭʼͼ���ֽ���
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
