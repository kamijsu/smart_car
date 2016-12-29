//==========================================================================
//�ļ����ƣ�adc.h
//���ܸ�Ҫ��K64 ADC�ײ���������ͷ�ļ�
//==========================================================================

#ifndef _ADC_H
#define _ADC_H

//��������ͷ�ļ�
#include "common.h"

//����ADCģ���
#define ADC_MOD0	(0)
#define ADC_MOD1	(1)

//����ADC�������ͨ����ţ�����ʹ�õ�K64��װ����Ϊ100����LQFP����Щ���ź�ʹ�����ֱ�ʾ������Щ�÷�װ�޸�ͨ������??��ʾ
#define ADC_DIFF_GROUP0		(0)		//ADC0_DP0:18    ADC0_DM0:19    ADC1_DP0:20    ADC1_DM0:21
#define ADC_DIFF_GROUP1		(1)		//ADC0_DP1:14    ADC0_DM1:15    ADC1_DP1:16    ADC1_DM1:17
#define ADC_DIFF_GROUP2		(2)		//ADC0_DP2:E2    ADC0_DM2:E3
#define ADC_DIFF_GROUP3		(3)		//ADC0_DP3:20    ADC0_DM3:21    ADC1_DP3:18    ADC1_DM3:19
#define ADC_DIFF_GROUP_TEMP	(26)	//�����¶ȴ�������������

//����ADC��������ͨ���ţ����ź�˵��ͬ��
#define ADC_SE4				(4)		//ADC0_SE4b:C2   ADC1_SE4a:E0   ADC1_SE4b:C8
#define ADC_SE5				(5)		//ADC0_SE5b:D1   ADC1_SE5a:E1   ADC1_SE5b:C9
#define ADC_SE6				(6)		//ADC0_SE6b:D5   ADC1_SE6a:E2   ADC1_SE6b:C10
#define ADC_SE7				(7)		//ADC0_SE7b:D6   ADC1_SE7a:E3   ADC1_SE7b:C11
#define ADC_SE8				(8)		//ADC0_SE8: B0   ADC1_SE8: B0
#define ADC_SE9				(9)		//ADC0_SE9: B1   ADC1_SE9: B1
#define ADC_SE10			(10)	//ADC0_SE10:A7   ADC1_SE10:B4
#define ADC_SE11			(11)	//ADC0_SE11:A8   ADC1_SE11:B5
#define ADC_SE12			(12)	//ADC0_SE12:B2   ADC1_SE12:B6
#define ADC_SE13			(13)	//ADC0_SE13:B3   ADC1_SE13:B7
#define ADC_SE14			(14)	//ADC0_SE14:C0   ADC1_SE14:B10
#define ADC_SE15			(15)	//ADC0_SE15:C1   ADC1_SE15:B11
#define ADC_SE16			(16)	//ADC0_SE16:??   ADC1_SE16:??
#define ADC_SE17			(17)	//ADC0_SE17:E24  ADC1_SE17:A17
#define ADC_SE18			(18)	//ADC0_SE18:E25  ADC1_SE18:26
#define ADC_SE21			(21)	//ADC0_SE21:??
#define ADC_SE22			(22)	//ADC0_SE22:??
#define ADC_SE23			(23)	//ADC0_SE23:27   ADC1_SE23:??
#define ADC_SE_TEMP			(26)	//�����¶ȴ�������������

//����ADC��������ͨ��ѡ��
#define ADC_SE_SEL_A		(0)		//ѡ��Aͨ��
#define ADC_SE_SEL_B		(1)		//ѡ��Bͨ��

//����ADC�ο���ѹ����λmV������Ĭ��ѡ��VREFH��VREFL(VTGΪvoltage��д)
#define ADC_VTG		(3300.0f)

//����ADCģ��ʱ�ӷ�Ƶ��������ѡ������ʱ��
#define ADC_CLK_DIV_1		(0)		//����ʱ��1��Ƶ
#define ADC_CLK_DIV_2		(1)		//����ʱ��2��Ƶ
#define ADC_CLK_DIV_4		(2)		//����ʱ��4��Ƶ
#define ADC_CLK_DIV_8		(3)		//����ʱ��8��Ƶ
#define ADC_CLK_DIV_16		(4)		//����ʱ��16��Ƶ(ѡ��16��Ƶʱ������ʱ��Ϊ����ʱ��/2)

//����ADC�������ȣ�����Ĭ��ѡ�������������
#define ADC_ACCURACY_SINGLE_8_DIFF_9	(0)		//����8λת�������9λת��
#define ADC_ACCURACY_SINGLE_10_DIFF_11	(2)		//����10λת�������11λת��
#define ADC_ACCURACY_SINGLE_12_DIFF_13	(1)		//����12λת�������13λת��
#define ADC_ACCURACY_SINGLE_DIFF_16		(3)		//���˲�־�16λת��

//����ADCӲ��������ֵ����(��ʹ��Ӳ�������ֵ����Ĭ��ʹ�ܳ���ת��)
#define ADC_HARDWARE_AVG_4			(0)		//4��������ֵ
#define ADC_HARDWARE_AVG_8			(1)		//8��������ֵ
#define ADC_HARDWARE_AVG_16			(2)		//16��������ֵ
#define ADC_HARDWARE_AVG_32			(3)		//32��������ֵ
#define ADC_HARDWARE_AVG_DISABLE	(4)		//��ֹӲ�������ֵ

//����ADC������ʱ��
#define ADC_ADLSTS_20		(0)		//����20��ADCK����
#define ADC_ADLSTS_12		(1)		//����12��ADCK����
#define ADC_ADLSTS_6		(2)		//����6��ADCK����
#define ADC_ADLSTS_2		(3)		//����2��ADCK����
#define ADC_ADLSTS_DISABLE	(4)		//���ó�ʱ������������ö�ʱ�����

//����ADC��������
#define ADC_ADHSC_NORMAL		(0)	//����ת��ʱ��
#define ADC_ADHSC_HIGH_SPEED	(1)	//����ת��ʱ��

//�����Ƿ�ʹ��ADCУ�Թ���
#define ADC_CAL_ENABLE			(1)	//ʹ��ADCУ�Թ���
#define ADC_CAL_DISABLE			(0)	//��ֹADCУ�Թ���

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
		uint8 adlsts, uint8 adhsc, uint8 cal);

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
uint16 adc_single_get_ad(uint8 mod, uint8 se_ch, uint8 se_sel);

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
float adc_single_get_vtg(uint8 mod, uint8 se_ch, uint8 se_sel);

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
int16 adc_diff_get_ad(uint8 mod, uint8 diff_group);

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
float adc_diff_get_vtg(uint8 mod, uint8 diff_group);

#endif
