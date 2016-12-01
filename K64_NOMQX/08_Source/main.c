//˵���������ļ����µ�Doc�ļ�����Readme.txt�ļ�
//============================================================================

#define GLOBLE_VAR  //ֻ����main.c�ж���һ�Σ�������ֹȫ�ֱ������ظ�����

#include "includes.h"   //������ͷ�ļ�

int main(void) {
	//1. ����������ʹ�õı���
	uint32 run_counter;
	uint16 send;
	float temp;

	int16 test;
	int32 test2;

	/* С����ز������� */
	Car car;

	/*���ڵ��Թ�������ʱ����*/
	float data_out[4];

	//2. �����ж�
	DISABLE_INTERRUPTS;

	//3. ��ʼ������ģ��
	light_init(LIGHT_BLUE, LIGHT_OFF); //���Ƴ�ʼ��
	//light_init(LIGHT_BLUE, LIGHT_OFF);
	uart_init(UART_USE, 115200, UART_PARITY_DISABLED, UART_STOP_BIT_1); //uart1��ʼ���������ã�����ģ�鲨����9600���޷���5ms�ж��д�������
//	uart_init(UART_USE, 115200);   //uart1��ʼ����������
	pit_init(PIT_CH0, 5);  //pit0��ʼ��������5ms
	motor_init(MOTOR0);			//������ʼ��
	motor_init(MOTOR1);			//�ҵ����ʼ��
//	gyro_acce_init();			//�����Ǽ��ٶȼƳ�ʼ��
	encoder_init(ENCODER0);		//���������ʼ��
//	encoder_init(ENCODER1);		//�ұ�������ʼ��
//	ems_init();					//��Ŵ�������ʼ��
	reed_switch_init();			//�ɻɹܳ�ʼ��
//ftm_init(FTM_MOD0,FTM_CLK_DIV_128,FTM_COUNTER_MODE_UP_DOWN,100000);
//
//ftm_pwm_single_init(FTM_MOD0,FTM_CH5,FTM_PWM_MODE_CENTER_ALIGNED,FTM_PWM_POL_NEGATIVE,5000);
	temp_sensor_init();

	//4. ���йر�������ֵ
	run_counter = 0;
	test = 0xFFFF;

	//5. ʹ��ģ���ж�
	pit_enable_int(PIT_CH0);   		//ʹ��pit�ж�
	uart_enable_re_int(UART_USE);   //ʹ��uart1�����ж�
	reed_switch_enable_int();

	uart_send_string(UART_USE,"test��\n");

	//6. �����ж�
	ENABLE_INTERRUPTS;

	//������ѭ��
	//��ѭ����ʼ==================================================================
	for (;;) {
		if(time0_flag.f_50ms){
			time0_flag.f_50ms = 0;
			data_out[0] = encoder_get_speed(ENCODER0) * 1000;
			visual_scope_output(UART_USE,data_out);
		}
		if (time0_flag.f_1s) {
//			uart_send_string(UART_USE, "���Ĳ���");
			time0_flag.f_1s = 0;
			light_change(LIGHT_BLUE);
//			printf("%d\n",encoder_get_and_clear_count(ENCODER0));
//			temp = temp_sensor_get();
//			send = (uint16) (temp * 1000);
//			uart_send1(UART_USE, send >> 8);
//			uart_send1(UART_USE, send);
		}

	} //��ѭ��end_for
	  //��ѭ������==================================================================
}
