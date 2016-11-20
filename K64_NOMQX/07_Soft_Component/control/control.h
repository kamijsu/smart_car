////==============================================================
////文件名称：control.h
////功能概要：PID控制头文件
////版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
////==============================================================
//
//#ifndef _CONTROL_H
//#define _CONTROL_H
//
//#include "common.h"    //包含公共要素头文件
//#include "motor.h"
//#include "encoder.h"
//#include "gyro_acce.h"
//#include "ems.h"
//
////控制模块可输出PWM上限
//#define CONTROL_PWM_MAX MOTOR_SPEED_MAX
//
////平衡控制模块的PWM比例上下限
//#define ANGLE_PWM_MAX	(0.95)
//#define ANGLE_PWM_MIN	(-0.95)
//
////速度控制模块的PWM比例上下限
//#define SPEED_PWM_MAX	(0.65)
//#define SPEED_PWM_MIN	(-0.65)
//
////各控制模块采样周期，单位ms
//#define ANGLE_CONTROL_PERIOD GYRO_ACCE_PERIOD	//角度控制周期
//#define SPEED_CONTROL_PERIOD ENCODER_PERIOD		//速度控制周期
//#define TURN_CONTROL_PERIOD	 EMS_PERIOD			//转向控制周期
//
////各控制模块采样周期倍数（针对角度控制模块）
//#define ANGLE_TIME	(1)
//#define SPEED_TIME	(SPEED_CONTROL_PERIOD / ANGLE_CONTROL_PERIOD)
//#define TURN_TIME	(TURN_CONTROL_PERIOD / ANGLE_CONTROL_PERIOD)
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
//		float angle_i, float angle_d);
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
//		float speed_i, float speed_d);
//
////===========================================================================
////函数名称：control_turn
////函数返回：转向控制模块对电机的PWM值
////参数说明：EMS_V:储存电磁传感器电压值的数组，单位V
////		   turn_p,turn_i,turn_d:转向控制的PID值，0即为不使用，i暂未使用
////功能概要：车模转向控制函数。根据电磁传感器采集的电压值计算车模电机的控制量
////===========================================================================
//int16_t control_turn(float *EMS_V, float turn_p, float turn_i, float turn_d);
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
//		uint8_t period_count);
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
//		uint8_t period_count);
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
//		uint8_t period_count);
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
//		int16_t turn_PWM);
//
////===========================================================================
////函数名称：control_stop
////函数返回：无
////参数说明：无
////功能概要：停车
////===========================================================================
//void control_stop(void);
//
//#endif
