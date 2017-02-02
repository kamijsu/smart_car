//==========================================================================
//文件名称：encoder.c
//功能概要：编码器应用驱动程序源文件
//==========================================================================

#include "encoder.h"

//各编码器所用FTM模块号
static const uint8 encoder_ftm_mod_table[] =
{ ENCODER0_FTM_MOD, ENCODER1_FTM_MOD };
//各编码器方向
static const uint8 encoder_dir_table[] = { ENCODER0_DIR, ENCODER1_DIR };

//==========================================================================
//函数名称: encoder_init
//函数返回: 无
//参数说明: encoder:编码器号:
//                 ENCODERx，x为编码器号;
//功能概要: 初始化编码器，相应配置在encoder.h中
//==========================================================================
void encoder_init(uint8 encoder) {
	uint8 mod;	//FTM模块号

	//获取FTM模块号
	mod = encoder_ftm_mod_table[encoder];

	//初始化FTM模块计数器为正交解码模式
	ftm_init(mod, ENCODER_FTM_CLK_DIV, FTM_COUNTER_MODE_QD, 0);
	//初始化正交解码功能
	ftm_qd_init(mod, ENCODER_MODE, encoder_dir_table[encoder]);
}

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
int32 encoder_get_count(uint8 encoder) {
	return ftm_qd_get_count(encoder_ftm_mod_table[encoder])
			<< ENCODER_FTM_CLK_DIV;
}

//==========================================================================
//函数名称: encoder_clear_count
//函数返回: 无
//参数说明: encoder:编码器号:
//                 ENCODERx，x为编码器号;
//功能概要: 清除计数器正常计数个数
//备注: 用于测试编码器
//==========================================================================
void encoder_clear_count(uint8 encoder) {
	ftm_qd_clear_count(encoder_ftm_mod_table[encoder]);
}

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
int32 encoder_get_and_clear_count(uint8 encoder) {
	return ftm_qd_get_and_clear_count(encoder_ftm_mod_table[encoder])
			<< ENCODER_FTM_CLK_DIV;
}

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
float encoder_get_speed(uint8 encoder) {
	//获取速度，速度=(正常计数个数/计数器倍频数/编码器分辨率)*2π*齿轮半径/采集速度周期
	return encoder_get_and_clear_count(encoder) * ENCODER_GEAR_R * 6.28f
			/ (ENCODER_FTM_FREQ_MUL * ENCODER_RES * ENCODER_PERIOD);
}
