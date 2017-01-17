//==========================================================================
//文件名称：frame.c
//功能概要：组帧通信协议软件程序源文件
//==========================================================================

#include "frame.h"

//内部定义帧缓冲区队列结构体
typedef struct {
	FrameInfo buffer[FRAME_BUFFER_SIZE];	//帧缓冲区
	uint8 front;	//队首下标
	uint8 rear;		//队尾下标
	uint8 num;		//队列中的帧数量
} FrameBufferQueue;

//静态帧缓冲区队列，初始化队首、队尾下标，队列中帧数量为0
static FrameBufferQueue buffer_queue = { { }, 0, 0, 0 };

//==========================================================================
//函数名称: frame_init
//函数返回: 无
//参数说明: 无
//功能概要: 使能组帧通信协议，相应配置在frame.h中
//==========================================================================
void frame_init() {
	//根据配置初始化相应的UART模块
	uart_init(FRAME_UART_MOD, FRAME_UART_BAUD, FRAME_UART_PARITY,
	FRAME_UART_STOP_BIT, FRAME_UART_BIT_ORDER);
}

//==========================================================================
//函数名称: frame_enable_re_int
//函数返回: 无
//参数说明: 无
//功能概要: 使能组帧通信协议接收中断，即相应的UART接收中断，通过组帧可以清除中断标志
//==========================================================================
void frame_enable_re_int() {
	uart_enable_re_int(FRAME_UART_MOD);
}

//==========================================================================
//函数名称: frame_disable_re_int
//函数返回: 无
//参数说明: 无
//功能概要: 关闭组帧通信协议接收中断，即相应的UART接收中断
//==========================================================================
void frame_disable_re_int() {
	uart_disable_re_int(FRAME_UART_MOD);
}

//==========================================================================
//函数名称: frame_send_info
//函数返回: 无
//参数说明: info:要发送的帧信息结构体
//功能概要: 发送帧信息结构体，自动添加帧头与帧校验序列
//==========================================================================
void frame_send_info(FrameInfo info) {
	uint16 info_len;	//帧信息结构体的字节数
	uint8* info_ptr;	//帧信息结构体首地址
	uint16 crc;			//CRC16校验码
	//帧信息结构体字节数为数据字节数+2
	info_len = info.len + 2;
	//获取帧信息结构体首地址
	info_ptr = (uint8*) &info;
	//计算CRC16校验码
	crc = crc16_modbus_block(info_ptr, info_len);
	//依顺序发送帧各字段
	uart_send1(FRAME_UART_MOD, FRAME_HEAD_LOW);
	uart_send1(FRAME_UART_MOD, FRAME_HEAD_HIGH);
	uart_sendN(FRAME_UART_MOD, info_ptr, info_len);
	uart_send1(FRAME_UART_MOD, (uint8) crc);
	uart_send1(FRAME_UART_MOD, crc >> 8);
}

//==========================================================================
//函数名称: frame_framing
//函数返回: 组帧结果:
//         FramingNone:    未接收到字节;
//         FramingFull:    帧缓冲区满，字节被丢弃;
//         FramingInvalid: 收到无效字节;
//         FramingHeadLow: 收到帧头低字节;
//         FramingHeadHigh:收到帧头高字节;
//         FramingType:    收到帧类型;
//         FramingLen:     收到数据长度;
//         FramingData:    收到数据;
//         FramingFCS:     收到FCS低字节;
//         FramingSuccess: 收到FCS高字节，且校验成功;
//         FramingFail:    收到FCS高字节，且校验失败，帧被丢弃;
//参数说明: 无
//功能概要: 从UART接收缓冲区中读取一个字节数据，并进行组帧，组的帧存至帧缓冲区中
//==========================================================================
FrameFramingResult frame_framing() {
	//标志，位0表示有无收到帧头低字节，位1表示有无收到帧头高字节，
	//位2表示有无收到类型，位3表示有无收到数据长度，位4表示是否接收完数据，
	//位5表示有无收到FCS低字节，位6表示FCS低字节是否校验成功
	static uint8 flags = 0;
	//已经接收的数据长度
	static uint8 received_len = 0;
	//上一次的CRC校验码
	static uint16 last_crc = CRC16_MODBUS_STREAM_INIT_CRC;
	uint8 byte;	//接收到的字节
	FrameFramingResult result;	//组帧结果

	//接收字节
	if (uart_re1(FRAME_UART_MOD, &byte)) {
		//接收字节成功时，检查帧缓冲区是否已满
		if (buffer_queue.num >= FRAME_BUFFER_SIZE) {
			//帧缓冲区满
			result = FramingFull;
		} else {
			//帧缓冲区不满时，根据标志做相应操作
			switch (flags) {
			case 0:		//未收到帧头低字节
				//若收到帧头低字节
				if (byte == FRAME_HEAD_LOW) {
					//更改标志
					flags = 1;
					result = FramingHeadLow;
				} else {
					//收到无效字节
					result = FramingInvalid;
				}
				break;
			case 1:		//未收到帧头高字节
				//若收到帧头高字节
				if (byte == FRAME_HEAD_HIGH) {
					//更改标志
					flags = 3;
					result = FramingHeadHigh;
				} else {
					//收到无效字节，重置标志
					flags = 0;
					result = FramingInvalid;
				}
				break;
			case 3:		//未收到帧类型
				//收到帧类型
				buffer_queue.buffer[buffer_queue.rear].type = byte;
				//进行校验
				last_crc = crc16_modbus_stream(last_crc, byte);
				//更改标志
				flags = 7;
				result = FramingType;
				break;
			case 7:		//未收到数据长度
				//收到数据长度
				buffer_queue.buffer[buffer_queue.rear].len = byte;
				//进行校验
				last_crc = crc16_modbus_stream(last_crc, byte);
				//根据数据长度更改标志
				flags = byte == 0 ? 31 : 15;
				result = FramingLen;
				break;
			case 15:	//未接收完数据
				//收到数据
				buffer_queue.buffer[buffer_queue.rear].data[received_len++] =
						byte;
				//进行校验
				last_crc = crc16_modbus_stream(last_crc, byte);
				//检查是否接收完毕
				if (received_len
						>= buffer_queue.buffer[buffer_queue.rear].len) {
					//更改标志
					flags = 31;
				}
				result = FramingData;
				break;
			case 31:	//未收到FCS低字节
				//检查FCS低字节是否校验正确
				if ((uint8) last_crc == byte) {
					//校验成功
					flags = 127;
				} else {
					//校验失败
					flags = 63;
				}
				//重置已经接收的数据长度
				received_len = 0;
				result = FramingFCS;
				break;
			case 63:	//FCS低字节校验失败
				//跳过检验FCS高字节，重置各静态变量
				flags = 0;
				last_crc = CRC16_MODBUS_STREAM_INIT_CRC;
				result = FramingFail;
				break;
			case 127:	//FCS低字节校验成功
				//校验FCS高字节
				if ((last_crc >> 8) == byte) {
					//校验成功，更改缓冲区队列队尾
					buffer_queue.rear = (buffer_queue.rear + 1)
							% FRAME_BUFFER_SIZE;
					buffer_queue.num++;
					result = FramingSuccess;
				} else {
					//校验失败
					result = FramingFail;
				}
				//重置各静态变量
				flags = 0;
				last_crc = CRC16_MODBUS_STREAM_INIT_CRC;
				break;
			}
		}
	} else {
		//未接收到字节
		result = FramingNone;
	}
	//返回结果
	return result;
}

//==========================================================================
//函数名称: frame_get_info
//函数返回: true:获取成功; false:获取失败
//参数说明: info:存储帧信息结构体的地址
//功能概要: 帧缓冲区不为空时，取出最先存储的一帧
//==========================================================================
bool frame_get_info(FrameInfo* info) {
	uint8 len;	//数据长度
	uint8 i;	//游标
	//若缓冲区为空，获取失败
	if (buffer_queue.num == 0) {
		return false;
	}
	//获取数据长度
	len = buffer_queue.buffer[buffer_queue.front].len;
	//赋值
	info->type = buffer_queue.buffer[buffer_queue.front].type;
	info->len = len;
	for (i = 0; i < len; i++) {
		info->data[i] = buffer_queue.buffer[buffer_queue.front].data[i];
	}
	//更改缓冲区队列队首
	buffer_queue.num--;
	buffer_queue.front = (buffer_queue.front + 1) % FRAME_BUFFER_SIZE;
	//获取成功
	return true;
}
