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
//UART0:  0=PTA2-PTA1;  1=PTA14-PTA15;  2=PTB17-PTB16;  3=PTD7-PTD6;
#define UART_MOD0_SETUP		(1)
//UART1:  0=PTC4-PTC3;  1=PTE0-PTE1;
#define UART_MOD1_SETUP		(1)
//UART2:  0=PTD3-PTD2;
#define UART_MOD2_SETUP		(0)
//UART3:  0=PTB11-PTB10;  1=PTC17-PTC16;  2=PTE4-PTE5;
#define UART_MOD3_SETUP		(0)
//UART4:  0=PTC15-PTC14;  1=PTE24-PTE25;
#define UART_MOD4_SETUP		(1)
//UART5:  0=PTD9-PTD8;  1=PTE8-PTE9;
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

//���巢�������ʱ��ѯ���޴�����RPΪroll poling��д��
#define UART_RP_TIME_SEND		(0xFBBBu)	//������ѯ���޴���
#define UART_RP_TIME_RECEIVE	(0xFBBBu)	//������ѯ���޴���

//==========================================================================
//��������: uart_init
//��������: ��
//����˵��: mod:UARTģ��ţ�UART_MODx��xΪģ���
//         baud:������:(600) | 1200 | 2400 | 4800 | 9600 | 14400 | 19200 |
//                     38400 | 56000 | 57600 | 115200
//         parity_mode:У��ģʽ��UART_PARITY_DISABLED:������У��;
//                     UART_PARITY_ODD:��У��; UART_PARITY_EVEN:żУ��
//         stop_bit:ֹͣλ��UART_STOP_BIT_1:1λֹͣλ; UART_STOP_BIT_2:2λֹͣλ
//���ܸ�Ҫ: ��ʼ��UARTģ��
//��ע: ������Ϊ600ʱ��UART0��UART1�޷�ʹ��
//==========================================================================
void uart_init(uint8 mod, uint32 baud, uint8 parity_mode, uint8 stop_bit);

//==========================================================================
//��������: uart_send1
//��������: true:���ͳɹ�; false:����ʧ��
//����˵��: mod:UARTģ��ţ�UART_MODx��xΪģ���
//         byte:��Ҫ���͵��ֽ�����
//���ܸ�Ҫ: ����1���ֽ�����
//==========================================================================
bool uart_send1(uint8 mod, uint8 byte);

//==========================================================================
//��������: uart_sendN
//��������: true:���ͳɹ�; false:����ʧ��
//����˵��: mod:UARTģ��ţ�UART_MODx��xΪģ���
//         len:���͵��ֽ���
//         buff:���ͻ�����
//���ܸ�Ҫ: ����N���ֽ�����
//==========================================================================
bool uart_sendN(uint8 mod, uint32 len, uint8* buff);

//==========================================================================
//��������: uart_send_string
//��������: true:���ͳɹ�; false:����ʧ��
//����˵��: mod:UARTģ��ţ�UART_MODx��xΪģ���
//         str:�����ַ������׵�ַ
//���ܸ�Ҫ: ����һ����'\0'�������ַ��������ᷢ��'\0'
//==========================================================================
bool uart_send_string(uint8 mod, uint8* str);

//==========================================================================
//��������: uart_re1
//��������: true:���ճɹ�; false:����ʧ��
//����˵��: mod:UARTģ��ţ�UART_MODx��xΪģ���
//         byte:��Ҫ�����ֽ����ݵĵ�ַ
//���ܸ�Ҫ: ����1���ֽ�����
//==========================================================================
bool uart_re1(uint8 mod, uint8* byte);

//==========================================================================
//��������: uart_re1_parity
//��������: true:���ճɹ�; false:����ʧ��
//����˵��: mod:UARTģ��ţ�UART_MODx��xΪģ���
//         byte:��Ҫ�����ֽ����ݵĵ�ַ
//         err:���ճɹ�ʱ����żУ�����޴�����δ����У�飬��ʼ���޴���
//���ܸ�Ҫ: ����1���ֽ����ݣ����ж�У��λ���޴���
//==========================================================================
bool uart_re1_parity(uint8 mod, uint8* byte, bool* err);

//==========================================================================
//��������: uart_enable_re_int
//��������: ��
//����˵��: mod:UARTģ��ţ�UART_MODx��xΪģ���
//���ܸ�Ҫ: ʹ�ܴ���ģ������жϣ�ͨ���������ݿ�������жϱ�־
//==========================================================================
void uart_enable_re_int(uint8 mod);

//==========================================================================
//��������: uart_disable_re_int
//��������: ��
//����˵��: mod:UARTģ��ţ�UART_MODx��xΪģ���
//���ܸ�Ҫ: �رմ���ģ������ж�
//==========================================================================
void uart_disable_re_int(uint8 mod);

//���ݸ�ģ�������õ������飬������ʹ�õ����ſ��ƼĴ���
#ifdef UART_MOD0_SETUP
#if(UART_MOD0_SETUP == 0)
#define UART_MOD0_TX_PCR	PORTA_PCR2
#define UART_MOD0_RX_PCR	PORTA_PCR1
#elif(UART_MOD0_SETUP == 1)
#define UART_MOD0_TX_PCR	PORTA_PCR14
#define UART_MOD0_RX_PCR	PORTA_PCR15
#elif(UART_MOD0_SETUP == 2)
#define UART_MOD0_TX_PCR	PORTB_PCR17
#define UART_MOD0_RX_PCR	PORTB_PCR16
#elif(UART_MOD0_SETUP == 3)
#define UART_MOD0_TX_PCR	PORTD_PCR7
#define UART_MOD0_RX_PCR	PORTD_PCR6
#endif
#endif

#ifdef UART_MOD1_SETUP
#if(UART_MOD1_SETUP == 0)
#define UART_MOD1_TX_PCR	PORTC_PCR4
#define UART_MOD1_RX_PCR	PORTC_PCR3
#elif(UART_MOD1_SETUP == 1)
#define UART_MOD1_TX_PCR	PORTE_PCR0
#define UART_MOD1_RX_PCR	PORTE_PCR1
#endif
#endif

#ifdef UART_MOD2_SETUP
#if(UART_MOD2_SETUP == 0)
#define UART_MOD2_TX_PCR	PORTD_PCR3
#define UART_MOD2_RX_PCR	PORTD_PCR2
#endif
#endif

#ifdef UART_MOD3_SETUP
#if(UART_MOD3_SETUP == 0)
#define UART_MOD3_TX_PCR	PORTB_PCR11
#define UART_MOD3_RX_PCR	PORTB_PCR10
#elif(UART_MOD3_SETUP == 1)
#define UART_MOD3_TX_PCR	PORTC_PCR17
#define UART_MOD3_RX_PCR	PORTC_PCR16
#elif(UART_MOD3_SETUP == 2)
#define UART_MOD3_TX_PCR	PORTE_PCR4
#define UART_MOD3_RX_PCR	PORTE_PCR5
#endif
#endif

#ifdef UART_MOD4_SETUP
#if(UART_MOD4_SETUP == 0)
#define UART_MOD4_TX_PCR	PORTC_PCR15
#define UART_MOD4_RX_PCR	PORTC_PCR14
#elif(UART_MOD4_SETUP == 1)
#define UART_MOD4_TX_PCR	PORTE_PCR24
#define UART_MOD4_RX_PCR	PORTE_PCR25
#endif
#endif

#ifdef UART_MOD5_SETUP
#if(UART_MOD5_SETUP == 0)
#define UART_MOD5_TX_PCR	PORTD_PCR9
#define UART_MOD5_RX_PCR	PORTD_PCR8
#elif(UART_MOD5_SETUP == 1)
#define UART_MOD5_TX_PCR	PORTE_PCR8
#define UART_MOD5_RX_PCR	PORTE_PCR9
#endif
#endif

#endif
