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

//����DACģ�黺��������ģʽ
#define DAC_BUFFER_MODE_NORMAL			(0)		//��ͨģʽ
#define DAC_BUFFER_MODE_SWING			(1)		//ҡ��ģʽ
#define DAC_BUFFER_MODE_ONE_TIME_SCAN	(2)		//����ɨ��ģʽ

//����DACģ�黺����ˮӡѡ��
#define DAC_WATERMARK_WORD_1	(0)		//ˮӡ���뻺����ָ��λ������1����
#define DAC_WATERMARK_WORD_2	(1)		//ˮӡ���뻺����ָ��λ������2����
#define DAC_WATERMARK_WORD_3	(2)		//ˮӡ���뻺����ָ��λ������3����
#define DAC_WATERMARK_WORD_4	(3)		//ˮӡ���뻺����ָ��λ������4����

//����DACģ�鴥��ת����ʽ
#define DAC_TRIGGER_HARDWARE	(0)		//Ӳ������
#define DAC_TRIGGER_SOFTWARE	(1)		//�������

void dac_init(uint8 mod, uint8 ref_vtg);

void dac_set_buffer_val(uint8 mod, uint8 index, uint16 val);

uint16 dac_get_buffer_val(uint8 mod, uint8 index);

void dac_enable_buffer(uint8 mod, uint8 buffer_mode, uint8 max_index,
		uint8 watermark, uint8 trigger);

void dac_disable_buffer(uint8 mod);

void dac_software_trigger(uint8 mod);

void dac_set_buffer_index(uint8 mod, uint8 index);

uint8 dac_get_buffer_index(uint8 mod);

#endif
