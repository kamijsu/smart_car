/*
 * includes.h
 *
 *  Created on: 2015��4��16��
 *      Author: SUMCU
 */

#ifndef INCLUDES_H_
#define INCLUDES_H_


//����ʹ�õ��Ĺ���ͷ�ļ�
#include "common.h"
#include "light.h"
#include "uart.h"
#include "printf.h"
#include "mqxlite.h"

//��ֹȫ�ֱ����ظ�������ǰ׺������
#ifdef GLOBLE_VAR                 //GLOBLE_VAR��main.c�ļ��к궨��
#define G_VAR_PREFIX          //main.c�ļ���ʹ��ȫ�ֱ������ӡ�extern��ǰ׺
#else
#define G_VAR_PREFIX  extern  //�����ļ���ʹ��ȫ�ֱ����Զ��ӡ�extern��ǰ׺
#endif

//����곣��
#define RUN_COUNTER_MAX   3500000ul   //С����˸Ƶ��

#endif /* INCLUDES_H_ */
