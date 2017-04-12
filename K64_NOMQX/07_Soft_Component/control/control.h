//==========================================================================
//�ļ����ƣ�control.h
//���ܸ�Ҫ��С�������������ͷ�ļ�
//==========================================================================

#ifndef _CONTROL_H
#define _CONTROL_H

#include "common.h"
#include "ftm.h"
#include "encoder.h"

//����PID�����ṹ��
typedef struct {
	float p;	//��������
	float i;	//���ֿ���
	float d;	//΢�ֿ���
} ControlPID, *ControlPIDPtr;

//����PWM�����ṹ��
typedef struct {
	int16 target_pwm;	//Ŀ��PWMֵ�����ʼ��
	int16 output_pwm;	//���������PWMֵ�����ʼ��
	uint8 period_counter;	//���ڼ�����
	uint8 period_num;	//���PWMֵ��ΪĿ��PWMֵ�����������������Ϊ1�����ʼ��
} ControlPWM, *ControlPWMPtr;

//����Ƕȿ���ģ�����
typedef struct {
	float angle;		//��ǰ�Ƕȣ���λ�㣬���ʼ��
	float angle_speed;	//��ǰ���ٶȣ���λ��/s
	float last_angle_speed;	//�ϴν��ٶȣ���λ��/s�����ʼ��
	float target_angle;	//Ŀ��Ƕȣ���λ�㣬���ʼ��
	ControlPID pid;		//PID���������ʼ��
	ControlPWM pwm;		//PWM���������ʼ��
} ControlAngle, *ControlAnglePtr;

//�����ٶȿ���ģ�����
typedef struct {
	float left_speed;	//�����ٶȣ���λm/s
	float right_speed;	//�����ٶȣ���λm/s
	float target_speed;	//Ŀ���ٶȣ���λm/s�����ʼ��
	float last_speed_err;	//�ϴ��ٶȲ�ֵ����λm/s�����ʼ��
	float distance_err;	//��������λm�����ʼ��
	ControlPID pid;		//PID���������ʼ��
	ControlPWM pwm;		//PWM���������ʼ��
} ControlSpeed, *ControlSpeedPtr;

//����С������
typedef struct {
	ControlAngle angle;	//�Ƕȿ��Ʋ���
	ControlSpeed speed;	//�ٶȿ��Ʋ���
	int16 left_motor_pwm;	//�������PWMֵ
	int16 right_motor_pwm;	//�ҵ�����PWMֵ
} ControlCar, *ControlCarPtr;

//PIDֵת��ΪPWMֵ�ı���
#define CONTROL_PID_PWM_RATIO 	FTM_PERIOD_ACCURACY

//����Ƕȿ���PIDֵ������
#define CONTROL_ANGLE_PID_VAL_MAX	(0.95f)
#define CONTROL_ANGLE_PID_VAL_MIN	(-0.95f)

//�����ٶȿ���PIDֵ������
#define CONTROL_SPEED_PID_VAL_MAX	(0.65f)
#define CONTROL_SPEED_PID_VAL_MIN	(-0.65f)

//�����ٶȿ������ڣ���λs�����ڼ������ͼ��ٶ�
#define CONTROL_SPEED_PERIOD 	(ENCODER_PERIOD / 1000.0f)

//==============================================================
//�������ƣ�control_angle_pid
//�������أ���
//����˵����angle:�Ƕȿ���ģ������ĵ�ַ
//���ܸ�Ҫ�����ݵ�ǰ�ǶȺͽ��ٶȣ����нǶ�PID����(δ��I)��
//         ����Ŀ��PWMֵ������PWM���ڼ��������Զ���¼�ϴν��ٶ�
//��ע: Pֵ:ʹ�ǶȽӽ�Ŀ��Ƕȣ���PWMֵ�����
//     Dֵ:ʹ���ٶȽӽ�0�������ƽǶȱ仯����PWMֵ�����
//==============================================================
void control_angle_pid(ControlAngle* angle);

//==============================================================
//�������ƣ�control_speed_pid
//�������أ���
//����˵����speed:�ٶȿ���ģ������ĵ�ַ
//���ܸ�Ҫ�����ݵ�ǰ���ֺ������ٶȣ������ٶ�PID���ƣ�
//         ����Ŀ��PWMֵ������PWM���ڼ�������
//         �Զ���¼�ϴ��ٶȲ�ֵ�;������
//��ע: Pֵ:ʹ�ٶȽӽ�Ŀ���ٶȣ���PWMֵ�����
//     Iֵ:ʹ�������ӽ�0�����ӿ��ٶȱ仯����PWMֵ�����
//     Dֵ:ʹ���ٶȽӽ�0���������ٶȱ仯����PWMֵ�����
//==============================================================
void control_speed_pid(ControlSpeed* speed);

//===========================================================================
//�������ƣ�control_update_output_pwm
//�������أ���
//����˵����pwm:PWM�����ĵ�ַ
//���ܸ�Ҫ�����������PWMֵ�����ַ�ʽ����ʹ�����PWMֵƽ���ر仯��Ŀ��PWMֵ
//===========================================================================
void control_update_output_pwm(ControlPWM* pwm);

//===========================================================================
//�������ƣ�control_update_motor_pwm
//�������أ���
//����˵����car:С�������ĵ�ַ
//���ܸ�Ҫ�����²����õ�������PWMֵ
//===========================================================================
void control_update_motor_pwm(ControlCar* car);

#endif
