#include "breathe_light.h"

//======================================================================
//函数名称：breathe_light_init
//函数参数：breath_light：BREATHE_LIGHT_GREEN|BREATHE_LIGHT_RED
//       duty：占空比，0-100，值越大灯越亮
//函数返回：无
//功能概要：呼吸灯驱动初始化。
//======================================================================
void breathe_light_init(uint_16 breath_light, float duty)
{
	//初始化TPM模块
	tpm_timer_init(BREATHE_LIGHT_TPM, BREATHE_LIGHT_FREQ, BREATHE_LIGHT_CYCLE);
	//初始化PWM通道
	pwm_init(breath_light, duty, BREATHE_LIGHT_ALIGN, BREATHE_LIGHT_POL);
}

//======================================================================
//函数名称：breathe_light_set
//函数参数：breath_light：BREATHE_LIGHT_GREEN|BREATHE_LIGHT_RED
//       duty：占空比，0-100，值越大灯越亮
//函数返回：无
//功能概要：设置呼吸灯亮度。
//======================================================================
void breathe_light_set(uint_16 breath_light, float duty)
{
	pwm_update(breath_light, duty);
}

//======================================================================
//函数名称：breathe_light_get
//函数参数：breath_light：BREATHE_LIGHT_GREEN|BREATHE_LIGHT_RED
//函数返回：呼吸灯亮度
//功能概要：获取呼吸灯当前亮度。
//======================================================================
float breathe_light_get(uint_16 breath_light)
{
	return pwm_get(breath_light);
}
