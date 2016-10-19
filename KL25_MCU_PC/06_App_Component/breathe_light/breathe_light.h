#ifndef _BREATHE_LIGHT_H
#define _BREATHE_LIGHT_H

#include "common.h"
#include "tpm.h"

//定义呼吸灯所用TPM模块号
#define BREATHE_LIGHT_TPM		TPM_2

//定义呼吸灯所用TPM模块的计数器频率
#define BREATHE_LIGHT_FREQ		(3000)

//定义呼吸灯所用TPM模块的计数周期，单位为ms
//（因为不开中断，频率与周期参数没有太大含义，但由于TPM模块的模数寄存器MOD仅有16位，所以频率与周期乘积不能超过65535）
#define BREATHE_LIGHT_CYCLE		(20)

//定义呼吸灯PWM通道对其方式
#define BREATHE_LIGHT_ALIGN		PWM_CENTER

//定义呼吸灯PWM通道极性（这里用负极性，所以对应的占空比越低，灯越暗）
#define BREATHE_LIGHT_POL		PWM_MINUS

//定义呼吸灯占空比的最大值与最小值
#define BREATHE_LIGHT_MAX_DUTY	PWM_MAX
#define BREATHE_LIGHT_MIN_DUTY	PWM_MIN

//定义每盏呼吸灯所用通道号
#define BREATHE_LIGHT_GREEN		TPM2_CH0
#define BREATHE_LIGHT_RED		TPM2_CH1

//======================================================================
//函数名称：breathe_light_init
//函数参数：breath_light：BREATHE_LIGHT_GREEN|BREATHE_LIGHT_RED
//       duty：占空比，0-100，值越大灯越亮
//函数返回：无
//功能概要：呼吸灯驱动初始化。
//======================================================================
void breathe_light_init(uint_16 breath_light, float duty);

//======================================================================
//函数名称：breathe_light_set
//函数参数：breath_light：BREATHE_LIGHT_GREEN|BREATHE_LIGHT_RED
//       duty：占空比，0-100，值越大灯越亮
//函数返回：无
//功能概要：设置呼吸灯亮度。
//======================================================================
void breathe_light_set(uint_16 breath_light, float duty);

//======================================================================
//函数名称：breathe_light_get
//函数参数：breath_light：BREATHE_LIGHT_GREEN|BREATHE_LIGHT_RED
//函数返回：呼吸灯亮度
//功能概要：获取呼吸灯当前亮度。
//======================================================================
float breathe_light_get(uint_16 breath_light);

#endif
