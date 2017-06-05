//说明见工程文件夹下的Doc文件夹内Readme.txt文件
//============================================================================

#define GLOBLE_VAR  //只需在main.c中定义一次，用来防止全局变量的重复定义

#include "includes.h"   //包含总头文件

int main(void) {
	//1. 声明主函数使用的变量

	uint32 start, end;	//开始和结束时间值变量
	uint32 i, j;
	int16 returnBuff[6];

	ControlCar car;	//小车参数

	//2. 关总中断
	DISABLE_INTERRUPTS;

	//3. 初始化外设模块
	light_init(LIGHT_BLUE, LIGHT_ON); //蓝灯初始化
	uart_init(UART_USE, 115200, UART_PARITY_DISABLED, UART_STOP_BIT_1,
	UART_BIT_ORDER_LSB); //UART0初始化

	//rng_init();

	temp_sensor_init();	//初始化温度传感器

	gyro_acce_init();	//初始化陀螺仪加速度传感器

	oled_init();		//初始化液晶屏

	pit_init(PIT_CH0, 5);  //PIT0初始化，周期5ms
	pit_init(PIT_CH1, 71582);	//PIT1初始化，做为调试用

	keyboard_init();	//初始化键盘

//	menu_oled_display();
//	custom_oled_display_init();

	encoder_init(ENCODER0);		//初始化左编码器
	encoder_init(ENCODER1);		//初始化右编码器

	camera_init(car.turn.raw_img);	//初始化摄像头

	motor_init();		//初始化电机

	time_init(0);		//初始化时间模块
	//注册各个时间通道
	time_register_timer(0, 5);
	time_register_timer(1, 50);
	time_register_timer(2, 50);
	time_register_timer(3, 1000);

	//4. 给有关变量赋初值
	raw_img_done = false;
	//初始化小车参数
	car.angle.angle = 0;
	car.angle.last_angle_speed = 0;
	car.angle.pwm.target_pwm = 0;
	car.angle.pwm.output_pwm = 0;
	car.angle.pwm.period_num = 1;
	car.speed.last_speed_err = 0;
	car.speed.distance_err = 0;
	car.speed.pwm.target_pwm = 0;
	car.speed.pwm.output_pwm = 0;
	car.speed.pwm.period_num = 10;
	car.turn.pwm.target_pwm = 0;
	car.turn.pwm.output_pwm = 0;
	car.turn.pwm.period_num = 5;
	car.turn.last_mid_err = 0;

	//0m/s时pid参数
	car.angle.target_angle = 1.5f;
	car.angle.pid.p = 0.2f;
	car.angle.pid.d = 0.0003f;

	car.speed.target_speed = 0.3;
	car.speed.pid.p = 2;
	car.speed.pid.i = 0.5f;
	car.speed.pid.d = 0;

	car.turn.pid.p = 0.0010f;
	car.turn.pid.d = 0.0002f;

	//5. 使能模块中断
	pit_enable_int(PIT_CH0);   		//使能PIT0中断
	uart_enable_re_int(UART_USE);   //使能UART0接收中断
	keyboard_enable_int();			//使能键盘中断
	camera_enable_collect_done_int();	//使能原始图像采集完毕中断
	camera_enable_vsync_int();		//使能场中断

	//6. 开总中断
	ENABLE_INTERRUPTS;

	//进入主循环
	//主循环开始==================================================================
	for (;;) {
		//图像采集完毕且到达50ms
		if (raw_img_done && time_is_timer_arrived(2)) {
			//重新开始计时
			time_restart_timer(2);
			//解压原始图像
			camera_extract_raw_img(car.turn.raw_img, (uint8*) car.turn.img);

			deal_image(car.turn.img, car.turn.returnBuff);

			get_image_midpoint(car.turn.returnBuff, &car.turn.midpoint);

			control_turn_pid(&car.turn);
//			oled_printf(0, 0, "%d", car.turn.returnBuff[0]);
//			oled_printf(0, 2, "%d", car.turn.returnBuff[1]);
//			oled_printf(0, 4, "%d", car.turn.returnBuff[2]);
			oled_printf(0, 6, "%4.1f", car.turn.midpoint);

			//显示图像
			custom_oled_show_img(car.turn.img);
			//发送图像至PC
//			vcan_send_raw_img(raw_img);
			//重置图像完成标志
			raw_img_done = false;
			//允许接收场中断
			camera_enable_vsync_int();
		}
		//到达5ms
		if (time_is_timer_arrived(0)) {
			//重新开始计时
			time_restart_timer(0);

			start = pit_get_time_us(1);

			//获取角度和角速度
			gyro_acce_get_phy(&car.angle.angle, &car.angle.angle_speed);

			//更新角度控制模块的目标PWM值
			control_angle_pid(&car.angle);

			//更新角度控制模块的输出PWM值
			control_update_output_pwm(&car.angle.pwm);

			//更新速度控制模块的输出PWM值
			control_update_output_pwm(&car.speed.pwm);

			//更新方向控制模块的输出PWM值
			control_update_output_pwm(&car.turn.pwm);

			//更新电机输出的PWM值
			control_update_motor_pwm(&car);

			end = pit_get_time_us(1);

//			oled_printf(0, 2, "%d", end - start);

//			oled_printf(0,0,"agl:%4.2f ",car.angle.angle);
//			oled_printf(0,2,"agl_speed:%4.2f",car.angle.angle_speed);
//			oled_printf(0, 4, "%4.2f  %4.2f  ", car.angle.angle,
//					car.angle.angle_speed);
//			oled_printf(0, 4, "angle_pwm:%d ", car.angle.pwm.output_pwm);
//			oled_printf(0, 6, "speed_pwm:%d  ", car.speed.pwm.output_pwm);
		}
		//到达50ms
		if (time_is_timer_arrived(1)) {
			//重新开始计时
			time_restart_timer(1);

			//获取左右轮速度
			car.speed.left_speed = encoder_get_speed(ENCODER0);
			car.speed.right_speed = encoder_get_speed(ENCODER1);

			//更新速度控制模块的目标PWM值
			control_speed_pid(&car.speed);

//			oled_printf(0, 4, "speed:%4.2f", (car.speed.left_speed+car.speed.right_speed)/2);

		}
		//到达1s
		if (time_is_timer_arrived(3)) {
			//重新开始计时
			time_restart_timer(3);
			light_change(LIGHT_BLUE);

//			custom_oled_update_temp();

		}

	} //主循环end_for
//主循环结束==================================================================
}

////说明见工程文件夹下的Doc文件夹内Readme.txt文件
////============================================================================
//
//#define GLOBLE_VAR  //只需在main.c中定义一次，用来防止全局变量的重复定义
//
//#include "includes.h"   //包含总头文件
//
//int main(void) {
//	//1. 声明主函数使用的变量
//
//	uint8 raw_img[CAMERA_RAW_IMG_BYTES];
//	uint8 img[CAMERA_IMG_HEIGHT][CAMERA_IMG_WIDTH];
//	uint32 start, end;
//	uint32 i, j;
//    i=0;
//	ControlCar car;
//	int16 returnBuff[6];
//	int16 midpoint;
//	float angle, angle_speed;
//	int32 fasong;
//	uint8 ceshi;
//	//2. 关总中断
//	DISABLE_INTERRUPTS;
//
//	//3. 初始化外设模块
//	light_init(LIGHT_BLUE, LIGHT_ON); //蓝灯初始化
//	uart_init(UART_USE, 115200, UART_PARITY_DISABLED, UART_STOP_BIT_1,
//	UART_BIT_ORDER_LSB); //uart0初始化
//	rng_init();
//	temp_sensor_init();
//	gyro_acce_init();
//
//	oled_init();
//
//	pit_init(PIT_CH0, 5);  //pit0初始化，周期5ms
//	pit_init(PIT_CH1, 71582);
//
//	keyboard_init();
//	//menu_oled_display();
//	custom_oled_display_init();
//	encoder_init(ENCODER0);		//初始化左编码器
//	encoder_init(ENCODER1);		//初始化右编码器
//	motor_init();
//	camera_init(raw_img);
//
//	uart_send_string(UART_USE, "camera init ok\n");
////	ceshi=1;
////    uart_printf(UART_USE,"xxx:%d",ceshi);
//
//
//	time_init(0);
//	time_register_timer(0, 5);
//	time_register_timer(1, 50);
//	time_register_timer(2, 50);
//	time_register_timer(3, 1000);
//
//	//4. 给有关变量赋初值
//	raw_img_done = false;
//	car.angle.angle = 0;
//	car.angle.last_angle_speed = 0;
//	car.angle.pwm.target_pwm = 0;
//	car.angle.pwm.output_pwm = 0;
//	car.angle.pwm.period_num = 1;
//	car.speed.last_speed_err = 0;
//	car.speed.distance_err = 0;
//	car.speed.pwm.target_pwm = 0;
//	car.speed.pwm.output_pwm = 0;
//	car.speed.pwm.period_num = 10;
//
//	car.angle.target_angle = 0;
//	car.angle.pid.p = 0.2f;
//	car.angle.pid.d = 0.001f;
//
//	car.speed.target_speed = 0;
//	car.speed.pid.p = 0;
//	car.speed.pid.i = 0;
//	car.speed.pid.d = 0;
//
//	//5. 使能模块中断
//	pit_enable_int(PIT_CH0);   		//使能pit中断
//	uart_enable_re_int(UART_USE);   //使能uart0接收中断
//	//keyboard_enable_int();
//	camera_enable_collect_done_int();
//	camera_enable_vsync_int();
//
//	//6. 开总中断
//	ENABLE_INTERRUPTS;
//	for(i=0;i<60;i++)
//	{
//
//		for(j=0;j<80;j++)
//		{
//			img[i][j]=0;
//		}
//	}
//	//进入主循环
//	//主循环开始==================================================================
//	for (;;) {
//		if (raw_img_done && time_is_timer_arrived(2)) {
//			time_restart_timer(2);
//
//			camera_extract_raw_img(raw_img, (uint8*) img);
//			uart_send_string(UART_USE, "\n");
////			custom_oled_show_img(img);
////			vcan_send_raw_img(raw_img);、
//			//图像处理
//			deal_image(img,returnBuff);
//			uart_printf(UART_USE,"mid_very_near:%d",returnBuff[0]);
//			uart_printf(UART_USE,"mid_near:%d",returnBuff[1]);
//			uart_printf(UART_USE,"mid_far:%d",returnBuff[2]);
//			uart_printf(UART_USE,"\ncount_very_near:%d",returnBuff[3]);
//			uart_printf(UART_USE,"count_near:%d",returnBuff[4]);
//			uart_printf(UART_USE,"count_far:%d\n\n",returnBuff[5]);
//			//获取图像中点，待测试。。。。
//			get_image_midpoint(returnBuff,midpoint);
//			uart_printf(UART_USE,"midpoint:%d\n\n",midpoint);
//            //方向更新控制，待测试。。。
//			//control_turn(int16 midpoint, float turn_p, float turn_i, float turn_d);
//
//			raw_img_done = false;
//			camera_enable_vsync_int();
//		}
//		if (time_is_timer_arrived(0)) {
//			time_restart_timer(0);
//
//			//获取角度和角速度
//			gyro_acce_get_phy(&car.angle.angle, &car.angle.angle_speed);
//
//			//更新角度控制模块的目标PWM值
//			control_angle_pid(&car.angle);
//
//			//更新角度控制模块的输出PWM值
//			control_update_output_pwm(&car.angle.pwm);
//
//			//更新速度控制模块的输出PWM值
//			control_update_output_pwm(&car.speed.pwm);
//
//			//更新电机输出的PWM值
//			control_update_motor_pwm(&car);
//
//
//			//oled_printf(0, 4, "%4.2f  %4.2f", car.angle.angle,
//			//		car.angle.angle_speed);
//			oled_printf(0, 4, "angle_pwm:%d ", car.angle.pwm.output_pwm);
//		}
//		if (time_is_timer_arrived(1)) {
//			time_restart_timer(1);
//
////			//获取左右轮速度
////			car.speed.left_speed = encoder_get_speed(ENCODER0);
////			car.speed.right_speed = encoder_get_speed(ENCODER1);
////
////			//更新速度控制模块的目标PWM值
////			control_speed_pid(&car.speed);
////
////			//oled_printf(0, 2, "speed:%4.2f %4.2f", speed_l, speed_r);
//
//		}
//		if (time_is_timer_arrived(3)) {
//			time_restart_timer(3);
//			light_change(LIGHT_BLUE);
//			oled_printf(0, 2, "l_pwm:%d",i++);
//			//if(i>10)i=0;
//			custom_oled_update_temp();
//
//		}
//
//	} //主循环end_for
////主循环结束==================================================================
//}
