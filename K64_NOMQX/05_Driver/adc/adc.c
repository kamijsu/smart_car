//==========================================================================
//�ļ����ƣ�adc.c
//���ܸ�Ҫ��K64 ADC�ײ���������Դ�ļ�
//==========================================================================

#include "adc.h"

//ADC��ģ�����ַ
static ADC_Type * const adc_table[] = ADC_BASE_PTRS;

//==========================================================================
//��������: adc_cal
//��������: true: У�Թ������óɹ�;
//         false:У�Թ�������ʧ��;
//����˵��: mod:ADCģ���:
//             ADC_MODx��xΪģ���;
//���ܸ�Ҫ: У��ADCģ��
//==========================================================================
static bool adc_cal(uint8 mod) {
	ADC_Type * adc_ptr;	//ADC����ַ
	uint16 cal_var;	//У��ֵ��ʱ����

	//��ȡADC����ַ
	adc_ptr = adc_table[mod];

	//�����������ã�ѡ������ʱ��8��Ƶ����ʱ�����������ʱ��Ϊ����ʱ��/2
	REG_SET_VAL(ADC_CFG1_REG(adc_ptr),
			0|ADC_CFG1_ADIV(ADC_CLK_DIV_8)|ADC_CFG1_ADLSMP_MASK|ADC_CFG1_ADICLK(1));
	//����ת������ʱ�����20������ADCK����
	REG_SET_VAL(ADC_CFG2_REG(adc_ptr), 0|ADC_CFG2_ADHSC_MASK);
	//ѡ����������������òο���ѹΪVREFH��VREFL
	REG_CLR_MASK(ADC_SC2_REG(adc_ptr), ADC_SC2_ADTRG_MASK|ADC_SC2_REFSEL_MASK);
	//���õ���ת�������������ֵ��
	REG_CLR_MASK(ADC_SC3_REG(adc_ptr), ADC_SC3_ADCO_MASK|ADC_SC3_AVGS_MASK);
	//ʹ��Ӳ�������ֵ�������ò�����ֵ��Ϊ32
	REG_SET_MASK(ADC_SC3_REG(adc_ptr),
			ADC_SC3_AVGE_MASK|ADC_SC3_AVGS(ADC_HARDWARE_AVG_32));
	//��ʼУ��
	REG_SET_MASK(ADC_SC3_REG(adc_ptr), ADC_SC3_CAL_MASK);
	//�ȴ�У�����
	while (!REG_GET_MASK(ADC_SC1_REG(adc_ptr,0), ADC_SC1_COCO_MASK)) {
	}
	//У��ʧ��
	if (REG_GET_MASK(ADC_SC3_REG(adc_ptr), ADC_SC3_CALF_MASK)) {
		return false;
	}
	//������������Ĵ���
	cal_var = ADC_CLP0_REG(adc_ptr);
	cal_var += ADC_CLP1_REG(adc_ptr);
	cal_var += ADC_CLP2_REG(adc_ptr);
	cal_var += ADC_CLP3_REG(adc_ptr);
	cal_var += ADC_CLP4_REG(adc_ptr);
	cal_var += ADC_CLPS_REG(adc_ptr);
	cal_var = 0x8000 | (cal_var >> 1);
	REG_SET_VAL(ADC_PG_REG(adc_ptr), cal_var);
	//���ø�������Ĵ���
	cal_var = ADC_CLM0_REG(adc_ptr);
	cal_var += ADC_CLM1_REG(adc_ptr);
	cal_var += ADC_CLM2_REG(adc_ptr);
	cal_var += ADC_CLM3_REG(adc_ptr);
	cal_var += ADC_CLM4_REG(adc_ptr);
	cal_var += ADC_CLMS_REG(adc_ptr);
	cal_var = 0x8000 | (cal_var >> 1);
	REG_SET_VAL(ADC_MG_REG(adc_ptr), cal_var);
	//���У�Ա�־
	REG_CLR_MASK(ADC_SC3_REG(adc_ptr), ADC_SC3_CAL_MASK);
	return true;
}

//==========================================================================
//��������: adc_init
//��������: true: ��ʹ��У�Թ��ܣ���У�Թ������óɹ������߽�ֹУ�Թ���;
//         false:��ʹ��У�Թ��ܣ���У�Թ�������ʧ��;
//����˵��: mod:ADCģ���:
//             ADC_MODx��xΪģ���;
//         clk_div:ʱ�ӷ�Ƶ��:
//                 ADC_CLK_DIV_x��xΪ��Ƶ����С;
//         accuracy:ADC��������:
//                  ADC_ACCURACY_SINGLE_8_DIFF_9:  ����8λת�������9λת��;
//                  ADC_ACCURACY_SINGLE_10_DIFF_11:����10λת�������11λת��;
//                  ADC_ACCURACY_SINGLE_12_DIFF_13:����12λת�������13λת��;
//                  ADC_ACCURACY_SINGLE_DIFF_16:   ���˲�־�16λת��;
//         hardware_avg:Ӳ��������ֵ����:
//                      ADC_HARDWARE_AVG_4:      4��������ֵ;
//                      ADC_HARDWARE_AVG_8:      8��������ֵ;
//                      ADC_HARDWARE_AVG_16:     16��������ֵ;
//                      ADC_HARDWARE_AVG_32:     32��������ֵ;
//                      ADC_HARDWARE_AVG_DISABLE:��ֹӲ�������ֵ;
//         adlsts:������ʱ��:
//                ADC_ADLSTS_20:     ����20��ADCK����;
//                ADC_ADLSTS_12:     ����12��ADCK����;
//                ADC_ADLSTS_6:      ����6��ADCK����;
//                ADC_ADLSTS_2:      ����2��ADCK����;
//                ADC_ADLSTS_DISABLE:���ó�ʱ������������ö�ʱ�����;
//         adhsc:��������:
//               ADC_ADHSC_NORMAL:    ����ת��ʱ��;
//               ADC_ADHSC_HIGH_SPEED:����ת��ʱ��;
//         cal:�Ƿ�ʹ��ADCУ�Թ���:
//             ADC_CAL_ENABLE: ʹ��ADCУ�Թ���;
//             ADC_CAL_DISABLE:��ֹADCУ�Թ���;
//���ܸ�Ҫ: ��ʼ��ADCģ��
//��ע: ����ʱ�� = SFCAdder + AverageNum * (BCT + LSTAdder + HSCAdder);
//     SFCAdder:��� adlsts == ADC_ADLSTS_DISABLE��
//              �� SFCAdder = 5��ADCK���� + 5������ʱ�����ڣ�
//              ���� SFCAdder = 3��ADCK���� + 5������ʱ������;
//     AverageNum:ADC_HARDWARE_AVG_DISABLE:AverageNum = 1;
//                ADC_HARDWARE_AVG_4:      AverageNum = 4;
//                ADC_HARDWARE_AVG_8:      AverageNum = 8;
//                ADC_HARDWARE_AVG_16:     AverageNum = 16;
//                ADC_HARDWARE_AVG_32:     AverageNum = 32;
//     BCT:����8λת��: BCT = 17��ADCK����; ���9λת��: BCT = 27��ADCK����;
//         ����10λת��:BCT = 20��ADCK����; ���11λת��:BCT = 30��ADCK����;
//         ����12λת��:BCT = 20��ADCK����; ���13λת��:BCT = 30��ADCK����;
//         ����16λת��:BCT = 25��ADCK����; ���16λת��:BCT = 34��ADCK����;
//     LSTAdder:ADC_ADLSTS_DISABLE:LSTAdder = 0 ��ADCK����;
//              ADC_ADLSTS_20:     LSTAdder = 20��ADCK����;
//              ADC_ADLSTS_12:     LSTAdder = 12��ADCK����;
//              ADC_ADLSTS_6:      LSTAdder = 6 ��ADCK����;
//              ADC_ADLSTS_2:      LSTAdder = 2 ��ADCK����;
//     HSCAdder:ADC_ADHSC_NORMAL:    HSCAdder = 0��ADCK����;
//              ADC_ADHSC_HIGH_SPEED:HSCAdder = 2��ADCK����;
//     ���У�ADCK����Ϊ����ʱ�ӷ�Ƶ���ʱ�����ڣ����������ʱ��Ƶ��Ϊ48MHz
//==========================================================================
bool adc_init(uint8 mod, uint8 clk_div, uint8 accuracy, uint8 hardware_avg,
		uint8 adlsts, uint8 adhsc, uint8 cal) {
	ADC_Type * adc_ptr;	//ADC����ַ
	bool result;	//У�Խ��

	//��ȡADC����ַ
	adc_ptr = adc_table[mod];
	result = true;	//Ĭ��У�Գɹ�

	//������ӦADCģ��ʱ����
	if (mod == ADC_MOD0) {
		REG_SET_MASK(SIM_SCGC6, SIM_SCGC6_ADC0_MASK);
	} else {
		REG_SET_MASK(SIM_SCGC3, SIM_SCGC3_ADC1_MASK);
	}
	//����PG��MG��ֵ
	REG_SET_VAL(ADC_PG_REG(adc_ptr), 0x8200);
	REG_SET_VAL(ADC_MG_REG(adc_ptr), 0x8200);
	//У��ADCģ��
	if (cal == ADC_CAL_ENABLE) {
		result = adc_cal(mod);
	}
	//��������ģʽ���������ʱ�ӷ�Ƶѡ�񡢲���ʱ������(��ѡ���ʱ�����)��ת��ģʽѡ������ʱ��ѡ��(��ѡ������ʱ��)
	REG_SET_VAL(ADC_CFG1_REG(adc_ptr), 0);
	//����ʱ��Ƶ��
	if (clk_div == ADC_CLK_DIV_16) {
		//����ʱ��8��Ƶ��������ʱ��Ϊ����ʱ��/2
		REG_SET_MASK(ADC_CFG1_REG(adc_ptr),
				ADC_CFG1_ADIV(ADC_CLK_DIV_8)|ADC_CFG1_ADICLK(1));
	} else {
		//���÷�Ƶ��
		REG_SET_MASK(ADC_CFG1_REG(adc_ptr), ADC_CFG1_ADIV(clk_div));
	}
	//���ò�������
	REG_SET_MASK(ADC_CFG1_REG(adc_ptr), ADC_CFG1_MODE(accuracy));
	//���ó�ʱ�����
	if (adlsts != ADC_ADLSTS_DISABLE) {
		REG_SET_MASK(ADC_CFG1_REG(adc_ptr), ADC_CFG1_ADLSMP_MASK);
		REG_CLR_MASK(ADC_CFG2_REG(adc_ptr), ADC_CFG2_ADLSTS_MASK);
		REG_SET_MASK(ADC_CFG2_REG(adc_ptr), ADC_CFG2_ADLSTS(adlsts));
	}
	//���ø�������
	if (adhsc == ADC_ADHSC_NORMAL) {
		REG_CLR_MASK(ADC_CFG2_REG(adc_ptr), ADC_CFG2_ADHSC_MASK);
	} else {
		REG_SET_MASK(ADC_CFG2_REG(adc_ptr), ADC_CFG2_ADHSC_MASK);
	}
	//ѡ����������������òο���ѹΪVREFH��VREFL
	REG_CLR_MASK(ADC_SC2_REG(adc_ptr), ADC_SC2_ADTRG_MASK|ADC_SC2_REFSEL_MASK);
	//����Ӳ�������ֵ
	if (hardware_avg == ADC_HARDWARE_AVG_DISABLE) {
		//��ֹӲ�������ֵ
		REG_CLR_MASK(ADC_SC3_REG(adc_ptr), ADC_SC3_AVGE_MASK);
	} else {
		//����Ӳ�������ֵ��
		REG_CLR_MASK(ADC_SC3_REG(adc_ptr), ADC_SC3_AVGS_MASK);
		//ʹ��Ӳ�������ֵ��ѡ������ֵ����ʹ�ܳ���ת��
		REG_SET_MASK(ADC_SC3_REG(adc_ptr),
				ADC_SC3_AVGE_MASK|ADC_SC3_ADCO_MASK|ADC_SC3_AVGS(hardware_avg));
	}
	return result;
}

//==========================================================================
//��������: adc_single_get_ad
//��������: ����ADֵ���޷���
//����˵��: mod:ADCģ���:
//             ADC_MODx��xΪģ���;
//         se_ch:ADC��������ͨ����:
//               ADC_SEx��xΪͨ����;
//         se_sel:��������ͨ��ѡ��:
//                ADC_SE_SEL_A:ѡ��Aͨ��;
//                ADC_SE_SEL_B:ѡ��Bͨ��;
//���ܸ�Ҫ: ����ʽ�ػ�ȡ��������ͨ����ADCת�����(ADֵ)
//��ע: ��ͨ����ABͨ��ѡ����se_sel��Ч��ѡ������ͨ������;
//     δ������Ӧ���ſ��ƼĴ�����MUXֵ�������Ӧ������ʹ��ǰ���ܱ�����Ϊ��������;
//     ����ʹ��adc_single_get_vtg��Ч�ʽϸú�����
//==========================================================================
uint16 adc_single_get_ad(uint8 mod, uint8 se_ch, uint8 se_sel) {
	ADC_Type * adc_ptr;	//ADC����ַ

	//��ȡADC����ַ
	adc_ptr = adc_table[mod];

	//ѡ��ABͨ��
	if (se_sel == ADC_SE_SEL_A) {
		REG_CLR_MASK(ADC_CFG2_REG(adc_ptr), ADC_CFG2_MUXSEL_MASK);
	} else {
		REG_SET_MASK(ADC_CFG2_REG(adc_ptr), ADC_CFG2_MUXSEL_MASK);
	}
	//��������ͨ���������õ�������
	REG_SET_VAL(ADC_SC1_REG(adc_ptr,0), ADC_SC1_ADCH(se_ch));
	//�ȴ�ADCת�����
	while (!REG_GET_MASK(ADC_SC1_REG(adc_ptr,0), ADC_SC1_COCO_MASK)) {
	}
	//����ת�����
	return ADC_R_REG(adc_ptr, 0);
}

//==========================================================================
//��������: adc_single_get_vtg
//��������: ������ѹֵ����λmV
//����˵��: mod:ADCģ���:
//             ADC_MODx��xΪģ���;
//         se_ch:ADC��������ͨ����:
//               ADC_SEx��xΪͨ����;
//         se_sel:��������ͨ��ѡ��:
//                ADC_SE_SEL_A:ѡ��Aͨ��;
//                ADC_SE_SEL_B:ѡ��Bͨ��;
//���ܸ�Ҫ: ����ʽ�ػ�ȡ��������ͨ����ADCת���������ת��Ϊ��ѹֵ����λmV
//��ע: ��ͨ����ABͨ��ѡ����se_sel��Ч��ѡ������ͨ������;
//     δ������Ӧ���ſ��ƼĴ�����MUXֵ�������Ӧ������ʹ��ǰ���ܱ�����Ϊ��������
//==========================================================================
float adc_single_get_vtg(uint8 mod, uint8 se_ch, uint8 se_sel) {
	ADC_Type * adc_ptr;	//ADC����ַ
	uint16 ad_val;		//����ADֵ���޷���
	float radio;		//ADֵת��ѹʱ�ı���ϵ��
	uint8 mode;			//ת��ģʽ

	//��ȡADC����ַ
	adc_ptr = adc_table[mod];

	//ѡ��ABͨ��
	if (se_sel == ADC_SE_SEL_A) {
		REG_CLR_MASK(ADC_CFG2_REG(adc_ptr), ADC_CFG2_MUXSEL_MASK);
	} else {
		REG_SET_MASK(ADC_CFG2_REG(adc_ptr), ADC_CFG2_MUXSEL_MASK);
	}
	//��������ͨ���������õ�������
	REG_SET_VAL(ADC_SC1_REG(adc_ptr,0), ADC_SC1_ADCH(se_ch));
	//�ȴ�ת������ڼ�������ϵ��
	//��ȡת��ģʽ
	mode = REG_GET_MASK(ADC_CFG1_REG(adc_ptr),
			ADC_CFG1_MODE_MASK) >> ADC_CFG1_MODE_SHIFT;
	//����ת��ģʽ�������ϵ��������Ϊ((1<<����λ��)-1)
	switch (mode) {
	case ADC_ACCURACY_SINGLE_8_DIFF_9:
		radio = ADC_VTG / 255.0f;
		break;
	case ADC_ACCURACY_SINGLE_10_DIFF_11:
		radio = ADC_VTG / 1023.0f;
		break;
	case ADC_ACCURACY_SINGLE_12_DIFF_13:
		radio = ADC_VTG / 4095.0f;
		break;
	case ADC_ACCURACY_SINGLE_DIFF_16:
		radio = ADC_VTG / 65535.0f;
		break;
	}
	//�ȴ�ADCת�����
	while (!REG_GET_MASK(ADC_SC1_REG(adc_ptr,0), ADC_SC1_COCO_MASK)) {
	}
	//��ȡ����ADֵ
	ad_val = ADC_R_REG(adc_ptr, 0);
	//ת��Ϊ��ѹֵ��������
	return ad_val * radio;
}

//==========================================================================
//��������: adc_diff_get_ad
//��������: ����ADֵ���з���
//����˵��: mod:ADCģ���:
//             ADC_MODx��xΪģ���;
//         diff_group:ADC�������ͨ�����:
//                    ADC_DIFF_GROUPx��xΪͨ�����;
//���ܸ�Ҫ: ����ʽ�ػ�ȡ�������ͨ�����ADCת�����(ADֵ)
//��ע: δ������Ӧ���ſ��ƼĴ�����MUXֵ�������Ӧ������ʹ��ǰ���ܱ�����Ϊ��������;
//     ����ʹ��adc_diff_get_vtg��Ч�ʽϸú�����
//==========================================================================
int16 adc_diff_get_ad(uint8 mod, uint8 diff_group) {
	ADC_Type * adc_ptr;	//ADC����ַ

	//��ȡADC����ַ
	adc_ptr = adc_table[mod];

	//��������ͨ���飬�����ò������
	REG_SET_VAL(ADC_SC1_REG(adc_ptr,0),
			ADC_SC1_ADCH(diff_group) | ADC_SC1_DIFF_MASK);
	//�ȴ�ADCת�����
	while (!REG_GET_MASK(ADC_SC1_REG(adc_ptr,0), ADC_SC1_COCO_MASK)) {
	}
	//����ת�����
	return ADC_R_REG(adc_ptr, 0);
}

//==========================================================================
//��������: adc_diff_get_vtg
//��������: ������ѹֵ����λmV
//����˵��: mod:ADCģ���:
//             ADC_MODx��xΪģ���;
//         diff_group:ADC�������ͨ�����:
//                    ADC_DIFF_GROUPx��xΪͨ�����;
//���ܸ�Ҫ: ����ʽ�ػ�ȡ�������ͨ�����ADCת���������ת��Ϊ��ѹֵ����λmV
//��ע: δ������Ӧ���ſ��ƼĴ�����MUXֵ�������Ӧ������ʹ��ǰ���ܱ�����Ϊ��������
//==========================================================================
float adc_diff_get_vtg(uint8 mod, uint8 diff_group) {
	ADC_Type * adc_ptr;	//ADC����ַ
	int16 ad_val;		//����ADֵ���з���
	float radio;		//ADֵת��ѹʱ�ı���ϵ��
	uint8 mode;			//ת��ģʽ

	//��ȡADC����ַ
	adc_ptr = adc_table[mod];

	//��������ͨ���飬�����ò������
	REG_SET_VAL(ADC_SC1_REG(adc_ptr,0),
			ADC_SC1_ADCH(diff_group) | ADC_SC1_DIFF_MASK);
	//�ȴ�ת������ڼ�������ϵ��
	//��ȡת��ģʽ
	mode = REG_GET_MASK(ADC_CFG1_REG(adc_ptr),
			ADC_CFG1_MODE_MASK) >> ADC_CFG1_MODE_SHIFT;
	//����ת��ģʽ�������ϵ��������Ϊ((1<<����λ��)-1)
	switch (mode) {
	case ADC_ACCURACY_SINGLE_8_DIFF_9:
		radio = ADC_VTG / 255.0f;
		break;
	case ADC_ACCURACY_SINGLE_10_DIFF_11:
		radio = ADC_VTG / 1023.0f;
		break;
	case ADC_ACCURACY_SINGLE_12_DIFF_13:
		radio = ADC_VTG / 4095.0f;
		break;
	case ADC_ACCURACY_SINGLE_DIFF_16:
		radio = ADC_VTG / 32767.0f;
		break;
	}
	//�ȴ�ADCת�����
	while (!REG_GET_MASK(ADC_SC1_REG(adc_ptr,0), ADC_SC1_COCO_MASK)) {
	}
	//��ȡ����ADֵ
	ad_val = ADC_R_REG(adc_ptr, 0);
	//ת��Ϊ��ѹֵ��������
	return ad_val * radio;
}
