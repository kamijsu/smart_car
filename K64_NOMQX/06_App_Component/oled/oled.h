//==========================================================================
//�ļ����ƣ�oled.h
//���ܸ�Ҫ��OLEDӦ����������ͷ�ļ�
//==========================================================================

#ifndef _OLED_H
#define _OLED_H

#include "common.h"
#include "gpio.h"
#include "spi.h"

//����OLED��ʹ�õĸ�λ�ź�����
#define OLED_RST		(COM_PORTC|5)	//C5
//����OLED��ʹ�õ�����/����ѡ���ź�����
#define OLED_DC			(COM_PORTC|6)	//C6

//����OLED��ʹ�õ�SPIģ��
#define OLED_SPI_MOD	SPI_MOD2	//SPI2��SCK:B21��SOUT:B22��SIN:B23(δ��)
//����OLED��ʹ�õ�Ƭѡ�ź�
#define OLED_SPI_CS		SPI_CS0		//SPI2��CS0:B20
//������ʹ��SPIģ������ú�(ע��:����ͬSPIģ�����ͬ���úű����ڲ�ͬӦ�ù������뱣֤���ǵ�������ͬ)
#define OLED_SPI_CONFIG			SPI_CONFIG0		//0�����ú�
//������ʹ��SPIģ��Ĳ����ʷ�Ƶ����
#define OLED_SPI_BAUD_SCALER	SPI_BAUD_SCALER_2	//����ʱ��Ƶ��Ϊ48MHzʱ��������Ϊ24Mbps
//������ʹ��SPIģ���֡��ʽ
#define OLED_SPI_FORMAT		SPI_FORMAT_CPOL0_CPHA0	//ƽʱ�͵�ƽ����һ����ȡ�����ڶ�������������
//������ʹ��SPIģ���֡����λ��
#define OLED_SPI_DATA_SIZE		(8)					//8λ����ÿ֡����һ���ֽ�����
//������ʹ��SPIģ���λ����˳��
#define OLED_SPI_BIT_ORDER		SPI_BIT_ORDER_MSB	//�����Чλ�����ȷ������λ
//������ʹ��SPIģ���CSC��ʱʱ��
#define OLED_SPI_CSC_SCALER		SPI_DELAY_SCALER_2	//����ʱ��Ƶ��Ϊ48MHzʱ����ʱʱ��Ϊ41.67ns
//������ʹ��SPIģ���ASC��ʱʱ��
#define OLED_SPI_ASC_SCALER		SPI_DELAY_SCALER_2	//����ʱ��Ƶ��Ϊ48MHzʱ����ʱʱ��Ϊ41.67ns
//������ʹ��SPIģ���DT��ʱʱ��
#define OLED_SPI_DT_SCALER		SPI_DELAY_SCALER_2	//����ʱ��Ƶ��Ϊ48MHzʱ����ʱʱ��Ϊ41.67ns

//���帴λ�ź���Ч��ƽ
#define OLED_RST_VALID_LEVEL	GPIO_LEVEL_LOW		//��Ч��ƽΪ��
//���ݸ�λ�ź���Ч��ƽ������Ч��ƽ
#if(OLED_RST_VALID_LEVEL == GPIO_LEVEL_LOW)
#define OLED_RST_INVALID_LEVEL	GPIO_LEVEL_HIGH		//��Ч��ƽΪ��
#else
#define OLED_RST_INVALID_LEVEL	GPIO_LEVEL_LOW		//��Ч��ƽΪ��
#endif

//��������/����ѡ���źŵ�����ѡ���ƽ
#define OLED_DC_DATA_LEVEL		GPIO_LEVEL_HIGH		//����ѡ���ƽΪ��
//��������/����ѡ���źŵ�����ѡ���ƽ��������ѡ���ƽ
#if(OLED_DC_DATA_LEVEL == GPIO_LEVEL_HIGH)
#define OLED_DC_CMD_LEVEL	GPIO_LEVEL_LOW		//����ѡ���ƽΪ��
#else
#define OLED_DC_CMD_LEVEL	GPIO_LEVEL_HIGH		//����ѡ���ƽΪ��
#endif

//����OLED�ֱ���
#define OLED_WIDTH				(128)				//���Ϊ128������
#define OLED_HEIGHT				(64)				//�߶�Ϊ64������

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

//����Ѱַ��ʽ
#define OLED_ADDRESSING_MODE_HORIZONTAL	(0)
#define OLED_ADDRESSING_MODE_VERTICAL	(1)
#define OLED_ADDRESSING_MODE_PAGE		(2)

//������������COM0-COM31��COM32-COM63����ӳ���Ƿ�ʹ��
#define OLED_COM_PIN_CONFIG_REMAP_ENABLE	(0x20)	//ʹ�ܽ���ӳ��
#define OLED_COM_PIN_CONFIG_REMAP_DISABLE	(0x00)	//�رս���ӳ��

//�����������õ�˳��
#define OLED_COM_PIN_CONFIG_SEQUENTIAL		(0x00)	//��˳��ӳ��
#define OLED_COM_PIN_CONFIG_ALTERNATIVE		(0x10)	//����ӳ��

//����VCOMHδѡ��ʱ�ĵ�ѹ
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
