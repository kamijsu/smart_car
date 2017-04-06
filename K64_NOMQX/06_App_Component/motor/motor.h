//==========================================================================
//文件名称：motor.h
//功能概要：电机应用驱动程序头文件
//==========================================================================

#ifndef _MOTOR_H
#define _MOTOR_H

#include "common.h"
#include "ftm.h"

//定义电机号
#define MOTOR0	(0)
#define MOTOR1	(1)

//定义电机所使用的FTM模块配置，所有电机使用同一FTM模块，可选配置见ftm.h(注意:若相同FTM模块被用于不同应用构件，请保证它们的配置相同)
//定义所使用的FTM模块
#define MOTOR_FTM_MOD				FTM_MOD0					//FTM0
//定义FTM模块时钟分频因子
#define MOTOR_FTM_CLK_DIV			FTM_CLK_DIV_1				//1分频，即工作频率60MHz
//定义FTM模块计数器模式
#define MOTOR_FTM_COUNTER_MODE		FTM_COUNTER_MODE_UP			//向上计数
//定义FTM模块计数器计数周期，单位μs
#define MOTOR_FTM_COUNTER_PERIOD	(50)						//50μs，根据相应分频因子，PWM波频率为20kHz
//定义FTM模块PWM功能的模式
#define MOTOR_FTM_PWM_MODE			FTM_PWM_MODE_EDGE_ALIGNED	//PWM波边沿对齐
//定义FTM模块PWM功能的极性
#define MOTOR_FTM_PWM_POL			FTM_PWM_POL_POSITIVE		//正极性，即占空比越大，高电平所占比例越高

//定义电机所使用的PWM通道，每个电机使用两个通道，相应的引脚由ftm.h中的FTM_MODx_CHx_PIN决定
#define MOTOR0_PWM0_CH				FTM_CH1		//FTM0_CH1	C2
#define MOTOR0_PWM1_CH				FTM_CH0		//FTM0_CH0	C1
#define MOTOR1_PWM0_CH				FTM_CH3		//FTM0_CH3	C4
#define MOTOR1_PWM1_CH				FTM_CH2		//FTM0_CH2	C3

//定义电机占空比上下限，当duty>=0时，PWM0占空比为|duty|，PWM1占空比为0，当duty<0时，PWM0占空比为0，PWM1占空比为|duty|
#define MOTOR_DUTY_MAX				(FTM_DUTY_MAX * 0.98f)
#define MOTOR_DUTY_MIN				(-(FTM_DUTY_MAX * 0.98f))

//==========================================================================
//函数名称: motor_init
//函数返回: 无
//参数说明: 无
//功能概要: 初始化所有电机，初始占空比为0，相应配置在motor.h中
//==========================================================================
void motor_init();

//==========================================================================
//函数名称: motor_set
//函数返回: 无
//参数说明: motor:电机号:
//               MOTORx，x为电机号;
//         duty:占空比，范围[MOTOR_DUTY_MIN(-9800),MOTOR_DUTY_MAX(9800)]
//功能概要: 设置电机的占空比，将在下一个计数周期更新
//备注: 当duty>=0时，PWM0占空比为|duty|，PWM1占空比为0，
//     当duty<0时，PWM0占空比为0，PWM1占空比为|duty|，
//     占空比已做上下限限幅
//==========================================================================
void motor_set(uint8 motor, int16 duty);

#endif
