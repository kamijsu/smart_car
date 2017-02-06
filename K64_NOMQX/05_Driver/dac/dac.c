//==========================================================================
//�ļ����ƣ�dac.c
//���ܸ�Ҫ��K64 DAC�ײ���������Դ�ļ�
//==========================================================================

#include "dac.h"

//DAC��ģ�����ַ
static DAC_Type * const dac_table[] = DAC_BASE_PTRS;
//DAC��ģ���ж������
static const IRQn_Type dac_irq_table[] = { DAC0_IRQn, DAC1_IRQn };

//==========================================================================
//��������: dac_init
//��������: ��
//����˵��: mod:DACģ���:
//             DAC_MODx��xΪģ���;
//         ref_vtg:�ο���ѹԴ:
//                 DAC_REF_VTG_VREFOUT:VREF_OUT;
//                 DAC_REF_VTG_VDDA:   VDDA;
//���ܸ�Ҫ: ��ʼ��DACģ�飬Ĭ�Ϲرջ�������Ĭ�����ֵΪ0����(�ο���ѹֵ/4096)
//==========================================================================
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

//==========================================================================
//��������: dac_set_buffer_val
//��������: ��
//����˵��: mod:DACģ���:
//             DAC_MODx��xΪģ���;
//         index:������λ�ã�ȡֵ��ΧΪ[0,15]
//         val:������ֵ��ȡֵ��ΧΪ[0,4095]
//���ܸ�Ҫ: ����ָ��������λ�õ�ֵ
//��ע: �����ѹֵ = �ο���ѹֵ * (val + 1) / 4096��
//     valΪ��������ǰָ��λ�õ�val;
//     δʹ�ܻ�����ʱ���������������λ��Ϊ0��ֵ;
//     �����Ƿ�ʹ�ܻ���������������λ�õ�ֵ��������
//==========================================================================
void dac_set_buffer_val(uint8 mod, uint8 index, uint16 val) {
	DAC_Type * dac_ptr;	//DAC����ַ

	//��ȡDAC����ַ
	dac_ptr = dac_table[mod];

	//���û�����ֵ
	REG_SET_VAL(DAC_DATL_REG(dac_ptr,index), DAC_DATL_DATA0(val));
	REG_SET_VAL(DAC_DATH_REG(dac_ptr,index),
			DAC_DATH_DATA1(val>>DAC_DATL_DATA0_WIDTH));
}

//==========================================================================
//��������: dac_get_buffer_val
//��������: ������ֵ��ȡֵ��ΧΪ[0,4095]
//����˵��: mod:DACģ���:
//             DAC_MODx��xΪģ���;
//         index:������λ�ã�ȡֵ��ΧΪ[0,15]
//���ܸ�Ҫ: ��ȡָ��������λ�õ�ֵ
//==========================================================================
uint16 dac_get_buffer_val(uint8 mod, uint8 index) {
	DAC_Type * dac_ptr;	//DAC����ַ

	//��ȡDAC����ַ
	dac_ptr = dac_table[mod];

	//���ػ�����ֵ
	return (DAC_DATH_REG(dac_ptr,index) << DAC_DATL_DATA0_WIDTH)
			| DAC_DATL_REG(dac_ptr, index);
}

//==========================================================================
//��������: dac_enable_buffer
//��������: ��
//����˵��: mod:DACģ���:
//             DAC_MODx��xΪģ���;
//         buffer_mode:����������ģʽ:
//                     DAC_BUFFER_MODE_NORMAL:       ��ͨģʽ;
//                     DAC_BUFFER_MODE_SWING:        ҡ��ģʽ;
//                     DAC_BUFFER_MODE_ONE_TIME_SCAN:����ɨ��ģʽ;
//         max_index:������ָ��λ�����ޣ�ȡֵ��ΧΪ[0,15]
//         watermark:������ˮӡѡ��:
//                   DAC_WATERMARK_WORD_x��ˮӡ���뻺����ָ��λ������x���֣�
//                   x��ȡֵΪ[1,4]
//         trigger:����ת����ʽ:
//                 DAC_TRIGGER_HARDWARE:Ӳ������;
//                 DAC_TRIGGER_SOFTWARE:�������;
//���ܸ�Ҫ: ʹ��DAC��������Ĭ�Ϲرո��������ж�
//��ע: ʹ�ܻ������󣬵�ǰָ��λ�ñ�����Ϊ0;
//
//     ����ģʽΪ��ͨģʽʱ����������ǰָ��λ���ڴ����ź���ÿ�μ�1��ֱ���������ޣ�
//     ��һ�������źŵ���ʱ��ָ������Ϊ0;
//
//     ����ģʽΪҡ��ģʽʱ��ָ��λ�õ������޺󣬲�ֱ������Ϊ0�����ǿ�ʼ�ݼ���
//     һ�����˻���0�������µ���;
//
//     ����ģʽΪ����ɨ��ģʽʱ��ָ��λ�õ������޺��յ������ź�ʱ��
//     ָ��λ�ý����ٱ仯;
//
//     ��ָ��λ������Ϊ2����5�������źţ�ָ��λ�ñ仯˳��Ϊ:
//     ��ͨģʽ:    0 - 1 - 2 - 0 - 1 - 2
//     ҡ��ģʽ:    0 - 1 - 2 - 1 - 0 - 1
//     ����ɨ��ģʽ: 0 - 1 - 2 - 2 - 2 - 2
//
//     ��ˮӡѡ��ΪDAC_WATERMARK_WORD_2��������ָ��λ������Ϊ15����ˮӡλ��Ϊ13��
//     ����ָ��λ������С��ˮӡ���룬���޷�����ˮӡλ��;
//
//     ��ѡ��Ӳ������������Ҫ����PDBģ��
//==========================================================================
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

//==========================================================================
//��������: dac_disable_buffer
//��������: ��
//����˵��: mod:DACģ���:
//             DAC_MODx��xΪģ���;
//���ܸ�Ҫ: �ر�DAC������
//��ע: �رջ������󣬵�ǰָ��λ�ñ�����Ϊ0;���������жϲ��ᱻ�ر�
//==========================================================================
void dac_disable_buffer(uint8 mod) {
	//�ر�DAC������
	REG_CLR_MASK(DAC_C1_REG(dac_table[mod]), DAC_C1_DACBFEN_MASK);
}

//==========================================================================
//��������: dac_software_trigger
//��������: ��
//����˵��: mod:DACģ���:
//             DAC_MODx��xΪģ���;
//���ܸ�Ҫ: ����һ���������
//��ע: δʹ�ܻ�����ʱ����������ǰָ��λ�ò���仯;
//     ʹ�ܻ�����ʱ����������ǰָ��λ�ý����չ���ģʽ������Ӧ�ı仯��
//     ������仯�󻺳���λ�õ�ֵ
//==========================================================================
void dac_software_trigger(uint8 mod) {
	//����һ���������
	REG_SET_MASK(DAC_C0_REG(dac_table[mod]), DAC_C0_DACSWTRG_MASK);
}

//==========================================================================
//��������: dac_set_buffer_index
//��������: ��
//����˵��: mod:DACģ���:
//             DAC_MODx��xΪģ���;
//         index:��������ǰָ��λ�ã�ȡֵ��ΧΪ[0,15]
//���ܸ�Ҫ: ���û�������ǰָ��λ��
//��ע: δʹ�ܻ�����ʱ�����õ�ǰָ��λ����Ч;
//     ʹ�ܻ�����ʱ����ǰָ��λ�ÿ��Գ�������������;
//     ������仯�󻺳���λ�õ�ֵ
//==========================================================================
void dac_set_buffer_index(uint8 mod, uint8 index) {
	DAC_Type * dac_ptr;	//DAC����ַ

	//��ȡDAC����ַ
	dac_ptr = dac_table[mod];

	//���û�������ǰָ��λ�ã����ַ�ʽ����������ָ��λ��Ϊ0
	REG_SET_VAL(DAC_C2_REG(dac_ptr),
			REG_GET_MASK(DAC_C2_REG(dac_ptr),DAC_C2_DACBFUP_MASK)|DAC_C2_DACBFRP(index));
}

//==========================================================================
//��������: dac_get_buffer_index
//��������: ��������ǰָ��λ�ã�ȡֵ��ΧΪ[0,15]
//����˵��: mod:DACģ���:
//             DAC_MODx��xΪģ���;
//���ܸ�Ҫ: ��ȡ��������ǰָ��λ��
//==========================================================================
uint8 dac_get_buffer_index(uint8 mod) {
	//��ȡ��������ǰָ��λ��
	return REG_GET_MASK(DAC_C2_REG(dac_table[mod]), DAC_C2_DACBFRP_MASK)
			>> DAC_C2_DACBFRP_SHIFT;
}

//==========================================================================
//��������: dac_set_buffer_int
//��������: ��
//����˵��: mod:DACģ���:
//             DAC_MODx��xΪģ���;
//         index_bottom_int:�Ƿ�ʹ��ָ��λ�õײ��ж�:
//                          true: ʹ���ж�;
//                          false:�ر��ж�;
//         index_top_int:�Ƿ�ʹ��ָ��λ�ö����ж�:
//                       true: ʹ���ж�;
//                       false:�ر��ж�;
//         watermark_int:�Ƿ�ʹ��ˮӡ�ж�:
//                       true: ʹ���ж�;
//                       false:�ر��ж�;
//���ܸ�Ҫ: ʹ�ܻ�رո��������ж�
//��ע: ��������ǰָ��λ��Ϊָ��λ������ʱ��������ָ��λ�õײ��ж�;
//     Ϊ0ʱ��������ָ��λ�ö����ж�;Ϊˮӡλ��ʱ��������ˮӡ�ж�;
//
//     ����ʹ���жϺ���ָ��λ�÷����仯���Żᷢ����Ӧ�ж�;
//
//     �����ж�δ��ʹ�ܣ�ָ��λ�õ����ж�λ�ã��жϱ�־�ᱻ��λ�������ᷢ���ж�
//==========================================================================
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

//==========================================================================
//��������: dac_get_index_bottom_int
//��������: true:�����ж�; false:δ�����ж�
//����˵��: mod:DACģ���:
//             DAC_MODx��xΪģ���;
//���ܸ�Ҫ: ��ȡָ��λ�õײ��жϵ��жϱ�־
//==========================================================================
bool dac_get_index_bottom_int(uint8 mod) {
	//��ȡ�жϱ�־
	return REG_GET_MASK(DAC_SR_REG(dac_table[mod]), DAC_SR_DACBFRPBF_MASK) ?
			true : false;
}

//==========================================================================
//��������: dac_clear_index_bottom_int
//��������: ��
//����˵��: mod:DACģ���:
//             DAC_MODx��xΪģ���;
//���ܸ�Ҫ: ���ָ��λ�õײ��жϵ��жϱ�־
//==========================================================================
void dac_clear_index_bottom_int(uint8 mod) {
	//����жϱ�־
	REG_CLR_MASK(DAC_SR_REG(dac_table[mod]), DAC_SR_DACBFRPBF_MASK);
}

//==========================================================================
//��������: dac_get_index_top_int
//��������: true:�����ж�; false:δ�����ж�
//����˵��: mod:DACģ���:
//             DAC_MODx��xΪģ���;
//���ܸ�Ҫ: ��ȡָ��λ�ö����жϵ��жϱ�־
//==========================================================================
bool dac_get_index_top_int(uint8 mod) {
	//��ȡ�жϱ�־
	return REG_GET_MASK(DAC_SR_REG(dac_table[mod]), DAC_SR_DACBFRPTF_MASK) ?
			true : false;
}

//==========================================================================
//��������: dac_clear_index_top_int
//��������: ��
//����˵��: mod:DACģ���:
//             DAC_MODx��xΪģ���;
//���ܸ�Ҫ: ���ָ��λ�ö����жϵ��жϱ�־
//==========================================================================
void dac_clear_index_top_int(uint8 mod) {
	//����жϱ�־
	REG_CLR_MASK(DAC_SR_REG(dac_table[mod]), DAC_SR_DACBFRPTF_MASK);
}

//==========================================================================
//��������: dac_get_watermark_int
//��������: true:�����ж�; false:δ�����ж�
//����˵��: mod:DACģ���:
//             DAC_MODx��xΪģ���;
//���ܸ�Ҫ: ��ȡˮӡ�жϵ��жϱ�־
//==========================================================================
bool dac_get_watermark_int(uint8 mod) {
	//��ȡ�жϱ�־
	return REG_GET_MASK(DAC_SR_REG(dac_table[mod]), DAC_SR_DACBFWMF_MASK) ?
			true : false;
}

//==========================================================================
//��������: dac_clear_watermark_int
//��������: ��
//����˵��: mod:DACģ���:
//             DAC_MODx��xΪģ���;
//���ܸ�Ҫ: ���ˮӡ�жϵ��жϱ�־
//==========================================================================
void dac_clear_watermark_int(uint8 mod) {
	//����жϱ�־
	REG_CLR_MASK(DAC_SR_REG(dac_table[mod]), DAC_SR_DACBFWMF_MASK);
}
