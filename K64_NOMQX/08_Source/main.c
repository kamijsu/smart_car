//˵���������ļ����µ�Doc�ļ�����Readme.txt�ļ�
//============================================================================

#define GLOBLE_VAR  //ֻ����main.c�ж���һ�Σ�������ֹȫ�ֱ������ظ�����

#include "includes.h"   //������ͷ�ļ�

int main(void) {
	//1. ����������ʹ�õı���

	uint8 raw_img[CAMERA_RAW_IMG_BYTES];	//ԭʼͼ��
	uint8 img[CAMERA_IMG_HEIGHT][CAMERA_IMG_WIDTH];	//��ѹ��ͼ������
	uint32 start, end;	//��ʼ�ͽ���ʱ��ֵ����
	uint32 i, j;

	ControlCar car;	//С������

	//2. �����ж�
	DISABLE_INTERRUPTS;

	//3. ��ʼ������ģ��
	light_init(LIGHT_BLUE, LIGHT_ON); //���Ƴ�ʼ��
	uart_init(UART_USE, 115200, UART_PARITY_DISABLED, UART_STOP_BIT_1,
	UART_BIT_ORDER_LSB); //UART0��ʼ��

	//rng_init();

	temp_sensor_init();	//��ʼ���¶ȴ�����

	gyro_acce_init();	//��ʼ�������Ǽ��ٶȴ�����

	oled_init();		//��ʼ��Һ����

	pit_init(PIT_CH0, 5);  //PIT0��ʼ��������5ms
	pit_init(PIT_CH1, 71582);	//PIT1��ʼ������Ϊ������

	keyboard_init();	//��ʼ������

//	menu_oled_display();
//	custom_oled_display_init();

	encoder_init(ENCODER0);		//��ʼ���������
	encoder_init(ENCODER1);		//��ʼ���ұ�����

	camera_init(raw_img);	//��ʼ������ͷ

	motor_init();		//��ʼ�����

	time_init(0);		//��ʼ��ʱ��ģ��
	//ע�����ʱ��ͨ��
	time_register_timer(0, 5);
	time_register_timer(1, 50);
	time_register_timer(2, 50);
	time_register_timer(3, 1000);

	//4. ���йر�������ֵ
	raw_img_done = false;
	//��ʼ��С������
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

	//0m/sʱpid����
	car.angle.target_angle = 6.0f;
	car.angle.pid.p = 0.2f;
	car.angle.pid.d = 0.0003f;

	car.speed.target_speed = 0;
	car.speed.pid.p = 2;
	car.speed.pid.i = 0.5f;
	car.speed.pid.d = 0;

	//5. ʹ��ģ���ж�
	pit_enable_int(PIT_CH0);   		//ʹ��PIT0�ж�
	uart_enable_re_int(UART_USE);   //ʹ��UART0�����ж�
	keyboard_enable_int();			//ʹ�ܼ����ж�
	camera_enable_collect_done_int();	//ʹ��ԭʼͼ��ɼ�����ж�
	camera_enable_vsync_int();		//ʹ�ܳ��ж�

	//6. �����ж�
	ENABLE_INTERRUPTS;

	//������ѭ��
	//��ѭ����ʼ==================================================================
	for (;;) {
		//ͼ��ɼ�����ҵ���50ms
		if (raw_img_done && time_is_timer_arrived(2)) {
			//���¿�ʼ��ʱ
			time_restart_timer(2);
			//��ѹԭʼͼ��
			camera_extract_raw_img(raw_img, (uint8*) img);
			//��ʾͼ��
			custom_oled_show_img(img);
			//����ͼ����PC
//			vcan_send_raw_img(raw_img);
			//����ͼ����ɱ�־
			raw_img_done = false;
			//������ճ��ж�
			camera_enable_vsync_int();
		}
		//����5ms
		if (time_is_timer_arrived(0)) {
			//���¿�ʼ��ʱ
			time_restart_timer(0);

			start = pit_get_time_us(1);

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

			end = pit_get_time_us(1);

//			oled_printf(0, 2, "%d", end - start);

//			oled_printf(0,0,"agl:%4.2f ",car.angle.angle);
//			oled_printf(0,2,"agl_speed:%4.2f",car.angle.angle_speed);
//			oled_printf(0, 4, "%4.2f  %4.2f  ", car.angle.angle,
//					car.angle.angle_speed);
//			oled_printf(0, 4, "angle_pwm:%d ", car.angle.pwm.output_pwm);
//			oled_printf(0, 6, "speed_pwm:%d  ", car.speed.pwm.output_pwm);
		}
		//����50ms
		if (time_is_timer_arrived(1)) {
			//���¿�ʼ��ʱ
			time_restart_timer(1);

			//��ȡ�������ٶ�
			car.speed.left_speed = encoder_get_speed(ENCODER0);
			car.speed.right_speed = encoder_get_speed(ENCODER1);

			//�����ٶȿ���ģ���Ŀ��PWMֵ
			control_speed_pid(&car.speed);

//			oled_printf(0, 4, "speed:%4.2f", (car.speed.left_speed+car.speed.right_speed)/2);

		}
		//����1s
		if (time_is_timer_arrived(3)) {
			//���¿�ʼ��ʱ
			time_restart_timer(3);
			light_change(LIGHT_BLUE);

//			custom_oled_update_temp();

		}

	} //��ѭ��end_for
//��ѭ������==================================================================
}
