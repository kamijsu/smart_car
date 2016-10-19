//˵���������ļ����µ�Doc�ļ�����Readme.txt�ļ�
//======================================================================

#define GLOBLE_VAR  //ֻ����main.c�ж���һ�Σ�������ֹȫ�ֱ������ظ�����

#include "includes.h"   //������ͷ�ļ�

int main(void)
{
	//1. ����������ʹ�õı���
	BreatheLightParams green_light;
	BreatheLightParams red_light;

	//2. �����ж�
	DISABLE_INTERRUPTS;

	//3. ��ʼ������ģ��
	light_init(LIGHT_BLUE, LIGHT_OFF);				//���Ƴ�ʼ��
	breathe_light_init(BREATHE_LIGHT_GREEN, 0);		//��ʼ���̺�����
	breathe_light_init(BREATHE_LIGHT_RED, 0);		//��ʼ���������
	uart_init(UART_USE, 9600);			//��ʼ��ʹ�ô��ڣ���includes.h�ж��壩��������Ϊ9600
	pit_init(PIT_CH0, 50);				//��ʼ��PITģ���0ͨ����ÿ50ms����һ���ж�

	//4. ���йر�������ֵ
	//��Ϣ���ж������β�±��ʼΪ0
	g_msg.front = 0;
	g_msg.rear = 0;
	//ʱ���־��0
	g_time_flag.f_50ms = 0;
	g_time_flag.f_1s = 0;
	//�̺�����δ��������Ч����ռ�ձ�Ϊ0�����ڱ�����ÿ�α仯��СΪ2.0
	green_light.is_start = 0;
	green_light.duty = 0;
	green_light.dir = 1;
	green_light.radio = 2.0;
	//�������δ��������Ч����ռ�ձ�Ϊ0�����ڱ�����ÿ�α仯��СΪ2.0
	red_light.is_start = 0;
	red_light.duty = 0;
	red_light.dir = 1;
	red_light.radio = 2.0;

	//5. ʹ��ģ���ж�
	uart_enable_re_int(UART_2);		//ʹ�ܴ���2�����ж�
	pit_enable_int(PIT_CH0);		//ʹ��PITģ��0ͨ���ж�

	uart_send_string(UART_USE, "��������ָ��ı���Կ���MCU��\r\n");
	uart_send_string(UART_USE, "0.��������\r\n");
	uart_send_string(UART_USE, "1.�ر�����\r\n");
	uart_send_string(UART_USE, "2.�����̵�\r\n");
	uart_send_string(UART_USE, "3.�ر��̵�\r\n");
	uart_send_string(UART_USE, "4.�������\r\n");
	uart_send_string(UART_USE, "5.�رպ��\r\n");
	uart_send_string(UART_USE, "6.�����̵ƺ�����Ч��\r\n");
	uart_send_string(UART_USE, "7.�ر��̵ƺ�����Ч��\r\n");
	uart_send_string(UART_USE, "8.������ƺ�����Ч��\r\n");
	uart_send_string(UART_USE, "9.�رպ�ƺ�����Ч��\r\n");

	//6. �����ж�
	ENABLE_INTERRUPTS;

	//======��ѭ����ʼ======================================================
	for (;;)
	{
		//ÿ1sִ��һ�εĳ������PIT�жϷ���������Ч��
		if (g_time_flag.f_1s)
		{
			//����ʱ���־
			g_time_flag.f_1s = 0;

			//####������Ϣ

		}

		//ÿ50msִ��һ�εĳ������PIT�жϷ���������Ч��
		if (g_time_flag.f_50ms)
		{
			//����ʱ���־
			g_time_flag.f_50ms = 0;

			//####��������ɺ�����ռ�ձȵı仯����������������Ч��ʱ��ÿ50ms����һ��ռ�ձ�

			//����Ϊ��ʾ

			//####�̵ƿ�������Ч��ʱִ��

				//####�ж��̵Ʊ仯����

					//####���ݱ仯������/��ռ�ձ�

				//####�����̵��µ�ռ�ձ�

			//####���ͬ��

		}

		//���ϴ�����Ϣ����
		if (g_msg.front != g_msg.rear)
		{
			//####��ȡ������Ϣ����������switch-case��䣬���ݿ�ͷ������ָ������Ӧ�Ĳ���

			//��ֹԽ��
			if (g_msg.front == 16)
			{
				g_msg.front = 0;
			}
		}

	}  //��ѭ��end_for
//======��ѭ������======================================================
}

