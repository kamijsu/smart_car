//===========================================================================
//文件名称：pwm.c
//功能概要：pwm底层驱动构件源文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//更新记录：2013-5-5   V3.0 李涛
//===========================================================================

#ifndef _PWM_H
#define _PWM_H

#include "common.h"
#include "gpio.h"

#define PWM_ACCURACY (10000) //允许占空比(duty)范围为0~PWM_ACCURACY,精度至少为1

//U_FTM模块号
#define U_FTM0  (0)
#define U_FTM1  (1)
#define U_FTM2  (2)
#define U_FTM3  (3)
//U_FTM通道号
#define U_FTMCH0  (0)
#define U_FTMCH1  (1)
#define U_FTMCH2  (2)
#define U_FTMCH3  (3)
#define U_FTMCH4  (4)
#define U_FTMCH5  (5)
#define U_FTMCH6  (6)
#define U_FTMCH7  (7)
//定义U_FTM通道号选择                     		  	可用的端口_引脚
#define U_FTM0_CH0 (U_PORTC|1) // A3  C1
#define U_FTM0_CH1 (U_PORTC|2) // A4  C2
#define U_FTM0_CH2 (U_PORTC|3) // A5  C3  C5
#define U_FTM0_CH3 (U_PORTC|4) // A6  C4
#define U_FTM0_CH4 (U_PORTD|4) // A7  B12 D4
#define U_FTM0_CH5 (U_PORTD|5) // A0  B13 D5
#define U_FTM0_CH6 (U_PORTD|6) // A1  D6
#define U_FTM0_CH7 (U_PORTD|7) // A2  D7

#define U_FTM1_CH0 (U_PORTA|12)// A8  A12  B0  B12
#define U_FTM1_CH1 (U_PORTA|13)// A9  A13  B1  B13

#define U_FTM2_CH0 (U_PORTA|10)// A10 B18
#define U_FTM2_CH1 (U_PORTA|11)// A11 B19

#define U_FTM3_CH0 (U_PORTE|5) // D0  E5
#define U_FTM3_CH1 (U_PORTE|6) // D1  E6
#define U_FTM3_CH2 (U_PORTE|7) // D2  E7
#define U_FTM3_CH3 (U_PORTE|8) // D3  E8
#define U_FTM3_CH4 (U_PORTE|9) // C8  E9
#define U_FTM3_CH5 (U_PORTE|10)// C9  E10
#define U_FTM3_CH6 (U_PORTE|11)// C10 E11
#define U_FTM3_CH7 (U_PORTE|12)// C11 E12

#define bus_clk_khz (48000)

//=========================================================================
//函数名称：pwm_init
//功能概要：pwm模块初始化。
//参数说明：ftm_no:模块号：U_FTM0 ~ U_FTM3
//		 channel:通道号:U_FTMCH0 ~ U_FTMCH7 详见pwm.h文件定义
//       period:周期 单位微秒（比如0.1ms为100,1ms为1000）
//       duty:占空比    0~PWM_ACCURACY(在PWM.h中定义)
//函数返回：	0	成功
//			1	模块号错误
//			2	通道号错误
//			3	频率大于时钟的二分之一
//=========================================================================
uint8_t pwm_init(uint8_t ftmNo, uint8_t channel, uint32_t period,
		uint16_t duty);

//=========================================================================
//函数名称：pwm_set
//功能概要：设置pwm模块。
//参数说明：ftm_no:模块号：U_FTM0 ~ U_FTM3
//		 channel:通道号:U_FTMCH0 ~ U_FTMCH7 详见pwm.h文件定义
//       period:周期 单位微秒（比如0.1ms为100,1ms为1000）
//       duty:占空比    0~PWM_ACCURACY(在PWM.h中定义)
//函数返回：	0	成功
//			1	模块号、通道号错误
//			2	频率大于时钟的二分之一
//			3	占空比错误
//=========================================================================
uint8_t pwm_set(uint8_t ftmNo, uint8_t channel, uint32_t period, uint16_t duty);

#endif

//===========================================================================
//声明：
//（1）我们开发的源代码，在本中心提供的硬件系统测试通过，真诚奉献给社会，不足之处，
//     欢迎指正。
//（2）对于使用非本中心硬件系统的用户，移植代码时，请仔细根据自己的硬件匹配。
//
//苏州大学飞思卡尔嵌入式中心（苏州华祥信息科技有限公司）
//技术咨询：0512-65214835  http://sumcu.suda.edu.cn
//业务咨询：0512-87661670,18915522016  http://www.hxtek.com.cn
