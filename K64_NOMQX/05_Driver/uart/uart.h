//==========================================================================
//�ļ����ƣ�uart.h
//���ܸ�Ҫ��K64 UART�ײ���������ͷ�ļ�
//==========================================================================

#ifndef _UART_H
#define _UART_H

//��������ͷ�ļ�
#include "common.h"

//���崮��ģ��ţ�MODΪmodule��д��
#define UART_MOD0	(0)
#define UART_MOD1	(1)
#define UART_MOD2	(2)
#define UART_MOD3	(3)
#define UART_MOD4	(4)
#define UART_MOD5	(5)

//���ø�����ģ�����������飬ͨ�����ĺ궨���ֵ��ѡ�������飬������ǰ��ΪTX������ΪRX
//UART0:  0=A2-A1;    1=A14-A15;  2=B17-B16;  3=D7-D6;
#define UART_MOD0_SETUP		(1)
//UART1:  0=C4-C3;    1=E0-E1;
#define UART_MOD1_SETUP		(1)
//UART2:  0=D3-D2;
#define UART_MOD2_SETUP		(0)
//UART3:  0=B11-B10;  1=C17-C16;  2=E4-E5;
#define UART_MOD3_SETUP		(0)
//UART4:  0=C15-C14;  1=E24-E25;
#define UART_MOD4_SETUP		(1)
//UART5:  0=D9-D8;    1=E8-E9;
#define UART_MOD5_SETUP		(0)

//����UART��ģ�鹤��ʱ��Ƶ�ʣ���λHz
#define UART_WORK_FREQ1		(SYSTEM_CLK_KHZ * 1000)	//UART0��UART1ʹ��ϵͳʱ��
#define UART_WORK_FREQ2		(BUS_CLK_KHZ * 1000)	//����UARTģ��ʹ������ʱ��

//����UARTģ��У��ģʽ
#define UART_PARITY_DISABLED	(0)		//������У��
#define UART_PARITY_ODD			(1)		//��У��
#define UART_PARITY_EVEN		(2)		//żУ��

//����UARTģ��ֹͣλ
#define UART_STOP_BIT_1			(1)		//1λֹͣλ
#define UART_STOP_BIT_2			(2)		//2λֹͣλ

//����UARTģ��λ����˳��
#define UART_BIT_ORDER_LSB		(0)		//�����Чλ����������ʼλ��Ϊλ0
#define UART_BIT_ORDER_MSB		(1)		//�����Чλ����������ʼλ��Ϊλ7

//���巢�������ʱ��ѯ���޴��������32λ(RPΪroll poling��д)
#define UART_RP_TIME_SEND		(0xFBBBu)	//������ѯ���޴���
#define UART_RP_TIME_RECEIVE	(0xFBBBu)	//������ѯ���޴���

//==========================================================================
//��������: uart_init
//��������: ��
//����˵��: mod:UARTģ���:
//             UART_MODx��xΪģ���;
//         baud:������:
//              (600) | 1200  | 2400  | 4800 | 9600 | 14400 | 19200 |
//              38400 | 56000 | 57600 | 115200
//         parity_mode:У��ģʽ:
//                     UART_PARITY_DISABLED:������У��;
//                     UART_PARITY_ODD:     ��У��;
//                     UART_PARITY_EVEN:    żУ��;
//         stop_bit:ֹͣλ:
//                  UART_STOP_BIT_1:1λֹͣλ;
//                  UART_STOP_BIT_2:2λֹͣλ;
//         bit_order:λ����˳��:
//                   UART_BIT_ORDER_LSB:�����Чλ;
//                   UART_BIT_ORDER_MSB:�����Чλ;
//���ܸ�Ҫ: ��ʼ��UARTģ��
//��ע: ������Ϊ600ʱ��UART0��UART1�޷�ʹ��;
//     λ����˳���Ӱ������λ������ʼλ��У��λ��ֹͣλ����Ӱ��
//==========================================================================
void uart_init(uint8 mod, uint32 baud, uint8 parity_mode, uint8 stop_bit,
		uint8 bit_order);

//==========================================================================
//��������: uart_send1
//��������: true:���ͳɹ�; false:����ʧ��
//����˵��: mod:UARTģ���:
//             UART_MODx��xΪģ���;
//         byte:��Ҫ���͵��ֽ�����
//���ܸ�Ҫ: ����1���ֽ�����
//==========================================================================
bool uart_send1(uint8 mod, uint8 byte);

//==========================================================================
//��������: uart_sendN
//��������: true:���ͳɹ�; false:����ʧ��
//����˵��: mod:UARTģ���:
//             UART_MODx��xΪģ���;
//         buff:���ͻ������׵�ַ
//         len:���͵��ֽ���
//���ܸ�Ҫ: ����N���ֽ�����
//==========================================================================
bool uart_sendN(uint8 mod, uint8* buff, uint32 len);

//==========================================================================
//��������: uart_send_string
//��������: true:���ͳɹ�; false:����ʧ��
//����˵��: mod:UARTģ���:
//             UART_MODx��xΪģ���;
//         str:�����ַ������׵�ַ
//���ܸ�Ҫ: ����һ����'\0'�������ַ��������ᷢ��'\0'
//==========================================================================
bool uart_send_string(uint8 mod, uint8* str);

//==========================================================================
//��������: uart_re1
//��������: true:���ճɹ�; false:����ʧ��
//����˵��: mod:UARTģ���:
//             UART_MODx��xΪģ���;
//         byte:��Ҫ�����ֽ����ݵĵ�ַ
//���ܸ�Ҫ: ����1���ֽ�����
//==========================================================================
bool uart_re1(uint8 mod, uint8* byte);

//==========================================================================
//��������: uart_re1_parity
//��������: true:���ճɹ�; false:����ʧ��
//����˵��: mod:UARTģ���:
//             UART_MODx��xΪģ���;
//         byte:��Ҫ�����ֽ����ݵĵ�ַ
//         err:���ճɹ�ʱ����żУ�����޴�����δ����У�飬��ʼ���޴���
//���ܸ�Ҫ: ����1���ֽ����ݣ����ж�У��λ���޴���
//==========================================================================
bool uart_re1_parity(uint8 mod, uint8* byte, bool* err);

//==========================================================================
//��������: uart_enable_re_int
//��������: ��
//����˵��: mod:UARTģ���:
//             UART_MODx��xΪģ���;
//���ܸ�Ҫ: ʹ�ܴ���ģ������жϣ�ͨ���������ݿ�������жϱ�־
//==========================================================================
void uart_enable_re_int(uint8 mod);

//==========================================================================
//��������: uart_disable_re_int
//��������: ��
//����˵��: mod:UARTģ���:
//             UART_MODx��xΪģ���;
//���ܸ�Ҫ: �رմ���ģ������ж�
//==========================================================================
void uart_disable_re_int(uint8 mod);

//���ݸ�ģ�������õ������飬������Ӧģ���RX��TX���źţ��Լ���Ӧ��PCR��MUXֵ
#ifdef UART_MOD0_SETUP
#if(UART_MOD0_SETUP == 0)
#define UART_MOD0_TX_PIN	(COM_PORTA|2)
#define UART_MOD0_RX_PIN	(COM_PORTA|1)
#define UART_MOD0_PCR_MUX	(2)
#else
#if(UART_MOD0_SETUP == 1)
#define UART_MOD0_TX_PIN	(COM_PORTA|14)
#define UART_MOD0_RX_PIN	(COM_PORTA|15)
#elif(UART_MOD0_SETUP == 2)
#define UART_MOD0_TX_PIN	(COM_PORTB|17)
#define UART_MOD0_RX_PIN	(COM_PORTB|16)
#elif(UART_MOD0_SETUP == 3)
#define UART_MOD0_TX_PIN	(COM_PORTD|7)
#define UART_MOD0_RX_PIN	(COM_PORTD|6)
#endif
#define UART_MOD0_PCR_MUX	(3)
#endif
#endif

#ifdef UART_MOD1_SETUP
#if(UART_MOD1_SETUP == 0)
#define UART_MOD1_TX_PIN	(COM_PORTC|4)
#define UART_MOD1_RX_PIN	(COM_PORTC|3)
#elif(UART_MOD1_SETUP == 1)
#define UART_MOD1_TX_PIN	(COM_PORTE|0)
#define UART_MOD1_RX_PIN	(COM_PORTE|1)
#endif
#define UART_MOD1_PCR_MUX	(3)
#endif

#ifdef UART_MOD2_SETUP
#if(UART_MOD2_SETUP == 0)
#define UART_MOD2_TX_PIN	(COM_PORTD|3)
#define UART_MOD2_RX_PIN	(COM_PORTD|2)
#endif
#define UART_MOD2_PCR_MUX	(3)
#endif

#ifdef UART_MOD3_SETUP
#if(UART_MOD3_SETUP == 0)
#define UART_MOD3_TX_PIN	(COM_PORTB|11)
#define UART_MOD3_RX_PIN	(COM_PORTB|10)
#elif(UART_MOD3_SETUP == 1)
#define UART_MOD3_TX_PIN	(COM_PORTC|17)
#define UART_MOD3_RX_PIN	(COM_PORTC|16)
#elif(UART_MOD3_SETUP == 2)
#define UART_MOD3_TX_PIN	(COM_PORTE|4)
#define UART_MOD3_RX_PIN	(COM_PORTE|5)
#endif
#define UART_MOD3_PCR_MUX	(3)
#endif

#ifdef UART_MOD4_SETUP
#if(UART_MOD4_SETUP == 0)
#define UART_MOD4_TX_PIN	(COM_PORTC|15)
#define UART_MOD4_RX_PIN	(COM_PORTC|14)
#elif(UART_MOD4_SETUP == 1)
#define UART_MOD4_TX_PIN	(COM_PORTE|24)
#define UART_MOD4_RX_PIN	(COM_PORTE|25)
#endif
#define UART_MOD4_PCR_MUX	(3)
#endif

#ifdef UART_MOD5_SETUP
#if(UART_MOD5_SETUP == 0)
#define UART_MOD5_TX_PIN	(COM_PORTD|9)
#define UART_MOD5_RX_PIN	(COM_PORTD|8)
#elif(UART_MOD5_SETUP == 1)
#define UART_MOD5_TX_PIN	(COM_PORTE|8)
#define UART_MOD5_RX_PIN	(COM_PORTE|9)
#endif
#define UART_MOD5_PCR_MUX	(3)
#endif

#endif
