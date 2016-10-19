//======================================================================
//�ļ����ƣ�includes.h
//���ܸ�Ҫ��Ӧ�ù�����ͷ�ļ�
//��Ȩ���У����ݴ�ѧ��˼����Ƕ��ʽ����(sumcu.suda.edu.cn)
//�汾���£�2012-11-12  V2.0
//======================================================================

#ifndef _INCLUDES_H
#define _INCLUDES_H

#include "common.h"
#include "gpio.h"
#include "light.h"
#include "breathe_light.h"
#include "uart.h"
#include "tpm.h"
#include "pit.h"
#include "structs.h"
#include <stdio.h>
#include <string.h>

//��ֹȫ�ֱ����ظ�������ǰ׺������
#ifdef GLOBLE_VAR                 //GLOBLE_VAR��main.c�ļ��к궨��
#define G_VAR_PREFIX          //main.c�ļ���ʹ��ȫ�ֱ������ӡ�extern��ǰ׺
#else
#define G_VAR_PREFIX  extern  //�����ļ���ʹ��ȫ�ֱ����Զ��ӡ�extern��ǰ׺
#endif

//����ȫ�ֱ���
//��Ϣ����
G_VAR_PREFIX MsgQueue g_msg;
//ʱ���־
G_VAR_PREFIX TimeFlag g_time_flag;

//����С����˸Ƶ��
#define RUN_COUNTER_MAX   3000000ul

//������ʹ�õĴ���
#define UART_USE UART_2

#endif
