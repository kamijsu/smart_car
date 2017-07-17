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
//备注: P值:使角度接近目标角度
//     D值:使角速度接近0，即抑制角度变化
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
//备注: P值:使速度接近目标速度
//     I值:使距离误差接近0，即加快速度变化
//     D值:使加速度接近0，即抑制速度变化
//==============================================================
void control_speed_pid(ParamSpeed* speed) {
	static const float speed_err_weight = 0.7f;			//当前速度差值权重
	static const float last_speed_err_weight = 0.3f;	//上次速度差值权重

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

	//距离误差限幅
	if (speed->distance_err > 2) {
		speed->distance_err = 2;
	} else if (speed->distance_err < -2) {
		speed->distance_err = -2;
	}

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

//==============================================================
//函数名称：control_find_left_edge
//函数返回：true:找到左边缘; false:未找到左边缘
//参数说明：img:解压后图像的首地址
//         row:要找边缘的行数
//         start_col:开始列数
//         end_col:结束列数
//         edge_col:存储边缘列数的地址
//功能概要：从右向左寻找左边缘，找到后存储
//==============================================================
static inline bool control_find_left_edge(
		uint8 img[CAMERA_IMG_HEIGHT][CAMERA_IMG_WIDTH], const int16 row,
		int16 start_col, int16 end_col, int16* edge_col) {
	int16 col;

	//使开始和结束列数合法
	start_col = MIN(start_col, CAMERA_IMG_WIDTH - 1);
	end_col = MAX(end_col, 2);

	//从右向左寻找左边缘
	for (col = start_col; col >= end_col; --col) {
		//该点为白点，且左边两个点为黑点时，为左边缘
		if (IS_BLACK(img[row][col - 1]) && IS_BLACK(img[row][col - 2])
		&& IS_WHITE(img[row][col])) {
			*edge_col = col;
			return true;
		}
	}
	return false;
}

//==============================================================
//函数名称：control_find_right_edge
//函数返回：true:找到右边缘; false:未找到右边缘
//参数说明：img:解压后图像的首地址
//         row:要找边缘的行数
//         start_col:开始列数
//         end_col:结束列数
//         edge_col:存储边缘列数的地址
//功能概要：从左向右寻找右边缘，找到后存储
//==============================================================
static inline bool control_find_right_edge(
		uint8 img[CAMERA_IMG_HEIGHT][CAMERA_IMG_WIDTH], const int16 row,
		int16 start_col, int16 end_col, int16* edge_col) {
	int16 col;

	//使开始和结束列数合法
	start_col = MAX(start_col, 0);
	end_col = MIN(end_col, CAMERA_IMG_WIDTH - 3);

	//从左向右寻找右边缘
	for (col = start_col; col <= end_col; ++col) {
		//该点为白点，且右边两个点为黑点时，为右边缘
		if (IS_BLACK(img[row][col + 1]) && IS_BLACK(img[row][col + 2])
		&& IS_WHITE(img[row][col])) {
			*edge_col = col;
			return true;
		}
	}
	return false;
}

//==============================================================
//函数名称：control_find_down_edge
//函数返回：true:找到下边缘; false:未找到下边缘
//参数说明：img:解压后图像的首地址
//         col:要找边缘的列数
//         start_row:开始行数
//         end_row:结束行数
//         edge_row:存储边缘行数的地址
//功能概要：从下向上寻找下边缘，找到后存储
//==============================================================
static inline bool control_find_down_edge(
		uint8 img[CAMERA_IMG_HEIGHT][CAMERA_IMG_WIDTH], const int16 col,
		int16 start_row, int16 end_row, int16* edge_row) {
	int16 row;

	//使开始和结束行数合法
	start_row = MIN(start_row, CAMERA_IMG_HEIGHT - 1);
	end_row = MAX(end_row, 2);

	//从下向上寻找下边缘
	for (row = start_row; row >= end_row; --row) {
		//该点为白点，且上面两个点为黑点时，为下边缘
		if (IS_BLACK(
				img[row - 1][col]) && IS_BLACK(img[row - 2][col]) && IS_WHITE(img[row][col])) {
			*edge_row = row;
			return true;
		}
	}
	return false;
}

//==============================================================
//函数名称：control_find_next_point
//函数返回：下一行点所在的列数
//参数说明：points:点的首地址，points[1]=2表示第1行的点在第2列
//         next_row:要找点的行数
//功能概要：运用线性回归（最小二乘法），根据前 CONTROL_BASE_ROW_NUM 行
//         的点预测下一行点所在列数
//==============================================================
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

	//线性回归（最小二乘法），相关知识在网上搜索
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
	//计算参数a和b，形如y=a+b*x
	param_b = tmp == 0 ? 0 : (avg_xy - avg_x * avg_y) / tmp;
	param_a = avg_y - param_b * avg_x;
	//计算下一行点所在的列数
	return (int16) (param_a + param_b * next_row + 0.5f);
}

//==============================================================
//函数名称：control_fix_points
//函数返回：无
//参数说明：points:点的首地址，points[1]=2表示第1行的点在第2列
//         start_row:开始行数
//         end_row:结束行数，需小于开始行数
//功能概要：根据开始行数和结束行数的值，在它们中间连一条直线
//==============================================================
static inline void control_fix_points(int16 points[CAMERA_IMG_HEIGHT],
		const int16 start_row, const int16 end_row) {
	float slope;
	int16 start_val;
	int16 end_val;
	int16 row;

	//行数非法，结束
	if (start_row <= end_row) {
		return;
	}

	//获取开始和结束值
	start_val = points[start_row];
	end_val = points[end_val];

	//计算斜率
	slope = (end_val - start_val) * 1.0f / (end_row - start_row);

	//修正中间行的值
	for (row = start_row - 1; row > end_row; --row) {
		points[row] = slope * (row - start_row) + start_val;
	}
}

//==============================================================
//函数名称：control_find_mid_points
//函数返回：无
//参数说明：turn:方向控制模块参数的地址
//功能概要：根据解压后图像数据，找出图像中点、左右边缘，以及该行是否有效
//==============================================================
void control_find_mid_points(ParamTurn* turn) {
	int16 row;	//行数
	int16 col;	//列数
	bool has_left_edge, has_right_edge;	//是否有左右行标志
	int16 has_no_edge_row_num;	//两边都丢线的行数
	bool has_down_edge;	//是否有下边缘标志
	int16 down_edge_row;	//下边缘行数

	//两边都丢线的行数置0
	has_no_edge_row_num = 0;

	//处理基行，也可以对基行中点取平均，赋给每个基行，这里仅做简单处理
	for (row = CAMERA_IMG_HEIGHT - 1;
			row > CAMERA_IMG_HEIGHT - 1 - CONTROL_BASE_ROW_NUM; --row) {
		//从中间找该行左右边缘
		has_left_edge = control_find_left_edge(turn->img, row,
		CAMERA_IMG_WIDTH >> 1, 0, turn->left_edges + row);
		has_right_edge = control_find_right_edge(turn->img, row,
		CAMERA_IMG_WIDTH >> 1, CAMERA_IMG_WIDTH, turn->right_edges + row);

		//未找到左右边缘时，赋极值
		if (!has_left_edge) {
			turn->left_edges[row] = 0;
		}
		if (!has_right_edge) {
			turn->right_edges[row] = CAMERA_IMG_WIDTH - 1;
		}

		//置该行有效
		turn->valid_row[row] = true;
		//中点为左右边缘平均
		turn->mid_points[row] = (turn->left_edges[row] + turn->right_edges[row])
				>> 1;
	}

	//处理剩余行数
	for (row = CAMERA_IMG_HEIGHT - 1 - CONTROL_BASE_ROW_NUM; row >= 0; --row) {
		//根据上一行中点，找左右边缘
		has_left_edge = control_find_left_edge(turn->img, row,
				turn->mid_points[row + 1], 0, turn->left_edges + row);

		has_right_edge = control_find_right_edge(turn->img, row,
				turn->mid_points[row + 1],
				CAMERA_IMG_WIDTH, turn->right_edges + row);

		//未找到左右边缘时，赋极值
		if (!has_left_edge) {
			turn->left_edges[row] = 0;
		}

		if (!has_right_edge) {
			turn->right_edges[row] = CAMERA_IMG_WIDTH - 1;
		}

		//置该行有效
		turn->valid_row[row] = true;

		//根据左右边缘情况，计算中点值
		if (turn->left_edges[row] > 0) {
			if (turn->right_edges[row] < CAMERA_IMG_WIDTH - 1) {
				//左右边缘都存在，取左右边缘均值
				has_no_edge_row_num = 0;
				turn->mid_points[row] = (turn->left_edges[row]
						+ turn->right_edges[row]) >> 1;
			} else {
				//左边缘存在，该行中点为上一行中点加上左边缘偏移
				has_no_edge_row_num = 0;
				turn->mid_points[row] = turn->mid_points[row + 1]
						+ (turn->left_edges[row] - turn->left_edges[row + 1]);
			}
		} else {
			if (turn->right_edges[row] < CAMERA_IMG_WIDTH - 1) {
				//右边缘存在，该行中点为上一行中点加上右边缘偏移
				has_no_edge_row_num = 0;
				turn->mid_points[row] = turn->mid_points[row + 1]
						+ (turn->right_edges[row] - turn->right_edges[row + 1]);
			} else {
				//左右边缘均不存在
				//利用线性回归预测该行中点
				turn->mid_points[row] = control_find_next_point(
						turn->mid_points, row);
				//两边丢线行数加1
				++has_no_edge_row_num;
				//连续3行两边丢线时，认为在十字或圆环赛道，进行特殊处理
				if (has_no_edge_row_num >= 3) {
					//在中点所在列，向上寻找下边缘
					col = turn->mid_points[row];
					has_down_edge = control_find_down_edge(turn->img, col, row,
							0, &down_edge_row);

					if (has_down_edge && down_edge_row >= 15) {
						//若存在下边缘，且下边缘所在行数大于15，认为为圆环（进环和出环情况一样）
						//置下边缘点为左边缘（让其右转）
						turn->left_edges[down_edge_row] = col;
						//将开始丢线行的左边缘和下边缘点连接起来
						control_fix_points(turn->left_edges,
								row + has_no_edge_row_num, down_edge_row);
						//根据左边缘偏移量和上一行中点，计算该行中点
						for (row = row + has_no_edge_row_num;
								row >= down_edge_row; --row) {
							turn->mid_points[row] = turn->mid_points[row + 1]
									+ (turn->left_edges[row]
											- turn->left_edges[row + 1]);
							turn->valid_row[row] = true;
						}
						//为了让其有效右转，置后面5行中点与前1行相同
						for (; row >= down_edge_row - 5; --row) {
							turn->mid_points[row] = turn->mid_points[row + 1];
							turn->valid_row[row] = true;
						}
						//置剩余行无效
						memset(turn->valid_row, false,
								sizeof(bool) * (down_edge_row - 5));
						//结束处理
						return;
					} else {
						//不存在下边缘时，置下边缘行数为0
						if (!has_down_edge) {
							down_edge_row = 0;
						}
						//不存在下边缘，或者下边缘行数小于15时，认为为十字

						//在下边缘行，寻址左右边缘
						has_left_edge = control_find_left_edge(turn->img,
								down_edge_row,
								CAMERA_IMG_WIDTH >> 1, 0,
								turn->left_edges + down_edge_row);

						has_right_edge = control_find_right_edge(turn->img,
								down_edge_row,
								CAMERA_IMG_WIDTH >> 1,
								CAMERA_IMG_WIDTH,
								turn->right_edges + down_edge_row);

						//未找到左右边缘时，赋极值
						if (!has_left_edge) {
							turn->left_edges[down_edge_row] = 0;
						}

						if (!has_right_edge) {
							turn->right_edges[down_edge_row] = CAMERA_IMG_WIDTH
									- 1;
						}

						//下边缘行中点为左右边缘均值
						turn->mid_points[down_edge_row] =
								(turn->left_edges[down_edge_row]
										+ turn->right_edges[down_edge_row])
										>> 1;

						//将丢线行中点和下边缘行中点连起来
						control_fix_points(turn->mid_points, row,
								down_edge_row);

						//连线行有效
						memset(turn->valid_row + down_edge_row, true,
								sizeof(bool) * (row - down_edge_row + 1));
						//剩余行无效
						memset(turn->valid_row, false,
								sizeof(bool) * down_edge_row);
						//结束处理
						return;
					}
				}
			}
		}

		//若该行中点越界，赋极值，置剩余行无效，结束处理
		if (turn->mid_points[row] <= 0) {
			turn->mid_points[row] = 0;
			memset(turn->valid_row, false, sizeof(bool) * row);
			return;
		} else if (turn->mid_points[row] >= CAMERA_IMG_WIDTH - 1) {
			turn->mid_points[row] = CAMERA_IMG_WIDTH - 1;
			memset(turn->valid_row, false, sizeof(bool) * row);
			return;
		}

	}

}

//==============================================================
//函数名称：control_cal_avg_mid_point
//函数返回：无
//参数说明：turn:方向控制模块参数的地址
//功能概要：根据每行中点值，计算中点加权平均值
//==============================================================
void control_cal_avg_mid_point(ParamTurn* turn) {
	//每行权重表，越近的权重越大
	static const uint8 weight[CAMERA_IMG_HEIGHT] = { 1, 1, 1, 1, 1, 1, 1, 1, 1,
			1,	//10
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //20
			3, 3, 3, 3, 3, 3, 3, 3, 3, 3, //30
			5, 5, 5, 5, 5, 5, 5, 5, 5, 5, //40
			13, 13, 13, 13, 13, 13, 13, 13, 13, 13, //50
			21, 21, 21, 21, 21, 21, 21, 21, 21, 21 //60
			};

	int32 val_sum;
	int32 weight_sum;
	int16 i;

	val_sum = 0;
	weight_sum = 0;
	//计算值和权重的和，直到无效行
	for (i = CAMERA_IMG_HEIGHT - 1; i >= 0 && turn->valid_row[i]; --i) {
		val_sum += weight[i] * turn->mid_points[i];
		weight_sum += weight[i];
	}
	//计算中点加权平均值
	turn->avg_mid_point = val_sum * 1.0f / weight_sum;
}

//==============================================================
//函数名称：control_turn_pid
//函数返回：无
//参数说明：turn:方向控制模块参数的地址
//功能概要：根据中点加权平均值，进行方向PID控制，
//         计算目标PWM值并重置PWM周期计数器，
//         自动记录上次中点误差
//备注: P值:使中点值接近目标中点值
//     D值:使中点误差变化接近0，即抑制方向变化
//==============================================================
void control_turn_pid(ParamTurn* turn) {
	static const float mid_err_weight = 0.7f;		//当前中点差值权重
	static const float last_mid_err_weight = 0.3f;	//上次中点差值权重

	float mid_err;	//中点误差

	float p_val, d_val;	//P值，D值
	float pid_val;	//PID值

	//计算中点误差
	mid_err = turn->avg_mid_point - turn->target_mid_point;
	//计算P值
	p_val =
			(mid_err * mid_err_weight + turn->last_mid_err * last_mid_err_weight)
					* turn->pid.p;

	//计算D值
	d_val = (mid_err - turn->last_mid_err) * turn->pid.d;

	//记录上次中点误差
	turn->last_mid_err = mid_err;

	//计算PID值
	pid_val = p_val + d_val;

	//计算目标PWM值
	turn->pwm.target_pwm = pid_val * CONTROL_PID_PWM_RATIO;
	//重置PWM周期计数器
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

//===========================================================================
//函数名称：control_stop_car
//函数返回：无
//参数说明：无
//功能概要：设置电机输出的PWM值为0
//===========================================================================
void control_stop_car() {
	//设置电机的占空比为0
	motor_set(MOTOR0, 0);
	motor_set(MOTOR1, 0);
}
