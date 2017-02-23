//˵���������ļ����µ�Doc�ļ�����Readme.txt�ļ�
//============================================================================

#define GLOBLE_VAR  //ֻ����main.c�ж���һ�Σ�������ֹȫ�ֱ������ظ�����

#include "includes.h"   //������ͷ�ļ�

int main(void) {
	//1. ����������ʹ�õı���
	uint32 start, end;
	uint32 i, j;

	uint8 src[1024];
	memset(src, 0x45, 1024);
	uint8 dest[1024];
	memset(dest, 0xFF, 1024);

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
	menu_oled_display();
//	custom_oled_display_init();

	dma_init(DMA_CH0, DMA_REQ_DISABLED, DMA_MODE_NORMAL, 4, 7, (uint32) src,
	DMA_DATA_WIDTH_BYTE_1, 1, DMA_MODULO_DISABLED, -28, (uint32) dest,
	DMA_DATA_WIDTH_BYTE_1, 1, DMA_MODULO_DISABLED, -28, false);

	dma_enable_req(0);

	//4. ���йر�������ֵ
	time0_flag.f_1s = 0;
	time0_flag.f_50ms = 0;

	//5. ʹ��ģ���ж�
	pit_enable_int(PIT_CH0);   		//ʹ��pit�ж�
	uart_enable_re_int(UART_USE);   //ʹ��uart1�����ж�
	dma_enable_major_int(0);

	//6. �����ж�
	ENABLE_INTERRUPTS;

	//������ѭ��
	//��ѭ����ʼ==================================================================
	for (;;) {
		if (time0_flag.f_50ms) {
			time0_flag.f_50ms = 0;
		}
		if (time0_flag.f_1s) {
			time0_flag.f_1s = 0;
			light_change(LIGHT_BLUE);

			start = pit_get_time_us(1);

//			custom_oled_update_temp();
//			uart_printf(1, "��ѭ������ж�:%d\r\n", dma_get_major_int(0));

//			memset(src, 0xFF, 1024);

//			uart_printf(1, "����:%X\r\n", DMA_ERR);
//			uart_printf(1, "ͨ��0״̬���ƼĴ���:%X\r\n", DMA_CSR(0));

			for (i = 0; i < 1024; i++) {
				if (dest[i] != 0xFF) {
//					uart_printf(1, "Ŀ���ַ��%2d���ֽ�:%X\r\n", i, dest[i]);
				}
			}

			end = pit_get_time_us(1);
//			uart_printf(UART_USE, "����ʱ�䣺%dus\r\n", end - start);

		}

	} //��ѭ��end_for
//��ѭ������==================================================================
}
