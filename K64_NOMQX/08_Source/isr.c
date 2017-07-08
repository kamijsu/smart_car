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
	FrameFramingResult res;
	uint8 ch;
	bool err;
	DISABLE_INTERRUPTS;

	if (uart_re1_parity(UART_MOD0, &ch, &err)) {
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
	static uint32 i = 0;
	uint8 key;

	DISABLE_INTERRUPTS;
	if (keyboard_get_int()) {
		if (keyboard_read(&key))
			menu_accept(key);
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

//============================================================================
//函数名称：PORTD_IRQHandler
//函数参数：
//函数返回：无
//功能概要：PORTD端口中断服务函数
//============================================================================
void PORTD_IRQHandler() {
	uint8_t n;    //引脚号

	DISABLE_INTERRUPTS; //关中断

	if (reed_switch_get_int()) {
		reed_switch_clear_int();
		reed_switch_disable_int();
		uart_send_string(UART_USE, "产生中断！\n");
	}

	ENABLE_INTERRUPTS; //恢复原总中断设置情况
}
