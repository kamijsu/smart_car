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
//��ע: Pֵ:ʹ�ǶȽӽ�Ŀ��Ƕ�
//     Dֵ:ʹ���ٶȽӽ�0�������ƽǶȱ仯
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
//��ע: Pֵ:ʹ�ٶȽӽ�Ŀ���ٶ�
//     Iֵ:ʹ�������ӽ�0�����ӿ��ٶȱ仯
//     Dֵ:ʹ���ٶȽӽ�0���������ٶȱ仯
//==============================================================
void control_speed_pid(ParamSpeed* speed) {
	static const float speed_err_weight = 0.7f;			//��ǰ�ٶȲ�ֵȨ��
	static const float last_speed_err_weight = 0.3f;	//�ϴ��ٶȲ�ֵȨ��

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

	//��������޷�
	if (speed->distance_err > 2) {
		speed->distance_err = 2;
	} else if (speed->distance_err < -2) {
		speed->distance_err = -2;
	}

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

//==============================================================
//�������ƣ�control_find_left_edge
//�������أ�true:�ҵ����Ե; false:δ�ҵ����Ե
//����˵����img:��ѹ��ͼ����׵�ַ
//         row:Ҫ�ұ�Ե������
//         start_col:��ʼ����
//         end_col:��������
//         edge_col:�洢��Ե�����ĵ�ַ
//���ܸ�Ҫ����������Ѱ�����Ե���ҵ���洢
//==============================================================
static inline bool control_find_left_edge(
		uint8 img[CAMERA_IMG_HEIGHT][CAMERA_IMG_WIDTH], const int16 row,
		int16 start_col, int16 end_col, int16* edge_col) {
	int16 col;

	//ʹ��ʼ�ͽ��������Ϸ�
	start_col = MIN(start_col, CAMERA_IMG_WIDTH - 1);
	end_col = MAX(end_col, 2);

	//��������Ѱ�����Ե
	for (col = start_col; col >= end_col; --col) {
		//�õ�Ϊ�׵㣬�����������Ϊ�ڵ�ʱ��Ϊ���Ե
		if (IS_BLACK(img[row][col - 1]) && IS_BLACK(img[row][col - 2])
		&& IS_WHITE(img[row][col])) {
			*edge_col = col;
			return true;
		}
	}
	return false;
}

//==============================================================
//�������ƣ�control_find_right_edge
//�������أ�true:�ҵ��ұ�Ե; false:δ�ҵ��ұ�Ե
//����˵����img:��ѹ��ͼ����׵�ַ
//         row:Ҫ�ұ�Ե������
//         start_col:��ʼ����
//         end_col:��������
//         edge_col:�洢��Ե�����ĵ�ַ
//���ܸ�Ҫ����������Ѱ���ұ�Ե���ҵ���洢
//==============================================================
static inline bool control_find_right_edge(
		uint8 img[CAMERA_IMG_HEIGHT][CAMERA_IMG_WIDTH], const int16 row,
		int16 start_col, int16 end_col, int16* edge_col) {
	int16 col;

	//ʹ��ʼ�ͽ��������Ϸ�
	start_col = MAX(start_col, 0);
	end_col = MIN(end_col, CAMERA_IMG_WIDTH - 3);

	//��������Ѱ���ұ�Ե
	for (col = start_col; col <= end_col; ++col) {
		//�õ�Ϊ�׵㣬���ұ�������Ϊ�ڵ�ʱ��Ϊ�ұ�Ե
		if (IS_BLACK(img[row][col + 1]) && IS_BLACK(img[row][col + 2])
		&& IS_WHITE(img[row][col])) {
			*edge_col = col;
			return true;
		}
	}
	return false;
}

//==============================================================
//�������ƣ�control_find_down_edge
//�������أ�true:�ҵ��±�Ե; false:δ�ҵ��±�Ե
//����˵����img:��ѹ��ͼ����׵�ַ
//         col:Ҫ�ұ�Ե������
//         start_row:��ʼ����
//         end_row:��������
//         edge_row:�洢��Ե�����ĵ�ַ
//���ܸ�Ҫ����������Ѱ���±�Ե���ҵ���洢
//==============================================================
static inline bool control_find_down_edge(
		uint8 img[CAMERA_IMG_HEIGHT][CAMERA_IMG_WIDTH], const int16 col,
		int16 start_row, int16 end_row, int16* edge_row) {
	int16 row;

	//ʹ��ʼ�ͽ��������Ϸ�
	start_row = MIN(start_row, CAMERA_IMG_HEIGHT - 1);
	end_row = MAX(end_row, 2);

	//��������Ѱ���±�Ե
	for (row = start_row; row >= end_row; --row) {
		//�õ�Ϊ�׵㣬������������Ϊ�ڵ�ʱ��Ϊ�±�Ե
		if (IS_BLACK(
				img[row - 1][col]) && IS_BLACK(img[row - 2][col]) && IS_WHITE(img[row][col])) {
			*edge_row = row;
			return true;
		}
	}
	return false;
}

//==============================================================
//�������ƣ�control_find_next_point
//�������أ���һ�е����ڵ�����
//����˵����points:����׵�ַ��points[1]=2��ʾ��1�еĵ��ڵ�2��
//         next_row:Ҫ�ҵ������
//���ܸ�Ҫ���������Իع飨��С���˷���������ǰ CONTROL_BASE_ROW_NUM ��
//         �ĵ�Ԥ����һ�е���������
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

	//���Իع飨��С���˷��������֪ʶ����������
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
	//�������a��b������y=a+b*x
	param_b = tmp == 0 ? 0 : (avg_xy - avg_x * avg_y) / tmp;
	param_a = avg_y - param_b * avg_x;
	//������һ�е����ڵ�����
	return (int16) (param_a + param_b * next_row + 0.5f);
}

//==============================================================
//�������ƣ�control_fix_points
//�������أ���
//����˵����points:����׵�ַ��points[1]=2��ʾ��1�еĵ��ڵ�2��
//         start_row:��ʼ����
//         end_row:������������С�ڿ�ʼ����
//���ܸ�Ҫ�����ݿ�ʼ�����ͽ���������ֵ���������м���һ��ֱ��
//==============================================================
static inline void control_fix_points(int16 points[CAMERA_IMG_HEIGHT],
		const int16 start_row, const int16 end_row) {
	float slope;
	int16 start_val;
	int16 end_val;
	int16 row;

	//�����Ƿ�������
	if (start_row <= end_row) {
		return;
	}

	//��ȡ��ʼ�ͽ���ֵ
	start_val = points[start_row];
	end_val = points[end_val];

	//����б��
	slope = (end_val - start_val) * 1.0f / (end_row - start_row);

	//�����м��е�ֵ
	for (row = start_row - 1; row > end_row; --row) {
		points[row] = slope * (row - start_row) + start_val;
	}
}

//==============================================================
//�������ƣ�control_find_mid_points
//�������أ���
//����˵����turn:�������ģ������ĵ�ַ
//���ܸ�Ҫ�����ݽ�ѹ��ͼ�����ݣ��ҳ�ͼ���е㡢���ұ�Ե���Լ������Ƿ���Ч
//==============================================================
void control_find_mid_points(ParamTurn* turn) {
	int16 row;	//����
	int16 col;	//����
	bool has_left_edge, has_right_edge;	//�Ƿ��������б�־
	int16 has_no_edge_row_num;	//���߶����ߵ�����
	bool has_down_edge;	//�Ƿ����±�Ե��־
	int16 down_edge_row;	//�±�Ե����

	//���߶����ߵ�������0
	has_no_edge_row_num = 0;

	//������У�Ҳ���ԶԻ����е�ȡƽ��������ÿ�����У���������򵥴���
	for (row = CAMERA_IMG_HEIGHT - 1;
			row > CAMERA_IMG_HEIGHT - 1 - CONTROL_BASE_ROW_NUM; --row) {
		//���м��Ҹ������ұ�Ե
		has_left_edge = control_find_left_edge(turn->img, row,
		CAMERA_IMG_WIDTH >> 1, 0, turn->left_edges + row);
		has_right_edge = control_find_right_edge(turn->img, row,
		CAMERA_IMG_WIDTH >> 1, CAMERA_IMG_WIDTH, turn->right_edges + row);

		//δ�ҵ����ұ�Եʱ������ֵ
		if (!has_left_edge) {
			turn->left_edges[row] = 0;
		}
		if (!has_right_edge) {
			turn->right_edges[row] = CAMERA_IMG_WIDTH - 1;
		}

		//�ø�����Ч
		turn->valid_row[row] = true;
		//�е�Ϊ���ұ�Եƽ��
		turn->mid_points[row] = (turn->left_edges[row] + turn->right_edges[row])
				>> 1;
	}

	//����ʣ������
	for (row = CAMERA_IMG_HEIGHT - 1 - CONTROL_BASE_ROW_NUM; row >= 0; --row) {
		//������һ���е㣬�����ұ�Ե
		has_left_edge = control_find_left_edge(turn->img, row,
				turn->mid_points[row + 1], 0, turn->left_edges + row);

		has_right_edge = control_find_right_edge(turn->img, row,
				turn->mid_points[row + 1],
				CAMERA_IMG_WIDTH, turn->right_edges + row);

		//δ�ҵ����ұ�Եʱ������ֵ
		if (!has_left_edge) {
			turn->left_edges[row] = 0;
		}

		if (!has_right_edge) {
			turn->right_edges[row] = CAMERA_IMG_WIDTH - 1;
		}

		//�ø�����Ч
		turn->valid_row[row] = true;

		//�������ұ�Ե����������е�ֵ
		if (turn->left_edges[row] > 0) {
			if (turn->right_edges[row] < CAMERA_IMG_WIDTH - 1) {
				//���ұ�Ե�����ڣ�ȡ���ұ�Ե��ֵ
				has_no_edge_row_num = 0;
				turn->mid_points[row] = (turn->left_edges[row]
						+ turn->right_edges[row]) >> 1;
			} else {
				//���Ե���ڣ������е�Ϊ��һ���е�������Եƫ��
				has_no_edge_row_num = 0;
				turn->mid_points[row] = turn->mid_points[row + 1]
						+ (turn->left_edges[row] - turn->left_edges[row + 1]);
			}
		} else {
			if (turn->right_edges[row] < CAMERA_IMG_WIDTH - 1) {
				//�ұ�Ե���ڣ������е�Ϊ��һ���е�����ұ�Եƫ��
				has_no_edge_row_num = 0;
				turn->mid_points[row] = turn->mid_points[row + 1]
						+ (turn->right_edges[row] - turn->right_edges[row + 1]);
			} else {
				//���ұ�Ե��������
				//�������Իع�Ԥ������е�
				turn->mid_points[row] = control_find_next_point(
						turn->mid_points, row);
				//���߶���������1
				++has_no_edge_row_num;
				//����3�����߶���ʱ����Ϊ��ʮ�ֻ�Բ���������������⴦��
				if (has_no_edge_row_num >= 3) {
					//���е������У�����Ѱ���±�Ե
					col = turn->mid_points[row];
					has_down_edge = control_find_down_edge(turn->img, col, row,
							0, &down_edge_row);

					if (has_down_edge && down_edge_row >= 15) {
						//�������±�Ե�����±�Ե������������15����ΪΪԲ���������ͳ������һ����
						//���±�Ե��Ϊ���Ե��������ת��
						turn->left_edges[down_edge_row] = col;
						//����ʼ�����е����Ե���±�Ե����������
						control_fix_points(turn->left_edges,
								row + has_no_edge_row_num, down_edge_row);
						//�������Եƫ��������һ���е㣬��������е�
						for (row = row + has_no_edge_row_num;
								row >= down_edge_row; --row) {
							turn->mid_points[row] = turn->mid_points[row + 1]
									+ (turn->left_edges[row]
											- turn->left_edges[row + 1]);
							turn->valid_row[row] = true;
						}
						//Ϊ��������Ч��ת���ú���5���е���ǰ1����ͬ
						for (; row >= down_edge_row - 5; --row) {
							turn->mid_points[row] = turn->mid_points[row + 1];
							turn->valid_row[row] = true;
						}
						//��ʣ������Ч
						memset(turn->valid_row, false,
								sizeof(bool) * (down_edge_row - 5));
						//��������
						return;
					} else {
						//�������±�Եʱ�����±�Ե����Ϊ0
						if (!has_down_edge) {
							down_edge_row = 0;
						}
						//�������±�Ե�������±�Ե����С��15ʱ����ΪΪʮ��

						//���±�Ե�У�Ѱַ���ұ�Ե
						has_left_edge = control_find_left_edge(turn->img,
								down_edge_row,
								CAMERA_IMG_WIDTH >> 1, 0,
								turn->left_edges + down_edge_row);

						has_right_edge = control_find_right_edge(turn->img,
								down_edge_row,
								CAMERA_IMG_WIDTH >> 1,
								CAMERA_IMG_WIDTH,
								turn->right_edges + down_edge_row);

						//δ�ҵ����ұ�Եʱ������ֵ
						if (!has_left_edge) {
							turn->left_edges[down_edge_row] = 0;
						}

						if (!has_right_edge) {
							turn->right_edges[down_edge_row] = CAMERA_IMG_WIDTH
									- 1;
						}

						//�±�Ե���е�Ϊ���ұ�Ե��ֵ
						turn->mid_points[down_edge_row] =
								(turn->left_edges[down_edge_row]
										+ turn->right_edges[down_edge_row])
										>> 1;

						//���������е���±�Ե���е�������
						control_fix_points(turn->mid_points, row,
								down_edge_row);

						//��������Ч
						memset(turn->valid_row + down_edge_row, true,
								sizeof(bool) * (row - down_edge_row + 1));
						//ʣ������Ч
						memset(turn->valid_row, false,
								sizeof(bool) * down_edge_row);
						//��������
						return;
					}
				}
			}
		}

		//�������е�Խ�磬����ֵ����ʣ������Ч����������
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
//�������ƣ�control_cal_avg_mid_point
//�������أ���
//����˵����turn:�������ģ������ĵ�ַ
//���ܸ�Ҫ������ÿ���е�ֵ�������е��Ȩƽ��ֵ
//==============================================================
void control_cal_avg_mid_point(ParamTurn* turn) {
	//ÿ��Ȩ�ر�Խ����Ȩ��Խ��
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
	//����ֵ��Ȩ�صĺͣ�ֱ����Ч��
	for (i = CAMERA_IMG_HEIGHT - 1; i >= 0 && turn->valid_row[i]; --i) {
		val_sum += weight[i] * turn->mid_points[i];
		weight_sum += weight[i];
	}
	//�����е��Ȩƽ��ֵ
	turn->avg_mid_point = val_sum * 1.0f / weight_sum;
}

//==============================================================
//�������ƣ�control_turn_pid
//�������أ���
//����˵����turn:�������ģ������ĵ�ַ
//���ܸ�Ҫ�������е��Ȩƽ��ֵ�����з���PID���ƣ�
//         ����Ŀ��PWMֵ������PWM���ڼ�������
//         �Զ���¼�ϴ��е����
//��ע: Pֵ:ʹ�е�ֵ�ӽ�Ŀ���е�ֵ
//     Dֵ:ʹ�е����仯�ӽ�0�������Ʒ���仯
//==============================================================
void control_turn_pid(ParamTurn* turn) {
	static const float mid_err_weight = 0.7f;		//��ǰ�е��ֵȨ��
	static const float last_mid_err_weight = 0.3f;	//�ϴ��е��ֵȨ��

	float mid_err;	//�е����

	float p_val, d_val;	//Pֵ��Dֵ
	float pid_val;	//PIDֵ

	//�����е����
	mid_err = turn->avg_mid_point - turn->target_mid_point;
	//����Pֵ
	p_val =
			(mid_err * mid_err_weight + turn->last_mid_err * last_mid_err_weight)
					* turn->pid.p;

	//����Dֵ
	d_val = (mid_err - turn->last_mid_err) * turn->pid.d;

	//��¼�ϴ��е����
	turn->last_mid_err = mid_err;

	//����PIDֵ
	pid_val = p_val + d_val;

	//����Ŀ��PWMֵ
	turn->pwm.target_pwm = pid_val * CONTROL_PID_PWM_RATIO;
	//����PWM���ڼ�����
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

//===========================================================================
//�������ƣ�control_stop_car
//�������أ���
//����˵������
//���ܸ�Ҫ�����õ�������PWMֵΪ0
//===========================================================================
void control_stop_car() {
	//���õ����ռ�ձ�Ϊ0
	motor_set(MOTOR0, 0);
	motor_set(MOTOR1, 0);
}
