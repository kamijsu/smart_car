//===========================================================================
//文件名称：motor.h
//功能概要：motor驱动构件头文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//版    本：V2.0, 20150522
//===========================================================================

#ifndef _MOTOR_H              //防止重复定义（_MOTOR_H  开头)
#define _MOTOR_H

//包含头文件
#include "common.h"
#include "pwm.h"

//两路PWM电机组号
#define MOTOR1 (0)
#define MOTOR2 (1)

//电机模块加通道号
#define MOTOR1_PWM1			U_FTM0
#define MOTOR1_PWM1_CH		U_FTMCH0	// U_FTM0_CH0    //PTC1

#define MOTOR1_PWM2			U_FTM0
#define MOTOR1_PWM2_CH		U_FTMCH1	// U_FTM0_CH1    //PTC2

#define MOTOR2_PWM1			U_FTM0
#define MOTOR2_PWM1_CH		U_FTMCH2	// U_FTM0_CH2    //PTC3

#define MOTOR2_PWM2			U_FTM0
#define MOTOR2_PWM2_CH		U_FTMCH3	// U_FTM0_CH3    //PTC4

//电机PWM周期
#define MOTOR_PERIOD (1000) //电机PWM周期，周期1ms（1000表示1ms,见PWM.h）

//电机转速上下限
#define MOTOR_SPEED_MAX		(PWM_ACCURACY)
#define MOTOR_SPEED_MIN		(-PWM_ACCURACY)

//====================================================================
//函数名称：motor_init
//函数返回：无
//参数说明：motorNo:选择电机 ：MOTOR1、MOTOR2
//功能概要：电机初始化，根据传入参数选择要初始化的电机（例：MOTOR1），然后根据宏定义的引脚（例：
//      MOTOR1_PWM1、MOTOR1_PWM2）和周期（MOTOR_PERIOD）初始化电机，初始占空比为0
//====================================================================
void motor_init(uint8_t motorNo);

//====================================================================
//函数名称：motor_set;
//函数返回：无
//参数说明：  motorNo:选择电机 ：MOTOR1、MOTOR2
//       duty：  占空比，控制车轮转速
//       有正负 ：MOTOR_SPEED_MIN ~ MOTOR_SPEED_MAX(具体值在PWM.h中定义)
//功能概要：设置电机速度，根据电机选择motor参数以及占空比duty来设置对应舵机的PWM占空比
//====================================================================
void motor_set(uint8_t motorNo, int16_t duty);

#endif //防止重复定义（_MOTOR_H 结尾)
