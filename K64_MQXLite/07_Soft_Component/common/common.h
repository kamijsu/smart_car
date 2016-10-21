//----------------------------------------------------------------------------
//common.h
//2015��4��3��
//Author: SUMCU
//----------------------------------------------------------------------------

#ifndef COMMON_H_
#define COMMON_H_

// 1��оƬ�Ĵ���ӳ���ļ����������ں������ļ�
#include "core_cmFunc.h"
#include "core_cmInstr.h"
#include "core_cm4.h"
#include "MK64F12.h"           // ����оƬͷ�ļ�
#include "system_MK64F12.h"    // ����оƬϵͳ��ʼ���ļ�

// 2�����忪�����ж�
#define ENABLE_INTERRUPTS   __enable_irq   // �����ж�
#define DISABLE_INTERRUPTS  __disable_irq  // �����ж�


// 3��λ�����꺯������λ����λ����üĴ���һλ��״̬��
#define BSET(bit,Register)  ((Register)|= (1<<(bit)))    // �üĴ�����һλ
#define BCLR(bit,Register)  ((Register) &= ~(1<<(bit)))  // ��Ĵ�����һλ
#define BGET(bit,Register)  (((Register) >> (bit)) & 1)  // ��üĴ���һλ��״̬


//4���ض�������������ͣ����ͱ����궨�壩
typedef unsigned char        uint_8;   // �޷���8λ�����ֽ�
typedef unsigned short int   uint_16;  // �޷���16λ������
typedef unsigned long int    uint_32;  // �޷���32λ��������
typedef unsigned long long   uint_64;  // �޷���64λ����������

typedef signed char          int_8;    // �з��ŵ�8λ�����ֽ�
typedef signed short int     int_16;   // �з��ŵ�16λ������
typedef signed long int      int_32;   // �з��ŵ�32λ��������
typedef signed long long     int_64;   // �з��ŵ�64λ����������

//5. ����ϵͳʹ�õ�ʱ��Ƶ��
#define  SYSTEM_CLK_KHZ   SystemCoreClock/1000     // оƬϵͳʱ��Ƶ��(KHz)
#define  BUS_CLK_KHZ      SYSTEM_CLK_KHZ/2         // оƬ����ʱ��Ƶ��(KHz)


#endif /* COMMON_H_ */
