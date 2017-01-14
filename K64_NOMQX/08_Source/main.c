//说明见工程文件夹下的Doc文件夹内Readme.txt文件
//============================================================================

#define GLOBLE_VAR  //只需在main.c中定义一次，用来防止全局变量的重复定义

#include "includes.h"   //包含总头文件

int main(void) {
	//1. 声明主函数使用的变量
	uint8 light_state;		//当前灯状态
	uint32 crc;
	uint8 data[50];
	uint32 stream;
	uint8 i;
	float temp;
	FrameInfo frame;
	FrameCmdInfo cmd;
//	FrameDataInfo data;
	double d;
	float f;
	uint16 crcres;

	/* 小车相关参数变量 */
	Car car;

	/*串口调试工具用临时变量*/
	float data_out[4];

	//2. 关总中断
	DISABLE_INTERRUPTS;

	//3. 初始化外设模块
	light_init(LIGHT_BLUE, LIGHT_ON); //蓝灯初始化
	light_init(LIGHT_GREEN, LIGHT_OFF);
	light_init(LIGHT_RED, LIGHT_OFF);
	//light_init(LIGHT_BLUE, LIGHT_OFF);
//	uart_init(UART_USE, 9600, UART_PARITY_DISABLED, UART_STOP_BIT_1,
//	UART_BIT_ORDER_LSB); //uart1初始化，蓝牙用，蓝牙模块波特率9600，无法在5ms中断中传输数据
//	uart_init(UART_USE, 115200);   //uart1初始化，串口用
	pit_init(PIT_CH0, 5);  //pit0初始化，周期5ms
//	motor_init(MOTOR0);			//左电机初始化
//	motor_init(MOTOR1);			//右电机初始化
//	gyro_acce_init();			//陀螺仪加速度计初始化
//	encoder_init(ENCODER0);		//左编码器初始化
//	encoder_init(ENCODER1);		//右编码器初始化
//	ems_init();					//电磁传感器初始化
//	reed_switch_init();			//干簧管初始化
//ftm_init(FTM_MOD0,FTM_CLK_DIV_128,FTM_COUNTER_MODE_UP_DOWN,100000);
//
//ftm_pwm_single_init(FTM_MOD0,FTM_CH5,FTM_PWM_MODE_CENTER_ALIGNED,FTM_PWM_POL_NEGATIVE,5000);
	temp_sensor_init();
//	crc_init_protocol(CRC_CRC16_MODBUS);
	frame_init();
	crc_init_protocol(CRC_CRC16_MODBUS);
	//4. 给有关变量赋初值
	time0_flag.f_1s = 0;
	time0_flag.f_50ms = 0;
	d = 1.3;
	f = 1.3f;
	//5. 使能模块中断
	pit_enable_int(PIT_CH0);   		//使能pit中断
//	uart_enable_re_int(UART_USE);   //使能uart1接收中断
	frame_enable_re_int();
//	reed_switch_enable_int();

//	uart_send_string(UART_USE,"test！\n");

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

	//6. 开总中断
	ENABLE_INTERRUPTS;

	//进入主循环
	//主循环开始==================================================================
	for (;;) {

		if (time0_flag.f_50ms) {
			time0_flag.f_50ms = 0;
//			data_out[0] = encoder_get_speed(ENCODER0) * 1000;
//			visual_scope_output(UART_USE,data_out);
		}
		if (time0_flag.f_1s) {
			f = sinf(f);
			f/=5.1f;
//			uart_sendN(UART_USE,(uint8 *)&f,4);
//			printf("%f\r\n",f);
//			uart_send1(UART_USE,f);
//			d *= 5.3;
//			f *= 5.3f;
//			uart_send1(UART_USE,d);
//			uart_send1(UART_USE,f);
			crcres = crc_cal(data,5);

			uart_sendN(UART_USE,(uint8*)&crcres,2);

			temp = temp_sensor_get_temp();
//			printf("%f\r\n", temp);

//			frame_send_info(frame);
//			crc = crc_cal(&frame.type,frame.len+2);
//			uart_sendN(UART_MOD1,&crc,2);
//
//			frame_cmd_send(cmd);
//			frame_string_send("啦啦啦\r\n");
			time0_flag.f_1s = 0;
			light_change(LIGHT_BLUE);
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

		//处理接收到的帧
		if (frame_get_info(&frame)) {
			//根据帧的类型做相应操作
			switch (frame.type) {
			case FRAME_STRING_TYPE:	//字符串帧
				//发送字符串回去
				frame_send_info(frame);
				break;
			case FRAME_CMD_TYPE:	//命令帧
				//解帧，获取命令信息
				if (frame_cmd_parse(frame, &cmd) == CmdParseSuccess) {
					//解帧成功时，根据命令类型做相应操作
					switch (cmd.type) {
					case 0x00:	//设置灯状态

						//light_state位0表示蓝灯状态，位1表示绿灯状态，位2表示红灯状态
						cmd.data[0] & 0x1 ?
								light_set(LIGHT_BLUE, LIGHT_ON) :
								light_set(LIGHT_BLUE, LIGHT_OFF);
						cmd.data[0] & 0x2 ?
								light_set(LIGHT_GREEN, LIGHT_ON) :
								light_set(LIGHT_GREEN, LIGHT_OFF);
						cmd.data[0] & 0x4 ?
								light_set(LIGHT_RED, LIGHT_ON) :
								light_set(LIGHT_RED, LIGHT_OFF);
						break;
					}
				}
				break;
			}
		}

	} //主循环end_for
	  //主循环结束==================================================================
}
