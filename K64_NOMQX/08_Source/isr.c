//===========================================================================
//�ļ����ƣ�isr.c
//���ܸ�Ҫ���Զ����жϷ�������Դ�ļ�
//��Ȩ���У����ݴ�ѧ��˼����Ƕ��ʽ����(sumcu.suda.edu.cn)
//�汾���£�2013-05-03    V1.0,2014-05-10    V2.3(WYH) 
//===========================================================================

#include "includes.h"

//==========================================================================
//�������ƣ�UART1_RX_TX_IRQHandler
//����˵������
//�������أ���
//���ܸ�Ҫ��UART1�жϷ�����
//==========================================================================
void UART1_RX_TX_IRQHandler() {
	uint8 ch;
	bool err;
	DISABLE_INTERRUPTS;

	if (uart_re1_parity(UART_MOD1, &ch, &err)) {
		if (!err) {
			uart_send1(UART_MOD1, ch);
		}
	}

	ENABLE_INTERRUPTS;
}

void FTM0_IRQHandler() {
	if (ftm_timer_get_int(0)) {
		ftm_timer_clear_int(0);
		time0_flag.f_1s = 1;

	}

}

//==========================================================================
//�������ƣ�PIT0_IRQHandler
//����˵������
//�������أ���
//���ܸ�Ҫ��PIT0�жϷ�����
//==========================================================================
void PIT0_IRQHandler() {
	static TimeCounter time_counter = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	DISABLE_INTERRUPTS; //���ж�

	pit_clear_int(PIT_CH0); //���־

	//����Ϊ��Ч��ͣ����û��ʹ�õ�ʱ���־

	++time_counter.c_5ms;
//	++time_counter.c_10ms;
//	++time_counter.c_15ms;
	++time_counter.c_20ms;
	++time_counter.c_50ms;
//	++time_counter.c_100ms;
	++time_counter.c_1s;
	++time_counter.c_5s;
	++time_counter.c_10s;
	++time_counter.c_15s;
//	++time_counter.c_30s;
//	++time_counter.c_1min;

	if (time_counter.c_5ms >= 1) {
		time_counter.c_5ms = 0;
		time0_flag.f_5ms = 1;
	}       //5ms�ñ�־
//	if (time_counter.c_10ms >= 2)
//	{
//		time_counter.c_10ms = 0;
//		time0_flag.f_10ms = 1;
//	}       //10ms�ñ�־
//	if (time_counter.c_15ms >= 3)
//	{
//		time_counter.c_15ms = 0;
//		time0_flag.f_15ms = 1;
//	}       //15ms�ñ�־
	if (time_counter.c_20ms >= 4) {
		time_counter.c_20ms = 0;
		time0_flag.f_20ms = 1;
	}       //20ms�ñ�־
	if (time_counter.c_50ms >= 10) {
		time_counter.c_50ms = 0;
		time0_flag.f_50ms = 1;
	}       //50ms�ñ�־
//	if (time_counter.c_100ms >= 20)
//	{
//		time_counter.c_100ms = 0;
//		time0_flag.f_100ms = 1;
//	}       //100ms�ñ�־
	if (time_counter.c_1s >= 200) {
		time_counter.c_1s = 0;
		time0_flag.f_1s = 1;
	}       //1s�ñ�־
	if (time_counter.c_5s >= 1000) {
		time_counter.c_5s = 0;
		time0_flag.f_5s = 1;
	}       //5s�ñ�־
	if (time_counter.c_10s >= 2000) {
		time_counter.c_10s = 0;
		time0_flag.f_10s = 1;
	}       //10s�ñ�־
	if (time_counter.c_15s >= 3000) {
		time_counter.c_15s = 0;
		time0_flag.f_15s = 1;
	}       //15s�ñ�־
//	if (time_counter.c_30s >= 6000)
//	{
//		time_counter.c_30s = 0;
//		time0_flag.f_30s = 1;
//	}       //30s�ñ�־
//	if (time_counter.c_1min >= 12000)
//	{
//		time_counter.c_1min = 0;
//		time0_flag.f_1min = 1;
//	}       //1min�ñ�־

	ENABLE_INTERRUPTS; //�ָ�ԭ���ж��������
}

//============================================================================
//�������ƣ�PORTD_IRQHandler
//����������
//�������أ���
//���ܸ�Ҫ��PORTD�˿��жϷ�����
//============================================================================
void PORTD_IRQHandler() {
	uint8_t n;    //���ź�

	DISABLE_INTERRUPTS; //���ж�

	if (reed_switch_get_int()) {
		reed_switch_clear_int();
		reed_switch_disable_int();
		uart_send_string(UART_USE,"�����жϣ�\n");
	}

	ENABLE_INTERRUPTS; //�ָ�ԭ���ж��������
}

