//说明见工程文件夹下的Doc文件夹内Readme.txt文件
//============================================================================

#define GLOBLE_VAR  //只需在main.c中定义一次，用来防止全局变量的重复定义

#include "includes.h"   //包含总头文件

int main(void) {
	//1. 声明主函数使用的变量
	uint32 start, end;
	uint32 i, j;
	uint8 raw_img[CAMERA_RAW_IMG_BYTES];
	uint8 img[CAMERA_IMG_HEIGHT][CAMERA_IMG_WIDTH];
	int32 duty = -2000;

	uint8 src1[10240];
	uint8 src[10240];
	for (i = 0; i < 10240; i++) {
		src[i] = i % 256;
	}
	uint8 dest1[10240];
	uint8 dest[10240];
	memset(dest, 0xFF, 10240);

	//2. 关总中断
	DISABLE_INTERRUPTS;

	//3. 初始化外设模块
	light_init(LIGHT_BLUE, LIGHT_ON); //蓝灯初始化
	uart_init(UART_USE, 115200, UART_PARITY_DISABLED, UART_STOP_BIT_1,
	UART_BIT_ORDER_LSB); //uart0初始化

	pit_init(PIT_CH0, 5);  //pit0初始化，周期5ms
	pit_init(PIT_CH1, 71582);
	rng_init();

//	temp_sensor_init();

	oled_init();

//	menu_oled_display();
//	custom_oled_display_init();

	camera_init(raw_img);
	motor_init();
	motor_set(0,duty);
	motor_set(1,duty);

	//4. 给有关变量赋初值
	time0_flag.f_1s = 0;
	time0_flag.f_50ms = 0;
	raw_img_done = false;

	//5. 使能模块中断
	pit_enable_int(PIT_CH0);   		//使能pit中断
	uart_enable_re_int(UART_USE);   //使能uart0接收中断
	camera_enable_collect_done_int();
	camera_enable_vsync_int();

	//6. 开总中断
	ENABLE_INTERRUPTS;

	//进入主循环
	//主循环开始==================================================================
	for (;;) {
		if (raw_img_done && time0_flag.f_50ms) {
			time0_flag.f_50ms = 0;

			camera_extract_raw_img(raw_img, (uint8*) img);

			custom_oled_show_img(img);

			vcan_send_raw_img(raw_img);

			raw_img_done = false;
			camera_enable_vsync_int();
		}
		if (time0_flag.f_1s) {
			time0_flag.f_1s = 0;
			light_change(LIGHT_BLUE);

			start = pit_get_time_us(1);
//			duty*=-1;
//			motor_set(0,duty);
//				motor_set(1,duty);
//			custom_oled_update_temp();

			end = pit_get_time_us(1);
//			uart_printf(UART_USE, "消耗时间：%dus\r\n", end - start);

		}

	} //主循环end_for
//主循环结束==================================================================
}
