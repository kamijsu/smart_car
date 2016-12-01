//==========================================================================
//文件名称：encoder.h
//功能概要：编码器应用驱动程序头文件
//==========================================================================

#ifndef _ENCODER_H
#define _ENCODER_H

#include "common.h"
#include "ftm.h"

//定义编码器号
#define ENCODER0	(0)
#define ENCODER1	(1)

//定义各编码器所使用的FTM模块，相应的引脚由ftm.h中的FTM_MODx_QD_SETUP决定
#define ENCODER0_FTM_MOD	FTM_MOD1	//编码器0，FTM1，PHA:B0，PHB:B1
#define ENCODER1_FTM_MOD	FTM_MOD2	//编码器1，FTM2，PHA:B18，PHB:B19

//定义编码器模式
#define ENCODER_MODE	FTM_QD_MODE_PHAB	//AB相编码器

//定义各编码器的方向，当方向为正方向时，顺时针旋转为正速度，当方向为负方向时，逆时针旋转为正速度
#define ENCODER0_DIR	FTM_QD_DIR_POSITIVE		//正方向
#define ENCODER1_DIR	FTM_QD_DIR_NEGATIVE		//负方向

//定义编码器所用FTM模块的时钟分频，(计数器计数个数*x)为正常计数个数，x为FTM_CLK_DIV_x的x
#define ENCODER_FTM_CLK_DIV		FTM_CLK_DIV_1

//定义编码器采集速度的周期，单位ms
#define ENCODER_PERIOD	(50)	//50ms

//定义编码器分辨率，500表示500个脉冲每转
#define ENCODER_RES		(500)	//500个脉冲每转

//定义编码器齿轮半径，单位mm
#define ENCODER_GEAR_R	(8)		//8mm

//根据编码器模式定义FTM模块计数器的倍频数
#ifdef ENCODER_MODE
#if(ENCODER_MODE == FTM_QD_MODE_PHAB)
#define ENCODER_FTM_FREQ_MUL	(4)
#elif(ENCODER_MODE == FTM_QD_MODE_COUNT_DIRECTION)
#define ENCODER_FTM_FREQ_MUL	(1)
#endif
#endif

//==========================================================================
//函数名称: encoder_init
//函数返回: 无
//参数说明: encoder:编码器号:
//                 ENCODERx，x为编码器号;
//功能概要: 初始化编码器，相应配置在encoder.h中
//==========================================================================
void encoder_init(uint8 encoder);

//==========================================================================
//函数名称: encoder_get_count
//函数返回: 计数器正常计数个数
//参数说明: encoder:编码器号:
//                 ENCODERx，x为编码器号;
//功能概要: 获取计数器正常计数个数，相应配置在encoder.h中
//备注: 正常计数个数带方向，正负分别代表不同方向，具体定义与设置有关;
//     若为AB相编码器，则编码器旋转一圈时，正常计数个数为4*编码器分辨率;
//     若为方向-脉冲型编码器，则编码器旋转一圈时，正常计数个数为编码器分辨率;
//     用于测试编码器
//==========================================================================
int32 encoder_get_count(uint8 encoder);

//==========================================================================
//函数名称: encoder_clear_count
//函数返回: 无
//参数说明: encoder:编码器号:
//                 ENCODERx，x为编码器号;
//功能概要: 清除计数器正常计数个数
//备注: 用于测试编码器
//==========================================================================
void encoder_clear_count(uint8 encoder);

//==========================================================================
//函数名称: encoder_get_and_clear_count
//函数返回: 计数器正常计数个数
//参数说明: encoder:编码器号:
//                 ENCODERx，x为编码器号;
//功能概要: 获取并清除计数器正常计数个数，相应配置在encoder.h中
//备注: 正常计数个数带方向，正负分别代表不同方向，具体定义与设置有关;
//     若为AB相编码器，则编码器旋转一圈时，正常计数个数为4*编码器分辨率;
//     若为方向-脉冲型编码器，则编码器旋转一圈时，正常计数个数为编码器分辨率;
//     用于测试编码器
//==========================================================================
int32 encoder_get_and_clear_count(uint8 encoder);

//==========================================================================
//函数名称: encoder_get_speed
//函数返回: 当前速度，单位m/s
//参数说明: encoder:编码器号:
//                 ENCODERx，x为编码器号;
//功能概要: 获取编码器对应轮胎的当前速度，单位m/s，相应配置在encoder.h中
//备注: 该函数需每 ENCODER_PERIOD ms调用一次，否则获取的速度不准确;
//     速度带方向，正负分别代表不同方向，具体定义与设置有关;
//     已清除计数器计数个数
//==========================================================================
float encoder_get_speed(uint8 encoder);

#endif

