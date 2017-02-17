//==========================================================================
//文件名称：dma.h
//功能概要：K64 DMA底层驱动程序头文件
//==========================================================================

#ifndef _DMA_H
#define _DMA_H

//包含公共头文件
#include "common.h"

//定义DMA通道号
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

//定义DMA通道的请求源
#define DMA_REQ_DISABLED		(0)		//关闭该通道的DMA请求源
#define DMA_REQ_UART0_RX		(2)		//UART0接收触发DMA请求
#define DMA_REQ_UART0_TX		(3)		//UART0发送触发DMA请求
#define DMA_REQ_UART1_RX		(4)		//UART1接收触发DMA请求
#define DMA_REQ_UART1_TX		(5)		//UART1发送触发DMA请求
#define DMA_REQ_UART2_RX		(6)		//UART2接收触发DMA请求
#define DMA_REQ_UART2_TX		(7)		//UART2发送触发DMA请求
#define DMA_REQ_UART3_RX		(8)		//UART3接收触发DMA请求
#define DMA_REQ_UART3_TX		(9)		//UART3发送触发DMA请求
#define DMA_REQ_UART4_RX_TX		(10)	//UART4接收或发送触发DMA请求
#define DMA_REQ_UART5_RX_TX		(11)	//UART5接收或发送触发DMA请求
#define DMA_REQ_I2S0_RX			(12)	//I2S0接收触发DMA请求
#define DMA_REQ_I2S0_TX			(13)	//I2S0发送触发DMA请求
#define DMA_REQ_SPI0_RX			(14)	//SPI0接收触发DMA请求
#define DMA_REQ_SPI0_TX			(15)	//SPI0发送触发DMA请求
#define DMA_REQ_SPI1_RX_TX		(16)	//SPI1接收或发送触发DMA请求
#define DMA_REQ_SPI2_RX_TX		(17)	//SPI2接收或发送触发DMA请求
#define DMA_REQ_I2C0			(18)	//I2C0触发DMA请求
#define DMA_REQ_I2C1_I2C2		(19)	//I2C1或I2C2触发DMA请求
#define DMA_REQ_FTM0_CH0		(20)	//FTM0的0通道触发DMA请求
#define DMA_REQ_FTM0_CH1		(21)	//FTM0的1通道触发DMA请求
#define DMA_REQ_FTM0_CH2		(22)	//FTM0的2通道触发DMA请求
#define DMA_REQ_FTM0_CH3		(23)	//FTM0的3通道触发DMA请求
#define DMA_REQ_FTM0_CH4		(24)	//FTM0的4通道触发DMA请求
#define DMA_REQ_FTM0_CH5		(25)	//FTM0的5通道触发DMA请求
#define DMA_REQ_FTM0_CH6		(26)	//FTM0的6通道触发DMA请求
#define DMA_REQ_FTM0_CH7		(27)	//FTM0的7通道触发DMA请求
#define DMA_REQ_FTM1_CH0		(28)	//FTM1的0通道触发DMA请求
#define DMA_REQ_FTM1_CH1		(29)	//FTM1的1通道触发DMA请求
#define DMA_REQ_FTM2_CH0		(30)	//FTM2的0通道触发DMA请求
#define DMA_REQ_FTM2_CH1		(31)	//FTM2的1通道触发DMA请求
#define DMA_REQ_FTM3_CH0		(32)	//FTM3的0通道触发DMA请求
#define DMA_REQ_FTM3_CH1		(33)	//FTM3的1通道触发DMA请求
#define DMA_REQ_FTM3_CH2		(34)	//FTM3的2通道触发DMA请求
#define DMA_REQ_FTM3_CH3		(35)	//FTM3的3通道触发DMA请求
#define DMA_REQ_FTM3_CH4		(36)	//FTM3的4通道触发DMA请求
#define DMA_REQ_FTM3_CH5		(37)	//FTM3的5通道触发DMA请求
#define DMA_REQ_FTM3_CH6		(38)	//FTM3的6通道触发DMA请求
#define DMA_REQ_FTM3_CH7		(39)	//FTM3的7通道触发DMA请求
#define DMA_REQ_ADC0			(40)	//ADC0触发DMA请求
#define DMA_REQ_ADC1			(41)	//ADC1触发DMA请求
#define DMA_REQ_CMP0			(42)	//CMP0触发DMA请求
#define DMA_REQ_CMP1			(43)	//CMP1触发DMA请求
#define DMA_REQ_CMP2			(44)	//CMP2触发DMA请求
#define DMA_REQ_DAC0			(45)	//DAC0触发DMA请求
#define DMA_REQ_DAC1			(46)	//DAC1触发DMA请求
#define DMA_REQ_CMT				(47)	//CMT触发DMA请求
#define DMA_REQ_PDB				(48)	//PDB触发DMA请求
#define DMA_REQ_PORTA			(49)	//PORTA触发DMA请求
#define DMA_REQ_PORTB			(50)	//PORTB触发DMA请求
#define DMA_REQ_PORTC			(51)	//PORTC触发DMA请求
#define DMA_REQ_PORTD			(52)	//PORTD触发DMA请求
#define DMA_REQ_PORTE			(53)	//PORTE触发DMA请求
#define DMA_REQ_IEEE1588_TIMER0	(54)	//IEEE1588计时器0触发DMA请求
#define DMA_REQ_IEEE1588_TIMER1	(55)	//IEEE1588计时器1触发DMA请求
#define DMA_REQ_IEEE1588_TIMER2	(56)	//IEEE1588计时器2触发DMA请求
#define DMA_REQ_IEEE1588_TIMER3	(57)	//IEEE1588计时器3触发DMA请求
#define DMA_REQ_ALWAYS_EN0		(58)	//始终触发DMA请求
#define DMA_REQ_ALWAYS_EN1		(59)	//始终触发DMA请求
#define DMA_REQ_ALWAYS_EN2		(60)	//始终触发DMA请求
#define DMA_REQ_ALWAYS_EN3		(61)	//始终触发DMA请求
#define DMA_REQ_ALWAYS_EN4		(62)	//始终触发DMA请求
#define DMA_REQ_ALWAYS_EN5		(63)	//始终触发DMA请求

//定义DMA通道的触发模式
#define DMA_MODE_NORMAL			(0)		//正常模式
#define DMA_MODE_PERIODIC		(1)		//周期触发模式

//定义DMA数据宽度
#define DMA_DATA_WIDTH_BYTE_1	(0)		//数据宽度为1个字节
#define DMA_DATA_WIDTH_BYTE_2	(1)		//数据宽度为2个字节
#define DMA_DATA_WIDTH_BYTE_4	(2)		//数据宽度为4个字节
#define DMA_DATA_WIDTH_BYTE_16	(4)		//数据宽度为16个字节
#define DMA_DATA_WIDTH_BYTE_32	(5)		//数据宽度为32个字节

//定义DMA模数大小
#define DMA_MODULO_DISABLED		(0x00)	//禁用模数功能
#define DMA_MODULO_BYTE_2		(0x01)	//模数大小为2个字节
#define DMA_MODULO_BYTE_4		(0x02)	//模数大小为4个字节
#define DMA_MODULO_BYTE_8		(0x03)	//模数大小为8个字节
#define DMA_MODULO_BYTE_16		(0x04)	//模数大小为16个字节
#define DMA_MODULO_BYTE_32		(0x05)	//模数大小为32个字节
#define DMA_MODULO_BYTE_64		(0x06)	//模数大小为64个字节
#define DMA_MODULO_BYTE_128		(0x07)	//模数大小为128个字节
#define DMA_MODULO_BYTE_256		(0x08)	//模数大小为256个字节
#define DMA_MODULO_BYTE_512		(0x09)	//模数大小为512个字节
#define DMA_MODULO_BYTE_1K		(0x0A)	//模数大小为1K个字节
#define DMA_MODULO_BYTE_2K		(0x0B)	//模数大小为2K个字节
#define DMA_MODULO_BYTE_4K		(0x0C)	//模数大小为4K个字节
#define DMA_MODULO_BYTE_8K		(0x0D)	//模数大小为8K个字节
#define DMA_MODULO_BYTE_16K		(0x0E)	//模数大小为16K个字节
#define DMA_MODULO_BYTE_32K		(0x0F)	//模数大小为32K个字节
#define DMA_MODULO_BYTE_64K		(0x10)	//模数大小为64K个字节
#define DMA_MODULO_BYTE_128K	(0x11)	//模数大小为128K个字节
#define DMA_MODULO_BYTE_256K	(0x12)	//模数大小为256K个字节
#define DMA_MODULO_BYTE_512K	(0x13)	//模数大小为512K个字节
#define DMA_MODULO_BYTE_1M		(0x14)	//模数大小为1M个字节
#define DMA_MODULO_BYTE_2M		(0x15)	//模数大小为2M个字节
#define DMA_MODULO_BYTE_4M		(0x16)	//模数大小为4M个字节
#define DMA_MODULO_BYTE_8M		(0x17)	//模数大小为8M个字节
#define DMA_MODULO_BYTE_16M		(0x18)	//模数大小为16M个字节
#define DMA_MODULO_BYTE_32M		(0x19)	//模数大小为32M个字节
#define DMA_MODULO_BYTE_64M		(0x1A)	//模数大小为64M个字节
#define DMA_MODULO_BYTE_128M	(0x1B)	//模数大小为128M个字节
#define DMA_MODULO_BYTE_256M	(0x1C)	//模数大小为256M个字节
#define DMA_MODULO_BYTE_512M	(0x1D)	//模数大小为512M个字节
#define DMA_MODULO_BYTE_1G		(0x1E)	//模数大小为1G个字节
#define DMA_MODULO_BYTE_2G		(0x1F)	//模数大小为2G个字节

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
