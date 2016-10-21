//==============================================================
//�ļ����ƣ�control.h
//���ܸ�Ҫ��PID����Դ�ļ�
//��Ȩ���У����ݴ�ѧ��˼����Ƕ��ʽ����(sumcu.suda.edu.cn)
//==============================================================

#include "control.h"

//==============================================================
//�������ƣ�control_angle
//�������أ��Ƕȿ���ģ��Ե����PWMֵ
//����˵����angle:С������ǣ���λ��
//         angle_speed:С���Ľ��ٶȣ���λ��/s
//		   angle_p,angle_i,angle_d:�Ƕȿ��Ƶ�PIDֵ��0��Ϊ��ʹ�ã�i��δʹ��
//���ܸ�Ҫ����ģֱ�����ƺ��������ݳ�ģ�ǶȺͽ��ٶȼ��㳵ģ����Ŀ�����
//==============================================================
int16_t control_angle(float angle, float angle_speed, float angle_p,
		float angle_i, float angle_d)
{
	static float last_angle_speed;
	float angle_P_value, angle_D_value;
	float f_value;
	int16_t angle_PWM;

	//����ʽPID���Ƽ���P������
	angle_P_value = angle * angle_p;

	//����ʽPID���Ƽ���D������
	angle_D_value = (angle_speed * 0.7 + last_angle_speed * 0.3) * angle_d;	//ʹD���Ƹ���ƽ��

	f_value = angle_P_value + angle_D_value;

	last_angle_speed = angle_speed;

	//�ԽǶȿ����޷�
	if (f_value > ANGLE_PWM_MAX)
		f_value = ANGLE_PWM_MAX;
	else if (f_value < ANGLE_PWM_MIN)
		f_value = ANGLE_PWM_MIN;

	angle_PWM = (int16_t) (f_value * CONTROL_PWM_MAX);

	return angle_PWM;
}

//============================================================================
//�������ƣ�control_speed
//�������أ��ٶȿ���ģ��Ե����PWMֵ
//����˵����speed_phy:С���������ٶȣ���λm/s
//		   aim_speed:С����Ŀ�������ٶȣ���λm/s
//		   speed_p,speed_i,speed_d:�ٶȿ��Ƶ�PIDֵ��0��Ϊ��ʹ��
//���ܸ�Ҫ����ģ�ٶȿ��ƺ��������ݳ�ģ�ٶȺ�Ŀ���ٶȼ��㳵ģ����Ŀ�����
//============================================================================
int16_t control_speed(float speed_phy, float aim_speed, float speed_p,
		float speed_i, float speed_d)
{
	static float speed_err, speed_I_value = 0;  //��̬�����洢�м����
	float last_speed_err, speed_EC;
	float speed_P_value, speed_D_value, KI;
	float f_value;
	int16_t speed_PWM;

	last_speed_err = speed_err;				//����һ�ε�ƫ���

	speed_err = speed_phy - aim_speed;		//�����µ�ƫ��ֵ

	speed_EC = speed_err - last_speed_err; 	//�����µ�ƫ��仯ֵ

	//����ʽPID���Ƽ���P������
	speed_P_value = (speed_err * 0.9 + last_speed_err * 0.1) * speed_p;	//ʹP���Ƹ���ƽ��

	//����ʽPID���Ƽ���I������
	KI = speed_err * SPEED_CONTROL_PERIOD / 1000 * speed_i;

	speed_I_value += KI;

	//�Ի��ֿ����޷�
	if (speed_I_value < SPEED_PWM_MIN)
	{
		speed_I_value = SPEED_PWM_MIN;
	}
	else if (speed_I_value > SPEED_PWM_MAX)
	{
		speed_I_value = SPEED_PWM_MAX;

	}

	//����ʽPID���Ƽ���D������
	speed_D_value = speed_EC / SPEED_CONTROL_PERIOD * 1000 * speed_d;

	f_value = (speed_P_value + speed_I_value + speed_D_value);

	//���ٶȿ����޷�
	if (f_value < SPEED_PWM_MIN)
	{
		f_value = SPEED_PWM_MIN;
	}
	else if (f_value > SPEED_PWM_MAX)
	{
		f_value = SPEED_PWM_MAX;

	}

	speed_PWM = (int16_t) (f_value * CONTROL_PWM_MAX);

	return speed_PWM;

}

//===========================================================================
//�������ƣ�control_turn
//�������أ�ת�����ģ��Ե����PWMֵ
//����˵����EMS_V:�����Ŵ�������ѹֵ�����飬��λV
//		   turn_p,turn_i,turn_d:ת����Ƶ�PIDֵ��0��Ϊ��ʹ�ã�i��δʹ��
//���ܸ�Ҫ����ģת����ƺ��������ݵ�Ŵ������ɼ��ĵ�ѹֵ���㳵ģ����Ŀ�����
//===========================================================================
int16_t control_turn(float *EMS_V, float turn_p, float turn_i, float turn_d)
{
	//�ϴ������Ŵ�����������ѹ��
	static float last_error_out = 0;
	static float last_error_in = 0;

	//�����Ŵ�����������ѹ��
	float error_out;
	float error_in;

	//�����Ŵ���������Ȩ��
	float weight_out = 0.5;
	float weight_in = 0.5;

	float turn_P_value, turn_D_value;
	float f_value;
	int16_t turn_PWM;

	//���������Ŵ�����������ѹ��
	error_out = (EMS_V[1] - EMS_V[0]) / (EMS_V[0] + EMS_V[1]);
	error_in = (EMS_V[3] - EMS_V[2]) / (EMS_V[2] + EMS_V[3]);

	//����ʽPID���Ƽ���P������
	turn_P_value = turn_p
			* (((error_out * 0.9) + (last_error_out * 0.1)) * weight_out
					+ ((error_in * 0.9) + (last_error_in * 0.1)) * weight_in);

	//����ʽPID���Ƽ���D������
	turn_D_value = turn_d
			* ((error_out - last_error_out) * weight_out
					+ (error_in - last_error_in) * weight_in)
			/ TURN_CONTROL_PERIOD * 1000;

	last_error_out = error_out;
	last_error_in = error_in;

	f_value = turn_P_value + turn_D_value;

	turn_PWM = (int16_t) (f_value * CONTROL_PWM_MAX);

	return turn_PWM;
}

//===========================================================================
//�������ƣ�control_angle_PWM_out
//�������أ��Ƕȿ���ģ��ƽ�������PWMֵ
//����˵����new_angle_PWM:�¼�����ĽǶ�PWMֵ
//		   last_angle_PWM:�ϴεĽǶ�PWMֵ
//		   period_count:���ڼ�����
//���ܸ�Ҫ��ʹ�Ƕȿ���ģ�����ƽ�����PWMֵ
//===========================================================================
int16_t control_angle_PWM_out(int16_t new_angle_PWM, int16_t last_angle_PWM,
		uint8_t period_count)
{
	int16_t PWM_err;
	int16_t angle_PWM;
	PWM_err = new_angle_PWM - last_angle_PWM;
	angle_PWM = PWM_err * (period_count) / ANGLE_TIME + last_angle_PWM;
	return angle_PWM;
}

//===========================================================================
//�������ƣ�control_speed_PWM_out
//�������أ��ٶȿ���ģ��ƽ�������PWMֵ
//����˵����new_speed_PWM:�¼�������ٶ�PWMֵ
//		   last_speed_PWM:�ϴε��ٶ�PWMֵ
//		   period_count:���ڼ�����
//���ܸ�Ҫ��ʹ�ٶȿ���ģ�����ƽ�����PWMֵ
//===========================================================================
int16_t control_speed_PWM_out(int16_t new_speed_PWM, int16_t last_speed_PWM,
		uint8_t period_count)
{
	int16_t PWM_err;
	int16_t speed_PWM;
	PWM_err = new_speed_PWM - last_speed_PWM;
	speed_PWM = PWM_err * (period_count) / SPEED_TIME + last_speed_PWM;
	return speed_PWM;
}

//===========================================================================
//�������ƣ�control_turn_PWM_out
//�������أ�ת�����ģ��ƽ�������PWMֵ
//����˵����new_turn_PWM:�¼������ת��PWMֵ
//		   last_turn_PWM:�ϴε�ת��PWMֵ
//		   period_count:���ڼ�����
//���ܸ�Ҫ��ʹת�����ģ�����ƽ�����PWMֵ
//===========================================================================
int16_t control_turn_PWM_out(int16_t new_turn_PWM, int16_t last_turn_PWM,
		uint8_t period_count)
{
	int16_t PWM_err;
	int16_t turn_PWM;
	PWM_err = new_turn_PWM - last_turn_PWM;
	turn_PWM = PWM_err * (period_count) / TURN_TIME + last_turn_PWM;
	return turn_PWM;
}

//===========================================================================
//�������ƣ�control_motor_output
//�������أ��Ե�������PWM��ƽ��ֵ
//����˵����angle_PWM:�Ƕ�PWMֵ
//		   speed_PWM:�ٶ�PWMֵ
//		   turn_PWM:ת��PWMֵ
//���ܸ�Ҫ�����ݽǶȡ��ٶȡ�ת�����ģ��Ե�����PWMֵ
//===========================================================================
int16_t control_motor_output(int16_t angle_PWM, int16_t speed_PWM,
		int16_t turn_PWM)
{
	int32_t left_motor_PWM, right_motor_PWM;

	left_motor_PWM = angle_PWM + speed_PWM + turn_PWM;
	right_motor_PWM = angle_PWM + speed_PWM - turn_PWM;

	motor_set(MOTOR1, left_motor_PWM);
	motor_set(MOTOR2, right_motor_PWM);

	return ((left_motor_PWM + right_motor_PWM) / 2);
}

//===========================================================================
//�������ƣ�control_stop
//�������أ���
//����˵������
//���ܸ�Ҫ��ͣ��
//===========================================================================
void control_stop(void)
{
	motor_set(MOTOR1, 0);
	motor_set(MOTOR2, 0);
}
