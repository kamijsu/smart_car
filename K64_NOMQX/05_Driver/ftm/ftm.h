//==========================================================================
//�ļ����ƣ�ftm.h
//���ܸ�Ҫ��K64 FTM�ײ���������ͷ�ļ�
//==========================================================================

#ifndef _FTM_H
#define _FTM_H

//��������ͷ�ļ�
#include "common.h"

//����FTMģ���
#define FTM_MOD0	(0)
#define FTM_MOD1	(1)
#define FTM_MOD2	(2)
#define FTM_MOD3	(3)

//����FTMģ��ͨ����
#define FTM_CH0		(0)
#define FTM_CH1		(1)
#define FTM_CH2		(2)
#define FTM_CH3		(3)
#define FTM_CH4		(4)
#define FTM_CH5		(5)
#define FTM_CH6		(6)
#define FTM_CH7		(7)

//FTM��ģ���ͨ�����������ã�ͨ������COM_PORTx|x��x��ѡ�����ţ�
//��ѡ�������ע������Ӧͨ���󷽡�ע�⣺B12|B13��Ҫ�ظ�����
#define FTM_MOD0_CH0_PIN	(COM_PORTA|3)	//A3   C1
#define FTM_MOD0_CH1_PIN	(COM_PORTA|4)	//A2   C2
#define FTM_MOD0_CH2_PIN	(COM_PORTA|5)	//A5   C3   C5
#define FTM_MOD0_CH3_PIN	(COM_PORTA|6)	//A6   C4
#define FTM_MOD0_CH4_PIN	(COM_PORTA|7)	//A7   B12  D4
#define FTM_MOD0_CH5_PIN	(COM_PORTA|0)	//A0   B13  D5
#define FTM_MOD0_CH6_PIN	(COM_PORTA|1)	//A1        D6
#define FTM_MOD0_CH7_PIN	(COM_PORTA|2)	//A2        D7

#define FTM_MOD1_CH0_PIN	(COM_PORTA|8)	//A8   A12  B0   B12
#define FTM_MOD1_CH1_PIN	(COM_PORTA|9)	//A9   A13  B1   B13

#define FTM_MOD2_CH0_PIN	(COM_PORTA|10)	//A10  B18
#define FTM_MOD2_CH1_PIN	(COM_PORTA|11)	//A11  B19

#define FTM_MOD3_CH0_PIN	(COM_PORTD|0)	//D0   E5
#define FTM_MOD3_CH1_PIN	(COM_PORTD|1)	//D1   E6
#define FTM_MOD3_CH2_PIN	(COM_PORTD|2)	//D2   E7
#define FTM_MOD3_CH3_PIN	(COM_PORTD|3)	//D3   E8
#define FTM_MOD3_CH4_PIN	(COM_PORTC|8)	//C8   E9
#define FTM_MOD3_CH5_PIN	(COM_PORTC|9)	//C9   E10
#define FTM_MOD3_CH6_PIN	(COM_PORTC|10)	//C10  E11
#define FTM_MOD3_CH7_PIN	(COM_PORTC|11)	//C11  E12

//FTMģ������ʱ��Ƶ�ʣ�����ʹ�õ�������ʱ�ӣ���λkHz
#define FTM_CLK_FREQ	BUS_CLK_KHZ

//FTMģ��ʱ�ӷ�Ƶ���ӣ�DIV������ִ����Ƶ���ӣ��乤��Ƶ��=ʱ��Ƶ��/��Ƶ����
#define FTM_CLK_DIV_1		(0)
#define FTM_CLK_DIV_2		(1)
#define FTM_CLK_DIV_4		(2)
#define FTM_CLK_DIV_8		(3)
#define FTM_CLK_DIV_16		(4)
#define FTM_CLK_DIV_32		(5)
#define FTM_CLK_DIV_64		(6)
#define FTM_CLK_DIV_128		(7)

//����FTMģ�������ģʽ
#define FTM_COUNTER_MODE_UP				(0)	//���ϼ���
#define FTM_COUNTER_MODE_UP_DOWN		(1)	//���¼���
#define FTM_COUNTER_MODE_FREE_RUNNING	(2)	//��������
#define FTM_COUNTER_MODE_QD				(3)	//��������

//==========================================================================
//��������: ftm_init
//��������: ��
//����˵��: mod:FTMģ��ţ�FTM_MODx��xΪģ���
//         clk_div:ʱ�ӷ�Ƶ���ӣ�FTM_CLK_DIV_x��xΪ��Ƶ���Ӵ�С
//         counter_mode:������ģʽ��FTM_COUNTER_MODE_UP:���ϼ���;
//                      FTM_COUNTER_MODE_UP_DOWN:���¼���;
//                      FTM_COUNTER_MODE_FREE_RUNNING:��������;
//                      FTM_COUNTER_MODE_QD:��������
//         counter_period:����ע
//���ܸ�Ҫ: ��ʼ��FTMģ�飬Ĭ��δ�����ж�
//��ע: ��ѡ�����ϼ���ģʽ�����¼���ģʽʱ��counter_periodΪ�������������ڣ���λms��
//     ������48000/x*counter_period<=num��48000Ϊ����ʹ�õ�����ʱ��Ƶ�ʣ���λkHz��
//     xΪFTM_CLK_DIV_x��x�����ϼ���ģʽʱnumΪ65536�����¼���ģʽʱnumΪ65534��
//     ���⣬���¼���ģʽʱ����ѡ��128��Ƶ��������Ϊż��;
//     ��ѡ����������ģʽʱ��counter_period��Ч;
//     ��ѡ����������ģʽʱ��counter_periodΪÿ�β����ж�ʱ���������Ѿ������ĸ�����
//     ��Χ[1,65536];
//==========================================================================
void ftm_init(uint8 mod, uint8 clk_div, uint8 counter_mode,
		uint8 counter_period);

//==========================================================================
//��������: ftm_timer_enable_int
//��������: ��
//����˵��: mod:FTMģ��ţ�FTM_MODx��xΪģ���
//         time:FTMģ��ÿtime���������ڲ���һ���ж����󣬷�Χ[1,32]
//���ܸ�Ҫ: ʹ��FTMģ���ʱ�ж�
//��ע: ��ӦFTMģ��ļ����������������ϼ���ģʽ�£�
//     �ж�ʱ������Ϊcounter_period*time����λms��
//     counter_periodΪ��ӦFTMģ��ļ������ڣ���λms
//==========================================================================
void ftm_timer_enable_int(uint8 mod, uint8 time);

//==========================================================================
//��������: ftm_disable_timer_int
//��������: ��
//����˵��: mod:FTMģ��ţ�FTM_MODx��xΪģ���
//���ܸ�Ҫ: �ر�FTMģ���ʱ�ж�
//==========================================================================
void ftm_disable_timer_int(uint8 mod);

//==========================================================================
//��������: ftm_get_timer_int
//��������: true:�����ж�; false:δ�����ж�
//����˵��: mod:FTMģ��ţ�FTM_MODx��xΪģ���
//���ܸ�Ҫ: ��ȡFTMģ���ʱ�жϱ�־
//==========================================================================
bool ftm_get_timer_int(uint8 mod);

//==========================================================================
//��������: ftm_clear_timer_int
//��������: ��
//����˵��: mod:FTMģ��ţ�FTM_MODx��xΪģ���
//���ܸ�Ҫ: ���FTMģ���ʱ�жϱ�־
//==========================================================================
void ftm_clear_timer_int(uint8 mod);

//����FTMģ��PWM���ܵ�ģʽ
#define FTM_PWM_MODE_EDGE_ALIGNED	(0)	//��ͨ����PWM�����ض���
#define FTM_PWM_MODE_CENTER_ALIGNED	(1)	//��ͨ����PWM�����Ķ���
#define FTM_PWM_MODE_COMBINE		(2)	//˫ͨ������ͨ����PWM����ͬ
#define FTM_PWM_MODE_COMPLEMENTARY	(3)	//˫ͨ������ͨ����PWM������

//����FTMģ��PWM���ܵļ��ԣ�POLΪpolarity��д��
#define FTM_PWM_POL_POSITIVE		(1)	//�����ԣ���ռ�ձ�Խ�󣬸ߵ�ƽ��ռ����Խ��
#define FTM_PWM_POL_NEGATIVE		(0)	//�����ԣ���ռ�ձ�Խ�󣬵͵�ƽ��ռ����Խ��

//����FTMģ��PWM���ܵ�ռ�ձȾ��ȣ������dutyΪռ�ձȣ�(duty/FTM_PWM_DUTY_ACCURACY)Ϊʵ��ռ�ձȣ���������Ϊ�˱��⸡��������
#define FTM_PWM_DUTY_ACCURACY	(10000u)
//����FTMģ��PWM����ռ�ձȵ����ֵ����Сֵ
#define FTM_PWM_DUTY_MIN			(0u)
#define FTM_PWM_DUTY_MAX			FTM_PWM_ACCURACY

//==========================================================================
//��������: ftm_pwm_single_init
//��������: ��
//����˵��: mod:FTMģ��ţ�FTM_MODx��xΪģ���
//         ch:FTMģ���ͨ���ţ�FTM_CHx��xΪͨ����
//         mode:PWMģʽ��FTM_PWM_MODE_EDGE_ALIGNED:���ض���ģʽ;
//              FTM_PWM_MODE_CENTER_ALIGNED:���Ķ���ģʽ
//         pol:PWM���ԣ�FTM_PWM_POL_POSITIVE:������; FTM_PWM_POL_NEGATIVE:������
//         duty:��ʼռ�ձȣ���Χ[0,FTM_PWM_DUTY_ACCURACY(10000)]������δ�޷�
//���ܸ�Ҫ: ��ʼ��FTMģ���ͨ��Ϊ��ͨ����PWM����
//��ע: ���ȳ�ʼ����ӦFTMģ��;
//     ��ѡ�����Ķ���ģʽʱ����ӦFTMģ���ʼ��ʱ��48000/div*period<=32768;
//     ��ѡ����ض���ģʽʱ��PWM����Ƶ��Ϊ(1000/period)����λHz����ѡ�����Ķ���ģʽʱ��
//     Ƶ��Ϊ((1000/period)/2)����λHz��periodΪ��ӦFTMģ��ļ������ڣ���λms;
//==========================================================================
void ftm_pwm_single_init(uint8 mod, uint8 ch, uint8 mode, uint8 pol,
		uint16 duty);

//����ͨ�������õ����źţ�������Ӧ��PCR��MUXֵ
#ifdef FTM_MOD0_CH0_PIN
#if(FTM_MOD0_CH0_PIN == (COM_PORTA|3))
#define FTM_MOD0_CH0_PCR_MUX	(3)
#elif(FTM_MOD0_CH0_PIN == (COM_PORTC|1))
#define FTM_MOD0_CH0_PCR_MUX	(4)
#endif
#endif

#ifdef FTM_MOD0_CH1_PIN
#if(FTM_MOD0_CH1_PIN == (COM_PORTA|4))
#define FTM_MOD0_CH1_PCR_MUX	(3)
#elif(FTM_MOD0_CH1_PIN == (COM_PORTC|2))
#define FTM_MOD0_CH1_PCR_MUX	(4)
#endif
#endif

#ifdef FTM_MOD0_CH2_PIN
#if(FTM_MOD0_CH2_PIN == (COM_PORTA|5))
#define FTM_MOD0_CH2_PCR_MUX	(3)
#elif(FTM_MOD0_CH2_PIN == (COM_PORTC|3))
#define FTM_MOD0_CH2_PCR_MUX	(4)
#elif(FTM_MOD0_CH2_PIN == (COM_PORTC|5))
#define FTM_MOD0_CH2_PCR_MUX	(7)
#endif
#endif

#ifdef FTM_MOD0_CH3_PIN
#if(FTM_MOD0_CH3_PIN == (COM_PORTA|6))
#define FTM_MOD0_CH3_PCR_MUX	(3)
#elif(FTM_MOD0_CH3_PIN == (COM_PORTC|4))
#define FTM_MOD0_CH3_PCR_MUX	(4)
#endif
#endif

#ifdef FTM_MOD0_CH4_PIN
#if(FTM_MOD0_CH4_PIN == (COM_PORTA|7))
#define FTM_MOD0_CH4_PCR_MUX	(3)
#elif(FTM_MOD0_CH4_PIN == (COM_PORTB|12) || FTM_MOD0_CH4_PIN == (COM_PORTD|4))
#define FTM_MOD0_CH4_PCR_MUX	(4)
#endif
#endif

#ifdef FTM_MOD0_CH5_PIN
#if(FTM_MOD0_CH5_PIN == (COM_PORTA|0))
#define FTM_MOD0_CH5_PCR_MUX	(3)
#elif(FTM_MOD0_CH5_PIN == (COM_PORTB|13) || FTM_MOD0_CH5_PIN == (COM_PORTD|5))
#define FTM_MOD0_CH5_PCR_MUX	(4)
#endif
#endif

#ifdef FTM_MOD0_CH6_PIN
#if(FTM_MOD0_CH6_PIN == (COM_PORTA|1))
#define FTM_MOD0_CH6_PCR_MUX	(3)
#elif(FTM_MOD0_CH6_PIN == (COM_PORTD|6))
#define FTM_MOD0_CH6_PCR_MUX	(4)
#endif
#endif

#ifdef FTM_MOD0_CH7_PIN
#if(FTM_MOD0_CH7_PIN == (COM_PORTA|2))
#define FTM_MOD0_CH7_PCR_MUX	(3)
#elif(FTM_MOD0_CH7_PIN == (COM_PORTD|7))
#define FTM_MOD0_CH7_PCR_MUX	(4)
#endif
#endif

#ifdef FTM_MOD1_CH0_PIN
#if(FTM_MOD1_CH0_PIN == (COM_PORTA|8) || FTM_MOD1_CH0_PIN == (COM_PORTA|12) || FTM_MOD1_CH0_PIN == (COM_PORTB|0)||FTM_MOD1_CH0_PIN == (COM_PORTB|12))
#define FTM_MOD1_CH0_PCR_MUX	(3)
#endif
#endif

#ifdef FTM_MOD1_CH1_PIN
#if(FTM_MOD1_CH1_PIN == (COM_PORTA|9) || FTM_MOD1_CH1_PIN == (COM_PORTA|13) || FTM_MOD1_CH1_PIN == (COM_PORTB|1) || FTM_MOD1_CH1_PIN == (COM_PORTB|13))
#define FTM_MOD1_CH1_PCR_MUX	(3)
#endif
#endif

#ifdef FTM_MOD2_CH0_PIN
#if(FTM_MOD2_CH0_PIN == (COM_PORTA|10) || FTM_MOD2_CH0_PIN == (COM_PORTB|18))
#define FTM_MOD2_CH0_PCR_MUX	(3)
#endif
#endif

#ifdef FTM_MOD2_CH1_PIN
#if(FTM_MOD2_CH1_PIN == (COM_PORTA|11) || FTM_MOD2_CH1_PIN == (COM_PORTB|19))
#define FTM_MOD2_CH1_PCR_MUX	(3)
#endif
#endif

#ifdef FTM_MOD3_CH0_PIN
#if(FTM_MOD3_CH0_PIN == (COM_PORTD|0))
#define FTM_MOD3_CH0_PCR_MUX	(4)
#elif(FTM_MOD3_CH0_PIN == (COM_PORTE|5))
#define FTM_MOD3_CH0_PCR_MUX	(6)
#endif
#endif

#ifdef FTM_MOD3_CH1_PIN
#if(FTM_MOD3_CH1_PIN == (COM_PORTD|1))
#define FTM_MOD3_CH1_PCR_MUX	(4)
#elif(FTM_MOD3_CH1_PIN == (COM_PORTE|6))
#define FTM_MOD3_CH1_PCR_MUX	(6)
#endif
#endif

#ifdef FTM_MOD3_CH2_PIN
#if(FTM_MOD3_CH2_PIN == (COM_PORTD|2))
#define FTM_MOD3_CH2_PCR_MUX	(4)
#elif(FTM_MOD3_CH2_PIN == (COM_PORTE|7))
#define FTM_MOD3_CH2_PCR_MUX	(6)
#endif
#endif

#ifdef FTM_MOD3_CH3_PIN
#if(FTM_MOD3_CH3_PIN == (COM_PORTD|3))
#define FTM_MOD3_CH3_PCR_MUX	(4)
#elif(FTM_MOD3_CH3_PIN == (COM_PORTE|8))
#define FTM_MOD3_CH3_PCR_MUX	(6)
#endif
#endif

#ifdef FTM_MOD3_CH4_PIN
#if(FTM_MOD3_CH4_PIN == (COM_PORTC|8))
#define FTM_MOD3_CH4_PCR_MUX	(3)
#elif(FTM_MOD3_CH4_PIN == (COM_PORTE|9))
#define FTM_MOD3_CH4_PCR_MUX	(6)
#endif
#endif

#ifdef FTM_MOD3_CH5_PIN
#if(FTM_MOD3_CH5_PIN == (COM_PORTC|9))
#define FTM_MOD3_CH5_PCR_MUX	(3)
#elif(FTM_MOD3_CH5_PIN == (COM_PORTE|10))
#define FTM_MOD3_CH5_PCR_MUX	(6)
#endif
#endif

#ifdef FTM_MOD3_CH6_PIN
#if(FTM_MOD3_CH6_PIN == (COM_PORTC|10))
#define FTM_MOD3_CH6_PCR_MUX	(3)
#elif(FTM_MOD3_CH6_PIN == (COM_PORTE|11))
#define FTM_MOD3_CH6_PCR_MUX	(6)
#endif
#endif

#ifdef FTM_MOD3_CH7_PIN
#if(FTM_MOD3_CH7_PIN == (COM_PORTC|11))
#define FTM_MOD3_CH7_PCR_MUX	(3)
#elif(FTM_MOD3_CH7_PIN == (COM_PORTE|12))
#define FTM_MOD3_CH7_PCR_MUX	(6)
#endif
#endif

#endif
