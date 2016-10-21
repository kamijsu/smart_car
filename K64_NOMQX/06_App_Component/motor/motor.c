//===========================================================================
//�ļ����ƣ�motor.c
//���ܸ�Ҫ��motor��������Դ�ļ�
//��Ȩ���У����ݴ�ѧ��˼����Ƕ��ʽ����(sumcu.suda.edu.cn)
//��    ����V2.0, 20150522
//===========================================================================

//====================================================================
//Ӳ������˵����
//==================================================================
#include "motor.h"

//====================================================================
//�������ƣ�motor_init
//�������أ���
//����˵����motorNo:ѡ���� ��MOTOR1��MOTOR2
//���ܸ�Ҫ�������ʼ�������ݴ������ѡ��Ҫ��ʼ���ĵ��������MOTOR1����Ȼ����ݺ궨������ţ�����
//      MOTOR1_PWM1��MOTOR1_PWM2�������ڣ�MOTOR_PERIOD����ʼ���������ʼռ�ձ�Ϊ0
//====================================================================
void motor_init(uint8_t motorNo)
{
	//���ݴ������motor������ѡ����1���ǵ��2
	if (motorNo == MOTOR1)  //��ʼ�����1
	{
		pwm_init(MOTOR1_PWM1, MOTOR1_PWM1_CH, MOTOR_PERIOD, 0);
		pwm_init(MOTOR1_PWM2, MOTOR1_PWM2_CH, MOTOR_PERIOD, 0);
	}
	else if (motorNo == MOTOR2)  //��ʼ�����2
	{
		pwm_init(MOTOR2_PWM1, MOTOR2_PWM1_CH, MOTOR_PERIOD, 0);
		pwm_init(MOTOR2_PWM2, MOTOR2_PWM2_CH, MOTOR_PERIOD, 0);
	}

}

//====================================================================
//�������ƣ�motor_set;
//�������أ���
//����˵����  motorNo:ѡ���� ��MOTOR1��MOTOR2
//       duty��  ռ�ձȣ����Ƴ���ת��
//       ������ ��MOTOR_SPEED_MIN ~ MOTOR_SPEED_MAX(����ֵ��PWM.h�ж���)
//���ܸ�Ҫ�����õ���ٶȣ����ݵ��ѡ��motor�����Լ�ռ�ձ�duty�����ö�Ӧ�����PWMռ�ձ�
//====================================================================
void motor_set(uint8_t motorNo, int16_t duty)
{
	//���ݴ������motor���������õ��1���ǵ��2
	if (motorNo == MOTOR1)  //���õ��1
	{
		//���ݴ������duty�����������ת���Ƿ�ת
		if (duty >= 0 && duty <= MOTOR_SPEED_MAX) //�����ת
		{
			pwm_set(MOTOR1_PWM1, MOTOR1_PWM1_CH, MOTOR_PERIOD, duty);
			pwm_set(MOTOR1_PWM2, MOTOR1_PWM2_CH, MOTOR_PERIOD, 0);
		}
		else if (duty > MOTOR_SPEED_MAX)
		{
			pwm_set(MOTOR1_PWM1, MOTOR1_PWM1_CH, MOTOR_PERIOD, MOTOR_SPEED_MAX);
			pwm_set(MOTOR1_PWM2, MOTOR1_PWM2_CH, MOTOR_PERIOD, 0);
		}
		else if (duty < 0 && duty >= MOTOR_SPEED_MIN) //�����ת
		{
			pwm_set(MOTOR1_PWM1, MOTOR1_PWM1_CH, MOTOR_PERIOD, 0);
			pwm_set(MOTOR1_PWM2, MOTOR1_PWM2_CH, MOTOR_PERIOD, -duty);
		}
		else
		{
			pwm_set(MOTOR1_PWM1, MOTOR1_PWM1_CH, MOTOR_PERIOD, 0);
			pwm_set(MOTOR1_PWM2, MOTOR1_PWM2_CH, MOTOR_PERIOD,
					-MOTOR_SPEED_MIN);
		}
	}
	else if (motorNo == MOTOR2) //���õ��2
	{
		if (duty >= 0 && duty <= MOTOR_SPEED_MAX) //�����ת
		{
			pwm_set(MOTOR2_PWM1, MOTOR2_PWM1_CH, MOTOR_PERIOD, duty);
			pwm_set(MOTOR2_PWM2, MOTOR2_PWM2_CH, MOTOR_PERIOD, 0);
		}
		else if (duty > MOTOR_SPEED_MAX)
		{
			pwm_set(MOTOR2_PWM1, MOTOR2_PWM1_CH, MOTOR_PERIOD, MOTOR_SPEED_MAX);
			pwm_set(MOTOR2_PWM2, MOTOR2_PWM2_CH, MOTOR_PERIOD, 0);
		}
		else if (duty < 0 && duty >= MOTOR_SPEED_MIN) //�����ת
		{
			pwm_set(MOTOR2_PWM1, MOTOR2_PWM1_CH, MOTOR_PERIOD, 0);
			pwm_set(MOTOR2_PWM2, MOTOR2_PWM2_CH, MOTOR_PERIOD, -duty);
		}
		else
		{
			pwm_set(MOTOR2_PWM1, MOTOR2_PWM1_CH, MOTOR_PERIOD, 0);
			pwm_set(MOTOR2_PWM2, MOTOR2_PWM2_CH, MOTOR_PERIOD,
					-MOTOR_SPEED_MIN);
		}
	}
}

