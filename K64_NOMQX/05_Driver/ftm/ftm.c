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
//函数名称: ftm_ch_set_mux
//函数返回: 无
//参数说明: mod:FTM模块号:
//             FTM_MODx，x为模块号;
//         ch:FTM模块的通道号:
//            FTM_CHx，x为通道号;
//功能概要: 设置该模块的该通道所对应的引脚为FTM模块通道功能
//==========================================================================
static void ftm_ch_set_mux(uint8 mod, uint8 ch) {
	uint8 port, pin;	//端口号与引脚号
	//根据通道号获取端口号与引脚号
	com_port_pin_resolution(ftm_pin_table[mod][ch], &port, &pin);
	//设置该端口的引脚为FTM通道功能
	REG_CLR_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_MUX_MASK);
	REG_SET_MASK(PORT_PCR_REG(port_table[port],pin),
			PORT_PCR_MUX(ftm_pcr_mux_table[mod][ch]));
}

//==========================================================================
//函数名称: ftm_init
//函数返回: 无
//参数说明: mod:FTM模块号:
//             FTM_MODx，x为模块号;
//         clk_div:时钟分频因子:
//                 FTM_CLK_DIV_x，x为分频因子大小;
//         counter_mode:计数器模式:
//                      FTM_COUNTER_MODE_UP:向上计数;
//                      FTM_COUNTER_MODE_UP_DOWN:上下计数;
//                      FTM_COUNTER_MODE_FREE_RUNNING:自由运行;
//                      FTM_COUNTER_MODE_QD:正交解码;
//         counter_period:见备注
//功能概要: 初始化FTM模块，默认未开启中断
//备注: 当选择向上计数模式或上下计数模式时，counter_period为计数器计数周期，单位ms，
//     需满足48000/x*counter_period<=num，48000为这里使用的总线时钟频率，单位kHz，
//     x为FTM_CLK_DIV_x的x，向上计数模式时num为65536，上下计数模式时num为65534，
//     另外，上下计数模式时，若选择128分频，周期需为偶数;
//     当选择自由运行模式时，counter_period无效;
//     当选择正交解码模式时，counter_period为每次产生中断时，计数器已经计数的个数，
//     范围[1,65536];
//==========================================================================
void ftm_init(uint8 mod, uint8 clk_div, uint8 counter_mode,
		uint8 counter_period) {
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
	//配置计数器模式
	switch (counter_mode) {
	case FTM_COUNTER_MODE_UP:
		//向上计数:QUADEN=0;CPWMS=0;
		REG_CLR_MASK(FTM_QDCTRL_REG(ftm_table[mod]), FTM_QDCTRL_QUADEN_MASK);
		REG_CLR_MASK(FTM_SC_REG(ftm_table[mod]), FTM_SC_CPWMS_MASK);
		//计数周期=(MOD-CNTIN+1)/时钟频率
		modulo = (FTM_CLK_FREQ >> clk_div) * counter_period - 1;
		break;
	case FTM_COUNTER_MODE_UP_DOWN:
		//上下计数:QUADEN=0;CPWMS=1，这里因为用不到FTM功能所以把FTMEN置0
		REG_CLR_MASK(FTM_MODE_REG(ftm_table[mod]), FTM_MODE_FTMEN_MASK);
		REG_CLR_MASK(FTM_QDCTRL_REG(ftm_table[mod]), FTM_QDCTRL_QUADEN_MASK);
		REG_SET_MASK(FTM_SC_REG(ftm_table[mod]), FTM_SC_CPWMS_MASK);
		//计数周期=2*(MOD-CNTIN)/时钟频率
		modulo = ((FTM_CLK_FREQ >> clk_div) * counter_period) >> 1;
		break;
	case FTM_COUNTER_MODE_FREE_RUNNING:
		//自由运行:FTMEN=1;QUADEN=0;CPWMS=0;CNTIN=0x0000;MOD=0xFFFF;
		REG_SET_MASK(FTM_MODE_REG(ftm_table[mod]), FTM_MODE_FTMEN_MASK);
		REG_CLR_MASK(FTM_QDCTRL_REG(ftm_table[mod]), FTM_QDCTRL_QUADEN_MASK);
		REG_CLR_MASK(FTM_SC_REG(ftm_table[mod]), FTM_SC_CPWMS_MASK);
		modulo = 0xFFFF;
		break;
	case FTM_COUNTER_MODE_QD:
		//正交解码:FTMEN=1;QUADEN=1;
		REG_SET_MASK(FTM_MODE_REG(ftm_table[mod]), FTM_MODE_FTMEN_MASK);
		REG_SET_MASK(FTM_QDCTRL_REG(ftm_table[mod]), FTM_QDCTRL_QUADEN_MASK);
		//MOD需要减1
		modulo = counter_period - 1;
		break;
	}
	//设置模数寄存器的值，会写到缓冲中
	REG_SET_VAL(FTM_MOD_REG(ftm_table[mod]), modulo);
	//使能PWM的自动载入功能，MOD、CNTIN、C(n)V、C(n+1)V在计数器到达MOD时载入缓冲器中的值
	REG_SET_VAL(FTM_PWMLOAD_REG(ftm_table[mod]), 0xFFFFFFFF);
	//设置时钟源为总线时钟，并且设置预分频因子
	REG_SET_MASK(FTM_SC_REG(ftm_table[mod]),
			(FTM_SC_CLKS(1) | FTM_SC_PS(clk_div)));
}

//==========================================================================
//函数名称: ftm_timer_enable_int
//函数返回: 无
//参数说明: mod:FTM模块号:
//             FTM_MODx，x为模块号;
//         time:FTM模块每time个计数周期产生一个中断请求，范围[1,32]
//功能概要: 使能FTM模块计时中断
//备注: 相应FTM模块的计数器需运行在向上计数模式下，
//     中断时间周期为counter_period*time，单位ms，
//     counter_period为相应FTM模块的计数周期，单位ms
//==========================================================================
void ftm_timer_enable_int(uint8 mod, uint8 time) {
	//设置产生中断的频率
	REG_CLR_MASK(FTM_CONF_REG(ftm_table[mod]), FTM_CONF_NUMTOF_MASK);
	REG_SET_MASK(FTM_CONF_REG(ftm_table[mod]), FTM_CONF_NUMTOF(time-1));
	//使能定时器溢出中断
	REG_SET_MASK(FTM_SC_REG(ftm_table[mod]), FTM_SC_TOIE_MASK);
	//允许接收该FTM模块中断请求
	ENABLE_IRQ(ftm_irq_table[mod]);
}

//==========================================================================
//函数名称: ftm_disable_int
//函数返回: 无
//参数说明: mod:FTM模块号:
//             FTM_MODx，x为模块号;
//功能概要: 关闭FTM模块中断，不会关闭FTM通道中断
//备注: 可以关闭ftm_timer_enable_int使能的中断
//==========================================================================
void ftm_disable_int(uint8 mod) {
	//关闭计数器溢出中断，未禁止接收该FTM模块中断请求，因为可能有通道会产生中断请求
	REG_CLR_MASK(FTM_SC_REG(ftm_table[mod]), FTM_SC_TOIE_MASK);
}

//==========================================================================
//函数名称: ftm_get_int
//函数返回: true:产生中断; false:未产生中断
//参数说明: mod:FTM模块号:
//             FTM_MODx，x为模块号;
//功能概要: 获取FTM模块中断标志，不会获取FTM通道中断标志
//备注: 可以获取ftm_timer_enable_int使能的中断的中断标志
//==========================================================================
bool ftm_get_int(uint8 mod) {
	//获取计数器溢出标志
	return (REG_GET_MASK(FTM_SC_REG(ftm_table[mod]), FTM_SC_TOF_MASK)) ?
			true : false;
}

//==========================================================================
//函数名称: ftm_clear_int
//函数返回: 无
//参数说明: mod:FTM模块号:
//             FTM_MODx，x为模块号;
//功能概要: 清除FTM模块中断标志，不会清除FTM通道中断标志
//备注: 可以清除ftm_timer_enable_int使能的中断的中断标志
//==========================================================================
void ftm_clear_int(uint8 mod) {
	//清除计数器溢出标志
	REG_CLR_MASK(FTM_SC_REG(ftm_table[mod]), FTM_SC_TOF_MASK);
}

//==========================================================================
//函数名称: ftm_pwm_single_init
//函数返回: 无
//参数说明: mod:FTM模块号:
//             FTM_MODx，x为模块号;
//         ch:FTM模块的通道号:
//            FTM_CHx，x为通道号;
//         mode:PWM模式:
//              FTM_PWM_MODE_EDGE_ALIGNED:边沿对齐模式;
//              FTM_PWM_MODE_CENTER_ALIGNED:中心对齐模式;
//         pol:PWM极性:
//             FTM_PWM_POL_POSITIVE:正极性;
//             FTM_PWM_POL_NEGATIVE:负极性;
//         duty:初始占空比，范围[0,FTM_PWM_DUTY_ACCURACY(10000)]，这里未限幅
//功能概要: 初始化FTM模块的通道为单通道的PWM功能
//备注: 当选择边沿对齐模式时，相应FTM模块的计数器需运行在向上计数模式下;
//     当选择中心对齐模式时，相应FTM模块的计数器需运行在上下计数模式下;
//     PWM波的频率为(1000/counter_period)，单位Hz，
//     counter_period为相应FTM模块的计数周期，单位ms
//==========================================================================
void ftm_pwm_single_init(uint8 mod, uint8 ch, uint8 mode, uint8 pol,
		uint16 duty) {
	uint8 shift;	//设置FTMx_COMBINE寄存器时的偏移量

	shift = (ch >> 1) << 3;
	//使能FTM模块通道功能
	ftm_ch_set_mux(mod, ch);
	//配置通道为相应的PWM功能
	//COMBINEn=0;COMPn=0;DECAPENn=0;SYNCEN=0;
	REG_CLR_MASK(FTM_COMBINE_REG(ftm_table[mod]),
			FTM_COMBINE_COMBINE0_MASK<<shift);
	REG_CLR_MASK(FTM_COMBINE_REG(ftm_table[mod]),
			FTM_COMBINE_COMP0_MASK<<shift);
	REG_CLR_MASK(FTM_COMBINE_REG(ftm_table[mod]),
			FTM_COMBINE_DECAPEN0_MASK<<shift);
	REG_CLR_MASK(FTM_COMBINE_REG(ftm_table[mod]),
			FTM_COMBINE_SYNCEN0_MASK<<shift);
	if (mode == FTM_PWM_MODE_EDGE_ALIGNED) {
		//MSB=1;
		REG_SET_MASK(FTM_CnSC_REG(ftm_table[mod],ch), FTM_CnSC_MSB_MASK);
	}
	//配置PWM波极性
	if (pol == FTM_PWM_POL_POSITIVE) {
		//ELSB=1;ELSA=0;
		REG_SET_MASK(FTM_CnSC_REG(ftm_table[mod],ch), FTM_CnSC_ELSB_MASK);
		REG_CLR_MASK(FTM_CnSC_REG(ftm_table[mod],ch), FTM_CnSC_ELSA_MASK);
	} else {
		//ELSA=1;
		REG_SET_MASK(FTM_CnSC_REG(ftm_table[mod],ch), FTM_CnSC_ELSA_MASK);
	}
	//设置初始占空比
	REG_SET_VAL(FTM_CnV_REG(ftm_table[mod],ch),
			(FTM_MOD_REG(ftm_table[mod]) * duty / FTM_PWM_DUTY_ACCURACY));
}

//==========================================================================
//函数名称: ftm_pwm_single_set
//函数返回: 无
//参数说明: mod:FTM模块号:
//             FTM_MODx，x为模块号;
//         ch:FTM模块的通道号:
//            FTM_CHx，x为通道号;
//         duty:占空比，范围[0,FTM_PWM_DUTY_ACCURACY(10000)]，这里未限幅
//功能概要: 设置该通道的占空比，将在下一个计数周期更新
//==========================================================================
void ftm_pwm_single_set(uint8 mod, uint8 ch, uint16 duty) {
	//设置占空比，会在下一个周期更新CnV的值
	REG_SET_VAL(FTM_CnV_REG(ftm_table[mod],ch),
			(FTM_MOD_REG(ftm_table[mod]) * duty / FTM_PWM_DUTY_ACCURACY));
}
