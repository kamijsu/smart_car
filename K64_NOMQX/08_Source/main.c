//说明见工程文件夹下的Doc文件夹内Readme.txt文件
//============================================================================

#define GLOBLE_VAR  //只需在main.c中定义一次，用来防止全局变量的重复定义

#include "includes.h"   //包含总头文件

int main(void) {
	//1. 声明主函数使用的变量
	uint32 run_counter;
	uint16 send;
	float temp;

	int16 test;

	/* 小车相关参数变量 */
	Car car;

	/*串口调试工具用临时变量*/
	float data_out[4];

	//2. 关总中断
	DISABLE_INTERRUPTS;

	//3. 初始化外设模块
	light_init(LIGHT_BLUE, LIGHT_OFF); //蓝灯初始化
	//light_init(LIGHT_BLUE, LIGHT_OFF);
	uart_init(UART_USE, 9600, UART_PARITY_DISABLED, UART_STOP_BIT_1); //uart1初始化，蓝牙用，蓝牙模块波特率9600，无法在5ms中断中传输数据
//	uart_init(UART_USE, 115200);   //uart1初始化，串口用
	pit_init(PIT_CH0, 5);  //pit0初始化，周期5ms
	motor_init(MOTOR0);			//左电机初始化
	motor_init(MOTOR1);			//右电机初始化
//	gyro_acce_init();			//陀螺仪加速度计初始化
//	encoder_init(ENCODER1);		//左编码器初始化
//	encoder_init(ENCODER2);		//右编码器初始化
//	ems_init();					//电磁传感器初始化
	reed_switch_init();			//干簧管初始化
//ftm_init(FTM_MOD0,FTM_CLK_DIV_128,FTM_COUNTER_MODE_UP_DOWN,100000);
//
//ftm_pwm_single_init(FTM_MOD0,FTM_CH5,FTM_PWM_MODE_CENTER_ALIGNED,FTM_PWM_POL_NEGATIVE,5000);
	temp_sensor_init();

	//4. 给有关变量赋初值
	run_counter = 0;
	test = 0xF0;

	//5. 使能模块中断
	pit_enable_int(PIT_CH0);   		//使能pit中断
	uart_enable_re_int(UART_USE);   //使能uart1接收中断
	reed_switch_enable_int();
//	encoder_enable_int(ENCODER1);	//使能左编码器中断
//	encoder_enable_int(ENCODER2);	//使能右编码器中断

	uart_send_string(UART_USE,"test！\n");

	//6. 开总中断
	ENABLE_INTERRUPTS;

	//进入主循环
	//主循环开始==================================================================
	for (;;) {
		motor_set(MOTOR0, 900);
		motor_set(MOTOR1, 900);
		if (time0_flag.f_1s) {
//			uart_send_string(UART_USE, "中文测试");
			time0_flag.f_1s = 0;
			light_change(LIGHT_BLUE);
			temp = (float)test;
			printf("%f\n",temp);
//			temp = temp_sensor_get();
//			send = (uint16) (temp * 1000);
//			uart_send1(UART_USE, send >> 8);
//			uart_send1(UART_USE, send);
		}

	} //主循环end_for
	  //主循环结束==================================================================
}
