//==========================================================================
//文件名称：dac.c
//功能概要：K64 DAC底层驱动程序源文件
//==========================================================================

#include "dac.h"

//DAC各模块基地址
static DAC_Type * const dac_table[] = DAC_BASE_PTRS;
//DAC各模块中断请求号
static const IRQn_Type dac_irq_table[] = { DAC0_IRQn, DAC1_IRQn };

//==========================================================================
//函数名称: dac_init
//函数返回: 无
//参数说明: mod:DAC模块号:
//             DAC_MODx，x为模块号;
//         ref_vtg:参考电压源:
//                 DAC_REF_VTG_VREFOUT:VREF_OUT;
//                 DAC_REF_VTG_VDDA:   VDDA;
//功能概要: 初始化DAC模块，默认关闭缓冲区，默认输出值为0，即(参考电压值/4096)
//==========================================================================
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

//==========================================================================
//函数名称: dac_set_buffer_val
//函数返回: 无
//参数说明: mod:DAC模块号:
//             DAC_MODx，x为模块号;
//         index:缓冲区位置，取值范围为[0,15]
//         val:缓冲区值，取值范围为[0,4095]
//功能概要: 设置指定缓冲区位置的值
//备注: 输出电压值 = 参考电压值 * (val + 1) / 4096，
//     val为缓冲区当前指针位置的val;
//     未使能缓冲区时，仅会输出缓冲区位置为0的值;
//     无论是否使能缓冲区，各缓冲区位置的值均可设置
//==========================================================================
void dac_set_buffer_val(uint8 mod, uint8 index, uint16 val) {
	DAC_Type * dac_ptr;	//DAC基地址

	//获取DAC基地址
	dac_ptr = dac_table[mod];

	//设置缓冲区值
	REG_SET_VAL(DAC_DATL_REG(dac_ptr,index), DAC_DATL_DATA0(val));
	REG_SET_VAL(DAC_DATH_REG(dac_ptr,index),
			DAC_DATH_DATA1(val>>DAC_DATL_DATA0_WIDTH));
}

//==========================================================================
//函数名称: dac_get_buffer_val
//函数返回: 缓冲区值，取值范围为[0,4095]
//参数说明: mod:DAC模块号:
//             DAC_MODx，x为模块号;
//         index:缓冲区位置，取值范围为[0,15]
//功能概要: 获取指定缓冲区位置的值
//==========================================================================
uint16 dac_get_buffer_val(uint8 mod, uint8 index) {
	DAC_Type * dac_ptr;	//DAC基地址

	//获取DAC基地址
	dac_ptr = dac_table[mod];

	//返回缓冲区值
	return (DAC_DATH_REG(dac_ptr,index) << DAC_DATL_DATA0_WIDTH)
			| DAC_DATL_REG(dac_ptr, index);
}

//==========================================================================
//函数名称: dac_enable_buffer
//函数返回: 无
//参数说明: mod:DAC模块号:
//             DAC_MODx，x为模块号;
//         buffer_mode:缓冲区工作模式:
//                     DAC_BUFFER_MODE_NORMAL:       普通模式;
//                     DAC_BUFFER_MODE_SWING:        摇摆模式;
//                     DAC_BUFFER_MODE_ONE_TIME_SCAN:单次扫描模式;
//         max_index:缓冲区指针位置上限，取值范围为[0,15]
//         watermark:缓冲区水印选择:
//                   DAC_WATERMARK_WORD_x，水印距离缓冲区指针位置上限x个字，
//                   x可取值为[1,4]
//         trigger:触发转换方式:
//                 DAC_TRIGGER_HARDWARE:硬件触发;
//                 DAC_TRIGGER_SOFTWARE:软件触发;
//功能概要: 使能DAC缓冲区，默认关闭各缓冲区中断
//备注: 使能缓冲区后，当前指针位置被重置为0;
//
//     工作模式为普通模式时，缓冲区当前指针位置在触发信号下每次加1，直到到达上限，
//     下一个触发信号到来时，指针重置为0;
//
//     工作模式为摇摆模式时，指针位置到达上限后，不直接重置为0，而是开始递减，
//     一步步退回至0，再重新递增;
//
//     工作模式为单次扫描模式时，指针位置到达上限后，收到触发信号时，
//     指针位置将不再变化;
//
//     若指针位置上限为2，共5个触发信号，指针位置变化顺序为:
//     普通模式:    0 - 1 - 2 - 0 - 1 - 2
//     摇摆模式:    0 - 1 - 2 - 1 - 0 - 1
//     单次扫描模式: 0 - 1 - 2 - 2 - 2 - 2
//
//     若水印选择为DAC_WATERMARK_WORD_2，缓冲区指针位置上限为15，则水印位置为13，
//     但若指针位置上限小于水印距离，将无法到达水印位置;
//
//     若选择硬件触发，则需要设置PDB模块
//==========================================================================
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

//==========================================================================
//函数名称: dac_disable_buffer
//函数返回: 无
//参数说明: mod:DAC模块号:
//             DAC_MODx，x为模块号;
//功能概要: 关闭DAC缓冲区
//备注: 关闭缓冲区后，当前指针位置被重置为0;各缓冲区中断不会被关闭
//==========================================================================
void dac_disable_buffer(uint8 mod) {
	//关闭DAC缓冲区
	REG_CLR_MASK(DAC_C1_REG(dac_table[mod]), DAC_C1_DACBFEN_MASK);
}

//==========================================================================
//函数名称: dac_software_trigger
//函数返回: 无
//参数说明: mod:DAC模块号:
//             DAC_MODx，x为模块号;
//功能概要: 进行一次软件触发
//备注: 未使能缓冲区时，缓冲区当前指针位置不会变化;
//     使能缓冲区时，缓冲区当前指针位置将按照工作模式进行相应的变化，
//     将输出变化后缓冲区位置的值
//==========================================================================
void dac_software_trigger(uint8 mod) {
	//进行一次软件触发
	REG_SET_MASK(DAC_C0_REG(dac_table[mod]), DAC_C0_DACSWTRG_MASK);
}

//==========================================================================
//函数名称: dac_set_buffer_index
//函数返回: 无
//参数说明: mod:DAC模块号:
//             DAC_MODx，x为模块号;
//         index:缓冲区当前指针位置，取值范围为[0,15]
//功能概要: 设置缓冲区当前指针位置
//备注: 未使能缓冲区时，设置当前指针位置无效;
//     使能缓冲区时，当前指针位置可以超过缓冲区上限;
//     将输出变化后缓冲区位置的值
//==========================================================================
void dac_set_buffer_index(uint8 mod, uint8 index) {
	DAC_Type * dac_ptr;	//DAC基地址

	//获取DAC基地址
	dac_ptr = dac_table[mod];

	//设置缓冲区当前指针位置，这种方式避免了设置指针位置为0
	REG_SET_VAL(DAC_C2_REG(dac_ptr),
			REG_GET_MASK(DAC_C2_REG(dac_ptr),DAC_C2_DACBFUP_MASK)|DAC_C2_DACBFRP(index));
}

//==========================================================================
//函数名称: dac_get_buffer_index
//函数返回: 缓冲区当前指针位置，取值范围为[0,15]
//参数说明: mod:DAC模块号:
//             DAC_MODx，x为模块号;
//功能概要: 获取缓冲区当前指针位置
//==========================================================================
uint8 dac_get_buffer_index(uint8 mod) {
	//获取缓冲区当前指针位置
	return REG_GET_MASK(DAC_C2_REG(dac_table[mod]), DAC_C2_DACBFRP_MASK)
			>> DAC_C2_DACBFRP_SHIFT;
}

//==========================================================================
//函数名称: dac_set_buffer_int
//函数返回: 无
//参数说明: mod:DAC模块号:
//             DAC_MODx，x为模块号;
//         index_bottom_int:是否使能指针位置底部中断:
//                          true: 使能中断;
//                          false:关闭中断;
//         index_top_int:是否使能指针位置顶部中断:
//                       true: 使能中断;
//                       false:关闭中断;
//         watermark_int:是否使能水印中断:
//                       true: 使能中断;
//                       false:关闭中断;
//功能概要: 使能或关闭各缓冲区中断
//备注: 缓冲区当前指针位置为指针位置上限时，将触发指针位置底部中断;
//     为0时，将触发指针位置顶部中断;为水印位置时，将触发水印中断;
//
//     仅当使能中断后，且指针位置发生变化，才会发生相应中断;
//
//     若该中断未被使能，指针位置到达中断位置，中断标志会被置位，但不会发生中断
//==========================================================================
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

//==========================================================================
//函数名称: dac_get_index_bottom_int
//函数返回: true:产生中断; false:未产生中断
//参数说明: mod:DAC模块号:
//             DAC_MODx，x为模块号;
//功能概要: 获取指针位置底部中断的中断标志
//==========================================================================
bool dac_get_index_bottom_int(uint8 mod) {
	//获取中断标志
	return REG_GET_MASK(DAC_SR_REG(dac_table[mod]), DAC_SR_DACBFRPBF_MASK) ?
			true : false;
}

//==========================================================================
//函数名称: dac_clear_index_bottom_int
//函数返回: 无
//参数说明: mod:DAC模块号:
//             DAC_MODx，x为模块号;
//功能概要: 清除指针位置底部中断的中断标志
//==========================================================================
void dac_clear_index_bottom_int(uint8 mod) {
	//清除中断标志
	REG_CLR_MASK(DAC_SR_REG(dac_table[mod]), DAC_SR_DACBFRPBF_MASK);
}

//==========================================================================
//函数名称: dac_get_index_top_int
//函数返回: true:产生中断; false:未产生中断
//参数说明: mod:DAC模块号:
//             DAC_MODx，x为模块号;
//功能概要: 获取指针位置顶部中断的中断标志
//==========================================================================
bool dac_get_index_top_int(uint8 mod) {
	//获取中断标志
	return REG_GET_MASK(DAC_SR_REG(dac_table[mod]), DAC_SR_DACBFRPTF_MASK) ?
			true : false;
}

//==========================================================================
//函数名称: dac_clear_index_top_int
//函数返回: 无
//参数说明: mod:DAC模块号:
//             DAC_MODx，x为模块号;
//功能概要: 清除指针位置顶部中断的中断标志
//==========================================================================
void dac_clear_index_top_int(uint8 mod) {
	//清除中断标志
	REG_CLR_MASK(DAC_SR_REG(dac_table[mod]), DAC_SR_DACBFRPTF_MASK);
}

//==========================================================================
//函数名称: dac_get_watermark_int
//函数返回: true:产生中断; false:未产生中断
//参数说明: mod:DAC模块号:
//             DAC_MODx，x为模块号;
//功能概要: 获取水印中断的中断标志
//==========================================================================
bool dac_get_watermark_int(uint8 mod) {
	//获取中断标志
	return REG_GET_MASK(DAC_SR_REG(dac_table[mod]), DAC_SR_DACBFWMF_MASK) ?
			true : false;
}

//==========================================================================
//函数名称: dac_clear_watermark_int
//函数返回: 无
//参数说明: mod:DAC模块号:
//             DAC_MODx，x为模块号;
//功能概要: 清除水印中断的中断标志
//==========================================================================
void dac_clear_watermark_int(uint8 mod) {
	//清除中断标志
	REG_CLR_MASK(DAC_SR_REG(dac_table[mod]), DAC_SR_DACBFWMF_MASK);
}
