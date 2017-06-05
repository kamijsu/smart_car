//==========================================================================
//文件名称：control.c
//功能概要：小车控制软件程序源文件
//==========================================================================

#include "control.h"
#include "motor.h"

//==============================================================
//函数名称：control_angle_pid
//函数返回：无
//参数说明：angle:角度控制模块参数的地址
//功能概要：根据当前角度和角速度，进行角度PID控制(未用I)，
//         计算目标PWM值并重置PWM周期计数器，自动记录上次角速度
//备注: P值:使角度接近目标角度，与PWM值正相关
//     D值:使角速度接近0，即抑制角度变化，与PWM值正相关
//==============================================================
void control_angle_pid(ControlAngle* angle) {
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
void control_speed_pid(ControlSpeed* speed) {
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

void control_turn_pid(ControlTurn* turn) {
	float mid_err;

	float p_val, d_val;
	float pid_val;

	mid_err = turn->midpoint - 40;
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
void control_update_output_pwm(ControlPWM* pwm) {
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
void control_update_motor_pwm(ControlCar* car) {
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
