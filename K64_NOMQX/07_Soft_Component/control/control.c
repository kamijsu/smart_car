//==========================================================================
//�ļ����ƣ�control.c
//���ܸ�Ҫ��С�������������Դ�ļ�
//==========================================================================

#include "control.h"
#include "motor.h"
#include "camera.h"
#include <string.h>

//�����ڲ���
#define IS_WHITE(pixel)	(pixel == CAMERA_EXTRACT_WHITE_VAL)	//���ص��Ƿ�Ϊ��
#define IS_BLACK(pixel)	(pixel == CAMERA_EXTRACT_BLACK_VAL)	//���ص��Ƿ�Ϊ��
#define MAX(a, b)	((a) > (b) ? (a) : (b))		//��ȡ���ֵ
#define MIN(a, b)	((a) < (b) ? (a) : (b))		//��ȡ��Сֵ

//==============================================================
//�������ƣ�control_angle_pid
//�������أ���
//����˵����angle:�Ƕȿ���ģ������ĵ�ַ
//���ܸ�Ҫ�����ݵ�ǰ�ǶȺͽ��ٶȣ����нǶ�PID����(δ��I)��
//         ����Ŀ��PWMֵ������PWM���ڼ��������Զ���¼�ϴν��ٶ�
//��ע: Pֵ:ʹ�ǶȽӽ�Ŀ��Ƕȣ���PWMֵ�����
//     Dֵ:ʹ���ٶȽӽ�0�������ƽǶȱ仯����PWMֵ�����
//==============================================================
void control_angle_pid(ParamAngle* angle) {
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
void control_speed_pid(ParamSpeed* speed) {
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

static inline bool control_find_left_edge(
		uint8 img[CAMERA_IMG_HEIGHT][CAMERA_IMG_WIDTH], const int16 row,
		int16 start_col, int16 end_col, int16* edge_col) {
	int16 col;

	start_col = MIN(start_col, CAMERA_IMG_WIDTH - 1);
	end_col = MAX(end_col, 2);

	for (col = start_col; col >= end_col; --col) {
		if (IS_BLACK(img[row][col - 1]) && IS_BLACK(img[row][col - 2])
		&& IS_WHITE(img[row][col])) {
			*edge_col = col;
			return true;
		}
	}
	return false;
}

static inline bool control_find_right_edge(
		uint8 img[CAMERA_IMG_HEIGHT][CAMERA_IMG_WIDTH], const int16 row,
		int16 start_col, int16 end_col, int16* edge_col) {
	int16 col;

	start_col = MAX(start_col, 0);
	end_col = MIN(end_col, CAMERA_IMG_WIDTH - 3);

	for (col = start_col; col <= end_col; ++col) {
		if (IS_BLACK(img[row][col + 1]) && IS_BLACK(img[row][col + 2])
		&& IS_WHITE(img[row][col])) {
			*edge_col = col;
			return true;
		}
	}
	return false;
}

static inline int16 control_find_next_point(
		const int16 points[CAMERA_IMG_HEIGHT], const int16 next_row) {
	int32 sum_x, sum_y, sum_xy, sum_xx;
	int32 x, y;
	float avg_x, avg_y, avg_xy, avg_xx;
	float param_a, param_b;
	int32 i;
	float tmp;

	sum_x = 0;
	sum_y = 0;
	sum_xy = 0;
	sum_xx = 0;

	for (i = 1; i <= CONTROL_BASE_ROW_NUM; ++i) {
		x = next_row + i;
		y = points[x];
		sum_x += x;
		sum_y += y;
		sum_xy += x * y;
		sum_xx += x * x;
	}
	avg_x = sum_x * 1.0f / CONTROL_BASE_ROW_NUM;
	avg_y = sum_y * 1.0f / CONTROL_BASE_ROW_NUM;
	avg_xy = sum_xy * 1.0f / CONTROL_BASE_ROW_NUM;
	avg_xx = sum_xx * 1.0f / CONTROL_BASE_ROW_NUM;
	tmp = avg_xx - avg_x * avg_x;
	param_b = tmp == 0 ? 0 : (avg_xy - avg_x * avg_y) / tmp;
	param_a = avg_y - param_b * avg_x;
	return (int16) (param_a + param_b * next_row + 0.5f);
}

void control_find_mid_points(ParamTurn* turn) {
	int16 row;
	bool has_left_edge, has_right_edge;
	int16 next_left_edge;
	int16 next_right_edge;

	for (row = CAMERA_IMG_HEIGHT - 1;
			row > CAMERA_IMG_HEIGHT - 1 - CONTROL_BASE_ROW_NUM; --row) {
		has_left_edge = control_find_left_edge(turn->img, row,
		CAMERA_IMG_WIDTH >> 1, 0, turn->left_edges + row);
		has_right_edge = control_find_right_edge(turn->img, row,
		CAMERA_IMG_WIDTH >> 1, CAMERA_IMG_WIDTH, turn->right_edges + row);
		if (!has_left_edge) {
			turn->left_edges[row] = 0;
		}
		if (!has_right_edge) {
			turn->right_edges[row] = CAMERA_IMG_WIDTH - 1;
		}
		turn->valid_row[row] = true;
		turn->mid_points[row] = (turn->left_edges[row] + turn->right_edges[row])
				>> 1;
	}

	for (row = CAMERA_IMG_HEIGHT - 1 - CONTROL_BASE_ROW_NUM; row >= 0; --row) {
		next_left_edge = control_find_next_point(turn->left_edges, row);
		next_right_edge = control_find_next_point(turn->right_edges, row);

		has_left_edge = control_find_left_edge(turn->img, row,
				next_left_edge + CONTROL_EDGE_RANGE,
				next_left_edge - CONTROL_EDGE_RANGE, turn->left_edges + row);

		has_right_edge = control_find_right_edge(turn->img, row,
				next_right_edge - CONTROL_EDGE_RANGE,
				next_right_edge + CONTROL_EDGE_RANGE, turn->right_edges + row);

		if (!has_left_edge) {
			turn->left_edges[row] = next_left_edge;
		}

		if (!has_right_edge) {
			turn->right_edges[row] = next_right_edge;
		}

		turn->valid_row[row] = true;

		if (turn->left_edges[row] > 0) {
			if (turn->right_edges[row] < CAMERA_IMG_WIDTH - 1) {
				turn->mid_points[row] = (turn->left_edges[row]
						+ turn->right_edges[row]) >> 1;
			} else {
				turn->mid_points[row] = turn->mid_points[row + 1]
						+ (turn->left_edges[row] - turn->left_edges[row + 1]);
			}
		} else {
			if (turn->right_edges[row] < CAMERA_IMG_WIDTH - 1) {
				turn->mid_points[row] = turn->mid_points[row + 1]
						+ (turn->right_edges[row] - turn->right_edges[row + 1]);
			} else {
				turn->mid_points[row] = control_find_next_point(
						turn->mid_points, row);
			}
		}

		if (turn->mid_points[row] <= 0) {
			turn->mid_points[row] = 0;
			memset(turn->valid_row, false, sizeof(bool) * row);
			break;
		} else if (turn->mid_points[row] >= CAMERA_IMG_WIDTH - 1) {
			turn->mid_points[row] = CAMERA_IMG_WIDTH - 1;
			memset(turn->valid_row, false, sizeof(bool) * row);
			break;
		}

	}

}

void control_cal_avg_mid_point(ParamTurn* turn) {
	static const uint8 weight[CAMERA_IMG_HEIGHT] = { 1, 1, 1, 1, 1, 2, 2, 2, 2,
			2,	//10
			2, 2, 3, 3, 3, 3, 3, 3, 3, 3, //20
			5, 5, 5, 5, 5, 5, 5, 5, 5, 5, //30
			8, 8, 8, 8, 8, 8, 13, 13, 13, 13, //40
			13, 13, 13, 13, 13, 13, 8, 8, 8, 8, //50
			5, 5, 5, 5, 5, 5, 5, 5, 5, 5 //60
			};

	int32 val_sum;
	int32 weight_sum;
	int16 i;

	val_sum = 0;
	weight_sum = 0;
	for (i = CAMERA_IMG_HEIGHT - 1; i >= 0 && turn->valid_row[i]; --i) {
		val_sum += weight[i] * turn->mid_points[i];
		weight_sum += weight[i];
	}

	turn->avg_mid_point = val_sum * 1.0f / weight_sum;
}

void control_turn_pid(ParamTurn* turn) {
	float mid_err;

	float p_val, d_val;
	float pid_val;

	mid_err = turn->avg_mid_point - (CAMERA_IMG_WIDTH >> 1);
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
void control_update_output_pwm(ParamPWM* pwm) {
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
void control_update_motor_pwm(ParamCar* car) {
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
