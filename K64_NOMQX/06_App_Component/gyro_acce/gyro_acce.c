////==============================================================
////文件名称：gyro_acce.c
////功能概要：陀螺仪、加速度传感器源文件
////版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
////==============================================================
//
//#include "gyro_acce.h"
//
////==============================================================
////函数名称：gyro_acce_init
////函数返回：无
////参数说明：无
////功能概要：陀螺仪、加速度传感器初始化。调用AD构件完成对陀螺仪、加速
////度传感器的MCU采样引脚的初始化，无需参数。
////==============================================================
//void gyro_acce_init(void)
//{
//	adc_init(GYRO_ACCE_ADC);
//}
//
////==============================================================
////函数名称：gyro_acce_getAD
////函数返回：无
////参数说明：angle_speed_AD:存放陀螺仪X轴角速度AD值的指针，该值往前倾增大，往后倾减小
////		 acce_speed_AD:存放加速度传感器Z轴加速度AD值的指针，该往前倾减小，往后倾增大
////功能概要：获取陀螺仪、加速度传感器AD数据。调用AD构件完成对陀螺仪、
////加速度传感器的MCU采样引脚的电平采样。
////==============================================================
//void gyro_acce_getAD(uint16_t *angle_speed_AD, uint16_t *acce_speed_AD)
//{
//	*angle_speed_AD = adc_ave(GYRO_ACCE_ADC, GYRO_CH, GYRO_ACCE_ACC,
//			GYRO_ACCE_SMP);    //PTB0		//X轴角速度
//	*acce_speed_AD = adc_ave(GYRO_ACCE_ADC, ACCE_CH, GYRO_ACCE_ACC,
//			GYRO_ACCE_SMP);    //PTB1		//Z轴加速度
//}
//
////==============================================================
////函数名称：gyro_acce_AD_to_phy
////函数返回：无
////参数说明：angle_speed_AD:陀螺仪X轴角速度AD值
////		 acce_speed_AD:加速度传感器Z轴加速度AD值
////       angle_speed:存放陀螺仪角速度的指针，单位°/s，往前倾值增大，往后倾值减小
////       acce_speed:存放加速度传感器加速度的指针，单位g，往前倾值增大，往后倾值减小
////功能概要：陀螺仪、加速度传感器的采样电压值转成角速度、加速度。
////==============================================================
//void gyro_acce_AD_to_phy(uint16_t angle_speed_AD, uint16_t acce_speed_AD,
//		float *angle_speed, float *acce_speed, uint16_t acce_set)
//{
//	//陀螺仪角速度
//	*angle_speed = ((angle_speed_AD - GYRO_SET) * GYRO_ACCE_VTG
//			/ (1 << GYRO_ACCE_ACC)) / ((GYRO_SF / 1000) * GYRO_MTP);
//
//	//加速度传感器
//	*acce_speed = -1 * ((acce_speed_AD - acce_set) * GYRO_ACCE_VTG
//							/ (1 << GYRO_ACCE_ACC)) / ACCE_RES;
//}
//
////==============================================================
////函数名称：gyro_acce_phy_to_angle
////函数返回：无
////参数说明： angle_speed:角速度，单位°/s
////       acce_speed:加速度，单位g
////       angle:通过卡尔曼滤波所获得的倾角
////		 angle_speed_new:通过卡尔曼滤波所获得的角速度
////功能概要：利用卡尔曼滤波把角速度、加速度转成车模倾角。
////==============================================================
//void gyro_acce_phy_to_angle(float angle_speed, float acce_speed, float *angle,
//		float *angle_speed_new)
//{
//	float dt = (GYRO_ACCE_PERIOD * 1.0) / 1000;
//	static float P[2][2] = { { 1, 0 }, { 0, 1 } };
//	static float Pdot[4] = { 0, 0, 0, 0 };
//	static const char C_0 = 1;
//	static float q_bias, angle_err, PCt_0, PCt_1, EE, K_0, K_1, t_0, t_1;
//
//	*angle += (angle_speed - q_bias) * dt;    //先验估计
//
//	// Pk-' 先验估计误差协方差的微分
//	Pdot[0] = Q_ANGLE - P[0][1] - P[1][0];
//	Pdot[1] = -P[1][1];
//	Pdot[2] = -P[1][1];
//	Pdot[3] = Q_GYRO;
//
//	// Pk- 先验估计误差协方差微分的积分 = 先验估计误差协方差
//	P[0][0] += Pdot[0] * dt;
//	P[0][1] += Pdot[1] * dt;
//	P[1][0] += Pdot[2] * dt;
//	P[1][1] += Pdot[3] * dt;
//
//	//zk-先验估计
//	if (acce_speed > 1)
//		angle_err = 90 - *angle;
//	else if (acce_speed < -1)
//		angle_err = -90 - *angle;
//	else
//		angle_err = asin(acce_speed) * 57.296 - *angle;	// 180 / π = 57.296
//
//	PCt_0 = C_0 * P[0][0];
//	PCt_1 = C_0 * P[1][0];
//
//	EE = R_ANGLE + C_0 * PCt_0;
//
//	K_0 = PCt_0 / EE;
//	K_1 = PCt_1 / EE;
//
//	t_0 = PCt_0;
//	t_1 = C_0 * P[0][1];
//
//	P[0][0] -= K_0 * t_0;	//后验估计误差协方差
//	P[0][1] -= K_0 * t_1;
//	P[1][0] -= K_1 * t_0;
//	P[1][1] -= K_1 * t_1;
//
//	//后验估计
//	*angle += K_0 * angle_err;
//	q_bias += K_1 * angle_err;
//
//	//输出值（后验估计）的微分 = 角速度
//	*angle_speed_new = angle_speed - q_bias;
//}
