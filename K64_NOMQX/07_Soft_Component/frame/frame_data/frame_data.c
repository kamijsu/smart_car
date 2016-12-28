//==========================================================================
//文件名称：frame_data.c
//功能概要：组帧通信协议数据组件程序源文件
//==========================================================================

#include "frame_data.h"

//==========================================================================
//函数名称: frame_data_send
//函数返回: 数据发送结果:
//         DataSendSuccess:   发送数据成功;
//         DataSendIllegalLen:非法数据信息长度;
//         DataSendInfoFail:  发送帧信息结构体失败;
//参数说明: data:要发送的帧数据信息结构体
//功能概要: 发送帧数据信息结构体，自动组装帧信息结构体
//备注: 需先使能组帧通信协议
//==========================================================================
FrameDataSendResult frame_data_send(FrameDataInfo data) {
	FrameInfo info;	//要发送的帧信息结构体
	uint8 i;		//游标
	//检查数据信息长度是否合法
	if (data.len > 253) {
		return DataSendIllegalLen;
	}
	//设置类型
	info.type = FRAME_DATA_TYPE;
	//设置长度
	info.len = data.len + 2;
	//设置数据
	info.data[0] = data.type;
	info.data[1] = data.len;
	for (i = 0; i < data.len; i++) {
		info.data[i + 2] = data.data[i];
	}
	//发送帧信息结构体
	if (frame_send_info(info)) {
		//发送成功
		return DataSendSuccess;
	} else {
		//发送失败
		return DataSendInfoFail;
	}
}

//==========================================================================
//函数名称: frame_data_parse
//函数返回: 数据解析结果:
//         DataParseSuccess:       数据解析成功;
//         DataParseWrongType:     帧类型错误;
//         DataParseIllegalInfoLen:帧数据长度非法;
//         DataParseIllegalDataLen:数据信息长度非法;
//参数说明: info:要解析的帧信息结构体
//         data:存储帧数据信息结构体的首地址
//功能概要: 解析帧信息结构体，解析成功时，存储相应帧数据信息结构体
//==========================================================================
FrameDataParseResult frame_data_parse(FrameInfo info, FrameDataInfo* data) {
	uint8 i;	//游标

	//检验帧类型是否正确
	if (info.type != FRAME_DATA_TYPE) {
		return DataParseWrongType;
	}
	//检验帧数据长度是否合法
	if (info.len < 2) {
		return DataParseIllegalInfoLen;
	}
	//检验数据信息长度是否合法
	if (info.data[1] != info.len - 2) {
		return DataParseIllegalDataLen;
	}
	//存储帧数据信息结构体
	data->type = info.data[0];
	data->len = info.data[1];
	for (i = 2; i < info.len; i++) {
		data->data[i - 2] = info.data[i];
	}
	return DataParseSuccess;
}
