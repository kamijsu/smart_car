//说明见工程文件夹下的Doc文件夹内Readme.txt文件
//============================================================================

#define GLOBLE_VAR  //只需在main.c中定义一次，用来防止全局变量的重复定义

#include "includes.h"   //包含总头文件

int main(void) {
	//1. 声明主函数使用的变量
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

	//2. 关总中断
	DISABLE_INTERRUPTS;

	//3. 初始化外设模块
	light_init(LIGHT_BLUE, LIGHT_ON); //蓝灯初始化
	uart_init(UART_USE, 115200, UART_PARITY_DISABLED, UART_STOP_BIT_1,
	UART_BIT_ORDER_LSB); //uart1初始化

	pit_init(PIT_CH0, 5);  //pit0初始化，周期5ms
	pit_init(PIT_CH1, 89478);
	rng_init();

	temp_sensor_init();

	oled_init();
	custom_oled_display_init();

	dma_init(DMA_CH0, DMA_REQ_PORTB, DMA_MODE_NORMAL, 1, 1, (uint32) src,
	DMA_DATA_WIDTH_BYTE_1, 1, DMA_MODULO_DISABLED, -1, (uint32) dest,
	DMA_DATA_WIDTH_BYTE_1, 1, DMA_MODULO_DISABLED, -1, false);
	dma_enable_req(0);

	gpio_init(COM_PORTB|0,GPIO_DIR_INPUT,GPIO_LEVEL_LOW);

	//4. 给有关变量赋初值
	time0_flag.f_1s = 0;
	time0_flag.f_50ms = 0;

	//5. 使能模块中断
	pit_enable_int(PIT_CH0);   		//使能pit中断
	uart_enable_re_int(UART_USE);   //使能uart1接收中断
	dma_enable_major_int(0);

	//6. 开总中断
	ENABLE_INTERRUPTS;

	//进入主循环
	//主循环开始==================================================================
	for (;;) {
		if (time0_flag.f_50ms) {
			time0_flag.f_50ms = 0;
		}
		if (time0_flag.f_1s) {
			time0_flag.f_1s = 0;
			light_change(LIGHT_BLUE);

			start = pit_get_time_us(1);

			custom_oled_update_temp();

			end = pit_get_time_us(1);
//			uart_printf(UART_USE, "消耗时间：%dus\r\n", end - start);

		}

	} //主循环end_for
//主循环结束==================================================================
}
