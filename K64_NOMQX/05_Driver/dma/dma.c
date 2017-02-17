//==========================================================================
//�ļ����ƣ�dma.c
//���ܸ�Ҫ��K64 DMA�ײ���������Դ�ļ�
//==========================================================================

#include "dma.h"

//DMA��ͨ���ж������
static const IRQn_Type dma_irq_table[] = { DMA0_IRQn, DMA1_IRQn, DMA2_IRQn,
		DMA3_IRQn, DMA4_IRQn, DMA5_IRQn, DMA6_IRQn, DMA7_IRQn, DMA8_IRQn,
		DMA9_IRQn, DMA10_IRQn, DMA11_IRQn, DMA12_IRQn, DMA13_IRQn, DMA14_IRQn,
		DMA15_IRQn };

void dma_init(uint8 ch, uint8 req, uint8 mode, uint32 minor_loop_bytes,
		uint16 major_loop_iteration_cnt, uint32 src_addr, uint8 src_data_width,
		int16 src_addr_offset, uint8 src_modulo, int32 src_addr_last_adj,
		uint32 dest_addr, uint8 dest_data_width, int16 dest_addr_offset,
		uint8 dest_modulo, int32 dest_addr_last_adj, bool auto_disable_req) {

	//����DMA��DMAMUXģ��ʱ����
	REG_SET_MASK(SIM_SCGC6, SIM_SCGC6_DMAMUX_MASK);
	REG_SET_MASK(SIM_SCGC7, SIM_SCGC7_DMA_MASK);

	//Ĭ�ϲ����ո�ͨ����DMA����
	REG_SET_VAL(DMA_CERQ, DMA_CERQ_CERQ(ch));

	//��ʱ�رո�DMAͨ���������ø�ͨ��������Դ��Ĭ�ϴ���ģʽΪ����ģʽ
	REG_SET_VAL(DMAMUX_CHCFG(ch), DMAMUX_CHCFG_SOURCE(req));

	//������ģʽΪ���ڴ���ģʽ
	if (mode == DMA_MODE_PERIODIC) {
		//����Ϊ���ڴ���ģʽ
		REG_SET_MASK(DMAMUX_CHCFG(ch), DMAMUX_CHCFG_TRIG_MASK);
	}

	//����CSR���ر���ѭ���жϣ��ر���ѭ��ͨ������
	if (auto_disable_req) {
		//����ʹ����ѭ����ɺ��Զ�������DMA����
		REG_SET_VAL(DMA_CSR(ch), DMA_CSR_DREQ_MASK);
	} else {
		REG_SET_VAL(DMA_CSR(ch), 0);
	}

	//����ÿ�θ�ѭ�������ֽ���
	REG_SET_VAL(DMA_NBYTES_MLNO(ch), DMA_NBYTES_MLNO_NBYTES(minor_loop_bytes));

	//������ѭ������������Ĭ�Ϲرո�ѭ��ͨ������
	REG_SET_VAL(DMA_CITER_ELINKNO(ch),
			DMA_CITER_ELINKNO_CITER(major_loop_iteration_cnt));
	REG_SET_VAL(DMA_BITER_ELINKNO(ch),
			DMA_BITER_ELINKNO_BITER(major_loop_iteration_cnt));

	//����Դ��ַ��Ŀ���ַ��ģ����С�����ݿ��
	REG_SET_VAL(DMA_ATTR(ch),
			DMA_ATTR_SMOD(src_modulo)|DMA_ATTR_SSIZE(src_data_width)|DMA_ATTR_DMOD(dest_modulo)|DMA_ATTR_DSIZE(dest_data_width));

	//����Դ��ַ
	REG_SET_VAL(DMA_SADDR(ch), DMA_SADDR_SADDR(src_addr));
	//����Դ��ַÿ�α���ȡ���ƫ����
	REG_SET_VAL(DMA_SOFF(ch), DMA_SOFF_SOFF(src_addr_offset));
	//������ѭ����ɺ�Դ��ַ��ƫ����
	REG_SET_VAL(DMA_SLAST(ch), DMA_SLAST_SLAST(src_addr_last_adj));

	//����Ŀ���ַ
	REG_SET_VAL(DMA_DADDR(ch), DMA_DADDR_DADDR(dest_addr));
	//����Ŀ���ַÿ�α�д����ƫ����
	REG_SET_VAL(DMA_DOFF(ch), DMA_DOFF_DOFF(dest_addr_offset));
	//������ѭ����ɺ�Ŀ���ַ��ƫ����
	REG_SET_VAL(DMA_DLAST_SGA(ch), DMA_DLAST_SGA_DLASTSGA(dest_addr_last_adj));

	//ʹ�ܸ�DMAͨ��
	REG_SET_MASK(DMAMUX_CHCFG(ch), DMAMUX_CHCFG_ENBL_MASK);
}

void dma_enable_req(uint8 ch) {
	//���ո�ͨ����DMA����
	REG_SET_VAL(DMA_SERQ, DMA_SERQ_SERQ(ch));
}

void dma_disable_req(uint8 ch) {
	//�����ո�ͨ����DMA����
	REG_SET_VAL(DMA_CERQ, DMA_CERQ_CERQ(ch));
}

void dma_enable_major_int(uint8 ch) {
	//�����ѭ������жϵ��жϱ�־
	REG_SET_VAL(DMA_CINT, DMA_CINT_CINT(ch));
	//ʹ����ѭ������ж�
	REG_SET_MASK(DMA_CSR(ch), DMA_CSR_INTMAJOR_MASK);
	//������ո�ͨ�����͵��ж�����
	ENABLE_IRQ(dma_irq_table[ch]);
}

void dma_disable_major_int(uint8 ch) {
	//�ر���ѭ������ж�
	REG_CLR_MASK(DMA_CSR(ch), DMA_CSR_INTMAJOR_MASK);
	//��ֹ���ո�ͨ�����͵��ж�����
	DISABLE_IRQ(dma_irq_table[ch]);
}

//������ͨ��ʹ���ж�ʱ���жϱ�־�Ż���λ
bool dma_get_major_int(uint8 ch) {
	//��ȡ��ѭ������жϵ��жϱ�־
	return REG_GET_SHIFT(DMA_INT,ch) ? true : false;
}

void dma_clear_major_int(uint8 ch) {
	//�����ѭ������жϵ��жϱ�־
	REG_SET_VAL(DMA_CINT, DMA_CINT_CINT(ch));
}

void dma_software_req(uint8 ch) {
	//�������һ��DMA����
	REG_SET_MASK(DMA_CSR(ch), DMA_CSR_START_MASK);
}
