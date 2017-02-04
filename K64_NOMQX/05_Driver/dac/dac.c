//==========================================================================
//�ļ����ƣ�dac.c
//���ܸ�Ҫ��K64 DAC�ײ���������Դ�ļ�
//==========================================================================

#include "dac.h"

//DAC��ģ�����ַ
static DAC_Type * const dac_table[] = DAC_BASE_PTRS;
//DAC��ģ���ж������
static const IRQn_Type dac_irq_table[] = { DAC0_IRQn, DAC1_IRQn };

void dac_init(uint8 mod, uint8 ref_vtg) {
	DAC_Type * dac_ptr;	//DAC����ַ

	//��ȡDAC����ַ
	dac_ptr = dac_table[mod];

	//������ӦDACģ��ʱ����
	if (mod == DAC_MOD0) {
		REG_SET_MASK(SIM_SCGC2, SIM_SCGC2_DAC0_MASK);
	} else {
		REG_SET_MASK(SIM_SCGC2, SIM_SCGC2_DAC1_MASK);
	}

	//Ĭ�Ϲرջ�����
	REG_CLR_MASK(DAC_C1_REG(dac_ptr), DAC_C1_DACBFEN_MASK);

	//���òο���ѹԴ
	if (ref_vtg == DAC_REF_VTG_VREFOUT) {
		REG_CLR_MASK(DAC_C0_REG(dac_ptr), DAC_C0_DACRFS_MASK);
	} else {
		REG_SET_MASK(DAC_C0_REG(dac_ptr), DAC_C0_DACRFS_MASK);
	}

	//ʹ��DACģ��
	REG_SET_MASK(DAC_C0_REG(dac_ptr), DAC_C0_DACEN_MASK);
}

void dac_set_buffer_val(uint8 mod, uint8 no, uint16 val) {
	DAC_Type * dac_ptr;	//DAC����ַ

	//��ȡDAC����ַ
	dac_ptr = dac_table[mod];

	//���û�����ֵ
	REG_SET_VAL(DAC_DATL_REG(dac_ptr,no), DAC_DATL_DATA0(val));
	REG_SET_VAL(DAC_DATH_REG(dac_ptr,no),
			DAC_DATH_DATA1(val>>DAC_DATL_DATA0_WIDTH));
}
