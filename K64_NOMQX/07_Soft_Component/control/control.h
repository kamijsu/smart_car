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

//定义图像基行数
#define CONTROL_BASE_ROW_NUM	(5)

//==============================================================
//函数名称：control_angle_pid
//函数返回：无
//参数说明：angle:角度控制模块参数的地址
//功能概要：根据当前角度和角速度，进行角度PID控制(未用I)，
//         计算目标PWM值并重置PWM周期计数器，自动记录上次角速度
//备注: P值:使角度接近目标角度
//     D值:使角速度接近0，即抑制角度变化
//==============================================================
void control_angle_pid(ParamAngle* angle);

//==============================================================
//函数名称：control_speed_pid
//函数返回：无
//参数说明：speed:速度控制模块参数的地址
//功能概要：根据当前左轮和右轮速度，进行速度PID控制，
//         计算目标PWM值并重置PWM周期计数器，
//         自动记录上次速度差值和距离误差
//备注: P值:使速度接近目标速度
//     I值:使距离误差接近0，即加快速度变化
//     D值:使加速度接近0，即抑制速度变化
//==============================================================
void control_speed_pid(ParamSpeed* speed);

//==============================================================
//函数名称：control_find_mid_points
//函数返回：无
//参数说明：turn:方向控制模块参数的地址
//功能概要：根据解压后图像数据，找出图像中点、左右边缘，以及该行是否有效
//==============================================================
void control_find_mid_points(ParamTurn* turn);

//==============================================================
//函数名称：control_cal_avg_mid_point
//函数返回：无
//参数说明：turn:方向控制模块参数的地址
//功能概要：根据每行中点值，计算中点加权平均值
//==============================================================
void control_cal_avg_mid_point(ParamTurn* turn);

//==============================================================
//函数名称：control_turn_pid
//函数返回：无
//参数说明：turn:方向控制模块参数的地址
//功能概要：根据中点加权平均值，进行方向PID控制，
//         计算目标PWM值并重置PWM周期计数器，
//         自动记录上次中点误差
//备注: P值:使中点值接近目标中点值
//     D值:使中点误差变化接近0，即抑制方向变化
//==============================================================
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
