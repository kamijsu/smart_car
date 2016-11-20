//说明见工程文件夹下的Doc文件夹内Readme.txt文件
//============================================================================

#define GLOBLE_VAR  //只需在main.c中定义一次，用来防止全局变量的重复定义

#include "includes.h"   //包含总头文件

int main(void) {
	//1. 声明主函数使用的变量
	uint32 run_counter;
	int16 count;
	uint16 temp;

	/* 小车相关参数变量 */
	Car car;

	/*串口调试工具用临时变量*/
	float data_out[4];

	//2. 关总中断
	DISABLE_INTERRUPTS;

	//3. 初始化外设模块
	light_init(LIGHT_RED, LIGHT_OFF); //蓝灯初始化
	//light_init(LIGHT_BLUE, LIGHT_OFF);
	uart_init(UART_USE, 9600, UART_PARITY_DISABLED, UART_STOP_BIT_1); //uart1初始化，蓝牙用，蓝牙模块波特率9600，无法在5ms中断中传输数据
//	uart_init(UART_USE, 115200);   //uart1初始化，串口用
	pit_init(PIT_CH0, 10000);  //pit0初始化，周期5ms
//	motor_init(MOTOR1);			//左电机初始化
//	motor_init(MOTOR2);			//右电机初始化
//	gyro_acce_init();			//陀螺仪加速度计初始化
//	encoder_init(ENCODER1);		//左编码器初始化
//	encoder_init(ENCODER2);		//右编码器初始化
//	ems_init();					//电磁传感器初始化
//	reed_switch_init();			//干簧管初始化

	adc_init(ADC_MOD0, ADC_CLK_DIV_16, ADC_ACCURACY_SINGLE_8_DIFF_9,
			ADC_HARDWARE_AVG_DISABLE, ADC_ADLSTS_DISABLE, ADC_ADHSC_NORMAL,
			ADC_CAL_ENABLE);

	//4. 给有关变量赋初值
	run_counter = 0;

	//5. 使能模块中断
	pit_enable_int(PIT_CH0);   		//使能pit中断
	uart_enable_re_int(UART_USE);   //使能uart1接收中断
//	encoder_enable_int(ENCODER1);	//使能左编码器中断
//	encoder_enable_int(ENCODER2);	//使能右编码器中断

	//6. 开总中断
	ENABLE_INTERRUPTS;

	//进入主循环
	//主循环开始==================================================================
	for (;;) {

		adc_single_get(ADC_MOD0,ADC_SE9,ADC_SE_SEL_A);
		run_counter++;

		if (time0_flag.f_10s) {
			time0_flag.f_10s = 0;
			uart_send1(UART_USE, run_counter >> 24);
			uart_send1(UART_USE, run_counter >> 16);
			uart_send1(UART_USE, run_counter >> 8);
			uart_send1(UART_USE, run_counter);
			run_counter=0;
		}

	} //主循环end_for
	  //主循环结束==================================================================
}
