#ifndef __APP_INC_H_
#define __APP_INC_H_
//---------------------------------------------------------------------------

//1.����ͷ�ļ�
#include "bsp.h"
#include "mqxlite.h"
#include "mqx_inc.h"
#include "light.h"
#include "lwevent.h"
#include "printf.h"
#include "string.h"



//2.����ȫ�ֱ���
//��ֹȫ�ֱ����ظ�������ǰ׺������
#ifdef GLOBLE_VAR             //GLOBLE_VAR��task_main.c�ļ��к궨��
#define G_VAR_PREFIX          //task_main.c�ļ���ʹ��ȫ�ֱ������ӡ�extern��ǰ׺
#else
#define G_VAR_PREFIX  extern  //�����ļ���ʹ��ȫ�ֱ����Զ��ӡ�extern��ǰ׺
#endif
//����ȫ�ֱ�����ȫ�ֱ�������ǰһ��ǰ׺G_VAR_PREFIX��

//G_VAR_PREFIX LWEVENT_STRUCT  lwevent_group1;      //�������¼���

#define DELAY_TIMES    200           // ÿ��tick��Ӧ5ms

//1.4 �����������¼���
G_VAR_PREFIX LWEVENT_STRUCT  lwevent_group;  //�������¼���

//3.�Ǽ�����ģ����
#define TASK_MAIN                     1
#define TASK_LIGHT                    2

//4.Ϊ���񴴽�����ջ
#define TASK_MAIN_STACK_SIZE      (sizeof(TD_STRUCT) + 2048 + PSP_STACK_ALIGNMENT + 1)
#define TASK_LIGHT_STACK_SIZE     (sizeof(TD_STRUCT) + 512 + PSP_STACK_ALIGNMENT + 1)

G_VAR_PREFIX uint_8 task_main_stack[TASK_MAIN_STACK_SIZE];
G_VAR_PREFIX uint_8 task_light_stack[TASK_LIGHT_STACK_SIZE];

//5.����������
void task_main(uint32_t initial_data);
void task_light(uint32_t initial_data);

//6.�����жϴ�����
void UART1_RX_ISR();


//---------------------------------------------------------------------------
#endif    //app_inc.h
