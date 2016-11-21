//˵���������ļ����µ�Doc�ļ�����Readme.txt�ļ�
//============================================================================

#define GLOBLE_VAR  //ֻ����main.c�ж���һ�Σ�������ֹȫ�ֱ������ظ�����

#include "includes.h"   //������ͷ�ļ�

int main(void) {
	//1. ����������ʹ�õı���
	uint32 run_counter;
	int16 count;
	int16 temp;

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
	pit_init(PIT_CH0, 5);  //pit0��ʼ��������5ms
//	motor_init(MOTOR1);			//������ʼ��
//	motor_init(MOTOR2);			//�ҵ����ʼ��
//	gyro_acce_init();			//�����Ǽ��ٶȼƳ�ʼ��
//	encoder_init(ENCODER1);		//���������ʼ��
//	encoder_init(ENCODER2);		//�ұ�������ʼ��
//	ems_init();					//��Ŵ�������ʼ��
//	reed_switch_init();			//�ɻɹܳ�ʼ��

	adc_init(ADC_MOD0, ADC_CLK_DIV_4, ADC_ACCURACY_SINGLE_12_DIFF_13,
			ADC_HARDWARE_AVG_16, ADC_ADLSTS_6, ADC_ADHSC_NORMAL,
			ADC_CAL_DISABLE);

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



		if (time0_flag.f_1s) {
			time0_flag.f_1s = 0;
			temp = adc_diff_get(ADC_MOD0,ADC_DIFF_GROUP1);
			temp = temp * 3300 / ((1<<12) - 1);
								uart_send1(UART_USE, temp >> 8);
								uart_send1(UART_USE, temp);
		}

	} //��ѭ��end_for
	  //��ѭ������==================================================================
}
