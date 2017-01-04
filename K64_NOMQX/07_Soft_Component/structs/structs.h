//==============================================================
//�ļ����ƣ�structs.h
//���ܸ�Ҫ�����ܳ��Զ���ṹ��ͷ�ļ�
//��Ȩ���У����ݴ�ѧ��˼����Ƕ��ʽ����(sumcu.suda.edu.cn)
//==============================================================

#ifndef _STRUCTS_H
#define _STRUCTS_H

#include "common.h"

//PID����
typedef struct {
	float p;	//��������
	float i;	//���ֿ���
	float d;	//΢�ֿ���
} CarPID, *CarPIDPtr;

//PWM����
typedef struct {
	int16_t new_PWM;	//�¼������PWMֵ
	int16_t last_PWM;	//�ϴμ������PWMֵ
	int16_t output_PWM;	//��������PWMֵ
} CarPWM, *CarPWMPtr;

//С����־����
typedef struct {
	uint8_t reed_switch;	//�ɻɹ��Ƿ��ͨ
	uint8_t over_speed;		//�ٶ��Ƿ����
	uint8_t is_starting;	//�Ƿ�������
	uint8_t can_stop;		//�Ƿ����ͣ��
} CarFlag, *CarFlagPtr;

//ʱ���־����
typedef struct {
	uint8_t f_5ms;
	uint8_t f_10ms;
	uint8_t f_15ms;
	uint8_t f_20ms;
	volatile uint8_t f_50ms;
	uint8_t f_100ms;
	volatile uint8_t f_1s;
	uint8_t f_5s;
	uint8_t f_10s;
	uint8_t f_15s;
	uint8_t f_30s;
	uint8_t f_1min;
} TimeFlag, *TimeFlagPtr;

//ʱ�����������
typedef struct {
	uint8_t c_5ms;
	uint8_t c_10ms;
	uint8_t c_15ms;
	uint8_t c_20ms;
	uint8_t c_50ms;
	uint8_t c_100ms;
	uint8_t c_1s;
	uint16_t c_5s;
	uint16_t c_10s;
	uint16_t c_15s;
	uint16_t c_30s;
	uint16_t c_1min;
} TimeCounter, *TimeCounterPtr;

//���������������
typedef struct {
	uint16_t left;		//���ֱ�����������
	uint16_t right;		//���ֱ�����������
} EncoderCounter, *EncoderCounterPtr;

//�Ƕȿ���ģ�����
typedef struct {
	uint16_t angle_speed_AD;	//������X����ٶ�ADֵ
	uint16_t acce_speed_AD;		//���ٶȴ�����Z����ٶ�ADֵ
	uint16_t acce_set;			//ƽ��ʱ���ٶȴ�����Z����ٶȵ�ADֵ
	float angle_speed_temp;		//���ٶ���ʱ������λ��/s
	float acce_speed_temp;		//���ٶ���ʱ������λg
	float angle;				//С����б�Ƕȣ���λ��
	float angle_speed;			//С�����ٶȣ���λ��/s
	CarPID pid;					//�Ƕȿ���PID����
	CarPWM pwm;					//�Ƕȿ���PWM����
	uint8_t period_counter;		//�Ƕȿ������ڼ�����
} CarAngle, *CarAnglePtr;

//�ٶȿ���ģ�����
typedef struct {
	float left_speed;			//С�������ٶȣ���λm/s
	float right_speed;			//С�������ٶȣ���λm/s
	float avg_speed;			//С��ƽ���ٶȣ���λm/s
	float aim_speed;			//С��Ŀ���ٶȣ���λm/s
	CarPID pid;					//�ٶȿ���PID����
	CarPWM pwm;					//�ٶȿ���PWM����
	uint8_t period_counter;		//�ٶȿ������ڼ�����
} CarSpeed, *CarSpeedPtr;

//ת�����ģ�����
typedef struct {
	float EMS_V[4];				//��Ŵ�������ѹֵ�����飬��λV
	CarPID pid;					//ת�����PID����
	CarPWM pwm;					//ת�����PWM����
	uint8_t period_counter;		//ת��������ڼ�����
} CarTurn, *CarTurnPtr;

//С����ز���
typedef struct {
	CarAngle angle;		//�Ƕȿ��Ʋ���
	CarSpeed speed;		//�ٶȿ��Ʋ���
	CarTurn turn;		//ת����Ʋ���
	int16_t output_PWM;	//�Ե������ĸ��ϵ�PWMֵ
} Car, *CarPtr;

#endif /* _STRUCTS_H */
