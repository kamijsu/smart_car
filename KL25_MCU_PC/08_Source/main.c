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

	//6. �����ж�
	ENABLE_INTERRUPTS;

	//======��ѭ����ʼ======================================================
	for (;;)
	{
		//ÿ1sִ��һ�εĳ���
		if (g_time_flag.f_1s)
		{
			//����ʱ���־
			g_time_flag.f_1s = 0;
			//������Ϣ

		}

		//ÿ50msִ��һ�εĳ���
		if (g_time_flag.f_50ms)
		{
			//����ʱ���־
			g_time_flag.f_50ms = 0;



		}

		//���ϴ�����Ϣ����
		if (g_msg.front != g_msg.rear)
		{
			//��ȡ������Ϣ������
			switch (g_msg.msg[g_msg.front++])
			{
			case '0':	//��������
				light_control(LIGHT_BLUE, LIGHT_ON);
				break;
			case '1':	//�ر�����
				light_control(LIGHT_BLUE, LIGHT_OFF);
				break;
			case '2':	//�����̵�
				//�رպ�����Ч��
				green_light.is_start = 0;
				//��������
				breathe_light_set(BREATHE_LIGHT_GREEN, BREATHE_LIGHT_MAX_DUTY);
				break;
			case '3':	//�ر��̵�
				//�رպ�����Ч��
				green_light.is_start = 0;
				//�����
				breathe_light_set(BREATHE_LIGHT_GREEN, BREATHE_LIGHT_MIN_DUTY);
				break;
			case '4':	//�������
				//�رպ�����Ч��
				red_light.is_start = 0;
				//��������
				breathe_light_set(BREATHE_LIGHT_RED, BREATHE_LIGHT_MAX_DUTY);
				break;
			case '5':	//�رպ��
				//�رպ�����Ч��
				red_light.is_start = 0;
				//�����
				breathe_light_set(BREATHE_LIGHT_RED, BREATHE_LIGHT_MIN_DUTY);
				break;
			case '6':	//�����̵ƺ�����Ч��
				//����������Ч��
				green_light.is_start = 1;
				//��ȡ�̺����Ƶ�ǰ����
				green_light.duty = breathe_light_get(BREATHE_LIGHT_GREEN);
				break;
			case '7':	//�ر��̵ƺ�����Ч��
				//�رպ�����Ч��
				green_light.is_start = 0;
				break;
			case '8':	//������ƺ�����Ч��
				//����������Ч��
				red_light.is_start = 1;
				//��ȡ������Ƶ�ǰ����
				red_light.duty = breathe_light_get(BREATHE_LIGHT_RED);
				break;
			case '9':	//�رպ�ƺ�����Ч��
				//�رպ�����Ч��
				red_light.is_start = 0;
				break;
			}
			//��ֹԽ��
			if (g_msg.front == 16)
			{
				g_msg.front = 0;
			}
		}

	}  //��ѭ��end_for
//======��ѭ������======================================================
}

