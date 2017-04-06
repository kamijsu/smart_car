//==========================================================================
//文件名称：motor.c
//功能概要：电机应用驱动程序源文件
//==========================================================================

#include "motor.h"

//各电机PWM0通道号
static const uint8 motor_pwm0_table[] = { MOTOR0_PWM0_CH, MOTOR1_PWM0_CH };
//各电机PWM1通道号
static const uint8 motor_pwm1_table[] = { MOTOR0_PWM1_CH, MOTOR1_PWM1_CH };

//==========================================================================
//函数名称: motor_init
//函数返回: 无
//参数说明: 无
//功能概要: 初始化所有电机，初始占空比为0，相应配置在motor.h中
//==========================================================================
void motor_init() {
	//初始化FTM模块
	ftm_init(MOTOR_FTM_MOD, MOTOR_FTM_CLK_DIV, MOTOR_FTM_COUNTER_MODE,
	MOTOR_FTM_COUNTER_PERIOD);
	//初始化MOTOR0
	ftm_pwm_single_init(MOTOR_FTM_MOD, motor_pwm0_table[MOTOR0],
	MOTOR_FTM_PWM_MODE,
	MOTOR_FTM_PWM_POL, 0);
	ftm_pwm_single_init(MOTOR_FTM_MOD, motor_pwm1_table[MOTOR0],
	MOTOR_FTM_PWM_MODE,
	MOTOR_FTM_PWM_POL, 0);
	//初始化MOTOR1
	ftm_pwm_single_init(MOTOR_FTM_MOD, motor_pwm0_table[MOTOR1],
	MOTOR_FTM_PWM_MODE,
	MOTOR_FTM_PWM_POL, 0);
	ftm_pwm_single_init(MOTOR_FTM_MOD, motor_pwm1_table[MOTOR1],
	MOTOR_FTM_PWM_MODE,
	MOTOR_FTM_PWM_POL, 0);
}

//==========================================================================
//函数名称: motor_set
//函数返回: 无
//参数说明: motor:电机号:
//               MOTORx，x为电机号;
//         duty:占空比，范围[MOTOR_DUTY_MIN(-9800),MOTOR_DUTY_MAX(9800)]
//功能概要: 设置电机的占空比，将在下一个计数周期更新
//备注: 当duty>=0时，PWM0占空比为|duty|，PWM1占空比为0，
//     当duty<0时，PWM0占空比为0，PWM1占空比为|duty|，
//     占空比已做上下限限幅
//==========================================================================
void motor_set(uint8 motor, int16 duty) {
	uint16 duty_set;	//所要设置的占空比

	if (duty >= 0) {
		//上限限幅
		duty_set = duty > MOTOR_DUTY_MAX ? MOTOR_DUTY_MAX : duty;

		//设置PWM0和PWM1
		ftm_pwm_single_set(MOTOR_FTM_MOD, motor_pwm0_table[motor], duty_set);
		ftm_pwm_single_set(MOTOR_FTM_MOD, motor_pwm1_table[motor], 0);
	} else {
		//下限限幅
		duty_set = duty < MOTOR_DUTY_MIN ? -MOTOR_DUTY_MIN : -duty;

		//设置PWM0和PWM1
		ftm_pwm_single_set(MOTOR_FTM_MOD, motor_pwm0_table[motor], 0);
		ftm_pwm_single_set(MOTOR_FTM_MOD, motor_pwm1_table[motor], duty_set);
	}
}

