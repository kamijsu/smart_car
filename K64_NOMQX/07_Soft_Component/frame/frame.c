//==========================================================================
//文件名称：frame.c
//功能概要：组帧通信协议软件程序源文件
//==========================================================================

#include "frame.h"
#include "crc16_modbus.h"
#include <string.h>

//内部定义帧缓冲区队列结构体
typedef struct {
	FrameInfo buffer[FRAME_BUFFER_SIZE];	//帧缓冲区
	uint8 front;		//队首下标
	uint8 rear;			//队尾下标
	uint8 num;			//队列中的帧数量
	uint8 flag;			//队列接收标志
	uint8 recv_len;		//已经接收的字节数
	uint16 last_crc;	//上一次的CRC校验码
} FrameBufferQueue, *FrameBufferQueuePtr;

//静态帧缓冲区队列
static FrameBufferQueue buffer_queue[FRAME_BUFFER_NUM];
//本机地址
static uint8 local_addr;

//==========================================================================
//函数名称: frame_init
//函数返回: 无
//参数说明: addr:本机地址，不能与广播地址相同
//功能概要: 使能组帧通信协议，相应配置在frame.h中
//==========================================================================
void frame_init(uint8 addr) {
	uint8 i;

	//设置本机地址
	local_addr = addr;
	//初始化各帧缓冲区队列
	for (i = 0; i < FRAME_BUFFER_NUM; ++i) {
		buffer_queue[i].front = 0;
		buffer_queue[i].rear = 0;
		buffer_queue[i].num = 0;
		buffer_queue[i].flag = 0;
		buffer_queue[i].recv_len = 0;
		buffer_queue[i].last_crc = CRC16_MODBUS_STREAM_INIT_CRC;
	}
}

//==========================================================================
//函数名称: frame_get_local_addr
//函数返回: 本机地址
//参数说明: 无
//功能概要: 获取本机地址
//==========================================================================
uint8 frame_get_local_addr() {
	return local_addr;
}

//==========================================================================
//函数名称: frame_info_to_frame
//函数返回: 无
//参数说明: info:帧信息结构体地址
//         frame:存储帧字节的首地址
//         frame_len:存储帧字节长度的地址
//功能概要: 将帧信息结构体转换为帧字节，自动添加帧头与帧校验序列
//备注: 帧字节最大长度为263个字节，因此建议frame数组长度大于等于263
//==========================================================================
void frame_info_to_frame(const FrameInfo* info, uint8* frame, uint16* frame_len) {
	uint16 crc;			//CRC16校验码

	//计算帧字节长度
	*frame_len = info->len + 8;

	//添加帧头
	frame[0] = FRAME_HEAD_LOW;
	frame[1] = FRAME_HEAD_HIGH;
	//添加目的地址
	frame[2] = info->dest_addr;
	//添加源地址
	frame[3] = info->src_addr;
	//添加类型
	frame[4] = info->type;
	//添加数据长度
	frame[5] = info->len;
	//添加数据
	memcpy(frame + 6, info->data, info->len);
	//添加帧校验序列
	crc = crc16_modbus_block(frame + 2, info->len + 4);
	frame[*frame_len - 2] = (uint8) crc;
	frame[*frame_len - 1] = crc >> 8;
}

//==========================================================================
//函数名称: frame_frame_to_info
//函数返回: true:转换成功; false:转换失败，非法帧字节
//参数说明: frame:帧字节的首地址
//         frame_len:帧字节长度
//         info:存储帧信息结构体的地址
//功能概要: 将帧字节转换为帧信息结构体，检验帧字节长度、帧头、帧校验序列
//==========================================================================
bool frame_frame_to_info(const uint8* frame, uint16 frame_len, FrameInfo* info) {
	uint16 crc;			//CRC16校验码
	uint8 len;			//数据长度

	//获取数据长度
	len = frame[5];
	//检验帧字节长度是否合法
	if (frame_len - 8 != len) {
		return false;
	}
	//检验帧头是否合法
	if (frame[0] != FRAME_HEAD_LOW || frame[1] != FRAME_HEAD_HIGH) {
		return false;
	}
	//检验帧校验序列是否合法
	crc = crc16_modbus_block(frame + 2, len + 4);
	if (frame[frame_len - 2] != (uint8) crc
			|| frame[frame_len - 1] != crc >> 8) {
		return false;
	}
	//添加目的地址
	info->dest_addr = frame[2];
	//添加源地址
	info->src_addr = frame[3];
	//添加类型
	info->type = frame[4];
	//添加数据长度
	info->len = len;
	//添加数据
	memcpy(info->data, frame + 6, len);
	//转换成功
	return true;
}

//==========================================================================
//函数名称: frame_framing
//函数返回: 组帧结果:
//         FramingInvalidBuffer:无效的帧缓冲区号;
//         FramingFull:         帧缓冲区满，字节被丢弃;
//         FramingInvalidByte:  收到无效字节;
//         FramingHeadLow:      收到帧头低字节;
//         FramingHeadHigh:     收到帧头高字节;
//         FramingDestAddr:     收到目的地址;
//         FramingSrcAddr:      收到源地址;
//         FramingType:         收到帧类型;
//         FramingLen:          收到数据长度;
//         FramingData:         收到数据;
//         FramingFCS:          收到FCS低字节;
//         FramingFail:         收到FCS高字节，校验失败，帧被丢弃;
//         FramingOthers:       收到FCS高字节，目标地址不为本机或广播地址，帧被丢弃;
//         FramingSuccess:      收到FCS高字节，且该帧合法，被存放至帧缓冲区；
//参数说明: buffer:帧缓冲区号，取值范围为[0, FRAME_BUFFER_NUM - 1]
//         byte:接收到的字节
//功能概要: 在指定的帧缓冲区中，放入接收到的字节进行组帧
//==========================================================================
FrameFramingResult frame_framing(uint8 buffer, uint8 byte) {
	FrameBufferQueuePtr queue;	//帧缓冲区队列指针
	FrameFramingResult result;	//组帧结果

	//无效的帧缓冲区号
	if (buffer >= FRAME_BUFFER_NUM) {
		return FramingInvalidBuffer;
	}
	//获取帧缓冲区队列指针
	queue = buffer_queue + buffer;
	//帧缓冲区满
	if (queue->num >= FRAME_BUFFER_SIZE) {
		return FramingFull;
	}
	//帧缓冲区不满时，根据队列标志做相应操作
	switch (queue->flag) {
	case 0:		//未收到帧头低字节
		//若收到帧头低字节
		if (byte == FRAME_HEAD_LOW) {
			//更改标志
			queue->flag = 1;
			result = FramingHeadLow;
		} else {
			//收到无效字节
			result = FramingInvalidByte;
		}
		break;
	case 1:		//未收到帧头高字节
		//若收到帧头高字节
		if (byte == FRAME_HEAD_HIGH) {
			//更改标志
			queue->flag = 2;
			result = FramingHeadHigh;
		} else {
			//收到无效字节，重置标志
			queue->flag = 0;
			result = FramingInvalidByte;
		}
		break;
	case 2:		//未收到目的地址
		//收到目的地址
		queue->buffer[queue->rear].dest_addr = byte;
		//进行校验
		queue->last_crc = crc16_modbus_stream(queue->last_crc, byte);
		//更改标志
		queue->flag = 3;
		result = FramingDestAddr;
		break;
	case 3:		//未收到源地址
		//收到源地址
		queue->buffer[queue->rear].src_addr = byte;
		//进行校验
		queue->last_crc = crc16_modbus_stream(queue->last_crc, byte);
		//更改标志
		queue->flag = 4;
		result = FramingSrcAddr;
		break;
	case 4:		//未收到帧类型
		//收到帧类型
		queue->buffer[queue->rear].type = byte;
		//进行校验
		queue->last_crc = crc16_modbus_stream(queue->last_crc, byte);
		//更改标志
		queue->flag = 5;
		result = FramingType;
		break;
	case 5:		//未收到数据长度
		//收到数据长度
		queue->buffer[queue->rear].len = byte;
		//进行校验
		queue->last_crc = crc16_modbus_stream(queue->last_crc, byte);
		//根据数据长度更改标志
		queue->flag = byte == 0 ? 7 : 6;
		result = FramingLen;
		break;
	case 6:		//未接收完数据
		//收到数据
		queue->buffer[queue->rear].data[queue->recv_len++] = byte;
		//进行校验
		queue->last_crc = crc16_modbus_stream(queue->last_crc, byte);
		//检查是否接收完毕
		if (queue->recv_len >= queue->buffer[queue->rear].len) {
			//更改标志
			queue->flag = 7;
			//重置已经接收的字节数
			queue->recv_len = 0;
		}
		result = FramingData;
		break;
	case 7:		//未收到FCS低字节
		//检查FCS低字节是否校验正确
		if ((uint8) queue->last_crc == byte) {
			//校验成功
			queue->flag = 9;
		} else {
			//校验失败
			queue->flag = 8;
		}
		result = FramingFCS;
		break;
	case 8:		//FCS低字节校验失败
		//跳过检验FCS高字节，重置各静态变量
		queue->flag = 0;
		queue->last_crc = CRC16_MODBUS_STREAM_INIT_CRC;
		result = FramingFail;
		break;
	case 9:		//FCS低字节校验成功
		//校验FCS高字节
		if ((queue->last_crc >> 8) == byte) {
			//校验成功，检查目的地址是否为本机或广播地址
			if (queue->buffer[queue->rear].dest_addr
					== local_addr|| queue->buffer[queue->rear].dest_addr == FRAME_BROADCAST_ADDR) {
				//更改缓冲区队列队尾
				queue->rear = (queue->rear + 1) % FRAME_BUFFER_SIZE;
				queue->num++;
				result = FramingSuccess;
			} else {
				result = FramingOthers;
			}
		} else {
			//校验失败
			result = FramingFail;
		}
		//重置各静态变量
		queue->flag = 0;
		queue->last_crc = CRC16_MODBUS_STREAM_INIT_CRC;
		break;
	}
	//返回结果
	return result;
}

//==========================================================================
//函数名称: frame_get_info
//函数返回: true:获取成功; false:获取失败
//参数说明: buffer:帧缓冲区号，取值范围为[0, FRAME_BUFFER_NUM - 1]
//         info:存储帧信息结构体的地址
//功能概要: 指定帧缓冲区不为空时，取出最先存储的一帧
//==========================================================================
bool frame_get_info(uint8 buffer, FrameInfo* info) {
	FrameBufferQueuePtr queue;	//帧缓冲区队列指针

	//无效的帧缓冲区号
	if (buffer >= FRAME_BUFFER_NUM) {
		return false;
	}
	//获取帧缓冲区队列指针
	queue = buffer_queue + buffer;
	//若缓冲区为空，获取失败
	if (queue->num == 0) {
		return false;
	}
	//复制帧信息结构体
	memcpy(info, queue->buffer + queue->front, sizeof(FrameInfo));
	//更改缓冲区队列队首
	queue->num--;
	queue->front = (queue->front + 1) % FRAME_BUFFER_SIZE;
	//获取成功
	return true;
}
