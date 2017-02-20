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

//定义DMA仲裁模式
#define DMA_ARBITRATION_MODE_FIXED			(0)	//固定优先级仲裁
#define DMA_ARBITRATION_MODE_ROUND_ROBIN	(1)	//轮询仲裁

//定义DMA通道状态
typedef enum {
	DMAChannelIdle,			//通道空闲
	DMAChannelStarting,		//通道正在启动
	DMAChannelExecuting,	//通道正在执行
	DMAChannelDone			//通道完成主循环
} DMAChannelState;

//==========================================================================
//函数名称: dma_init
//函数返回: 无
//参数说明: ch:DMA通道号:
//            DMA_CHx，x为通道号;
//         req:该通道的请求源:
//             DMA_REQ_xxx，具体见头文件中的宏定义;
//         mode:该通道的触发模式:
//              DMA_MODE_NORMAL:  正常模式;
//              DMA_MODE_PERIODIC:周期触发模式;
//         minor_loop_bytes:每次副循环传输字节数，取值范围为[1,4294967295]
//         major_loop_iteration_cnt:主循环迭代次数，即一次主循环中副循环的次数，
//                                  取值范围为[1,32767]，但若要使能副循环通道连接，
//                                  取值范围变为[1,511]
//         src_addr:源地址
//         src_data_width:源地址数据宽度，即每次从源地址读取的字节数:
//                        DMA_DATA_WIDTH_BYTE_x，数据宽度为x个字节，x可取值为:
//                        1 | 2 | 4 | 16 | 32
//         src_addr_offset:源地址每次被读取后的偏移量，取值范围为[-32768,32767]
//         src_modulo:源地址模数大小:
//                    DMA_MODULO_DISABLED:禁用模数功能;
//                    DMA_MODULO_BYTE_x:模数大小为x个字节，x可取值为:
//                    2  | 4  | 8  | 16 | 32  | 64  | 128 | 256  | 512  |
//                    1K | 2K | 4K | 8K | 16K | 32K | 64K | 128K | 256K | 512K |
//                    1M | 2M | 4M | 8M | 16M | 32M | 64M | 128M | 256M | 512M |
//                    1G | 2G
//         src_addr_last_adj:主循环完成后源地址的偏移量，
//                           取值范围为[-2147483648,2147483647]
//         dest_addr:目标地址
//         dest_data_width:目标地址数据宽度，即每次向目标地址写入的字节数，
//                         可选参数同src_data_width
//         dest_addr_offset:目标地址每次被写入后的偏移量，取值范围同src_addr_offset
//         dest_modulo:目标地址模数大小，可选参数同src_modulo
//         dest_addr_last_adj:主循环完成后目标地址的偏移量，
//                            取值范围同src_addr_last_adj
//         auto_disable_req:是否使能主循环完成后自动不接收DMA请求:
//                          true: 使能主循环完成后自动不接收DMA请求;
//                          false:关闭主循环完成后自动不接收DMA请求;
//功能概要: 初始化DMA通道，默认关闭主循环完成中断，默认不接收该通道的DMA请求，
//         默认关闭主循环和副循环通道连接
//备注: 一次DMA请求被接收后，进行一次副循环;
//
//     不同通道不要设置相同的请求源，这会造成不可知的结果(关闭请求源和始终触发除外)，
//     请求源若要可以触发DMA请求，需使能相应请求源的DMA功能;
//
//     正常模式下，请求源每触发一次，发起一次DMA请求;
//     周期触发模式下，每一个周期触发一次，若触发时请求源也正被触发，发起一次DMA请求，
//     周期由PIT通道的中断周期决定，仅DMA0-DMA3通道可以使用周期触发模式，
//     对应的PIT通道为PIT0-PIT3;
//
//     每次副循环传输字节数(minor_loop_bytes)必须为源地址和目标地址数据宽度的倍数;
//     地址必须与数据宽度匹配，若数据宽度为4字节，地址必须4字节对齐，即地址为4的倍数;
//     地址每次被读取/写入后的偏移量必须为数据宽度的倍数;
//
//     一次主循环由若干次副循环组成，一次副循环由若干次读写组成，每次读写中，
//     读次数和写次数的对应关系可以为一对一，也可以为多对一和一对多，
//     这由源地址和目标地址的数据宽度决定，若源地址数据宽度为2字节，
//     目标地址数据宽度为4字节，读2次源地址，再写1次目标地址，组成一次读写，
//     在这次读写中，源地址偏移2次，目标地址偏移1次;
//
//     使能模数功能后，会依照模数大小，使得地址在偏移时，仅有低位可以变化，
//     而高位不会变化，若模数大小为2的n次方，则可变化的位数为n，
//     如模数大小为16个字节，初始地址为0x12345670，仅低4位可以变化，
//     而0x1234567x不会变化;
//     但若主循环完成后的偏移使得地址的高位发生变化，在下一次主循环中，
//     将会保证变化后的高位不变;
//     可以利用模数功能构建一个循环队列;
//==========================================================================
void dma_init(uint8 ch, uint8 req, uint8 mode, uint32 minor_loop_bytes,
		uint16 major_loop_iteration_cnt, uint32 src_addr, uint8 src_data_width,
		int16 src_addr_offset, uint8 src_modulo, int32 src_addr_last_adj,
		uint32 dest_addr, uint8 dest_data_width, int16 dest_addr_offset,
		uint8 dest_modulo, int32 dest_addr_last_adj, bool auto_disable_req);

//==========================================================================
//函数名称: dma_enable_req
//函数返回: 无
//参数说明: ch:DMA通道号:
//            DMA_CHx，x为通道号;
//功能概要: 接收该通道的DMA请求
//==========================================================================
void dma_enable_req(uint8 ch);

//==========================================================================
//函数名称: dma_disable_req
//函数返回: 无
//参数说明: ch:DMA通道号:
//            DMA_CHx，x为通道号;
//功能概要: 不接收该通道的DMA请求
//==========================================================================
void dma_disable_req(uint8 ch);

//==========================================================================
//函数名称: dma_enable_major_int
//函数返回: 无
//参数说明: ch:DMA通道号:
//            DMA_CHx，x为通道号;
//功能概要: 使能主循环完成中断，当一次主循环完成时，触发中断
//备注: 即使发生中断，若DMA请求被触发，且该通道接收DMA请求，数据传输仍会进行
//==========================================================================
void dma_enable_major_int(uint8 ch);

//==========================================================================
//函数名称: dma_disable_major_int
//函数返回: 无
//参数说明: ch:DMA通道号:
//            DMA_CHx，x为通道号;
//功能概要: 关闭主循环完成中断
//==========================================================================
void dma_disable_major_int(uint8 ch);

//==========================================================================
//函数名称: dma_get_major_int
//函数返回: true:产生中断; false:未产生中断
//参数说明: ch:DMA通道号:
//            DMA_CHx，x为通道号;
//功能概要: 获取主循环完成中断的中断标志
//备注: 仅当该通道使能中断时，中断标志才会置位
//==========================================================================
bool dma_get_major_int(uint8 ch);

//==========================================================================
//函数名称: dma_clear_major_int
//函数返回: 无
//参数说明: ch:DMA通道号:
//            DMA_CHx，x为通道号;
//功能概要: 清除主循环完成中断的中断标志
//==========================================================================
void dma_clear_major_int(uint8 ch);

//==========================================================================
//函数名称: dma_software_req
//函数返回: 无
//参数说明: ch:DMA通道号:
//            DMA_CHx，x为通道号;
//功能概要: 软件触发一次DMA请求
//备注: 无论是否接收该通道的DMA请求，软件触发的DMA请求一定被接收;
//     若通道状态为正在启动，此时软件触发的DMA请求会丢失
//==========================================================================
void dma_software_req(uint8 ch);

//==========================================================================
//函数名称: dma_get_ch_state
//函数返回: 通道状态:
//         DMAChannelIdle:     通道空闲;
//         DMAChannelStarting: 通道正在启动;
//         DMAChannelExecuting:通道正在执行;
//         DMAChannelDone:     通道完成主循环;
//参数说明: ch:DMA通道号:
//            DMA_CHx，x为通道号;
//功能概要: 获取通道状态
//备注: 通道状态若为空闲，此时可能请求源发起了一次DMA请求，但尚未执行，
//     也可能通道已经完成了一次副循环，但尚未完成主循环;
//
//     通道状态若为正在启动，此时可能软件触发了一次DMA请求，
//     也可能因通道连接被触发了一次DMA请求，但尚未执行;
//
//     通道状态若为正在执行，此时正在执行该通道的副循环;
//
//     通道状态若为完成主循环，此时该通道已经完成了一次主循环
//==========================================================================
DMAChannelState dma_get_ch_state(uint8 ch);

//==========================================================================
//函数名称: dma_get_major_loop_iteration_cnt
//函数返回: 主循环剩余迭代次数
//参数说明: ch:DMA通道号:
//            DMA_CHx，x为通道号;
//功能概要: 获取主循环剩余迭代次数
//==========================================================================
uint16 dma_get_major_loop_iteration_cnt(uint8 ch);

//==========================================================================
//函数名称: dma_get_src_addr
//函数返回: 当前源地址
//参数说明: ch:DMA通道号:
//            DMA_CHx，x为通道号;
//功能概要: 获取当前源地址
//==========================================================================
uint32 dma_get_src_addr(uint8 ch);

//==========================================================================
//函数名称: dma_set_src_addr
//函数返回: 无
//参数说明: ch:DMA通道号:
//            DMA_CHx，x为通道号;
//         src_addr:当前源地址，必须与数据宽度匹配
//功能概要: 设置当前源地址
//备注: 请设置为不接收该通道的DMA请求后，再进行设置
//==========================================================================
void dma_set_src_addr(uint8 ch, uint32 src_addr);

//==========================================================================
//函数名称: dma_get_dest_addr
//函数返回: 当前目标地址
//参数说明: ch:DMA通道号:
//            DMA_CHx，x为通道号;
//功能概要: 获取当前目标地址
//==========================================================================
uint32 dma_get_dest_addr(uint8 ch);

//==========================================================================
//函数名称: dma_set_dest_addr
//函数返回: 无
//参数说明: ch:DMA通道号:
//            DMA_CHx，x为通道号;
//         dest_addr:当前目标地址，必须与数据宽度匹配
//功能概要: 设置当前目标地址
//备注: 请设置为不接收该通道的DMA请求后，再进行设置
//==========================================================================
void dma_set_dest_addr(uint8 ch, uint32 dest_addr);

//==========================================================================
//函数名称: dma_set_auto_disable_req
//函数返回: 无
//参数说明: ch:DMA通道号:
//            DMA_CHx，x为通道号;
//         enable:是否使能主循环完成后自动不接收DMA请求:
//                true: 使能主循环完成后自动不接收DMA请求;
//                false:关闭主循环完成后自动不接收DMA请求;
//功能概要: 设置是否使能主循环完成后自动不接收DMA请求
//备注: 请设置为不接收该通道的DMA请求后，再进行设置
//==========================================================================
void dma_set_auto_disable_req(uint8 ch, bool enable);

//==========================================================================
//函数名称: dma_set_minor_link
//函数返回: 无
//参数说明: ch:DMA通道号:
//            DMA_CHx，x为通道号;
//         enable:是否使能副循环通道连接:
//                true: 使能副循环通道连接;
//                false:关闭副循环通道连接;
//         link_ch:连接通道的通道号，该参数仅在enable为true时有效:
//                 DMA_CHx，x为通道号;
//功能概要: 设置是否使能副循环通道连接
//备注: 请设置为不接收该通道的DMA请求后，再进行设置;
//     当该通道的一次副循环完成后，自动软件触发一次连接通道的DMA请求;
//     若此次副循环完成后即完成主循环，将不会触发连接通道的DMA请求
//==========================================================================
void dma_set_minor_link(uint8 ch, bool enable, uint8 link_ch);

//==========================================================================
//函数名称: dma_set_major_link
//函数返回: 无
//参数说明: ch:DMA通道号:
//            DMA_CHx，x为通道号;
//         enable:是否使能主循环通道连接:
//                true: 使能主循环通道连接;
//                false:关闭主循环通道连接;
//         link_ch:连接通道的通道号，该参数仅在enable为true时有效:
//                 DMA_CHx，x为通道号;
//功能概要: 设置是否使能主循环通道连接
//备注: 请设置为不接收该通道的DMA请求后，再进行设置;
//     当该通道的一次主循环完成后，自动软件触发一次连接通道的DMA请求
//==========================================================================
void dma_set_major_link(uint8 ch, bool enable, uint8 link_ch);

//==========================================================================
//函数名称: dma_set_arbitration_mode
//函数返回: 无
//参数说明: arbitration_mode:仲裁模式:
//                          DMA_ARBITRATION_MODE_FIXED:      固定优先级仲裁;
//                          DMA_ARBITRATION_MODE_ROUND_ROBIN:轮询仲裁;
//功能概要: 设置仲裁模式
//备注: 默认为固定优先级仲裁;
//
//     固定优先级仲裁时，若同时有多个通道的DMA请求被触发，按照通道的优先级，
//     先响应优先级最高的通道，0-15优先级从低到高，0为最低优先级，15为最高优先级;
//
//     轮询仲裁时，从15号通道至0通道依次查询通道是否有DMA请求，若有，则响应该请求，
//     若无，则继续查询下一个通道，重复该过程，这种仲裁模式与优先级无关
//==========================================================================
void dma_set_arbitration_mode(uint8 arbitration_mode);

//==========================================================================
//函数名称: dma_get_priority
//函数返回: 该通道的优先级，取值范围为[0,15]
//参数说明: ch:DMA通道号:
//            DMA_CHx，x为通道号;
//功能概要: 获取该通道的优先级
//==========================================================================
uint8 dma_get_priority(uint8 ch);

//==========================================================================
//函数名称: dma_set_priority
//函数返回: 无
//参数说明: ch:DMA通道号:
//            DMA_CHx，x为通道号;
//         priority:该通道的优先级，取值范围为[0,15]
//功能概要: 设置该通道的优先级
//备注: 0-15优先级从低到高，0为最低优先级，15为最高优先级;
//     各通道默认优先级为通道号，0号通道优先级为0，15号通道优先级为15;
//     各通道优先级不能重复，每个优先级必须独自属于一个通道;
//
//     要设置优先级，一种方法为先设置不接收相关通道的DMA请求，
//     优先级设置完毕后再恢复接收，另一种方法为先设置为轮询仲裁，
//     优先级设置完毕后再恢复为固定优先级仲裁
//==========================================================================
void dma_set_priority(uint8 ch, uint8 priority);

//==========================================================================
//函数名称: dma_set_channel_preemption
//函数返回: 无
//参数说明: ch:DMA通道号:
//            DMA_CHx，x为通道号;
//         enable:该通道是否可以被抢占:
//                true: 该通道可以被抢占;
//                false:该通道不可以被抢占;
//功能概要: 设置该通道是否可以被抢占
//备注: 各通道默认不可以被抢占;
//     通道抢占仅在固定优先级仲裁模式下有效;
//     若一个高优先级通道发起一次DMA请求，而此时一个低优先级通道正在执行数据传输，
//     在低优先级通道完成当前的读写后，暂时挂起低优先级通道，
//     而先执行高优先级通道的数据传输，这就是抢占
//==========================================================================
void dma_set_channel_preemption(uint8 ch, bool enable);

//==========================================================================
//函数名称: dma_set_preempt_ability
//函数返回: 无
//参数说明: ch:DMA通道号:
//            DMA_CHx，x为通道号;
//         enable:该通道是否可以抢占别的通道:
//                true: 该通道可以抢占别的通道;
//                false:该通道不可以抢占别的通道;
//功能概要: 设置该通道是否可以抢占别的通道
//备注: 各通道默认可以抢占别的通道;
//     通道抢占仅在固定优先级仲裁模式下有效
//==========================================================================
void dma_set_preempt_ability(uint8 ch, bool enable);

#endif
