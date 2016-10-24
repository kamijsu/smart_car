//===========================================================================
//�ļ����ƣ�motor.h
//���ܸ�Ҫ��motor��������ͷ�ļ�
//��Ȩ���У����ݴ�ѧ��˼����Ƕ��ʽ����(sumcu.suda.edu.cn)
//��    ����V2.0, 20150522
//===========================================================================

#ifndef _MOTOR_H              //��ֹ�ظ����壨_MOTOR_H  ��ͷ)
#define _MOTOR_H

//����ͷ�ļ�
#include "common.h"
#include "pwm.h"

//��·PWM������
#define MOTOR1 (0)
#define MOTOR2 (1)

//���ģ���ͨ����
#define MOTOR1_PWM1			U_FTM0
#define MOTOR1_PWM1_CH		U_FTMCH0	// U_FTM0_CH0    //PTC1

#define MOTOR1_PWM2			U_FTM0
#define MOTOR1_PWM2_CH		U_FTMCH1	// U_FTM0_CH1    //PTC2

#define MOTOR2_PWM1			U_FTM0
#define MOTOR2_PWM1_CH		U_FTMCH2	// U_FTM0_CH2    //PTC3

#define MOTOR2_PWM2			U_FTM0
#define MOTOR2_PWM2_CH		U_FTMCH3	// U_FTM0_CH3    //PTC4

//���PWM����
#define MOTOR_PERIOD (1000) //���PWM���ڣ�����1ms��1000��ʾ1ms,��PWM.h��

//���ת��������
#define MOTOR_SPEED_MAX		(PWM_ACCURACY)
#define MOTOR_SPEED_MIN		(-PWM_ACCURACY)

//====================================================================
//�������ƣ�motor_init
//�������أ���
//����˵����motorNo:ѡ���� ��MOTOR1��MOTOR2
//���ܸ�Ҫ�������ʼ�������ݴ������ѡ��Ҫ��ʼ���ĵ��������MOTOR1����Ȼ����ݺ궨������ţ�����
//      MOTOR1_PWM1��MOTOR1_PWM2�������ڣ�MOTOR_PERIOD����ʼ���������ʼռ�ձ�Ϊ0
//====================================================================
void motor_init(uint8_t motorNo);

//====================================================================
//�������ƣ�motor_set;
//�������أ���
//����˵����  motorNo:ѡ���� ��MOTOR1��MOTOR2
//       duty��  ռ�ձȣ����Ƴ���ת��
//       ������ ��MOTOR_SPEED_MIN ~ MOTOR_SPEED_MAX(����ֵ��PWM.h�ж���)
//���ܸ�Ҫ�����õ���ٶȣ����ݵ��ѡ��motor�����Լ�ռ�ձ�duty�����ö�Ӧ�����PWMռ�ձ�
//====================================================================
void motor_set(uint8_t motorNo, int16_t duty);

#endif //��ֹ�ظ����壨_MOTOR_H ��β)
