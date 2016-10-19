#ifndef _STRUCTS_H
#define _STRUCTS_H

#include "common.h"

//�����Ʋ���
typedef struct
{
	uint_8 is_start;	//�Ƿ�ʼ������־��0Ϊδ��ʼ��1Ϊ��ʼ
	float duty;			//ռ�ձ�
	uint_8 dir;			//�仯����0Ϊ�䰵��1Ϊ����
	float radio;		//ÿ�α仯��ռ�ձȴ�С
} BreatheLightParams, *BreatheLightParamsPtr;

//ʱ���־����
typedef struct
{
	uint_8 f_50ms;
	uint_8 f_1s;
} TimeFlag, *TimeFlagPtr;

//ʱ�����������
typedef struct
{
	uint_8 c_50ms;
	uint_8 c_1s;
} TimeCounter, *TimeCounterPtr;

//��Ϣ����
typedef struct
{
	uint_8 msg[16];		//�洢��Ϣ������
	uint_16 front;		//�����±�
	uint_16 rear;		//��β�±�
} MsgQueue, *MsgQueuePtr;

#endif
