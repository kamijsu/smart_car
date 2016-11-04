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
//	gyro_acce_init();			//陀螺仪加速度计初始化
//	encoder_init(ENCODER1);		//左编码器初始化
//	encoder_init(ENCODER2);		//右编码器初始化
//	ems_init();					//电磁传感器初始化
//	reed_switch_init();			//干簧管初始化
	ftm_init(FTM_MOD1,FTM_CLK_DIV_128,FTM_COUNTER_MODE_UP,100);

	ftm_init(FTM_MOD0,FTM_CLK_DIV_128,FTM_COUNTER_MODE_UP,100);
//ftm_pwm_combine_init(FTM_MOD0,FTM_CH_GROUP2,FTM_PWM_MODE_COMBINE,FTM_PWM_POL_NEGATIVE,9998,10000);
	//ftm_pwm_single_init(FTM_MOD0,FTM_CH4,FTM_PWM_MODE_CENTER_ALIGNED,FTM_PWM_POL_NEGATIVE,5000);
	ftm_pwm_single_init(FTM_MOD0,FTM_CH4,FTM_PWM_MODE_EDGE_ALIGNED,FTM_PWM_POL_NEGATIVE,5000);
//ftm_pwm_single_set(FTM_MOD0,FTM_CH4,2);
//ftm_ic_init(FTM_MOD0,FTM_CH1,FTM_CAPTURE_MODE_DOUBLE_EDGE);
//
	ftm_oc_init(FTM_MOD0,FTM_CH0,FTM_OC_MODE_SET,9999);


	ftm_ic_init(FTM_MOD0,FTM_CH0,FTM_IC_MODE_DOUBLE_EDGE);

	//4. 给有关变量赋初值
	run_counter = 0;



	//5. 使能模块中断
//	ftm_ch_enable_int(FTM_MOD0,FTM_CH1);
//	ftm_ch_enable_int(FTM_MOD0,FTM_CH0);

//gpio_pull(COM_PORTC|3,GPIO_LEVEL_LOW);
//gpio_pull(COM_PORTC|4,GPIO_LEVEL_LOW);
		ftm_decap_enable_int(FTM_MOD1,FTM_CH_GROUP0);

	//pit_enable_int(PIT_CH0);   		//使能pit中断
	uart_enable_re_int(UART_USE);   //使能uart1接收中断
	encoder_enable_int(ENCODER1);	//使能左编码器中断
	encoder_enable_int(ENCODER2);	//使能右编码器中断
	//ftm_timer_enable_int(FTM_MOD1,1);
	//6. 开总中断
	ENABLE_INTERRUPTS;
	ftm_decap_reenable(FTM_MOD1,FTM_CH_GROUP0);

	//进入主循环
	//主循环开始==================================================================
	for (;;) {

		//run_counter = 3.14*5.123;
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
