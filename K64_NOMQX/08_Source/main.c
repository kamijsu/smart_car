//˵���������ļ����µ�Doc�ļ�����Readme.txt�ļ�
//============================================================================

#define GLOBLE_VAR  //ֻ����main.c�ж���һ�Σ�������ֹȫ�ֱ������ظ�����

#include "includes.h"   //������ͷ�ļ�

int main(void)
{
	//1. ����������ʹ�õı���
	uint32_t run_counter;     //��ѭ��������

	/* С����ز������� */
	Car car;

	/*���ڵ��Թ�������ʱ����*/
	float data_out[4];

	//2. �����ж�
	DISABLE_INTERRUPTS;

	//3. ��ʼ������ģ��
	light_init(LIGHT_BLUE, LIGHT_ON); //���Ƴ�ʼ��
	uart_init(UART_USE, 9600);     //uart1��ʼ���������ã�����ģ�鲨����9600���޷���5ms�ж��д�������
//	uart_init(UART_USE, 115200);   //uart1��ʼ����������
	pit_init(U_PIT0, BUS_CLK_KHZ * 5);  //pit0��ʼ��������5ms
	motor_init(MOTOR1);			//������ʼ��
	motor_init(MOTOR2);			//�ҵ����ʼ��
	gyro_acce_init();			//�����Ǽ��ٶȼƳ�ʼ��
	encoder_init(ENCODER1);		//���������ʼ��
	encoder_init(ENCODER2);		//�ұ�������ʼ��
	ems_init();					//��Ŵ�������ʼ��
	reed_switch_init();			//�ɻɹܳ�ʼ��

	//4. ���йر�������ֵ
	run_counter = 0;			//С�Ƽ�������0
	car.angle.angle = 0;		//�Ƕ���0

	/* PWM���������0 */
	car.angle.pwm.new_PWM = 0;
	car.angle.pwm.last_PWM = 0;
	car.angle.period_counter = 0;
	car.speed.pwm.new_PWM = 0;
	car.speed.pwm.last_PWM = 0;
	car.speed.period_counter = 0;
	car.turn.pwm.new_PWM = 0;
	car.turn.pwm.last_PWM = 0;
	car.turn.period_counter = 0;

	/* 0m/sʱ���� */
	car.speed.aim_speed = 0.0;		//Ŀ���ٶȳ�ʼ������λm/s
	car.angle.acce_set = 0x0925;	//0m/sʱƽ��λ��
	car.angle.pid.p = 0.2;
	car.angle.pid.i = 0;
	car.angle.pid.d = 0.001;

	car.speed.pid.p = 1;
	car.speed.pid.i = 0;
	car.speed.pid.d = 0;

	car.turn.pid.p = 0;
	car.turn.pid.i = 0;
	car.turn.pid.d = 0;

	/* ȫ�ֱ��� */
	encoder_counter.left = 0;	//������������������0
	encoder_counter.right = 0;	//�ұ����������������0
	car_flag.reed_switch = 0;	//�ɻɹ�δ��ͨ
	car_flag.over_speed = 0;	//С��δ����
	car_flag.is_starting = 1;	//С��������
	car_flag.can_stop = 0;		//����ͣ��

	/* ʱ���־������0 */
	time0_flag.f_5ms = 0;
	time0_flag.f_10ms = 0;
	time0_flag.f_15ms = 0;
	time0_flag.f_20ms = 0;
	time0_flag.f_50ms = 0;
	time0_flag.f_100ms = 0;
	time0_flag.f_1s = 0;
	time0_flag.f_5s = 0;
	time0_flag.f_10s = 0;
	time0_flag.f_15s = 0;
	time0_flag.f_30s = 0;
	time0_flag.f_1min = 0;

	//5. ʹ��ģ���ж�
	pit_enable_int(U_PIT0);   		//ʹ��pit�ж�
	uart_enable_re_int(UART_USE);   //ʹ��uart1�����ж�
	encoder_enable_int(ENCODER1);	//ʹ����������ж�
	encoder_enable_int(ENCODER2);	//ʹ���ұ������ж�

	//6. �����ж�
	ENABLE_INTERRUPTS;

	//������ѭ��
	//��ѭ����ʼ==================================================================
	for (;;)
	{

		run_counter++;
		if (run_counter > RUN_COUNTER_MAX)
		{
			run_counter = 0;		   //��ѭ����������������
			light_change(LIGHT_BLUE);      //���ƣ�LIGHT_BLUE��״̬�仯
		}

		//С������ͣ��ʱִ�����г���
		if (!car_flag.over_speed && !car_flag.reed_switch)
		{

			/******************
			 �Ƕȿ���ģ��
			 *****************/
			if (time0_flag.f_5ms == 1)    // 5ms IF�������ִ��һ��
			{
				time0_flag.f_5ms = 0;

				//�����Ǽ��ٶȴ�����AD�ɼ�
				gyro_acce_getAD(&car.angle.angle_speed_AD,
						&car.angle.acce_speed_AD);

				//�򴮿ڷ��ͽ��ٶ��Լ����ٶȵ�ADֵ,���ڻ�ȡ�����Ǽ��ٶȴ�������ʼ״̬ADֵ
//				uart_send1(UART_USE,0x0);
//				uart_send1(UART_USE,car.angle.angle_speed_AD >> 8);
//				uart_send1(UART_USE,car.angle.angle_speed_AD);
//				uart_send1(UART_USE,car.angle.acce_speed_AD >> 8);
//				uart_send1(UART_USE,car.angle.acce_speed_AD);
//				uart_send1(UART_USE,0xff);

				//�����Ǽ��ٶȴ�����ADֵ�ع�����ֵ,���ٶȵ�λΪ��/s,���ٶȵ�λΪg
				gyro_acce_AD_to_phy(car.angle.angle_speed_AD,
						car.angle.acce_speed_AD, &car.angle.angle_speed_temp,
						&car.angle.acce_speed_temp, car.angle.acce_set);

				//�򴮿ڷ��ͽ��ٶ��Լ����ٶȵ�����ֵ,���ٶȵ�λΪ��/s,���ٶȵ�λΪm/s^2
//				uart_send1(UART_USE,(int)car.angle.angle_speed_temp);
//				uart_send1(UART_USE,(int)(car.angle.acce_speed_temp * 9.8));

				//ͨ���������˲��õ����ӵ��������ٶ�,��λΪ��͡�/s
				gyro_acce_phy_to_angle(car.angle.angle_speed_temp,
						car.angle.acce_speed_temp, &car.angle.angle,
						&car.angle.angle_speed);

				//�򴮿ڵ��Թ��߷����˲�ǰ��Ƕ�����ٶȣ����ڵ����˲�Ч��
//				data_out[0] = car.angle.angle * 1000;							//�˲�����б�Ƕ�*1000
//				data_out[1] = car.angle.angle_speed * 1000;						//�˲�����ٶ�*1000
//				data_out[2] = asin(car.angle.acce_speed_temp) * 57.296 * 1000;	//�˲�ǰ��б�Ƕ�*1000
//				data_out[3] = car.angle.angle_speed_temp * 1000;				//�˲�ǰ���ٶ�*1000
//				visual_scope_output(UART_USE, data_out);

				//�����µĽǶȿ���ģ��Ե��PWMֵ
				car.angle.pwm.new_PWM = control_angle(car.angle.angle,
						car.angle.angle_speed, car.angle.pid.p, car.angle.pid.i,
						car.angle.pid.d);

				//�Ƕȿ���ģ��ƽ�����ڼ���������
				car.angle.period_counter = 0;

				//�Ƕȿ���ģ��ƽ�����PWMֵ
				++car.angle.period_counter;
				car.angle.pwm.output_PWM = control_angle_PWM_out(
						car.angle.pwm.new_PWM, car.angle.pwm.last_PWM,
						car.angle.period_counter);
				car.angle.pwm.last_PWM = car.angle.pwm.output_PWM;

				//�ٶȿ���ģ��ƽ�����PWMֵ
				++car.speed.period_counter;
				car.speed.pwm.output_PWM = control_speed_PWM_out(
						car.speed.pwm.new_PWM, car.speed.pwm.last_PWM,
						car.speed.period_counter);
				car.speed.pwm.last_PWM = car.speed.pwm.output_PWM;

				//ת�����ģ��ƽ�����PWMֵ
				++car.turn.period_counter;
				car.turn.pwm.output_PWM = control_turn_PWM_out(
						car.turn.pwm.new_PWM, car.turn.pwm.last_PWM,
						car.turn.period_counter);
				car.turn.pwm.last_PWM = car.turn.pwm.output_PWM;

				//���ݽǶȡ��ٶȡ�ת�����ģ��Ե�����PWMֵ
				car.output_PWM = control_motor_output(car.angle.pwm.output_PWM,
						car.speed.pwm.output_PWM, car.turn.pwm.output_PWM);

				//�򴮿ڷ��͸�PWMֵ�����ڲ鿴PID��������Ч��
//				data_out[0] = (float)(car.angle.pwm.output_PWM);	//�Ƕȿ���ģ��Ե�����PWMֵ
//				data_out[1] = (float)(car.speed.pwm.output_PWM);	//�ٶȿ���ģ��Ե�����PWMֵ
//				data_out[2] = (float)(car.turn.pwm.output_PWM);		//ת�����ģ��Ե�����PWMֵ
//				data_out[3] = (float)(car.output_PWM);				//��ģ��Ե�����PWM����ֵ
//				visual_scope_output(UART_USE, data_out);
			}

			/******************
			 ת�����ģ��
			 *****************/
			if (time0_flag.f_20ms == 1) // 20mS IF�������ִ��һ��
			{
				time0_flag.f_20ms = 0;

				//�ɼ����������ݣ���λV
				ems_get_phy(car.turn.EMS_V);

				//�򴮿ڷ��ʹ�������ѹֵ�����ڲ鿴��Ŵ������ɼ�Ч��
//				data_out[0] = car.turn.EMS_V[0] *1000;	//�����Ŵ����� * 1000
//				data_out[1] = car.turn.EMS_V[2] *1000;	//���ڵ�Ŵ����� * 1000
//				data_out[2] = car.turn.EMS_V[3] *1000;	//���ڵ�Ŵ����� * 1000
//				data_out[3] = car.turn.EMS_V[1] *1000;	//�����Ŵ����� * 1000
//				visual_scope_output(UART_USE, data_out);

				//ת�����ģ��ƽ�����ڼ���������
				car.turn.period_counter = 0;

				//��¼�ϴ�ת�����PWMֵ
				car.turn.pwm.last_PWM = car.turn.pwm.new_PWM;

				//�����µ�ת�����ģ��Ե��PWMֵ
				car.turn.pwm.new_PWM = control_turn(car.turn.EMS_V,
						car.turn.pid.p, car.turn.pid.i, car.turn.pid.d);

				//������ʱ�������򴮿ڷ������ݲ鿴ת��ģ��������
//				float error_out, error_in;
//				error_out = (car.turn.EMS_V[1] - car.turn.EMS_V[0]) / (car.turn.EMS_V[0] + car.turn.EMS_V[1]);
//				error_in = (car.turn.EMS_V[3] - car.turn.EMS_V[2]) / (car.turn.EMS_V[2] + car.turn.EMS_V[3]);
//
//				data_out[0] = error_out * 1000;			//���Ŵ�����������ѹ�� * 1000
//				data_out[1] = error_in * 1000;			//�ڵ�Ŵ�����������ѹ�� * 1000
//				data_out[2] = car.turn.pwm.new_PWM;		//�µ�ת�����ģ��Ե��PWMֵ
//				data_out[3] = car.turn.pwm.output_PWM;	//ת�����ģ��ʵ�ʶԵ��PWMֵ
//				visual_scope_output(UART_USE, data_out);
			}

			/******************
			 �ٶȿ���ģ��
			 *****************/
			if (time0_flag.f_50ms == 1)
			{
				time0_flag.f_50ms = 0;

				//��ȡ�������ٶȣ���λm/s
				encoder_get_speed(encoder_counter.left, &car.speed.left_speed);
				encoder_get_speed(encoder_counter.right,
						&car.speed.right_speed);

				//�������������������
				encoder_counter.left = 0;
				encoder_counter.right = 0;

				//����ƽ���ٶ�
				car.speed.avg_speed = (car.speed.left_speed
						+ car.speed.right_speed) / 2;

				//�򴮿ڵ��Թ��߷����������ٶ�
//				data_out[0] = car.speed.left_speed * 1000;	//�����ٶ� * 1000
//				data_out[1] = car.speed.right_speed * 1000;	//�����ٶ� * 1000
//				data_out[2] = car.speed.avg_speed * 1000;	//ƽ���ٶ� * 1000
//				visual_scope_output(UART_USE, data_out);

				//�ٶȱ���
//				if (car.speed.avg_speed > 2.5)
//				{
//					car_flag.over_speed = 1;
//				}

				//�ٶȿ���ģ��ƽ�����ڼ���������
				car.speed.period_counter = 0;

				//��¼�ϴ��ٶȿ���PWMֵ
				car.speed.pwm.last_PWM = car.speed.pwm.new_PWM;

				//�����µ��ٶȿ���ģ��Ե��PWMֵ
				car.speed.pwm.new_PWM = control_speed(car.speed.avg_speed,
						car.speed.aim_speed, car.speed.pid.p, car.speed.pid.i,
						car.speed.pid.d);

				//�򴮿ڷ������ݲ鿴�ٶ�ģ��������
//				data_out[0] = (float) (car.angle.pwm.output_PWM);//�Ƕȿ���ģ��Ե�����PWMֵ
//				data_out[1] = (float) (car.speed.pwm.output_PWM);//�ٶȿ���ģ��Ե�����PWMֵ
//				data_out[2] = car.speed.avg_speed * 10000;		//ƽ���ٶ� * 10000
//				data_out[3] = (float) (car_flag.can_stop * 10000);//0Ϊ����ͣ����10000Ϊ����ͣ��
//				visual_scope_output(UART_USE, data_out);

				//�򴮿ڷ��͸�PWMֵ�����ڲ鿴PID��������Ч��
//				data_out[0] = (float)(car.angle.pwm.output_PWM);	//�Ƕȿ���ģ��Ե�����PWMֵ
//				data_out[1] = (float)(car.speed.pwm.output_PWM);	//�ٶȿ���ģ��Ե�����PWMֵ
//				data_out[2] = (float)(car.turn.pwm.output_PWM);		//ת�����ģ��Ե�����PWMֵ
//				data_out[3] = (float)(car.output_PWM);				//��ģ��Ե�����PWM����ֵ
//				visual_scope_output(UART_USE, data_out);

			}

			/******************
			 �𲽻���
			 *****************/
			if(time0_flag.f_5s == 1)
			{
				time0_flag.f_5s = 0;

				if(car_flag.is_starting)
				{
					car_flag.is_starting = 0;

					/* 0.4m/sʱPID���� */
//					car.speed.aim_speed = 0.4;		//Ŀ���ٶȳ�ʼ������λm/s
//					car.angle.acce_set -= 0x20;		//0.4m/sʱƽ��λ��
//					car.angle.pid.p = 0.2;
//					car.angle.pid.i = 0;
//					car.angle.pid.d = 0.001;
//
//					car.speed.pid.p = 1.5;
//					car.speed.pid.i = 0.1;
//					car.speed.pid.d = 0;
//
//					car.turn.pid.p = 0.125;
//					car.turn.pid.i = 0;
//					car.turn.pid.d = 0.02;

					/* 0.8m/sʱPID���� */
//					car.speed.aim_speed = 0.8;		//Ŀ���ٶȳ�ʼ������λm/s
//					car.angle.acce_set -= 0x24;		//0.8m/sʱƽ��λ��
//					car.angle.pid.p = 0.2;
//					car.angle.pid.i = 0;
//					car.angle.pid.d = 0.001;
//
//					car.speed.pid.p = 1.0;
//					car.speed.pid.i = 0.05;
//					car.speed.pid.d = 0;
//
//					car.turn.pid.p = 0.22;
//					car.turn.pid.i = 0;
//					car.turn.pid.d = 0.04;

					/* 1.0m/sʱPID���� */
					car.speed.aim_speed = 1.0;		//Ŀ���ٶȳ�ʼ������λm/s
					car.angle.acce_set -= 0x24;		//1.0m/sʱƽ��λ��
					car.angle.pid.p = 0.2;
					car.angle.pid.i = 0;
					car.angle.pid.d = 0.001;

					car.speed.pid.p = 0.8;
					car.speed.pid.i = 0.05;
					car.speed.pid.d = 0;

					car.turn.pid.p = 0.25;
					car.turn.pid.i = 0;
					car.turn.pid.d = 0.04;

					/* 1.2m/sʱPID���� */
//					car.speed.aim_speed = 1.2;		//Ŀ���ٶȳ�ʼ������λm/s
//					car.angle.acce_set -= 0x26;		//1.2m/sʱƽ��λ��
//					car.angle.pid.p = 0.2;
//					car.angle.pid.i = 0;
//					car.angle.pid.d = 0.001;
//
//					car.speed.pid.p = 0.8;
//					car.speed.pid.i = 0.05;
//					car.speed.pid.d = 0;
//
//					car.turn.pid.p = 0.4;
//					car.turn.pid.i = 0;
//					car.turn.pid.d = 0.06;
				}

			}

			/******************
			 ͣ��ʹ��
			 *****************/
			if (time0_flag.f_15s == 1)
			{
				time0_flag.f_15s = 0;

				if(!car_flag.can_stop)
				{
					car_flag.can_stop = 1;
					reed_switch_enable_int();
				}
			}

		}
		//ͣ��
		else
		{
			control_stop();
		}

	} //��ѭ��end_for
	  //��ѭ������==================================================================
}
