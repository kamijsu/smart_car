//==============================================================
//�ļ����ƣ�control.h
//���ܸ�Ҫ��PID����ͷ�ļ�
//��Ȩ���У����ݴ�ѧ��˼����Ƕ��ʽ����(sumcu.suda.edu.cn)
//==============================================================

#ifndef _CONTROL_H
#define _CONTROL_H

#include "common.h"    //��������Ҫ��ͷ�ļ�
#include "motor.h"
#include "encoder.h"
#include "gyro_acce.h"
#include "ems.h"

//����ģ������PWM����
#define CONTROL_PWM_MAX MOTOR_SPEED_MAX

//ƽ�����ģ���PWM����������
#define ANGLE_PWM_MAX	(0.95)
#define ANGLE_PWM_MIN	(-0.95)

//�ٶȿ���ģ���PWM����������
#define SPEED_PWM_MAX	(0.65)
#define SPEED_PWM_MIN	(-0.65)

//������ģ��������ڣ���λms
#define ANGLE_CONTROL_PERIOD GYRO_ACCE_PERIOD	//�Ƕȿ�������
#define SPEED_CONTROL_PERIOD ENCODER_PERIOD		//�ٶȿ�������
#define TURN_CONTROL_PERIOD	 EMS_PERIOD			//ת���������

//������ģ��������ڱ�������ԽǶȿ���ģ�飩
#define ANGLE_TIME	(1)
#define SPEED_TIME	(SPEED_CONTROL_PERIOD / ANGLE_CONTROL_PERIOD)
#define TURN_TIME	(TURN_CONTROL_PERIOD / ANGLE_CONTROL_PERIOD)

//==============================================================
//�������ƣ�control_angle
//�������أ��Ƕȿ���ģ��Ե����PWMֵ
//����˵����angle:С������ǣ���λ��
//         angle_speed:С���Ľ��ٶȣ���λ��/s
//		   angle_p,angle_i,angle_d:�Ƕȿ��Ƶ�PIDֵ��0��Ϊ��ʹ�ã�i��δʹ��
//���ܸ�Ҫ����ģֱ�����ƺ��������ݳ�ģ�ǶȺͽ��ٶȼ��㳵ģ����Ŀ�����
//==============================================================
int16_t control_angle(float angle, float angle_speed, float angle_p,
		float angle_i, float angle_d);

//============================================================================
//�������ƣ�control_speed
//�������أ��ٶȿ���ģ��Ե����PWMֵ
//����˵����speed_phy:С���������ٶȣ���λm/s
//		   aim_speed:С����Ŀ�������ٶȣ���λm/s
//		   speed_p,speed_i,speed_d:�ٶȿ��Ƶ�PIDֵ��0��Ϊ��ʹ��
//���ܸ�Ҫ����ģ�ٶȿ��ƺ��������ݳ�ģ�ٶȺ�Ŀ���ٶȼ��㳵ģ����Ŀ�����
//============================================================================
int16_t control_speed(float speed_phy, float aim_speed, float speed_p,
		float speed_i, float speed_d);

//===========================================================================
//�������ƣ�control_turn
//�������أ�ת�����ģ��Ե����PWMֵ
//����˵����EMS_V:�����Ŵ�������ѹֵ�����飬��λV
//		   turn_p,turn_i,turn_d:ת����Ƶ�PIDֵ��0��Ϊ��ʹ�ã�i��δʹ��
//���ܸ�Ҫ����ģת����ƺ��������ݵ�Ŵ������ɼ��ĵ�ѹֵ���㳵ģ����Ŀ�����
//===========================================================================
int16_t control_turn(float *EMS_V, float turn_p, float turn_i, float turn_d);

//===========================================================================
//�������ƣ�control_angle_PWM_out
//�������أ��Ƕȿ���ģ��ƽ�������PWMֵ
//����˵����new_angle_PWM:�¼�����ĽǶ�PWMֵ
//		   last_angle_PWM:�ϴεĽǶ�PWMֵ
//		   period_count:���ڼ�����
//���ܸ�Ҫ��ʹ�Ƕȿ���ģ�����ƽ�����PWMֵ
//===========================================================================
int16_t control_angle_PWM_out(int16_t new_angle_PWM, int16_t last_angle_PWM,
		uint8_t period_count);

//===========================================================================
//�������ƣ�control_speed_PWM_out
//�������أ��ٶȿ���ģ��ƽ�������PWMֵ
//����˵����new_speed_PWM:�¼�������ٶ�PWMֵ
//		   last_speed_PWM:�ϴε��ٶ�PWMֵ
//		   period_count:���ڼ�����
//���ܸ�Ҫ��ʹ�ٶȿ���ģ�����ƽ�����PWMֵ
//===========================================================================
int16_t control_speed_PWM_out(int16_t new_speed_PWM, int16_t last_speed_PWM,
		uint8_t period_count);

//===========================================================================
//�������ƣ�control_turn_PWM_out
//�������أ�ת�����ģ��ƽ�������PWMֵ
//����˵����new_turn_PWM:�¼������ת��PWMֵ
//		   last_turn_PWM:�ϴε�ת��PWMֵ
//		   period_count:���ڼ�����
//���ܸ�Ҫ��ʹת�����ģ�����ƽ�����PWMֵ
//===========================================================================
int16_t control_turn_PWM_out(int16_t new_turn_PWM, int16_t last_turn_PWM,
		uint8_t period_count);

//===========================================================================
//�������ƣ�control_motor_output
//�������أ��Ե�������PWM��ƽ��ֵ
//����˵����angle_PWM:�Ƕ�PWMֵ
//		   speed_PWM:�ٶ�PWMֵ
//		   turn_PWM:ת��PWMֵ
//���ܸ�Ҫ�����ݽǶȡ��ٶȡ�ת�����ģ��Ե�����PWMֵ
//===========================================================================
int16_t control_motor_output(int16_t angle_PWM, int16_t speed_PWM,
		int16_t turn_PWM);

//===========================================================================
//�������ƣ�control_stop
//�������أ���
//����˵������
//���ܸ�Ҫ��ͣ��
//===========================================================================
void control_stop(void);

#endif
