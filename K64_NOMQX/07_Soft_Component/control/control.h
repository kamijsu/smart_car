//==========================================================================
//文件名称：control.h
//功能概要：小车控制软件程序头文件
//==========================================================================

#ifndef _CONTROL_H
#define _CONTROL_H

#include "common.h"
#include "ftm.h"
#include "encoder.h"
#include "camera.h"

//定义PID参数结构体
typedef struct {
	float p;	//比例控制
	float i;	//积分控制
	float d;	//微分控制
} ControlPID, *ControlPIDPtr;

//定义PWM参数结构体
typedef struct {
	int16 target_pwm;	//目标PWM值，需初始化
	int16 output_pwm;	//本次输出的PWM值，需初始化
	uint8 period_counter;	//周期计数器
	uint8 period_num;	//输出PWM值变为目标PWM值所需的周期数，至少为1，需初始化
} ControlPWM, *ControlPWMPtr;

//定义角度控制模块参数
typedef struct {
	float angle;		//当前角度，单位°，需初始化
	float angle_speed;	//当前角速度，单位°/s
	float last_angle_speed;	//上次角速度，单位°/s，需初始化
	float target_angle;	//目标角度，单位°，需初始化
	ControlPID pid;		//PID参数，需初始化
	ControlPWM pwm;		//PWM参数，需初始化
} ControlAngle, *ControlAnglePtr;

//定义速度控制模块参数
typedef struct {
	float left_speed;	//左轮速度，单位m/s
	float right_speed;	//右轮速度，单位m/s
	float target_speed;	//目标速度，单位m/s，需初始化
	float last_speed_err;	//上次速度差值，单位m/s，需初始化
	float distance_err;	//距离误差，单位m，需初始化
	ControlPID pid;		//PID参数，需初始化
	ControlPWM pwm;		//PWM参数，需初始化
} ControlSpeed, *ControlSpeedPtr;

//定义方向控制模块参数
typedef struct {
	uint8 raw_img[CAMERA_RAW_IMG_BYTES];	//原始图像
	uint8 img[CAMERA_IMG_HEIGHT][CAMERA_IMG_WIDTH];	//解压后图像数据
	int16 returnBuff[6];
	float midpoint;
	float last_mid_err;	//需初始化
	ControlPID pid;		//PID参数，需初始化
	ControlPWM pwm;		//PWM参数，需初始化
} ControlTurn, *ControlTurnPtr;

//定义小车参数
typedef struct {
	ControlAngle angle;	//角度控制参数
	ControlSpeed speed;	//速度控制参数
	ControlTurn turn;	//方向控制参数
	int16 left_motor_pwm;	//左电机输出PWM值
	int16 right_motor_pwm;	//右电机输出PWM值
} ControlCar, *ControlCarPtr;

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

//==============================================================
//函数名称：control_angle_pid
//函数返回：无
//参数说明：angle:角度控制模块参数的地址
//功能概要：根据当前角度和角速度，进行角度PID控制(未用I)，
//         计算目标PWM值并重置PWM周期计数器，自动记录上次角速度
//备注: P值:使角度接近目标角度，与PWM值正相关
//     D值:使角速度接近0，即抑制角度变化，与PWM值正相关
//==============================================================
void control_angle_pid(ControlAngle* angle);

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
void control_speed_pid(ControlSpeed* speed);

void control_turn_pid(ControlTurn* turn);

//===========================================================================
//函数名称：control_update_output_pwm
//函数返回：无
//参数说明：pwm:PWM参数的地址
//功能概要：更新输出的PWM值，这种方式可以使输出的PWM值平滑地变化至目标PWM值
//===========================================================================
void control_update_output_pwm(ControlPWM* pwm);

//===========================================================================
//函数名称：control_update_motor_pwm
//函数返回：无
//参数说明：car:小车参数的地址
//功能概要：更新并设置电机输出的PWM值
//===========================================================================
void control_update_motor_pwm(ControlCar* car);

#endif
