//==========================================================================
//文件名称：frame_string.c
//功能概要：组帧通信协议字符串组件程序源文件
//==========================================================================

#include "frame_string.h"

//==========================================================================
//函数名称: frame_string_send
//函数返回: true:发送字符串成功; false:字符串长度超过255字节
//参数说明: str:要发送的字符串首地址
//功能概要: 发送以'\0'结束的字符串，自动组装帧信息结构体
//备注: 需先使能组帧通信协议;字符串长度不能超过255字节，不包括'\0'，不发送'\0'
//==========================================================================
bool frame_string_send(uint8* str) {
	FrameInfo info;	//要发送的帧信息结构体

	//设置类型
	info.type = FRAME_STRING_TYPE;
	//遍历字符串，获取长度，并赋值
	for (info.len = 0; str[info.len] != '\0' && info.len < 255; info.len++) {
		info.data[info.len] = str[info.len];
	}
	//检查字符串长度是否超过255字节
	if (info.len == 255 && str[255] != '\0') {
		return false;
	}
	//发送帧信息结构体
	frame_send_info(info);
	return true;
}

//==========================================================================
//函数名称: frame_string_parse
//函数返回: 字符串解析结果:
//         StringParseSuccess:      字符串解析成功;
//         StringParseWrongType:    帧类型错误;
//         StringParseIllegalString:字符串非法，即在帧指示的字符串长度内出现'\0';
//参数说明: info:要解析的帧信息结构体
//         str:存储字符串的首地址
//功能概要: 解析帧信息结构体，解析成功时，存储相应字符串
//备注: 会自动在字符串结束时添加'\0'
//==========================================================================
FrameStringParseResult frame_string_parse(FrameInfo info, uint8* str) {
	uint8 i;	//游标
	//检验帧类型是否正确
	if (info.type != FRAME_STRING_TYPE) {
		return StringParseWrongType;
	}
	//检验是否非法字符串
	for (i = 0; i < info.len; i++) {
		if (info.data[i] == '\0') {
			return StringParseIllegalString;
		}
	}
	//存储字符串
	for (i = 0; i < info.len; i++) {
		str[i] = info.data[i];
	}
	//为字符串添加'\0';
	str[i] = '\0';
	return StringParseSuccess;
}
