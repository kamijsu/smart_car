//===========================================================================
//文件名称：isr.c
//功能概要：自定义中断服务例程源文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//版本更新：2013-05-03    V1.0,2014-05-10    V2.3(WYH)
//===========================================================================

#include "includes.h"

//==========================================================================
//函数名称：UART1_RX_TX_IRQHandler
//参数说明：无
//函数返回：无
//功能概要：UART1中断服务函数
//==========================================================================
void UART1_RX_TX_IRQHandler() {
	FrameFramingResult res;
	uint8 ch;
	bool err = true;
	DISABLE_INTERRUPTS;

//	res = frame_framing();
//	uart_send1(UART_MOD1, res);

	if (uart_re1_parity(UART_MOD1, &ch, &err)) {
		if (!err) {
//			switch (ch)
//			{
//			case 'n':
//				menu_oled_next_item(); break;
//			case 'l':
//				menu_oled_last_item(); break;
//			case 's':
//				menu_oled_select(); break;
//			case 'o':
//				menu_oled_save(); break;
//			case 'r':
//				menu_oled_reset(); break;
//			default: break;
//			}

//			for (uint32 i = 0; i < sizeof(raw_img); i++){
//				uart_printf(1, "%X ", raw_img[i]);
//			}
//			ch = spi_master_send(SPI_MOD2, SPI_CONFIG0, SPI_CS0, ch,
//					SPI_CONT_DISABLE);
			uart_send1(UART_MOD1, ch);
//			oled_write_data(ch);
		}

	}

	ENABLE_INTERRUPTS;
}

void PORTA_IRQHandler() {
	static uint32 i = 0;
	uint8 key;

	DISABLE_INTERRUPTS;

	oled_printf(0, 4, "time:%d", i++);

	if (keyboard_get_int()) {

		if (keyboard_read(&key)) {
			oled_printf(0, 2, "key:%c   ", key);
		} else {
			oled_printf(0, 2, "key:null");
		}
		keyboard_clear_int();
	}

	ENABLE_INTERRUPTS;
}

void PORTC_IRQHandler() {
	DISABLE_INTERRUPTS;
	if (camera_get_vsync_int()) {
		camera_start_collecting();
		camera_disable_vsync_int();
		camera_clear_vsync_int();
	}
	ENABLE_INTERRUPTS;
}

void DMA0_IRQHandler() {
	DISABLE_INTERRUPTS;
	camera_stop_collecting();
	camera_clear_collect_done_int();
	raw_img_done = true;
	ENABLE_INTERRUPTS;
}

void DMA1_IRQHandler() {
	DISABLE_INTERRUPTS;

	if (dma_get_major_int(1)) {
		dma_clear_major_int(1);
		uart_printf(1, "DMA1主循环完成！\r\n");

	}

	ENABLE_INTERRUPTS;
}

void DMA2_IRQHandler() {
	DISABLE_INTERRUPTS;

	if (dma_get_major_int(2)) {
		dma_clear_major_int(2);
		uart_printf(1, "DMA2主循环完成！\r\n");

	}

	ENABLE_INTERRUPTS;
}

void DAC0_IRQHandler() {
	DISABLE_INTERRUPTS;
	if (dac_get_index_top_int(0)) {
		dac_clear_index_top_int(0);
		uart_printf(1, "索引到达0\r\n");
	}
	if (dac_get_index_bottom_int(0)) {
		dac_clear_index_bottom_int(0);
		uart_printf(1, "索引到达上限\r\n");
	}
	if (dac_get_watermark_int(0)) {
		dac_clear_watermark_int(0);
		uart_printf(1, "索引到达水印\r\n");
	}
	ENABLE_INTERRUPTS;
}

void SPI0_IRQHandler() {
	uint32 data;
	DISABLE_INTERRUPTS;
	if (spi_slave_re(SPI_MOD0, &data)) {
		uart_printf(1, "SPI0接收到数据:%X\r\n", data);
		if (!spi_slave_send(SPI_MOD0, data)) {
			uart_send_string(UART_USE, "SPI发送失败!\r\n");
		}
	} else {
		uart_send_string(UART_USE, "SPI接收失败!\r\n");
	}
	ENABLE_INTERRUPTS;
}

void I2C0_IRQHandler() {
	static uint8 time = 0;

	I2CSlaveIntType int_type;
	uint8 data;

	DISABLE_INTERRUPTS;

	int_type = i2c_slave_handle_int(0);
//	uart_printf(1,"从机0中断类型:%d\r\n",int_type);
	switch (int_type) {
	case I2CSlaveDataReInt:
		data = i2c_slave_re(0);
//		uart_printf(1, "从机0接收数据:%X\r\n", data);
		break;
	case I2CSlaveCalledSendInt:
		time = 0;
		i2c_slave_send(0, time * 0x11);
		//		uart_printf(1, "叫我发数据了\r\n");
		break;
	case I2CSlaveDataSendInt:
		time = (time + 1) % 16;
//		data = rng_next_uint8();
		i2c_slave_send(0, time * 0x11);
		//		uart_printf(1, "从机2发送数据:%X\r\n", data);
		break;
	}

	ENABLE_INTERRUPTS;
}

void I2C2_IRQHandler() {
	static uint8 time = 0;
	static bool re = true;
	I2CSlaveIntType int_type;
	I2CMasterResult result;
	uint8 data;

	DISABLE_INTERRUPTS;

	int_type = i2c_slave_handle_int(2);
//	uart_printf(1, "从机2中断类型:%d\r\n", int_type);
	switch (int_type) {
	case I2CSlaveCalledReInt:
//		i2c_slave_set_ack(2,false);
		break;
	case I2CSlaveDataReInt:
//		i2c_slave_set_ack(2,false);
		//不读取数据会导致总线被占据，从而无法进行下一次传输
		data = i2c_slave_re(2);
		uart_printf(1, "从机2接收数据:%X\r\n", data);
		break;
	case I2CSlaveCalledGeneralInt:
		break;
	case I2CSlaveCalledSendInt:
		time = 0;
		i2c_slave_send(2, time * 0x11);
//		uart_printf(1, "叫我发数据了\r\n");
		break;
	case I2CSlaveDataSendInt:
		time = (time + 1) % 16;
		//		data = rng_next_uint8();
		i2c_slave_send(2, time * 0x11);
		break;
	}

	ENABLE_INTERRUPTS;
}

void FTM0_IRQHandler() {
	DISABLE_INTERRUPTS;
	if (ftm_timer_get_int(0)) {
		ftm_timer_clear_int(0);
//		time0_flag.f_1s = 1;
//		uart_printf(1,"!!\r\n");
	}
	ENABLE_INTERRUPTS;
}

//==========================================================================
//函数名称：PIT0_IRQHandler
//参数说明：无
//函数返回：无
//功能概要：PIT0中断服务函数
//==========================================================================
void PIT0_IRQHandler() {
	DISABLE_INTERRUPTS; //关中断

	pit_clear_int(PIT_CH0); //清标志

	time_update(5);

	ENABLE_INTERRUPTS; //恢复原总中断设置情况
}

void PIT2_IRQHandler() {
	pit_clear_int(PIT_CH2); //清标志
}

//============================================================================
//函数名称：PORTD_IRQHandler
//函数参数：
//函数返回：无
//功能概要：PORTD端口中断服务函数
//============================================================================
void PORTD_IRQHandler() {
	uint8_t n;    //引脚号

	DISABLE_INTERRUPTS; //关中断

	if (reed_switch_get_int()) {
		reed_switch_clear_int();
		reed_switch_disable_int();
		uart_send_string(UART_USE, "产生中断！\n");
	}

	ENABLE_INTERRUPTS; //恢复原总中断设置情况
}
