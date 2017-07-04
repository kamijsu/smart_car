//==========================================================================
//文件名称：param.h
//功能概要：小车参数软件程序头文件
//==========================================================================

#ifndef _PARAM_H
#define _PARAM_H

#include "common.h"
#include "camera.h"

//定义PID参数结构体
typedef struct {
	float p;	//比例控制
	float i;	//积分控制
	float d;	//微分控制
} ParamPID, *ParamPIDPtr;

//定义PWM参数结构体
typedef struct {
	int16 target_pwm;	//目标PWM值，需初始化
	int16 output_pwm;	//本次输出的PWM值，需初始化
	uint8 period_counter;	//周期计数器
	uint8 period_num;	//输出PWM值变为目标PWM值所需的周期数，至少为1，需初始化
} ParamPWM, *ParamPWMPtr;

//定义角度控制模块参数
typedef struct {
	float angle;		//当前角度，单位°，需初始化
	float angle_speed;	//当前角速度，单位°/s
	float last_angle_speed;	//上次角速度，单位°/s，需初始化
	float target_angle;	//目标角度，单位°，需初始化
	ParamPID pid;		//PID参数，需初始化
	ParamPWM pwm;		//PWM参数，需初始化
} ParamAngle, *ParamAnglePtr;

//定义速度控制模块参数
typedef struct {
	float left_speed;	//左轮速度，单位m/s
	float right_speed;	//右轮速度，单位m/s
	float target_speed;	//目标速度，单位m/s，需初始化
	float last_speed_err;	//上次速度差值，单位m/s，需初始化
	float distance_err;	//距离误差，单位m，需初始化
	ParamPID pid;		//PID参数，需初始化
	ParamPWM pwm;		//PWM参数，需初始化
} ParamSpeed, *ParamSpeedPtr;

//定义方向控制模块参数
typedef struct {
	uint8 raw_img[CAMERA_RAW_IMG_BYTES];	//原始图像
	uint8 img[CAMERA_IMG_HEIGHT][CAMERA_IMG_WIDTH];	//解压后图像数据
	int16 returnBuff[6];
	float midpoint;
	float last_mid_err;	//需初始化
	ParamPID pid;		//PID参数，需初始化
	ParamPWM pwm;		//PWM参数，需初始化
} ParamTurn, *ParamTurnPtr;

//定义小车参数
typedef struct {
	ParamAngle angle;	//角度控制参数
	ParamSpeed speed;	//速度控制参数
	ParamTurn turn;	//方向控制参数
	int16 left_motor_pwm;	//左电机输出PWM值
	int16 right_motor_pwm;	//右电机输出PWM值
} ParamCar, *ParamCarPtr;

#endif
