//==========================================================================
//�ļ����ƣ�motor.c
//���ܸ�Ҫ�����Ӧ����������Դ�ļ�
//==========================================================================

#include "motor.h"

//�����PWM0ͨ����
static const uint8 motor_pwm0_table[] = { MOTOR0_PWM0_CH, MOTOR1_PWM0_CH };
//�����PWM1ͨ����
static const uint8 motor_pwm1_table[] = { MOTOR0_PWM1_CH, MOTOR1_PWM1_CH };

//==========================================================================
//��������: motor_init
//��������: ��
//����˵��: motor:�����:
//               MOTORx��xΪ�����;
//���ܸ�Ҫ: ��ʼ���������ʼռ�ձ�Ϊ0����Ӧ������motor.h��
//==========================================================================
void motor_init(uint8 motor) {
	//��ʼ��FTMģ��
	ftm_init(MOTOR_FTM_MOD, MOTOR_FTM_CLK_DIV, MOTOR_FTM_COUNTER_MODE,
	MOTOR_FTM_COUNTER_PERIOD);
	//��ʼ��PWM0��PWM1
	ftm_pwm_single_init(MOTOR_FTM_MOD, motor_pwm0_table[motor],
	MOTOR_FTM_PWM_MODE,
	MOTOR_FTM_PWM_POL, 0);
	ftm_pwm_single_init(MOTOR_FTM_MOD, motor_pwm1_table[motor],
	MOTOR_FTM_PWM_MODE,
	MOTOR_FTM_PWM_POL, 0);
}

//==========================================================================
//��������: motor_set
//��������: ��
//����˵��: motor:�����:
//               MOTORx��xΪ�����;
//         duty:ռ�ձȣ���Χ[MOTOR_DUTY_MIN(-10000),MOTOR_DUTY_MAX(10000)]
//���ܸ�Ҫ: ���õ����ռ�ձȣ�������һ���������ڸ���
//��ע: ��duty>=0ʱ��PWM0ռ�ձ�Ϊ|duty|��PWM1ռ�ձ�Ϊ0��
//     ��duty<0ʱ��PWM0ռ�ձ�Ϊ0��PWM1ռ�ձ�Ϊ|duty|��
//     ռ�ձ������������޷�
//==========================================================================
void motor_set(uint8 motor, int16 duty) {
	uint16 duty_set;	//��Ҫ���õ�ռ�ձ�

	if (duty >= 0) {
		//�����޷�
		duty_set = duty > MOTOR_DUTY_MAX ? MOTOR_DUTY_MAX : duty;

		//����PWM0��PWM1
		ftm_pwm_single_set(MOTOR_FTM_MOD, motor_pwm0_table[motor], duty_set);
		ftm_pwm_single_set(MOTOR_FTM_MOD, motor_pwm1_table[motor], 0);
	} else {
		//�����޷�
		duty_set = duty < MOTOR_DUTY_MIN ? -MOTOR_DUTY_MIN : -duty;

		//����PWM0��PWM1
		ftm_pwm_single_set(MOTOR_FTM_MOD, motor_pwm0_table[motor], 0);
		ftm_pwm_single_set(MOTOR_FTM_MOD, motor_pwm1_table[motor], duty_set);
	}
}

