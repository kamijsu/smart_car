//˵���������ļ����µ�Doc�ļ�����Readme.txt�ļ�
//============================================================================

#define GLOBLE_VAR  //ֻ����main.c�ж���һ�Σ�������ֹȫ�ֱ������ظ�����

#include "includes.h"   //������ͷ�ļ�

int main(void) {
	//1. ����������ʹ�õı���
	uint32 start, end;
	uint32 i, j;


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
	uart_init(UART_USE, 115200, UART_PARITY_DISABLED, UART_STOP_BIT_1,
	UART_BIT_ORDER_LSB); //uart1��ʼ��

	pit_init(PIT_CH0, 5);  //pit0��ʼ��������5ms
	pit_init(PIT_CH1, 89478);
	rng_init();

	temp_sensor_init();

	oled_init();
	custom_oled_display_init();

	camera_init(raw_img);

//	dma_init(1,			//����ͨ��
//				DMA_REQ_PORTB,			//��������Դ
//				DMA_MODE_NORMAL,		//����Ϊ����ģʽ
//				1,						//ÿ�θ�ѭ������1���ֽ�
//				1,	//��ѭ��������������ԭʼͼ���ֽ���
//				(uint32)src,	//����Դ��ַ
//				DMA_DATA_WIDTH_BYTE_1,	//ÿ�δ�Դ��ַ��ȡ1���ֽ�
//				0,						//Դ��ַ����ȡ��ƫ��
//				DMA_MODULO_DISABLED,	//����ģ������
//				0,						//��ѭ����ɺ�Դ��ַ��ƫ��
//				(uint32) dest,		//����Ŀ���ַ
//				DMA_DATA_WIDTH_BYTE_1,	//ÿ����Ŀ���ַд��1���ֽ�
//				0,						//Ŀ���ַ��д���ƫ��1���ֽ�
//				DMA_MODULO_DISABLED,	//����ģ������
//				0,	//��ѭ����ɺ�Ŀ���ַ�ָ�Ϊ��ʼĿ���ַ
//				false);					//ʹ����ѭ����ɺ��Զ�������DMA����
//	gpio_init(COM_PORTB|0,GPIO_DIR_INPUT,GPIO_LEVEL_LOW);
//	gpio_enable_dma(COM_PORTB|0,GPIO_DMA_RISING_EDGE);

//	gpio_init(COM_PORTB|0,GPIO_DIR_OUTPUT,GPIO_LEVEL_LOW);

	//4. ���йر�������ֵ
	time0_flag.f_1s = 0;
	time0_flag.f_50ms = 0;

	//5. ʹ��ģ���ж�
	pit_enable_int(PIT_CH0);   		//ʹ��pit�ж�
	uart_enable_re_int(UART_USE);   //ʹ��uart1�����ж�
	camera_enable_collect_done_int();
	camera_enable_vsync_int();

	//6. �����ж�
	ENABLE_INTERRUPTS;

	//������ѭ��
	//��ѭ����ʼ==================================================================
	for (;;) {
		if (time0_flag.f_50ms) {
			camera_enable_vsync_int();
			time0_flag.f_50ms = 0;
		}
		if (time0_flag.f_1s) {
			time0_flag.f_1s = 0;
			light_change(LIGHT_BLUE);

			start = pit_get_time_us(1);

//			custom_oled_update_temp();
//			oled_printf(0,4,"%X",gpio_get_int(COM_PORTB|0));
//			oled_printf(0,6,"%X",DMA_HRS);


			end = pit_get_time_us(1);
//			uart_printf(UART_USE, "����ʱ�䣺%dus\r\n", end - start);

		}

	} //��ѭ��end_for
//��ѭ������==================================================================
}
