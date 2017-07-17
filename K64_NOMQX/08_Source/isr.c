//===========================================================================
//文件名称：isr.c
//功能概要：自定义中断服务例程源文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//版本更新：2013-05-03    V1.0,2014-05-10    V2.3(WYH)
//===========================================================================

#include "includes.h"

//==========================================================================
//函数名称：UART0_RX_TX_IRQHandler
//参数说明：无
//函数返回：无
//功能概要：UART1中断服务函数
//==========================================================================
void UART0_RX_TX_IRQHandler() {
	uint8 ch;
	bool err;
	DISABLE_INTERRUPTS;

	//接收一个字节数据
	if (uart_re1_parity(UART_MOD0, &ch, &err)) {
		//校验正确时，加入0通道组帧
		if (!err) {
			frame_framing(0, ch);
		}
	}

	ENABLE_INTERRUPTS;
}

//==========================================================================
//函数名称：PORTA_IRQHandler
//参数说明：无
//函数返回：无
//功能概要：PORTA中断服务函数
//==========================================================================
void PORTA_IRQHandler() {
	uint8 key;

	DISABLE_INTERRUPTS;

	//若发生键盘中断
	if (keyboard_get_int()) {
		//读取键盘字符
		if (keyboard_read(&key)) {
			//将字符输入菜单中
			menu_accept(key);
		}
		//清除键盘中断标志
		keyboard_clear_int();
	}

	ENABLE_INTERRUPTS;
}

//==========================================================================
//函数名称：PORTC_IRQHandler
//参数说明：无
//函数返回：无
//功能概要：PORTC中断服务函数
//==========================================================================
void PORTC_IRQHandler() {
	DISABLE_INTERRUPTS;
	//接收到场中断
	if (camera_get_vsync_int()) {
		//开始采集数据
		camera_start_collecting();
		//停止接收场中断
		camera_disable_vsync_int();
		//关闭场中断标志
		camera_clear_vsync_int();
	}
	ENABLE_INTERRUPTS;
}

//==========================================================================
//函数名称：DMA0_IRQHandler
//参数说明：无
//函数返回：无
//功能概要：DMA0中断服务函数
//==========================================================================
void DMA0_IRQHandler() {
	DISABLE_INTERRUPTS;
	//停止采集数据
	camera_stop_collecting();
	//清除采集完成中断标志
	camera_clear_collect_done_int();
	//图像采集完成
	raw_img_done = true;
	ENABLE_INTERRUPTS;
}

//==========================================================================
//函数名称：PIT0_IRQHandler
//参数说明：无
//函数返回：无
//功能概要：PIT0中断服务函数
//==========================================================================
void PIT0_IRQHandler() {
	DISABLE_INTERRUPTS; //关中断

	pit_clear_int(PIT_CH0); //清标志

	time_update(5);	//增加5ms时间

	ENABLE_INTERRUPTS; //恢复原总中断设置情况
}
