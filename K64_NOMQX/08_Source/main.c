//˵���������ļ����µ�Doc�ļ�����Readme.txt�ļ�
//============================================================================

#define GLOBLE_VAR  //ֻ����main.c�ж���һ�Σ�������ֹȫ�ֱ������ظ�����

#include "includes.h"   //������ͷ�ļ�

int main(void) {
	//1. ����������ʹ�õı���

	uint8 raw_img[CAMERA_RAW_IMG_BYTES];
	uint8 img[CAMERA_IMG_HEIGHT][CAMERA_IMG_WIDTH];
	int32 duty = 1000;
	float speed_l, speed_r;
	uint32 start, end;
	uint32 i, j;
	float angle, angle_speed;

	//2. �����ж�
	DISABLE_INTERRUPTS;

	//3. ��ʼ������ģ��
	light_init(LIGHT_BLUE, LIGHT_ON); //���Ƴ�ʼ��
	uart_init(UART_USE, 115200, UART_PARITY_DISABLED, UART_STOP_BIT_1,
	UART_BIT_ORDER_LSB); //uart0��ʼ��

	pit_init(PIT_CH0, 5);  //pit0��ʼ��������5ms
	pit_init(PIT_CH1, 71582);
	rng_init();

	temp_sensor_init();

	gyro_acce_init();

	oled_init();

//	menu_oled_display();
	custom_oled_display_init();

	encoder_init(ENCODER0);		//��ʼ���������
	encoder_init(ENCODER1);		//��ʼ���ұ�����

//	camera_init(raw_img);
	motor_init();
	motor_set(0, duty);
	motor_set(1, duty);

	//4. ���йر�������ֵ
	time0_flag.f_1s = 0;
	time0_flag.f_50ms = 0;
	time0_flag.f_5ms = 0;
	raw_img_done = false;

	//5. ʹ��ģ���ж�
	pit_enable_int(PIT_CH0);   		//ʹ��pit�ж�
	uart_enable_re_int(UART_USE);   //ʹ��uart0�����ж�
//	camera_enable_collect_done_int();
//	camera_enable_vsync_int();

	//6. �����ж�
	ENABLE_INTERRUPTS;

	//������ѭ��
	//��ѭ����ʼ==================================================================
	for (;;) {
		if (raw_img_done) {

//			camera_extract_raw_img(raw_img, (uint8*) img);
//
//			custom_oled_show_img(img);

//			vcan_send_raw_img(raw_img);

//			raw_img_done = false;
//			camera_enable_vsync_int();
		}
		if (time0_flag.f_5ms) {
			time0_flag.f_5ms = 0;

			start = pit_get_time_us(1);
			gyro_acce_get_phy(&angle, &angle_speed);
			end = pit_get_time_us(1);
			oled_printf(0, 4, "angle:%4.2f  ", angle);
			oled_printf(0, 6, "speed:%4.2f  ", angle_speed);


			oled_printf(0, 2, "time:%d", end-start);

		}
		if (time0_flag.f_50ms) {
			time0_flag.f_50ms = 0;

			speed_l = encoder_get_speed(ENCODER0);
			speed_r = encoder_get_speed(ENCODER1);

//			oled_printf(0, 2, "speed:%4.2f %4.2f", speed_l, speed_r);

		}
		if (time0_flag.f_1s) {
			time0_flag.f_1s = 0;
			light_change(LIGHT_BLUE);

			custom_oled_update_temp();

		}

	} //��ѭ��end_for
//��ѭ������==================================================================
}
