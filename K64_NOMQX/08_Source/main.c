//说明见工程文件夹下的Doc文件夹内Readme.txt文件
//============================================================================

#define GLOBLE_VAR  //只需在main.c中定义一次，用来防止全局变量的重复定义

#include "includes.h"   //包含总头文件

//定义内部使用宏
#define ABS(x) ((x) > 0 ? (x) : -(x))

int main(void) {
	//1. 声明主函数使用的变量
	uint32 start, end;	//开始和结束时间值变量
	FrameInfo info;		//帧信息结构体
	uint8 frame[263];	//帧字节
	uint16 frame_len;	//帧字节长度
	bool can_send_param;	//可以发送参数标志
	bool can_send_img;		//可以发送图片标志
	bool img_copy_done;		//图片拷贝完成标志
	bool stop_car;	//停车标志
	bool started;	//启动标志
	uint8 raw_img_copy[CAMERA_RAW_IMG_BYTES];	//拷贝原始图片
	int16 mid_points_copy[CAMERA_IMG_HEIGHT];	//拷贝中点
	int16 left_edges_copy[CAMERA_IMG_HEIGHT];	//拷贝左边缘
	int16 right_edges_copy[CAMERA_IMG_HEIGHT];	//拷贝右边缘
	bool valid_row_copy[CAMERA_IMG_HEIGHT];		//拷贝有效行
	ParamCar car;	//小车参数

	//2. 关总中断
	DISABLE_INTERRUPTS;

	//3. 初始化外设模块
	light_init(LIGHT_BLUE, LIGHT_ON); //蓝灯初始化
	uart_init(UART_USE, 115200, UART_PARITY_DISABLED, UART_STOP_BIT_1,
	UART_BIT_ORDER_LSB); //UART0初始化

	gyro_acce_init();	//初始化陀螺仪加速度传感器

	oled_init();		//初始化液晶屏

	pit_init(PIT_CH0, 5);  //PIT0初始化，周期5ms
	pit_init(PIT_CH1, 71582);	//PIT1初始化，做为调试用

	keyboard_init();	//初始化键盘
	param_init(&car);	//初始化参数
	param_get(&car, 1);	//获取第1套参数，保持平衡参数
	//初始化菜单显示，根据输入的mode，选择读入存放在相应flash位置的参数
	menu_init(&car);

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
	time_register_timer(4, 100);
	time_register_timer(5, 5000);

	//使能组帧通信协议
	frame_init(0);

	//4. 给有关变量赋初值
	raw_img_done = false;
	can_send_param = false;
	can_send_img = false;
	img_copy_done = false;
	stop_car = false;
	started = false;

	//0m/s时pid参数
//	car.angle.target_angle = 12.5f;
//	car.angle.pid.p = 0.15f;
//	car.angle.pid.d = 0.0003f;
//
//	car.speed.target_speed = 0.0f;
//	car.speed.pid.p = 3;
//	car.speed.pid.i = 0.1f;
//	car.speed.pid.d = 0;
//
//	car.turn.pid.p = 0.003f;
//	car.turn.pid.d = 0.008f;
//	car.turn.target_mid_point = 42;

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

//			start = pit_get_time_us(1);

			//寻找中点
			control_find_mid_points(&car.turn);

			//计算加权平均值
			control_cal_avg_mid_point(&car.turn);

//			end = pit_get_time_us(1);

			//更新方向控制模块的目标PWM值
			control_turn_pid(&car.turn);

			//显示图像
			if (menu_can_show_img()) {
				oled_printf(0, 6, "%.2f", car.angle.angle);
				oled_printf(0, 0, "%.2f", car.turn.avg_mid_point);
//				oled_printf(0, 2, "%d", end - start);
				custom_oled_show_img(car.turn.img);
			}

			//拷贝原始图像，以供发送
			if (can_send_img && !img_copy_done) {
				img_copy_done = true;
				memcpy(raw_img_copy, car.turn.raw_img,
						sizeof(uint8) * CAMERA_RAW_IMG_BYTES);
				memcpy(mid_points_copy, car.turn.mid_points,
						sizeof(int16) * CAMERA_IMG_HEIGHT);
				memcpy(left_edges_copy, car.turn.left_edges,
						sizeof(int16) * CAMERA_IMG_HEIGHT);
				memcpy(right_edges_copy, car.turn.right_edges,
						sizeof(int16) * CAMERA_IMG_HEIGHT);
				memcpy(valid_row_copy, car.turn.valid_row,
						sizeof(bool) * CAMERA_IMG_HEIGHT);
			}

			//发送图像至PC
//			vcan_send_raw_img(car.turn.raw_img);	//52ms

			//重置图像完成标志
			raw_img_done = false;
			//允许接收场中断
			camera_enable_vsync_int();
		}

		//到达5ms
		if (time_is_timer_arrived(0)) {
			//重新开始计时
			time_restart_timer(0);

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

			//要停车的话，停车，否则更新PWM值
			if (stop_car) {
				//停车
				control_stop_car();
			} else {
				//更新电机输出的PWM值
				control_update_motor_pwm(&car);
			}

		}

		//到达50ms
		if (time_is_timer_arrived(1)) {
			//重新开始计时
			time_restart_timer(1);

			//获取左右轮速度
			car.speed.left_speed = encoder_get_speed(ENCODER0);
			car.speed.right_speed = encoder_get_speed(ENCODER1);

			//速度保护，若速度过高，停车
			if (ABS(car.speed.left_speed) + ABS(car.speed.right_speed) >= 10) {
				stop_car = true;
			}

			//更新速度控制模块的目标PWM值
			control_speed_pid(&car.speed);

//			oled_printf(0, 4, "speed:%4.2f", (car.speed.left_speed+car.speed.right_speed)/2);

		}

		//到达1s
		if (time_is_timer_arrived(3)) {
			//重新开始计时
			time_restart_timer(3);
			//小灯闪烁
			light_change(LIGHT_BLUE);
		}

		//到达100ms
		if (time_is_timer_arrived(4)) {
			//重新开始计时
			time_restart_timer(4);

			//若可以发送参数，则发送至上位机
			if (can_send_param) {
				//7ms
				custom_send_param_to_host(&car);
			}
			//若可以发送图像，则发送至上位机
			if (can_send_img && img_copy_done) {
				img_copy_done = false;
				custom_send_mid_points_to_host(mid_points_copy, valid_row_copy);
				custom_send_edges_to_host(left_edges_copy, right_edges_copy,
						valid_row_copy);
				//54ms
				custom_send_raw_img_to_host(raw_img_copy);
			}

		}

		//到达5s
		if (!started && time_is_timer_arrived(5)) {
			//更换参数，启动小车
			param_get(&car, 2);
			started = true;
		}

		if (frame_get_info(0, &info)) {
			switch (info.type) {
			case 0:	//字符串帧
				frame_info_to_frame(&info, frame, &frame_len);
				uart_sendN(UART_USE, frame, frame_len);
				break;
			case 7:	//设置是否发送图像
				can_send_img = info.data[0] == 1 ? true : false;
				break;
			case 8:	//设置是否发送参数
				can_send_param = info.data[0] == 1 ? true : false;
				break;
			case 9:	//设置是否停车
				stop_car = info.data[0] == 1 ? true : false;
				break;
			}
		}

	} //主循环end_for
//主循环结束==================================================================
}
