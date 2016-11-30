//==========================================================================
//�ļ����ƣ�ftm.h
//���ܸ�Ҫ��K64 FTM�ײ���������ͷ�ļ�
//==========================================================================

#ifndef _FTM_H
#define _FTM_H

//��������ͷ�ļ�
#include "common.h"
//����gpio.h����ΪҪ�õ��������Ź���
#include "gpio.h"

//==========================================================================
//˵��: ��ͬһFTMģ�����໥���ݵĹ���:
//     (���ض����PWM�������벶׽������Ƚϡ���ʱ)
//     (����ģʽPWM��������ģʽPWM��)
//     �����������κι����޷�����
//==========================================================================

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
#define FTM_MOD0_CH2_PIN	(COM_PORTC|3)	//A5   C3   C5
#define FTM_MOD0_CH3_PIN	(COM_PORTC|4)	//A6   C4
#define FTM_MOD0_CH4_PIN	(COM_PORTD|4)	//A7   B12  D4
#define FTM_MOD0_CH5_PIN	(COM_PORTD|5)	//A0   B13  D5
#define FTM_MOD0_CH6_PIN	(COM_PORTA|1)	//A1        D6
#define FTM_MOD0_CH7_PIN	(COM_PORTA|2)	//A2        D7

#define FTM_MOD1_CH0_PIN	(COM_PORTB|0)	//A8   A12  B0   B12
#define FTM_MOD1_CH1_PIN	(COM_PORTB|1)	//A9   A13  B1   B13

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

//����FTMģ���������빦�����������飬ͨ�����ĺ궨���ֵ��ѡ�������飬������ǰ��ΪPHA������ΪPHB
//FTM1_QD:  0:A8-A9;   1:A12-A13; 2:B0-B1; 3:B12-B13
#define FTM_MOD1_QD_SETUP	(2)
//FTM2_QD:  0:A10-A11; 1:B18-B19;
#define FTM_MOD2_QD_SETUP	(0)

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
#define FTM_COUNTER_MODE_QD				(2)	//��������

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
#define FTM_IC_MODE_RISING_EDGE		(0)		//�����ز�׽
#define FTM_IC_MODE_FALLING_EDGE	(1)		//�½��ز�׽
#define FTM_IC_MODE_DOUBLE_EDGE		(2)		//˫���ز�׽

//����FTMģ������ȽϹ���ģʽ
#define FTM_OC_MODE_TOGGLE		(0)		//�Ƚϳɹ���ת��ƽ
#define FTM_OC_MODE_SET			(1)		//�Ƚϳɹ����øߵ�ƽ
#define FTM_OC_MODE_CLEAR		(2)		//�Ƚϳɹ����õ͵�ƽ

//����FTMģ���������빦��ģʽ
#define FTM_QD_MODE_PHAB				(0)		//AB�������
#define FTM_QD_MODE_COUNT_DIRECTION		(1)		//����-�����ͱ�����

//����FTMģ���������빦�ܷ���
#define FTM_QD_DIR_POSITIVE		(0)		//������
#define FTM_QD_DIR_NEGATIVE		(1)		//������

//==========================================================================
//��������: ftm_init
//��������: ��
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//         clk_div:ʱ�ӷ�Ƶ����:
//                 FTM_CLK_DIV_x��xΪ��Ƶ���Ӵ�С;
//         counter_mode:������ģʽ:
//                      FTM_COUNTER_MODE_UP:     ���ϼ���;
//                      FTM_COUNTER_MODE_UP_DOWN:���¼���;
//                      FTM_COUNTER_MODE_QD:     ��������;
//         counter_period:����ע
//���ܸ�Ҫ: ��ʼ��FTMģ�飬Ĭ��δ�����ж�
//��ע: ��ѡ�����ϼ���ģʽ�����¼���ģʽʱ��counter_periodΪ�������������ڣ���λ��s��
//     ��ѡ�����ϼ���ģʽʱ��������48000/x*counter_period/1000<=65536��
//     ��ѡ�����¼���ģʽʱ��������48000/x*counter_period/1000/2<=65535��
//     48000Ϊ����ʹ�õ�����ʱ��Ƶ�ʣ���λkHz��xΪFTM_CLK_DIV_x��x��
//     ��ע��counter_period��ֵ���Է�ֹ�������ȶ�ʧ;
//
//     ��ѡ����������ģʽʱ��counter_period��Ч���Ҵ�ʱclk_div�����������Ƶ����
//     (��������������*x)Ϊ��������������xΪFTM_CLK_DIV_x��x;
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
//     �ж�ʱ������Ϊcounter_period*time����λ��s��
//     counter_periodΪ��ӦFTMģ��ļ������ڣ���λ��s
//==========================================================================
void ftm_timer_enable_int(uint8 mod, uint8 time);

//==========================================================================
//��������: ftm_timer_disable_int
//��������: ��
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//���ܸ�Ҫ: �ر�FTMģ���ʱ�ж�
//==========================================================================
void ftm_timer_disable_int(uint8 mod);

//==========================================================================
//��������: ftm_timer_get_int
//��������: true:�����ж�; false:δ�����ж�
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//���ܸ�Ҫ: ��ȡFTMģ���ʱ�жϱ�־
//==========================================================================
bool ftm_timer_get_int(uint8 mod);

//==========================================================================
//��������: ftm_timer_clear_int
//��������: ��
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//���ܸ�Ҫ: ���FTMģ���ʱ�жϱ�־
//==========================================================================
void ftm_timer_clear_int(uint8 mod);

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
//
//     ��ѡ�����Ķ���ģʽʱ����ӦFTMģ��ļ����������������¼���ģʽ��;
//
//     PWM����Ƶ��Ϊ(1000000/counter_period)����λHz��
//     counter_periodΪ��ӦFTMģ��ļ������ڣ���λ��s
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
//     PWM����Ƶ��Ϊ(1000000/counter_period)����λHz��
//     counter_periodΪ��ӦFTMģ��ļ������ڣ���λ��s;
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
//              FTM_IC_MODE_RISING_EDGE: �����ز�׽;
//              FTM_IC_MODE_FALLING_EDGE:�½��ز�׽;
//              FTM_IC_MODE_DOUBLE_EDGE: ˫���ز�׽;
//���ܸ�Ҫ: ��ʼ��FTMģ���ͨ��Ϊ���벶׽���ܣ�Ĭ��δ�����ж�
//��ע: ��ӦFTMģ��ļ����������������ϼ���ģʽ��;δ���������ŵ���
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
//     �����ز�׽ģʽ:����ֵΪ��ͬ�����������ض���ģʽ��������PWM��ռ�ձ�;
//     �½��ز�׽ģʽ:����ֵΪ��ͬ�����������ض���ģʽ��������PWM��ռ�ձ�;
//==========================================================================
uint16 ftm_ic_get_ratio(uint8 mod, uint8 ch);

//==========================================================================
//��������: ftm_oc_init
//��������: ��
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//         ch:FTMģ���ͨ����:
//            FTM_CHx��xΪͨ����;
//         mode:����Ƚϵ�ģʽ:
//              FTM_OC_MODE_TOGGLE:�Ƚϳɹ���ת��ƽ;
//              FTM_OC_MODE_SET:   �Ƚϳɹ����øߵ�ƽ;
//              FTM_OC_MODE_CLEAR: �Ƚϳɹ����õ͵�ƽ;
//         ratio:�Ƚϳɹ���ʱ��ռ�����������ڵı�������Χ[0,FTM_PERIOD_ACCURACY(10000))
//���ܸ�Ҫ: ��ʼ��FTMģ���ͨ��Ϊ����ȽϹ��ܣ�Ĭ��δ�����ж�
//��ע: ��ӦFTMģ��ļ����������������ϼ���ģʽ��;
//     ratio����FTM_PERIOD_ACCURACY(10000)Ϊ�ٷֱȵı���
//==========================================================================
void ftm_oc_init(uint8 mod, uint8 ch, uint8 mode, uint16 ratio);

//==========================================================================
//��������: ftm_oc_change_mode
//��������: ��
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//         ch:FTMģ���ͨ����:
//            FTM_CHx��xΪͨ����;
//         mode:����Ƚϵ�ģʽ:
//              FTM_OC_MODE_TOGGLE:�Ƚϳɹ���ת��ƽ;
//              FTM_OC_MODE_SET:   �Ƚϳɹ����øߵ�ƽ;
//              FTM_OC_MODE_CLEAR: �Ƚϳɹ����õ͵�ƽ;
//���ܸ�Ҫ: ��������ȽϹ���ͨ����ģʽ
//==========================================================================
void ftm_oc_change_mode(uint8 mod, uint8 ch, uint8 mode);

//==========================================================================
//��������: ftm_oc_set_ratio
//��������: ��
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//         ch:FTMģ���ͨ����:
//            FTM_CHx��xΪͨ����;
//         ratio:�Ƚϳɹ���ʱ��ռ�����������ڵı�������Χ[0,FTM_PERIOD_ACCURACY(10000))
//���ܸ�Ҫ: ��������ȽϹ���ͨ���ıȽϳɹ�ʱ��
//==========================================================================
void ftm_oc_set_ratio(uint8 mod, uint8 ch, uint16 ratio);

//==========================================================================
//��������: ftm_ch_enable_int
//��������: ��
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//         ch:FTMģ���ͨ����:
//            FTM_CHx��xΪͨ����;
//���ܸ�Ҫ: ʹ��ͨ���ж�
//��ע: ����ʹ�����벶׽������ȽϹ���ͨ�����ж�:
//     ���벶׽:��������ź����㲶׽����ʱ�������ж�;
//     ����Ƚ�:���Ƚϳɹ�ʱ�������ж�;
//==========================================================================
void ftm_ch_enable_int(uint8 mod, uint8 ch);

//==========================================================================
//��������: ftm_ch_disable_int
//��������: ��
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//         ch:FTMģ���ͨ����:
//            FTM_CHx��xΪͨ����;
//���ܸ�Ҫ: �ر�ͨ���ж�
//==========================================================================
void ftm_ch_disable_int(uint8 mod, uint8 ch);

//==========================================================================
//��������: ftm_ch_get_int
//��������: true:�����ж�; false:δ�����ж�;
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//         ch:FTMģ���ͨ����:
//            FTM_CHx��xΪͨ����;
//���ܸ�Ҫ: ��ȡͨ�����жϱ�־
//==========================================================================
bool ftm_ch_get_int(uint8 mod, uint8 ch);

//==========================================================================
//��������: ftm_ch_clear_int
//��������: ��
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//         ch:FTMģ���ͨ����:
//            FTM_CHx��xΪͨ����;
//���ܸ�Ҫ: ���ͨ�����жϱ�־
//==========================================================================
void ftm_ch_clear_int(uint8 mod, uint8 ch);

//==========================================================================
//��������: ftm_qd_init
//��������: ��
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;�������FTM_MOD1��FTM_MOD2����ʹ��;
//         mode:�������빦��ģʽ:
//              FTM_QD_MODE_PHAB:           AB�������;
//              FTM_QD_MODE_COUNT_DIRECTION:����-�����ͱ�����;
//         dir:�������빦�ܷ���(�������ע):
//             FTM_QD_DIR_POSITIVE:������;
//             FTM_QD_DIR_NEGATIVE:������;
//���ܸ�Ҫ: ��ʼ��FTMģ��Ϊ�������빦�ܣ����������ŵ���
//��ע: ��ӦFTMģ��ļ���������������������ģʽ��;
//     ���źſ���ͨ���궨��ѡ��;
//
//     ��ѡ��AB�������ģʽ���������ŷֱ���ڱ�������AB����;��ʱ������Ϊ��������4��Ƶ��
//     �����������ֱ���Ϊ500����500��A�������500��B������ÿȦ�������������2000��;
//     ��������תȦ��=(��������������*x)/4/�������ֱ��ʣ�xΪFTM_CLK_DIV_x��x;
//     ������Ϊ������ʱ��˳ʱ����תʱ���������ӣ���ʱ����תʱ����������;
//     ������Ϊ������ʱ��˳ʱ����תʱ���������٣���ʱ����תʱ����������;
//
//     ��ѡ����-�����ͱ�����ģʽ��A�����Ŷ�������ʣ�B�����ž����������ķ���
//     ��B�����ŵ�ƽ���ı�ʱ��������������ô��ת����������������/���ٵ�;
//     ������Ϊ������ʱ��B������Ϊ�ߵ�ƽʱ���������ӣ�Ϊ�͵�ƽʱ����������;
//     ������Ϊ������ʱ��B������Ϊ�ߵ�ƽʱ���������٣�Ϊ�͵�ƽʱ����������;
//     ��������תȦ��=(��������������*x)/�������ֱ��ʣ�xΪFTM_CLK_DIV_x��x;
//==========================================================================
void ftm_qd_init(uint8 mod, uint8 mode, uint8 dir);

//==========================================================================
//��������: ftm_qd_get_count
//��������: ��������������
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;�������FTM_MOD1��FTM_MOD2����ʹ��;
//���ܸ�Ҫ: ��ȡ��������ǰ��������
//��ע: �������������������ֱ����ͬ���򣬾��嶨���������й�
//==========================================================================
int16 ftm_qd_get_count(uint8 mod);

//==========================================================================
//��������: ftm_qd_clear_count
//��������: ��
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;�������FTM_MOD1��FTM_MOD2����ʹ��;
//���ܸ�Ҫ: �����������������
//==========================================================================
void ftm_qd_clear_count(uint8 mod);

//==========================================================================
//��������: ftm_qd_get_and_clear_count
//��������: ��������������
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;�������FTM_MOD1��FTM_MOD2����ʹ��;
//���ܸ�Ҫ: ��ȡ��������ǰ�������������������������
//��ע: �������������������ֱ����ͬ���򣬾��嶨���������й�
//==========================================================================
int16 ftm_qd_get_and_clear_count(uint8 mod);

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

//�����������빦����ѡ�����飬����PHA��PHB�������Լ���Ӧ��PCR��MUXֵ
#ifdef FTM_MOD1_QD_SETUP
#if(FTM_MOD1_QD_SETUP == 0)
#define FTM_MOD1_QD_PHA_PIN		(COM_PORTA|8)
#define FTM_MOD1_QD_PHB_PIN		(COM_PORTA|9)
#elif(FTM_MOD1_QD_SETUP == 1)
#define FTM_MOD1_QD_PHA_PIN		(COM_PORTA|12)
#define FTM_MOD1_QD_PHB_PIN		(COM_PORTA|13)
#elif(FTM_MOD1_QD_SETUP == 2)
#define FTM_MOD1_QD_PHA_PIN		(COM_PORTB|0)
#define FTM_MOD1_QD_PHB_PIN		(COM_PORTB|1)
#elif(FTM_MOD1_QD_SETUP == 3)
#define FTM_MOD1_QD_PHA_PIN		(COM_PORTB|12)
#define FTM_MOD1_QD_PHB_PIN		(COM_PORTB|13)
#endif
#if(FTM_MOD1_QD_SETUP == 1)
#define FTM_MOD1_QD_PCR_MUX		(7)
#else
#define FTM_MOD1_QD_PCR_MUX		(6)
#endif
#endif

#ifdef FTM_MOD2_QD_SETUP
#if(FTM_MOD2_QD_SETUP == 0)
#define FTM_MOD2_QD_PHA_PIN		(COM_PORTA|10)
#define FTM_MOD2_QD_PHB_PIN		(COM_PORTA|11)
#elif(FTM_MOD2_QD_SETUP == 1)
#define FTM_MOD2_QD_PHA_PIN		(COM_PORTB|18)
#define FTM_MOD2_QD_PHB_PIN		(COM_PORTB|19)
#endif
#define FTM_MOD2_QD_PCR_MUX		(6)
#endif

#endif
