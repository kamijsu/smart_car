//==========================================================================
//�ļ����ƣ�dac.h
//���ܸ�Ҫ��K64 DAC�ײ���������ͷ�ļ�
//==========================================================================

#ifndef _DAC_H
#define _DAC_H

//��������ͷ�ļ�
#include "common.h"

//����DACģ���
#define DAC_MOD0	(0)		//���ź�:27
#define DAC_MOD1	(1)		//K64 100����LQFP��װ�޸�ģ��

//����DACģ��ο���ѹԴ
#define DAC_REF_VTG_VREFOUT		(0)		//VREF_OUT
#define DAC_REF_VTG_VDDA		(1)		//VDDA

void dac_init(uint8 mod, uint8 ref_vtg);

void dac_set_buffer_val(uint8 mod, uint8 no, uint16 val);

#endif
