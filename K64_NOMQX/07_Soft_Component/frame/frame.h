//==========================================================================
//文件名称：frame.h
//功能概要：组帧通信协议软件程序头文件
//==========================================================================

#ifndef _FRAME_H
#define _FRAME_H

#include "common.h"
#include "crc16_modbus.h"
#include "uart.h"

//==========================================================================
//说明: 帧格式: <帧头> <类型> <数据长度> <数据> <帧校验序列>;
//     字段                  字段长度                说明
//     <帧头>       2字节                     表示帧开始传输
//     <类型>       1字节                     表示该帧类型
//     <数据长度>    1字节                    表示传输的数据字节数
//     <数据>       <数据长度>      传输的数据，不能超过255字节
//     <帧校验序列>  2字节                     采用CRC16校验，校验失败时，帧被丢弃，
//                                 校验字段为<类型> <数据长度> <数据>
//传输顺序: 帧头低字节-帧头高字节-类型-数据长度-数据低字节-数据高字节-FCS低字节-FCS高字节
//备注: uint16型等超过1字节的数据，存储时高字节在高地址，而&取得的首地址为低地址，
//     因此若数据为0xFEDA，传输顺序为0xDA-0xFE
//==========================================================================

//设置组帧通信协议所使用的UART模块号
#define FRAME_UART_MOD		UART_MOD1	//Tx:E0 Rx:E1
//设置组帧通信协议所使用的UART波特率
#define FRAME_UART_BAUD		(9600)
//设置组帧通信协议所使用的UART校验模式
#define FRAME_UART_PARITY		UART_PARITY_DISABLED	//不启用校验
//设置组帧通信协议所使用的UART停止位
#define FRAME_UART_STOP_BIT		UART_STOP_BIT_1			//1位停止位
//设置组帧通信协议所使用的UART位传输顺序
#define FRAME_UART_BIT_ORDER	UART_BIT_ORDER_LSB		//最低有效位

//定义帧头，为0xABFE
#define FRAME_HEAD_HIGH		(0xAB)	//高字节
#define FRAME_HEAD_LOW		(0xFE)	//低字节，先传输与接收

//定义帧信息结构体，每个帧信息结构体占用257字节的RAM
typedef struct {
	uint8 type;			//帧类型
	uint8 len;			//数据长度
	uint8 data[255];	//数据
} FrameInfo;

//定义帧缓冲区大小，即最多可以缓存 FRAME_BUFFER_SIZE 个帧信息结构体，缓冲区满时，接收到的字节被丢弃
#define FRAME_BUFFER_SIZE	(5)

//定义组帧结果枚举类型
typedef enum {
	FramingNone,		//未接收到字节
	FramingFull,		//帧缓冲区满，字节被丢弃
	FramingInvalid,		//收到无效字节
	FramingHeadLow,		//收到帧头低字节
	FramingHeadHigh,	//收到帧头高字节
	FramingType,		//收到帧类型
	FramingLen,			//收到数据长度
	FramingData,		//收到数据
	FramingFCS,			//收到FCS低字节
	FramingSuccess,		//收到FCS高字节，且校验成功
	FramingFail			//收到FCS高字节，且校验失败，帧被丢弃
} FrameFramingResult;

//==========================================================================
//函数名称: frame_init
//函数返回: 无
//参数说明: 无
//功能概要: 使能组帧通信协议，相应配置在frame.h中
//==========================================================================
void frame_init();

//==========================================================================
//函数名称: frame_enable_re_int
//函数返回: 无
//参数说明: 无
//功能概要: 使能组帧通信协议接收中断，即相应的UART接收中断，通过组帧可以清除中断标志
//==========================================================================
void frame_enable_re_int();

//==========================================================================
//函数名称: frame_disable_re_int
//函数返回: 无
//参数说明: 无
//功能概要: 关闭组帧通信协议接收中断，即相应的UART接收中断
//==========================================================================
void frame_disable_re_int();

//==========================================================================
//函数名称: frame_send_info
//函数返回: 无
//参数说明: info:要发送的帧信息结构体
//功能概要: 发送帧信息结构体，自动添加帧头与帧校验序列
//==========================================================================
void frame_send_info(FrameInfo info);

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
FrameFramingResult frame_framing();

//==========================================================================
//函数名称: frame_get_info
//函数返回: true:获取成功; false:获取失败
//参数说明: info:存储帧信息结构体的地址
//功能概要: 帧缓冲区不为空时，取出最先存储的一帧
//==========================================================================
bool frame_get_info(FrameInfo* info);

#endif
