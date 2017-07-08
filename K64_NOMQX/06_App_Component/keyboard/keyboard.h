//==========================================================================
//�ļ����ƣ�keyboard.h
//���ܸ�Ҫ������Ӧ����������ͷ�ļ�
//==========================================================================

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "common.h"
#include "gpio.h"
#include "pit.h"

//��Ҫ�ṩһ�����õ���ʱ����
#define KEYBOARD_DELAY_MS(ms)		pit_delay_ms(PIT_CH1, ms)
//������̶�ȡ�ɹ�֮�����ʱʱ�䣬��λms
#define KEYBOARD_TIME_DELAY 		(200)

//������̵��������ţ���ΪGPIO���
#define KEYBOARD_ROW0       (COM_PORTB | 10)
#define KEYBOARD_ROW1       (COM_PORTB | 9)
#define KEYBOARD_ROW2       (COM_PORTA | 4)
#define KEYBOARD_ROW3       (COM_PORTA | 5)
//������̵��������ţ���ΪGPIO���룬����Ϊͬһ�˿�������
#define KEYBOARD_COL0       (COM_PORTA | 14)
#define KEYBOARD_COL1       (COM_PORTA | 15)
#define KEYBOARD_COL2       (COM_PORTA | 16)
#define KEYBOARD_COL3       (COM_PORTA | 17)

//������̰����ַ�
#define KEYBOARD_KEY_ROW0_COL0	('7')
#define KEYBOARD_KEY_ROW0_COL1	('8')
#define KEYBOARD_KEY_ROW0_COL2	('9')
#define KEYBOARD_KEY_ROW0_COL3	('P')

#define KEYBOARD_KEY_ROW1_COL0	('4')
#define KEYBOARD_KEY_ROW1_COL1	('5')
#define KEYBOARD_KEY_ROW1_COL2	('6')
#define KEYBOARD_KEY_ROW1_COL3	('N')

#define KEYBOARD_KEY_ROW2_COL0	('1')
#define KEYBOARD_KEY_ROW2_COL1	('2')
#define KEYBOARD_KEY_ROW2_COL2	('3')
#define KEYBOARD_KEY_ROW2_COL3	('B')

#define KEYBOARD_KEY_ROW3_COL0	('0')
#define KEYBOARD_KEY_ROW3_COL1	('.')
#define KEYBOARD_KEY_ROW3_COL2	('C')
#define KEYBOARD_KEY_ROW3_COL3	('O')

//�������ɨ����ֵ��ɨ����ͬ��ֵ��������ֵʱ����ȡ��ֵ
#define KEYBORAD_THRESHOLD	(5)

//==========================================================================
//��������: keyboard_init
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: ��ʼ�����̣���Ӧ������keyboard.h��
//��ע: �������������ŵ���
//==========================================================================
void keyboard_init();

//==========================================================================
//��������: keyboard_read
//��������: true:��ȡ�ɹ�; false:��ȡʧ�ܣ��޼�����
//����˵��: key:�洢���̰����ַ��ĵ�ַ
//���ܸ�Ҫ: ��ȡ���̣��ɹ�ʱ�洢��ȡ�ļ��̰����ַ�������ʱ�����ʱ��
//==========================================================================
bool keyboard_read(uint8* key);

//==========================================================================
//��������: keyboard_enable_int
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: ʹ�ܼ����жϣ����а���������ʱ�������ж�
//��ע: �����������ڶ˿�ΪPORTAʱ���жϷ�����ΪPORTA_IRQHandler
//==========================================================================
void keyboard_enable_int();

//==========================================================================
//��������: keyboard_disable_int
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: �رռ����ж�
//==========================================================================
void keyboard_disable_int();

//==========================================================================
//��������: keyboard_get_int
//��������: true:�����ж�; false:δ�����ж�
//����˵��: ��
//���ܸ�Ҫ: ��ȡ�����жϱ�־
//==========================================================================
bool keyboard_get_int();

//==========================================================================
//��������: keyboard_clear_int
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: ��������жϱ�־
//��ע: �ڶ�ȡ���̺�������жϱ�־
//==========================================================================
void keyboard_clear_int();

#endif
