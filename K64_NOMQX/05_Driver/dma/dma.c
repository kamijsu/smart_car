//==========================================================================
//文件名称：dma.c
//功能概要：K64 DMA底层驱动程序源文件
//==========================================================================

#include "dma.h"

//DMA各通道中断请求号
static const IRQn_Type dma_irq_table[] = { DMA0_IRQn, DMA1_IRQn, DMA2_IRQn,
		DMA3_IRQn, DMA4_IRQn, DMA5_IRQn, DMA6_IRQn, DMA7_IRQn, DMA8_IRQn,
		DMA9_IRQn, DMA10_IRQn, DMA11_IRQn, DMA12_IRQn, DMA13_IRQn, DMA14_IRQn,
		DMA15_IRQn };

void dma_init(uint8 ch, uint8 req, uint8 mode, uint32 minor_loop_bytes,
		uint16 major_loop_iteration_cnt, uint32 src_addr, uint8 src_data_width,
		int16 src_addr_offset, uint8 src_modulo, int32 src_addr_last_adj,
		uint32 dest_addr, uint8 dest_data_width, int16 dest_addr_offset,
		uint8 dest_modulo, int32 dest_addr_last_adj, bool auto_disable_req) {

	//开启DMA和DMAMUX模块时钟门
	REG_SET_MASK(SIM_SCGC6, SIM_SCGC6_DMAMUX_MASK);
	REG_SET_MASK(SIM_SCGC7, SIM_SCGC7_DMA_MASK);

	//默认不接收该通道的DMA请求
	REG_SET_VAL(DMA_CERQ, DMA_CERQ_CERQ(ch));

	//暂时关闭该DMA通道，并设置该通道的请求源，默认触发模式为正常模式
	REG_SET_VAL(DMAMUX_CHCFG(ch), DMAMUX_CHCFG_SOURCE(req));

	//若触发模式为周期触发模式
	if (mode == DMA_MODE_PERIODIC) {
		//更改为周期触发模式
		REG_SET_MASK(DMAMUX_CHCFG(ch), DMAMUX_CHCFG_TRIG_MASK);
	}

	//重置CSR，关闭主循环中断，关闭主循环通道连接
	if (auto_disable_req) {
		//额外使能主循环完成后自动不接收DMA请求
		REG_SET_VAL(DMA_CSR(ch), DMA_CSR_DREQ_MASK);
	} else {
		REG_SET_VAL(DMA_CSR(ch), 0);
	}

	//设置每次副循环传输字节数
	REG_SET_VAL(DMA_NBYTES_MLNO(ch), DMA_NBYTES_MLNO_NBYTES(minor_loop_bytes));

	//设置主循环迭代次数，默认关闭副循环通道连接
	REG_SET_VAL(DMA_CITER_ELINKNO(ch),
			DMA_CITER_ELINKNO_CITER(major_loop_iteration_cnt));
	REG_SET_VAL(DMA_BITER_ELINKNO(ch),
			DMA_BITER_ELINKNO_BITER(major_loop_iteration_cnt));

	//设置源地址和目标地址的模数大小和数据宽度
	REG_SET_VAL(DMA_ATTR(ch),
			DMA_ATTR_SMOD(src_modulo)|DMA_ATTR_SSIZE(src_data_width)|DMA_ATTR_DMOD(dest_modulo)|DMA_ATTR_DSIZE(dest_data_width));

	//设置源地址
	REG_SET_VAL(DMA_SADDR(ch), DMA_SADDR_SADDR(src_addr));
	//设置源地址每次被读取后的偏移量
	REG_SET_VAL(DMA_SOFF(ch), DMA_SOFF_SOFF(src_addr_offset));
	//设置主循环完成后源地址的偏移量
	REG_SET_VAL(DMA_SLAST(ch), DMA_SLAST_SLAST(src_addr_last_adj));

	//设置目标地址
	REG_SET_VAL(DMA_DADDR(ch), DMA_DADDR_DADDR(dest_addr));
	//设置目标地址每次被写入后的偏移量
	REG_SET_VAL(DMA_DOFF(ch), DMA_DOFF_DOFF(dest_addr_offset));
	//设置主循环完成后目标地址的偏移量
	REG_SET_VAL(DMA_DLAST_SGA(ch), DMA_DLAST_SGA_DLASTSGA(dest_addr_last_adj));

	//使能该DMA通道
	REG_SET_MASK(DMAMUX_CHCFG(ch), DMAMUX_CHCFG_ENBL_MASK);
}

void dma_enable_req(uint8 ch) {
	//接收该通道的DMA请求
	REG_SET_VAL(DMA_SERQ, DMA_SERQ_SERQ(ch));
}

void dma_disable_req(uint8 ch) {
	//不接收该通道的DMA请求
	REG_SET_VAL(DMA_CERQ, DMA_CERQ_CERQ(ch));
}

void dma_enable_major_int(uint8 ch) {
	//清除主循环完成中断的中断标志
	REG_SET_VAL(DMA_CINT, DMA_CINT_CINT(ch));
	//使能主循环完成中断
	REG_SET_MASK(DMA_CSR(ch), DMA_CSR_INTMAJOR_MASK);
	//允许接收该通道发送的中断请求
	ENABLE_IRQ(dma_irq_table[ch]);
}

void dma_disable_major_int(uint8 ch) {
	//关闭主循环完成中断
	REG_CLR_MASK(DMA_CSR(ch), DMA_CSR_INTMAJOR_MASK);
	//禁止接收该通道发送的中断请求
	DISABLE_IRQ(dma_irq_table[ch]);
}

//仅当该通道使能中断时，中断标志才会置位
bool dma_get_major_int(uint8 ch) {
	//获取主循环完成中断的中断标志
	return REG_GET_SHIFT(DMA_INT,ch) ? true : false;
}

void dma_clear_major_int(uint8 ch) {
	//清除主循环完成中断的中断标志
	REG_SET_VAL(DMA_CINT, DMA_CINT_CINT(ch));
}

void dma_software_req(uint8 ch) {
	//软件触发一次DMA请求
	REG_SET_MASK(DMA_CSR(ch), DMA_CSR_START_MASK);
}
