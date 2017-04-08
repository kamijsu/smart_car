//==========================================================================
//文件名称：gyro_acce.c
//功能概要：陀螺仪加速度传感器应用驱动程序源文件
//==========================================================================

#include "gyro_acce.h"
#include <math.h>

//==========================================================================
//函数名称: gyro_acce_init
//函数返回: true: 若使能校对功能，且校对功能配置成功，或者禁止校对功能;
//         false:若使能校对功能，且校对功能配置失败;
//参数说明: 无
//功能概要: 初始化陀螺仪加速度传感器，相应配置在gyro_acce.h中
//==========================================================================
bool gyro_acce_init() {
	return adc_init(GYRO_ACCE_ADC_MOD, GYRO_ACCE_ADC_CLK_DIV,
	GYRO_ACCE_ADC_ACCURACY,
	GYRO_ACCE_ADC_HARDWARE_AVG, GYRO_ACCE_ADC_ADLSTS,
	GYRO_ACCE_ADC_ADHSC, GYRO_ACCE_ADC_ENABLE_CAL);
}

//==========================================================================
//函数名称: gyro_acce_get_vtg
//函数返回: 无
//参数说明: gyro_vtg:存储陀螺仪X轴角速度电压值的地址，单位mV，
//                  往前倾值减小，往后倾值增大
//         acce_vtg:存储加速度传感器Z轴加速度电压值的地址，单位mV，
//                  往前倾值增大，往后倾值减小
//功能概要: 获取采集到的X轴角速度和Z轴加速度的电压值
//备注: 可以用于设置GYRO_ZERO_VTG和ACCE_ZERO_VTG
//==========================================================================
void gyro_acce_get_vtg(float* gyro_vtg, float* acce_vtg) {
	*gyro_vtg = adc_single_get_vtg(GYRO_ACCE_ADC_MOD, GYRO_ADC_SE,
	GYRO_ADC_SE_SEL);
	*acce_vtg = adc_single_get_vtg(GYRO_ACCE_ADC_MOD, ACCE_ADC_SE,
	ACCE_ADC_SE_SEL);
}

//==========================================================================
//函数名称: gyro_acce_get_raw_phy
//函数返回: 无
//参数说明: raw_angle_speed:存储X轴角速度的地址，单位°/s，
//                         往前倾值增大，往后倾值减小
//         raw_acce_speed:存储Z轴加速度的地址，单位g，
//                        往前倾值增大，往后倾值减小
//功能概要: 获取采集到的X轴角速度和Z轴加速度
//==========================================================================
void gyro_acce_get_raw_phy(float* raw_angle_speed, float* raw_acce_speed) {
	float gyro_vtg, acce_vtg;	//X轴角速度和Z轴加速度的电压值

	//获取X轴角速度和Z轴加速度的电压值
	gyro_acce_get_vtg(&gyro_vtg, &acce_vtg);

	//计算X轴角速度
	*raw_angle_speed = (GYRO_ZERO_VTG - gyro_vtg) / (GYRO_SF * GYRO_MTP);
	//计算Z轴加速度
	*raw_acce_speed = (acce_vtg - ACCE_ZERO_VTG) / ACCE_RES;
}

//==========================================================================
//函数名称: gyro_acce_get_phy
//函数返回: 无
//参数说明: angle:存储角度的地址，单位°，往前倾值增大，往后倾值减小
//         angle_speed:存储角速度的地址，单位°/s，往前倾值增大，往后倾值减小
//功能概要: 获取卡尔曼滤波后的角度和角速度
//备注: 该函数需每 GYRO_ACCE_PERIOD s调用一次，否则获取的角度和角速度不准确;
//     angle参数为上一次滤波后的角度，初始值为0
//==========================================================================
void gyro_acce_get_phy(float* angle, float* angle_speed) {
	static const float dt = GYRO_ACCE_PERIOD;	//时间间隔为采样周期
	static const char C_0 = 1;
	static float P[2][2] = { { 1, 0 }, { 0, 1 } };
	static float Pdot[4] = { 0, 0, 0, 0 };
	static float q_bias = 0;

	float angle_err, PCt_0, PCt_1, EE, K_0, K_1, t_0, t_1;
	float raw_angle_speed, raw_acce_speed;		//X轴角速度和Z轴加速度

	//获取X轴角速度和Z轴加速度
	gyro_acce_get_raw_phy(&raw_angle_speed, &raw_acce_speed);

	//先验估计
	*angle += (raw_angle_speed - q_bias) * dt;

	//Pk-' 先验估计误差协方差的微分
	Pdot[0] = GYRO_ACCE_Q_ANGLE - P[0][1] - P[1][0];
	Pdot[1] = -P[1][1];
	Pdot[2] = -P[1][1];
	Pdot[3] = GYRO_ACCE_Q_GYRO;

	//Pk- 先验估计误差协方差微分的积分 = 先验估计误差协方差
	P[0][0] += Pdot[0] * dt;
	P[0][1] += Pdot[1] * dt;
	P[1][0] += Pdot[2] * dt;
	P[1][1] += Pdot[3] * dt;

	//zk-先验估计
	//防止越界
	if (raw_acce_speed > 1) {
		raw_acce_speed = 1;
	} else if (raw_acce_speed < -1) {
		raw_acce_speed = -1;
	}
	//计算角度差值
	angle_err = asinf(raw_acce_speed) * 57.295f - *angle;	// 180 / π = 57.295

	PCt_0 = C_0 * P[0][0];
	PCt_1 = C_0 * P[1][0];

	EE = GYRO_ACCE_R_ANGLE + C_0 * PCt_0;

	K_0 = PCt_0 / EE;
	K_1 = PCt_1 / EE;

	t_0 = PCt_0;
	t_1 = C_0 * P[0][1];

	//后验估计误差协方差
	P[0][0] -= K_0 * t_0;
	P[0][1] -= K_0 * t_1;
	P[1][0] -= K_1 * t_0;
	P[1][1] -= K_1 * t_1;

	//后验估计
	*angle += K_0 * angle_err;
	q_bias += K_1 * angle_err;

	//输出值（后验估计）的微分 = 角速度
	*angle_speed = raw_angle_speed - q_bias;
}
