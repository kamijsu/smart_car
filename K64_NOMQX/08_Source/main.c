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

	/* С����ز������� */
	Car car;

	/*���ڵ��Թ�������ʱ����*/
	float data_out[4];

	//2. �����ж�
	DISABLE_INTERRUPTS;

	//3. ��ʼ������ģ��
	light_init(LIGHT_BLUE, LIGHT_ON); //���Ƴ�ʼ��
	light_init(LIGHT_GREEN, LIGHT_OFF);
	light_init(LIGHT_RED, LIGHT_OFF);
	//light_init(LIGHT_BLUE, LIGHT_OFF);
//	uart_init(UART_USE, 9600, UART_PARITY_DISABLED, UART_STOP_BIT_1,
//	UART_BIT_ORDER_LSB); //uart1��ʼ���������ã�����ģ�鲨����9600���޷���5ms�ж��д�������
//	uart_init(UART_USE, 115200);   //uart1��ʼ����������
	pit_init(PIT_CH0, 5);  //pit0��ʼ��������5ms
	pit_init(PIT_CH1, 89478);
	rng_init();
//	motor_init(MOTOR0);			//������ʼ��
//	motor_init(MOTOR1);			//�ҵ����ʼ��
//	gyro_acce_init();			//�����Ǽ��ٶȼƳ�ʼ��
	encoder_init(ENCODER0);		//���������ʼ��
//	encoder_init(ENCODER1);		//�ұ�������ʼ��
//	ems_init();					//��Ŵ�������ʼ��
//	reed_switch_init();			//�ɻɹܳ�ʼ��
//ftm_init(FTM_MOD0,FTM_CLK_DIV_128,FTM_COUNTER_MODE_UP,100000);
//
//ftm_pwm_single_init(FTM_MOD0,FTM_CH4,FTM_PWM_MODE_EDGE_ALIGNED,FTM_PWM_POL_NEGATIVE,5000);

//ftm_oc_init(FTM_MOD0,FTM_CH4,FTM_OC_MODE_TOGGLE,5000);
	temp_sensor_init();
//	crc_init_protocol(CRC_CRC16_MODBUS);
	frame_init();
	//4. ���йر�������ֵ
	time0_flag.f_1s = 0;
	time0_flag.f_50ms = 0;
//	d = 1.3;
//	f = 1.3f;
	//5. ʹ��ģ���ж�
	pit_enable_int(PIT_CH0);   		//ʹ��pit�ж�
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

//	result = flash_partition(FLASH_DFLASH_SIZE_0, FLASH_EEPROM_SIZE_64,
//			FLASH_EEPROM_SPLIT_1_3);
//	uart_send1(UART_USE, result);

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

			uvar32 = pit_get_time(1);

//			reg = SIM_FCFG1;
////			reg = *(uint32*) 0x000003FD;
//			uart_send1(UART_USE, reg >> 24);
//			uart_send1(UART_USE, reg >> 16);
//			uart_send1(UART_USE, reg >> 8);
//			uart_send1(UART_USE, reg);

//			reg = FTFE_FSTAT;
//			uart_send1(UART_USE, reg >> 24);
//			uart_send1(UART_USE, reg >> 16);
//			uart_send1(UART_USE, reg >> 8);
//			uart_send1(UART_USE, reg);
//			*(uint32*) 0x14000000 = 0xAAAAAAAA;
//			reg = *(uint32*) 0x14000000;
//			uart_send1(UART_USE, reg >> 24);
//			uart_send1(UART_USE, reg >> 16);
//			uart_send1(UART_USE, reg >> 8);
//			uart_send1(UART_USE, reg);

//			uart_send1(UART_USE,*(uint8*)addr);
//			uart_send1(UART_USE,*(uint8*)(addr+1));
			for (i = 0; i < 128; i++) {
				data[i * 8] = 0x00;
				data[i * 8 + 1] = 0x11;
				data[i * 8 + 2] = 0x22;
				data[i * 8 + 3] = 0x33;
				data[i * 8 + 4] = 0x44;
				data[i * 8 + 5] = 0x55;
				data[i * 8 + 6] = 0x66;
				data[i * 8 + 7] = 0x77;
			}
			sector = 0;
			offset = 0;
			num = 27;

			reg = FTFE_FDPROT;
			uart_send1(UART_USE, reg);

			FTFE_FDPROT = 0x00;

			reg = FTFE_FDPROT;
			uart_send1(UART_USE, reg);

//			REG_CLR_SHIFT(FTFE_FEPROT, 0);
//
////			flash_flexram_set_mode(FLASH_FLEXRAM_MODE_RAM);
////
////			flash_flexram_read(offset, num, read_data);
////			uart_sendN(UART_USE, read_data, num);
////
//			rng_next_bytes(data, num);
//			uart_sendN(UART_USE, data, num);
//
//			flash_flexram_write(offset, num, data);

			flash_flexram_read(offset, num, read_data);
			uart_sendN(UART_USE, read_data, num);
//
//			offset += 8;
//			rng_next_bytes(data, num);
//			uart_sendN(UART_USE, data, num);
//			flash_flexram_write(offset, num, data);
//
//			flash_flexram_read(offset, num, read_data);
//			uart_sendN(UART_USE, read_data, num);
//
//			flash_flexram_set_mode(FLASH_FLEXRAM_MODE_EEPROM);
//			flash_flexram_read(offset, num, read_data);
//			uart_sendN(UART_USE, read_data, num);

//			reg = flash_flexram_get_mode();
//			uart_send1(UART_USE, reg);
//
//			flash_flexram_set_mode(FLASH_FLEXRAM_MODE_RAM);
//
//			reg = flash_flexram_get_mode();
//			uart_send1(UART_USE, reg);

//			reg = FTFE_FCNFG;
//			uart_send1(UART_USE, reg);
//
//			addr = FLASH_ADDR_FLEXRAM;
//			*(uint32*) (addr + 0) = 0x85;
//			while (!(FTFE_FCNFG & FTFE_FCNFG_EEERDY_MASK)) {
////				uart_send_string(1,"����!\r\n");
//			}
//			*(uint32*) (addr + 0) = 0x73;
//			while (!(FTFE_FCNFG & FTFE_FCNFG_EEERDY_MASK)){
////				uart_send_string(1,"����!\r\n");
//			}
//
//			memcpy(read_data, (void*) addr, 26);
//			uart_sendN(UART_USE, read_data, 26);

//			reg = FTFE_FCNFG;
//			uart_send1(UART_USE, reg);
//			read_data[0] = *(uint8*) (addr + 1);
//			uart_send1(UART_USE, read_data[0]);
//			*(volatile uint8*)(addr+1) = 0x56;
//			reg = *(uint8*)FLASH_ADDR_RAM;
//			uart_send1(UART_USE, reg);

//			rng_next_bytes(read_data, 32);

//			memcpy(read_data,(void*)(addr+1),16);
//			uart_sendN(UART_USE, read_data, 16);

//			uart_send1(UART_USE, *(volatile uint8*)(addr+1));
//
//			memcpy(read_data, (void*) addr, 26);
//			uart_sendN(UART_USE, read_data, 26);

//			flash_write(FLASH_BLK_DFLASH, sector, offset, num, data);
//
////			flash_erase_sector(1,0);
//
//			result = flash_check_empty_dflash();
//			uart_send1(UART_USE, result);

//			if (flash_partition_get_info(&dflash_size, &eeprom_size,
//					&eeprom_split)) {
//				uart_send1(UART_USE, dflash_size);
//				uart_send1(UART_USE, eeprom_size);
//				uart_send1(UART_USE, eeprom_split);
//			}
//			result = flash_erase_dflash();
//			uart_send1(UART_USE, result);
//
//			reg = SIM_FCFG1;
//			uart_send1(1,reg>>24);
//			uart_send1(1,reg>>16);
//			uart_send1(1,reg>>8);
//			uart_send1(1,reg);

//			flash_erase_sector(FLASH_BLK_DFLASH, sector);
//			rng_next_bytes(data, 48);
//
////			flash_read(FLASH_BLK_DFLASH, 0, 0, 27, read_data);
////						uart_sendN(UART_USE, read_data, 27);
//			result = flash_write(FLASH_BLK_DFLASH, sector, offset, num, data);
//			uart_send1(UART_USE, result);
////			FMC_PFB0CR |= FMC_PFB0CR_S_B_INV_MASK;
//			flash_read(FLASH_BLK_DFLASH, sector, 0, 26, read_data);
//			uart_sendN(UART_USE, read_data, 26);

//			result = flash_write(FLASH_BLK_DFLASH, 31, 4072, 24, data);
//			uart_send1(UART_USE, result);
//
//			flash_read(FLASH_BLK_DFLASH, 31, 4072, 20, read_data);
//			uart_sendN(UART_USE, read_data, 26);
//
//			result = flash_erase_sector(FLASH_BLK_DFLASH, 31);
//			uart_send1(UART_USE, result);
//
//			flash_read(FLASH_BLK_DFLASH, 0, 0, 26, read_data);
//			uart_sendN(UART_USE, read_data, 26);
//
//			flash_read(FLASH_BLK_DFLASH, 31, 4072, 20, read_data);
//			uart_sendN(UART_USE, read_data, 27);
//			return 1;
			uvar322 = pit_get_time(1);
//			printf("%d\r\n", (int32) (uvar322 - uvar32));
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
