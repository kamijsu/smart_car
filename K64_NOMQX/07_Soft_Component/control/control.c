//==========================================================================
//�ļ����ƣ�control.c
//���ܸ�Ҫ��С�������������Դ�ļ�
//==========================================================================

#include "control.h"
#include "motor.h"

//==============================================================
//�������ƣ�control_angle_pid
//�������أ���
//����˵����angle:�Ƕȿ���ģ������ĵ�ַ
//���ܸ�Ҫ�����ݵ�ǰ�ǶȺͽ��ٶȣ����нǶ�PID����(δ��I)��
//         ����Ŀ��PWMֵ������PWM���ڼ��������Զ���¼�ϴν��ٶ�
//��ע: Pֵ:ʹ�ǶȽӽ�Ŀ��Ƕȣ���PWMֵ�����
//     Dֵ:ʹ���ٶȽӽ�0�������ƽǶȱ仯����PWMֵ�����
//==============================================================
void control_angle_pid(ControlAngle* angle) {
	static const float angle_speed_weight = 0.7f;		//��ǰ���ٶ�Ȩ��
	static const float last_angle_speed_weight = 0.3f;	//�ϴν��ٶ�Ȩ��

	float p_val, d_val;	//Pֵ��Dֵ
	float pid_val;		//PIDֵ

	//����Pֵ
	p_val = (angle->angle - angle->target_angle) * angle->pid.p;
	//����Dֵ��ʹ���ϴν��ٶȿ���ʹD���Ƹ���ƽ��
	d_val = (angle->angle_speed * angle_speed_weight
			+ angle->last_angle_speed * last_angle_speed_weight) * angle->pid.d;

	//��¼�ϴν��ٶ�
	angle->last_angle_speed = angle->angle_speed;

	//����PIDֵ
	pid_val = p_val + d_val;
	//��PIDֵ�����޷�
	if (pid_val > CONTROL_ANGLE_PID_VAL_MAX) {
		pid_val = CONTROL_ANGLE_PID_VAL_MAX;
	} else if (pid_val < CONTROL_ANGLE_PID_VAL_MIN) {
		pid_val = CONTROL_ANGLE_PID_VAL_MIN;
	}

	//����Ŀ��PWMֵ
	angle->pwm.target_pwm = pid_val * CONTROL_PID_PWM_RATIO;
	//����PWM���ڼ�����
	angle->pwm.period_counter = 0;
}

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
void control_speed_pid(ControlSpeed* speed) {
	static const float speed_err_weight = 0.9f;			//��ǰ�ٶȲ�ֵȨ��
	static const float last_speed_err_weight = 0.1f;	//�ϴ��ٶȲ�ֵȨ��

	float speed_err;	//��ǰ�ٶȲ�ֵ
	float p_val, i_val, d_val;	//Pֵ��Iֵ��Dֵ
	float pid_val;		//PIDֵ

	//���㵱ǰ�ٶȲ�ֵ
	speed_err = (speed->left_speed + speed->right_speed) / 2
			- speed->target_speed;

	//����Pֵ��ʹ���ϴ��ٶȲ�ֵ����ʹP���Ƹ���ƽ��
	p_val = (speed_err * speed_err_weight
			+ speed->last_speed_err * last_speed_err_weight) * speed->pid.p;

	//����������
	speed->distance_err += (speed_err * CONTROL_SPEED_PERIOD);
	//����Iֵ
	i_val = speed->distance_err * speed->pid.i;

	//����Dֵ
	d_val = (speed_err - speed->last_speed_err) / CONTROL_SPEED_PERIOD
			* speed->pid.d;

	//��¼�ϴ��ٶȲ�ֵ
	speed->last_speed_err = speed_err;

	//����PIDֵ
	pid_val = p_val + i_val + d_val;
	//��PIDֵ�����޷�
	if (pid_val > CONTROL_SPEED_PID_VAL_MAX) {
		pid_val = CONTROL_SPEED_PID_VAL_MAX;
	} else if (pid_val < CONTROL_SPEED_PID_VAL_MIN) {
		pid_val = CONTROL_SPEED_PID_VAL_MIN;
	}

	//����Ŀ��PWMֵ
	speed->pwm.target_pwm = pid_val * CONTROL_PID_PWM_RATIO;
	//����PWM���ڼ�����
	speed->pwm.period_counter = 0;
}

void control_turn_pid(ControlTurn* turn) {
	float mid_err;

	float p_val, d_val;
	float pid_val;

	mid_err = turn->midpoint - 40;
	//����Pֵ
	p_val = mid_err * turn->pid.p;

	d_val = (turn->last_mid_err - mid_err) * turn->pid.d;

	turn->last_mid_err = mid_err;

	//����PIDֵ
	pid_val = p_val + d_val;

	turn->pwm.target_pwm = pid_val * CONTROL_PID_PWM_RATIO;
	turn->pwm.period_counter = 0;
}

//===========================================================================
//�������ƣ�control_update_output_pwm
//�������أ���
//����˵����pwm:PWM�����ĵ�ַ
//���ܸ�Ҫ�����������PWMֵ�����ַ�ʽ����ʹ�����PWMֵƽ���ر仯��Ŀ��PWMֵ
//===========================================================================
void control_update_output_pwm(ControlPWM* pwm) {
	int16 pwm_err;	//PWM��ֵ

	//����PWM��ֵ
	pwm_err = pwm->target_pwm - pwm->output_pwm;
	//����PWM���ڼ�����
	++pwm->period_counter;
	//���±��������PWMֵ
	pwm->output_pwm = pwm_err * pwm->period_counter / pwm->period_num
			+ pwm->output_pwm;
}

//===========================================================================
//�������ƣ�control_update_motor_pwm
//�������أ���
//����˵����car:С�������ĵ�ַ
//���ܸ�Ҫ�����²����õ�������PWMֵ
//===========================================================================
void control_update_motor_pwm(ControlCar* car) {
	//�������ҵ�������PWMֵ
	car->left_motor_pwm = car->angle.pwm.output_pwm + car->speed.pwm.output_pwm
			+ car->turn.pwm.output_pwm;
	car->right_motor_pwm = car->angle.pwm.output_pwm + car->speed.pwm.output_pwm
			- car->turn.pwm.output_pwm;
	//���õ����ռ�ձ�
	motor_set(MOTOR0, car->left_motor_pwm);
	motor_set(MOTOR1, car->right_motor_pwm);
}

////===========================================================================
////�������ƣ�control_turn
////�������أ�ת�����ģ��Ե����PWMֵ
////����˵����EMS_V:�����Ŵ�������ѹֵ�����飬��λV
////		   turn_p,turn_i,turn_d:ת����Ƶ�PIDֵ��0��Ϊ��ʹ�ã�i��δʹ��
////���ܸ�Ҫ����ģת����ƺ��������ݵ�Ŵ������ɼ��ĵ�ѹֵ���㳵ģ����Ŀ�����
////===========================================================================
//int16_t control_turn(float *EMS_V, float turn_p, float turn_i, float turn_d) {
//	//�ϴ������Ŵ�����������ѹ��
//	static float last_error_out = 0;
//	static float last_error_in = 0;
//
//	//�����Ŵ�����������ѹ��
//	float error_out;
//	float error_in;
//
//	//�����Ŵ���������Ȩ��
//	float weight_out = 0.5;
//	float weight_in = 0.5;
//
//	float turn_P_value, turn_D_value;
//	float f_value;
//	int16_t turn_PWM;
//
//	//���������Ŵ�����������ѹ��
//	error_out = (EMS_V[1] - EMS_V[0]) / (EMS_V[0] + EMS_V[1]);
//	error_in = (EMS_V[3] - EMS_V[2]) / (EMS_V[2] + EMS_V[3]);
//
//	//����ʽPID���Ƽ���P������
//	turn_P_value = turn_p
//			* (((error_out * 0.9) + (last_error_out * 0.1)) * weight_out
//					+ ((error_in * 0.9) + (last_error_in * 0.1)) * weight_in);
//
//	//����ʽPID���Ƽ���D������
//	turn_D_value = turn_d
//			* ((error_out - last_error_out) * weight_out
//					+ (error_in - last_error_in) * weight_in)
//			/ TURN_CONTROL_PERIOD * 1000;
//
//	last_error_out = error_out;
//	last_error_in = error_in;
//
//	f_value = turn_P_value + turn_D_value;
//
//	turn_PWM = (int16_t) (f_value * CONTROL_PWM_MAX);
//
//	return turn_PWM;
//}
