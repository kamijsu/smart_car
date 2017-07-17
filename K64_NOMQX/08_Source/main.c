//˵���������ļ����µ�Doc�ļ�����Readme.txt�ļ�
//============================================================================

#define GLOBLE_VAR  //ֻ����main.c�ж���һ�Σ�������ֹȫ�ֱ������ظ�����

#include "includes.h"   //������ͷ�ļ�

//�����ڲ�ʹ�ú�
#define ABS(x) ((x) > 0 ? (x) : -(x))

int main(void) {
	//1. ����������ʹ�õı���
	uint32 start, end;	//��ʼ�ͽ���ʱ��ֵ����
	FrameInfo info;		//֡��Ϣ�ṹ��
	uint8 frame[263];	//֡�ֽ�
	uint16 frame_len;	//֡�ֽڳ���
	bool can_send_param;	//���Է��Ͳ�����־
	bool can_send_img;		//���Է���ͼƬ��־
	bool img_copy_done;		//ͼƬ������ɱ�־
	bool stop_car;	//ͣ����־
	bool started;	//������־
	uint8 raw_img_copy[CAMERA_RAW_IMG_BYTES];	//����ԭʼͼƬ
	int16 mid_points_copy[CAMERA_IMG_HEIGHT];	//�����е�
	int16 left_edges_copy[CAMERA_IMG_HEIGHT];	//�������Ե
	int16 right_edges_copy[CAMERA_IMG_HEIGHT];	//�����ұ�Ե
	bool valid_row_copy[CAMERA_IMG_HEIGHT];		//������Ч��
	ParamCar car;	//С������

	//2. �����ж�
	DISABLE_INTERRUPTS;

	//3. ��ʼ������ģ��
	light_init(LIGHT_BLUE, LIGHT_ON); //���Ƴ�ʼ��
	uart_init(UART_USE, 115200, UART_PARITY_DISABLED, UART_STOP_BIT_1,
	UART_BIT_ORDER_LSB); //UART0��ʼ��

	gyro_acce_init();	//��ʼ�������Ǽ��ٶȴ�����

	oled_init();		//��ʼ��Һ����

	pit_init(PIT_CH0, 5);  //PIT0��ʼ��������5ms
	pit_init(PIT_CH1, 71582);	//PIT1��ʼ������Ϊ������

	keyboard_init();	//��ʼ������
	param_init(&car);	//��ʼ������
	param_get(&car, 1);	//��ȡ��1�ײ���������ƽ�����
	//��ʼ���˵���ʾ�����������mode��ѡ�����������Ӧflashλ�õĲ���
	menu_init(&car);

	encoder_init(ENCODER0);		//��ʼ���������
	encoder_init(ENCODER1);		//��ʼ���ұ�����

	camera_init(car.turn.raw_img);	//��ʼ������ͷ

	motor_init();		//��ʼ�����

	time_init(0);		//��ʼ��ʱ��ģ��
	//ע�����ʱ��ͨ��
	time_register_timer(0, 5);
	time_register_timer(1, 50);
	time_register_timer(2, 50);
	time_register_timer(3, 1000);
	time_register_timer(4, 100);
	time_register_timer(5, 5000);

	//ʹ����֡ͨ��Э��
	frame_init(0);

	//4. ���йر�������ֵ
	raw_img_done = false;
	can_send_param = false;
	can_send_img = false;
	img_copy_done = false;
	stop_car = false;
	started = false;

	//0m/sʱpid����
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
			camera_extract_raw_img(car.turn.raw_img, (uint8*) car.turn.img);

//			start = pit_get_time_us(1);

			//Ѱ���е�
			control_find_mid_points(&car.turn);

			//�����Ȩƽ��ֵ
			control_cal_avg_mid_point(&car.turn);

//			end = pit_get_time_us(1);

			//���·������ģ���Ŀ��PWMֵ
			control_turn_pid(&car.turn);

			//��ʾͼ��
			if (menu_can_show_img()) {
				oled_printf(0, 6, "%.2f", car.angle.angle);
				oled_printf(0, 0, "%.2f", car.turn.avg_mid_point);
//				oled_printf(0, 2, "%d", end - start);
				custom_oled_show_img(car.turn.img);
			}

			//����ԭʼͼ���Թ�����
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

			//����ͼ����PC
//			vcan_send_raw_img(car.turn.raw_img);	//52ms

			//����ͼ����ɱ�־
			raw_img_done = false;
			//������ճ��ж�
			camera_enable_vsync_int();
		}

		//����5ms
		if (time_is_timer_arrived(0)) {
			//���¿�ʼ��ʱ
			time_restart_timer(0);

			//��ȡ�ǶȺͽ��ٶ�
			gyro_acce_get_phy(&car.angle.angle, &car.angle.angle_speed);

			//���½Ƕȿ���ģ���Ŀ��PWMֵ
			control_angle_pid(&car.angle);

			//���½Ƕȿ���ģ������PWMֵ
			control_update_output_pwm(&car.angle.pwm);

			//�����ٶȿ���ģ������PWMֵ
			control_update_output_pwm(&car.speed.pwm);

			//���·������ģ������PWMֵ
			control_update_output_pwm(&car.turn.pwm);

			//Ҫͣ���Ļ���ͣ�����������PWMֵ
			if (stop_car) {
				//ͣ��
				control_stop_car();
			} else {
				//���µ�������PWMֵ
				control_update_motor_pwm(&car);
			}

		}

		//����50ms
		if (time_is_timer_arrived(1)) {
			//���¿�ʼ��ʱ
			time_restart_timer(1);

			//��ȡ�������ٶ�
			car.speed.left_speed = encoder_get_speed(ENCODER0);
			car.speed.right_speed = encoder_get_speed(ENCODER1);

			//�ٶȱ��������ٶȹ��ߣ�ͣ��
			if (ABS(car.speed.left_speed) + ABS(car.speed.right_speed) >= 10) {
				stop_car = true;
			}

			//�����ٶȿ���ģ���Ŀ��PWMֵ
			control_speed_pid(&car.speed);

//			oled_printf(0, 4, "speed:%4.2f", (car.speed.left_speed+car.speed.right_speed)/2);

		}

		//����1s
		if (time_is_timer_arrived(3)) {
			//���¿�ʼ��ʱ
			time_restart_timer(3);
			//С����˸
			light_change(LIGHT_BLUE);
		}

		//����100ms
		if (time_is_timer_arrived(4)) {
			//���¿�ʼ��ʱ
			time_restart_timer(4);

			//�����Է��Ͳ�������������λ��
			if (can_send_param) {
				//7ms
				custom_send_param_to_host(&car);
			}
			//�����Է���ͼ����������λ��
			if (can_send_img && img_copy_done) {
				img_copy_done = false;
				custom_send_mid_points_to_host(mid_points_copy, valid_row_copy);
				custom_send_edges_to_host(left_edges_copy, right_edges_copy,
						valid_row_copy);
				//54ms
				custom_send_raw_img_to_host(raw_img_copy);
			}

		}

		//����5s
		if (!started && time_is_timer_arrived(5)) {
			//��������������С��
			param_get(&car, 2);
			started = true;
		}

		if (frame_get_info(0, &info)) {
			switch (info.type) {
			case 0:	//�ַ���֡
				frame_info_to_frame(&info, frame, &frame_len);
				uart_sendN(UART_USE, frame, frame_len);
				break;
			case 7:	//�����Ƿ���ͼ��
				can_send_img = info.data[0] == 1 ? true : false;
				break;
			case 8:	//�����Ƿ��Ͳ���
				can_send_param = info.data[0] == 1 ? true : false;
				break;
			case 9:	//�����Ƿ�ͣ��
				stop_car = info.data[0] == 1 ? true : false;
				break;
			}
		}

	} //��ѭ��end_for
//��ѭ������==================================================================
}
