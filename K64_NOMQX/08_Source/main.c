//˵���������ļ����µ�Doc�ļ�����Readme.txt�ļ�
//============================================================================

#define GLOBLE_VAR  //ֻ����main.c�ж���һ�Σ�������ֹȫ�ֱ������ظ�����

#include "includes.h"   //������ͷ�ļ�

int main(void) {
	//1. ����������ʹ�õı���
	uint8 light_state;		//��ǰ��״̬
	uint32 crc;
	uint8 data[50];
	uint32 stream;
	uint32 reg;
	uint32 i,j;
	float temp;
	FrameInfo frame;
	FrameCmdInfo cmd;
//	FrameDataInfo data;
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
	float mat[128][128];

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
	d = 1.3;
	f = 1.3f;
	//5. ʹ��ģ���ж�
	pit_enable_int(PIT_CH0);   		//ʹ��pit�ж�
//	uart_enable_re_int(UART_USE);   //ʹ��uart1�����ж�
	frame_enable_re_int();
//	reed_switch_enable_int();
//	ftm_timer_enable_int(FTM_MOD0,10);

//	uart_send_string(UART_USE,"test��\n");

	frame.len = 255;
	data[0] = 0x2;
	data[1] = 0x3;
	data[2] = 0x0;
	data[3] = 0x1;
	data[4] = 0x7;
	data[5] = 0xB1;
	data[6] = 0xAF;
	data[7] = 0x04;

	data[8] = 0x92;
	data[9] = 0x91;
	data[10] = 0x73;
	data[11] = 0x88;

	cmd.type = 0x00;
	cmd.len = 1;
	cmd.data[0] = 7;

	//6. �����ж�
	ENABLE_INTERRUPTS;

	//������ѭ��
	//��ѭ����ʼ==================================================================
	for (;;) {
		if (time0_flag.f_50ms) {
			time0_flag.f_50ms = 0;
//			data_out[0] = encoder_get_speed(ENCODER0) * 1000;
//			visual_scope_output(UART_USE,data_out);
		}
		if (time0_flag.f_1s) {
			time0_flag.f_1s = 0;
			light_change(LIGHT_BLUE);

//			uart_sendN(UART_USE,(uint8 *)&f,4);
//			printf("%f\r\n",f);
//			uart_send1(UART_USE,f);
//			d *= 5.3;
//			f *= 5.3f;
//			uart_send1(UART_USE,d);
//			uart_send1(UART_USE,f);

			temp = temp_sensor_get_temp();
//			printf("%f\r\n", temp);


			uvar32 = pit_get_time(1);

			for(i=0;i<128;i++){
				for(j=0;j<128;j++){
					mat[i][j] *= mat[(i+1)%128][(j+1)%128];
				}
			}

			uvar322 = pit_get_time(1);
			printf("%d\r\n", (int32) (uvar322 - uvar32));



			uart_send1(UART_USE,mat[0][0]);

//			frame_send_info(frame);
//			crc = crc_cal(&frame.type,frame.len+2);
//			uart_sendN(UART_MOD1,&crc,2);
//
//			frame_cmd_send(cmd);
//			frame_string_send("������\r\n");

//			printf("%d\n",encoder_get_and_clear_count(ENCODER0));
//			temp = temp_sensor_get_temp();
//			uart_send1(UART_MOD1,((uint32)temp*1000)>>24);
//			uart_send1(UART_MOD1,((uint32)temp*1000)>>16);
//			uart_send1(UART_MOD1,((uint32)temp*1000)>>8);
//			uart_send1(UART_MOD1,((uint32)temp*1000));
//			printf("%f\r\n",temp);
//
//			temp = adc_diff_get_vtg(ADC_MOD0,TEMP_SENSOR_ADC_DIFF_GROUP);
//			printf("%f\r\n",temp);
//			send = (uint16) (temp * 1000);
//			uart_send1(UART_USE, send >> 8);
//			uart_send1(UART_USE, send);
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
