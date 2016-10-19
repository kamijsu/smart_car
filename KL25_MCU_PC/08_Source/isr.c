//=====================================================================
//文件名称：isr.c
//功能概要： 中断底层驱动构件源文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//更新记录：2012-11-12   V1.0
//=====================================================================

#include "includes.h"

//========================中断函数服务例程===============================

//=====================================================================
//函数名称：UART2_IRQHandler（串口2接收中断服务例程）
//功能概要：串口2接收一个数据，并将其打出
//=====================================================================
void UART2_IRQHandler(void)
{
	uint_8 ch;
	uint_8 flag = 1;

	DISABLE_INTERRUPTS;

	ch = uart_re1(UART_2, &flag);
	//获取成功
	if (0 == flag)
	{
		//将收到的指令添加至消息队列中
		g_msg.msg[g_msg.rear++] = ch;
		//防止越界
		if (g_msg.rear == 16)
		{
			g_msg.rear = 0;
		}
	}

	ENABLE_INTERRUPTS;
}

//==========================================================================
//函数名称：PIT_IRQHandler
//函数返回：无
//参数说明：无
//功能概要：PIT中断服务例程，清中断标志
//调用举例：无
//==========================================================================
void PIT_IRQHandler(void)
{
	//时间计数器
	static TimeCounter time_counter =
	{ 0, 0 };

	DISABLE_INTERRUPTS;

	//中断为PIT_CH0产生的中断
	if (pit_get_int(PIT_CH0) == 1)
	{
		//清中断标志
		pit_clear_int(PIT_CH0);

		//计数器增加
		++time_counter.c_50ms;
		++time_counter.c_1s;

		//50ms产生一个中断
		if (time_counter.c_50ms >= 1)
		{
			//到达50ms
			time_counter.c_50ms = 0;
			g_time_flag.f_50ms = 1;
		}

		if (time_counter.c_1s >= 20)
		{
			//到达1s
			time_counter.c_1s = 0;
			g_time_flag.f_1s = 1;
		}

	}

	ENABLE_INTERRUPTS;
}
