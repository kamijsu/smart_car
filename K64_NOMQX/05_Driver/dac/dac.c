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

void dac_set_buffer_val(uint8 mod, uint8 no, uint16 val) {
	DAC_Type * dac_ptr;	//DAC基地址

	//获取DAC基地址
	dac_ptr = dac_table[mod];

	//设置缓冲区值
	REG_SET_VAL(DAC_DATL_REG(dac_ptr,no), DAC_DATL_DATA0(val));
	REG_SET_VAL(DAC_DATH_REG(dac_ptr,no),
			DAC_DATH_DATA1(val>>DAC_DATL_DATA0_WIDTH));
}
