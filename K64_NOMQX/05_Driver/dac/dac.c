//==========================================================================
//文件名称：dac.c
//功能概要：K64 DAC底层驱动程序源文件
//==========================================================================

#include "dac.h"

//DAC各模块基地址
static DAC_Type * const dac_table[] = DAC_BASE_PTRS;
//DAC各模块中断请求号
static const IRQn_Type dac_irq_table[] = { DAC0_IRQn, DAC1_IRQn };

void dac_init(uint8 mod, uint8 ref_vtg) {
	DAC_Type * dac_ptr;	//DAC基地址

	//获取DAC基地址
	dac_ptr = dac_table[mod];

	//开启相应DAC模块时钟门
	if (mod == DAC_MOD0) {
		REG_SET_MASK(SIM_SCGC2, SIM_SCGC2_DAC0_MASK);
	} else {
		REG_SET_MASK(SIM_SCGC2, SIM_SCGC2_DAC1_MASK);
	}

	//默认关闭缓冲区
	REG_CLR_MASK(DAC_C1_REG(dac_ptr), DAC_C1_DACBFEN_MASK);

	//设置参考电压源
	if (ref_vtg == DAC_REF_VTG_VREFOUT) {
		REG_CLR_MASK(DAC_C0_REG(dac_ptr), DAC_C0_DACRFS_MASK);
	} else {
		REG_SET_MASK(DAC_C0_REG(dac_ptr), DAC_C0_DACRFS_MASK);
	}

	//使能DAC模块
	REG_SET_MASK(DAC_C0_REG(dac_ptr), DAC_C0_DACEN_MASK);
}

//无论是否使能缓冲区，各缓冲区值均可设置
void dac_set_buffer_val(uint8 mod, uint8 index, uint16 val) {
	DAC_Type * dac_ptr;	//DAC基地址

	//获取DAC基地址
	dac_ptr = dac_table[mod];

	//设置缓冲区值
	REG_SET_VAL(DAC_DATL_REG(dac_ptr,index), DAC_DATL_DATA0(val));
	REG_SET_VAL(DAC_DATH_REG(dac_ptr,index),
			DAC_DATH_DATA1(val>>DAC_DATL_DATA0_WIDTH));
}

uint16 dac_get_buffer_val(uint8 mod, uint8 index) {
	DAC_Type * dac_ptr;	//DAC基地址

	//获取DAC基地址
	dac_ptr = dac_table[mod];

	//返回缓冲区值
	return (DAC_DATH_REG(dac_ptr,index) << DAC_DATL_DATA0_WIDTH)
			| DAC_DATL_REG(dac_ptr, index);
}

void dac_enable_buffer(uint8 mod, uint8 buffer_mode, uint8 max_index,
		uint8 watermark, uint8 trigger) {
	DAC_Type * dac_ptr;	//DAC基地址

	//获取DAC基地址
	dac_ptr = dac_table[mod];

	//关闭各缓冲区中断
	REG_CLR_MASK(DAC_C0_REG(dac_ptr),
			DAC_C0_DACBWIEN_MASK|DAC_C0_DACBTIEN_MASK|DAC_C0_DACBBIEN_MASK);

	//设置触发转换方式
	if (trigger == DAC_TRIGGER_HARDWARE) {
		REG_CLR_MASK(DAC_C0_REG(dac_ptr), DAC_C0_DACTRGSEL_MASK);
	} else {
		REG_SET_MASK(DAC_C0_REG(dac_ptr), DAC_C0_DACTRGSEL_MASK);
	}

	//设置缓冲区工作模式和水印
	REG_CLR_MASK(DAC_C1_REG(dac_ptr), DAC_C1_DACBFMD_MASK|DAC_C1_DACBFWM_MASK);
	REG_SET_MASK(DAC_C1_REG(dac_ptr),
			DAC_C1_DACBFMD(buffer_mode)|DAC_C1_DACBFWM(watermark));

	//设置缓冲区指针位置上限，并且重置当前指针位置为0
	REG_SET_VAL(DAC_C2_REG(dac_ptr), DAC_C2_DACBFUP(max_index));

	//使能DAC缓冲区
	REG_SET_MASK(DAC_C1_REG(dac_ptr), DAC_C1_DACBFEN_MASK);
}

//关闭缓冲区后，当前指针位置被置为0
void dac_disable_buffer(uint8 mod) {
	//关闭DAC缓冲区
	REG_CLR_MASK(DAC_C1_REG(dac_table[mod]), DAC_C1_DACBFEN_MASK);
}

//未使能缓冲区时，缓冲区当前指针位置不会变化
void dac_software_trigger(uint8 mod) {
	//进行一次软件触发
	REG_SET_MASK(DAC_C0_REG(dac_table[mod]), DAC_C0_DACSWTRG_MASK);
}

//未使能缓冲区时，设置当前指针位置无效
void dac_set_buffer_index(uint8 mod, uint8 index) {
	DAC_Type * dac_ptr;	//DAC基地址

	//获取DAC基地址
	dac_ptr = dac_table[mod];

	//设置缓冲区当前指针位置，这种方式避免了设置指针位置为0
	REG_SET_VAL(DAC_C2_REG(dac_ptr),
			REG_GET_MASK(DAC_C2_REG(dac_ptr),DAC_C2_DACBFUP_MASK)|DAC_C2_DACBFRP(index));
}

uint8 dac_get_buffer_index(uint8 mod) {
	//获取缓冲区当前指针位置
	return REG_GET_MASK(DAC_C2_REG(dac_table[mod]), DAC_C2_DACBFRP_MASK)
			>> DAC_C2_DACBFRP_SHIFT;
}

void dac_set_buffer_int(uint8 mod, bool index_bottom_int, bool index_top_int,
		bool watermark_int) {
	DAC_Type * dac_ptr;	//DAC基地址

	//获取DAC基地址
	dac_ptr = dac_table[mod];

	if (index_bottom_int) {
		//清除中断标志
		REG_CLR_MASK(DAC_SR_REG(dac_ptr), DAC_SR_DACBFRPBF_MASK);
		//使能指针位置底部中断
		REG_SET_MASK(DAC_C0_REG(dac_ptr), DAC_C0_DACBBIEN_MASK);
	} else {
		//关闭指针位置底部中断
		REG_CLR_MASK(DAC_C0_REG(dac_ptr), DAC_C0_DACBBIEN_MASK);
	}

	if (index_top_int) {
		//清除中断标志
		REG_CLR_MASK(DAC_SR_REG(dac_ptr), DAC_SR_DACBFRPTF_MASK);
		//使能指针位置顶部中断
		REG_SET_MASK(DAC_C0_REG(dac_ptr), DAC_C0_DACBTIEN_MASK);
	} else {
		//关闭指针位置顶部中断
		REG_CLR_MASK(DAC_C0_REG(dac_ptr), DAC_C0_DACBTIEN_MASK);
	}

	if (watermark_int) {
		//清除中断标志
		REG_CLR_MASK(DAC_SR_REG(dac_ptr), DAC_SR_DACBFWMF_MASK);
		//使能水印中断
		REG_SET_MASK(DAC_C0_REG(dac_ptr), DAC_C0_DACBWIEN_MASK);
	} else {
		//关闭水印中断
		REG_CLR_MASK(DAC_C0_REG(dac_ptr), DAC_C0_DACBWIEN_MASK);
	}

	if (!index_bottom_int && !index_top_int && !watermark_int) {
		//若关闭所有中断，则禁止接收该DAC模块中断请求
		DISABLE_IRQ(dac_irq_table[mod]);
	} else {
		//若有中断使能，则允许接收该DAC模块中断请求
		ENABLE_IRQ(dac_irq_table[mod]);
	}
}

bool dac_get_index_bottom_int(uint8 mod) {
	//获取中断标志
	return REG_GET_MASK(DAC_SR_REG(dac_table[mod]), DAC_SR_DACBFRPBF_MASK) ?
			true : false;
}

void dac_clear_index_bottom_int(uint8 mod) {
	//清除中断标志
	REG_CLR_MASK(DAC_SR_REG(dac_table[mod]), DAC_SR_DACBFRPBF_MASK);
}

bool dac_get_index_top_int(uint8 mod) {
	//获取中断标志
	return REG_GET_MASK(DAC_SR_REG(dac_table[mod]), DAC_SR_DACBFRPTF_MASK) ?
			true : false;
}

void dac_clear_index_top_int(uint8 mod) {
	//清除中断标志
	REG_CLR_MASK(DAC_SR_REG(dac_table[mod]), DAC_SR_DACBFRPTF_MASK);
}

bool dac_get_watermark_int(uint8 mod) {
	//获取中断标志
	return REG_GET_MASK(DAC_SR_REG(dac_table[mod]), DAC_SR_DACBFWMF_MASK) ?
			true : false;
}

void dac_clear_watermark_int(uint8 mod) {
	//清除中断标志
	REG_CLR_MASK(DAC_SR_REG(dac_table[mod]), DAC_SR_DACBFWMF_MASK);
}
