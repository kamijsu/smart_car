//˵���������ļ����µ�Doc�ļ�����Readme.txt�ļ�

#define GLOBLE_VAR  //ֻ����main.c�ж���һ�Σ�������ֹȫ�ֱ������ظ�����

#include "includes.h"

extern const MQXLITE_INITIALIZATION_STRUCT  MQX_init_struct;

void main(void)
{
    // ��ʼ��mqx
	_mqxlite_init((MQXLITE_INITIALIZATION_STRUCT_PTR) &MQX_init_struct);
    // ����mqx
	_mqxlite();
}
