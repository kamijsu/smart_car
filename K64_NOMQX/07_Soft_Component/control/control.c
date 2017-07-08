//==========================================================================
//文件名称：control.c
//功能概要：小车控制软件程序源文件
//==========================================================================

#include "control.h"
#include "motor.h"
#include "camera.h"
#include <string.h>

//定义内部宏
#define IS_WHITE(pixel)	(pixel == CAMERA_EXTRACT_WHITE_VAL)	//像素点是否为白
#define IS_BLACK(pixel)	(pixel == CAMERA_EXTRACT_BLACK_VAL)	//像素点是否为黑
#define MAX(a, b)	((a) > (b) ? (a) : (b))		//获取最大值
#define MIN(a, b)	((a) < (b) ? (a) : (b))		//获取最小值

//==============================================================
//函数名称：control_angle_pid
//函数返回：无
//参数说明：angle:角度控制模块参数的地址
//功能概要：根据当前角度和角速度，进行角度PID控制(未用I)，
//         计算目标PWM值并重置PWM周期计数器，自动记录上次角速度
//备注: P值:使角度接近目标角度，与PWM值正相关
//     D值:使角速度接近0，即抑制角度变化，与PWM值正相关
//==============================================================
void control_angle_pid(ParamAngle* angle) {
	static const float angle_speed_weight = 0.7f;		//当前角速度权重
	static const float last_angle_speed_weight = 0.3f;	//上次角速度权重

	float p_val, d_val;	//P值和D值
	float pid_val;		//PID值

	//计算P值
	p_val = (angle->angle - angle->target_angle) * angle->pid.p;
	//计算D值，使用上次角速度可以使D控制更加平滑
	d_val = (angle->angle_speed * angle_speed_weight
			+ angle->last_angle_speed * last_angle_speed_weight) * angle->pid.d;

	//记录上次角速度
	angle->last_angle_speed = angle->angle_speed;

	//计算PID值
	pid_val = p_val + d_val;
	//对PID值进行限幅
	if (pid_val > CONTROL_ANGLE_PID_VAL_MAX) {
		pid_val = CONTROL_ANGLE_PID_VAL_MAX;
	} else if (pid_val < CONTROL_ANGLE_PID_VAL_MIN) {
		pid_val = CONTROL_ANGLE_PID_VAL_MIN;
	}

	//计算目标PWM值
	angle->pwm.target_pwm = pid_val * CONTROL_PID_PWM_RATIO;
	//重置PWM周期计数器
	angle->pwm.period_counter = 0;
}

//==============================================================
//函数名称：control_speed_pid
//函数返回：无
//参数说明：speed:速度控制模块参数的地址
//功能概要：根据当前左轮和右轮速度，进行速度PID控制，
//         计算目标PWM值并重置PWM周期计数器，
//         自动记录上次速度差值和距离误差
//备注: P值:使速度接近目标速度，与PWM值正相关
//     I值:使距离误差接近0，即加快速度变化，与PWM值正相关
//     D值:使加速度接近0，即抑制速度变化，与PWM值正相关
//==============================================================
void control_speed_pid(ParamSpeed* speed) {
	static const float speed_err_weight = 0.9f;			//当前速度差值权重
	static const float last_speed_err_weight = 0.1f;	//上次速度差值权重

	float speed_err;	//当前速度差值
	float p_val, i_val, d_val;	//P值，I值，D值
	float pid_val;		//PID值

	//计算当前速度差值
	speed_err = (speed->left_speed + speed->right_speed) / 2
			- speed->target_speed;

	//计算P值，使用上次速度差值可以使P控制更加平滑
	p_val = (speed_err * speed_err_weight
			+ speed->last_speed_err * last_speed_err_weight) * speed->pid.p;

	//计算距离误差
	speed->distance_err += (speed_err * CONTROL_SPEED_PERIOD);
	//计算I值
	i_val = speed->distance_err * speed->pid.i;

	//计算D值
	d_val = (speed_err - speed->last_speed_err) / CONTROL_SPEED_PERIOD
			* speed->pid.d;

	//记录上次速度差值
	speed->last_speed_err = speed_err;

	//计算PID值
	pid_val = p_val + i_val + d_val;
	//对PID值进行限幅
	if (pid_val > CONTROL_SPEED_PID_VAL_MAX) {
		pid_val = CONTROL_SPEED_PID_VAL_MAX;
	} else if (pid_val < CONTROL_SPEED_PID_VAL_MIN) {
		pid_val = CONTROL_SPEED_PID_VAL_MIN;
	}

	//计算目标PWM值
	speed->pwm.target_pwm = pid_val * CONTROL_PID_PWM_RATIO;
	//重置PWM周期计数器
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
	//计算P值
	p_val = mid_err * turn->pid.p;

	d_val = (turn->last_mid_err - mid_err) * turn->pid.d;

	turn->last_mid_err = mid_err;

	//计算PID值
	pid_val = p_val + d_val;

	turn->pwm.target_pwm = pid_val * CONTROL_PID_PWM_RATIO;
	turn->pwm.period_counter = 0;
}

//===========================================================================
//函数名称：control_update_output_pwm
//函数返回：无
//参数说明：pwm:PWM参数的地址
//功能概要：更新输出的PWM值，这种方式可以使输出的PWM值平滑地变化至目标PWM值
//===========================================================================
void control_update_output_pwm(ParamPWM* pwm) {
	int16 pwm_err;	//PWM差值

	//计算PWM差值
	pwm_err = pwm->target_pwm - pwm->output_pwm;
	//更新PWM周期计数器
	++pwm->period_counter;
	//更新本次输出的PWM值
	pwm->output_pwm = pwm_err * pwm->period_counter / pwm->period_num
			+ pwm->output_pwm;
}

//===========================================================================
//函数名称：control_update_motor_pwm
//函数返回：无
//参数说明：car:小车参数的地址
//功能概要：更新并设置电机输出的PWM值
//===========================================================================
void control_update_motor_pwm(ParamCar* car) {
	//更新左右电机输出的PWM值
	car->left_motor_pwm = car->angle.pwm.output_pwm + car->speed.pwm.output_pwm
			+ car->turn.pwm.output_pwm;
	car->right_motor_pwm = car->angle.pwm.output_pwm + car->speed.pwm.output_pwm
			- car->turn.pwm.output_pwm;
	//设置电机的占空比
	motor_set(MOTOR0, car->left_motor_pwm);
	motor_set(MOTOR1, car->right_motor_pwm);
}

////===========================================================================
////函数名称：control_turn
////函数返回：转向控制模块对电机的PWM值
////参数说明：EMS_V:储存电磁传感器电压值的数组，单位V
////		   turn_p,turn_i,turn_d:转向控制的PID值，0即为不使用，i暂未使用
////功能概要：车模转向控制函数。根据电磁传感器采集的电压值计算车模电机的控制量
////===========================================================================
//int16_t control_turn(float *EMS_V, float turn_p, float turn_i, float turn_d) {
//	//上次内外电磁传感器比例电压差
//	static float last_error_out = 0;
//	static float last_error_in = 0;
//
//	//内外电磁传感器比例电压差
//	float error_out;
//	float error_in;
//
//	//内外电磁传感器控制权重
//	float weight_out = 0.5;
//	float weight_in = 0.5;
//
//	float turn_P_value, turn_D_value;
//	float f_value;
//	int16_t turn_PWM;
//
//	//计算内外电磁传感器比例电压差
//	error_out = (EMS_V[1] - EMS_V[0]) / (EMS_V[0] + EMS_V[1]);
//	error_in = (EMS_V[3] - EMS_V[2]) / (EMS_V[2] + EMS_V[3]);
//
//	//增量式PID控制计算P调节量
//	turn_P_value = turn_p
//			* (((error_out * 0.9) + (last_error_out * 0.1)) * weight_out
//					+ ((error_in * 0.9) + (last_error_in * 0.1)) * weight_in);
//
//	//增量式PID控制计算D调节量
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
