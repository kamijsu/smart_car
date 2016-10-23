//============================================================================
//�ļ����ƣ�includes.h
//���ܸ�Ҫ��Ӧ�ù�����ͷ�ļ�
//��Ȩ���У����ݴ�ѧ��˼����Ƕ��ʽ����(sumcu.suda.edu.cn)
//�汾���£�2012-11-12  V2.0
//          2014-11-9  V3.0
//============================================================================

#ifndef _INCLUDES_H
#define _INCLUDES_H

#include "common.h"
#include "adc.h"
#include "gpio.h"
#include "pit.h"
#include "pwm.h"
#include "uart.h"
#include "ems.h"
#include "encoder.h"
#include "gyro_acce.h"
#include "light.h"
#include "motor.h"
#include "reed_switch.h"
#include "control.h"
#include "structs.h"
#include "printf.h"
#include "visual_scope.h"

//��ֹȫ�ֱ����ظ�������ǰ׺��������
#ifdef GLOBLE_VAR                 //GLOBLE_VAR��main.c�ļ��к궨��
#define G_VAR_PREFIX          //main.c�ļ���ʹ��ȫ�ֱ������ӡ�extern��ǰ׺
#else
#define G_VAR_PREFIX  extern  //�����ļ���ʹ��ȫ�ֱ����Զ��ӡ�extern��ǰ׺
#endif

//����ȫ�ֱ���
//ʱ���־����
G_VAR_PREFIX TimeFlag time0_flag;

//���������������
G_VAR_PREFIX EncoderCounter encoder_counter;

//С����־����
G_VAR_PREFIX CarFlag car_flag;

//����С����˸Ƶ��
#define RUN_COUNTER_MAX   3500000ul

//����ʹ�õĵ��Ժ�
#define UART_USE  U_UART1

/* ����
 * UART_USE:    PTE0   TX;                PTE1   RX
 * ���ڽ���:		white  RX;                green  TX
 * MOTOR1:      PTC1(purple) PWM1;        PTC2(grey)    PWM2
 * MOTOR2:      PTC3(purple) PWM3;        PTC4(grey)    PWM4
 * gyro_acce:   PTE24(green) X����ٶ�(AR2); PTE25(blue)   Z����ٶ�(Zout)
 * SPEED1:		PTD0(black);       5V(brown);       GND(blue)
 * SPEED2:		PTD1(white);       5V(brown);       GND(blue)
 * REED_SWITCH_INPUT  PTD2;
 * REED_SWITCH_OUTPUT PTD3;
 * EMS_CH0:		PTB0(green)  OUTA
 * EMS_CH1:		PTB1(blue)   OUTC
 * EMS_CH2:		PTB2(green)  OUTB
 * EMS_CH3:		PTB3(blue)   OUTD
 * */

#endif