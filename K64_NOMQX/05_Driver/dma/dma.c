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
//各通道优先级寄存器地址
static vuint8* const dchpri_table[] = { &DMA_DCHPRI0, &DMA_DCHPRI1,
		&DMA_DCHPRI2, &DMA_DCHPRI3, &DMA_DCHPRI4, &DMA_DCHPRI5, &DMA_DCHPRI6,
		&DMA_DCHPRI7, &DMA_DCHPRI8, &DMA_DCHPRI9, &DMA_DCHPRI10, &DMA_DCHPRI11,
		&DMA_DCHPRI12, &DMA_DCHPRI13, &DMA_DCHPRI14, &DMA_DCHPRI15 };

//==========================================================================
//函数名称: dma_init
//函数返回: 无
//参数说明: ch:DMA通道号:
//            DMA_CHx，x为通道号;
//         req:该通道的请求源:
//             DMA_REQ_xxx，具体见头文件中的宏定义;
//         mode:该通道的触发模式:
//              DMA_MODE_NORMAL:  正常模式;
//              DMA_MODE_PERIODIC:周期触发模式;
//         minor_loop_bytes:每次副循环传输字节数，取值范围为[1,4294967295]
//         major_loop_iteration_cnt:主循环迭代次数，即一次主循环中副循环的次数，
//                                  取值范围为[1,32767]，但若要使能副循环通道连接，
//                                  取值范围变为[1,511]
//         src_addr:源地址
//         src_data_width:源地址数据宽度，即每次从源地址读取的字节数:
//                        DMA_DATA_WIDTH_BYTE_x，数据宽度为x个字节，x可取值为:
//                        1 | 2 | 4 | 16 | 32
//         src_addr_offset:源地址每次被读取后的偏移量，取值范围为[-32768,32767]
//         src_modulo:源地址模数大小:
//                    DMA_MODULO_DISABLED:禁用模数功能;
//                    DMA_MODULO_BYTE_x:模数大小为x个字节，x可取值为:
//                    2  | 4  | 8  | 16 | 32  | 64  | 128 | 256  | 512  |
//                    1K | 2K | 4K | 8K | 16K | 32K | 64K | 128K | 256K | 512K |
//                    1M | 2M | 4M | 8M | 16M | 32M | 64M | 128M | 256M | 512M |
//                    1G | 2G
//         src_addr_last_adj:主循环完成后源地址的偏移量，
//                           取值范围为[-2147483648,2147483647]
//         dest_addr:目标地址
//         dest_data_width:目标地址数据宽度，即每次向目标地址写入的字节数，
//                         可选参数同src_data_width
//         dest_addr_offset:目标地址每次被写入后的偏移量，取值范围同src_addr_offset
//         dest_modulo:目标地址模数大小，可选参数同src_modulo
//         dest_addr_last_adj:主循环完成后目标地址的偏移量，
//                            取值范围同src_addr_last_adj
//         auto_disable_req:是否使能主循环完成后自动不接收DMA请求:
//                          true: 使能主循环完成后自动不接收DMA请求;
//                          false:关闭主循环完成后自动不接收DMA请求;
//功能概要: 初始化DMA通道，默认关闭主循环完成中断，默认不接收该通道的DMA请求，
//         默认关闭主循环和副循环通道连接
//备注: 一次DMA请求被接收后，进行一次副循环;
//
//     不同通道不要设置相同的请求源，这会造成不可知的结果(关闭请求源和始终触发除外)，
//     请求源若要可以触发DMA请求，需使能相应请求源的DMA功能;
//
//     正常模式下，请求源每触发一次，发起一次DMA请求;
//     周期触发模式下，每一个周期触发一次，若触发时请求源也正被触发，发起一次DMA请求，
//     周期由PIT通道的中断周期决定，仅DMA0-DMA3通道可以使用周期触发模式，
//     对应的PIT通道为PIT0-PIT3;
//
//     每次副循环传输字节数(minor_loop_bytes)必须为源地址和目标地址数据宽度的倍数;
//     地址必须与数据宽度匹配，若数据宽度为4字节，地址必须4字节对齐，即地址为4的倍数;
//     地址每次被读取/写入后的偏移量必须为数据宽度的倍数;
//
//     一次主循环由若干次副循环组成，一次副循环由若干次读写组成，每次读写中，
//     读次数和写次数的对应关系可以为一对一，也可以为多对一和一对多，
//     这由源地址和目标地址的数据宽度决定，若源地址数据宽度为2字节，
//     目标地址数据宽度为4字节，读2次源地址，再写1次目标地址，组成一次读写，
//     在这次读写中，源地址偏移2次，目标地址偏移1次;
//
//     使能模数功能后，会依照模数大小，使得地址在偏移时，仅有低位可以变化，
//     而高位不会变化，若模数大小为2的n次方，则可变化的位数为n，
//     如模数大小为16个字节，初始地址为0x12345670，仅低4位可以变化，
//     而0x1234567x不会变化;
//     但若主循环完成后的偏移使得地址的高位发生变化，在下一次主循环中，
//     将会保证变化后的高位不变;
//     可以利用模数功能构建一个循环队列;
//==========================================================================
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

//==========================================================================
//函数名称: dma_enable_req
//函数返回: 无
//参数说明: ch:DMA通道号:
//            DMA_CHx，x为通道号;
//功能概要: 接收该通道的DMA请求
//==========================================================================
void dma_enable_req(uint8 ch) {
	//接收该通道的DMA请求
	REG_SET_VAL(DMA_SERQ, DMA_SERQ_SERQ(ch));
}

//==========================================================================
//函数名称: dma_disable_req
//函数返回: 无
//参数说明: ch:DMA通道号:
//            DMA_CHx，x为通道号;
//功能概要: 不接收该通道的DMA请求
//==========================================================================
void dma_disable_req(uint8 ch) {
	//不接收该通道的DMA请求
	REG_SET_VAL(DMA_CERQ, DMA_CERQ_CERQ(ch));
}

//==========================================================================
//函数名称: dma_enable_major_int
//函数返回: 无
//参数说明: ch:DMA通道号:
//            DMA_CHx，x为通道号;
//功能概要: 使能主循环完成中断，当一次主循环完成时，触发中断
//备注: 即使发生中断，若DMA请求被触发，且该通道接收DMA请求，数据传输仍会进行
//==========================================================================
void dma_enable_major_int(uint8 ch) {
	//清除主循环完成中断的中断标志
	REG_SET_VAL(DMA_CINT, DMA_CINT_CINT(ch));
	//使能主循环完成中断
	REG_SET_MASK(DMA_CSR(ch), DMA_CSR_INTMAJOR_MASK);
	//允许接收该通道发送的中断请求
	ENABLE_IRQ(dma_irq_table[ch]);
}

//==========================================================================
//函数名称: dma_disable_major_int
//函数返回: 无
//参数说明: ch:DMA通道号:
//            DMA_CHx，x为通道号;
//功能概要: 关闭主循环完成中断
//==========================================================================
void dma_disable_major_int(uint8 ch) {
	//关闭主循环完成中断
	REG_CLR_MASK(DMA_CSR(ch), DMA_CSR_INTMAJOR_MASK);
	//禁止接收该通道发送的中断请求
	DISABLE_IRQ(dma_irq_table[ch]);
}

//==========================================================================
//函数名称: dma_get_major_int
//函数返回: true:产生中断; false:未产生中断
//参数说明: ch:DMA通道号:
//            DMA_CHx，x为通道号;
//功能概要: 获取主循环完成中断的中断标志
//备注: 仅当该通道使能中断时，中断标志才会置位
//==========================================================================
bool dma_get_major_int(uint8 ch) {
	//获取主循环完成中断的中断标志
	return REG_GET_SHIFT(DMA_INT,ch) ? true : false;
}

//==========================================================================
//函数名称: dma_clear_major_int
//函数返回: 无
//参数说明: ch:DMA通道号:
//            DMA_CHx，x为通道号;
//功能概要: 清除主循环完成中断的中断标志
//==========================================================================
void dma_clear_major_int(uint8 ch) {
	//清除主循环完成中断的中断标志
	REG_SET_VAL(DMA_CINT, DMA_CINT_CINT(ch));
}

//==========================================================================
//函数名称: dma_software_req
//函数返回: 无
//参数说明: ch:DMA通道号:
//            DMA_CHx，x为通道号;
//功能概要: 软件触发一次DMA请求
//备注: 无论是否接收该通道的DMA请求，软件触发的DMA请求一定被接收;
//     若通道状态为正在启动，此时软件触发的DMA请求会丢失
//==========================================================================
void dma_software_req(uint8 ch) {
	//软件触发一次DMA请求
	REG_SET_VAL(DMA_SSRT, DMA_SSRT_SSRT(ch));
}

//==========================================================================
//函数名称: dma_get_ch_state
//函数返回: 通道状态:
//         DMAChannelIdle:     通道空闲;
//         DMAChannelStarting: 通道正在启动;
//         DMAChannelExecuting:通道正在执行;
//         DMAChannelDone:     通道完成主循环;
//参数说明: ch:DMA通道号:
//            DMA_CHx，x为通道号;
//功能概要: 获取通道状态
//备注: 通道状态若为空闲，此时可能请求源发起了一次DMA请求，但尚未执行，
//     也可能通道已经完成了一次副循环，但尚未完成主循环;
//
//     通道状态若为正在启动，此时可能软件触发了一次DMA请求，
//     也可能因通道连接被触发了一次DMA请求，但尚未执行;
//
//     通道状态若为正在执行，此时正在执行该通道的副循环;
//
//     通道状态若为完成主循环，此时该通道已经完成了一次主循环
//==========================================================================
DMAChannelState dma_get_ch_state(uint8 ch) {
	uint16 state;	//通道控制和状态寄存器的值

	//获取通道控制和状态寄存器的值
	state = DMA_CSR(ch);

	if (REG_GET_MASK(state, DMA_CSR_START_MASK)) {
		//返回通道正在启动
		return DMAChannelStarting;
	}
	if (REG_GET_MASK(state, DMA_CSR_ACTIVE_MASK)) {
		//返回通道正在执行
		return DMAChannelExecuting;
	}
	if (REG_GET_MASK(state, DMA_CSR_DONE_MASK)) {
		//返回通道完成主循环
		return DMAChannelDone;
	}
	//返回通道空闲
	return DMAChannelIdle;
}

//==========================================================================
//函数名称: dma_get_major_loop_iteration_cnt
//函数返回: 主循环剩余迭代次数
//参数说明: ch:DMA通道号:
//            DMA_CHx，x为通道号;
//功能概要: 获取主循环剩余迭代次数
//==========================================================================
uint16 dma_get_major_loop_iteration_cnt(uint8 ch) {
	//查看是否使能副循环通道连接
	if (REG_GET_MASK(DMA_CITER_ELINKNO(ch), DMA_CITER_ELINKNO_ELINK_MASK)) {
		//使能副循环通道连接时
		return REG_GET_MASK(DMA_CITER_ELINKYES(ch),
				DMA_CITER_ELINKYES_CITER_MASK) >> DMA_CITER_ELINKYES_CITER_SHIFT;
	} else {
		//关闭副循环通道连接时
		return REG_GET_MASK(DMA_CITER_ELINKNO(ch), DMA_CITER_ELINKNO_CITER_MASK)
				>> DMA_CITER_ELINKNO_CITER_SHIFT;
	}
}

//==========================================================================
//函数名称: dma_get_src_addr
//函数返回: 当前源地址
//参数说明: ch:DMA通道号:
//            DMA_CHx，x为通道号;
//功能概要: 获取当前源地址
//==========================================================================
uint32 dma_get_src_addr(uint8 ch) {
	//返回当前源地址
	return DMA_SADDR(ch);
}

//==========================================================================
//函数名称: dma_set_src_addr
//函数返回: 无
//参数说明: ch:DMA通道号:
//            DMA_CHx，x为通道号;
//         src_addr:当前源地址，必须与数据宽度匹配
//功能概要: 设置当前源地址
//备注: 请设置为不接收该通道的DMA请求后，再进行设置
//==========================================================================
void dma_set_src_addr(uint8 ch, uint32 src_addr) {
	//设置源地址
	REG_SET_VAL(DMA_SADDR(ch), DMA_SADDR_SADDR(src_addr));
}

//==========================================================================
//函数名称: dma_get_dest_addr
//函数返回: 当前目标地址
//参数说明: ch:DMA通道号:
//            DMA_CHx，x为通道号;
//功能概要: 获取当前目标地址
//==========================================================================
uint32 dma_get_dest_addr(uint8 ch) {
	//返回当前目标地址
	return DMA_DADDR(ch);
}

//==========================================================================
//函数名称: dma_set_dest_addr
//函数返回: 无
//参数说明: ch:DMA通道号:
//            DMA_CHx，x为通道号;
//         dest_addr:当前目标地址，必须与数据宽度匹配
//功能概要: 设置当前目标地址
//备注: 请设置为不接收该通道的DMA请求后，再进行设置
//==========================================================================
void dma_set_dest_addr(uint8 ch, uint32 dest_addr) {
	//设置目标地址
	REG_SET_VAL(DMA_DADDR(ch), DMA_DADDR_DADDR(dest_addr));
}

//==========================================================================
//函数名称: dma_set_auto_disable_req
//函数返回: 无
//参数说明: ch:DMA通道号:
//            DMA_CHx，x为通道号;
//         enable:是否使能主循环完成后自动不接收DMA请求:
//                true: 使能主循环完成后自动不接收DMA请求;
//                false:关闭主循环完成后自动不接收DMA请求;
//功能概要: 设置是否使能主循环完成后自动不接收DMA请求
//备注: 请设置为不接收该通道的DMA请求后，再进行设置
//==========================================================================
void dma_set_auto_disable_req(uint8 ch, bool enable) {
	if (enable) {
		//使能主循环完成后自动不接收DMA请求
		REG_SET_MASK(DMA_CSR(ch), DMA_CSR_DREQ_MASK);
	} else {
		//关闭主循环完成后自动不接收DMA请求
		REG_CLR_MASK(DMA_CSR(ch), DMA_CSR_DREQ_MASK);
	}
}

//当该通道的一次副循环完成后，自动软件触发一次连接通道的DMA请求
//若此次副循环完成后即完成主循环，将不会触发连接通道的DMA请求
void dma_set_minor_link(uint8 ch, bool enable, uint8 link_ch) {
	//清除连接通道
	REG_CLR_MASK(DMA_CITER_ELINKYES(ch), DMA_CITER_ELINKYES_LINKCH_MASK);
	REG_CLR_MASK(DMA_BITER_ELINKYES(ch), DMA_BITER_ELINKYES_LINKCH_MASK);
	if (enable) {
		//使能副循环通道连接，并设置连接通道
		REG_SET_MASK(DMA_CITER_ELINKYES(ch),
				DMA_CITER_ELINKYES_ELINK_MASK|DMA_CITER_ELINKYES_LINKCH(link_ch));
		REG_SET_MASK(DMA_BITER_ELINKYES(ch),
				DMA_BITER_ELINKYES_ELINK_MASK|DMA_BITER_ELINKYES_LINKCH(link_ch));
	} else {
		//关闭副循环通道连接
		REG_CLR_MASK(DMA_CITER_ELINKNO(ch), DMA_CITER_ELINKNO_ELINK_MASK);
		REG_CLR_MASK(DMA_BITER_ELINKNO(ch), DMA_BITER_ELINKNO_ELINK_MASK);
	}
}

//当该通道的一次主循环完成后，自动软件触发一次连接通道的DMA请求
void dma_set_major_link(uint8 ch, bool enable, uint8 link_ch) {
	//清除该通道完成标志
	REG_SET_VAL(DMA_CDNE, DMA_CDNE_CDNE(ch));
	if (enable) {
		//清除连接通道
		REG_CLR_MASK(DMA_CSR(ch), DMA_CSR_MAJORLINKCH_MASK);
		//使能主循环通道连接，并设置连接通道
		REG_SET_MASK(DMA_CSR(ch),
				DMA_CSR_MAJORELINK_MASK|DMA_CSR_MAJORLINKCH(link_ch));
	} else {
		//关闭主循环通道连接
		REG_CLR_MASK(DMA_CSR(ch), DMA_CSR_MAJORELINK_MASK);
	}
}

void dma_set_arbitration_mode(uint8 arbitration_mode) {
	if (arbitration_mode == DMA_ARBITRATION_MODE_ROUND_ROBIN) {
		//轮询仲裁
		REG_SET_MASK(DMA_CR, DMA_CR_ERCA_MASK);
	} else {
		//固定优先级仲裁
		REG_CLR_MASK(DMA_CR, DMA_CR_ERCA_MASK);
	}
}

uint8 dma_get_priority(uint8 ch) {
	//获取该通道的优先级
	return REG_GET_MASK(*dchpri_table[ch], DMA_DCHPRI0_CHPRI_MASK)
			>> DMA_DCHPRI0_CHPRI_SHIFT;
}

void dma_set_priority(uint8 ch, uint8 priority) {
	//清除原先优先级
	REG_CLR_MASK(*dchpri_table[ch], DMA_DCHPRI0_CHPRI_MASK);
	//设置新优先级
	REG_SET_MASK(*dchpri_table[ch], DMA_DCHPRI0_CHPRI(priority));
}

void dma_set_channel_preemption(uint8 ch, bool enable) {
	if (enable) {
		//该通道可以被抢占
		REG_SET_MASK(*dchpri_table[ch], DMA_DCHPRI0_ECP_MASK);
	} else {
		//该通道不可以被抢占
		REG_CLR_MASK(*dchpri_table[ch], DMA_DCHPRI0_ECP_MASK);
	}
}

void dma_set_preempt_ability(uint8 ch, bool enable) {
	if (enable) {
		//该通道可以抢占别的通道
		REG_CLR_MASK(*dchpri_table[ch], DMA_DCHPRI0_DPA_MASK);
	} else {
		//该通道不可以抢占别的通道
		REG_SET_MASK(*dchpri_table[ch], DMA_DCHPRI0_DPA_MASK);
	}
}
