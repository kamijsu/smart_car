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

//每次副循环传输字节数必须为源地址和目标地址数据宽度的倍数;
//地址必须与数据宽度匹配，若数据宽度为4字节，地址必须4字节对齐，即地址为4的倍数;
//地址每次被读取/写入后的偏移量必须为数据宽度的倍数;
//
//一次主循环由若干次副循环组成，一次副循环由若干次读写组成，每次读写中，
//读次数和写次数的对应关系可以为一对一，也可以为多对一和一对多，这由源地址和目标地址的数据宽度决定，
//若源地址数据宽度为2字节，目标地址数据宽度为4字节，读2次源地址，再写1次目标地址，组成一次读写，
//在这次读写中，源地址偏移2次，目标地址偏移1次;
//
//使能模数功能后，会依照模数大小，使得地址在偏移时，仅有低位可以变化，而高位不会变化，
//若模数大小为2的n次方，则可变化的位数为n，如模数大小为16个字节，初始地址为0x12345670，
//仅低4位可以变化，而0x1234567x不会变化;
//但若主循环完成后的偏移使得地址的高位发生变化，在下一次主循环中，将会保证变化后的高位不变;
//可以利用模数功能构件一个循环队列;
//
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

//即使发生中断，若DMA请求被触发，且该通道接收DMA请求，数据传输仍会进行
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

//无论是否接收该通道的DMA请求，软件触发的DMA请求一定被接收
void dma_software_req(uint8 ch) {
	//软件触发一次DMA请求
	REG_SET_VAL(DMA_SSRT, DMA_SSRT_SSRT(ch));
}

//获取主循环剩余迭代次数
uint16 dma_get_major_loop_iteration_cnt(uint8 ch) {
	//查看是否使能副循环通道连接
	if (REG_GET_MASK(DMA_CITER_ELINKNO(ch), DMA_CITER_ELINKNO_ELINK_MASK)) {
		//使能副循环通道连接
		return REG_GET_MASK(DMA_CITER_ELINKYES(ch),
				DMA_CITER_ELINKYES_CITER_MASK) >> DMA_CITER_ELINKYES_CITER_SHIFT;
	} else {
		//关闭副循环通道连接
		return REG_GET_MASK(DMA_CITER_ELINKNO(ch), DMA_CITER_ELINKNO_CITER_MASK)
				>> DMA_CITER_ELINKNO_CITER_SHIFT;
	}
}

uint32 dma_get_src_addr(uint8 ch) {
	//返回当前源地址
	return DMA_SADDR(ch);
}

void dma_set_src_addr(uint8 ch, uint32 src_addr) {
	//设置源地址
	REG_SET_VAL(DMA_SADDR(ch), DMA_SADDR_SADDR(src_addr));
}

uint32 dma_get_dest_addr(uint8 ch) {
	//返回当前目标地址
	return DMA_DADDR(ch);
}

void dma_set_dest_addr(uint8 ch, uint32 dest_addr) {
	//设置目标地址
	REG_SET_VAL(DMA_DADDR(ch), DMA_DADDR_DADDR(dest_addr));
}

void dma_set_auto_disable_req(uint8 ch, bool enable) {
	if (enable) {
		//使能主循环完成后自动不接收DMA请求
		REG_SET_MASK(DMA_CSR(ch), DMA_CSR_DREQ_MASK);
	} else {
		//关闭主循环完成后自动不接收DMA请求
		REG_CLR_MASK(DMA_CSR(ch), DMA_CSR_DREQ_MASK);
	}
}
