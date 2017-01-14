//===========================================================================
//文件名称：isr.c
//功能概要：自定义中断服务例程源文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//版本更新：2013-05-03    V1.0,2014-05-10    V2.3(WYH) 
//===========================================================================

#include "includes.h"

//==========================================================================
//函数名称：UART1_RX_TX_IRQHandler
//参数说明：无
//函数返回：无
//功能概要：UART1中断服务函数
//==========================================================================
void UART1_RX_TX_IRQHandler() {
	FrameFramingResult res;
	uint8 ch;
	bool err = true;
	DISABLE_INTERRUPTS;

	res = frame_framing();
	uart_send1(UART_MOD1, res);

//	if (uart_re1_parity(UART_MOD1, &ch, &err)) {
//		if (!err) {
//			uart_send1(UART_MOD1, ch);
//		}
//	}

	ENABLE_INTERRUPTS;
}

void FTM0_IRQHandler() {
	if (ftm_timer_get_int(0)) {
		ftm_timer_clear_int(0);
		time0_flag.f_1s = 1;

	}

}

//==========================================================================
//函数名称：PIT0_IRQHandler
//参数说明：无
//函数返回：无
//功能概要：PIT0中断服务函数
//==========================================================================
void PIT0_IRQHandler() {
	static TimeCounter time_counter = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	DISABLE_INTERRUPTS; //关中断

	pit_clear_int(PIT_CH0); //清标志

	//这里为了效率停用了没有使用的时间标志
//	uart_send1(UART_MOD1,'s');
	++time_counter.c_5ms;
//	++time_counter.c_10ms;
//	++time_counter.c_15ms;
	++time_counter.c_20ms;
	++time_counter.c_50ms;
//	++time_counter.c_100ms;
	++time_counter.c_1s;
	++time_counter.c_5s;
	++time_counter.c_10s;
	++time_counter.c_15s;
//	++time_counter.c_30s;
//	++time_counter.c_1min;

	if (time_counter.c_5ms >= 1) {
		time_counter.c_5ms = 0;
		time0_flag.f_5ms = 1;
	}       //5ms置标志
//	if (time_counter.c_10ms >= 2)
//	{
//		time_counter.c_10ms = 0;
//		time0_flag.f_10ms = 1;
//	}       //10ms置标志
//	if (time_counter.c_15ms >= 3)
//	{
//		time_counter.c_15ms = 0;
//		time0_flag.f_15ms = 1;
//	}       //15ms置标志
	if (time_counter.c_20ms >= 4) {
		time_counter.c_20ms = 0;
		time0_flag.f_20ms = 1;
	}       //20ms置标志
	if (time_counter.c_50ms >= 10) {
		time_counter.c_50ms = 0;
		time0_flag.f_50ms = 1;
	}       //50ms置标志
//	if (time_counter.c_100ms >= 20)
//	{
//		time_counter.c_100ms = 0;
//		time0_flag.f_100ms = 1;
//	}       //100ms置标志
	if (time_counter.c_1s >= 200) {
		time_counter.c_1s = 0;
		time0_flag.f_1s = 1;
	}       //1s置标志
	if (time_counter.c_5s >= 1000) {
		time_counter.c_5s = 0;
		time0_flag.f_5s = 1;
	}       //5s置标志
	if (time_counter.c_10s >= 2000) {
		time_counter.c_10s = 0;
		time0_flag.f_10s = 1;
	}       //10s置标志
	if (time_counter.c_15s >= 3000) {
		time_counter.c_15s = 0;
		time0_flag.f_15s = 1;
	}       //15s置标志
//	if (time_counter.c_30s >= 6000)
//	{
//		time_counter.c_30s = 0;
//		time0_flag.f_30s = 1;
//	}       //30s置标志
//	if (time_counter.c_1min >= 12000)
//	{
//		time_counter.c_1min = 0;
//		time0_flag.f_1min = 1;
//	}       //1min置标志

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
