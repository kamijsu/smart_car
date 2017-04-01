//˵���������ļ����µ�Doc�ļ�����Readme.txt�ļ�
//============================================================================

#define GLOBLE_VAR  //ֻ����main.c�ж���һ�Σ�������ֹȫ�ֱ������ظ�����

#include "includes.h"   //������ͷ�ļ�

int main(void) {
	//1. ����������ʹ�õı���
	uint32 start, end;
	uint32 i, j;
	uint8 raw_img[CAMERA_RAW_IMG_BYTES];
	uint8 img[CAMERA_IMG_HEIGHT][CAMERA_IMG_WIDTH];
	int32 duty = 2000;
	FrameInfo info;
	uint8 frame[263];
	uint16 frame_len;

	uint8 src1[10240];
	uint8 src[10240];
	for (i = 0; i < 10240; i++) {
		src[i] = i % 256;
	}
	uint8 dest1[10240];
	uint8 dest[10240];
	memset(dest, 0xFF, 10240);

	//2. �����ж�
	DISABLE_INTERRUPTS;

	//3. ��ʼ������ģ��
	light_init(LIGHT_BLUE, LIGHT_ON); //���Ƴ�ʼ��
	uart_init(UART_USE, 9600, UART_PARITY_DISABLED, UART_STOP_BIT_1,
	UART_BIT_ORDER_LSB); //uart0��ʼ��
	frame_init(3);

	pit_init(PIT_CH0, 5);  //pit0��ʼ��������5ms
	pit_init(PIT_CH1, 71582);
	rng_init();

	temp_sensor_init();

	oled_init();

//	menu_oled_display();
//	custom_oled_display_init();

	camera_init(raw_img);
	motor_init();
	motor_set(0,duty);
	motor_set(1,duty);

	//4. ���йر�������ֵ
	time0_flag.f_1s = 0;
	time0_flag.f_50ms = 0;
	raw_img_done = false;

	//5. ʹ��ģ���ж�
	pit_enable_int(PIT_CH0);   		//ʹ��pit�ж�
	uart_enable_re_int(UART_USE);   //ʹ��uart0�����ж�
	camera_enable_collect_done_int();
	camera_enable_vsync_int();

	//6. �����ж�
	ENABLE_INTERRUPTS;

	//������ѭ��
	//��ѭ����ʼ==================================================================
	for (;;) {
		if (raw_img_done && time0_flag.f_50ms) {
			time0_flag.f_50ms = 0;

			camera_extract_raw_img(raw_img, (uint8*) img);

			custom_oled_show_img(img);

//			vcan_send_raw_img(raw_img);

			raw_img_done = false;
			camera_enable_vsync_int();
		}
		if (time0_flag.f_1s) {
			time0_flag.f_1s = 0;
			light_change(LIGHT_BLUE);

			start = pit_get_time_us(1);
//			duty*=-1;
//			motor_set(0,duty);
//			motor_set(1,duty);
//			custom_oled_update_temp();
			info.src_addr = frame_get_local_addr();
			info.dest_addr = 3;
			info.type = 2;
			info.len = 1;
			frame_info_to_frame(&info,frame,&frame_len);
			uart_sendN(UART_USE,frame,frame_len);

			end = pit_get_time_us(1);
//			uart_printf(UART_USE, "����ʱ�䣺%dus\r\n", end - start);

		}

	} //��ѭ��end_for
//��ѭ������==================================================================
}
