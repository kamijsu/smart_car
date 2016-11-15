//==========================================================================
//�ļ����ƣ�adc.c
//���ܸ�Ҫ��K64 ADC�ײ���������Դ�ļ�
//==========================================================================

#include "adc.h"

//============================================================================
//�������ƣ�adc_init
//�������أ�0 �ɹ� ��1 ʧ��
//����˵����MoudelNumber��ģ���
//���ܸ�Ҫ��AD��ʼ��
//============================================================================
uint8_t adc_init(int MoudelNumber) {
	if (MoudelNumber == 0) //ģ��0
			{
		//��ADC0ģ��ʱ��
		SIM_SCGC6 |= (SIM_SCGC6_ADC0_MASK);
	} else if (MoudelNumber == 1) //ģ��1
			{
		//��ADC1ģ��ʱ��
		SIM_SCGC3 |= (SIM_SCGC3_ADC1_MASK);
	} else {
		return 0;
	}

	return 1;
}

//============================================================================
//�������ƣ�ad_once
//�������أ�16λ�޷��ŵ�ADֵ 
//����˵����MoudelNumber��ģ���
//               Channel��ͨ����
//              accuracy������
//���ܸ�Ҫ���ɼ�һ��һ·ģ������ADֵ    
//============================================================================
uint16_t adc_once(int MoudelNumber, int Channel, uint8_t accuracy) //�ɼ�ĳ·ģ������ADֵ
{
	uint16_t result = 0;
	uint8_t ADCCfg1Mode = 0;
	ADC_MemMapPtr ADCMoudel; //����ADCģ���ַָ��

	switch (accuracy) {
	case 8:
		ADCCfg1Mode = 0x00;
		break;
	case 12:
		ADCCfg1Mode = 0x01;
		break;
	case 10:
		ADCCfg1Mode = 0x02;
		break;
	case 16:
		ADCCfg1Mode = 0x03;
		break;
	default:
		ADCCfg1Mode = 0x00;
	}

	if (MoudelNumber == 0) //ѡ��ADCģ��0
			{
		ADCMoudel = ADC0_BASE_PTR;
	} else               //ѡ��ADCģ��1
	{
		ADCMoudel = ADC1_BASE_PTR;
	}

	//����������Դģʽ������ʱ�ӣ�����ʱ��4��Ƶ��������ʱ��ʹ�ܣ����þ���
	ADC_CFG1_REG(ADCMoudel) = ADLPC_NORMAL | ADC_CFG1_ADIV(ADIV_4) | ADLSMP_LONG
	| ADC_CFG1_MODE(ADCCfg1Mode)
	| ADC_CFG1_ADICLK(ADICLK_BUS);

	//���ý�ֹ�첽ʱ��ʹ�������ADxxatͨ��ѡ�񣬸������ã�������ʱ��
	ADC_CFG2_REG(ADCMoudel) = MUXSEL_ADCA | ADACKEN_DISABLED | ADHSC_HISPEED
			| ADC_CFG2_ADLSTS(ADLSTS_20);

	//����ͨ����
	ADC_SC1_REG(ADCMoudel,A) = AIEN_ON | DIFF_SINGLE | ADC_SC1_ADCH(Channel);
	//�ȴ�ת�����
	while (( ADC_SC1_REG(ADCMoudel,A) & ADC_SC1_COCO_MASK) != ADC_SC1_COCO_MASK) {

	}
	//��ȡת�����
	result = ADC_R_REG(ADCMoudel, A);
	//��ADCת����ɱ�־
	ADC_SC1_REG(ADCMoudel,A) &= ~ADC_SC1_COCO_MASK;

	return result;
}

//============================================================================
//�������ƣ�ad_mid
//�������أ�16λ�޷��ŵ�ADֵ 
//����˵����MoudelNumber��ģ���
//               Channel��ͨ����
//              accuracy������
//���ܸ�Ҫ����ֵ�˲���Ľ��(��Χ:0-4095) 
//============================================================================
uint16_t adc_mid(int MoudelNumber, int Channel, uint8_t accuracy) {
	uint16_t i, j, k, tmp;
	//1.ȡ3��A/Dת�����
	i = adc_once(MoudelNumber, Channel, accuracy);
	j = adc_once(MoudelNumber, Channel, accuracy);
	k = adc_once(MoudelNumber, Channel, accuracy);
	//2.ȡ��ֵ
	if (i > j) {
		tmp = i;
		i = j;
		j = tmp;
	}
	if (k > j)
		tmp = j;
	else if (k > i)
		tmp = k;
	else
		tmp = i;
	return tmp;
}

//============================================================================
//�������ƣ�ad_ave
//�������أ�16λ�޷��ŵ�ADֵ 
//����˵����MoudelNumber��ģ���
//               Channel��ͨ����
//              accuracy������
//                     N:��ֵ�˲�����(��Χ:0~255)
//���ܸ�Ҫ����ֵ�˲���Ľ��(��Χ:0-4095) 
//============================================================================
uint16_t adc_ave(int MoudelNumber, int Channel, uint8_t accuracy, uint8_t N) {
	uint32_t tmp = 0;
	uint8_t i;
	for (i = 0; i < N; i++)
		tmp += adc_mid(MoudelNumber, Channel, accuracy);
	tmp = tmp / N;
	return (uint16_t) tmp;
}
