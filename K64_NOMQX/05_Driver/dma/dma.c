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
//��ͨ�����ȼ��Ĵ�����ַ
static vuint8* const dchpri_table[] = { &DMA_DCHPRI0, &DMA_DCHPRI1,
		&DMA_DCHPRI2, &DMA_DCHPRI3, &DMA_DCHPRI4, &DMA_DCHPRI5, &DMA_DCHPRI6,
		&DMA_DCHPRI7, &DMA_DCHPRI8, &DMA_DCHPRI9, &DMA_DCHPRI10, &DMA_DCHPRI11,
		&DMA_DCHPRI12, &DMA_DCHPRI13, &DMA_DCHPRI14, &DMA_DCHPRI15 };

//==========================================================================
//��������: dma_init
//��������: ��
//����˵��: ch:DMAͨ����:
//            DMA_CHx��xΪͨ����;
//         req:��ͨ��������Դ:
//             DMA_REQ_xxx�������ͷ�ļ��еĺ궨��;
//         mode:��ͨ���Ĵ���ģʽ:
//              DMA_MODE_NORMAL:  ����ģʽ;
//              DMA_MODE_PERIODIC:���ڴ���ģʽ;
//         minor_loop_bytes:ÿ�θ�ѭ�������ֽ�����ȡֵ��ΧΪ[1,4294967295]
//         major_loop_iteration_cnt:��ѭ��������������һ����ѭ���и�ѭ���Ĵ�����
//                                  ȡֵ��ΧΪ[1,32767]������Ҫʹ�ܸ�ѭ��ͨ�����ӣ�
//                                  ȡֵ��Χ��Ϊ[1,511]
//         src_addr:Դ��ַ
//         src_data_width:Դ��ַ���ݿ�ȣ���ÿ�δ�Դ��ַ��ȡ���ֽ���:
//                        DMA_DATA_WIDTH_BYTE_x�����ݿ��Ϊx���ֽڣ�x��ȡֵΪ:
//                        1 | 2 | 4 | 16 | 32
//         src_addr_offset:Դ��ַÿ�α���ȡ���ƫ������ȡֵ��ΧΪ[-32768,32767]
//         src_modulo:Դ��ַģ����С:
//                    DMA_MODULO_DISABLED:����ģ������;
//                    DMA_MODULO_BYTE_x:ģ����СΪx���ֽڣ�x��ȡֵΪ:
//                    2  | 4  | 8  | 16 | 32  | 64  | 128 | 256  | 512  |
//                    1K | 2K | 4K | 8K | 16K | 32K | 64K | 128K | 256K | 512K |
//                    1M | 2M | 4M | 8M | 16M | 32M | 64M | 128M | 256M | 512M |
//                    1G | 2G
//         src_addr_last_adj:��ѭ����ɺ�Դ��ַ��ƫ������
//                           ȡֵ��ΧΪ[-2147483648,2147483647]
//         dest_addr:Ŀ���ַ
//         dest_data_width:Ŀ���ַ���ݿ�ȣ���ÿ����Ŀ���ַд����ֽ�����
//                         ��ѡ����ͬsrc_data_width
//         dest_addr_offset:Ŀ���ַÿ�α�д����ƫ������ȡֵ��Χͬsrc_addr_offset
//         dest_modulo:Ŀ���ַģ����С����ѡ����ͬsrc_modulo
//         dest_addr_last_adj:��ѭ����ɺ�Ŀ���ַ��ƫ������
//                            ȡֵ��Χͬsrc_addr_last_adj
//         auto_disable_req:�Ƿ�ʹ����ѭ����ɺ��Զ�������DMA����:
//                          true: ʹ����ѭ����ɺ��Զ�������DMA����;
//                          false:�ر���ѭ����ɺ��Զ�������DMA����;
//���ܸ�Ҫ: ��ʼ��DMAͨ����Ĭ�Ϲر���ѭ������жϣ�Ĭ�ϲ����ո�ͨ����DMA����
//         Ĭ�Ϲر���ѭ���͸�ѭ��ͨ������
//��ע: һ��DMA���󱻽��պ󣬽���һ�θ�ѭ��;
//
//     ��ͬͨ����Ҫ������ͬ������Դ�������ɲ���֪�Ľ��(�ر�����Դ��ʼ�մ�������)��
//     ����Դ��Ҫ���Դ���DMA������ʹ����Ӧ����Դ��DMA����;
//
//     ����ģʽ�£�����Դÿ����һ�Σ�����һ��DMA����;
//     ���ڴ���ģʽ�£�ÿһ�����ڴ���һ�Σ�������ʱ����ԴҲ��������������һ��DMA����
//     ������PITͨ�����ж����ھ�������DMA0-DMA3ͨ������ʹ�����ڴ���ģʽ��
//     ��Ӧ��PITͨ��ΪPIT0-PIT3;
//
//     ÿ�θ�ѭ�������ֽ���(minor_loop_bytes)����ΪԴ��ַ��Ŀ���ַ���ݿ�ȵı���;
//     ��ַ���������ݿ��ƥ�䣬�����ݿ��Ϊ4�ֽڣ���ַ����4�ֽڶ��룬����ַΪ4�ı���;
//     ��ַÿ�α���ȡ/д����ƫ��������Ϊ���ݿ�ȵı���;
//
//     һ����ѭ�������ɴθ�ѭ����ɣ�һ�θ�ѭ�������ɴζ�д��ɣ�ÿ�ζ�д�У�
//     ��������д�����Ķ�Ӧ��ϵ����Ϊһ��һ��Ҳ����Ϊ���һ��һ�Զ࣬
//     ����Դ��ַ��Ŀ���ַ�����ݿ�Ⱦ�������Դ��ַ���ݿ��Ϊ2�ֽڣ�
//     Ŀ���ַ���ݿ��Ϊ4�ֽڣ���2��Դ��ַ����д1��Ŀ���ַ�����һ�ζ�д��
//     ����ζ�д�У�Դ��ַƫ��2�Σ�Ŀ���ַƫ��1��;
//
//     ʹ��ģ�����ܺ󣬻�����ģ����С��ʹ�õ�ַ��ƫ��ʱ�����е�λ���Ա仯��
//     ����λ����仯����ģ����СΪ2��n�η�����ɱ仯��λ��Ϊn��
//     ��ģ����СΪ16���ֽڣ���ʼ��ַΪ0x12345670������4λ���Ա仯��
//     ��0x1234567x����仯;
//     ������ѭ����ɺ��ƫ��ʹ�õ�ַ�ĸ�λ�����仯������һ����ѭ���У�
//     ���ᱣ֤�仯��ĸ�λ����;
//     ��������ģ�����ܹ���һ��ѭ������;
//==========================================================================
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

//==========================================================================
//��������: dma_enable_req
//��������: ��
//����˵��: ch:DMAͨ����:
//            DMA_CHx��xΪͨ����;
//���ܸ�Ҫ: ���ո�ͨ����DMA����
//==========================================================================
void dma_enable_req(uint8 ch) {
	//���ո�ͨ����DMA����
	REG_SET_VAL(DMA_SERQ, DMA_SERQ_SERQ(ch));
}

//==========================================================================
//��������: dma_disable_req
//��������: ��
//����˵��: ch:DMAͨ����:
//            DMA_CHx��xΪͨ����;
//���ܸ�Ҫ: �����ո�ͨ����DMA����
//==========================================================================
void dma_disable_req(uint8 ch) {
	//�����ո�ͨ����DMA����
	REG_SET_VAL(DMA_CERQ, DMA_CERQ_CERQ(ch));
}

//==========================================================================
//��������: dma_enable_major_int
//��������: ��
//����˵��: ch:DMAͨ����:
//            DMA_CHx��xΪͨ����;
//���ܸ�Ҫ: ʹ����ѭ������жϣ���һ����ѭ�����ʱ�������ж�
//��ע: ��ʹ�����жϣ���DMA���󱻴������Ҹ�ͨ������DMA�������ݴ����Ի����
//==========================================================================
void dma_enable_major_int(uint8 ch) {
	//�����ѭ������жϵ��жϱ�־
	REG_SET_VAL(DMA_CINT, DMA_CINT_CINT(ch));
	//ʹ����ѭ������ж�
	REG_SET_MASK(DMA_CSR(ch), DMA_CSR_INTMAJOR_MASK);
	//������ո�ͨ�����͵��ж�����
	ENABLE_IRQ(dma_irq_table[ch]);
}

//==========================================================================
//��������: dma_disable_major_int
//��������: ��
//����˵��: ch:DMAͨ����:
//            DMA_CHx��xΪͨ����;
//���ܸ�Ҫ: �ر���ѭ������ж�
//==========================================================================
void dma_disable_major_int(uint8 ch) {
	//�ر���ѭ������ж�
	REG_CLR_MASK(DMA_CSR(ch), DMA_CSR_INTMAJOR_MASK);
	//��ֹ���ո�ͨ�����͵��ж�����
	DISABLE_IRQ(dma_irq_table[ch]);
}

//==========================================================================
//��������: dma_get_major_int
//��������: true:�����ж�; false:δ�����ж�
//����˵��: ch:DMAͨ����:
//            DMA_CHx��xΪͨ����;
//���ܸ�Ҫ: ��ȡ��ѭ������жϵ��жϱ�־
//��ע: ������ͨ��ʹ���ж�ʱ���жϱ�־�Ż���λ
//==========================================================================
bool dma_get_major_int(uint8 ch) {
	//��ȡ��ѭ������жϵ��жϱ�־
	return REG_GET_SHIFT(DMA_INT,ch) ? true : false;
}

//==========================================================================
//��������: dma_clear_major_int
//��������: ��
//����˵��: ch:DMAͨ����:
//            DMA_CHx��xΪͨ����;
//���ܸ�Ҫ: �����ѭ������жϵ��жϱ�־
//==========================================================================
void dma_clear_major_int(uint8 ch) {
	//�����ѭ������жϵ��жϱ�־
	REG_SET_VAL(DMA_CINT, DMA_CINT_CINT(ch));
}

//==========================================================================
//��������: dma_software_req
//��������: ��
//����˵��: ch:DMAͨ����:
//            DMA_CHx��xΪͨ����;
//���ܸ�Ҫ: �������һ��DMA����
//��ע: �����Ƿ���ո�ͨ����DMA�������������DMA����һ��������;
//     ��ͨ��״̬Ϊ������������ʱ���������DMA����ᶪʧ
//==========================================================================
void dma_software_req(uint8 ch) {
	//�������һ��DMA����
	REG_SET_VAL(DMA_SSRT, DMA_SSRT_SSRT(ch));
}

//==========================================================================
//��������: dma_get_ch_state
//��������: ͨ��״̬:
//         DMAChannelIdle:     ͨ������;
//         DMAChannelStarting: ͨ����������;
//         DMAChannelExecuting:ͨ������ִ��;
//         DMAChannelDone:     ͨ�������ѭ��;
//����˵��: ch:DMAͨ����:
//            DMA_CHx��xΪͨ����;
//���ܸ�Ҫ: ��ȡͨ��״̬
//��ע: ͨ��״̬��Ϊ���У���ʱ��������Դ������һ��DMA���󣬵���δִ�У�
//     Ҳ����ͨ���Ѿ������һ�θ�ѭ��������δ�����ѭ��;
//
//     ͨ��״̬��Ϊ������������ʱ�������������һ��DMA����
//     Ҳ������ͨ�����ӱ�������һ��DMA���󣬵���δִ��;
//
//     ͨ��״̬��Ϊ����ִ�У���ʱ����ִ�и�ͨ���ĸ�ѭ��;
//
//     ͨ��״̬��Ϊ�����ѭ������ʱ��ͨ���Ѿ������һ����ѭ��
//==========================================================================
DMAChannelState dma_get_ch_state(uint8 ch) {
	uint16 state;	//ͨ�����ƺ�״̬�Ĵ�����ֵ

	//��ȡͨ�����ƺ�״̬�Ĵ�����ֵ
	state = DMA_CSR(ch);

	if (REG_GET_MASK(state, DMA_CSR_START_MASK)) {
		//����ͨ����������
		return DMAChannelStarting;
	}
	if (REG_GET_MASK(state, DMA_CSR_ACTIVE_MASK)) {
		//����ͨ������ִ��
		return DMAChannelExecuting;
	}
	if (REG_GET_MASK(state, DMA_CSR_DONE_MASK)) {
		//����ͨ�������ѭ��
		return DMAChannelDone;
	}
	//����ͨ������
	return DMAChannelIdle;
}

//==========================================================================
//��������: dma_get_major_loop_iteration_cnt
//��������: ��ѭ��ʣ���������
//����˵��: ch:DMAͨ����:
//            DMA_CHx��xΪͨ����;
//���ܸ�Ҫ: ��ȡ��ѭ��ʣ���������
//==========================================================================
uint16 dma_get_major_loop_iteration_cnt(uint8 ch) {
	//�鿴�Ƿ�ʹ�ܸ�ѭ��ͨ������
	if (REG_GET_MASK(DMA_CITER_ELINKNO(ch), DMA_CITER_ELINKNO_ELINK_MASK)) {
		//ʹ�ܸ�ѭ��ͨ������ʱ
		return REG_GET_MASK(DMA_CITER_ELINKYES(ch),
				DMA_CITER_ELINKYES_CITER_MASK) >> DMA_CITER_ELINKYES_CITER_SHIFT;
	} else {
		//�رո�ѭ��ͨ������ʱ
		return REG_GET_MASK(DMA_CITER_ELINKNO(ch), DMA_CITER_ELINKNO_CITER_MASK)
				>> DMA_CITER_ELINKNO_CITER_SHIFT;
	}
}

//==========================================================================
//��������: dma_get_src_addr
//��������: ��ǰԴ��ַ
//����˵��: ch:DMAͨ����:
//            DMA_CHx��xΪͨ����;
//���ܸ�Ҫ: ��ȡ��ǰԴ��ַ
//==========================================================================
uint32 dma_get_src_addr(uint8 ch) {
	//���ص�ǰԴ��ַ
	return DMA_SADDR(ch);
}

//==========================================================================
//��������: dma_set_src_addr
//��������: ��
//����˵��: ch:DMAͨ����:
//            DMA_CHx��xΪͨ����;
//         src_addr:��ǰԴ��ַ�����������ݿ��ƥ��
//���ܸ�Ҫ: ���õ�ǰԴ��ַ
//��ע: ������Ϊ�����ո�ͨ����DMA������ٽ�������
//==========================================================================
void dma_set_src_addr(uint8 ch, uint32 src_addr) {
	//����Դ��ַ
	REG_SET_VAL(DMA_SADDR(ch), DMA_SADDR_SADDR(src_addr));
}

//==========================================================================
//��������: dma_get_dest_addr
//��������: ��ǰĿ���ַ
//����˵��: ch:DMAͨ����:
//            DMA_CHx��xΪͨ����;
//���ܸ�Ҫ: ��ȡ��ǰĿ���ַ
//==========================================================================
uint32 dma_get_dest_addr(uint8 ch) {
	//���ص�ǰĿ���ַ
	return DMA_DADDR(ch);
}

//==========================================================================
//��������: dma_set_dest_addr
//��������: ��
//����˵��: ch:DMAͨ����:
//            DMA_CHx��xΪͨ����;
//         dest_addr:��ǰĿ���ַ�����������ݿ��ƥ��
//���ܸ�Ҫ: ���õ�ǰĿ���ַ
//��ע: ������Ϊ�����ո�ͨ����DMA������ٽ�������
//==========================================================================
void dma_set_dest_addr(uint8 ch, uint32 dest_addr) {
	//����Ŀ���ַ
	REG_SET_VAL(DMA_DADDR(ch), DMA_DADDR_DADDR(dest_addr));
}

//==========================================================================
//��������: dma_set_auto_disable_req
//��������: ��
//����˵��: ch:DMAͨ����:
//            DMA_CHx��xΪͨ����;
//         enable:�Ƿ�ʹ����ѭ����ɺ��Զ�������DMA����:
//                true: ʹ����ѭ����ɺ��Զ�������DMA����;
//                false:�ر���ѭ����ɺ��Զ�������DMA����;
//���ܸ�Ҫ: �����Ƿ�ʹ����ѭ����ɺ��Զ�������DMA����
//��ע: ������Ϊ�����ո�ͨ����DMA������ٽ�������
//==========================================================================
void dma_set_auto_disable_req(uint8 ch, bool enable) {
	if (enable) {
		//ʹ����ѭ����ɺ��Զ�������DMA����
		REG_SET_MASK(DMA_CSR(ch), DMA_CSR_DREQ_MASK);
	} else {
		//�ر���ѭ����ɺ��Զ�������DMA����
		REG_CLR_MASK(DMA_CSR(ch), DMA_CSR_DREQ_MASK);
	}
}

//==========================================================================
//��������: dma_set_minor_link
//��������: ��
//����˵��: ch:DMAͨ����:
//            DMA_CHx��xΪͨ����;
//         enable:�Ƿ�ʹ�ܸ�ѭ��ͨ������:
//                true: ʹ�ܸ�ѭ��ͨ������;
//                false:�رո�ѭ��ͨ������;
//         link_ch:����ͨ����ͨ���ţ��ò�������enableΪtrueʱ��Ч:
//                 DMA_CHx��xΪͨ����;
//���ܸ�Ҫ: �����Ƿ�ʹ�ܸ�ѭ��ͨ������
//��ע: ������Ϊ�����ո�ͨ����DMA������ٽ�������;
//     ����ͨ����һ�θ�ѭ����ɺ��Զ��������һ������ͨ����DMA����;
//     ���˴θ�ѭ����ɺ������ѭ���������ᴥ������ͨ����DMA����
//==========================================================================
void dma_set_minor_link(uint8 ch, bool enable, uint8 link_ch) {
	//�������ͨ��
	REG_CLR_MASK(DMA_CITER_ELINKYES(ch), DMA_CITER_ELINKYES_LINKCH_MASK);
	REG_CLR_MASK(DMA_BITER_ELINKYES(ch), DMA_BITER_ELINKYES_LINKCH_MASK);
	if (enable) {
		//ʹ�ܸ�ѭ��ͨ�����ӣ�����������ͨ��
		REG_SET_MASK(DMA_CITER_ELINKYES(ch),
				DMA_CITER_ELINKYES_ELINK_MASK|DMA_CITER_ELINKYES_LINKCH(link_ch));
		REG_SET_MASK(DMA_BITER_ELINKYES(ch),
				DMA_BITER_ELINKYES_ELINK_MASK|DMA_BITER_ELINKYES_LINKCH(link_ch));
	} else {
		//�رո�ѭ��ͨ������
		REG_CLR_MASK(DMA_CITER_ELINKNO(ch), DMA_CITER_ELINKNO_ELINK_MASK);
		REG_CLR_MASK(DMA_BITER_ELINKNO(ch), DMA_BITER_ELINKNO_ELINK_MASK);
	}
}

//==========================================================================
//��������: dma_set_major_link
//��������: ��
//����˵��: ch:DMAͨ����:
//            DMA_CHx��xΪͨ����;
//         enable:�Ƿ�ʹ����ѭ��ͨ������:
//                true: ʹ����ѭ��ͨ������;
//                false:�ر���ѭ��ͨ������;
//         link_ch:����ͨ����ͨ���ţ��ò�������enableΪtrueʱ��Ч:
//                 DMA_CHx��xΪͨ����;
//���ܸ�Ҫ: �����Ƿ�ʹ����ѭ��ͨ������
//��ע: ������Ϊ�����ո�ͨ����DMA������ٽ�������;
//     ����ͨ����һ����ѭ����ɺ��Զ��������һ������ͨ����DMA����
//==========================================================================
void dma_set_major_link(uint8 ch, bool enable, uint8 link_ch) {
	//�����ͨ����ɱ�־
	REG_SET_VAL(DMA_CDNE, DMA_CDNE_CDNE(ch));
	if (enable) {
		//�������ͨ��
		REG_CLR_MASK(DMA_CSR(ch), DMA_CSR_MAJORLINKCH_MASK);
		//ʹ����ѭ��ͨ�����ӣ�����������ͨ��
		REG_SET_MASK(DMA_CSR(ch),
				DMA_CSR_MAJORELINK_MASK|DMA_CSR_MAJORLINKCH(link_ch));
	} else {
		//�ر���ѭ��ͨ������
		REG_CLR_MASK(DMA_CSR(ch), DMA_CSR_MAJORELINK_MASK);
	}
}

//==========================================================================
//��������: dma_set_arbitration_mode
//��������: ��
//����˵��: arbitration_mode:�ٲ�ģʽ:
//                          DMA_ARBITRATION_MODE_FIXED:      �̶����ȼ��ٲ�;
//                          DMA_ARBITRATION_MODE_ROUND_ROBIN:��ѯ�ٲ�;
//���ܸ�Ҫ: �����ٲ�ģʽ
//��ע: Ĭ��Ϊ�̶����ȼ��ٲ�;
//
//     �̶����ȼ��ٲ�ʱ����ͬʱ�ж��ͨ����DMA���󱻴���������ͨ�������ȼ���
//     ����Ӧ���ȼ���ߵ�ͨ����0-15���ȼ��ӵ͵��ߣ�0Ϊ������ȼ���15Ϊ������ȼ�;
//
//     ��ѯ�ٲ�ʱ����15��ͨ����0ͨ�����β�ѯͨ���Ƿ���DMA�������У�����Ӧ������
//     ���ޣ��������ѯ��һ��ͨ�����ظ��ù���
//==========================================================================
void dma_set_arbitration_mode(uint8 arbitration_mode) {
	if (arbitration_mode == DMA_ARBITRATION_MODE_ROUND_ROBIN) {
		//��ѯ�ٲ�
		REG_SET_MASK(DMA_CR, DMA_CR_ERCA_MASK);
	} else {
		//�̶����ȼ��ٲ�
		REG_CLR_MASK(DMA_CR, DMA_CR_ERCA_MASK);
	}
}

//==========================================================================
//��������: dma_get_priority
//��������: ��ͨ�������ȼ���ȡֵ��ΧΪ[0,15]
//����˵��: ch:DMAͨ����:
//            DMA_CHx��xΪͨ����;
//���ܸ�Ҫ: ��ȡ��ͨ�������ȼ�
//==========================================================================
uint8 dma_get_priority(uint8 ch) {
	//��ȡ��ͨ�������ȼ�
	return REG_GET_MASK(*dchpri_table[ch], DMA_DCHPRI0_CHPRI_MASK)
			>> DMA_DCHPRI0_CHPRI_SHIFT;
}

void dma_set_priority(uint8 ch, uint8 priority) {
	//���ԭ�����ȼ�
	REG_CLR_MASK(*dchpri_table[ch], DMA_DCHPRI0_CHPRI_MASK);
	//���������ȼ�
	REG_SET_MASK(*dchpri_table[ch], DMA_DCHPRI0_CHPRI(priority));
}

void dma_set_channel_preemption(uint8 ch, bool enable) {
	if (enable) {
		//��ͨ�����Ա���ռ
		REG_SET_MASK(*dchpri_table[ch], DMA_DCHPRI0_ECP_MASK);
	} else {
		//��ͨ�������Ա���ռ
		REG_CLR_MASK(*dchpri_table[ch], DMA_DCHPRI0_ECP_MASK);
	}
}

void dma_set_preempt_ability(uint8 ch, bool enable) {
	if (enable) {
		//��ͨ��������ռ���ͨ��
		REG_CLR_MASK(*dchpri_table[ch], DMA_DCHPRI0_DPA_MASK);
	} else {
		//��ͨ����������ռ���ͨ��
		REG_SET_MASK(*dchpri_table[ch], DMA_DCHPRI0_DPA_MASK);
	}
}
