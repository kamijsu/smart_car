//==========================================================================
//文件名称：control.h
//功能概要：小车控制软件程序头文件
//==========================================================================

#ifndef _CONTROL_H
#define _CONTROL_H

#include "common.h"
#include "ftm.h"
#include "encoder.h"
#include "param.h"

//PID值转化为PWM值的比率
#define CONTROL_PID_PWM_RATIO 	FTM_PERIOD_ACCURACY

//定义角度控制PID值上下限
#define CONTROL_ANGLE_PID_VAL_MAX	(0.95f)
#define CONTROL_ANGLE_PID_VAL_MIN	(-0.95f)

//定义速度控制PID值上下限
#define CONTROL_SPEED_PID_VAL_MAX	(0.65f)
#define CONTROL_SPEED_PID_VAL_MIN	(-0.65f)

//定义速度控制周期，单位s，用于计算距离和加速度
#define CONTROL_SPEED_PERIOD 	(ENCODER_PERIOD / 1000.0f)

#define CONTROL_BASE_ROW_NUM	(5)
#define CONTROL_EDGE_RANGE		(3)

//==============================================================
//函数名称：control_angle_pid
//函数返回：无
//参数说明：angle:角度控制模块参数的地址
//功能概要：根据当前角度和角速度，进行角度PID控制(未用I)，
//         计算目标PWM值并重置PWM周期计数器，自动记录上次角速度
//备注: P值:使角度接近目标角度，与PWM值正相关
//     D值:使角速度接近0，即抑制角度变化，与PWM值正相关
//==============================================================
void control_angle_pid(ParamAngle* angle);

//==============================================================
//函数名称：control_speed_pid
//函数返回：无
//参数说明：speed:速度控制模块参数的地址
//功能概要：根据当前左轮和右轮速度，进行速度PID控制，
//         计算目标PWM值并重置PWM周期计数器，
//         自动记录上次速度差值和距离误差
//备注: P值:使速度接近目标速度，与PWM值正相关
//     I值:使距离误差接近0，即加快速度变化，与PWM值正相关
//     D值:使加速度接近0，即抑制速度变化，与PWM值正相关
//==============================================================
void control_speed_pid(ParamSpeed* speed);

void control_find_mid_points(ParamTurn* turn);

void control_cal_avg_mid_point(ParamTurn* turn);

void control_cal_slope(ParamTurn* turn);

void control_turn_pid(ParamTurn* turn);

//===========================================================================
//函数名称：control_update_output_pwm
//函数返回：无
//参数说明：pwm:PWM参数的地址
//功能概要：更新输出的PWM值，这种方式可以使输出的PWM值平滑地变化至目标PWM值
//===========================================================================
void control_update_output_pwm(ParamPWM* pwm);

//===========================================================================
//函数名称：control_update_motor_pwm
//函数返回：无
//参数说明：car:小车参数的地址
//功能概要：更新并设置电机输出的PWM值
//===========================================================================
void control_update_motor_pwm(ParamCar* car);

//===========================================================================
//函数名称：control_stop_car
//函数返回：无
//参数说明：无
//功能概要：设置电机输出的PWM值为0
//===========================================================================
void control_stop_car();

#endif
