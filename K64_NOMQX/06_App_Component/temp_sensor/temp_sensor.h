//==========================================================================
//�ļ����ƣ�temp_sensor.h
//���ܸ�Ҫ���¶ȴ�����Ӧ����������ͷ�ļ�
//==========================================================================

#ifndef _TEMP_SENSOR_H
#define _TEMP_SENSOR_H

#include "adc.h"

//�����¶ȴ�������ʹ�õ�ADCģ�����ã���ѡ���ü�adc.h(ע��:����ͬADCģ�鱻���ڲ�ͬӦ�ù������뱣֤���ǵ�������ͬ)
//������ʹ�õ�ADCģ��
#define TEMP_SENSOR_ADC_MOD				ADC_MOD0						//ADC0
//����ADCģ��ʱ�ӷ�Ƶ��
#define TEMP_SENSOR_ADC_CLK_DIV			ADC_CLK_DIV_4					//����ʱ��4��Ƶ
//����ADC��������
#define TEMP_SENSOR_ADC_ACCURACY		ADC_ACCURACY_SINGLE_DIFF_16		//���16λת��
//����ADCӲ��������ֵ����
#define TEMP_SENSOR_ADC_HARDWARE_AVG	ADC_HARDWARE_AVG_8				//8��������ֵ
//����ADC������ʱ��
#define TEMP_SENSOR_ADC_ADLSTS			ADC_ADLSTS_12					//����12��ADCK����
//����ADC��������
#define TEMP_SENSOR_ADC_ADHSC			ADC_ADHSC_NORMAL				//����ת��ʱ��
//�����Ƿ�ʹ��ADCУ�Թ���
#define TEMP_SENSOR_ADC_CAL				ADC_CAL_ENABLE					//ʹ��ADCУ�Թ���
//����ADC�ο���ѹ����λmV
#define TEMP_SENSOR_ADC_VTG				ADC_VTG							//3300mV
//������ʹ�õ�ADC���ͨ����(����ʹ�ò�������ȡADCת�����)
#define TEMP_SENSOR_ADC_DIFF_GROUP		ADC_DIFF_GROUP_TEMP

//�����¶ȴ�����Ӳ���������¶� = 25 - ((VTEMP - VTEMP25) / m)
//�¶ȴ�����25��Cʱ�ĵ�ѹֵ����λmV
#define TEMP_SENSOR_VTEMP25				(716)
//�¶ȴ���������ϵ������λmV/��C
#define TEMP_SENSOR_M					(1.62f)

//���ݶ����ADC�������ȶ�������λ����
#ifdef TEMP_SENSOR_ADC_ACCURACY
#if(TEMP_SENSOR_ADC_ACCURACY == ADC_ACCURACY_SINGLE_8_DIFF_9)
#define TEMP_SENSOR_ADC_DATA_BITS	(8)
#elif(TEMP_SENSOR_ADC_ACCURACY == ADC_ACCURACY_SINGLE_10_DIFF_11)
#define TEMP_SENSOR_ADC_DATA_BITS	(10)
#elif(TEMP_SENSOR_ADC_ACCURACY == ADC_ACCURACY_SINGLE_12_DIFF_13)
#define TEMP_SENSOR_ADC_DATA_BITS	(12)
#elif(TEMP_SENSOR_ADC_ACCURACY == ADC_ACCURACY_SINGLE_DIFF_16)
#define TEMP_SENSOR_ADC_DATA_BITS	(15)
#endif
#endif

//==========================================================================
//��������: temp_sensor_init
//��������: true: ��ʹ��У�Թ��ܣ���У�Թ������óɹ������߽�ֹУ�Թ���;
//         false:��ʹ��У�Թ��ܣ���У�Թ�������ʧ��;
//����˵��: ��
//���ܸ�Ҫ: ��ʼ���¶ȴ���������Ӧ������temp_sensor.h��
//��ע: ������ʱ��Ƶ��Ϊ48MHz��ʹ������ʱ��4��Ƶ�����16λת����Ӳ������8��������ֵ��
//     ������ʱ�����12��ADCK���ڣ�����ת��ʱ�������ʱ��ԼΪ31.02��s
//==========================================================================
bool temp_sensor_init();

//==========================================================================
//��������: temp_sensor_get
//��������: �¶ȴ����������¶ȣ���λ��C
//����˵��: ��
//���ܸ�Ҫ: ��ȡ�¶ȴ����������¶ȣ���λ��C
//��ע: ��ʹ�ò��9λת��ʱ���ֱ���ԼΪ7.99��C�����¶ȱ仯Լ7.99��Cʱ���ſ��Լ�����;
//     ��ʹ�ò��11λת��ʱ���ֱ���ԼΪ1.99��C;
//     ��ʹ�ò��13λת��ʱ���ֱ���ԼΪ0.50��C;
//     ��ʹ�ò��16λת��ʱ���ֱ���ԼΪ0.06��C;
//==========================================================================
float temp_sensor_get();

#endif
