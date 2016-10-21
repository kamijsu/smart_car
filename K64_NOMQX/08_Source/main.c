//说明见工程文件夹下的Doc文件夹内Readme.txt文件
//============================================================================

#define GLOBLE_VAR  //只需在main.c中定义一次，用来防止全局变量的重复定义

#include "includes.h"   //包含总头文件

int main(void)
{
	//1. 声明主函数使用的变量
	uint32_t run_counter;     //主循环计数器

	/* 小车相关参数变量 */
	Car car;

	/*串口调试工具用临时变量*/
	float data_out[4];

	//2. 关总中断
	DISABLE_INTERRUPTS;

	//3. 初始化外设模块
	light_init(LIGHT_BLUE, LIGHT_ON); //蓝灯初始化
	uart_init(UART_USE, 9600);     //uart1初始化，蓝牙用，蓝牙模块波特率9600，无法在5ms中断中传输数据
//	uart_init(UART_USE, 115200);   //uart1初始化，串口用
	pit_init(U_PIT0, BUS_CLK_KHZ * 5);  //pit0初始化，周期5ms
	motor_init(MOTOR1);			//左电机初始化
	motor_init(MOTOR2);			//右电机初始化
	gyro_acce_init();			//陀螺仪加速度计初始化
	encoder_init(ENCODER1);		//左编码器初始化
	encoder_init(ENCODER2);		//右编码器初始化
	ems_init();					//电磁传感器初始化
	reed_switch_init();			//干簧管初始化

	//4. 给有关变量赋初值
	run_counter = 0;			//小灯计数器置0
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
	pit_enable_int(U_PIT0);   		//使能pit中断
	uart_enable_re_int(UART_USE);   //使能uart1接收中断
	encoder_enable_int(ENCODER1);	//使能左编码器中断
	encoder_enable_int(ENCODER2);	//使能右编码器中断

	//6. 开总中断
	ENABLE_INTERRUPTS;

	//进入主循环
	//主循环开始==================================================================
	for (;;)
	{

		run_counter++;
		if (run_counter > RUN_COUNTER_MAX)
		{
			run_counter = 0;		   //主循环次数计数器清零
			light_change(LIGHT_BLUE);      //蓝灯（LIGHT_BLUE）状态变化
		}

		//小车无需停车时执行下列程序
		if (!car_flag.over_speed && !car_flag.reed_switch)
		{

			/******************
			 角度控制模块
			 *****************/
			if (time0_flag.f_5ms == 1)    // 5ms IF里面语句执行一次
			{
				time0_flag.f_5ms = 0;

				//陀螺仪加速度传感器AD采集
				gyro_acce_getAD(&car.angle.angle_speed_AD,
						&car.angle.acce_speed_AD);

				//向串口发送角速度以及加速度的AD值,用于获取陀螺仪加速度传感器初始状态AD值
//				uart_send1(UART_USE,0x0);
//				uart_send1(UART_USE,car.angle.angle_speed_AD >> 8);
//				uart_send1(UART_USE,car.angle.angle_speed_AD);
//				uart_send1(UART_USE,car.angle.acce_speed_AD >> 8);
//				uart_send1(UART_USE,car.angle.acce_speed_AD);
//				uart_send1(UART_USE,0xff);

				//陀螺仪加速度传感器AD值回归物理值,角速度单位为°/s,加速度单位为g
				gyro_acce_AD_to_phy(car.angle.angle_speed_AD,
						car.angle.acce_speed_AD, &car.angle.angle_speed_temp,
						&car.angle.acce_speed_temp, car.angle.acce_set);

				//向串口发送角速度以及加速度的物理值,角速度单位为°/s,加速度单位为m/s^2
//				uart_send1(UART_USE,(int)car.angle.angle_speed_temp);
//				uart_send1(UART_USE,(int)(car.angle.acce_speed_temp * 9.8));

				//通过卡尔曼滤波得到车子的倾角与角速度,单位为°和°/s
				gyro_acce_phy_to_angle(car.angle.angle_speed_temp,
						car.angle.acce_speed_temp, &car.angle.angle,
						&car.angle.angle_speed);

				//向串口调试工具发送滤波前后角度与角速度，用于调试滤波效果
//				data_out[0] = car.angle.angle * 1000;							//滤波后倾斜角度*1000
//				data_out[1] = car.angle.angle_speed * 1000;						//滤波后角速度*1000
//				data_out[2] = asin(car.angle.acce_speed_temp) * 57.296 * 1000;	//滤波前倾斜角度*1000
//				data_out[3] = car.angle.angle_speed_temp * 1000;				//滤波前角速度*1000
//				visual_scope_output(UART_USE, data_out);

				//计算新的角度控制模块对电机PWM值
				car.angle.pwm.new_PWM = control_angle(car.angle.angle,
						car.angle.angle_speed, car.angle.pid.p, car.angle.pid.i,
						car.angle.pid.d);

				//角度控制模块平滑周期计数器清零
				car.angle.period_counter = 0;

				//角度控制模块平滑输出PWM值
				++car.angle.period_counter;
				car.angle.pwm.output_PWM = control_angle_PWM_out(
						car.angle.pwm.new_PWM, car.angle.pwm.last_PWM,
						car.angle.period_counter);
				car.angle.pwm.last_PWM = car.angle.pwm.output_PWM;

				//速度控制模块平滑输出PWM值
				++car.speed.period_counter;
				car.speed.pwm.output_PWM = control_speed_PWM_out(
						car.speed.pwm.new_PWM, car.speed.pwm.last_PWM,
						car.speed.period_counter);
				car.speed.pwm.last_PWM = car.speed.pwm.output_PWM;

				//转向控制模块平滑输出PWM值
				++car.turn.period_counter;
				car.turn.pwm.output_PWM = control_turn_PWM_out(
						car.turn.pwm.new_PWM, car.turn.pwm.last_PWM,
						car.turn.period_counter);
				car.turn.pwm.last_PWM = car.turn.pwm.output_PWM;

				//根据角度、速度、转向控制模块对电机输出PWM值
				car.output_PWM = control_motor_output(car.angle.pwm.output_PWM,
						car.speed.pwm.output_PWM, car.turn.pwm.output_PWM);

				//向串口发送各PWM值，用于查看PID参数调整效果
//				data_out[0] = (float)(car.angle.pwm.output_PWM);	//角度控制模块对电机输出PWM值
//				data_out[1] = (float)(car.speed.pwm.output_PWM);	//速度控制模块对电机输出PWM值
//				data_out[2] = (float)(car.turn.pwm.output_PWM);		//转向控制模块对电机输出PWM值
//				data_out[3] = (float)(car.output_PWM);				//各模块对电机输出PWM复合值
//				visual_scope_output(UART_USE, data_out);
			}

			/******************
			 转向控制模块
			 *****************/
			if (time0_flag.f_20ms == 1) // 20mS IF里面语句执行一次
			{
				time0_flag.f_20ms = 0;

				//采集传感器数据，单位V
				ems_get_phy(car.turn.EMS_V);

				//向串口发送传感器电压值，用于查看电磁传感器采集效果
//				data_out[0] = car.turn.EMS_V[0] *1000;	//左外电磁传感器 * 1000
//				data_out[1] = car.turn.EMS_V[2] *1000;	//左内电磁传感器 * 1000
//				data_out[2] = car.turn.EMS_V[3] *1000;	//右内电磁传感器 * 1000
//				data_out[3] = car.turn.EMS_V[1] *1000;	//右外电磁传感器 * 1000
//				visual_scope_output(UART_USE, data_out);

				//转向控制模块平滑周期计数器清零
				car.turn.period_counter = 0;

				//记录上次转向控制PWM值
				car.turn.pwm.last_PWM = car.turn.pwm.new_PWM;

				//计算新的转向控制模块对电机PWM值
				car.turn.pwm.new_PWM = control_turn(car.turn.EMS_V,
						car.turn.pid.p, car.turn.pid.i, car.turn.pid.d);

				//定义临时变量以向串口发送数据查看转向模块控制情况
//				float error_out, error_in;
//				error_out = (car.turn.EMS_V[1] - car.turn.EMS_V[0]) / (car.turn.EMS_V[0] + car.turn.EMS_V[1]);
//				error_in = (car.turn.EMS_V[3] - car.turn.EMS_V[2]) / (car.turn.EMS_V[2] + car.turn.EMS_V[3]);
//
//				data_out[0] = error_out * 1000;			//外电磁传感器比例电压差 * 1000
//				data_out[1] = error_in * 1000;			//内电磁传感器比例电压差 * 1000
//				data_out[2] = car.turn.pwm.new_PWM;		//新的转向控制模块对电机PWM值
//				data_out[3] = car.turn.pwm.output_PWM;	//转向控制模块实际对电机PWM值
//				visual_scope_output(UART_USE, data_out);
			}

			/******************
			 速度控制模块
			 *****************/
			if (time0_flag.f_50ms == 1)
			{
				time0_flag.f_50ms = 0;

				//获取左右轮速度，单位m/s
				encoder_get_speed(encoder_counter.left, &car.speed.left_speed);
				encoder_get_speed(encoder_counter.right,
						&car.speed.right_speed);

				//编码器脉冲计数器清零
				encoder_counter.left = 0;
				encoder_counter.right = 0;

				//计算平均速度
				car.speed.avg_speed = (car.speed.left_speed
						+ car.speed.right_speed) / 2;

				//向串口调试工具发送左右轮速度
//				data_out[0] = car.speed.left_speed * 1000;	//左轮速度 * 1000
//				data_out[1] = car.speed.right_speed * 1000;	//右轮速度 * 1000
//				data_out[2] = car.speed.avg_speed * 1000;	//平均速度 * 1000
//				visual_scope_output(UART_USE, data_out);

				//速度保护
//				if (car.speed.avg_speed > 2.5)
//				{
//					car_flag.over_speed = 1;
//				}

				//速度控制模块平滑周期计数器清零
				car.speed.period_counter = 0;

				//记录上次速度控制PWM值
				car.speed.pwm.last_PWM = car.speed.pwm.new_PWM;

				//计算新的速度控制模块对电机PWM值
				car.speed.pwm.new_PWM = control_speed(car.speed.avg_speed,
						car.speed.aim_speed, car.speed.pid.p, car.speed.pid.i,
						car.speed.pid.d);

				//向串口发送数据查看速度模块控制情况
//				data_out[0] = (float) (car.angle.pwm.output_PWM);//角度控制模块对电机输出PWM值
//				data_out[1] = (float) (car.speed.pwm.output_PWM);//速度控制模块对电机输出PWM值
//				data_out[2] = car.speed.avg_speed * 10000;		//平均速度 * 10000
//				data_out[3] = (float) (car_flag.can_stop * 10000);//0为不能停车，10000为可以停车
//				visual_scope_output(UART_USE, data_out);

				//向串口发送各PWM值，用于查看PID参数调整效果
//				data_out[0] = (float)(car.angle.pwm.output_PWM);	//角度控制模块对电机输出PWM值
//				data_out[1] = (float)(car.speed.pwm.output_PWM);	//速度控制模块对电机输出PWM值
//				data_out[2] = (float)(car.turn.pwm.output_PWM);		//转向控制模块对电机输出PWM值
//				data_out[3] = (float)(car.output_PWM);				//各模块对电机输出PWM复合值
//				visual_scope_output(UART_USE, data_out);

			}

			/******************
			 起步换速
			 *****************/
			if(time0_flag.f_5s == 1)
			{
				time0_flag.f_5s = 0;

				if(car_flag.is_starting)
				{
					car_flag.is_starting = 0;

					/* 0.4m/s时PID参数 */
//					car.speed.aim_speed = 0.4;		//目标速度初始化，单位m/s
//					car.angle.acce_set -= 0x20;		//0.4m/s时平衡位置
//					car.angle.pid.p = 0.2;
//					car.angle.pid.i = 0;
//					car.angle.pid.d = 0.001;
//
//					car.speed.pid.p = 1.5;
//					car.speed.pid.i = 0.1;
//					car.speed.pid.d = 0;
//
//					car.turn.pid.p = 0.125;
//					car.turn.pid.i = 0;
//					car.turn.pid.d = 0.02;

					/* 0.8m/s时PID参数 */
//					car.speed.aim_speed = 0.8;		//目标速度初始化，单位m/s
//					car.angle.acce_set -= 0x24;		//0.8m/s时平衡位置
//					car.angle.pid.p = 0.2;
//					car.angle.pid.i = 0;
//					car.angle.pid.d = 0.001;
//
//					car.speed.pid.p = 1.0;
//					car.speed.pid.i = 0.05;
//					car.speed.pid.d = 0;
//
//					car.turn.pid.p = 0.22;
//					car.turn.pid.i = 0;
//					car.turn.pid.d = 0.04;

					/* 1.0m/s时PID参数 */
					car.speed.aim_speed = 1.0;		//目标速度初始化，单位m/s
					car.angle.acce_set -= 0x24;		//1.0m/s时平衡位置
					car.angle.pid.p = 0.2;
					car.angle.pid.i = 0;
					car.angle.pid.d = 0.001;

					car.speed.pid.p = 0.8;
					car.speed.pid.i = 0.05;
					car.speed.pid.d = 0;

					car.turn.pid.p = 0.25;
					car.turn.pid.i = 0;
					car.turn.pid.d = 0.04;

					/* 1.2m/s时PID参数 */
//					car.speed.aim_speed = 1.2;		//目标速度初始化，单位m/s
//					car.angle.acce_set -= 0x26;		//1.2m/s时平衡位置
//					car.angle.pid.p = 0.2;
//					car.angle.pid.i = 0;
//					car.angle.pid.d = 0.001;
//
//					car.speed.pid.p = 0.8;
//					car.speed.pid.i = 0.05;
//					car.speed.pid.d = 0;
//
//					car.turn.pid.p = 0.4;
//					car.turn.pid.i = 0;
//					car.turn.pid.d = 0.06;
				}

			}

			/******************
			 停车使能
			 *****************/
			if (time0_flag.f_15s == 1)
			{
				time0_flag.f_15s = 0;

				if(!car_flag.can_stop)
				{
					car_flag.can_stop = 1;
					reed_switch_enable_int();
				}
			}

		}
		//停车
		else
		{
			control_stop();
		}

	} //主循环end_for
	  //主循环结束==================================================================
}
