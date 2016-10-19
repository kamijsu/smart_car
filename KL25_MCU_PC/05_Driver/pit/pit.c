//==========================================================================
//文件名称：pit.c
//功能概要：KL25 pit底层驱动程序文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//更新记录：2016-3-20   V4.0
//==========================================================================

#include "pit.h"

//==========================================================================
//函数名称: pit_init                                                         
//函数返回: 无                                          
//参数说明: PIT_CHn:PIT模块的通道号，PIT_CH0或PIT_CH1。
//         int_ms:中断周期，以ms为单位。系统总线时钟为24MHz，最大值为178956ms
//功能概要: PIT模块初始化。
//==========================================================================
void pit_init(uint_8 PIT_CHn, uint_32 int_ms)
{
	BSET(SIM_SCGC6_PIT_SHIFT, SIM_SCGC6);			//开PIT时钟门
	BCLR(PIT_MCR_MDIS_SHIFT, PIT_MCR);				//使能PIT模块
	BSET(PIT_MCR_FRZ_SHIFT, PIT_MCR);				//调试模式下禁止
	PIT_LDVAL(PIT_CHn) = int_ms * PIT_WORK_FREQ;	//设置计数器最大初始化值
	PIT_TCTRL(PIT_CHn) |= PIT_TCTRL_TEN_MASK;		//使能pit通道定时器
	PIT_TCTRL(PIT_CHn) &= ~(PIT_TCTRL_TIE_MASK);	//默认禁止pit通道中断
}

//==========================================================================
//函数名称: pit_enable_int
//参数说明: PIT_CHn:PIT模块的通道号，PIT_CH0或PIT_CH1。
//函数返回: 无
//功能概要: 使能某一通道的PIT中断。
//==========================================================================
void pit_enable_int(uint_8 PIT_CHn)
{
	PIT_TCTRL(PIT_CHn) |= (PIT_TCTRL_TIE_MASK);	//开PIT通道中断
	NVIC_EnableIRQ(PIT_IRQn);					//开接收引脚的IRQ中断
}

//==========================================================================
//函数名称: pit_disable_int
//参数说明: PIT_CHn:PIT模块的通道号，PIT_CH0或PIT_CH1。
//函数返回: 无
//功能概要: 禁止某一通道的PIT中断。
//==========================================================================
void pit_disable_int(uint_8 PIT_CHn)
{
	PIT_TCTRL(PIT_CHn) &= ~(PIT_TCTRL_TIE_MASK);   //关PIT通道中断
	NVIC_DisableIRQ(PIT_IRQn);                     //关接收引脚的IRQ中断
}

//==========================================================================
//函数名称: pit_get_int
//参数说明: PIT_CHn:PIT模块的通道号，PIT_CH0或PIT_CH1。
//函数返回: 中断标志 1=有中断产生;0=无中断产生
//功能概要: 获取某一通道的PIT中断状态。
//==========================================================================
uint_8 pit_get_int(uint_8 PIT_CHn)
{
	return (PIT_TFLG(PIT_CHn) & PIT_TFLG_TIF_MASK) == (PIT_TFLG_TIF_MASK);
}

//==========================================================================
//函数名称: pit_clear_int
//参数说明: PIT_CHn:PIT模块的通道号，PIT_CH0或PIT_CH1。
//函数返回: 无
//功能概要: 清除某一通道的PIT中断标志。
//==========================================================================
void pit_clear_int(uint_8 PIT_CHn)
{
	PIT_TFLG(PIT_CHn) |= PIT_TFLG_TIF_MASK;
}
