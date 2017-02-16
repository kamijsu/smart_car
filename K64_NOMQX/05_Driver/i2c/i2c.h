//==========================================================================
//文件名称：i2c.h
//功能概要：K64 I2C底层驱动程序头文件
//==========================================================================

#ifndef _I2C_H
#define _I2C_H

//包含公共头文件
#include "common.h"

//定义I2C模块号
#define I2C_MOD0	(0)
#define I2C_MOD1	(1)
#define I2C_MOD2	(2)

//定义I2C各模块的引脚设置，通过更改COM_PORTx|x的x以选择引脚，
//SCL、SDA分别为时钟信号、数据信号引脚
#define I2C_MOD0_SCL_PIN	(COM_PORTB|2)	//B0  B2  D2  D8  E24
#define I2C_MOD0_SDA_PIN	(COM_PORTB|3)	//B1  B3  D3  D9  E25

#define I2C_MOD1_SCL_PIN	(COM_PORTC|10)	//C10 E1
#define I2C_MOD1_SDA_PIN	(COM_PORTC|11)	//C11 E0

#define I2C_MOD2_SCL_PIN	(COM_PORTA|14)	//A12 A14
#define I2C_MOD2_SDA_PIN	(COM_PORTA|13)	//A11 A13

//定义I2C乘数因子
#define I2C_MUL_1	(0)		//乘数因子为1
#define I2C_MUL_2	(1)		//乘数因子为2
#define I2C_MUL_4	(2)		//乘数因子为4

//定义I2C地址模式
#define I2C_ADDR_MODE_BITS_7	(7)		//7位地址模式
#define I2C_ADDR_MODE_BITS_10	(10)	//10位地址模式

//定义I2C响应超时数值
#define I2C_TIMEOUT_MAX		(0x2000000u)	//约4s

//定义I2C主机通信结果
typedef enum {
	I2CMasterSuccess,			//通信成功
	I2CMasterNAck,				//从机未响应
	I2CMasterArbitrationLost,	//发生仲裁丢失
	I2CMasterTimeoutSlave,		//从机响应超时
	I2CMasterTimeoutStop,		//停止信号响应超时
	I2CMasterIsBusy				//主机正在工作
} I2CMasterResult;

//定义I2C从机中断类型
typedef enum {
	I2CSlaveNoInt,				//无中断发生
	I2CSlaveCalledGeneralInt,	//被广播寻址中断
	I2CSlaveCalledReInt,		//被寻址接收中断
	I2CSlaveCalledSendInt,		//被寻址发送中断
	I2CSlaveDataReInt,			//数据接收中断
	I2CSlaveDataSendInt,		//数据发送中断
	I2CSlaveDataSendOverInt,	//数据发送完毕中断
	I2CSlaveArbitrationLostInt	//仲裁丢失中断
} I2CSlaveIntType;

//==========================================================================
//函数名称: i2c_init
//函数返回: 无
//参数说明: mod:I2C模块号:
//             I2C_MODx，x为模块号;
//         mul:乘数因子:
//             I2C_MUL_x，x为乘数因子大小，x可取值为:
//             1 | 2 | 4
//         icr:时钟速率，取值范围为[0x00,0x3F]，具体解释见备注
//         addr_mode:该I2C模块地址模式:
//                   I2C_ADDR_MODE_BITS_7: 7位地址模式;
//                   I2C_ADDR_MODE_BITS_10:10位地址模式;
//         addr:该I2C模块地址:
//              7位地址模式时，取值范围为[0x08,0x77]，其余地址被保留
//              10位地址模式时，取值范围为(0x000,0x3FF]
//         enable_general_call_addr:是否使能该模块的广播呼叫地址:
//                                  true: 使能广播呼叫地址，该模块可以被广播寻址;
//                                  false:关闭广播呼叫地址;
//功能概要: 初始化I2C模块，为从机接收模式，默认关闭从机中断，且允许发送应答信号
//备注: 已使能引脚的开漏输出和上拉电阻;
//
//     该I2C模块波特率 = 总线时钟频率(Hz) / (mul * SCL divider)，
//     波特率不要超过400Kbps，否则通信易受到干扰;
//     SDA保持时间 = 总线时钟周期(s) * mul * SDA hold value，
//     SDA保持时间为SCL信号下降沿跳变后，至SDA信号变化的时间;
//     SCL开始保持时间 = 总线时钟周期(s) * mul * SCL start hold value，
//     SCL开始保持时间为开始信号发生后，至SCL信号第一个下降沿跳变的时间;
//     SCL停止保持时间 = 总线时钟周期(s) * mul * SCL stop hold value，
//     SCL停止保持时间为SCL信号最后一个上升沿跳变后，至停止信号发生的时间;
//
//     icr参数决定SCL divider、SDA hold value、SCL start hold value、
//     SCL stop hold value，具体关系如下表所示:
//
//     |icr |SCL divider|SDA hold value|SCL start |SCL stop  |
//     |    |           |              |hold value|hold value|
//     |0x00|         20|             7|         6|        11|
//     |0x01|         22|             7|         7|        12|
//     |0x02|         24|             8|         8|        13|
//     |0x03|         26|             8|         9|        14|
//     |0x04|         28|             9|        10|        15|
//     |0x05|         30|             9|        11|        16|
//     |0x06|         34|            10|        13|        18|
//     |0x07|         40|            10|        16|        21|
//     |0x08|         28|             7|        10|        15|
//     |0x09|         32|             7|        12|        17|
//     |0x0A|         36|             9|        14|        19|
//     |0x0B|         40|             9|        16|        21|
//     |0x0C|         44|            11|        18|        23|
//     |0x0D|         48|            11|        20|        25|
//     |0x0E|         56|            13|        24|        29|
//     |0x0F|         68|            13|        30|        35|
//     |0x10|         48|             9|        18|        25|
//     |0x11|         56|             9|        22|        29|
//     |0x12|         64|            13|        26|        33|
//     |0x13|         72|            13|        30|        37|
//     |0x14|         80|            17|        34|        41|
//     |0x15|         88|            17|        38|        45|
//     |0x16|        104|            21|        46|        53|
//     |0x17|        128|            21|        58|        65|
//     |0x18|         80|             9|        38|        41|
//     |0x19|         96|             9|        46|        49|
//     |0x1A|        112|            17|        54|        57|
//     |0x1B|        128|            17|        62|        65|
//     |0x1C|        144|            25|        70|        73|
//     |0x1D|        160|            25|        78|        81|
//     |0x1E|        192|            33|        94|        97|
//     |0x1F|        240|            33|       118|       121|
//     |0x20|        160|            17|        78|        81|
//     |0x21|        192|            17|        94|        97|
//     |0x22|        224|            33|       110|       113|
//     |0x23|        256|            33|       126|       129|
//     |0x24|        288|            49|       142|       145|
//     |0x25|        320|            49|       158|       161|
//     |0x26|        384|            65|       190|       193|
//     |0x27|        480|            65|       238|       241|
//     |0x28|        320|            33|       158|       161|
//     |0x29|        384|            33|       190|       193|
//     |0x2A|        448|            65|       222|       225|
//     |0x2B|        512|            65|       254|       257|
//     |0x2C|        576|            97|       286|       289|
//     |0x2D|        640|            97|       318|       321|
//     |0x2E|        768|           129|       382|       385|
//     |0x2F|        960|           129|       478|       481|
//     |0x30|        640|            65|       318|       321|
//     |0x31|        768|            65|       382|       385|
//     |0x32|        896|           129|       446|       449|
//     |0x33|       1024|           129|       510|       513|
//     |0x34|       1152|           193|       574|       577|
//     |0x35|       1280|           193|       638|       641|
//     |0x36|       1536|           257|       766|       769|
//     |0x37|       1920|           257|       958|       961|
//     |0x38|       1280|           129|       638|       641|
//     |0x39|       1536|           129|       766|       769|
//     |0x3A|       1792|           257|       894|       897|
//     |0x3B|       2048|           257|      1022|      1025|
//     |0x3C|       2304|           385|      1150|      1153|
//     |0x3D|       2560|           385|      1278|      1281|
//     |0x3E|       3072|           513|      1534|      1537|
//     |0x3F|       3840|           513|      1918|      1921|
//
//     若总线时钟频率为48MHz，mul为I2C_MUL_2，icr为0x20，则波特率为150Kbps，
//     SDA保持时间为0.71μs，SCL开始保持时间为3.25μs，SCL停止保持时间为3.375μs
//==========================================================================
void i2c_init(uint8 mod, uint8 mul, uint8 icr, uint8 addr_mode, uint16 addr,
		bool enable_general_call_addr);

//==========================================================================
//函数名称: i2c_master_send
//函数返回: 主机通信结果:
//         I2CMasterSuccess:        通信成功;
//         I2CMasterNAck:           从机未响应;
//         I2CMasterArbitrationLost:发生仲裁丢失;
//         I2CMasterTimeoutSlave:   从机响应超时;
//         I2CMasterTimeoutStop:    停止信号响应超时;
//         I2CMasterIsBusy:         主机正在工作;
//参数说明: mod:I2C模块号:
//             I2C_MODx，x为模块号;
//         addr_mode:通信从机对象的地址模式:
//                   I2C_ADDR_MODE_BITS_7: 7位地址模式;
//                   I2C_ADDR_MODE_BITS_10:10位地址模式;
//         addr:通信从机对象的地址;
//         data:要发送数据的首地址;
//         len:要发送数据的字节数;
//功能概要: 该I2C模块转换为主机模式，向指定从机发送数据后，恢复为从机模式
//备注: addr_mode和addr仅与从机有关，和本机无关，不同地址模式的I2C模块可以相互通信，
//     通信波特率由本机决定;
//     当地址模式为7位地址模式，且地址为0x00时，为广播呼叫地址;
//
//     注意，若发生响应超时(从机或停止信号)，从机有可能已经卡死，需要复位从机;
//     若同时有多个主机发送数据，本机竞争失败时，会发生仲裁丢失，
//     或从机卡死，不释放总线时，也会发生仲裁丢失
//==========================================================================
I2CMasterResult i2c_master_send(uint8 mod, uint8 addr_mode, uint16 addr,
		uint8* data, uint32 len);

//==========================================================================
//函数名称: i2c_master_send_re
//函数返回: 主机通信结果:
//         I2CMasterSuccess:        通信成功;
//         I2CMasterNAck:           从机未响应;
//         I2CMasterArbitrationLost:发生仲裁丢失;
//         I2CMasterTimeoutSlave:   从机响应超时;
//         I2CMasterTimeoutStop:    停止信号响应超时;
//         I2CMasterIsBusy:         主机正在工作;
//参数说明: mod:I2C模块号:
//             I2C_MODx，x为模块号;
//         addr_mode:通信从机对象的地址模式:
//                   I2C_ADDR_MODE_BITS_7: 7位地址模式;
//                   I2C_ADDR_MODE_BITS_10:10位地址模式;
//         addr:通信从机对象的地址;
//         send_data:要发送数据的首地址;
//         send_len:要发送数据的字节数;
//         re_data:存储接收数据的首地址;
//         re_len:要接收数据的字节数;
//功能概要: 该I2C模块转换为主机模式，向指定从机发送数据，并接收数据后，恢复为从机模式
//备注: addr_mode和addr仅与从机有关，和本机无关，不同地址模式的I2C模块可以相互通信，
//     通信波特率由本机决定;
//     无法使用广播呼叫地址，因为广播呼叫地址仅能用于发送数据;
//
//     注意，若发生响应超时(从机或停止信号)，从机有可能已经卡死，需要复位从机;
//     若同时有多个主机发送数据，本机竞争失败时，会发生仲裁丢失，
//     或从机卡死，不释放总线时，也会发生仲裁丢失
//==========================================================================
I2CMasterResult i2c_master_send_re(uint8 mod, uint8 addr_mode, uint16 addr,
		uint8* send_data, uint32 send_len, uint8* re_data, uint32 re_len);

//==========================================================================
//函数名称: i2c_master_re
//函数返回: 主机通信结果:
//         I2CMasterSuccess:        通信成功;
//         I2CMasterNAck:           从机未响应;
//         I2CMasterArbitrationLost:发生仲裁丢失;
//         I2CMasterTimeoutSlave:   从机响应超时;
//         I2CMasterTimeoutStop:    停止信号响应超时;
//         I2CMasterIsBusy:         主机正在工作;
//参数说明: mod:I2C模块号:
//             I2C_MODx，x为模块号;
//         addr_mode:通信从机对象的地址模式:
//                   I2C_ADDR_MODE_BITS_7: 7位地址模式;
//                   I2C_ADDR_MODE_BITS_10:10位地址模式;
//         addr:通信从机对象的地址;
//         data:存储接收数据的首地址;
//         len:要接收数据的字节数;
//功能概要: 该I2C模块转换为主机模式，向指定从机接收数据后，恢复为从机模式
//备注: addr_mode和addr仅与从机有关，和本机无关，不同地址模式的I2C模块可以相互通信，
//     通信波特率由本机决定;
//     无法使用广播呼叫地址，因为广播呼叫地址仅能用于发送数据;
//
//     注意，若发生响应超时(从机或停止信号)，从机有可能已经卡死，需要复位从机;
//     若同时有多个主机发送数据，本机竞争失败时，会发生仲裁丢失，
//     或从机卡死，不释放总线时，也会发生仲裁丢失
//==========================================================================
I2CMasterResult i2c_master_re(uint8 mod, uint8 addr_mode, uint16 addr,
		uint8* data, uint32 len);

//==========================================================================
//函数名称: i2c_slave_enable_int
//函数返回: 无
//参数说明: mod:I2C模块号:
//             I2C_MODx，x为模块号;
//功能概要: 使能从机中断，被主机通信时，会发生中断
//==========================================================================
void i2c_slave_enable_int(uint8 mod);

//==========================================================================
//函数名称: i2c_slave_disable_int
//函数返回: 无
//参数说明: mod:I2C模块号:
//             I2C_MODx，x为模块号;
//功能概要: 关闭从机中断
//==========================================================================
void i2c_slave_disable_int(uint8 mod);

//==========================================================================
//函数名称: i2c_slave_set_ack
//函数返回: 无
//参数说明: mod:I2C模块号:
//             I2C_MODx，x为模块号;
//         ack:是否发送应答信号:
//             true: 发送应答信号;
//             false:发送非应答信号;
//功能概要: 设置从机应答信号类型
//备注: 从机地址匹配时(包括广播地址)，一定发送应答信号，
//     匹配到广播地址后，接收的数据一定发送应答信号;
//     更改应答信号类型后，下一次发生应答信号请求时生效
//==========================================================================
void i2c_slave_set_ack(uint8 mod, bool ack);

//==========================================================================
//函数名称: i2c_slave_handle_int
//函数返回: 从机中断类型:
//         I2CSlaveNoInt:             无中断发生;
//         I2CSlaveCalledGeneralInt:  被广播寻址中断;
//         I2CSlaveCalledReInt:       被寻址接收中断;
//         I2CSlaveCalledSendInt:     被寻址发送中断;
//         I2CSlaveDataReInt:         数据接收中断;
//         I2CSlaveDataSendInt:       数据发送中断;
//         I2CSlaveDataSendOverInt:   数据发送完毕中断;
//         I2CSlaveArbitrationLostInt:仲裁丢失中断;
//参数说明: mod:I2C模块号:
//             I2C_MODx，x为模块号;
//功能概要: 处理从机中断，并返回中断类型
//备注: 在中断服务例程中，需要调用该函数，另外，
//     中断类型为I2CSlaveDataReInt时，需要接收一字节数据，
//     中断类型为I2CSlaveCalledSendInt或I2CSlaveDataSendInt时，需要发送一字节数据;
//     若不接收或发送数据，总线会被从机占据，主机会在下一次传输数据时等待从机释放总线;
//
//     主机发送n个字节数据时，从机中断过程如下:
//     [I2CSlaveDataReInt] - I2CSlaveCalledReInt - I2CSlaveDataReInt * n;
//
//     主机广播发送n个字节数据时，从机中断过程如下:
//     I2CSlaveCalledGeneralInt - I2CSlaveDataReInt * n;
//
//     主机发送n个字节数据，接收m个字节数据时，从机中断过程如下:
//     [I2CSlaveDataReInt] - I2CSlaveCalledReInt - I2CSlaveDataReInt * n -
//     I2CSlaveCalledSendInt - I2CSlaveDataSendInt * (m-1) -
//     I2CSlaveDataSendOverInt;
//
//     主机接收n个字节数据时，从机中断过程如下:
//     [I2CSlaveDataReInt] - [I2CSlaveCalledReInt] - I2CSlaveCalledSendInt -
//     I2CSlaveDataSendInt * (n-1) - I2CSlaveDataSendOverInt;
//
//     加[]的中断为从机在10位地址模式时，会额外发生的中断，从机在被寻址接收前，
//     将会额外接收一个字节数据，为0b1111 0xx0，xx为从机高2位地址，
//     若从机地址为0x100，将接收到0xF2，需要自行将该数据视为无效数据;
//
//     若从机为10位地址模式，且地址为0x000，I2CSlaveCalledReInt会被误判为
//     I2CSlaveCalledGeneralInt，因此建议不要将地址设为0x000;
//
//     中断发生时，应答信号已经按照设置发送出去
//==========================================================================
I2CSlaveIntType i2c_slave_handle_int(uint8 mod);

//==========================================================================
//函数名称: i2c_slave_re
//函数返回: 从机接收到的数据
//参数说明: mod:I2C模块号:
//             I2C_MODx，x为模块号;
//功能概要: 从机接收一个字节数据
//==========================================================================
uint8 i2c_slave_re(uint8 mod);

//==========================================================================
//函数名称: i2c_slave_send
//函数返回: 无
//参数说明: mod:I2C模块号:
//             I2C_MODx，x为模块号;
//         data:要发送的数据
//功能概要: 从机发送一个字节数据
//==========================================================================
void i2c_slave_send(uint8 mod, uint8 data);

//根据I2C模块所设置的引脚号，定义相应的PCR的MUX值
#ifdef I2C_MOD0_SCL_PIN
#if(I2C_MOD0_SCL_PIN == (COM_PORTE|24))
#define I2C_MOD0_SCL_PCR_MUX	(5)
#elif(I2C_MOD0_SCL_PIN == (COM_PORTD|2))
#define I2C_MOD0_SCL_PCR_MUX	(7)
#else
#define I2C_MOD0_SCL_PCR_MUX	(2)
#endif
#endif

#ifdef I2C_MOD0_SDA_PIN
#if(I2C_MOD0_SDA_PIN == (COM_PORTE|25))
#define I2C_MOD0_SDA_PCR_MUX	(5)
#elif(I2C_MOD0_SDA_PIN == (COM_PORTD|3))
#define I2C_MOD0_SDA_PCR_MUX	(7)
#else
#define I2C_MOD0_SDA_PCR_MUX	(2)
#endif
#endif

#ifdef I2C_MOD1_SCL_PIN
#if(I2C_MOD1_SCL_PIN == (COM_PORTE|1))
#define I2C_MOD1_SCL_PCR_MUX	(6)
#else
#define I2C_MOD1_SCL_PCR_MUX	(2)
#endif
#endif

#ifdef I2C_MOD1_SDA_PIN
#if(I2C_MOD1_SDA_PIN == (COM_PORTE|0))
#define I2C_MOD1_SDA_PCR_MUX	(6)
#else
#define I2C_MOD1_SDA_PCR_MUX	(2)
#endif
#endif

#ifdef I2C_MOD2_SCL_PIN
#define I2C_MOD2_SCL_PCR_MUX	(5)
#endif

#ifdef I2C_MOD2_SDA_PIN
#define I2C_MOD2_SDA_PCR_MUX	(5)
#endif

#endif
