//===========================================================================
//�ļ����ƣ�pwm.c
//���ܸ�Ҫ��pwm�ײ���������Դ�ļ�
//��Ȩ���У����ݴ�ѧ��˼����Ƕ��ʽ����(sumcu.suda.edu.cn)
//���¼�¼��2013-5-5   V3.0 ����
//===========================================================================

#ifndef _PWM_H
#define _PWM_H

#include "common.h"
#include "gpio.h"

#define PWM_ACCURACY (10000) //����ռ�ձ�(duty)��ΧΪ0~PWM_ACCURACY,��������Ϊ1

//U_FTMģ���
#define U_FTM0  (0)
#define U_FTM1  (1)
#define U_FTM2  (2)
#define U_FTM3  (3)
//U_FTMͨ����
#define U_FTMCH0  (0)
#define U_FTMCH1  (1)
#define U_FTMCH2  (2)
#define U_FTMCH3  (3)
#define U_FTMCH4  (4)
#define U_FTMCH5  (5)
#define U_FTMCH6  (6)
#define U_FTMCH7  (7)
//����U_FTMͨ����ѡ��                     		  	���õĶ˿�_����
#define U_FTM0_CH0 (U_PORTC|1) // A3  C1
#define U_FTM0_CH1 (U_PORTC|2) // A4  C2
#define U_FTM0_CH2 (U_PORTC|3) // A5  C3  C5
#define U_FTM0_CH3 (U_PORTC|4) // A6  C4
#define U_FTM0_CH4 (U_PORTD|4) // A7  B12 D4
#define U_FTM0_CH5 (U_PORTD|5) // A0  B13 D5
#define U_FTM0_CH6 (U_PORTD|6) // A1  D6
#define U_FTM0_CH7 (U_PORTD|7) // A2  D7

#define U_FTM1_CH0 (U_PORTA|12)// A8  A12  B0  B12
#define U_FTM1_CH1 (U_PORTA|13)// A9  A13  B1  B13

#define U_FTM2_CH0 (U_PORTA|10)// A10 B18
#define U_FTM2_CH1 (U_PORTA|11)// A11 B19

#define U_FTM3_CH0 (U_PORTE|5) // D0  E5
#define U_FTM3_CH1 (U_PORTE|6) // D1  E6
#define U_FTM3_CH2 (U_PORTE|7) // D2  E7
#define U_FTM3_CH3 (U_PORTE|8) // D3  E8
#define U_FTM3_CH4 (U_PORTE|9) // C8  E9
#define U_FTM3_CH5 (U_PORTE|10)// C9  E10
#define U_FTM3_CH6 (U_PORTE|11)// C10 E11
#define U_FTM3_CH7 (U_PORTE|12)// C11 E12

#define bus_clk_khz (50000)

//=========================================================================
//�������ƣ�pwm_init
//���ܸ�Ҫ��pwmģ���ʼ����
//����˵����ftm_no:ģ��ţ�U_FTM0 ~ U_FTM3
//		 channel:ͨ����:U_FTMCH0 ~ U_FTMCH7 ���pwm.h�ļ�����
//       period:���� ��λ΢�루����0.1msΪ100,1msΪ1000��
//       duty:ռ�ձ�    0~PWM_ACCURACY(��PWM.h�ж���)
//�������أ�	0	�ɹ�
//			1	ģ��Ŵ���
//			2	ͨ���Ŵ���
//			3	Ƶ�ʴ���ʱ�ӵĶ���֮һ
//=========================================================================
uint8_t pwm_init(uint8_t ftmNo, uint8_t channel, uint32_t period,
		uint16_t duty);

//=========================================================================
//�������ƣ�pwm_set
//���ܸ�Ҫ������pwmģ�顣
//����˵����ftm_no:ģ��ţ�U_FTM0 ~ U_FTM3
//		 channel:ͨ����:U_FTMCH0 ~ U_FTMCH7 ���pwm.h�ļ�����
//       period:���� ��λ΢�루����0.1msΪ100,1msΪ1000��
//       duty:ռ�ձ�    0~PWM_ACCURACY(��PWM.h�ж���)
//�������أ�	0	�ɹ�
//			1	ģ��š�ͨ���Ŵ���
//			2	Ƶ�ʴ���ʱ�ӵĶ���֮һ
//			3	ռ�ձȴ���
//=========================================================================
uint8_t pwm_set(uint8_t ftmNo, uint8_t channel, uint32_t period, uint16_t duty);

#endif

//===========================================================================
//������
//��1�����ǿ�����Դ���룬�ڱ������ṩ��Ӳ��ϵͳ����ͨ������Ϸ��׸���ᣬ����֮����
//     ��ӭָ����
//��2������ʹ�÷Ǳ�����Ӳ��ϵͳ���û�����ֲ����ʱ������ϸ�����Լ���Ӳ��ƥ�䡣
//
//���ݴ�ѧ��˼����Ƕ��ʽ���ģ����ݻ�����Ϣ�Ƽ����޹�˾��
//������ѯ��0512-65214835  http://sumcu.suda.edu.cn
//ҵ����ѯ��0512-87661670,18915522016  http://www.hxtek.com.cn
