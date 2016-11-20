//˵���������ļ����µ�Doc�ļ�����Readme.txt�ļ�
//============================================================================

#define GLOBLE_VAR  //ֻ����main.c�ж���һ�Σ�������ֹȫ�ֱ������ظ�����

#include "includes.h"   //������ͷ�ļ�

int main(void) {
	//1. ����������ʹ�õı���
	uint32 run_counter;
	int16 count;
	uint16 temp;

	/* С����ز������� */
	Car car;

	/*���ڵ��Թ�������ʱ����*/
	float data_out[4];

	//2. �����ж�
	DISABLE_INTERRUPTS;

	//3. ��ʼ������ģ��
	light_init(LIGHT_RED, LIGHT_OFF); //���Ƴ�ʼ��
	//light_init(LIGHT_BLUE, LIGHT_OFF);
	uart_init(UART_USE, 9600, UART_PARITY_DISABLED, UART_STOP_BIT_1); //uart1��ʼ���������ã�����ģ�鲨����9600���޷���5ms�ж��д�������
//	uart_init(UART_USE, 115200);   //uart1��ʼ����������
	pit_init(PIT_CH0, 10000);  //pit0��ʼ��������5ms
//	motor_init(MOTOR1);			//������ʼ��
//	motor_init(MOTOR2);			//�ҵ����ʼ��
//	gyro_acce_init();			//�����Ǽ��ٶȼƳ�ʼ��
//	encoder_init(ENCODER1);		//���������ʼ��
//	encoder_init(ENCODER2);		//�ұ�������ʼ��
//	ems_init();					//��Ŵ�������ʼ��
//	reed_switch_init();			//�ɻɹܳ�ʼ��

	adc_init(ADC_MOD0, ADC_CLK_DIV_16, ADC_ACCURACY_SINGLE_8_DIFF_9,
			ADC_HARDWARE_AVG_DISABLE, ADC_ADLSTS_DISABLE, ADC_ADHSC_NORMAL,
			ADC_CAL_ENABLE);

	//4. ���йر�������ֵ
	run_counter = 0;

	//5. ʹ��ģ���ж�
	pit_enable_int(PIT_CH0);   		//ʹ��pit�ж�
	uart_enable_re_int(UART_USE);   //ʹ��uart1�����ж�
//	encoder_enable_int(ENCODER1);	//ʹ����������ж�
//	encoder_enable_int(ENCODER2);	//ʹ���ұ������ж�

	//6. �����ж�
	ENABLE_INTERRUPTS;

	//������ѭ��
	//��ѭ����ʼ==================================================================
	for (;;) {

		adc_single_get(ADC_MOD0,ADC_SE9,ADC_SE_SEL_A);
		run_counter++;

		if (time0_flag.f_10s) {
			time0_flag.f_10s = 0;
			uart_send1(UART_USE, run_counter >> 24);
			uart_send1(UART_USE, run_counter >> 16);
			uart_send1(UART_USE, run_counter >> 8);
			uart_send1(UART_USE, run_counter);
			run_counter=0;
		}

	} //��ѭ��end_for
	  //��ѭ������==================================================================
}
