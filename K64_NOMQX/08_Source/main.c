//说明见工程文件夹下的Doc文件夹内Readme.txt文件
//============================================================================

#define GLOBLE_VAR  //只需在main.c中定义一次，用来防止全局变量的重复定义

#include "includes.h"   //包含总头文件

int main(void) {
	//1. 声明主函数使用的变量
uint32 run_counter;


	/* 小车相关参数变量 */
	Car car;

	/*串口调试工具用临时变量*/
	float data_out[4];

	//2. 关总中断
	DISABLE_INTERRUPTS;

	//3. 初始化外设模块
	light_init(LIGHT_RED, LIGHT_OFF); //蓝灯初始化
	//light_init(LIGHT_BLUE, LIGHT_OFF);
	uart_init(UART_USE,9600,UART_PARITY_DISABLED,UART_STOP_BIT_1);     //uart1初始化，蓝牙用，蓝牙模块波特率9600，无法在5ms中断中传输数据
//	uart_init(UART_USE, 115200);   //uart1初始化，串口用
	//pit_init(PIT_CH0, 5);  //pit0初始化，周期5ms
//	motor_init(MOTOR1);			//左电机初始化
//	motor_init(MOTOR2);			//右电机初始化
	gyro_acce_init();			//陀螺仪加速度计初始化
//	encoder_init(ENCODER1);		//左编码器初始化
//	encoder_init(ENCODER2);		//右编码器初始化
	ems_init();					//电磁传感器初始化
	reed_switch_init();			//干簧管初始化
	ftm_init(FTM_MOD1,FTM_CLK_DIV_128,FTM_COUNTER_MODE_UP,150);

	ftm_init(FTM_MOD0,FTM_CLK_DIV_128,FTM_COUNTER_MODE_UP_DOWN,100);
//ftm_pwm_combine_init(FTM_MOD0,FTM_CH_GROUP2,FTM_PWM_MODE_COMPLEMENTARY,FTM_PWM_POL_NEGATIVE,1000,8000);
	ftm_pwm_single_init(FTM_MOD0,FTM_CH4,FTM_PWM_MODE_CENTER_ALIGNED,FTM_PWM_POL_NEGATIVE,5000);
	ftm_pwm_single_init(FTM_MOD1,FTM_CH0,FTM_PWM_MODE_EDGE_ALIGNED,FTM_PWM_POL_NEGATIVE,2000);
ftm_pwm_single_set(FTM_MOD0,FTM_CH4,2);
ftm_ic_init(FTM_MOD1,FTM_CH1,FTM_CAPTURE_RISING_EDGE);

	//4. 给有关变量赋初值
	run_counter = 0;

	car.angle.angle = 0;		//角度置0

	/* PWM计算参数置0 */
	car.angle.pwm.new_PWM = 0;
	car.angle.pwm.last_PWM = 0;
	car.angle.period_counter = 0;
	car.speed.pwm.new_PWM = 0;
	car.speed.pwm.last_PWM = 0;
	car.speed.period_counter = 0;
	car.turn.pwm.new_PWM = 0;
	car.turn.pwm.last_PWM = 0;
	car.turn.period_counter = 0;

	/* 0m/s时参数 */
	car.speed.aim_speed = 0.0;		//目标速度初始化，单位m/s
	car.angle.acce_set = 0x0925;	//0m/s时平衡位置
	car.angle.pid.p = 0.2;
	car.angle.pid.i = 0;
	car.angle.pid.d = 0.001;

	car.speed.pid.p = 1;
	car.speed.pid.i = 0;
	car.speed.pid.d = 0;

	car.turn.pid.p = 0;
	car.turn.pid.i = 0;
	car.turn.pid.d = 0;

	/* 全局变量 */
	encoder_counter.left = 0;	//左编码器脉冲计数器置0
	encoder_counter.right = 0;	//右编码器脉冲计数器置0
	car_flag.reed_switch = 0;	//干簧管未接通
	car_flag.over_speed = 0;	//小车未超速
	car_flag.is_starting = 1;	//小车正在起步
	car_flag.can_stop = 0;		//不能停车

	/* 时间标志参数置0 */
	time0_flag.f_5ms = 0;
	time0_flag.f_10ms = 0;
	time0_flag.f_15ms = 0;
	time0_flag.f_20ms = 0;
	time0_flag.f_50ms = 0;
	time0_flag.f_100ms = 0;
	time0_flag.f_1s = 0;
	time0_flag.f_5s = 0;
	time0_flag.f_10s = 0;
	time0_flag.f_15s = 0;
	time0_flag.f_30s = 0;
	time0_flag.f_1min = 0;

	//5. 使能模块中断
	ftm_ic_enable_int(FTM_MOD1,FTM_CH1);
	//pit_enable_int(PIT_CH0);   		//使能pit中断
	uart_enable_re_int(UART_USE);   //使能uart1接收中断
	encoder_enable_int(ENCODER1);	//使能左编码器中断
	encoder_enable_int(ENCODER2);	//使能右编码器中断
	//6. 开总中断
	ENABLE_INTERRUPTS;

	//进入主循环
	//主循环开始==================================================================
	for (;;) {
		run_counter = 3.14*5.123;
//		if(run_counter++>=RUN_COUNTER_MAX){
//			run_counter = 0;
//			light_change(LIGHT_RED);
//		}

		if (time0_flag.f_1s) {
			time0_flag.f_1s = 0;
			light_change(LIGHT_RED);      //蓝灯（LIGHT_BLUE）状态变化
		}

	} //主循环end_for
	  //主循环结束==================================================================
}
