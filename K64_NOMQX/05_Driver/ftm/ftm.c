//==========================================================================
//�ļ����ƣ�ftm.c
//���ܸ�Ҫ��K64 FTM�ײ���������Դ�ļ�
//==========================================================================

#include "ftm.h"

//FTM��ģ�����ַ
static FTM_Type * const ftm_table[] = { FTM0, FTM1, FTM2, FTM3 };
//FTM��ģ���ͨ�����ź�
static const uint8 ftm_pin_table[][8] = { { FTM_MOD0_CH0_PIN, FTM_MOD0_CH1_PIN,
FTM_MOD0_CH2_PIN, FTM_MOD0_CH3_PIN, FTM_MOD0_CH4_PIN, FTM_MOD0_CH5_PIN,
FTM_MOD0_CH6_PIN, FTM_MOD0_CH7_PIN }, { FTM_MOD1_CH0_PIN, FTM_MOD1_CH1_PIN }, {
FTM_MOD2_CH0_PIN, FTM_MOD2_CH1_PIN }, { FTM_MOD3_CH0_PIN,
FTM_MOD3_CH1_PIN, FTM_MOD3_CH2_PIN, FTM_MOD3_CH3_PIN, FTM_MOD3_CH4_PIN,
FTM_MOD3_CH5_PIN, FTM_MOD3_CH6_PIN, FTM_MOD3_CH7_PIN } };
//FTM��ģ���ͨ��PCR��MUXֵ
static const uint8 ftm_pcr_mux_table[][8] = { { FTM_MOD0_CH0_PCR_MUX,
FTM_MOD0_CH1_PCR_MUX, FTM_MOD0_CH2_PCR_MUX, FTM_MOD0_CH3_PCR_MUX,
FTM_MOD0_CH4_PCR_MUX, FTM_MOD0_CH5_PCR_MUX, FTM_MOD0_CH6_PCR_MUX,
FTM_MOD0_CH7_PCR_MUX }, { FTM_MOD1_CH0_PCR_MUX, FTM_MOD1_CH1_PCR_MUX }, {
FTM_MOD2_CH0_PCR_MUX, FTM_MOD2_CH1_PCR_MUX }, {
FTM_MOD3_CH0_PCR_MUX, FTM_MOD3_CH1_PCR_MUX, FTM_MOD3_CH2_PCR_MUX,
FTM_MOD3_CH3_PCR_MUX, FTM_MOD3_CH4_PCR_MUX, FTM_MOD3_CH5_PCR_MUX,
FTM_MOD3_CH6_PCR_MUX, FTM_MOD3_CH7_PCR_MUX } };
//FTM��ģ���ж������
static const IRQn_Type ftm_irq_table[] = { FTM0_IRQn, FTM1_IRQn, FTM2_IRQn,
		FTM3_IRQn };

//==========================================================================
//��������: ftm_init
//��������: ��
//����˵��: mod:FTMģ��ţ�FTM_MODx��xΪģ���
//         period:FTMģ��������ڣ����С�������Ƶ���Ӵ�С�йأ���λms
//         div:ʱ�ӷ�Ƶ���ӣ�FTM_CLK_DIV_x��xΪ��Ƶ���Ӵ�С
//���ܸ�Ҫ: ��ʼ��FTMģ�飬Ĭ��δ�����ж�
//��ע: 48000/div*period<=65536��48000Ϊ����ʹ�õ�����ʱ��Ƶ�ʣ���λkHz
//==========================================================================
void ftm_init(uint8 mod, uint8 period, uint8 div) {
	uint16 modulo;	//ģ���Ĵ�����ֵ

	//������ӦFTMģ��ʱ����
	switch (mod) {
	case FTM_MOD0:
		REG_SET_MASK(SIM_SCGC6, SIM_SCGC6_FTM0_MASK);
		break;
	case FTM_MOD1:
		REG_SET_MASK(SIM_SCGC6, SIM_SCGC6_FTM1_MASK);
		break;
	case FTM_MOD2:
		REG_SET_MASK(SIM_SCGC3, SIM_SCGC3_FTM2_MASK);
		break;
	case FTM_MOD3:
		REG_SET_MASK(SIM_SCGC3, SIM_SCGC3_FTM3_MASK);
		break;
	}
	//��ֹд���������������д�����ļĴ�����ֵ
	REG_SET_MASK(FTM_MODE_REG(ftm_table[mod]), FTM_MODE_WPDIS_MASK);
	//�Ƚ�ֹFTM�����������Ԥ��Ƶ���ӣ����ҽ�ֹ�ж�
	REG_SET_VAL(FTM_SC_REG(ftm_table[mod]), 0);
	//�����ǰ�������Ĵ�����ֵ
	REG_SET_VAL(FTM_CNT_REG(ftm_table[mod]), 0);
	//���ü�����ÿ�εĳ�ʼֵΪ0����д��������
	REG_SET_VAL(FTM_CNTIN_REG(ftm_table[mod]), 0);
	//���ݷ�Ƶ������������ڼ���ģ����ֵ����������=(MOD-CNTIN+1)/ʱ��Ƶ��
	modulo = (FTM_CLK_FREQ >> div) * period - 1;
	//����ģ���Ĵ�����ֵ����д��������
	REG_SET_VAL(FTM_MOD_REG(ftm_table[mod]), modulo);
	//�������MOD��CNTINд�����ֵ
	REG_SET_VAL(FTM_PWMLOAD_REG(ftm_table[mod]), 0xFFFFFFFF);
	//����ʱ��ԴΪ����ʱ�ӣ���������Ԥ��Ƶ����
	REG_SET_MASK(FTM_SC_REG(ftm_table[mod]), (FTM_SC_CLKS(1) | FTM_SC_PS(div)));
}

//==========================================================================
//��������: ftm_enable_timer_int
//��������: ��
//����˵��: mod:FTMģ��ţ�FTM_MODx��xΪģ���
//         time:FTMģ��ÿtime���������ڲ���һ���ж����󣬷�Χ[1,32]
//���ܸ�Ҫ: ʹ��FTMģ���ʱ�жϣ��ж�ʱ������Ϊperiod*time����λms
//==========================================================================
void ftm_enable_timer_int(uint8 mod, uint8 time) {
	//���ò����жϵ�Ƶ��
	REG_CLR_MASK(FTM_CONF_REG(ftm_table[mod]), FTM_CONF_NUMTOF_MASK);
	REG_SET_MASK(FTM_CONF_REG(ftm_table[mod]), FTM_CONF_NUMTOF(time-1));
	//ʹ�ܶ�ʱ������ж�
	REG_SET_MASK(FTM_SC_REG(ftm_table[mod]), FTM_SC_TOIE_MASK);
	//������ո�FTMģ���ж�����
	ENABLE_IRQ(ftm_irq_table[mod]);
}

//==========================================================================
//��������: ftm_disable_timer_int
//��������: ��
//����˵��: mod:FTMģ��ţ�FTM_MODx��xΪģ���
//���ܸ�Ҫ: �ر�FTMģ���ʱ�ж�
//==========================================================================
void ftm_disable_timer_int(uint8 mod) {
	//�رն�ʱ������жϣ�δ��ֹ���ո�FTMģ���ж�������Ϊ������ͨ��������ж�����
	REG_CLR_MASK(FTM_SC_REG(ftm_table[mod]), FTM_SC_TOIE_MASK);
}

//==========================================================================
//��������: ftm_get_timer_int
//��������: true:�����ж�; false:δ�����ж�
//����˵��: mod:FTMģ��ţ�FTM_MODx��xΪģ���
//���ܸ�Ҫ: ��ȡFTMģ���ʱ�жϱ�־
//==========================================================================
bool ftm_get_timer_int(uint8 mod) {
	//��ȡ��ʱ�������־
	return (REG_GET_MASK(FTM_SC_REG(ftm_table[mod]), FTM_SC_TOF_MASK)) ?
			true : false;
}

//==========================================================================
//��������: ftm_clear_timer_int
//��������: ��
//����˵��: mod:FTMģ��ţ�FTM_MODx��xΪģ���
//���ܸ�Ҫ: ���FTMģ���ʱ�жϱ�־
//==========================================================================
void ftm_clear_timer_int(uint8 mod) {
	//�����ʱ�������־
	REG_CLR_MASK(FTM_SC_REG(ftm_table[mod]), FTM_SC_TOF_MASK);
}
