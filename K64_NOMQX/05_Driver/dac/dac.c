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

//�����Ƿ�ʹ�ܻ���������������ֵ��������
void dac_set_buffer_val(uint8 mod, uint8 index, uint16 val) {
	DAC_Type * dac_ptr;	//DAC����ַ

	//��ȡDAC����ַ
	dac_ptr = dac_table[mod];

	//���û�����ֵ
	REG_SET_VAL(DAC_DATL_REG(dac_ptr,index), DAC_DATL_DATA0(val));
	REG_SET_VAL(DAC_DATH_REG(dac_ptr,index),
			DAC_DATH_DATA1(val>>DAC_DATL_DATA0_WIDTH));
}

uint16 dac_get_buffer_val(uint8 mod, uint8 index) {
	DAC_Type * dac_ptr;	//DAC����ַ

	//��ȡDAC����ַ
	dac_ptr = dac_table[mod];

	//���ػ�����ֵ
	return (DAC_DATH_REG(dac_ptr,index) << DAC_DATL_DATA0_WIDTH)
			| DAC_DATL_REG(dac_ptr, index);
}

void dac_enable_buffer(uint8 mod, uint8 buffer_mode, uint8 max_index,
		uint8 watermark, uint8 trigger) {
	DAC_Type * dac_ptr;	//DAC����ַ

	//��ȡDAC����ַ
	dac_ptr = dac_table[mod];

	//�رո��������ж�
	REG_CLR_MASK(DAC_C0_REG(dac_ptr),
			DAC_C0_DACBWIEN_MASK|DAC_C0_DACBTIEN_MASK|DAC_C0_DACBBIEN_MASK);

	//���ô���ת����ʽ
	if (trigger == DAC_TRIGGER_HARDWARE) {
		REG_CLR_MASK(DAC_C0_REG(dac_ptr), DAC_C0_DACTRGSEL_MASK);
	} else {
		REG_SET_MASK(DAC_C0_REG(dac_ptr), DAC_C0_DACTRGSEL_MASK);
	}

	//���û���������ģʽ��ˮӡ
	REG_CLR_MASK(DAC_C1_REG(dac_ptr), DAC_C1_DACBFMD_MASK|DAC_C1_DACBFWM_MASK);
	REG_SET_MASK(DAC_C1_REG(dac_ptr),
			DAC_C1_DACBFMD(buffer_mode)|DAC_C1_DACBFWM(watermark));

	//���û�����ָ��λ�����ޣ��������õ�ǰָ��λ��Ϊ0
	REG_SET_VAL(DAC_C2_REG(dac_ptr), DAC_C2_DACBFUP(max_index));

	//ʹ��DAC������
	REG_SET_MASK(DAC_C1_REG(dac_ptr), DAC_C1_DACBFEN_MASK);
}

//�رջ������󣬵�ǰָ��λ�ñ���Ϊ0
void dac_disable_buffer(uint8 mod) {
	//�ر�DAC������
	REG_CLR_MASK(DAC_C1_REG(dac_table[mod]), DAC_C1_DACBFEN_MASK);
}

//δʹ�ܻ�����ʱ����������ǰָ��λ�ò���仯
void dac_software_trigger(uint8 mod) {
	//����һ���������
	REG_SET_MASK(DAC_C0_REG(dac_table[mod]), DAC_C0_DACSWTRG_MASK);
}

//δʹ�ܻ�����ʱ�����õ�ǰָ��λ����Ч
void dac_set_buffer_index(uint8 mod, uint8 index) {
	DAC_Type * dac_ptr;	//DAC����ַ

	//��ȡDAC����ַ
	dac_ptr = dac_table[mod];

	//���û�������ǰָ��λ�ã����ַ�ʽ����������ָ��λ��Ϊ0
	REG_SET_VAL(DAC_C2_REG(dac_ptr),
			REG_GET_MASK(DAC_C2_REG(dac_ptr),DAC_C2_DACBFUP_MASK)|DAC_C2_DACBFRP(index));
}

uint8 dac_get_buffer_index(uint8 mod) {
	//��ȡ��������ǰָ��λ��
	return REG_GET_MASK(DAC_C2_REG(dac_table[mod]), DAC_C2_DACBFRP_MASK)
			>> DAC_C2_DACBFRP_SHIFT;
}

void dac_set_buffer_int(uint8 mod, bool index_bottom_int, bool index_top_int,
		bool watermark_int) {
	DAC_Type * dac_ptr;	//DAC����ַ

	//��ȡDAC����ַ
	dac_ptr = dac_table[mod];

	if (index_bottom_int) {
		//����жϱ�־
		REG_CLR_MASK(DAC_SR_REG(dac_ptr), DAC_SR_DACBFRPBF_MASK);
		//ʹ��ָ��λ�õײ��ж�
		REG_SET_MASK(DAC_C0_REG(dac_ptr), DAC_C0_DACBBIEN_MASK);
	} else {
		//�ر�ָ��λ�õײ��ж�
		REG_CLR_MASK(DAC_C0_REG(dac_ptr), DAC_C0_DACBBIEN_MASK);
	}

	if (index_top_int) {
		//����жϱ�־
		REG_CLR_MASK(DAC_SR_REG(dac_ptr), DAC_SR_DACBFRPTF_MASK);
		//ʹ��ָ��λ�ö����ж�
		REG_SET_MASK(DAC_C0_REG(dac_ptr), DAC_C0_DACBTIEN_MASK);
	} else {
		//�ر�ָ��λ�ö����ж�
		REG_CLR_MASK(DAC_C0_REG(dac_ptr), DAC_C0_DACBTIEN_MASK);
	}

	if (watermark_int) {
		//����жϱ�־
		REG_CLR_MASK(DAC_SR_REG(dac_ptr), DAC_SR_DACBFWMF_MASK);
		//ʹ��ˮӡ�ж�
		REG_SET_MASK(DAC_C0_REG(dac_ptr), DAC_C0_DACBWIEN_MASK);
	} else {
		//�ر�ˮӡ�ж�
		REG_CLR_MASK(DAC_C0_REG(dac_ptr), DAC_C0_DACBWIEN_MASK);
	}

	if (!index_bottom_int && !index_top_int && !watermark_int) {
		//���ر������жϣ����ֹ���ո�DACģ���ж�����
		DISABLE_IRQ(dac_irq_table[mod]);
	} else {
		//�����ж�ʹ�ܣ���������ո�DACģ���ж�����
		ENABLE_IRQ(dac_irq_table[mod]);
	}
}

bool dac_get_index_bottom_int(uint8 mod) {
	//��ȡ�жϱ�־
	return REG_GET_MASK(DAC_SR_REG(dac_table[mod]), DAC_SR_DACBFRPBF_MASK) ?
			true : false;
}

void dac_clear_index_bottom_int(uint8 mod) {
	//����жϱ�־
	REG_CLR_MASK(DAC_SR_REG(dac_table[mod]), DAC_SR_DACBFRPBF_MASK);
}

bool dac_get_index_top_int(uint8 mod) {
	//��ȡ�жϱ�־
	return REG_GET_MASK(DAC_SR_REG(dac_table[mod]), DAC_SR_DACBFRPTF_MASK) ?
			true : false;
}

void dac_clear_index_top_int(uint8 mod) {
	//����жϱ�־
	REG_CLR_MASK(DAC_SR_REG(dac_table[mod]), DAC_SR_DACBFRPTF_MASK);
}

bool dac_get_watermark_int(uint8 mod) {
	//��ȡ�жϱ�־
	return REG_GET_MASK(DAC_SR_REG(dac_table[mod]), DAC_SR_DACBFWMF_MASK) ?
			true : false;
}

void dac_clear_watermark_int(uint8 mod) {
	//����жϱ�־
	REG_CLR_MASK(DAC_SR_REG(dac_table[mod]), DAC_SR_DACBFWMF_MASK);
}
