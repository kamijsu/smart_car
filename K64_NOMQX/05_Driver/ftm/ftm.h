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

//����FTMģ��ͨ�����
#define FTM_CH_GROUP0	(0)		//CH0��CH1
#define FTM_CH_GROUP1	(1)		//CH2��CH3
#define FTM_CH_GROUP2	(2)		//CH4��CH5
#define FTM_CH_GROUP3	(3)		//CH6��CH7

//FTM��ģ���ͨ�����������ã�ͨ������COM_PORTx|x��x��ѡ�����ţ�
//��ѡ�������ע������Ӧͨ���󷽡�ע�⣺B12|B13��Ҫ�ظ�����
#define FTM_MOD0_CH0_PIN	(COM_PORTC|1)	//A3   C1
#define FTM_MOD0_CH1_PIN	(COM_PORTC|2)	//A2   C2
#define FTM_MOD0_CH2_PIN	(COM_PORTA|5)	//A5   C3   C5
#define FTM_MOD0_CH3_PIN	(COM_PORTA|6)	//A6   C4
#define FTM_MOD0_CH4_PIN	(COM_PORTD|4)	//A7   B12  D4
#define FTM_MOD0_CH5_PIN	(COM_PORTD|5)	//A0   B13  D5
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

//����FTMģ��PWM���ܵ�ģʽ
#define FTM_PWM_MODE_EDGE_ALIGNED	(0)	//��ͨ����PWM�����ض���
#define FTM_PWM_MODE_CENTER_ALIGNED	(1)	//��ͨ����PWM�����Ķ���
#define FTM_PWM_MODE_COMBINE		(2)	//˫ͨ������ͨ����PWM����ͬ
#define FTM_PWM_MODE_COMPLEMENTARY	(3)	//˫ͨ������ͨ����PWM������

//����FTMģ��PWM���ܵļ��ԣ�POLΪpolarity��д��
#define FTM_PWM_POL_POSITIVE		(1)	//�����ԣ���ռ�ձ�Խ�󣬸ߵ�ƽ��ռ����Խ��
#define FTM_PWM_POL_NEGATIVE		(0)	//�����ԣ���ռ�ձ�Խ�󣬵͵�ƽ��ռ����Խ��

//����FTMģ��ļ������ھ��ȣ�(duty/FTM_PERIOD_ACCURACY)��Ϊʵ��ռ�ձȣ�������ҳ�dutyΪռ�ձȣ��ر��˰ٷ�������Ϊ�˱��⸡��������
#define FTM_PERIOD_ACCURACY			(10000u)
//����FTMģ��ռ�ձȵ����ֵ����Сֵ
#define FTM_DUTY_MIN				(0u)
#define FTM_DUTY_MAX				FTM_PERIOD_ACCURACY

//����FTMģ�鲶׽ģʽ
#define FTM_CAPTURE_RISING_EDGE		(0)		//�����ز�׽
#define FTM_CAPTURE_FALLING_EDGE	(1)		//�½��ز�׽
#define FTM_CAPTURE_DOUBLE_EDGE		(2)		//˫���ز�׽

//==========================================================================
//��������: ftm_init
//��������: ��
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//         clk_div:ʱ�ӷ�Ƶ����:
//                 FTM_CLK_DIV_x��xΪ��Ƶ���Ӵ�С;
//         counter_mode:������ģʽ:
//                      FTM_COUNTER_MODE_UP:          ���ϼ���;
//                      FTM_COUNTER_MODE_UP_DOWN:     ���¼���;
//                      FTM_COUNTER_MODE_FREE_RUNNING:��������;
//                      FTM_COUNTER_MODE_QD:          ��������;
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
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//         time:FTMģ��ÿtime���������ڲ���һ���ж����󣬷�Χ[1,32]
//���ܸ�Ҫ: ʹ��FTMģ���ʱ�ж�
//��ע: ��ӦFTMģ��ļ����������������ϼ���ģʽ�£�
//     �ж�ʱ������Ϊcounter_period*time����λms��
//     counter_periodΪ��ӦFTMģ��ļ������ڣ���λms
//==========================================================================
void ftm_timer_enable_int(uint8 mod, uint8 time);

//==========================================================================
//��������: ftm_disable_int
//��������: ��
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//���ܸ�Ҫ: �ر�FTMģ���жϣ�����ر�FTMͨ���ж�
//��ע: ���Թر�ftm_timer_enable_intʹ�ܵ��ж�
//==========================================================================
void ftm_disable_int(uint8 mod);

//==========================================================================
//��������: ftm_get_int
//��������: true:�����ж�; false:δ�����ж�
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//���ܸ�Ҫ: ��ȡFTMģ���жϱ�־�������ȡFTMͨ���жϱ�־
//��ע: ���Ի�ȡftm_timer_enable_intʹ�ܵ��жϵ��жϱ�־
//==========================================================================
bool ftm_get_int(uint8 mod);

//==========================================================================
//��������: ftm_clear_int
//��������: ��
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//���ܸ�Ҫ: ���FTMģ���жϱ�־���������FTMͨ���жϱ�־
//��ע: �������ftm_timer_enable_intʹ�ܵ��жϵ��жϱ�־
//==========================================================================
void ftm_clear_int(uint8 mod);

//==========================================================================
//��������: ftm_pwm_single_init
//��������: ��
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//         ch:FTMģ���ͨ����:
//            FTM_CHx��xΪͨ����;
//         mode:PWMģʽ:
//              FTM_PWM_MODE_EDGE_ALIGNED:  ���ض���ģʽ;
//              FTM_PWM_MODE_CENTER_ALIGNED:���Ķ���ģʽ;
//         pol:PWM����:
//             FTM_PWM_POL_POSITIVE:������;
//             FTM_PWM_POL_NEGATIVE:������;
//         duty:��ʼռ�ձȣ���Χ[FTM_DUTY_MIN(0),FTM_DUTY_MAX(10000)]������δ�޷�
//���ܸ�Ҫ: ��ʼ��FTMģ���ͨ��Ϊ��ͨ����PWM����
//��ע: ��ѡ����ض���ģʽʱ����ӦFTMģ��ļ����������������ϼ���ģʽ��;
//     ��ѡ�����Ķ���ģʽʱ����ӦFTMģ��ļ����������������¼���ģʽ��;
//     PWM����Ƶ��Ϊ(1000/counter_period)����λHz��
//     counter_periodΪ��ӦFTMģ��ļ������ڣ���λms
//==========================================================================
void ftm_pwm_single_init(uint8 mod, uint8 ch, uint8 mode, uint8 pol,
		uint16 duty);

//==========================================================================
//��������: ftm_pwm_single_set
//��������: ��
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//         ch:FTMģ���ͨ����:
//            FTM_CHx��xΪͨ����;
//         duty:ռ�ձȣ���Χ[FTM_DUTY_MIN(0),FTM_DUTY_MAX(10000)]������δ�޷�
//���ܸ�Ҫ: ���ø�ͨ����ռ�ձȣ�������һ���������ڸ���
//==========================================================================
void ftm_pwm_single_set(uint8 mod, uint8 ch, uint16 duty);

//==========================================================================
//��������: ftm_pwm_combine_init
//��������: ��
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//         ch_group:FTMģ���ͨ�����:
//                  FTM_CH_GROUPx��xΪͨ�����;
//         mode:PWMģʽ:
//              FTM_PWM_MODE_COMBINE:      ����ģʽ����ͨ�������ͬ;
//              FTM_PWM_MODE_COMPLEMENTARY:����ģʽ����ͨ������෴;
//         pol:PWM����:
//             FTM_PWM_POL_POSITIVE:������;
//             FTM_PWM_POL_NEGATIVE:������;
//         duty1:��ʼռ�ձ�1����Χ[FTM_DUTY_MIN(0),FTM_DUTY_MAX(10000)]������δ�޷�
//         duty2:��ʼռ�ձ�2����Χ[FTM_DUTY_MIN(0),FTM_DUTY_MAX(10000)]������δ�޷�
//���ܸ�Ҫ: ��ʼ��FTMģ���ͨ����Ϊ˫ͨ����PWM����
//��ע: ��ӦFTMģ��ļ����������������ϼ���ģʽ��;
//     ����PWM����ռ�ձ�Ϊ(duty2-duty1)��duty1��С��duty2����duty1����duty2��
//     PWM�����Խ��ٴη�ת;
//     PWM����Ƶ��Ϊ(1000/counter_period)����λHz��
//     counter_periodΪ��ӦFTMģ��ļ������ڣ���λms;
//     ż��ͨ�������PWM��������ѡ������ģʽ������һ���ģ�������ģʽֻ�������ͨ��
//==========================================================================
void ftm_pwm_combine_init(uint8 mod, uint8 ch_group, uint8 mode, uint8 pol,
		uint16 duty1, uint16 duty2);

//==========================================================================
//��������: ftm_pwm_combine_set
//��������: ��
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//         ch_group:FTMģ���ͨ�����:
//                  FTM_CH_GROUPx��xΪͨ�����;
//         duty1:ռ�ձ�1����Χ[FTM_DUTY_MIN(0),FTM_DUTY_MAX(10000)]������δ�޷�
//         duty2:ռ�ձ�2����Χ[FTM_DUTY_MIN(0),FTM_DUTY_MAX(10000)]������δ�޷�
//���ܸ�Ҫ: ���ø�ͨ�����ռ�ձȣ�������һ���������ڸ���
//��ע: ����PWM����ռ�ձ�Ϊ(duty2-duty1)��duty1��С��duty2����duty1����duty2��
//     PWM�����Խ��ٴη�ת
//==========================================================================
void ftm_pwm_combine_set(uint8 mod, uint8 ch_group, uint16 duty1, uint16 duty2);

//==========================================================================
//��������: ftm_ic_init
//��������: ��
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//         ch:FTMģ���ͨ����:
//            FTM_CHx��xΪͨ����;
//         mode:���벶׽��ģʽ:
//              FTM_CAPTURE_RISING_EDGE: �����ز�׽;
//              FTM_CAPTURE_FALLING_EDGE:�½��ز�׽;
//              FTM_CAPTURE_DOUBLE_EDGE: ˫���ز�׽;
//���ܸ�Ҫ: ��ʼ��FTMģ���ͨ��Ϊ���벶׽����
//��ע: ��ӦFTMģ��ļ����������������ϼ���ģʽ��;
//==========================================================================
void ftm_ic_init(uint8 mod, uint8 ch, uint8 mode);

//==========================================================================
//��������: ftm_ic_get_ratio
//��������: ���㲶׽����ʱ�ı���
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//         ch:FTMģ���ͨ����:
//            FTM_CHx��xΪͨ����;
//���ܸ�Ҫ: ��ȡ�����㲶׽����ʱ����������������ռ�����������ڵı���
//��ע: ����ֵ����FTM_PERIOD_ACCURACY(10000)Ϊ�ٷֱȵı���;
//     ������������PWM��ռ�ձȣ�����:
//     �����ز�׽ģʽ:����ֵΪ��ͬ�������ڡ����ض���ģʽ��������PWM��ռ�ձ�;
//     �½��ز�׽ģʽ:����ֵΪ��ͬ�������ڡ����ض���ģʽ��������PWM��ռ�ձ�;
//==========================================================================
uint16 ftm_ic_get_ratio(uint8 mod, uint8 ch);

//==========================================================================
//��������: ftm_ic_enable_int
//��������: ��
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//         ch:FTMģ���ͨ����:
//            FTM_CHx��xΪͨ����;
//���ܸ�Ҫ: ʹ�����벶׽ͨ���ж�
//==========================================================================
void ftm_ic_enable_int(uint8 mod, uint8 ch);

//==========================================================================
//��������: ftm_ic_disable_int
//��������: ��
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//         ch:FTMģ���ͨ����:
//            FTM_CHx��xΪͨ����;
//���ܸ�Ҫ: �ر����벶׽ͨ���ж�
//==========================================================================
void ftm_ic_disable_int(uint8 mod, uint8 ch);

//==========================================================================
//��������: ftm_ic_get_int
//��������: ��
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//         ch:FTMģ���ͨ����:
//            FTM_CHx��xΪͨ����;
//���ܸ�Ҫ: ��ȡ���벶׽����ͨ�����жϱ�־
//==========================================================================
bool ftm_ic_get_int(uint8 mod, uint8 ch);

//==========================================================================
//��������: ftm_ic_clear_int
//��������: ��
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//         ch:FTMģ���ͨ����:
//            FTM_CHx��xΪͨ����;
//���ܸ�Ҫ: ������벶׽����ͨ�����жϱ�־
//==========================================================================
void ftm_ic_clear_int(uint8 mod, uint8 ch);

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
