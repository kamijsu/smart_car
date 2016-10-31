//==========================================================================
//文件名称：ftm.c
//功能概要：K64 FTM底层驱动程序源文件
//==========================================================================

#include "ftm.h"

//FTM各模块基地址
static FTM_Type * const ftm_table[] = { FTM0, FTM1, FTM2, FTM3 };
//FTM各模块各通道引脚号
static const uint8 ftm_pin_table[][8] = { { FTM_MOD0_CH0_PIN, FTM_MOD0_CH1_PIN,
FTM_MOD0_CH2_PIN, FTM_MOD0_CH3_PIN, FTM_MOD0_CH4_PIN, FTM_MOD0_CH5_PIN,
FTM_MOD0_CH6_PIN, FTM_MOD0_CH7_PIN }, { FTM_MOD1_CH0_PIN, FTM_MOD1_CH1_PIN }, {
FTM_MOD2_CH0_PIN, FTM_MOD2_CH1_PIN }, { FTM_MOD3_CH0_PIN,
FTM_MOD3_CH1_PIN, FTM_MOD3_CH2_PIN, FTM_MOD3_CH3_PIN, FTM_MOD3_CH4_PIN,
FTM_MOD3_CH5_PIN, FTM_MOD3_CH6_PIN, FTM_MOD3_CH7_PIN } };
//FTM各模块各通道PCR的MUX值
static const uint8 ftm_pcr_mux_table[][8] = { { FTM_MOD0_CH0_PCR_MUX,
FTM_MOD0_CH1_PCR_MUX, FTM_MOD0_CH2_PCR_MUX, FTM_MOD0_CH3_PCR_MUX,
FTM_MOD0_CH4_PCR_MUX, FTM_MOD0_CH5_PCR_MUX, FTM_MOD0_CH6_PCR_MUX,
FTM_MOD0_CH7_PCR_MUX }, { FTM_MOD1_CH0_PCR_MUX, FTM_MOD1_CH1_PCR_MUX }, {
FTM_MOD2_CH0_PCR_MUX, FTM_MOD2_CH1_PCR_MUX }, {
FTM_MOD3_CH0_PCR_MUX, FTM_MOD3_CH1_PCR_MUX, FTM_MOD3_CH2_PCR_MUX,
FTM_MOD3_CH3_PCR_MUX, FTM_MOD3_CH4_PCR_MUX, FTM_MOD3_CH5_PCR_MUX,
FTM_MOD3_CH6_PCR_MUX, FTM_MOD3_CH7_PCR_MUX } };
//FTM各模块中断请求号
static const IRQn_Type ftm_irq_table[] = { FTM0_IRQn, FTM1_IRQn, FTM2_IRQn,
		FTM3_IRQn };

//==========================================================================
//函数名称: ftm_init
//函数返回: 无
//参数说明: mod:FTM模块号，FTM_MODx，x为模块号
//         period:FTM模块计数周期，其大小上限与分频因子大小有关，单位ms
//         div:时钟分频因子，FTM_CLK_DIV_x，x为分频因子大小
//功能概要: 初始化FTM模块，默认未开启中断
//备注: 48000/div*period<=65536，48000为这里使用的总线时钟频率，单位kHz
//==========================================================================
void ftm_init(uint8 mod, uint8 period, uint8 div) {
	uint16 modulo;	//模数寄存器的值

	//开启相应FTM模块时钟门
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
	//禁止写保护，即允许更改写保护的寄存器的值
	REG_SET_MASK(FTM_MODE_REG(ftm_table[mod]), FTM_MODE_WPDIS_MASK);
	//先禁止FTM计数器，清除预分频因子，并且禁止中断
	REG_SET_VAL(FTM_SC_REG(ftm_table[mod]), 0);
	//清除当前计数器寄存器的值
	REG_SET_VAL(FTM_CNT_REG(ftm_table[mod]), 0);
	//设置计数器每次的初始值为0，会写到缓冲中
	REG_SET_VAL(FTM_CNTIN_REG(ftm_table[mod]), 0);
	//根据分频因子与计数周期计算模数的值，计数周期=(MOD-CNTIN+1)/时钟频率
	modulo = (FTM_CLK_FREQ >> div) * period - 1;
	//设置模数寄存器的值，会写到缓冲中
	REG_SET_VAL(FTM_MOD_REG(ftm_table[mod]), modulo);
	//允许加载MOD、CNTIN写缓冲的值
	REG_SET_VAL(FTM_PWMLOAD_REG(ftm_table[mod]), 0xFFFFFFFF);
	//设置时钟源为总线时钟，并且设置预分频因子
	REG_SET_MASK(FTM_SC_REG(ftm_table[mod]), (FTM_SC_CLKS(1) | FTM_SC_PS(div)));
}

//==========================================================================
//函数名称: ftm_enable_timer_int
//函数返回: 无
//参数说明: mod:FTM模块号，FTM_MODx，x为模块号
//         time:FTM模块每time个计数周期产生一个中断请求，范围[1,32]
//功能概要: 使能FTM模块计时中断，中断时间周期为period*time，单位ms
//==========================================================================
void ftm_enable_timer_int(uint8 mod, uint8 time) {
	//设置产生中断的频率
	REG_CLR_MASK(FTM_CONF_REG(ftm_table[mod]), FTM_CONF_NUMTOF_MASK);
	REG_SET_MASK(FTM_CONF_REG(ftm_table[mod]), FTM_CONF_NUMTOF(time-1));
	//使能定时器溢出中断
	REG_SET_MASK(FTM_SC_REG(ftm_table[mod]), FTM_SC_TOIE_MASK);
	//允许接收该FTM模块中断请求
	ENABLE_IRQ(ftm_irq_table[mod]);
}

//==========================================================================
//函数名称: ftm_disable_timer_int
//函数返回: 无
//参数说明: mod:FTM模块号，FTM_MODx，x为模块号
//功能概要: 关闭FTM模块计时中断
//==========================================================================
void ftm_disable_timer_int(uint8 mod) {
	//关闭定时器溢出中断，未禁止接收该FTM模块中断请求，因为可能有通道会产生中断请求
	REG_CLR_MASK(FTM_SC_REG(ftm_table[mod]), FTM_SC_TOIE_MASK);
}

//==========================================================================
//函数名称: ftm_get_timer_int
//函数返回: true:产生中断; false:未产生中断
//参数说明: mod:FTM模块号，FTM_MODx，x为模块号
//功能概要: 获取FTM模块计时中断标志
//==========================================================================
bool ftm_get_timer_int(uint8 mod) {
	//获取定时器溢出标志
	return (REG_GET_MASK(FTM_SC_REG(ftm_table[mod]), FTM_SC_TOF_MASK)) ?
			true : false;
}

//==========================================================================
//函数名称: ftm_clear_timer_int
//函数返回: 无
//参数说明: mod:FTM模块号，FTM_MODx，x为模块号
//功能概要: 清除FTM模块计时中断标志
//==========================================================================
void ftm_clear_timer_int(uint8 mod) {
	//清除定时器溢出标志
	REG_CLR_MASK(FTM_SC_REG(ftm_table[mod]), FTM_SC_TOF_MASK);
}
