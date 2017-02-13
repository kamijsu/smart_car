//˵���������ļ����µ�Doc�ļ�����Readme.txt�ļ�
//============================================================================

#define GLOBLE_VAR  //ֻ����main.c�ж���һ�Σ�������ֹȫ�ֱ������ظ�����

#include "includes.h"   //������ͷ�ļ�

int main(void) {
	//1. ����������ʹ�õı���
	uint8 light_state;		//��ǰ��״̬
	uint32 crc;
	uint8 data[1024];
	uint32 stream;
	uint32 reg;
	uint32 i, j;
	float temp;
	FrameInfo frame;
	FrameCmdInfo cmd;
	double d;
	float f;
	uint16 crcres;
	uint8 uvar8;
	uint16 uvar16;
	uint32 uvar32;
	uint32 uvar322;
	int8 var8;
	int16 var16;
	int32 var32;

	/* С����ز������� */
	Car car;
	float display;
	/*���ڵ��Թ�������ʱ����*/
	float data_out[4];

	//2. �����ж�
	DISABLE_INTERRUPTS;

	//3. ��ʼ������ģ��
	light_init(LIGHT_BLUE, LIGHT_ON); //���Ƴ�ʼ��
//	light_init(LIGHT_GREEN, LIGHT_OFF);
//	light_init(LIGHT_RED, LIGHT_OFF);
			//light_init(LIGHT_BLUE, LIGHT_OFF);
//	uart_init(UART_USE, 9600, UART_PARITY_DISABLED, UART_STOP_BIT_1,
//	UART_BIT_ORDER_LSB); //uart1��ʼ���������ã�����ģ�鲨����9600���޷���5ms�ж��д�������
//	uart_init(UART_USE, 115200);   //uart1��ʼ����������
	pit_init(PIT_CH0, 5);  //pit0��ʼ��������5ms
	pit_init(PIT_CH1, 89478);
//	gpio_init(COM_PORTC | 0, GPIO_OUTPUT, GPIO_LEVEL_LOW);
//	gpio_init(COM_PORTB | 19, GPIO_OUTPUT, GPIO_LEVEL_LOW);
//	pit_init(PIT_CH2,1);
	rng_init();
	temp_sensor_init();
//	motor_init(MOTOR0);			//������ʼ��
//	motor_init(MOTOR1);			//�ҵ����ʼ��
//	gyro_acce_init();			//�����Ǽ��ٶȼƳ�ʼ��
//	encoder_init(ENCODER0);		//���������ʼ��
//	encoder_init(ENCODER1);		//�ұ�������ʼ��
//	ems_init();					//��Ŵ�������ʼ��
//	reed_switch_init();			//�ɻɹܳ�ʼ��
//ftm_init(FTM_MOD0,FTM_CLK_DIV_128,FTM_COUNTER_MODE_UP,100000);
//
//ftm_pwm_single_init(FTM_MOD0,FTM_CH4,FTM_PWM_MODE_EDGE_ALIGNED,FTM_PWM_POL_NEGATIVE,5000);

//ftm_oc_init(FTM_MOD0,FTM_CH4,FTM_OC_MODE_TOGGLE,5000);

//	crc_init_protocol(CRC_CRC16_MODBUS);
	frame_init();
//	spi_master_init(SPI_MOD2, SPI_CONFIG0, SPI_BAUD_SCALER_12,
//	SPI_FORMAT_CPOL0_CPHA0, 8, SPI_BIT_ORDER_MSB,
//	SPI_DELAY_SCALER_12, SPI_DELAY_SCALER_12,
//	SPI_DELAY_SCALER_12);
	oled_init();
	custom_oled_display_init();

	i2c_init(I2C_MOD0, I2C_MUL_1, 0x3F, I2C_ADDR_MODE_BITS_10, 0x10, true);
	i2c_init(I2C_MOD2, I2C_MUL_1, 0x3F, I2C_ADDR_MODE_BITS_10, 0x012, true);

//	i2c_slave_set_ack(2,false);

//	gpio_init(COM_PORTA|13,GPIO_DIR_OUTPUT,GPIO_LEVEL_LOW);
//	REG_SET_MASK(I2C2_C1,I2C_C1_TX_MASK);
//	i2c_slave_set_ack(2,false);
//	REG_SET_MASK(I2C2_C1,I2C_C1_TX_MASK);

	//4. ���йر�������ֵ
	time0_flag.f_1s = 0;
	time0_flag.f_50ms = 0;
//	d = 1.3;
	f = -13.3f;
	//5. ʹ��ģ���ж�
	pit_enable_int(PIT_CH0);   		//ʹ��pit�ж�
//	pit_enable_int(PIT_CH2);   		//ʹ��pit�ж�
//	uart_enable_re_int(UART_USE);   //ʹ��uart1�����ж�
	i2c_slave_enable_int(I2C_MOD0);
	i2c_slave_enable_int(I2C_MOD2);
	frame_enable_re_int();
//	reed_switch_enable_int();
//	ftm_timer_enable_int(FTM_MOD0,10);

	frame.len = 255;
	for (i = 0; i <= 15; i++) {
		data[i] = 0x11 * i;
	}
//	memset(data, 0xEE, 1024);
	uint8 read_data[1024];
	memset(read_data, 0xCC, 1024);

	cmd.type = 0x00;
	cmd.len = 1;
	cmd.data[0] = 7;

	uint8 re_data[1024];
	uint32 num = 1024;

//	result = flash_partition(FLASH_DFLASH_SIZE_32, FLASH_EEPROM_SIZE_128,
//			FLASH_EEPROM_SPLIT_1_7);
//	uart_send1(UART_USE, result);
	I2CMasterResult result;
	uint8 mod = 0;
	uint8 addr_mode = 10;
	uint16 addr = 0x12;

	//6. �����ж�
	ENABLE_INTERRUPTS;

	//������ѭ��
	//��ѭ����ʼ==================================================================
	for (;;) {

		if (rng_next_bool()) {
			if (rng_next_bool()) {
				result = i2c_master_send(mod, addr_mode, addr, data, num);
				uart_printf(1, "����%d���ͽ��:%d\r\n", mod, result);
			} else {
				memset(re_data, 0xFE, 1024);
				result = i2c_master_re(mod, addr_mode, addr, re_data, num);
				uart_printf(1, "����%d���ս��:%d\r\n", mod, result);
				if (result == I2CMasterSuccess) {
					for (i = 0; i < num; i++) {
						j = i % 16;
						if (re_data[i] != 0x11 * j) {
							uart_printf(1, "�д������ݽ��յ�\r\n");
							break;
						}
					}
				}
			}
		} else {
			if (rng_next_bool()) {
				memset(re_data, 0xFE, 1024);

				result = i2c_master_send_re(mod, addr_mode, addr, data, num,
						re_data, num);

				uart_printf(1, "����%d���ͽ��ս��:%d\r\n", mod, result);
				if (result == I2CMasterSuccess) {
					for (i = 0; i < num; i++) {
						j = i % 16;
						if (re_data[i] != 0x11 * j) {
							uart_printf(1, "�д������ݽ��յ�\r\n");
							break;
						}
					}
				}
			} else {

			}
		}
		pit_delay_ms(1, rng_next_uint8());

//		uint8 re_data[1024];
//		memset(re_data, 0xFE, 1024);
//		result = i2c_master_send_re(0, 0x12, data, 1024, re_data, 1024);
//
//		uart_printf(1, "����0���:%d\r\n", result);
//		if (result == I2CMasterSuccess) {
//			for (i = 0; i < 50; i++) {
//				j=i%16;
//				if (re_data[i] != 0x11 * j) {
//					uart_printf(1, "�д������ݽ��յ�\r\n");
//					break;
//				}
//			}
//		}
//		pit_delay_ms(1,rng_next_uint8());
//		uart_printf(1,"����״̬:%X\r\n",I2C0_S);
//		uart_printf(1,"�������ƼĴ���:%X\r\n",I2C0_C1);
		if (time0_flag.f_50ms) {
			time0_flag.f_50ms = 0;
		}
		if (time0_flag.f_1s) {
			time0_flag.f_1s = 0;
			light_change(LIGHT_BLUE);

			uvar32 = pit_get_time_us(1);
//			I2C0_S = 0xFF;
//			uint8 dummy = I2C0_D;
//			uart_printf(1,"dummy:%X\r\n",dummy);

//			result = i2c_master_send_re(0, 0x12, data, 1, re_data, 1);
//			for (i = 0; i < 3; i++) {
//				uart_printf(1, "����0���յ�����:%X\r\n", re_data[i]);
//			}

//			result = i2c_master_send(0, 1, 0x00, data, 3);
//			uart_printf(1, "����0���:%d\r\n", result);
//			result = i2c_master_send(0, 1, 0x301, data, 3);
//			uart_printf(1, "����0���:%d\r\n", result);

//			memset(re_data, 0xFE, 1024);
//
//			result = i2c_master_send_re(0, 7, 0x012, data, num, re_data, num);
//
//			uart_printf(1, "����0���:%d\r\n", result);
//			if (result == I2CMasterSuccess) {
//				for (i = 0; i < num; i++) {
//					j = i % 16;
//					if (re_data[i] != 0x11 * j) {
//						uart_printf(1, "�д������ݽ��յ�\r\n");
//						break;
//					}
//				}
//			}
//
//			result = i2c_master_send(0, 7, 0x012, data, num);
//			uart_printf(1, "����0���:%d\r\n", result);
//
//			memset(re_data, 0xFE, 1024);
//			result = i2c_master_re(0, 7, 0x012, re_data, num);
//
//			uart_printf(1, "����0���:%d\r\n", result);
//			if (result == I2CMasterSuccess) {
//				for (i = 0; i < num; i++) {
//					j = i % 16;
//					if (re_data[i] != 0x11 * j) {
//						uart_printf(1, "�д������ݽ��յ�\r\n");
//						break;
//					}
//				}
//			}
//
//			result = i2c_master_send_re(0, 7, 0x012, data, num, re_data, num);
//
//			uart_printf(1, "����0���:%d\r\n", result);
//			if (result == I2CMasterSuccess) {
//				for (i = 0; i < num; i++) {
//					j = i % 16;
//					if (re_data[i] != 0x11 * j) {
//						uart_printf(1, "�д������ݽ��յ�\r\n");
//						break;
//					}
//				}
//			}

//			result = i2c_master_send(0,0x12,data,3);
//			result = i2c_master_send(0,0x12,data,3);
//			result = i2c_master_send(2,0x10,data,3);
//			uart_printf(1, "%d\r\n", result);
//			result = i2c_master_send(2,0x00,data,10);
//			uart_printf(1,"%d\r\n",result);
			oled_fill(0x00);
			oled_printf(0, 4, "I2C2S:%X", I2C2_S);

			oled_printf(0, 6, "SDA:%d SCL:%d", gpio_get_level(COM_PORTB | 1),
					gpio_get_level(COM_PORTB | 0));
			oled_printf(0, 2, "I2C0S:%X", I2C0_S);

//			custom_oled_update_temp();

			uvar322 = pit_get_time_us(1);
//			uart_printf(1,"����ʱ�䣺%dus\r\n", (int32) (uvar322 - uvar32));

		}

//		//������յ���֡
//		if (frame_get_info(&frame)) {
//			//����֡����������Ӧ����
//			switch (frame.type) {
//			case FRAME_STRING_TYPE:	//�ַ���֡
//				//�����ַ�����ȥ
////				frame_send_info(frame);
//				frame_string_parse(frame, buff);
//				uart_printf(UART_USE, "%s", buff);
//				break;
//			case FRAME_CMD_TYPE:	//����֡
//				//��֡����ȡ������Ϣ
//				if (frame_cmd_parse(frame, &cmd) == CmdParseSuccess) {
//					//��֡�ɹ�ʱ������������������Ӧ����
//					switch (cmd.type) {
//					case 0x00:	//���õ�״̬
//
//						//light_stateλ0��ʾ����״̬��λ1��ʾ�̵�״̬��λ2��ʾ���״̬
//						cmd.data[0] & 0x1 ?
//								light_set(LIGHT_BLUE, LIGHT_ON) :
//								light_set(LIGHT_BLUE, LIGHT_OFF);
//						cmd.data[0] & 0x2 ?
//								light_set(LIGHT_GREEN, LIGHT_ON) :
//								light_set(LIGHT_GREEN, LIGHT_OFF);
//						cmd.data[0] & 0x4 ?
//								light_set(LIGHT_RED, LIGHT_ON) :
//								light_set(LIGHT_RED, LIGHT_OFF);
//						break;
//					}
//				}
//				break;
//			}
//		}

	} //��ѭ��end_for
//��ѭ������==================================================================
}
