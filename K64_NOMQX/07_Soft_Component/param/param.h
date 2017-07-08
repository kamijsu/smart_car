//==========================================================================
//文件名称：param.h
//功能概要：小车参数软件程序头文件
//==========================================================================

#ifndef _PARAM_H
#define _PARAM_H

#include "common.h"
#include "camera.h"

#define PARAM_FLASH_SECTOR (20) //flash中存放参数的扇区号
#define PARAM_FLASH_OFFSET (0)  //flash中存放参数的扇区内偏移地址
#define PARAM_FLASH_NUM	   (40) //flash中参数的字节数

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
	int16 mid_points[CAMERA_IMG_HEIGHT];
	int16 left_edges[CAMERA_IMG_HEIGHT];
	int16 right_edges[CAMERA_IMG_HEIGHT];
	bool valid_row[CAMERA_IMG_HEIGHT];
	float avg_mid_point;
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

//==========================================================================
//函数名称: param_init
//函数返回: 无
//参数说明: param：存储参数值的首地址
//功能概要: 参数初始化，初始化不需要从flash中读的参数
//==========================================================================
void param_init(ParamCarPtr param);

//==========================================================================
//函数名称: param_get
//函数返回: 无
//参数说明: param：存储参数值的首地址
//         mode：选择第mode套参数
//功能概要: 从flash中读取所有参数值，第PARAM_FLASH_SECTOR + mode号扇区的
//         偏移PARAM_FLASH_OFFSET开始，存放第mode套参数，
//         每套参数长度PARAM_FLASH_NUM字节
//==========================================================================
void param_get(ParamCarPtr param, uint8 mode);

//==========================================================================
//函数名称: param_update
//函数返回: 无
//参数说明: param：存储参数值的首地址
//         mode：选择第mode套参数
//功能概要: 更新参数值，将缓冲区中修改过的参数保存到flash中
//         第PARAM_FLASH_SECTOR + mode号扇区的
//         偏移PARAM_FLASH_OFFSET开始，存放第mode套参数，
//         每套参数长度PARAM_FLASH_NUM字节
//==========================================================================
void param_update(ParamCarPtr param, uint8 mode);

#endif
