//==========================================================================
//�ļ����ƣ�ftm.c
//���ܸ�Ҫ��K64 FTM�ײ���������Դ�ļ�
//==========================================================================

#include "ftm.h"

//FTM��ģ�����ַ
static FTM_Type * const ftm_table[] = { FTM0, FTM1, FTM2, FTM3 };
//FTM��ģ���ж������
static const IRQn_Type ftm_irq_table[] = { FTM0_IRQn, FTM1_IRQn, FTM2_IRQn,
		FTM3_IRQn };

