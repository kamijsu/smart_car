//˵���������ļ����µ�Doc�ļ�����Readme.txt�ļ�
//============================================================================

#define GLOBLE_VAR  //ֻ����main.c�ж���һ�Σ�������ֹȫ�ֱ������ظ�����

#include "includes.h"   //������ͷ�ļ�

int main(void) {
	//1. ����������ʹ�õı���

	uint8 raw_img[CAMERA_RAW_IMG_BYTES];
	uint8 img[CAMERA_IMG_HEIGHT][CAMERA_IMG_WIDTH];
	uint32 start, end;
	uint32 i, j;

	ControlCar car;

	float angle, angle_speed;

	//2. �����ж�
	DISABLE_INTERRUPTS;

	//3. ��ʼ������ģ��
	light_init(LIGHT_BLUE, LIGHT_ON); //���Ƴ�ʼ��
	uart_init(UART_USE, 115200, UART_PARITY_DISABLED, UART_STOP_BIT_1,
	UART_BIT_ORDER_LSB); //uart0��ʼ��

	rng_init();

	temp_sensor_init();

	gyro_acce_init();

	oled_init();

	pit_init(PIT_CH0, 5);  //pit0��ʼ��������5ms
	pit_init(PIT_CH1, 71582);

	keyboard_init();

//	menu_oled_display();
	custom_oled_display_init();

	encoder_init(ENCODER0);		//��ʼ���������
	encoder_init(ENCODER1);		//��ʼ���ұ�����

	camera_init(raw_img);

	motor_init();

	time_init(0);
	time_register_timer(0, 5);
	time_register_timer(1, 50);
	time_register_timer(2, 50);
	time_register_timer(3, 1000);

	//4. ���йر�������ֵ
	raw_img_done = false;
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

	car.angle.target_angle = 0;
	car.angle.pid.p = 0.2f;
	car.angle.pid.d = 0.001f;

	car.speed.target_speed = 0;
	car.speed.pid.p = 0;
	car.speed.pid.i = 0;
	car.speed.pid.d = 0;

	//5. ʹ��ģ���ж�
	pit_enable_int(PIT_CH0);   		//ʹ��pit�ж�
	uart_enable_re_int(UART_USE);   //ʹ��uart0�����ж�
	keyboard_enable_int();
	camera_enable_collect_done_int();
	camera_enable_vsync_int();

	//6. �����ж�
	ENABLE_INTERRUPTS;

	//������ѭ��
	//��ѭ����ʼ==================================================================
	for (;;) {
		if (raw_img_done && time_is_timer_arrived(2)) {
			time_restart_timer(2);

			camera_extract_raw_img(raw_img, (uint8*) img);

//			custom_oled_show_img(img);

//			vcan_send_raw_img(raw_img);

			raw_img_done = false;
			camera_enable_vsync_int();
		}
		if (time_is_timer_arrived(0)) {
			time_restart_timer(0);

			//��ȡ�ǶȺͽ��ٶ�
			gyro_acce_get_phy(&car.angle.angle, &car.angle.angle_speed);

			//���½Ƕȿ���ģ���Ŀ��PWMֵ
			control_angle_pid(&car.angle);

			//���½Ƕȿ���ģ������PWMֵ
			control_update_output_pwm(&car.angle.pwm);

			//�����ٶȿ���ģ������PWMֵ
			control_update_output_pwm(&car.speed.pwm);

			//���µ�������PWMֵ
			control_update_motor_pwm(&car);

			oled_printf(0, 2, "l_pwm:%d  ", car.left_motor_pwm);
			oled_printf(0, 4, "%4.2f  %4.2f", car.angle.angle,
					car.angle.angle_speed);
			oled_printf(0, 6, "angle_pwm:%d ", car.angle.pwm.output_pwm);
		}
		if (time_is_timer_arrived(1)) {
			time_restart_timer(1);

			//��ȡ�������ٶ�
			car.speed.left_speed = encoder_get_speed(ENCODER0);
			car.speed.right_speed = encoder_get_speed(ENCODER1);

			//�����ٶȿ���ģ���Ŀ��PWMֵ
			control_speed_pid(&car.speed);

//			oled_printf(0, 2, "speed:%4.2f %4.2f", speed_l, speed_r);

		}
		if (time_is_timer_arrived(3)) {
			time_restart_timer(3);
			light_change(LIGHT_BLUE);

			custom_oled_update_temp();

		}

	} //��ѭ��end_for
//��ѭ������==================================================================
}
