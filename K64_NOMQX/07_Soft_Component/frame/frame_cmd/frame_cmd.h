//==========================================================================
//文件名称：frame_cmd.h
//功能概要：组帧通信协议命令组件程序头文件
//==========================================================================

#ifndef _FRAME_CMD_H
#define _FRAME_CMD_H

#include "common.h"
#include "frame.h"

//==========================================================================
//说明: 帧的<数据>字段格式:<命令类型> <命令信息长度> <命令信息>
//     <命令类型>为1个字节，表明该命令的类型;
//     <命令信息长度>为1个字节，表示命令信息的长度，不能超过253;
//     <命令信息>为剩余字节，具体格式由命令类型决定，长度最多不能超过253字节;
//传输顺序: 命令类型-命令信息长度-命令信息低字节-命令信息高字节
//==========================================================================

//定义命令组件所使用的帧类型，不能与其他组件冲突
#define FRAME_CMD_TYPE	(0x02)

//定义帧命令信息结构体，每个帧命令信息结构体占用255字节的RAM
typedef struct {
	uint8 type;			//命令类型
	uint8 len;			//命令信息长度
	uint8 data[253];	//命令信息
} FrameCmdInfo;

//定义命令组件发送结果枚举类型
typedef enum {
	CmdSendSuccess,		//发送命令成功
	CmdSendIllegalLen,	//非法命令信息长度
	CmdSendInfoFail		//发送帧信息结构体失败
} FrameCmdSendResult;

//定义命令组件解析结果枚举类型
typedef enum {
	CmdParseSuccess, 		//命令解析成功
	CmdParseWrongType, 		//帧类型错误
	CmdParseIllegalInfoLen,	//帧数据长度非法
	CmdParseIllegalCmdLen	//命令信息长度非法
} FrameCmdParseResult;

//==========================================================================
//函数名称: frame_cmd_send
//函数返回: 命令发送结果:
//         CmdSendSuccess:   发送命令成功;
//         CmdSendIllegalLen:非法命令信息长度;
//         CmdSendInfoFail:  发送帧信息结构体失败;
//参数说明: cmd:要发送的帧命令信息结构体
//功能概要: 发送帧命令信息结构体，自动组装帧信息结构体
//备注: 需先使能组帧通信协议
//==========================================================================
FrameCmdSendResult frame_cmd_send(FrameCmdInfo cmd);

//==========================================================================
//函数名称: frame_cmd_parse
//函数返回: 命令解析结果:
//         CmdParseSuccess:       命令解析成功;
//         CmdParseWrongType:     帧类型错误;
//         CmdParseIllegalInfoLen:帧数据长度非法;
//         CmdParseIllegalCmdLen: 命令信息长度非法;
//参数说明: info:要解析的帧信息结构体
//         cmd:存储帧命令信息结构体的首地址
//功能概要: 解析帧信息结构体，解析成功时，存储相应帧命令信息结构体
//==========================================================================
FrameCmdParseResult frame_cmd_parse(FrameInfo info, FrameCmdInfo* cmd);

#endif
