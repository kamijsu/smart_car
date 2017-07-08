//===========================================================================
//�ļ����ƣ�isr.c
//���ܸ�Ҫ���Զ����жϷ�������Դ�ļ�
//��Ȩ���У����ݴ�ѧ��˼����Ƕ��ʽ����(sumcu.suda.edu.cn)
//�汾���£�2013-05-03    V1.0,2014-05-10    V2.3(WYH)
//===========================================================================

#include "includes.h"

//==========================================================================
//�������ƣ�UART0_RX_TX_IRQHandler
//����˵������
//�������أ���
//���ܸ�Ҫ��UART1�жϷ�����
//==========================================================================
void UART0_RX_TX_IRQHandler() {
	FrameFramingResult res;
	uint8 ch;
	bool err;
	DISABLE_INTERRUPTS;

	if (uart_re1_parity(UART_MOD0, &ch, &err)) {
		if (!err) {
			frame_framing(0, ch);
		}
	}

	ENABLE_INTERRUPTS;
}

//==========================================================================
//�������ƣ�PORTA_IRQHandler
//����˵������
//�������أ���
//���ܸ�Ҫ��PORTA�жϷ�����
//==========================================================================
void PORTA_IRQHandler() {
	static uint32 i = 0;
	uint8 key;

	DISABLE_INTERRUPTS;
	if (keyboard_get_int()) {
		if (keyboard_read(&key))
			menu_accept(key);
		keyboard_clear_int();
	}
	ENABLE_INTERRUPTS;
}

//==========================================================================
//�������ƣ�PORTC_IRQHandler
//����˵������
//�������أ���
//���ܸ�Ҫ��PORTC�жϷ�����
//==========================================================================
void PORTC_IRQHandler() {
	DISABLE_INTERRUPTS;
	//���յ����ж�
	if (camera_get_vsync_int()) {
		//��ʼ�ɼ�����
		camera_start_collecting();
		//ֹͣ���ճ��ж�
		camera_disable_vsync_int();
		//�رճ��жϱ�־
		camera_clear_vsync_int();
	}
	ENABLE_INTERRUPTS;
}

//==========================================================================
//�������ƣ�DMA0_IRQHandler
//����˵������
//�������أ���
//���ܸ�Ҫ��DMA0�жϷ�����
//==========================================================================
void DMA0_IRQHandler() {
	DISABLE_INTERRUPTS;
	//ֹͣ�ɼ�����
	camera_stop_collecting();
	//����ɼ�����жϱ�־
	camera_clear_collect_done_int();
	//ͼ��ɼ����
	raw_img_done = true;
	ENABLE_INTERRUPTS;
}

//==========================================================================
//�������ƣ�PIT0_IRQHandler
//����˵������
//�������أ���
//���ܸ�Ҫ��PIT0�жϷ�����
//==========================================================================
void PIT0_IRQHandler() {
	DISABLE_INTERRUPTS; //���ж�

	pit_clear_int(PIT_CH0); //���־

	time_update(5);	//����5msʱ��

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
		uart_send_string(UART_USE, "�����жϣ�\n");
	}

	ENABLE_INTERRUPTS; //�ָ�ԭ���ж��������
}
