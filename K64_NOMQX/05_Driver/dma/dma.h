//==========================================================================
//�ļ����ƣ�dma.h
//���ܸ�Ҫ��K64 DMA�ײ���������ͷ�ļ�
//==========================================================================

#ifndef _DMA_H
#define _DMA_H

//��������ͷ�ļ�
#include "common.h"

//����DMAͨ����
#define DMA_CH0		(0)
#define DMA_CH1		(1)
#define DMA_CH2		(2)
#define DMA_CH3		(3)
#define DMA_CH4		(4)
#define DMA_CH5		(5)
#define DMA_CH6		(6)
#define DMA_CH7		(7)
#define DMA_CH8		(8)
#define DMA_CH9		(9)
#define DMA_CH10	(10)
#define DMA_CH11	(11)
#define DMA_CH12	(12)
#define DMA_CH13	(13)
#define DMA_CH14	(14)
#define DMA_CH15	(15)

//����DMAͨ��������Դ
#define DMA_REQ_DISABLED		(0)		//�رո�ͨ����DMA����Դ
#define DMA_REQ_UART0_RX		(2)		//UART0���մ���DMA����
#define DMA_REQ_UART0_TX		(3)		//UART0���ʹ���DMA����
#define DMA_REQ_UART1_RX		(4)		//UART1���մ���DMA����
#define DMA_REQ_UART1_TX		(5)		//UART1���ʹ���DMA����
#define DMA_REQ_UART2_RX		(6)		//UART2���մ���DMA����
#define DMA_REQ_UART2_TX		(7)		//UART2���ʹ���DMA����
#define DMA_REQ_UART3_RX		(8)		//UART3���մ���DMA����
#define DMA_REQ_UART3_TX		(9)		//UART3���ʹ���DMA����
#define DMA_REQ_UART4_RX_TX		(10)	//UART4���ջ��ʹ���DMA����
#define DMA_REQ_UART5_RX_TX		(11)	//UART5���ջ��ʹ���DMA����
#define DMA_REQ_I2S0_RX			(12)	//I2S0���մ���DMA����
#define DMA_REQ_I2S0_TX			(13)	//I2S0���ʹ���DMA����
#define DMA_REQ_SPI0_RX			(14)	//SPI0���մ���DMA����
#define DMA_REQ_SPI0_TX			(15)	//SPI0���ʹ���DMA����
#define DMA_REQ_SPI1_RX_TX		(16)	//SPI1���ջ��ʹ���DMA����
#define DMA_REQ_SPI2_RX_TX		(17)	//SPI2���ջ��ʹ���DMA����
#define DMA_REQ_I2C0			(18)	//I2C0����DMA����
#define DMA_REQ_I2C1_I2C2		(19)	//I2C1��I2C2����DMA����
#define DMA_REQ_FTM0_CH0		(20)	//FTM0��0ͨ������DMA����
#define DMA_REQ_FTM0_CH1		(21)	//FTM0��1ͨ������DMA����
#define DMA_REQ_FTM0_CH2		(22)	//FTM0��2ͨ������DMA����
#define DMA_REQ_FTM0_CH3		(23)	//FTM0��3ͨ������DMA����
#define DMA_REQ_FTM0_CH4		(24)	//FTM0��4ͨ������DMA����
#define DMA_REQ_FTM0_CH5		(25)	//FTM0��5ͨ������DMA����
#define DMA_REQ_FTM0_CH6		(26)	//FTM0��6ͨ������DMA����
#define DMA_REQ_FTM0_CH7		(27)	//FTM0��7ͨ������DMA����
#define DMA_REQ_FTM1_CH0		(28)	//FTM1��0ͨ������DMA����
#define DMA_REQ_FTM1_CH1		(29)	//FTM1��1ͨ������DMA����
#define DMA_REQ_FTM2_CH0		(30)	//FTM2��0ͨ������DMA����
#define DMA_REQ_FTM2_CH1		(31)	//FTM2��1ͨ������DMA����
#define DMA_REQ_FTM3_CH0		(32)	//FTM3��0ͨ������DMA����
#define DMA_REQ_FTM3_CH1		(33)	//FTM3��1ͨ������DMA����
#define DMA_REQ_FTM3_CH2		(34)	//FTM3��2ͨ������DMA����
#define DMA_REQ_FTM3_CH3		(35)	//FTM3��3ͨ������DMA����
#define DMA_REQ_FTM3_CH4		(36)	//FTM3��4ͨ������DMA����
#define DMA_REQ_FTM3_CH5		(37)	//FTM3��5ͨ������DMA����
#define DMA_REQ_FTM3_CH6		(38)	//FTM3��6ͨ������DMA����
#define DMA_REQ_FTM3_CH7		(39)	//FTM3��7ͨ������DMA����
#define DMA_REQ_ADC0			(40)	//ADC0����DMA����
#define DMA_REQ_ADC1			(41)	//ADC1����DMA����
#define DMA_REQ_CMP0			(42)	//CMP0����DMA����
#define DMA_REQ_CMP1			(43)	//CMP1����DMA����
#define DMA_REQ_CMP2			(44)	//CMP2����DMA����
#define DMA_REQ_DAC0			(45)	//DAC0����DMA����
#define DMA_REQ_DAC1			(46)	//DAC1����DMA����
#define DMA_REQ_CMT				(47)	//CMT����DMA����
#define DMA_REQ_PDB				(48)	//PDB����DMA����
#define DMA_REQ_PORTA			(49)	//PORTA����DMA����
#define DMA_REQ_PORTB			(50)	//PORTB����DMA����
#define DMA_REQ_PORTC			(51)	//PORTC����DMA����
#define DMA_REQ_PORTD			(52)	//PORTD����DMA����
#define DMA_REQ_PORTE			(53)	//PORTE����DMA����
#define DMA_REQ_IEEE1588_TIMER0	(54)	//IEEE1588��ʱ��0����DMA����
#define DMA_REQ_IEEE1588_TIMER1	(55)	//IEEE1588��ʱ��1����DMA����
#define DMA_REQ_IEEE1588_TIMER2	(56)	//IEEE1588��ʱ��2����DMA����
#define DMA_REQ_IEEE1588_TIMER3	(57)	//IEEE1588��ʱ��3����DMA����
#define DMA_REQ_ALWAYS_EN0		(58)	//ʼ�մ���DMA����
#define DMA_REQ_ALWAYS_EN1		(59)	//ʼ�մ���DMA����
#define DMA_REQ_ALWAYS_EN2		(60)	//ʼ�մ���DMA����
#define DMA_REQ_ALWAYS_EN3		(61)	//ʼ�մ���DMA����
#define DMA_REQ_ALWAYS_EN4		(62)	//ʼ�մ���DMA����
#define DMA_REQ_ALWAYS_EN5		(63)	//ʼ�մ���DMA����

//����DMAͨ���Ĵ���ģʽ
#define DMA_MODE_NORMAL			(0)		//����ģʽ
#define DMA_MODE_PERIODIC		(1)		//���ڴ���ģʽ

//����DMA���ݿ��
#define DMA_DATA_WIDTH_BYTE_1	(0)		//���ݿ��Ϊ1���ֽ�
#define DMA_DATA_WIDTH_BYTE_2	(1)		//���ݿ��Ϊ2���ֽ�
#define DMA_DATA_WIDTH_BYTE_4	(2)		//���ݿ��Ϊ4���ֽ�
#define DMA_DATA_WIDTH_BYTE_16	(4)		//���ݿ��Ϊ16���ֽ�
#define DMA_DATA_WIDTH_BYTE_32	(5)		//���ݿ��Ϊ32���ֽ�

//����DMAģ����С
#define DMA_MODULO_DISABLED		(0x00)	//����ģ������
#define DMA_MODULO_BYTE_2		(0x01)	//ģ����СΪ2���ֽ�
#define DMA_MODULO_BYTE_4		(0x02)	//ģ����СΪ4���ֽ�
#define DMA_MODULO_BYTE_8		(0x03)	//ģ����СΪ8���ֽ�
#define DMA_MODULO_BYTE_16		(0x04)	//ģ����СΪ16���ֽ�
#define DMA_MODULO_BYTE_32		(0x05)	//ģ����СΪ32���ֽ�
#define DMA_MODULO_BYTE_64		(0x06)	//ģ����СΪ64���ֽ�
#define DMA_MODULO_BYTE_128		(0x07)	//ģ����СΪ128���ֽ�
#define DMA_MODULO_BYTE_256		(0x08)	//ģ����СΪ256���ֽ�
#define DMA_MODULO_BYTE_512		(0x09)	//ģ����СΪ512���ֽ�
#define DMA_MODULO_BYTE_1K		(0x0A)	//ģ����СΪ1K���ֽ�
#define DMA_MODULO_BYTE_2K		(0x0B)	//ģ����СΪ2K���ֽ�
#define DMA_MODULO_BYTE_4K		(0x0C)	//ģ����СΪ4K���ֽ�
#define DMA_MODULO_BYTE_8K		(0x0D)	//ģ����СΪ8K���ֽ�
#define DMA_MODULO_BYTE_16K		(0x0E)	//ģ����СΪ16K���ֽ�
#define DMA_MODULO_BYTE_32K		(0x0F)	//ģ����СΪ32K���ֽ�
#define DMA_MODULO_BYTE_64K		(0x10)	//ģ����СΪ64K���ֽ�
#define DMA_MODULO_BYTE_128K	(0x11)	//ģ����СΪ128K���ֽ�
#define DMA_MODULO_BYTE_256K	(0x12)	//ģ����СΪ256K���ֽ�
#define DMA_MODULO_BYTE_512K	(0x13)	//ģ����СΪ512K���ֽ�
#define DMA_MODULO_BYTE_1M		(0x14)	//ģ����СΪ1M���ֽ�
#define DMA_MODULO_BYTE_2M		(0x15)	//ģ����СΪ2M���ֽ�
#define DMA_MODULO_BYTE_4M		(0x16)	//ģ����СΪ4M���ֽ�
#define DMA_MODULO_BYTE_8M		(0x17)	//ģ����СΪ8M���ֽ�
#define DMA_MODULO_BYTE_16M		(0x18)	//ģ����СΪ16M���ֽ�
#define DMA_MODULO_BYTE_32M		(0x19)	//ģ����СΪ32M���ֽ�
#define DMA_MODULO_BYTE_64M		(0x1A)	//ģ����СΪ64M���ֽ�
#define DMA_MODULO_BYTE_128M	(0x1B)	//ģ����СΪ128M���ֽ�
#define DMA_MODULO_BYTE_256M	(0x1C)	//ģ����СΪ256M���ֽ�
#define DMA_MODULO_BYTE_512M	(0x1D)	//ģ����СΪ512M���ֽ�
#define DMA_MODULO_BYTE_1G		(0x1E)	//ģ����СΪ1G���ֽ�
#define DMA_MODULO_BYTE_2G		(0x1F)	//ģ����СΪ2G���ֽ�

void dma_init(uint8 ch, uint8 req, uint8 mode, uint32 minor_loop_bytes,
		uint16 major_loop_iteration_cnt, uint32 src_addr, uint8 src_data_width,
		int16 src_addr_offset, uint8 src_modulo, int32 src_addr_last_adj,
		uint32 dest_addr, uint8 dest_data_width, int16 dest_addr_offset,
		uint8 dest_modulo, int32 dest_addr_last_adj, bool auto_disable_req);

void dma_enable_req(uint8 ch);

void dma_disable_req(uint8 ch);

void dma_enable_major_int(uint8 ch);

void dma_disable_major_int(uint8 ch);

bool dma_get_major_int(uint8 ch);

void dma_clear_major_int(uint8 ch);

void dma_software_req(uint8 ch);

#endif
