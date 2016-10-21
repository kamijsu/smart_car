//==========================================================================
//文件名称：pit.c
//功能概要：K64 pit底层驱动程序文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//版本更新：2011-11-13  V1.0  初始版本。
//         2014-11-9   V2.0  KDS和新框架
//==========================================================================

#include "pit.h"

//==========================================================================
//函数名称: pit_init
//函数返回: 无
//参数说明: pitNo:表示pit通道号。
//		 timeout:表示定时溢出开始递减的值
//功能概要: 设置相关寄存器的值        
//==========================================================================
void pit_init(uint8_t pitNo, uint32_t timeout)
{
	SIM_SCGC6 |= SIM_SCGC6_PIT_MASK;              //使能PIT时钟
	PIT_MCR &= ~(PIT_MCR_MDIS_MASK);              //调试模式下禁止
	PIT_MCR |= PIT_MCR_FRZ_MASK;                  //使能PIT模块时钟
	PIT_LDVAL(pitNo) = timeout;                   //设置周期
	PIT_TCTRL(pitNo) |= PIT_TCTRL_TEN_MASK;       //使能pit模块运行
	PIT_TCTRL(pitNo) &= ~(PIT_TCTRL_TIE_MASK);    //关pit中断
}

//==========================================================================
//函数名称: pit_enable_int
//函数返回: 无                                              
//参数说明: pitNo: 表示pit通道号
//功能概要: 开PIT中断                                                                                                     
//==========================================================================
void pit_enable_int(uint8_t pitNo)
{
	PIT_TCTRL(pitNo) |= (PIT_TCTRL_TIE_MASK); //开pit中断
	NVIC_EnableIRQ(PIT0_IRQn + pitNo);		//开接收引脚的IRQ中断
}

//==========================================================================
//函数名称: pit_disable_int
//函数返回: 无                                              
//参数说明: pitNo: 表示pit通道号
//功能概要: 关闭PIT中断                                                                                                     
//==========================================================================
void pit_disable_int(uint8_t pitNo)
{
	PIT_TCTRL(pitNo) &= ~(PIT_TCTRL_TIE_MASK);		//关pit中断
	NVIC_DisableIRQ(PIT0_IRQn + pitNo);		//关接收引脚的IRQ中断
}
