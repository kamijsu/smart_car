//==========================================================================
//文件名称：frame_cmd.c
//功能概要：组帧通信协议命令组件程序源文件
//==========================================================================

#include "frame_cmd.h"

//==========================================================================
//函数名称: frame_cmd_send
//函数返回: true:发送命令成功; false:非法命令信息长度
//参数说明: cmd:要发送的帧命令信息结构体
//功能概要: 发送帧命令信息结构体，自动组装帧信息结构体
//备注: 需先使能组帧通信协议
//==========================================================================
bool frame_cmd_send(FrameCmdInfo cmd) {
	FrameInfo info;	//要发送的帧信息结构体
	uint8 i;		//游标
	//检查命令信息长度是否合法
	if (cmd.len > 253) {
		return false;
	}
	//设置类型
	info.type = FRAME_CMD_TYPE;
	//设置长度
	info.len = cmd.len + 2;
	//设置数据
	info.data[0] = cmd.type;
	info.data[1] = cmd.len;
	for (i = 0; i < cmd.len; i++) {
		info.data[i + 2] = cmd.data[i];
	}
	//发送帧信息结构体
	frame_send_info(info);
	return true;
}

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
FrameCmdParseResult frame_cmd_parse(FrameInfo info, FrameCmdInfo* cmd) {
	uint8 i;	//游标

	//检验帧类型是否正确
	if (info.type != FRAME_CMD_TYPE) {
		return CmdParseWrongType;
	}
	//检验帧数据长度是否合法
	if (info.len < 2) {
		return CmdParseIllegalInfoLen;
	}
	//检验命令信息长度是否合法
	if (info.data[1] != info.len - 2) {
		return CmdParseIllegalCmdLen;
	}
	//存储帧命令信息结构体
	cmd->type = info.data[0];
	cmd->len = info.data[1];
	for (i = 2; i < info.len; i++) {
		cmd->data[i - 2] = info.data[i];
	}
	return CmdParseSuccess;
}
