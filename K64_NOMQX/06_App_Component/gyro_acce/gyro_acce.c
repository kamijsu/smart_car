//==========================================================================
//�ļ����ƣ�gyro_acce.c
//���ܸ�Ҫ�������Ǽ��ٶȴ�����Ӧ����������Դ�ļ�
//==========================================================================

#include "gyro_acce.h"
#include <math.h>

//==========================================================================
//��������: gyro_acce_init
//��������: true: ��ʹ��У�Թ��ܣ���У�Թ������óɹ������߽�ֹУ�Թ���;
//         false:��ʹ��У�Թ��ܣ���У�Թ�������ʧ��;
//����˵��: ��
//���ܸ�Ҫ: ��ʼ�������Ǽ��ٶȴ���������Ӧ������gyro_acce.h��
//==========================================================================
bool gyro_acce_init() {
	return adc_init(GYRO_ACCE_ADC_MOD, GYRO_ACCE_ADC_CLK_DIV,
	GYRO_ACCE_ADC_ACCURACY,
	GYRO_ACCE_ADC_HARDWARE_AVG, GYRO_ACCE_ADC_ADLSTS,
	GYRO_ACCE_ADC_ADHSC, GYRO_ACCE_ADC_ENABLE_CAL);
}

//==========================================================================
//��������: gyro_acce_get_vtg
//��������: ��
//����˵��: gyro_vtg:�洢������X����ٶȵ�ѹֵ�ĵ�ַ����λmV��
//                  ��ǰ��ֵ��С��������ֵ����
//         acce_vtg:�洢���ٶȴ�����Z����ٶȵ�ѹֵ�ĵ�ַ����λmV��
//                  ��ǰ��ֵ����������ֵ��С
//���ܸ�Ҫ: ��ȡ�ɼ�����X����ٶȺ�Z����ٶȵĵ�ѹֵ
//��ע: ������������GYRO_ZERO_VTG��ACCE_ZERO_VTG
//==========================================================================
void gyro_acce_get_vtg(float* gyro_vtg, float* acce_vtg) {
	*gyro_vtg = adc_single_get_vtg(GYRO_ACCE_ADC_MOD, GYRO_ADC_SE,
	GYRO_ADC_SE_SEL);
	*acce_vtg = adc_single_get_vtg(GYRO_ACCE_ADC_MOD, ACCE_ADC_SE,
	ACCE_ADC_SE_SEL);
}

//==========================================================================
//��������: gyro_acce_get_raw_phy
//��������: ��
//����˵��: raw_angle_speed:�洢X����ٶȵĵ�ַ����λ��/s��
//                         ��ǰ��ֵ����������ֵ��С
//         raw_acce_speed:�洢Z����ٶȵĵ�ַ����λg��
//                        ��ǰ��ֵ����������ֵ��С
//���ܸ�Ҫ: ��ȡ�ɼ�����X����ٶȺ�Z����ٶ�
//==========================================================================
void gyro_acce_get_raw_phy(float* raw_angle_speed, float* raw_acce_speed) {
	float gyro_vtg, acce_vtg;	//X����ٶȺ�Z����ٶȵĵ�ѹֵ

	//��ȡX����ٶȺ�Z����ٶȵĵ�ѹֵ
	gyro_acce_get_vtg(&gyro_vtg, &acce_vtg);

	//����X����ٶ�
	*raw_angle_speed = (GYRO_ZERO_VTG - gyro_vtg) / (GYRO_SF * GYRO_MTP);
	//����Z����ٶ�
	*raw_acce_speed = (acce_vtg - ACCE_ZERO_VTG) / ACCE_RES;
}

//==========================================================================
//��������: gyro_acce_get_phy
//��������: ��
//����˵��: angle:�洢�Ƕȵĵ�ַ����λ�㣬��ǰ��ֵ����������ֵ��С
//         angle_speed:�洢���ٶȵĵ�ַ����λ��/s����ǰ��ֵ����������ֵ��С
//���ܸ�Ҫ: ��ȡ�������˲���ĽǶȺͽ��ٶ�
//��ע: �ú�����ÿ GYRO_ACCE_PERIOD s����һ�Σ������ȡ�ĽǶȺͽ��ٶȲ�׼ȷ;
//     angle����Ϊ��һ���˲���ĽǶȣ���ʼֵΪ0
//==========================================================================
void gyro_acce_get_phy(float* angle, float* angle_speed) {
	static const float dt = GYRO_ACCE_PERIOD;	//ʱ����Ϊ��������
	static const char C_0 = 1;
	static float P[2][2] = { { 1, 0 }, { 0, 1 } };
	static float Pdot[4] = { 0, 0, 0, 0 };
	static float q_bias = 0;

	float angle_err, PCt_0, PCt_1, EE, K_0, K_1, t_0, t_1;
	float raw_angle_speed, raw_acce_speed;		//X����ٶȺ�Z����ٶ�

	//��ȡX����ٶȺ�Z����ٶ�
	gyro_acce_get_raw_phy(&raw_angle_speed, &raw_acce_speed);

	//�������
	*angle += (raw_angle_speed - q_bias) * dt;

	//Pk-' ����������Э�����΢��
	Pdot[0] = GYRO_ACCE_Q_ANGLE - P[0][1] - P[1][0];
	Pdot[1] = -P[1][1];
	Pdot[2] = -P[1][1];
	Pdot[3] = GYRO_ACCE_Q_GYRO;

	//Pk- ����������Э����΢�ֵĻ��� = ����������Э����
	P[0][0] += Pdot[0] * dt;
	P[0][1] += Pdot[1] * dt;
	P[1][0] += Pdot[2] * dt;
	P[1][1] += Pdot[3] * dt;

	//zk-�������
	//��ֹԽ��
	if (raw_acce_speed > 1) {
		raw_acce_speed = 1;
	} else if (raw_acce_speed < -1) {
		raw_acce_speed = -1;
	}
	//����ǶȲ�ֵ
	angle_err = asinf(raw_acce_speed) * 57.295f - *angle;	// 180 / �� = 57.295

	PCt_0 = C_0 * P[0][0];
	PCt_1 = C_0 * P[1][0];

	EE = GYRO_ACCE_R_ANGLE + C_0 * PCt_0;

	K_0 = PCt_0 / EE;
	K_1 = PCt_1 / EE;

	t_0 = PCt_0;
	t_1 = C_0 * P[0][1];

	//����������Э����
	P[0][0] -= K_0 * t_0;
	P[0][1] -= K_0 * t_1;
	P[1][0] -= K_1 * t_0;
	P[1][1] -= K_1 * t_1;

	//�������
	*angle += K_0 * angle_err;
	q_bias += K_1 * angle_err;

	//���ֵ��������ƣ���΢�� = ���ٶ�
	*angle_speed = raw_angle_speed - q_bias;
}
