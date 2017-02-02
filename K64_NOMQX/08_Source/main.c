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
	uint32 addr;
	uint8* ptr;
	uint16 offset;
	uint16 num;
	uint8 sector;
	FlashResult result;
	uint8 dflash_size;
	uint8 eeprom_size;
	uint8 eeprom_split;
	uint32* ptr32;
	uint8 buff[1024];

	/* С����ز������� */
	Car car;
	float display;
	/*���ڵ��Թ�������ʱ����*/
	float data_out[4];

	//2. �����ж�
	DISABLE_INTERRUPTS;
//vsprintf()
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
	//4. ���йر�������ֵ
	time0_flag.f_1s = 0;
	time0_flag.f_50ms = 0;
//	d = 1.3;
	f = -13.3f;
	//5. ʹ��ģ���ж�
	pit_enable_int(PIT_CH0);   		//ʹ��pit�ж�
//	pit_enable_int(PIT_CH2);   		//ʹ��pit�ж�
//	uart_enable_re_int(UART_USE);   //ʹ��uart1�����ж�
	frame_enable_re_int();
//	reed_switch_enable_int();
//	ftm_timer_enable_int(FTM_MOD0,10);

	frame.len = 255;
	memset(data, 0xEE, 1024);
//	FMC_PFB0CR |= FMC_PFB0CR_S_B_INV_MASK;
	uint8 read_data[1024];
	memset(read_data, 0xCC, 1024);

	cmd.type = 0x00;
	cmd.len = 1;
	cmd.data[0] = 7;

//	oled_display_str(0, 0, "123");
//	oled_display_str(0, 2, "happy new year:)");
//	oled_display_str(0, 6, "happy new year:)");
//	oled_set_scroll(OLED_SCROLL_DIR_RIGHT, OLED_PAGE0, OLED_PAGE1,
//	OLED_SCROLL_INTERVAL_FRAMES_2);
//	oled_set_start_line(16);
//	oled_set_scroll_with_vertical(OLED_SCROLL_DIR_RIGHT, 2, 3,
//			OLED_SCROLL_INTERVAL_FRAMES_25, 0, 16, 1);
//	oled_scroll_start();

	i = 1;

//	result = flash_partition(FLASH_DFLASH_SIZE_32, FLASH_EEPROM_SIZE_128,
//			FLASH_EEPROM_SPLIT_1_7);
//	uart_send1(UART_USE, result);

	oled_set_scroll(OLED_SCROLL_DIR_LEFT, 0, 1, OLED_SCROLL_INTERVAL_FRAMES_2);
	oled_set_contrast(255);
//	oled_set_display_clk(15,2);
//	oled_set_precharge_period(8,8);
//	oled_set_remap(false,false);
//

//	oled_display_str(0, 2, "123");

//	oled_set_multiplex_ratio(48);
//	oled_display_str(0, 0, "1");
//	oled_display_str(0, 2, "2");
//	oled_display_str(0, 4, "3");
//	oled_display_str(0, 6, "4");
//	oled_set_display_offset(16);
	display = 11594945.35874f;
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

			uvar32 = pit_get_time_us(1);

//			display = arm_sin_f32(display);
//			f/=2.0f;

//
//			ptr32 = (uint32*)malloc(sizeof(uint32)*30720);
//			if(ptr32 == NULL){
//				uart_send_string(1,"�ڴ�����!\r\n");
//			}
//			else{
//				uart_send_string(1,"����ɹ�!\r\n");
//			}
//			free(ptr32);
//			i = rng_next_uint32();
//			var32 = uart_printf(1, "%d.%03d\r\n", (int32)f,(uint32)((f*1000 - (int32)f * 1000) * -1));
//			var32 = uart_printf(1, "%#X\r\n", rng_next_uint32());
//			var32 = uart_printf(1, "%d\r\n", (int32)(f*1000));
//			var32 = uart_printf(1, "���Ĳ���");

			float fval;		//Ҫ���͵ĸ�����
			int32 d;			//��������������
			int32 temp;		//��ʱ����
			uint32 f;		//������С������

			fval = display;

			//��ȡ��������������
			d = (int32) fval;
			//��ȡ������С�����֣�����Ϊ3λ
			temp = (int32) ((fval * 1000) - d * 1000);
			f = temp > 0 ? temp : -temp;
			//��ֵΪ(-1.0f,0.0f)ʱ����Ҫ���ⷢ�͸���
			if (fval < 0.0f && fval > -1.0f) {
				uart_printf(UART_MOD1, "-");
			}
			//���͸�ʽ����ĸ�����
			uart_printf(UART_MOD1, "%d.%03u\r\n", d, f);

			display /= -2.0f;

//			uart_printf(1, "%d\r\n", var32);
//			uart_send_string(1,"12345678901234567890123456789012345678901234567890");
//			__VALIST
//			va_start();
//			if (i == 1) {
//				oled_set_contrast(0);
////				oled_scroll_stop();
//				i = 0;
//			} else {
//				oled_set_contrast(255);
////				oled_scroll_start();
//				i = 1;
//			}

//			oled_display_str(0, 0, "[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~");
//			oled_scroll_start();
//			pit_delay_ms(1, 1000);
//
//
//
//			oled_scroll_stop();
//			pit_delay_ms(1, 1000);

			uvar322 = pit_get_time_us(1);
//			uart_printf(1,"\r\n%d\r\n", (int32) (uvar322 - uvar32));

		}

//		//������յ���֡
//		if (frame_get_info(&frame)) {
//			//����֡����������Ӧ����
//			switch (frame.type) {
//			case FRAME_STRING_TYPE:	//�ַ���֡
//				//�����ַ�����ȥ
//				frame_send_info(frame);
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
