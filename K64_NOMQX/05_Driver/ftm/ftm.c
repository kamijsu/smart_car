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
//��������: ftm_ch_set_mux
//��������: ��
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//         ch:FTMģ���ͨ����:
//            FTM_CHx��xΪͨ����;
//���ܸ�Ҫ: ���ø�ģ��ĸ�ͨ������Ӧ������ΪFTMģ��ͨ������
//==========================================================================
static void ftm_ch_set_mux(uint8 mod, uint8 ch) {
	uint8 port, pin;	//�˿ں������ź�
	//����ͨ���Ż�ȡ�˿ں������ź�
	com_port_pin_resolution(ftm_pin_table[mod][ch], &port, &pin);
	//���øö˿ڵ�����ΪFTMͨ������
	REG_CLR_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_MUX_MASK);
	REG_SET_MASK(PORT_PCR_REG(port_table[port],pin),
			PORT_PCR_MUX(ftm_pcr_mux_table[mod][ch]));
}

//==========================================================================
//��������: ftm_cap_set_mode
//��������: ��
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//         ch:FTMģ���ͨ����:
//            FTM_CHx��xΪͨ����;
//         mode:��׽ģʽ:
//              FTM_CAPTURE_MODE_RISING_EDGE: �����ز�׽;
//              FTM_CAPTURE_MODE_FALLING_EDGE:�½��ز�׽;
//              FTM_CAPTURE_MODE_DOUBLE_EDGE: ˫���ز�׽;
//���ܸ�Ҫ: ����ͨ���Ĳ�׽ģʽ
//==========================================================================
static void ftm_cap_set_mode(uint8 mod, uint8 ch, uint8 mode) {
	switch (mode) {
	case FTM_CAPTURE_MODE_RISING_EDGE:
		//ELSB=0;ELSA=1;
		REG_CLR_MASK(FTM_CnSC_REG(ftm_table[mod],ch), FTM_CnSC_ELSB_MASK);
		REG_SET_MASK(FTM_CnSC_REG(ftm_table[mod],ch), FTM_CnSC_ELSA_MASK);
		break;
	case FTM_CAPTURE_MODE_FALLING_EDGE:
		//ELSB=1;ELSA=0;
		REG_SET_MASK(FTM_CnSC_REG(ftm_table[mod],ch), FTM_CnSC_ELSB_MASK);
		REG_CLR_MASK(FTM_CnSC_REG(ftm_table[mod],ch), FTM_CnSC_ELSA_MASK);
		break;
	case FTM_CAPTURE_MODE_DOUBLE_EDGE:
		//ELSB=1;ELSA=1;
		REG_SET_MASK(FTM_CnSC_REG(ftm_table[mod],ch), FTM_CnSC_ELSB_MASK);
		REG_SET_MASK(FTM_CnSC_REG(ftm_table[mod],ch), FTM_CnSC_ELSA_MASK);
		break;
	}
}

//==========================================================================
//��������: ftm_init
//��������: ��
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//         clk_div:ʱ�ӷ�Ƶ����:
//                 FTM_CLK_DIV_x��xΪ��Ƶ���Ӵ�С;
//         counter_mode:������ģʽ:
//                      FTM_COUNTER_MODE_UP:          ���ϼ���;
//                      FTM_COUNTER_MODE_UP_DOWN:     ���¼���;
//                      FTM_COUNTER_MODE_FREE_RUNNING:��������;
//                      FTM_COUNTER_MODE_QD:          ��������;
//         counter_period:����ע
//���ܸ�Ҫ: ��ʼ��FTMģ�飬Ĭ��δ�����ж�
//��ע: ��ѡ�����ϼ���ģʽ�����¼���ģʽʱ��counter_periodΪ�������������ڣ���λms��
//     ������48000/x*counter_period<=num��48000Ϊ����ʹ�õ�����ʱ��Ƶ�ʣ���λkHz��
//     xΪFTM_CLK_DIV_x��x�����ϼ���ģʽʱnumΪ65536�����¼���ģʽʱnumΪ65534��
//     ���⣬���¼���ģʽʱ����ѡ��128��Ƶ��������Ϊż��(����������Ȼᶪʧ);
//     ��ѡ����������ģʽʱ��counter_period��Ч��
//     ��������Ĭ��Ϊ65536/(48000/x)����λms����С����;
//     ��ѡ����������ģʽʱ��counter_periodΪÿ�β����ж�ʱ���������Ѿ������ĸ�����
//     ��Χ[1,65536];
//==========================================================================
void ftm_init(uint8 mod, uint8 clk_div, uint8 counter_mode,
		uint8 counter_period) {
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
	//���ü�����ģʽ
	switch (counter_mode) {
	case FTM_COUNTER_MODE_UP:
		//���ϼ���:QUADEN=0;CPWMS=0;
		REG_CLR_MASK(FTM_QDCTRL_REG(ftm_table[mod]), FTM_QDCTRL_QUADEN_MASK);
		REG_CLR_MASK(FTM_SC_REG(ftm_table[mod]), FTM_SC_CPWMS_MASK);
		//��������=(MOD-CNTIN+1)/ʱ��Ƶ��
		modulo = (FTM_CLK_FREQ >> clk_div) * counter_period - 1;
		break;
	case FTM_COUNTER_MODE_UP_DOWN:
		//���¼���:QUADEN=0;CPWMS=1
		REG_CLR_MASK(FTM_QDCTRL_REG(ftm_table[mod]), FTM_QDCTRL_QUADEN_MASK);
		REG_SET_MASK(FTM_SC_REG(ftm_table[mod]), FTM_SC_CPWMS_MASK);
		//��������=2*(MOD-CNTIN)/ʱ��Ƶ��
		modulo = ((FTM_CLK_FREQ >> clk_div) * counter_period) >> 1;
		break;
	case FTM_COUNTER_MODE_FREE_RUNNING:
		//��������:FTMEN=1;QUADEN=0;CPWMS=0;CNTIN=0x0000;MOD=0xFFFF;
		REG_SET_MASK(FTM_MODE_REG(ftm_table[mod]), FTM_MODE_FTMEN_MASK);
		REG_CLR_MASK(FTM_QDCTRL_REG(ftm_table[mod]), FTM_QDCTRL_QUADEN_MASK);
		REG_CLR_MASK(FTM_SC_REG(ftm_table[mod]), FTM_SC_CPWMS_MASK);
		modulo = 0xFFFF;
		break;
	case FTM_COUNTER_MODE_QD:
		//��������:FTMEN=1;QUADEN=1;
		REG_SET_MASK(FTM_MODE_REG(ftm_table[mod]), FTM_MODE_FTMEN_MASK);
		REG_SET_MASK(FTM_QDCTRL_REG(ftm_table[mod]), FTM_QDCTRL_QUADEN_MASK);
		//MOD��Ҫ��1
		modulo = counter_period - 1;
		break;
	}
	//����ģ���Ĵ�����ֵ����д��������
	REG_SET_VAL(FTM_MOD_REG(ftm_table[mod]), modulo);
	//ʹ��PWM���Զ����빦�ܣ�MOD��CNTIN��C(n)V��C(n+1)V�ڼ���������MODʱ���뻺�����е�ֵ
	REG_SET_VAL(FTM_PWMLOAD_REG(ftm_table[mod]), 0xFFFFFFFF);
	//����ʱ��ԴΪ����ʱ�ӣ���������Ԥ��Ƶ����
	REG_SET_MASK(FTM_SC_REG(ftm_table[mod]),
			(FTM_SC_CLKS(1) | FTM_SC_PS(clk_div)));
}

//==========================================================================
//��������: ftm_timer_enable_int
//��������: ��
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//         time:FTMģ��ÿtime���������ڲ���һ���ж����󣬷�Χ[1,32]
//���ܸ�Ҫ: ʹ��FTMģ���ʱ�ж�
//��ע: ��ӦFTMģ��ļ����������������ϼ���ģʽ�£�
//     �ж�ʱ������Ϊcounter_period*time����λms��
//     counter_periodΪ��ӦFTMģ��ļ������ڣ���λms
//==========================================================================
void ftm_timer_enable_int(uint8 mod, uint8 time) {
	//���ò����жϵ�Ƶ��
	REG_CLR_MASK(FTM_CONF_REG(ftm_table[mod]), FTM_CONF_NUMTOF_MASK);
	REG_SET_MASK(FTM_CONF_REG(ftm_table[mod]), FTM_CONF_NUMTOF(time - 1));
	//ʹ�ܶ�ʱ������ж�
	REG_SET_MASK(FTM_SC_REG(ftm_table[mod]), FTM_SC_TOIE_MASK);
	//������ո�FTMģ���ж�����
	ENABLE_IRQ(ftm_irq_table[mod]);
}

//==========================================================================
//��������: ftm_disable_int
//��������: ��
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//���ܸ�Ҫ: �ر�FTMģ���жϣ�����ر�FTMͨ���ж�
//��ע: ���Թر�ftm_timer_enable_intʹ�ܵ��ж�
//==========================================================================
void ftm_disable_int(uint8 mod) {
	//�رռ���������жϣ�δ��ֹ���ո�FTMģ���ж�������Ϊ������ͨ��������ж�����
	REG_CLR_MASK(FTM_SC_REG(ftm_table[mod]), FTM_SC_TOIE_MASK);
}

//==========================================================================
//��������: ftm_get_int
//��������: true:�����ж�; false:δ�����ж�
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//���ܸ�Ҫ: ��ȡFTMģ���жϱ�־�������ȡFTMͨ���жϱ�־
//��ע: ���Ի�ȡftm_timer_enable_intʹ�ܵ��жϵ��жϱ�־
//==========================================================================
bool ftm_get_int(uint8 mod) {
	//��ȡ�����������־
	return (REG_GET_MASK(FTM_SC_REG(ftm_table[mod]), FTM_SC_TOF_MASK)) ?
			true : false;
}

//==========================================================================
//��������: ftm_clear_int
//��������: ��
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//���ܸ�Ҫ: ���FTMģ���жϱ�־���������FTMͨ���жϱ�־
//��ע: �������ftm_timer_enable_intʹ�ܵ��жϵ��жϱ�־
//==========================================================================
void ftm_clear_int(uint8 mod) {
	//��������������־
	REG_CLR_MASK(FTM_SC_REG(ftm_table[mod]), FTM_SC_TOF_MASK);
}

//==========================================================================
//��������: ftm_pwm_single_init
//��������: ��
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//         ch:FTMģ���ͨ����:
//            FTM_CHx��xΪͨ����;
//         mode:PWMģʽ:
//              FTM_PWM_MODE_EDGE_ALIGNED:  ���ض���ģʽ;
//              FTM_PWM_MODE_CENTER_ALIGNED:���Ķ���ģʽ;
//         pol:PWM����:
//             FTM_PWM_POL_POSITIVE:������;
//             FTM_PWM_POL_NEGATIVE:������;
//         duty:��ʼռ�ձȣ���Χ[FTM_DUTY_MIN(0),FTM_DUTY_MAX(10000)]������δ�޷�
//���ܸ�Ҫ: ��ʼ��FTMģ���ͨ��Ϊ��ͨ����PWM����
//��ע: ��ѡ����ض���ģʽʱ����ӦFTMģ��ļ����������������ϼ���ģʽ��;
//     ��ѡ�����Ķ���ģʽʱ����ӦFTMģ��ļ����������������¼���ģʽ��;
//     PWM����Ƶ��Ϊ(1000/counter_period)����λHz��
//     counter_periodΪ��ӦFTMģ��ļ������ڣ���λms
//==========================================================================
void ftm_pwm_single_init(uint8 mod, uint8 ch, uint8 mode, uint8 pol,
		uint16 duty) {
	uint8 shift;	//����FTMx_COMBINE�Ĵ���ʱ��ƫ����

	shift = (ch >> 1) << 3;	//����COMBINEn���8λ
	//ʹ��FTMģ��ͨ������
	ftm_ch_set_mux(mod, ch);
	//�ر�FTM���ܣ����رյĻ��޷�ʹ�õ�ͨ��
	REG_CLR_MASK(FTM_MODE_REG(ftm_table[mod]), FTM_MODE_FTMEN_MASK);
	//�ر�ͨ���ж�
	REG_CLR_MASK(FTM_CnSC_REG(ftm_table[mod],ch), FTM_CnSC_CHIE_MASK);
	//����ͨ��Ϊ��Ӧ��PWM����
	//COMBINEn=0;COMPn=0;DECAPENn=0;SYNCEN=0;
	REG_CLR_MASK(FTM_COMBINE_REG(ftm_table[mod]),
			FTM_COMBINE_COMBINE0_MASK<<shift);
	REG_CLR_MASK(FTM_COMBINE_REG(ftm_table[mod]),
			FTM_COMBINE_COMP0_MASK<<shift);
	REG_CLR_MASK(FTM_COMBINE_REG(ftm_table[mod]),
			FTM_COMBINE_DECAPEN0_MASK<<shift);
	REG_CLR_MASK(FTM_COMBINE_REG(ftm_table[mod]),
			FTM_COMBINE_SYNCEN0_MASK<<shift);
	if (mode == FTM_PWM_MODE_EDGE_ALIGNED) {
		//MSB=1;
		REG_SET_MASK(FTM_CnSC_REG(ftm_table[mod],ch), FTM_CnSC_MSB_MASK);
	}
	//����PWM������
	if (pol == FTM_PWM_POL_POSITIVE) {
		//ELSB=1;ELSA=0;
		REG_SET_MASK(FTM_CnSC_REG(ftm_table[mod],ch), FTM_CnSC_ELSB_MASK);
		REG_CLR_MASK(FTM_CnSC_REG(ftm_table[mod],ch), FTM_CnSC_ELSA_MASK);
	} else {
		//ELSA=1;
		REG_SET_MASK(FTM_CnSC_REG(ftm_table[mod],ch), FTM_CnSC_ELSA_MASK);
	}
	//���ó�ʼռ�ձ�
	ftm_pwm_single_set(mod, ch, duty);
}

//==========================================================================
//��������: ftm_pwm_single_set
//��������: ��
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//         ch:FTMģ���ͨ����:
//            FTM_CHx��xΪͨ����;
//         duty:ռ�ձȣ���Χ[FTM_DUTY_MIN(0),FTM_DUTY_MAX(10000)]������δ�޷�
//���ܸ�Ҫ: ���ø�ͨ����ռ�ձȣ�������һ���������ڸ���
//==========================================================================
void ftm_pwm_single_set(uint8 mod, uint8 ch, uint16 duty) {
	//����ռ�ձȣ�������һ�����ڸ���CnV��ֵ
	//���Ķ���ģʽ
	if (REG_GET_MASK(FTM_SC_REG(ftm_table[mod]), FTM_SC_CPWMS_MASK)) {
		REG_SET_VAL(FTM_CnV_REG(ftm_table[mod],ch),
				(FTM_MOD_REG(ftm_table[mod]) * duty / FTM_PERIOD_ACCURACY));
	} else {
		//���ض���ģʽ
		REG_SET_VAL(FTM_CnV_REG(ftm_table[mod],ch),
				((FTM_MOD_REG(ftm_table[mod]) + 1) * duty / FTM_PERIOD_ACCURACY));
	}
}

//==========================================================================
//��������: ftm_pwm_combine_init
//��������: ��
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//         ch_group:FTMģ���ͨ�����:
//                  FTM_CH_GROUPx��xΪͨ�����;
//         mode:PWMģʽ:
//              FTM_PWM_MODE_COMBINE:      ����ģʽ����ͨ�������ͬ;
//              FTM_PWM_MODE_COMPLEMENTARY:����ģʽ����ͨ������෴;
//         pol:PWM����:
//             FTM_PWM_POL_POSITIVE:������;
//             FTM_PWM_POL_NEGATIVE:������;
//         duty1:��ʼռ�ձ�1����Χ[FTM_DUTY_MIN(0),FTM_DUTY_MAX(10000)]������δ�޷�
//         duty2:��ʼռ�ձ�2����Χ[FTM_DUTY_MIN(0),FTM_DUTY_MAX(10000)]������δ�޷�
//���ܸ�Ҫ: ��ʼ��FTMģ���ͨ����Ϊ˫ͨ����PWM����
//��ע: ��ӦFTMģ��ļ����������������ϼ���ģʽ��;
//     ����PWM����ռ�ձ�Ϊ(duty2-duty1)��duty1��С��duty2����duty1����duty2��
//     PWM�����Խ��ٴη�ת;
//     PWM����Ƶ��Ϊ(1000/counter_period)����λHz��
//     counter_periodΪ��ӦFTMģ��ļ������ڣ���λms;
//     ż��ͨ�������PWM��������ѡ������ģʽ������һ���ģ�������ģʽֻ�������ͨ��
//==========================================================================
void ftm_pwm_combine_init(uint8 mod, uint8 ch_group, uint8 mode, uint8 pol,
		uint16 duty1, uint16 duty2) {
	uint8 ch0, ch1;	//����ͨ����
	uint8 shift;	//����FTMx_COMBINE�Ĵ���ʱ��ƫ����

	ch0 = ch_group << 1;	//ż��ͨ��
	ch1 = ch0 + 1;			//����ͨ��
	shift = ch_group << 3;	//����COMBINEn���8λ
	//ʹ��FTMģ��ͨ������
	ftm_ch_set_mux(mod, ch0);
	ftm_ch_set_mux(mod, ch1);
	//ʹ��FTM����
	REG_SET_MASK(FTM_MODE_REG(ftm_table[mod]), FTM_MODE_FTMEN_MASK);
	//�ر�ͨ���ж�
	REG_CLR_MASK(FTM_CnSC_REG(ftm_table[mod],ch0), FTM_CnSC_CHIE_MASK);
	REG_CLR_MASK(FTM_CnSC_REG(ftm_table[mod],ch1), FTM_CnSC_CHIE_MASK);
	//����ͨ��Ϊ��Ӧ��PWM����
	//COMBINEn=1;DECAPENn=0;SYNCEN=1;
	REG_SET_MASK(FTM_COMBINE_REG(ftm_table[mod]),
			FTM_COMBINE_COMBINE0_MASK<<shift);
	REG_CLR_MASK(FTM_COMBINE_REG(ftm_table[mod]),
			FTM_COMBINE_DECAPEN0_MASK<<shift);
	REG_SET_MASK(FTM_COMBINE_REG(ftm_table[mod]),
			FTM_COMBINE_SYNCEN0_MASK<<shift);
	if (mode == FTM_PWM_MODE_COMBINE) {
		//COMPn=0;
		REG_CLR_MASK(FTM_COMBINE_REG(ftm_table[mod]),
				FTM_COMBINE_COMP0_MASK<<shift);
	} else {
		//COMPn=1;
		REG_SET_MASK(FTM_COMBINE_REG(ftm_table[mod]),
				FTM_COMBINE_COMP0_MASK<<shift);
	}
	//����PWM������
	if (pol == FTM_PWM_POL_POSITIVE) {
		//ELSB=1;ELSA=0;
		REG_SET_MASK(FTM_CnSC_REG(ftm_table[mod],ch0), FTM_CnSC_ELSB_MASK);
		REG_CLR_MASK(FTM_CnSC_REG(ftm_table[mod],ch0), FTM_CnSC_ELSA_MASK);
		REG_SET_MASK(FTM_CnSC_REG(ftm_table[mod],ch1), FTM_CnSC_ELSB_MASK);
		REG_CLR_MASK(FTM_CnSC_REG(ftm_table[mod],ch1), FTM_CnSC_ELSA_MASK);
	} else {
		//ELSA=1;
		REG_SET_MASK(FTM_CnSC_REG(ftm_table[mod],ch0), FTM_CnSC_ELSA_MASK);
		REG_SET_MASK(FTM_CnSC_REG(ftm_table[mod],ch1), FTM_CnSC_ELSA_MASK);
	}
	//��Сװ�ص�ʹ�ܣ����������ﵽCNTINֵʱ��Ϊͬ����һ�������;��ֹӲ������ͬ��;ͬ��ʱ��������������
	REG_SET_VAL(FTM_SYNC_REG(ftm_table[mod]), FTM_SYNC_CNTMIN_MASK);
	//���ó�ʼռ�ձ�
	ftm_pwm_combine_set(mod, ch_group, duty1, duty2);
}

//==========================================================================
//��������: ftm_pwm_combine_set
//��������: ��
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//         ch_group:FTMģ���ͨ�����:
//                  FTM_CH_GROUPx��xΪͨ�����;
//         duty1:ռ�ձ�1����Χ[FTM_DUTY_MIN(0),FTM_DUTY_MAX(10000)]������δ�޷�
//         duty2:ռ�ձ�2����Χ[FTM_DUTY_MIN(0),FTM_DUTY_MAX(10000)]������δ�޷�
//���ܸ�Ҫ: ���ø�ͨ�����ռ�ձȣ�������һ���������ڸ���
//��ע: ����PWM����ռ�ձ�Ϊ(duty2-duty1)��duty1��С��duty2����duty1����duty2��
//     PWM�����Խ��ٴη�ת
//==========================================================================
void ftm_pwm_combine_set(uint8 mod, uint8 ch_group, uint16 duty1, uint16 duty2) {
	//����ż��ͨ��ռ�ձ�
	REG_SET_VAL(FTM_CnV_REG(ftm_table[mod],ch_group << 1),
			((FTM_MOD_REG(ftm_table[mod]) + 1) * duty1 / FTM_PERIOD_ACCURACY));
	//��������ͨ��ռ�ձ�
	REG_SET_VAL(FTM_CnV_REG(ftm_table[mod],(ch_group << 1) + 1),
			((FTM_MOD_REG(ftm_table[mod]) + 1) * duty2 / FTM_PERIOD_ACCURACY));
	//���������һ�������ͬ������CnV
	REG_SET_MASK(FTM_SYNC_REG(ftm_table[mod]), FTM_SYNC_SWSYNC_MASK);
}

//==========================================================================
//��������: ftm_ic_init
//��������: ��
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//         ch:FTMģ���ͨ����:
//            FTM_CHx��xΪͨ����;
//         mode:���벶׽��ģʽ:
//              FTM_CAPTURE_MODE_RISING_EDGE: �����ز�׽;
//              FTM_CAPTURE_MODE_FALLING_EDGE:�½��ز�׽;
//              FTM_CAPTURE_MODE_DOUBLE_EDGE: ˫���ز�׽;
//���ܸ�Ҫ: ��ʼ��FTMģ���ͨ��Ϊ���벶׽���ܣ�Ĭ��δ�����ж�
//��ע: ��ӦFTMģ��ļ����������������ϼ���ģʽ��
//==========================================================================
void ftm_ic_init(uint8 mod, uint8 ch, uint8 mode) {
	uint8 shift;	//����FTMx_COMBINE�Ĵ���ʱ��ƫ����

	shift = (ch >> 1) << 3;	//����COMBINEn���8λ
	//ʹ��FTMģ��ͨ������
	ftm_ch_set_mux(mod, ch);
	//�ر�FTM���ܣ����رյĻ��޷�ʹ�õ�ͨ��
	REG_CLR_MASK(FTM_MODE_REG(ftm_table[mod]), FTM_MODE_FTMEN_MASK);
	//�ر�ͨ���ж�
	REG_CLR_MASK(FTM_CnSC_REG(ftm_table[mod],ch), FTM_CnSC_CHIE_MASK);
	//����ͨ��Ϊ���벶׽����
	//COMBINEn=0;COMPn=0;DECAPENn=0;SYNCEN=0;
	REG_CLR_MASK(FTM_COMBINE_REG(ftm_table[mod]),
			FTM_COMBINE_COMBINE0_MASK<<shift);
	REG_CLR_MASK(FTM_COMBINE_REG(ftm_table[mod]),
			FTM_COMBINE_COMP0_MASK<<shift);
	REG_CLR_MASK(FTM_COMBINE_REG(ftm_table[mod]),
			FTM_COMBINE_DECAPEN0_MASK<<shift);
	REG_CLR_MASK(FTM_COMBINE_REG(ftm_table[mod]),
			FTM_COMBINE_SYNCEN0_MASK<<shift);
	ftm_cap_set_mode(mod, ch, mode);
	//MSB=0;MSA=0;
	REG_CLR_MASK(FTM_CnSC_REG(ftm_table[mod],ch), FTM_CnSC_MSB_MASK);
	REG_CLR_MASK(FTM_CnSC_REG(ftm_table[mod],ch), FTM_CnSC_MSA_MASK);
}

//==========================================================================
//��������: ftm_ic_get_ratio
//��������: ���㲶׽����ʱ�ı���
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//         ch:FTMģ���ͨ����:
//            FTM_CHx��xΪͨ����;
//���ܸ�Ҫ: ��ȡ�����㲶׽����ʱ����������������ռ�����������ڵı���
//��ע: ����ֵ����FTM_PERIOD_ACCURACY(10000)Ϊ�ٷֱȵı���;
//     ������������PWM��ռ�ձȣ�����:
//     �����ز�׽ģʽ:����ֵΪ��ͬ�����������ض���ģʽ��������PWM��ռ�ձ�;
//     �½��ز�׽ģʽ:����ֵΪ��ͬ�����������ض���ģʽ��������PWM��ռ�ձ�;
//==========================================================================
uint16 ftm_ic_get_ratio(uint8 mod, uint8 ch) {
	return FTM_CnV_REG(ftm_table[mod],ch) * FTM_PERIOD_ACCURACY
			/ (FTM_MOD_REG(ftm_table[mod]) + 1);
}

//==========================================================================
//��������: ftm_oc_init
//��������: ��
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//         ch:FTMģ���ͨ����:
//            FTM_CHx��xΪͨ����;
//         mode:����Ƚϵ�ģʽ:
//              FTM_OC_MODE_TOGGLE:�Ƚϳɹ���ת��ƽ;
//              FTM_OC_MODE_SET:   �Ƚϳɹ����øߵ�ƽ;
//              FTM_OC_MODE_CLEAR: �Ƚϳɹ����õ͵�ƽ;
//         ratio:�Ƚϳɹ���ʱ��ռ�����������ڵı�������Χ[0,FTM_PERIOD_ACCURACY(10000))
//���ܸ�Ҫ: ��ʼ��FTMģ���ͨ��Ϊ����ȽϹ��ܣ�Ĭ��δ�����ж�
//��ע: ��ӦFTMģ��ļ����������������ϼ���ģʽ��;
//     ratio����FTM_PERIOD_ACCURACY(10000)Ϊ�ٷֱȵı���
//==========================================================================
void ftm_oc_init(uint8 mod, uint8 ch, uint8 mode, uint16 ratio) {
	uint8 shift;	//����FTMx_COMBINE�Ĵ���ʱ��ƫ����

	shift = (ch >> 1) << 3;	//����COMBINEn���8λ
	//ʹ��FTMģ��ͨ������
	ftm_ch_set_mux(mod, ch);
	//�ر�FTM����
	REG_CLR_MASK(FTM_MODE_REG(ftm_table[mod]), FTM_MODE_FTMEN_MASK);
	//�ر�ͨ���ж�
	REG_CLR_MASK(FTM_CnSC_REG(ftm_table[mod],ch), FTM_CnSC_CHIE_MASK);
	//����ͨ��Ϊ����ȽϹ���
	//COMBINEn=0;COMPn=0;DECAPENn=0;SYNCEN=0;
	REG_CLR_MASK(FTM_COMBINE_REG(ftm_table[mod]),
			FTM_COMBINE_COMBINE0_MASK<<shift);
	REG_CLR_MASK(FTM_COMBINE_REG(ftm_table[mod]),
			FTM_COMBINE_COMP0_MASK<<shift);
	REG_CLR_MASK(FTM_COMBINE_REG(ftm_table[mod]),
			FTM_COMBINE_DECAPEN0_MASK<<shift);
	REG_CLR_MASK(FTM_COMBINE_REG(ftm_table[mod]),
			FTM_COMBINE_SYNCEN0_MASK<<shift);
	ftm_oc_change_mode(mod, ch, mode);
	//MSB=0;MSA=1;
	REG_CLR_MASK(FTM_CnSC_REG(ftm_table[mod],ch), FTM_CnSC_MSB_MASK);
	REG_SET_MASK(FTM_CnSC_REG(ftm_table[mod],ch), FTM_CnSC_MSA_MASK);
	//���ñȽϳɹ���
	ftm_oc_set_ratio(mod, ch, ratio);
}

//==========================================================================
//��������: ftm_oc_change_mode
//��������: ��
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//         ch:FTMģ���ͨ����:
//            FTM_CHx��xΪͨ����;
//         mode:����Ƚϵ�ģʽ:
//              FTM_OC_MODE_TOGGLE:�Ƚϳɹ���ת��ƽ;
//              FTM_OC_MODE_SET:   �Ƚϳɹ����øߵ�ƽ;
//              FTM_OC_MODE_CLEAR: �Ƚϳɹ����õ͵�ƽ;
//���ܸ�Ҫ: ��������ȽϹ���ͨ����ģʽ
//==========================================================================
void ftm_oc_change_mode(uint8 mod, uint8 ch, uint8 mode) {
	switch (mode) {
	case FTM_OC_MODE_TOGGLE:
		//ELSB=0;ELSA=1;
		REG_CLR_MASK(FTM_CnSC_REG(ftm_table[mod],ch), FTM_CnSC_ELSB_MASK);
		REG_SET_MASK(FTM_CnSC_REG(ftm_table[mod],ch), FTM_CnSC_ELSA_MASK);
		break;
	case FTM_OC_MODE_SET:
		//ELSB=1;ELSA=1;
		REG_SET_MASK(FTM_CnSC_REG(ftm_table[mod],ch), FTM_CnSC_ELSB_MASK);
		REG_SET_MASK(FTM_CnSC_REG(ftm_table[mod],ch), FTM_CnSC_ELSA_MASK);
		break;
	case FTM_OC_MODE_CLEAR:
		//ELSB=1;ELSA=0;
		REG_SET_MASK(FTM_CnSC_REG(ftm_table[mod],ch), FTM_CnSC_ELSB_MASK);
		REG_CLR_MASK(FTM_CnSC_REG(ftm_table[mod],ch), FTM_CnSC_ELSA_MASK);
		break;
	}
}

//==========================================================================
//��������: ftm_oc_set_ratio
//��������: ��
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//         ch:FTMģ���ͨ����:
//            FTM_CHx��xΪͨ����;
//         ratio:�Ƚϳɹ���ʱ��ռ�����������ڵı�������Χ[0,FTM_PERIOD_ACCURACY(10000))
//���ܸ�Ҫ: ��������ȽϹ���ͨ���ıȽϳɹ�ʱ��
//==========================================================================
void ftm_oc_set_ratio(uint8 mod, uint8 ch, uint16 ratio) {
	REG_SET_VAL(FTM_CnV_REG(ftm_table[mod],ch),
			((FTM_MOD_REG(ftm_table[mod]) + 1) * ratio / FTM_PERIOD_ACCURACY));
}

//==========================================================================
//��������: ftm_ch_enable_int
//��������: ��
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//         ch:FTMģ���ͨ����:
//            FTM_CHx��xΪͨ����;
//���ܸ�Ҫ: ʹ��ͨ���ж�
//��ע: ����ʹ�����벶׽������ȽϹ���ͨ�����ж�:
//     ���벶׽:��������ź����㲶׽����ʱ�������ж�;
//     ����Ƚ�:���Ƚϳɹ�ʱ�������ж�;
//==========================================================================
void ftm_ch_enable_int(uint8 mod, uint8 ch) {
	//ʹ��ͨ���¼��ж�
	REG_SET_MASK(FTM_CnSC_REG(ftm_table[mod],ch), FTM_CnSC_CHIE_MASK);
	//������ո�FTMģ���ж�����
	ENABLE_IRQ(ftm_irq_table[mod]);
}

//==========================================================================
//��������: ftm_ch_disable_int
//��������: ��
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//         ch:FTMģ���ͨ����:
//            FTM_CHx��xΪͨ����;
//���ܸ�Ҫ: �ر�ͨ���ж�
//==========================================================================
void ftm_ch_disable_int(uint8 mod, uint8 ch) {
	//�ر�ͨ���¼��жϣ�δ��ֹ���ո�FTMģ���ж�������Ϊ�����б��ͨ��������ж�����
	REG_CLR_MASK(FTM_CnSC_REG(ftm_table[mod],ch), FTM_CnSC_CHIE_MASK);
}

//==========================================================================
//��������: ftm_ch_get_int
//��������: true:�����ж�; false:δ�����ж�;
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//         ch:FTMģ���ͨ����:
//            FTM_CHx��xΪͨ����;
//���ܸ�Ҫ: ��ȡͨ�����жϱ�־
//==========================================================================
bool ftm_ch_get_int(uint8 mod, uint8 ch) {
	return (REG_GET_MASK(FTM_CnSC_REG(ftm_table[mod],ch), FTM_CnSC_CHF_MASK)) ?
			true : false;
}

//==========================================================================
//��������: ftm_ch_clear_int
//��������: ��
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//         ch:FTMģ���ͨ����:
//            FTM_CHx��xΪͨ����;
//���ܸ�Ҫ: ���ͨ�����жϱ�־
//==========================================================================
void ftm_ch_clear_int(uint8 mod, uint8 ch) {
	REG_CLR_MASK(FTM_CnSC_REG(ftm_table[mod],ch), FTM_CnSC_CHF_MASK);
}

//==========================================================================
//��������: ftm_decap_init
//��������: ��
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//         ch_group:FTMģ���ͨ�����:
//                  FTM_CH_GROUPx��xΪͨ�����;
//         decap_mode:˫���ز�׽ģʽ:
//                    FTM_DECAP_MODE_ONCE:      ���β�׽;
//                    FTM_DECAP_MODE_CONTINUOUS:������׽;
//         ch_mode0:ż��ͨ����׽ģʽ:
//                  FTM_CAPTURE_MODE_RISING_EDGE: �����ز�׽;
//                  FTM_CAPTURE_MODE_FALLING_EDGE:�½��ز�׽;
//                  FTM_CAPTURE_MODE_DOUBLE_EDGE: ˫���ز�׽;
//         ch_mode1:����ͨ����׽ģʽ������ͬ��
//���ܸ�Ҫ: ��ʼ��FTMģ���ͨ����Ϊ˫���ز�׽����
//��ע: ��ӦFTMģ��ļ���������������������ģʽ��;
//     ����ż��ͨ���Ჶ׽���룬����ͨ��������ᱻ����;
//     ########################ûд�꣡�����������������������ch_mode0��ch_mode1
//==========================================================================
void ftm_decap_init(uint8 mod, uint8 ch_group, uint8 decap_mode, uint8 ch_mode0,
		uint8 ch_mode1) {
	uint8 ch0, ch1;	//����ͨ����
	uint8 shift;	//����FTMx_COMBINE�Ĵ���ʱ��ƫ����
//	uint8 port0, pin0,port1,pin1;	//�˿ں������ź�

	ch0 = ch_group << 1;	//ż��ͨ��
	ch1 = ch0 + 1;			//����ͨ��
//	//����ͨ���Ż�ȡ�˿ں������ź�
//			com_port_pin_resolution(ftm_pin_table[mod][ch0], &port0, &pin0);
//			com_port_pin_resolution(ftm_pin_table[mod][ch1], &port1, &pin1);
	shift = ch_group << 3;	//����COMBINEn���8λ
	//ʹ��FTMģ��ͨ������
	ftm_ch_set_mux(mod, ch0);
	ftm_ch_set_mux(mod, ch1);
	//�ر�ͨ���ж�
	REG_CLR_MASK(FTM_CnSC_REG(ftm_table[mod],ch0), FTM_CnSC_CHIE_MASK);
	REG_CLR_MASK(FTM_CnSC_REG(ftm_table[mod],ch1), FTM_CnSC_CHIE_MASK);
	//����ͨ��Ϊ˫���ز�׽����
	if (decap_mode == FTM_DECAP_MODE_ONCE) {
		//MSA=0;
		REG_CLR_MASK(FTM_CnSC_REG(ftm_table[mod],ch0), FTM_CnSC_MSA_MASK);
		REG_CLR_MASK(FTM_CnSC_REG(ftm_table[mod],ch1), FTM_CnSC_MSA_MASK);
	} else {
		//MSA=1;
		REG_SET_MASK(FTM_CnSC_REG(ftm_table[mod],ch0), FTM_CnSC_MSA_MASK);
		REG_SET_MASK(FTM_CnSC_REG(ftm_table[mod],ch1), FTM_CnSC_MSA_MASK);
	}
	//��������ͨ���Ĳ�׽ģʽ
	ftm_cap_set_mode(mod, ch0, ch_mode0);
	ftm_cap_set_mode(mod, ch1, ch_mode1);
	//COMBINEn=0;SYNCEN=0;COMPn=0;DECAPENn=1;
	REG_CLR_MASK(FTM_COMBINE_REG(ftm_table[mod]),
			FTM_COMBINE_COMBINE0_MASK<<shift);
	REG_CLR_MASK(FTM_COMBINE_REG(ftm_table[mod]),
			FTM_COMBINE_SYNCEN0_MASK<<shift);
	REG_CLR_MASK(FTM_COMBINE_REG(ftm_table[mod]),
			FTM_COMBINE_COMP0_MASK<<shift);
	REG_SET_MASK(FTM_COMBINE_REG(ftm_table[mod]),
			FTM_COMBINE_DECAPEN0_MASK<<shift);
//	REG_SET_MASK(PORT_PCR_REG(port_table[port0],pin0), PORT_PCR_PS_MASK);	//������������ʹ��
//	REG_SET_MASK(PORT_PCR_REG(port_table[port0],pin0), PORT_PCR_PE_MASK);	//��������������
//	REG_SET_MASK(PORT_PCR_REG(port_table[port1],pin1), PORT_PCR_PS_MASK);	//������������ʹ��
//		REG_SET_MASK(PORT_PCR_REG(port_table[port1],pin1), PORT_PCR_PE_MASK);	//��������������
}

//==========================================================================
//��������: ftm_decap_get_ratio
//��������: ��Ҫ��׽�Ĳ�����ռ�����������ڵı���
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//         ch_group:FTMģ���ͨ�����:
//                  FTM_CH_GROUPx��xΪͨ�����;
//���ܸ�Ҫ: ��ȡ��Ҫ��׽�Ĳ�����ռ�����������ڵı���
//��ע: ����ֵ����FTM_PERIOD_ACCURACY(10000)Ϊ�ٷֱȵı���
//==========================================================================
uint16 ftm_decap_get_ratio(uint8 mod, uint8 ch_group) {
	uint16 ratio0, ratio1;
	//�ȶ�C(n)V���ٶ�C(n+1)V
	ratio0 = FTM_CnV_REG(ftm_table[mod], ch_group << 1);
	ratio1 = FTM_CnV_REG(ftm_table[mod], (ch_group << 1) + 1);
	return (ratio1 - ratio0) * FTM_PERIOD_ACCURACY / 0x10000;
}

//==========================================================================
//��������: ftm_decap_enable_int
//��������: ��
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//         ch_group:FTMģ���ͨ�����:
//                  FTM_CH_GROUPx��xΪͨ�����;
//���ܸ�Ҫ: ʹ��˫���ز�׽���ܵ�ͨ������жϣ�������ͨ���Ĳ�׽������������ʱ�������ж�
//==========================================================================
void ftm_decap_enable_int(uint8 mod, uint8 ch_group) {
	//ʹ��ͨ���¼��ж�
//	REG_SET_MASK(FTM_CnSC_REG(ftm_table[mod],ch_group << 1),
//			FTM_CnSC_CHIE_MASK);
	REG_SET_MASK(FTM_CnSC_REG(ftm_table[mod],(ch_group << 1) + 1),
			FTM_CnSC_CHIE_MASK);
	//������ո�FTMģ���ж�����
	ENABLE_IRQ(ftm_irq_table[mod]);
}

//==========================================================================
//��������: ftm_decap_disable_int
//��������: ��
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//         ch_group:FTMģ���ͨ�����:
//                  FTM_CH_GROUPx��xΪͨ�����;
//���ܸ�Ҫ: �ر�˫���ز�׽���ܵ�ͨ������ж�
//==========================================================================
void ftm_decap_disable_int(uint8 mod, uint8 ch_group) {
	//�ر�ͨ���¼��ж�
	REG_CLR_MASK(FTM_CnSC_REG(ftm_table[mod],ch_group << 1),
			FTM_CnSC_CHIE_MASK);
	REG_CLR_MASK(FTM_CnSC_REG(ftm_table[mod],(ch_group << 1) + 1),
			FTM_CnSC_CHIE_MASK);
}

//==========================================================================
//��������: ftm_decap_get_int
//��������: true:�����ж�; false:δ�����ж�
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//         ch_group:FTMģ���ͨ�����:
//                  FTM_CH_GROUPx��xΪͨ�����;
//���ܸ�Ҫ: ��ȡ˫���ز�׽���ܵ�ͨ������жϱ�־
//==========================================================================
bool ftm_decap_get_int(uint8 mod, uint8 ch_group) {
	return (REG_GET_MASK(FTM_CnSC_REG(ftm_table[mod],(ch_group << 1) + 1),
			FTM_CnSC_CHF_MASK)) ? true : false;
}

//==========================================================================
//��������: ftm_decap_clear_int
//��������: ��
//����˵��: mod:FTMģ���:
//             FTM_MODx��xΪģ���;
//         ch_group:FTMģ���ͨ�����:
//                  FTM_CH_GROUPx��xΪͨ�����;
//���ܸ�Ҫ: ���˫���ز�׽���ܵ�ͨ������жϱ�־
//==========================================================================
void ftm_decap_clear_int(uint8 mod, uint8 ch_group) {
	REG_CLR_MASK(FTM_CnSC_REG(ftm_table[mod],ch_group << 1), FTM_CnSC_CHF_MASK);
	REG_CLR_MASK(FTM_CnSC_REG(ftm_table[mod],(ch_group << 1) + 1),
			FTM_CnSC_CHF_MASK);
}
