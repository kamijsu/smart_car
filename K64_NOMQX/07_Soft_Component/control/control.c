////==============================================================
////文件名称：control.h
////功能概要：PID控制源文件
////版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
////==============================================================
//
//#include "control.h"
//
////==============================================================
////函数名称：control_angle
////函数返回：角度控制模块对电机的PWM值
////参数说明：angle:小车的倾角，单位°
////         angle_speed:小车的角速度，单位°/s
////		   angle_p,angle_i,angle_d:角度控制的PID值，0即为不使用，i暂未使用
////功能概要：车模直立控制函数。根据车模角度和角速度计算车模电机的控制量
////==============================================================
//int16_t control_angle(float angle, float angle_speed, float angle_p,
//		float angle_i, float angle_d)
//{
//	static float last_angle_speed;
//	float angle_P_value, angle_D_value;
//	float f_value;
//	int16_t angle_PWM;
//
//	//增量式PID控制计算P调节量
//	angle_P_value = angle * angle_p;
//
//	//增量式PID控制计算D调节量
//	angle_D_value = (angle_speed * 0.7 + last_angle_speed * 0.3) * angle_d;	//使D控制更加平滑
//
//	f_value = angle_P_value + angle_D_value;
//
//	last_angle_speed = angle_speed;
//
//	//对角度控制限幅
//	if (f_value > ANGLE_PWM_MAX)
//		f_value = ANGLE_PWM_MAX;
//	else if (f_value < ANGLE_PWM_MIN)
//		f_value = ANGLE_PWM_MIN;
//
//	angle_PWM = (int16_t) (f_value * CONTROL_PWM_MAX);
//
//	return angle_PWM;
//}
//
////============================================================================
////函数名称：control_speed
////函数返回：速度控制模块对电机的PWM值
////参数说明：speed_phy:小车的物理速度，单位m/s
////		   aim_speed:小车的目标物理速度，单位m/s
////		   speed_p,speed_i,speed_d:速度控制的PID值，0即为不使用
////功能概要：车模速度控制函数。根据车模速度和目标速度计算车模电机的控制量
////============================================================================
//int16_t control_speed(float speed_phy, float aim_speed, float speed_p,
//		float speed_i, float speed_d)
//{
//	static float speed_err, speed_I_value = 0;  //静态变量存储中间变量
//	float last_speed_err, speed_EC;
//	float speed_P_value, speed_D_value, KI;
//	float f_value;
//	int16_t speed_PWM;
//
//	last_speed_err = speed_err;				//将上一次的偏差保存
//
//	speed_err = speed_phy - aim_speed;		//计算新的偏差值
//
//	speed_EC = speed_err - last_speed_err; 	//计算新的偏差变化值
//
//	//增量式PID控制计算P调节量
//	speed_P_value = (speed_err * 0.9 + last_speed_err * 0.1) * speed_p;	//使P控制更加平滑
//
//	//增量式PID控制计算I调节量
//	KI = speed_err * SPEED_CONTROL_PERIOD / 1000 * speed_i;
//
//	speed_I_value += KI;
//
//	//对积分控制限幅
//	if (speed_I_value < SPEED_PWM_MIN)
//	{
//		speed_I_value = SPEED_PWM_MIN;
//	}
//	else if (speed_I_value > SPEED_PWM_MAX)
//	{
//		speed_I_value = SPEED_PWM_MAX;
//
//	}
//
//	//增量式PID控制计算D调节量
//	speed_D_value = speed_EC / SPEED_CONTROL_PERIOD * 1000 * speed_d;
//
//	f_value = (speed_P_value + speed_I_value + speed_D_value);
//
//	//对速度控制限幅
//	if (f_value < SPEED_PWM_MIN)
//	{
//		f_value = SPEED_PWM_MIN;
//	}
//	else if (f_value > SPEED_PWM_MAX)
//	{
//		f_value = SPEED_PWM_MAX;
//
//	}
//
//	speed_PWM = (int16_t) (f_value * CONTROL_PWM_MAX);
//
//	return speed_PWM;
//
//}
//
////===========================================================================
////函数名称：control_turn
////函数返回：转向控制模块对电机的PWM值
////参数说明：EMS_V:储存电磁传感器电压值的数组，单位V
////		   turn_p,turn_i,turn_d:转向控制的PID值，0即为不使用，i暂未使用
////功能概要：车模转向控制函数。根据电磁传感器采集的电压值计算车模电机的控制量
////===========================================================================
//int16_t control_turn(float *EMS_V, float turn_p, float turn_i, float turn_d)
//{
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
//
////===========================================================================
////函数名称：control_angle_PWM_out
////函数返回：角度控制模块平滑输出的PWM值
////参数说明：new_angle_PWM:新计算出的角度PWM值
////		   last_angle_PWM:上次的角度PWM值
////		   period_count:周期计数器
////功能概要：使角度控制模块可以平滑输出PWM值
////===========================================================================
//int16_t control_angle_PWM_out(int16_t new_angle_PWM, int16_t last_angle_PWM,
//		uint8_t period_count)
//{
//	int16_t PWM_err;
//	int16_t angle_PWM;
//	PWM_err = new_angle_PWM - last_angle_PWM;
//	angle_PWM = PWM_err * (period_count) / ANGLE_TIME + last_angle_PWM;
//	return angle_PWM;
//}
//
////===========================================================================
////函数名称：control_speed_PWM_out
////函数返回：速度控制模块平滑输出的PWM值
////参数说明：new_speed_PWM:新计算出的速度PWM值
////		   last_speed_PWM:上次的速度PWM值
////		   period_count:周期计数器
////功能概要：使速度控制模块可以平滑输出PWM值
////===========================================================================
//int16_t control_speed_PWM_out(int16_t new_speed_PWM, int16_t last_speed_PWM,
//		uint8_t period_count)
//{
//	int16_t PWM_err;
//	int16_t speed_PWM;
//	PWM_err = new_speed_PWM - last_speed_PWM;
//	speed_PWM = PWM_err * (period_count) / SPEED_TIME + last_speed_PWM;
//	return speed_PWM;
//}
//
////===========================================================================
////函数名称：control_turn_PWM_out
////函数返回：转向控制模块平滑输出的PWM值
////参数说明：new_turn_PWM:新计算出的转向PWM值
////		   last_turn_PWM:上次的转向PWM值
////		   period_count:周期计数器
////功能概要：使转向控制模块可以平滑输出PWM值
////===========================================================================
//int16_t control_turn_PWM_out(int16_t new_turn_PWM, int16_t last_turn_PWM,
//		uint8_t period_count)
//{
//	int16_t PWM_err;
//	int16_t turn_PWM;
//	PWM_err = new_turn_PWM - last_turn_PWM;
//	turn_PWM = PWM_err * (period_count) / TURN_TIME + last_turn_PWM;
//	return turn_PWM;
//}
//
////===========================================================================
////函数名称：control_motor_output
////函数返回：对电机输出的PWM的平均值
////参数说明：angle_PWM:角度PWM值
////		   speed_PWM:速度PWM值
////		   turn_PWM:转向PWM值
////功能概要：根据角度、速度、转向控制模块对电机输出PWM值
////===========================================================================
//int16_t control_motor_output(int16_t angle_PWM, int16_t speed_PWM,
//		int16_t turn_PWM)
//{
//	int32_t left_motor_PWM, right_motor_PWM;
//
//	left_motor_PWM = angle_PWM + speed_PWM + turn_PWM;
//	right_motor_PWM = angle_PWM + speed_PWM - turn_PWM;
//
//	motor_set(MOTOR1, left_motor_PWM);
//	motor_set(MOTOR2, right_motor_PWM);
//
//	return ((left_motor_PWM + right_motor_PWM) / 2);
//}
//
////===========================================================================
////函数名称：control_stop
////函数返回：无
////参数说明：无
////功能概要：停车
////===========================================================================
//void control_stop(void)
//{
//	motor_set(MOTOR1, 0);
//	motor_set(MOTOR2, 0);
//}
