//==========================================================================
//�ļ����ƣ�pit.h
//���ܸ�Ҫ��K64 PIT�ײ���������Դ�ļ�
//==========================================================================

#include "pit.h"

//PITģ���ͨ�����ж������
static const IRQn_Type pit_irq[4] =
		{ PIT0_IRQn, PIT1_IRQn, PIT2_IRQn, PIT3_IRQn };

//==========================================================================
//��������: pit_init
//��������: ��
//����˵��: PIT_CHx:PITͨ���ţ���ʹ�ú궨��
//       ms:��ͨ�������жϵ�ʱ�����ڣ���λms����Χ0-71582ms
//���ܸ�Ҫ: PITģ��ͨ����ʼ��
//==========================================================================
void pit_init(uint8 PIT_CHx, uint32 ms) {
	uint32 load_value;		//��ͨ������ֵ�Ĵ�����ֵ

	REG_SET_MASK(SIM_SCGC6, SIM_SCGC6_PIT_MASK);	//��PITʱ����
	REG_CLR_MASK(PIT_MCR, PIT_MCR_MDIS_MASK);		//PITģ�鶨ʱ��ʱ��ʹ��
	REG_SET_MASK(PIT_MCR, PIT_MCR_FRZ_MASK);		//�ڵ���ģʽ�¶�ʱ��ֹͣ
	load_value = PIT_WORK_FREQ * ms;	//���ݲ����жϵ�ʱ�����ڼ������ֵ�Ĵ�����ֵ
	REG_SET_VAL(PIT_LDVAL(PIT_CHx), load_value);	//���ø�ͨ������ֵ�Ĵ�����ֵ
	REG_SET_MASK(PIT_TCTRL(PIT_CHx), PIT_TCTRL_TEN_MASK);	//��ͨ����ʱ��ʱ��ʹ��
	REG_CLR_MASK(PIT_TCTRL(PIT_CHx), PIT_TCTRL_TIE_MASK);	//��ֹ��ͨ�������ж�����
}

//==========================================================================
//��������: pit_enable_int
//��������: ��                                              
//����˵��: PIT_CHx:PITͨ���ţ���ʹ�ú궨��
//���ܸ�Ҫ: ʹ��PITͨ���ж�
//==========================================================================
void pit_enable_int(uint8 PIT_CHx) {
	REG_SET_MASK(PIT_TFLG(PIT_CHx), PIT_TFLG_TIF_MASK);	//�������ͨ���жϱ�־���Է�ֹʹ���жϺ����������ж�
	REG_SET_MASK(PIT_TCTRL(PIT_CHx), PIT_TCTRL_TIE_MASK);	//�����ͨ�������ж�����
	ENABLE_IRQ(pit_irq[PIT_CHx]);							//������ո�ͨ�����͵��ж�����
}

//==========================================================================
//��������: pit_disable_int
//��������: ��                                              
//����˵��: PIT_CHx:PITͨ���ţ���ʹ�ú궨��
//���ܸ�Ҫ: �ر�PITͨ���ж�
//==========================================================================
void pit_disable_int(uint8 PIT_CHx) {
	REG_CLR_MASK(PIT_TCTRL(PIT_CHx), PIT_TCTRL_TIE_MASK);	//��ֹ��ͨ�������ж�����
	DISABLE_IRQ(pit_irq[PIT_CHx]);							//��ֹ���ո�ͨ�����͵��ж�����
}

//==========================================================================
//��������: pit_get_int
//��������: PIT_INT_TRUE:��ͨ�������жϣ�PIT_INT_FALSE:��ͨ��δ�����ж�
//����˵��: PIT_CHx:PITͨ���ţ���ʹ�ú궨��
//���ܸ�Ҫ: ��ȡPITͨ���жϱ�־
//==========================================================================
uint8 pit_get_int(uint8 PIT_CHx) {
	//��ȡ��ͨ����ʱ����־�Ĵ������жϱ�־��ֵ��������ֵ�ж��Ƿ�����ж�
	return REG_GET_MASK(PIT_TFLG(PIT_CHx),
			PIT_TFLG_TIF_MASK) == PIT_TFLG_TIF_MASK ?
			PIT_INT_TRUE : PIT_INT_FALSE;
}

//==========================================================================
//��������: pit_disable_int
//��������: ��
//����˵��: PIT_CHx:PITͨ���ţ���ʹ�ú궨��
//���ܸ�Ҫ: ���PITͨ���жϱ�־
//==========================================================================
void pit_clear_int(uint8 PIT_CHx) {
	REG_SET_MASK(PIT_TFLG(PIT_CHx), PIT_TFLG_TIF_MASK);	//�����ͨ���жϱ�־
}
