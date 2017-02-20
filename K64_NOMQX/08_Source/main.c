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
	for(i=0;i<10240;i++){
		src[i] = i%256;
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


	dma_init(DMA_CH1, DMA_REQ_ALWAYS_EN0, DMA_MODE_NORMAL, 16, 6, (uint32) src1,
		DMA_DATA_WIDTH_BYTE_1, 1, DMA_MODULO_DISABLED, -16*6, (uint32) dest1,
		DMA_DATA_WIDTH_BYTE_1, 1, DMA_MODULO_DISABLED, -16*6, false);
	dma_enable_req(1);


	dma_init(DMA_CH2, DMA_REQ_ALWAYS_EN0, DMA_MODE_NORMAL, 1, 6, (uint32) src1,
			DMA_DATA_WIDTH_BYTE_1, 1, DMA_MODULO_DISABLED, -6, (uint32) dest1,
			DMA_DATA_WIDTH_BYTE_1, 1, DMA_MODULO_DISABLED, -6, false);
		dma_enable_req(2);

	dma_init(DMA_CH0, DMA_REQ_DISABLED, DMA_MODE_NORMAL, 1, 511, (uint32) src,
	DMA_DATA_WIDTH_BYTE_1, 1, DMA_MODULO_DISABLED, -1, (uint32) dest,
	DMA_DATA_WIDTH_BYTE_1, 1, DMA_MODULO_DISABLED, -1, false);
//	dma_set_arbitration_mode(DMA_ARBITRATION_MODE_ROUND_ROBIN);
//	for(i=0;i<16;i++){
//		dma_set_channel_preemption(i,true);
//		dma_set_preempt_ability(i,true);
//	}
//	dma_set_priority(0,15);
//	dma_set_priority(15,0);
//	dma_set_minor_link(0,true,1);
//	dma_set_major_link(0,true,2);
	dma_enable_req(0);

	//4. ���йر�������ֵ
	time0_flag.f_1s = 0;
	time0_flag.f_50ms = 0;

	//5. ʹ��ģ���ж�
	pit_enable_int(PIT_CH0);   		//ʹ��pit�ж�
	uart_enable_re_int(UART_USE);   //ʹ��uart1�����ж�
	dma_enable_major_int(0);
//	dma_enable_major_int(1);
//	dma_enable_major_int(2);

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

			custom_oled_update_temp();

//			dma_show_pri();
//			uart_printf(1, "ͨ��0��ѭ��ʣ���������:%d\r\n", dma_get_major_loop_iteration_cnt(0));
			uart_printf(1, "ͨ��1��ѭ��ʣ���������:%d\r\n", dma_get_major_loop_iteration_cnt(1));
			uart_printf(1, "ͨ��2��ѭ��ʣ���������:%d\r\n", dma_get_major_loop_iteration_cnt(2));
			uart_printf(1, "ͨ��1��ǰԴ  ��ַ:%X\r\n", dma_get_src_addr(1));
			uart_printf(1, "ͨ��1��ǰĿ���ַ:%X\r\n", dma_get_dest_addr(1));

//			dma_set_src_addr(0,(uint32)src);
//			dma_set_dest_addr(0,(uint32)dest);

//			memset(src, 0xFF, 1024);

			uart_printf(1, "����:%X\r\n", DMA_ES);
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
