////==============================================================
////�ļ����ƣ�gyro_acce.c
////���ܸ�Ҫ�������ǡ����ٶȴ�����Դ�ļ�
////��Ȩ���У����ݴ�ѧ��˼����Ƕ��ʽ����(sumcu.suda.edu.cn)
////==============================================================
//
//#include "gyro_acce.h"
//
////==============================================================
////�������ƣ�gyro_acce_init
////�������أ���
////����˵������
////���ܸ�Ҫ�������ǡ����ٶȴ�������ʼ��������AD������ɶ������ǡ�����
////�ȴ�������MCU�������ŵĳ�ʼ�������������
////==============================================================
//void gyro_acce_init(void)
//{
//	adc_init(GYRO_ACCE_ADC);
//}
//
////==============================================================
////�������ƣ�gyro_acce_getAD
////�������أ���
////����˵����angle_speed_AD:���������X����ٶ�ADֵ��ָ�룬��ֵ��ǰ�������������С
////		 acce_speed_AD:��ż��ٶȴ�����Z����ٶ�ADֵ��ָ�룬����ǰ���С������������
////���ܸ�Ҫ����ȡ�����ǡ����ٶȴ�����AD���ݡ�����AD������ɶ������ǡ�
////���ٶȴ�������MCU�������ŵĵ�ƽ������
////==============================================================
//void gyro_acce_getAD(uint16_t *angle_speed_AD, uint16_t *acce_speed_AD)
//{
//	*angle_speed_AD = adc_ave(GYRO_ACCE_ADC, GYRO_CH, GYRO_ACCE_ACC,
//			GYRO_ACCE_SMP);    //PTB0		//X����ٶ�
//	*acce_speed_AD = adc_ave(GYRO_ACCE_ADC, ACCE_CH, GYRO_ACCE_ACC,
//			GYRO_ACCE_SMP);    //PTB1		//Z����ٶ�
//}
//
////==============================================================
////�������ƣ�gyro_acce_AD_to_phy
////�������أ���
////����˵����angle_speed_AD:������X����ٶ�ADֵ
////		 acce_speed_AD:���ٶȴ�����Z����ٶ�ADֵ
////       angle_speed:��������ǽ��ٶȵ�ָ�룬��λ��/s����ǰ��ֵ����������ֵ��С
////       acce_speed:��ż��ٶȴ��������ٶȵ�ָ�룬��λg����ǰ��ֵ����������ֵ��С
////���ܸ�Ҫ�������ǡ����ٶȴ������Ĳ�����ѹֵת�ɽ��ٶȡ����ٶȡ�
////==============================================================
//void gyro_acce_AD_to_phy(uint16_t angle_speed_AD, uint16_t acce_speed_AD,
//		float *angle_speed, float *acce_speed, uint16_t acce_set)
//{
//	//�����ǽ��ٶ�
//	*angle_speed = ((angle_speed_AD - GYRO_SET) * GYRO_ACCE_VTG
//			/ (1 << GYRO_ACCE_ACC)) / ((GYRO_SF / 1000) * GYRO_MTP);
//
//	//���ٶȴ�����
//	*acce_speed = -1 * ((acce_speed_AD - acce_set) * GYRO_ACCE_VTG
//							/ (1 << GYRO_ACCE_ACC)) / ACCE_RES;
//}
//
////==============================================================
////�������ƣ�gyro_acce_phy_to_angle
////�������أ���
////����˵���� angle_speed:���ٶȣ���λ��/s
////       acce_speed:���ٶȣ���λg
////       angle:ͨ���������˲�����õ����
////		 angle_speed_new:ͨ���������˲�����õĽ��ٶ�
////���ܸ�Ҫ�����ÿ������˲��ѽ��ٶȡ����ٶ�ת�ɳ�ģ��ǡ�
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
//	*angle += (angle_speed - q_bias) * dt;    //�������
//
//	// Pk-' ����������Э�����΢��
//	Pdot[0] = Q_ANGLE - P[0][1] - P[1][0];
//	Pdot[1] = -P[1][1];
//	Pdot[2] = -P[1][1];
//	Pdot[3] = Q_GYRO;
//
//	// Pk- ����������Э����΢�ֵĻ��� = ����������Э����
//	P[0][0] += Pdot[0] * dt;
//	P[0][1] += Pdot[1] * dt;
//	P[1][0] += Pdot[2] * dt;
//	P[1][1] += Pdot[3] * dt;
//
//	//zk-�������
//	if (acce_speed > 1)
//		angle_err = 90 - *angle;
//	else if (acce_speed < -1)
//		angle_err = -90 - *angle;
//	else
//		angle_err = asin(acce_speed) * 57.296 - *angle;	// 180 / �� = 57.296
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
//	P[0][0] -= K_0 * t_0;	//����������Э����
//	P[0][1] -= K_0 * t_1;
//	P[1][0] -= K_1 * t_0;
//	P[1][1] -= K_1 * t_1;
//
//	//�������
//	*angle += K_0 * angle_err;
//	q_bias += K_1 * angle_err;
//
//	//���ֵ��������ƣ���΢�� = ���ٶ�
//	*angle_speed_new = angle_speed - q_bias;
//}
