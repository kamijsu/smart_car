//==========================================================================
//文件名称：vcan.c
//功能概要：山外多功能调试助手软件程序源文件
//==========================================================================

#include "vcan.h"
#include "camera.h"

//==========================================================================
//函数名称: vcan_send_raw_img
//函数返回: 无
//参数说明: raw_img:原始图像首地址
//功能概要: 在山外多功能调试助手上显示采集的图像信息
//==========================================================================
void vcan_send_raw_img(uint8* raw_img) {
	//山外多功能调试助手所使用的命令
	uint8 cmdf[2] = { VCAN_CMD_IMG, ~VCAN_CMD_IMG };
	uint8 cmdr[2] = { ~VCAN_CMD_IMG, VCAN_CMD_IMG };

	//发送命令
	uart_sendN(VCAN_UART_MOD, cmdf, sizeof(cmdf));
	//发送图像
	uart_sendN(VCAN_UART_MOD, raw_img, CAMERA_RAW_IMG_BYTES);
	//发送命令
	uart_sendN(VCAN_UART_MOD, cmdr, sizeof(cmdr));
}

//==========================================================================
//函数名称: vcan_send_img
//函数返回: 无
//参数说明: img:图像首地址
//功能概要: 在山外多功能调试助手上显示图像信息
//==========================================================================
void vcan_send_img(uint8* img) {
	//山外多功能调试助手所使用的命令
	uint8 cmdf[2] = { VCAN_CMD_IMG, ~VCAN_CMD_IMG };
	uint8 cmdr[2] = { ~VCAN_CMD_IMG, VCAN_CMD_IMG };

	//发送命令
	uart_sendN(VCAN_UART_MOD, cmdf, sizeof(cmdf));
	//发送图像
	uart_sendN(VCAN_UART_MOD, img, CAMERA_IMG_HEIGHT * CAMERA_IMG_WIDTH);
	//发送命令
	uart_sendN(VCAN_UART_MOD, cmdr, sizeof(cmdr));
}
