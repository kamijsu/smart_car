//==========================================================================
//�ļ����ƣ�motor.h
//���ܸ�Ҫ�����Ӧ����������ͷ�ļ�
//==========================================================================

#ifndef _MOTOR_H
#define _MOTOR_H

#include "common.h"
#include "ftm.h"

//��������
#define MOTOR0	(0)
#define MOTOR1	(1)

//��������ʹ�õ�FTMģ�����ã����е��ʹ��ͬһFTMģ�飬��ѡ���ü�ftm.h(ע��:����ͬFTMģ�鱻���ڲ�ͬӦ�ù������뱣֤���ǵ�������ͬ)
//������ʹ�õ�FTMģ��
#define MOTOR_FTM_MOD				FTM_MOD0					//FTM0
//����FTMģ��ʱ�ӷ�Ƶ����
#define MOTOR_FTM_CLK_DIV			FTM_CLK_DIV_1				//1��Ƶ��������Ƶ��48MHz
//����FTMģ�������ģʽ
#define MOTOR_FTM_COUNTER_MODE		FTM_COUNTER_MODE_UP			//���ϼ���
//����FTMģ���������������
#define MOTOR_FTM_COUNTER_PERIOD	(1)							//1ms��������Ӧ��Ƶ���ӣ�PWM��Ƶ��Ϊ1000Hz
//����FTMģ��PWM���ܵ�ģʽ
#define MOTOR_FTM_PWM_MODE			FTM_PWM_MODE_EDGE_ALIGNED	//PWM�����ض���
//����FTMģ��PWM���ܵļ���
#define MOTOR_FTM_PWM_POL			FTM_PWM_POL_POSITIVE		//�����ԣ���ռ�ձ�Խ�󣬸ߵ�ƽ��ռ����Խ��

//��������ʹ�õ�PWMͨ����ÿ�����ʹ������ͨ������Ӧ��������ftm.h�е�FTM_MODx_CHx_PIN����
#define MOTOR0_PWM0_CH				FTM_CH0		//FTM0_CH0	C1
#define MOTOR0_PWM1_CH				FTM_CH1		//FTM0_CH1	C2
#define MOTOR1_PWM0_CH				FTM_CH2		//FTM0_CH2	C3
#define MOTOR1_PWM1_CH				FTM_CH3		//FTM0_CH3	C4

//������ռ�ձ������ޣ���duty>=0ʱ��PWM0ռ�ձ�Ϊ|duty|��PWM1ռ�ձ�Ϊ0����duty<0ʱ��PWM0ռ�ձ�Ϊ0��PWM1ռ�ձ�Ϊ|duty|
#define MOTOR_DUTY_MAX				FTM_DUTY_MAX
#define MOTOR_DUTY_MIN				(-FTM_DUTY_MAX)

//==========================================================================
//��������: motor_init
//��������: ��
//����˵��: motor:�����:
//               MOTORx��xΪ�����;
//���ܸ�Ҫ: ��ʼ���������ʼռ�ձ�Ϊ0����Ӧ������motor.h��
//==========================================================================
void motor_init(uint8 motor);

//==========================================================================
//��������: motor_set
//��������: ��
//����˵��: motor:�����:
//               MOTORx��xΪ�����;
//         duty:ռ�ձȣ���Χ[MOTOR_DUTY_MIN(-10000),MOTOR_DUTY_MAX(10000)]
//���ܸ�Ҫ: ���õ����ռ�ձȣ�������һ���������ڸ���
//��ע: ��duty>=0ʱ��PWM0ռ�ձ�Ϊ|duty|��PWM1ռ�ձ�Ϊ0��
//     ��duty<0ʱ��PWM0ռ�ձ�Ϊ0��PWM1ռ�ձ�Ϊ|duty|��
//     ռ�ձ������������޷�
//==========================================================================
void motor_set(uint8 motor, int16 duty);

#endif
