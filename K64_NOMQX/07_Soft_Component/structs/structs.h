//==============================================================
//文件名称：structs.h
//功能概要：智能车自定义结构体头文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//==============================================================

#ifndef _STRUCTS_H
#define _STRUCTS_H

#include "common.h"

//PID参数
typedef struct {
	float p;	//比例控制
	float i;	//积分控制
	float d;	//微分控制
} CarPID, *CarPIDPtr;

//PWM参数
typedef struct {
	int16_t new_PWM;	//新计算出的PWM值
	int16_t last_PWM;	//上次计算出的PWM值
	int16_t output_PWM;	//输出电机的PWM值
} CarPWM, *CarPWMPtr;

//小车标志参数
typedef struct {
	uint8_t reed_switch;	//干簧管是否接通
	uint8_t over_speed;		//速度是否过快
	uint8_t is_starting;	//是否正在起步
	uint8_t can_stop;		//是否可以停车
} CarFlag, *CarFlagPtr;

//时间标志参数
typedef struct {
	uint8_t f_5ms;
	uint8_t f_10ms;
	uint8_t f_15ms;
	uint8_t f_20ms;
	volatile uint8_t f_50ms;
	uint8_t f_100ms;
	volatile uint8_t f_1s;
	uint8_t f_5s;
	uint8_t f_10s;
	uint8_t f_15s;
	uint8_t f_30s;
	uint8_t f_1min;
} TimeFlag, *TimeFlagPtr;

//时间计数器参数
typedef struct {
	uint8_t c_5ms;
	uint8_t c_10ms;
	uint8_t c_15ms;
	uint8_t c_20ms;
	uint8_t c_50ms;
	uint8_t c_100ms;
	uint8_t c_1s;
	uint16_t c_5s;
	uint16_t c_10s;
	uint16_t c_15s;
	uint16_t c_30s;
	uint16_t c_1min;
} TimeCounter, *TimeCounterPtr;

//编码器脉冲计数器
typedef struct {
	uint16_t left;		//左轮编码器计数器
	uint16_t right;		//右轮编码器计数器
} EncoderCounter, *EncoderCounterPtr;

//角度控制模块参数
typedef struct {
	uint16_t angle_speed_AD;	//陀螺仪X轴角速度AD值
	uint16_t acce_speed_AD;		//加速度传感器Z轴加速度AD值
	uint16_t acce_set;			//平衡时加速度传感器Z轴加速度的AD值
	float angle_speed_temp;		//角速度临时量，单位°/s
	float acce_speed_temp;		//加速度临时量，单位g
	float angle;				//小车倾斜角度，单位°
	float angle_speed;			//小车角速度，单位°/s
	CarPID pid;					//角度控制PID参数
	CarPWM pwm;					//角度控制PWM参数
	uint8_t period_counter;		//角度控制周期计数器
} CarAngle, *CarAnglePtr;

//速度控制模块参数
typedef struct {
	float left_speed;			//小车左轮速度，单位m/s
	float right_speed;			//小车右轮速度，单位m/s
	float avg_speed;			//小车平均速度，单位m/s
	float aim_speed;			//小车目标速度，单位m/s
	CarPID pid;					//速度控制PID参数
	CarPWM pwm;					//速度控制PWM参数
	uint8_t period_counter;		//速度控制周期计数器
} CarSpeed, *CarSpeedPtr;

//转向控制模块参数
typedef struct {
	float EMS_V[4];				//电磁传感器电压值的数组，单位V
	CarPID pid;					//转向控制PID参数
	CarPWM pwm;					//转向控制PWM参数
	uint8_t period_counter;		//转向控制周期计数器
} CarTurn, *CarTurnPtr;

//小车相关参数
typedef struct {
	CarAngle angle;		//角度控制参数
	CarSpeed speed;		//速度控制参数
	CarTurn turn;		//转向控制参数
	int16_t output_PWM;	//对电机输出的复合的PWM值
} Car, *CarPtr;

#endif /* _STRUCTS_H */
