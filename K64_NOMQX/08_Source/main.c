//˵���������ļ����µ�Doc�ļ�����Readme.txt�ļ�
//============================================================================

#define GLOBLE_VAR  //ֻ����main.c�ж���һ�Σ�������ֹȫ�ֱ������ظ�����

#include "includes.h"   //������ͷ�ļ�

int main(void) {
	//1. ����������ʹ�õı���
uint32 run_counter;


	/* С����ز������� */
	Car car;

	/*���ڵ��Թ�������ʱ����*/
	float data_out[4];

	//2. �����ж�
	DISABLE_INTERRUPTS;

	//3. ��ʼ������ģ��
	light_init(LIGHT_RED, LIGHT_OFF); //���Ƴ�ʼ��
	//light_init(LIGHT_BLUE, LIGHT_OFF);
	uart_init(UART_USE,9600,UART_PARITY_DISABLED,UART_STOP_BIT_1);     //uart1��ʼ���������ã�����ģ�鲨����9600���޷���5ms�ж��д�������
//	uart_init(UART_USE, 115200);   //uart1��ʼ����������
	//pit_init(PIT_CH0, 5);  //pit0��ʼ��������5ms
//	motor_init(MOTOR1);			//������ʼ��
//	motor_init(MOTOR2);			//�ҵ����ʼ��
//	gyro_acce_init();			//�����Ǽ��ٶȼƳ�ʼ��
//	encoder_init(ENCODER1);		//���������ʼ��
//	encoder_init(ENCODER2);		//�ұ�������ʼ��
//	ems_init();					//��Ŵ�������ʼ��
//	reed_switch_init();			//�ɻɹܳ�ʼ��
	ftm_init(FTM_MOD1,FTM_CLK_DIV_128,FTM_COUNTER_MODE_UP,100);

	ftm_init(FTM_MOD0,FTM_CLK_DIV_128,FTM_COUNTER_MODE_UP,100);
//ftm_pwm_combine_init(FTM_MOD0,FTM_CH_GROUP2,FTM_PWM_MODE_COMBINE,FTM_PWM_POL_NEGATIVE,9998,10000);
	//ftm_pwm_single_init(FTM_MOD0,FTM_CH4,FTM_PWM_MODE_CENTER_ALIGNED,FTM_PWM_POL_NEGATIVE,5000);
	ftm_pwm_single_init(FTM_MOD0,FTM_CH4,FTM_PWM_MODE_EDGE_ALIGNED,FTM_PWM_POL_NEGATIVE,5000);
//ftm_pwm_single_set(FTM_MOD0,FTM_CH4,2);
//ftm_ic_init(FTM_MOD0,FTM_CH1,FTM_CAPTURE_MODE_DOUBLE_EDGE);
//
	ftm_oc_init(FTM_MOD0,FTM_CH0,FTM_OC_MODE_SET,9999);


	ftm_ic_init(FTM_MOD0,FTM_CH0,FTM_IC_MODE_DOUBLE_EDGE);

	//4. ���йر�������ֵ
	run_counter = 0;



	//5. ʹ��ģ���ж�
//	ftm_ch_enable_int(FTM_MOD0,FTM_CH1);
//	ftm_ch_enable_int(FTM_MOD0,FTM_CH0);

//gpio_pull(COM_PORTC|3,GPIO_LEVEL_LOW);
//gpio_pull(COM_PORTC|4,GPIO_LEVEL_LOW);
		ftm_decap_enable_int(FTM_MOD1,FTM_CH_GROUP0);

	//pit_enable_int(PIT_CH0);   		//ʹ��pit�ж�
	uart_enable_re_int(UART_USE);   //ʹ��uart1�����ж�
	encoder_enable_int(ENCODER1);	//ʹ����������ж�
	encoder_enable_int(ENCODER2);	//ʹ���ұ������ж�
	//ftm_timer_enable_int(FTM_MOD1,1);
	//6. �����ж�
	ENABLE_INTERRUPTS;
	ftm_decap_reenable(FTM_MOD1,FTM_CH_GROUP0);

	//������ѭ��
	//��ѭ����ʼ==================================================================
	for (;;) {

		//run_counter = 3.14*5.123;
//		if(run_counter++>=RUN_COUNTER_MAX){
//			run_counter = 0;
//			light_change(LIGHT_RED);
//		}

		if (time0_flag.f_1s) {
			time0_flag.f_1s = 0;
			light_change(LIGHT_RED);      //���ƣ�LIGHT_BLUE��״̬�仯
		}

	} //��ѭ��end_for
	  //��ѭ������==================================================================
}
