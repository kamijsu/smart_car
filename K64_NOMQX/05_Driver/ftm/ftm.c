//==========================================================================
//文件名称：ftm.c
//功能概要：K64 FTM底层驱动程序源文件
//==========================================================================

#include "ftm.h"

//FTM各模块基地址
static FTM_Type * const ftm_table[] = FTM_BASE_PTRS;
//FTM各模块各通道引脚号
static const uint8 ftm_ch_pin_table[][8] = { { FTM_MOD0_CH0_PIN,
FTM_MOD0_CH1_PIN,
FTM_MOD0_CH2_PIN, FTM_MOD0_CH3_PIN, FTM_MOD0_CH4_PIN, FTM_MOD0_CH5_PIN,
FTM_MOD0_CH6_PIN, FTM_MOD0_CH7_PIN }, { FTM_MOD1_CH0_PIN,
FTM_MOD1_CH1_PIN }, {
FTM_MOD2_CH0_PIN, FTM_MOD2_CH1_PIN }, { FTM_MOD3_CH0_PIN,
FTM_MOD3_CH1_PIN, FTM_MOD3_CH2_PIN, FTM_MOD3_CH3_PIN, FTM_MOD3_CH4_PIN,
FTM_MOD3_CH5_PIN, FTM_MOD3_CH6_PIN, FTM_MOD3_CH7_PIN } };
//FTM各模块各通道PCR的MUX值
static const uint8 ftm_ch_pcr_mux_table[][8] = { { FTM_MOD0_CH0_PCR_MUX,
FTM_MOD0_CH1_PCR_MUX, FTM_MOD0_CH2_PCR_MUX, FTM_MOD0_CH3_PCR_MUX,
FTM_MOD0_CH4_PCR_MUX, FTM_MOD0_CH5_PCR_MUX, FTM_MOD0_CH6_PCR_MUX,
FTM_MOD0_CH7_PCR_MUX }, { FTM_MOD1_CH0_PCR_MUX, FTM_MOD1_CH1_PCR_MUX }, {
FTM_MOD2_CH0_PCR_MUX, FTM_MOD2_CH1_PCR_MUX }, {
FTM_MOD3_CH0_PCR_MUX, FTM_MOD3_CH1_PCR_MUX, FTM_MOD3_CH2_PCR_MUX,
FTM_MOD3_CH3_PCR_MUX, FTM_MOD3_CH4_PCR_MUX, FTM_MOD3_CH5_PCR_MUX,
FTM_MOD3_CH6_PCR_MUX, FTM_MOD3_CH7_PCR_MUX } };
//FTM正交分解功能PHA的引脚号
static const uint8 ftm_qd_pha_pin_table[] = { FTM_MOD1_QD_PHA_PIN,
FTM_MOD2_QD_PHA_PIN };
//FTM正交分解功能PHB的引脚号
static const uint8 ftm_qd_phb_pin_table[] = { FTM_MOD1_QD_PHB_PIN,
FTM_MOD2_QD_PHB_PIN };
//FTM正交分解功能PCR的MUX值
static const uint8 ftm_qd_pcr_mux_table[] = { FTM_MOD1_QD_PCR_MUX,
FTM_MOD2_QD_PCR_MUX };
//FTM各模块中断请求号
static const IRQn_Type ftm_irq_table[] = { FTM0_IRQn, FTM1_IRQn, FTM2_IRQn,
		FTM3_IRQn };

//==========================================================================
//函数名称: ftm_init
//函数返回: 无
//参数说明: mod:FTM模块号:
//             FTM_MODx，x为模块号;
//         clk_div:时钟分频因子:
//                 FTM_CLK_DIV_x，x为分频因子大小，x可取值为:
//                 1 | 2 | 4 | 8 | 16 | 32 | 64 | 128
//         counter_mode:计数器模式:
//                      FTM_COUNTER_MODE_UP:     向上计数;
//                      FTM_COUNTER_MODE_UP_DOWN:上下计数;
//                      FTM_COUNTER_MODE_QD:     正交解码;
//         counter_period:见备注
//功能概要: 初始化FTM模块，默认未开启中断
//备注: 当选择向上计数模式或上下计数模式时，counter_period为计数器计数周期，单位μs，
//     当选择向上计数模式时，需满足48000/x*counter_period/1000<=65536，
//     当选择上下计数模式时，需满足48000/x*counter_period/1000/2<=65535，
//     48000为这里使用的总线时钟频率，单位kHz，x为FTM_CLK_DIV_x的x，
//     请注意counter_period的值，以防止计数精度丢失;
//
//     当选择正交解码模式时，counter_period无效，且此时clk_div代表计数器分频数，
//     (计数器计数个数*x)为正常计数个数，x为FTM_CLK_DIV_x的x;
//==========================================================================
void ftm_init(uint8 mod, uint8 clk_div, uint8 counter_mode,
		uint32 counter_period) {
	uint16 modulo;	//模数寄存器的值
	FTM_Type * ftm_ptr;	//FTM基地址

	//获取FTM基地址
	ftm_ptr = ftm_table[mod];

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
	REG_SET_MASK(FTM_MODE_REG(ftm_ptr), FTM_MODE_WPDIS_MASK);
	//先禁止FTM计数器，清除预分频因子，并且禁止中断
	REG_SET_VAL(FTM_SC_REG(ftm_ptr), 0);
	//清除当前计数器寄存器的值
	REG_SET_VAL(FTM_CNT_REG(ftm_ptr), 0);
	//设置计数器每次的初始值为0，会写到缓冲中
	REG_SET_VAL(FTM_CNTIN_REG(ftm_ptr), 0);
	//配置计数器模式(其实还有个自由运行模式，可以用来激活双边沿捕捉模式，但是经过测试无法使用该模式)
	switch (counter_mode) {
	case FTM_COUNTER_MODE_UP:
		//向上计数:QUADEN=0;CPWMS=0;
		REG_CLR_MASK(FTM_QDCTRL_REG(ftm_ptr), FTM_QDCTRL_QUADEN_MASK);
		REG_CLR_MASK(FTM_SC_REG(ftm_ptr), FTM_SC_CPWMS_MASK);
		//计数周期=(MOD-CNTIN+1)/时钟频率
		modulo = (FTM_CLK_FREQ >> clk_div) * counter_period / 1000 - 1;
		break;
	case FTM_COUNTER_MODE_UP_DOWN:
		//上下计数:QUADEN=0;CPWMS=1
		REG_CLR_MASK(FTM_QDCTRL_REG(ftm_ptr), FTM_QDCTRL_QUADEN_MASK);
		REG_SET_MASK(FTM_SC_REG(ftm_ptr), FTM_SC_CPWMS_MASK);
		//计数周期=2*(MOD-CNTIN)/时钟频率
		modulo = ((FTM_CLK_FREQ >> clk_div) * counter_period / 1000) >> 1;
		break;
	case FTM_COUNTER_MODE_QD:
		//正交解码:FTMEN=1;QUADEN=1;(CPWMS=0;)
		REG_SET_MASK(FTM_MODE_REG(ftm_ptr), FTM_MODE_FTMEN_MASK);
		REG_SET_MASK(FTM_QDCTRL_REG(ftm_ptr), FTM_QDCTRL_QUADEN_MASK);
		REG_CLR_MASK(FTM_SC_REG(ftm_ptr), FTM_SC_CPWMS_MASK);
		//MOD设为最大值，以方便直接通过CNT的值判断正负
		modulo = FTM_MOD_MOD_MASK;
		break;
	}
	//设置模数寄存器的值，会写到缓冲中
	REG_SET_VAL(FTM_MOD_REG(ftm_ptr), modulo);
	//使能PWM的自动载入功能，MOD、CNTIN、C(n)V、C(n+1)V在计数器到达MOD时载入缓冲器中的值
	REG_SET_VAL(FTM_PWMLOAD_REG(ftm_ptr), 0xFFFFFFFF);
	//设置时钟源为总线时钟，并且设置预分频因子
	REG_SET_MASK(FTM_SC_REG(ftm_ptr), (FTM_SC_CLKS(1) | FTM_SC_PS(clk_div)));
}

//==========================================================================
//函数名称: ftm_timer_enable_int
//函数返回: 无
//参数说明: mod:FTM模块号:
//             FTM_MODx，x为模块号;
//         time:FTM模块每time个计数周期产生一个中断请求，范围[1,32]
//功能概要: 使能FTM模块计时中断
//备注: 相应FTM模块的计数器需运行在向上计数模式下，
//     中断时间周期为counter_period*time，单位μs，
//     counter_period为相应FTM模块的计数周期，单位μs
//==========================================================================
void ftm_timer_enable_int(uint8 mod, uint8 time) {
	FTM_Type * ftm_ptr;	//FTM基地址

	//获取FTM基地址
	ftm_ptr = ftm_table[mod];

	//设置产生中断的频率
	REG_CLR_MASK(FTM_CONF_REG(ftm_ptr), FTM_CONF_NUMTOF_MASK);
	REG_SET_MASK(FTM_CONF_REG(ftm_ptr), FTM_CONF_NUMTOF(time - 1));
	//清除定时器溢出标志
	REG_CLR_MASK(FTM_SC_REG(ftm_ptr), FTM_SC_TOF_MASK);
	//使能定时器溢出中断
	REG_SET_MASK(FTM_SC_REG(ftm_ptr), FTM_SC_TOIE_MASK);
	//允许接收该FTM模块中断请求
	ENABLE_IRQ(ftm_irq_table[mod]);
}

//==========================================================================
//函数名称: ftm_timer_disable_int
//函数返回: 无
//参数说明: mod:FTM模块号:
//             FTM_MODx，x为模块号;
//功能概要: 关闭FTM模块计时中断
//==========================================================================
void ftm_timer_disable_int(uint8 mod) {
	//关闭定时器溢出中断，未禁止接收该FTM模块中断请求，因为可能有通道会产生中断请求
	REG_CLR_MASK(FTM_SC_REG(ftm_table[mod]), FTM_SC_TOIE_MASK);
}

//==========================================================================
//函数名称: ftm_timer_get_int
//函数返回: true:产生中断; false:未产生中断
//参数说明: mod:FTM模块号:
//             FTM_MODx，x为模块号;
//功能概要: 获取FTM模块计时中断标志
//==========================================================================
bool ftm_timer_get_int(uint8 mod) {
	//获取定时器溢出标志
	return (REG_GET_MASK(FTM_SC_REG(ftm_table[mod]), FTM_SC_TOF_MASK)) ?
			true : false;
}

//==========================================================================
//函数名称: ftm_timer_clear_int
//函数返回: 无
//参数说明: mod:FTM模块号:
//             FTM_MODx，x为模块号;
//功能概要: 清除FTM模块计时中断标志
//==========================================================================
void ftm_timer_clear_int(uint8 mod) {
	//清除定时器溢出标志
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
//              FTM_PWM_MODE_EDGE_ALIGNED:  边沿对齐模式;
//              FTM_PWM_MODE_CENTER_ALIGNED:中心对齐模式;
//         pol:PWM极性:
//             FTM_PWM_POL_POSITIVE:正极性;
//             FTM_PWM_POL_NEGATIVE:负极性;
//         duty:初始占空比，范围[FTM_DUTY_MIN(0),FTM_DUTY_MAX(10000)]，这里未限幅
//功能概要: 初始化FTM模块的通道为单通道的PWM功能
//备注: 当选择边沿对齐模式时，相应FTM模块的计数器需运行在向上计数模式下;
//
//     当选择中心对齐模式时，相应FTM模块的计数器需运行在上下计数模式下;
//
//     PWM波的频率为(1000000/counter_period)，单位Hz，
//     counter_period为相应FTM模块的计数周期，单位μs
//==========================================================================
void ftm_pwm_single_init(uint8 mod, uint8 ch, uint8 mode, uint8 pol,
		uint16 duty) {
	FTM_Type * ftm_ptr;	//FTM基地址
	uint8 shift;	//设置FTMx_COMBINE寄存器时的偏移量

	//获取FTM基地址
	ftm_ptr = ftm_table[mod];
	shift = (ch >> 1) << 3;	//相邻COMBINEn相差8位

	//使能FTM模块通道功能
	com_port_pin_set_mux(ftm_ch_pin_table[mod][ch],
			ftm_ch_pcr_mux_table[mod][ch]);
	//关闭FTM功能，不关闭的话无法使用单通道
	REG_CLR_MASK(FTM_MODE_REG(ftm_ptr), FTM_MODE_FTMEN_MASK);
	//关闭通道中断
	REG_CLR_MASK(FTM_CnSC_REG(ftm_ptr,ch), FTM_CnSC_CHIE_MASK);
	//配置通道为相应的PWM功能
	//COMBINEn=0;COMPn=0;DECAPENn=0;SYNCEN=0;
	REG_CLR_MASK(FTM_COMBINE_REG(ftm_ptr),
			(FTM_COMBINE_COMBINE0_MASK|FTM_COMBINE_COMP0_MASK|FTM_COMBINE_DECAPEN0_MASK|FTM_COMBINE_SYNCEN0_MASK)<<shift);
	if (mode == FTM_PWM_MODE_EDGE_ALIGNED) {
		//MSB=1;
		REG_SET_MASK(FTM_CnSC_REG(ftm_ptr,ch), FTM_CnSC_MSB_MASK);
	}
	//配置PWM波极性
	if (pol == FTM_PWM_POL_POSITIVE) {
		//ELSB=1;ELSA=0;
		REG_SET_MASK(FTM_CnSC_REG(ftm_ptr,ch), FTM_CnSC_ELSB_MASK);
		REG_CLR_MASK(FTM_CnSC_REG(ftm_ptr,ch), FTM_CnSC_ELSA_MASK);
	} else {
		//ELSA=1;
		REG_SET_MASK(FTM_CnSC_REG(ftm_ptr,ch), FTM_CnSC_ELSA_MASK);
	}
	//设置初始占空比
	ftm_pwm_single_set(mod, ch, duty);
}

//==========================================================================
//函数名称: ftm_pwm_single_set
//函数返回: 无
//参数说明: mod:FTM模块号:
//             FTM_MODx，x为模块号;
//         ch:FTM模块的通道号:
//            FTM_CHx，x为通道号;
//         duty:占空比，范围[FTM_DUTY_MIN(0),FTM_DUTY_MAX(10000)]，这里未限幅
//功能概要: 设置该通道的占空比，将在下一个计数周期更新
//==========================================================================
void ftm_pwm_single_set(uint8 mod, uint8 ch, uint16 duty) {
	FTM_Type * ftm_ptr;	//FTM基地址

	//获取FTM基地址
	ftm_ptr = ftm_table[mod];

	//设置占空比，会在下一个周期更新CnV的值
	//中心对齐模式
	if (REG_GET_MASK(FTM_SC_REG(ftm_ptr), FTM_SC_CPWMS_MASK)) {
		REG_SET_VAL(FTM_CnV_REG(ftm_ptr,ch),
				(FTM_MOD_REG(ftm_ptr) * duty / FTM_PERIOD_ACCURACY));
	} else {
		//边沿对齐模式
		REG_SET_VAL(FTM_CnV_REG(ftm_ptr,ch),
				((FTM_MOD_REG(ftm_ptr) + 1) * duty / FTM_PERIOD_ACCURACY));
	}
}

//==========================================================================
//函数名称: ftm_pwm_combine_init
//函数返回: 无
//参数说明: mod:FTM模块号:
//             FTM_MODx，x为模块号;
//         ch_group:FTM模块的通道组号:
//                  FTM_CH_GROUPx，x为通道组号;
//         mode:PWM模式:
//              FTM_PWM_MODE_COMBINE:      联合模式，两通道输出相同;
//              FTM_PWM_MODE_COMPLEMENTARY:互补模式，两通道输出相反;
//         pol:PWM极性:
//             FTM_PWM_POL_POSITIVE:正极性;
//             FTM_PWM_POL_NEGATIVE:负极性;
//         duty1:初始占空比1，范围[FTM_DUTY_MIN(0),FTM_DUTY_MAX(10000)]，这里未限幅
//         duty2:初始占空比2，范围[FTM_DUTY_MIN(0),FTM_DUTY_MAX(10000)]，这里未限幅
//功能概要: 初始化FTM模块的通道组为双通道的PWM功能
//备注: 相应FTM模块的计数器需运行在向上计数模式下;
//     最终PWM波的占空比为(duty2-duty1)，duty1需小于duty2，若duty1大于duty2，
//     PWM波极性将再次反转;
//     PWM波的频率为(1000000/counter_period)，单位Hz，
//     counter_period为相应FTM模块的计数周期，单位μs;
//     偶数通道输出的PWM波，无论选择哪种模式，都是一样的，即互补模式只针对奇数通道
//==========================================================================
void ftm_pwm_combine_init(uint8 mod, uint8 ch_group, uint8 mode, uint8 pol,
		uint16 duty1, uint16 duty2) {
	FTM_Type * ftm_ptr;	//FTM基地址
	uint8 ch0, ch1;	//两个通道号
	uint8 shift;	//设置FTMx_COMBINE寄存器时的偏移量

	//获取FTM基地址
	ftm_ptr = ftm_table[mod];
	ch0 = ch_group << 1;	//偶数通道
	ch1 = ch0 + 1;			//奇数通道
	shift = ch_group << 3;	//相邻COMBINEn相差8位

	//使能FTM模块通道功能
	com_port_pin_set_mux(ftm_ch_pin_table[mod][ch0],
			ftm_ch_pcr_mux_table[mod][ch0]);
	com_port_pin_set_mux(ftm_ch_pin_table[mod][ch1],
			ftm_ch_pcr_mux_table[mod][ch1]);
	//使能FTM功能
	REG_SET_MASK(FTM_MODE_REG(ftm_ptr), FTM_MODE_FTMEN_MASK);
	//关闭通道中断
	REG_CLR_MASK(FTM_CnSC_REG(ftm_ptr,ch0), FTM_CnSC_CHIE_MASK);
	REG_CLR_MASK(FTM_CnSC_REG(ftm_ptr,ch1), FTM_CnSC_CHIE_MASK);
	//配置通道为相应的PWM功能
	//COMBINEn=1;DECAPENn=0;SYNCEN=1;
	REG_SET_MASK(FTM_COMBINE_REG(ftm_ptr),
			(FTM_COMBINE_COMBINE0_MASK|FTM_COMBINE_SYNCEN0_MASK)<<shift);
	REG_CLR_MASK(FTM_COMBINE_REG(ftm_ptr), FTM_COMBINE_DECAPEN0_MASK<<shift);
	if (mode == FTM_PWM_MODE_COMBINE) {
		//COMPn=0;
		REG_CLR_MASK(FTM_COMBINE_REG(ftm_ptr), FTM_COMBINE_COMP0_MASK<<shift);
	} else {
		//COMPn=1;
		REG_SET_MASK(FTM_COMBINE_REG(ftm_ptr), FTM_COMBINE_COMP0_MASK<<shift);
	}
	//配置PWM波极性
	if (pol == FTM_PWM_POL_POSITIVE) {
		//ELSB=1;ELSA=0;
		REG_SET_MASK(FTM_CnSC_REG(ftm_ptr,ch0), FTM_CnSC_ELSB_MASK);
		REG_CLR_MASK(FTM_CnSC_REG(ftm_ptr,ch0), FTM_CnSC_ELSA_MASK);
		REG_SET_MASK(FTM_CnSC_REG(ftm_ptr,ch1), FTM_CnSC_ELSB_MASK);
		REG_CLR_MASK(FTM_CnSC_REG(ftm_ptr,ch1), FTM_CnSC_ELSA_MASK);
	} else {
		//ELSA=1;
		REG_SET_MASK(FTM_CnSC_REG(ftm_ptr,ch0), FTM_CnSC_ELSA_MASK);
		REG_SET_MASK(FTM_CnSC_REG(ftm_ptr,ch1), FTM_CnSC_ELSA_MASK);
	}
	//最小装载点使能，即计数器达到CNTIN值时，为同步的一个载入点;禁止硬件触发同步;同步时计数器继续计数
	REG_SET_VAL(FTM_SYNC_REG(ftm_ptr), FTM_SYNC_CNTMIN_MASK);
	//设置初始占空比
	ftm_pwm_combine_set(mod, ch_group, duty1, duty2);
}

//==========================================================================
//函数名称: ftm_pwm_combine_set
//函数返回: 无
//参数说明: mod:FTM模块号:
//             FTM_MODx，x为模块号;
//         ch_group:FTM模块的通道组号:
//                  FTM_CH_GROUPx，x为通道组号;
//         duty1:占空比1，范围[FTM_DUTY_MIN(0),FTM_DUTY_MAX(10000)]，这里未限幅
//         duty2:占空比2，范围[FTM_DUTY_MIN(0),FTM_DUTY_MAX(10000)]，这里未限幅
//功能概要: 设置该通道组的占空比，将在下一个计数周期更新
//备注: 最终PWM波的占空比为(duty2-duty1)，duty1需小于duty2，若duty1大于duty2，
//     PWM波极性将再次反转
//==========================================================================
void ftm_pwm_combine_set(uint8 mod, uint8 ch_group, uint16 duty1, uint16 duty2) {
	FTM_Type * ftm_ptr;	//FTM基地址

	//获取FTM基地址
	ftm_ptr = ftm_table[mod];

	//设置偶数通道占空比
	REG_SET_VAL(FTM_CnV_REG(ftm_ptr,ch_group << 1),
			((FTM_MOD_REG(ftm_ptr) + 1) * duty1 / FTM_PERIOD_ACCURACY));
	//设置奇数通道占空比
	REG_SET_VAL(FTM_CnV_REG(ftm_ptr,(ch_group << 1) + 1),
			((FTM_MOD_REG(ftm_ptr) + 1) * duty2 / FTM_PERIOD_ACCURACY));
	//软件触发下一个载入点同步更新CnV
	REG_SET_MASK(FTM_SYNC_REG(ftm_ptr), FTM_SYNC_SWSYNC_MASK);
}

//==========================================================================
//函数名称: ftm_ic_init
//函数返回: 无
//参数说明: mod:FTM模块号:
//             FTM_MODx，x为模块号;
//         ch:FTM模块的通道号:
//            FTM_CHx，x为通道号;
//         mode:输入捕捉的模式:
//              FTM_IC_MODE_RISING_EDGE: 上升沿捕捉;
//              FTM_IC_MODE_FALLING_EDGE:下降沿捕捉;
//              FTM_IC_MODE_DOUBLE_EDGE: 双边沿捕捉;
//功能概要: 初始化FTM模块的通道为输入捕捉功能，默认未开启中断
//备注: 相应FTM模块的计数器需运行在向上计数模式下;未上下拉引脚电阻
//==========================================================================
void ftm_ic_init(uint8 mod, uint8 ch, uint8 mode) {
	FTM_Type * ftm_ptr;	//FTM基地址
	uint8 shift;	//设置FTMx_COMBINE寄存器时的偏移量

	//获取FTM基地址
	ftm_ptr = ftm_table[mod];
	shift = (ch >> 1) << 3;	//相邻COMBINEn相差8位

	//使能FTM模块通道功能
	com_port_pin_set_mux(ftm_ch_pin_table[mod][ch],
			ftm_ch_pcr_mux_table[mod][ch]);
	//关闭FTM功能，不关闭的话无法使用单通道
	REG_CLR_MASK(FTM_MODE_REG(ftm_ptr), FTM_MODE_FTMEN_MASK);
	//关闭通道中断
	REG_CLR_MASK(FTM_CnSC_REG(ftm_ptr,ch), FTM_CnSC_CHIE_MASK);
	//配置通道为输入捕捉功能
	//COMBINEn=0;COMPn=0;DECAPENn=0;SYNCEN=0;
	REG_CLR_MASK(FTM_COMBINE_REG(ftm_ptr),
			(FTM_COMBINE_COMBINE0_MASK|FTM_COMBINE_COMP0_MASK|FTM_COMBINE_DECAPEN0_MASK|FTM_COMBINE_SYNCEN0_MASK)<<shift);
	switch (mode) {
	case FTM_IC_MODE_RISING_EDGE:
		//ELSB=0;ELSA=1;
		REG_CLR_MASK(FTM_CnSC_REG(ftm_ptr,ch), FTM_CnSC_ELSB_MASK);
		REG_SET_MASK(FTM_CnSC_REG(ftm_ptr,ch), FTM_CnSC_ELSA_MASK);
		break;
	case FTM_IC_MODE_FALLING_EDGE:
		//ELSB=1;ELSA=0;
		REG_SET_MASK(FTM_CnSC_REG(ftm_ptr,ch), FTM_CnSC_ELSB_MASK);
		REG_CLR_MASK(FTM_CnSC_REG(ftm_ptr,ch), FTM_CnSC_ELSA_MASK);
		break;
	case FTM_IC_MODE_DOUBLE_EDGE:
		//ELSB=1;ELSA=1;
		REG_SET_MASK(FTM_CnSC_REG(ftm_ptr,ch),
				FTM_CnSC_ELSB_MASK|FTM_CnSC_ELSA_MASK);
		break;
	}
	//MSB=0;MSA=0;
	REG_CLR_MASK(FTM_CnSC_REG(ftm_ptr,ch), FTM_CnSC_MSB_MASK|FTM_CnSC_MSA_MASK);
}

//==========================================================================
//函数名称: ftm_ic_get_ratio
//函数返回: 满足捕捉条件时的比例
//参数说明: mod:FTM模块号:
//             FTM_MODx，x为模块号;
//         ch:FTM模块的通道号:
//            FTM_CHx，x为通道号;
//功能概要: 获取当满足捕捉条件时，计数器计数个数占整个计数周期的比例
//备注: 返回值除以FTM_PERIOD_ACCURACY(10000)为百分比的比例;
//     可以用来测量PWM波占空比，比如:
//     上升沿捕捉模式:返回值为相同计数器、边沿对齐模式、负极性PWM波占空比;
//     下降沿捕捉模式:返回值为相同计数器、边沿对齐模式、正极性PWM波占空比;
//==========================================================================
uint16 ftm_ic_get_ratio(uint8 mod, uint8 ch) {
	FTM_Type * ftm_ptr;	//FTM基地址

	//获取FTM基地址
	ftm_ptr = ftm_table[mod];

	return FTM_CnV_REG(ftm_ptr,ch) * FTM_PERIOD_ACCURACY
			/ (FTM_MOD_REG(ftm_ptr) + 1);
}

//==========================================================================
//函数名称: ftm_oc_init
//函数返回: 无
//参数说明: mod:FTM模块号:
//             FTM_MODx，x为模块号;
//         ch:FTM模块的通道号:
//            FTM_CHx，x为通道号;
//         mode:输出比较的模式:
//              FTM_OC_MODE_TOGGLE:比较成功后反转电平;
//              FTM_OC_MODE_SET:   比较成功后置高电平;
//              FTM_OC_MODE_CLEAR: 比较成功后置低电平;
//         ratio:比较成功的时间占整个计数周期的比例，范围[0,FTM_PERIOD_ACCURACY(10000))
//功能概要: 初始化FTM模块的通道为输出比较功能，默认未开启中断
//备注: 相应FTM模块的计数器需运行在向上计数模式下;
//     ratio除以FTM_PERIOD_ACCURACY(10000)为百分比的比例
//==========================================================================
void ftm_oc_init(uint8 mod, uint8 ch, uint8 mode, uint16 ratio) {
	FTM_Type * ftm_ptr;	//FTM基地址
	uint8 shift;	//设置FTMx_COMBINE寄存器时的偏移量

	//获取FTM基地址
	ftm_ptr = ftm_table[mod];
	shift = (ch >> 1) << 3;	//相邻COMBINEn相差8位

	//使能FTM模块通道功能
	com_port_pin_set_mux(ftm_ch_pin_table[mod][ch],
			ftm_ch_pcr_mux_table[mod][ch]);
	//关闭FTM功能
	REG_CLR_MASK(FTM_MODE_REG(ftm_ptr), FTM_MODE_FTMEN_MASK);
	//关闭通道中断
	REG_CLR_MASK(FTM_CnSC_REG(ftm_ptr,ch), FTM_CnSC_CHIE_MASK);
	//配置通道为输出比较功能
	//COMBINEn=0;COMPn=0;DECAPENn=0;SYNCEN=0;
	REG_CLR_MASK(FTM_COMBINE_REG(ftm_ptr),
			(FTM_COMBINE_COMBINE0_MASK|FTM_COMBINE_COMP0_MASK|FTM_COMBINE_DECAPEN0_MASK|FTM_COMBINE_SYNCEN0_MASK)<<shift);
	//设置输出比较模式
	ftm_oc_change_mode(mod, ch, mode);
	//MSB=0;MSA=1;
	REG_CLR_MASK(FTM_CnSC_REG(ftm_ptr,ch), FTM_CnSC_MSB_MASK);
	REG_SET_MASK(FTM_CnSC_REG(ftm_ptr,ch), FTM_CnSC_MSA_MASK);
	//设置比较成功点
	ftm_oc_set_ratio(mod, ch, ratio);
}

//==========================================================================
//函数名称: ftm_oc_change_mode
//函数返回: 无
//参数说明: mod:FTM模块号:
//             FTM_MODx，x为模块号;
//         ch:FTM模块的通道号:
//            FTM_CHx，x为通道号;
//         mode:输出比较的模式:
//              FTM_OC_MODE_TOGGLE:比较成功后反转电平;
//              FTM_OC_MODE_SET:   比较成功后置高电平;
//              FTM_OC_MODE_CLEAR: 比较成功后置低电平;
//功能概要: 更改输出比较功能通道的模式
//==========================================================================
void ftm_oc_change_mode(uint8 mod, uint8 ch, uint8 mode) {
	FTM_Type * ftm_ptr;	//FTM基地址

	//获取FTM基地址
	ftm_ptr = ftm_table[mod];

	switch (mode) {
	case FTM_OC_MODE_TOGGLE:
		//ELSB=0;ELSA=1;
		REG_CLR_MASK(FTM_CnSC_REG(ftm_ptr,ch), FTM_CnSC_ELSB_MASK);
		REG_SET_MASK(FTM_CnSC_REG(ftm_ptr,ch), FTM_CnSC_ELSA_MASK);
		break;
	case FTM_OC_MODE_SET:
		//ELSB=1;ELSA=1;
		REG_SET_MASK(FTM_CnSC_REG(ftm_ptr,ch),
				FTM_CnSC_ELSB_MASK|FTM_CnSC_ELSA_MASK);
		break;
	case FTM_OC_MODE_CLEAR:
		//ELSB=1;ELSA=0;
		REG_SET_MASK(FTM_CnSC_REG(ftm_ptr,ch), FTM_CnSC_ELSB_MASK);
		REG_CLR_MASK(FTM_CnSC_REG(ftm_ptr,ch), FTM_CnSC_ELSA_MASK);
		break;
	}
}

//==========================================================================
//函数名称: ftm_oc_set_ratio
//函数返回: 无
//参数说明: mod:FTM模块号:
//             FTM_MODx，x为模块号;
//         ch:FTM模块的通道号:
//            FTM_CHx，x为通道号;
//         ratio:比较成功的时间占整个计数周期的比例，范围[0,FTM_PERIOD_ACCURACY(10000))
//功能概要: 更改输出比较功能通道的比较成功时间
//==========================================================================
void ftm_oc_set_ratio(uint8 mod, uint8 ch, uint16 ratio) {
	FTM_Type * ftm_ptr;	//FTM基地址

	//获取FTM基地址
	ftm_ptr = ftm_table[mod];

	REG_SET_VAL(FTM_CnV_REG(ftm_ptr,ch),
			((FTM_MOD_REG(ftm_ptr) + 1) * ratio / FTM_PERIOD_ACCURACY));
}

//==========================================================================
//函数名称: ftm_ch_enable_int
//函数返回: 无
//参数说明: mod:FTM模块号:
//             FTM_MODx，x为模块号;
//         ch:FTM模块的通道号:
//            FTM_CHx，x为通道号;
//功能概要: 使能通道中断
//备注: 可以使能输入捕捉和输出比较功能通道的中断:
//     输入捕捉:当输入的信号满足捕捉条件时，产生中断;
//     输出比较:当比较成功时，产生中断;
//==========================================================================
void ftm_ch_enable_int(uint8 mod, uint8 ch) {
	FTM_Type * ftm_ptr;	//FTM基地址

	//获取FTM基地址
	ftm_ptr = ftm_table[mod];

	//清除通道中断标志
	REG_CLR_MASK(FTM_CnSC_REG(ftm_ptr,ch), FTM_CnSC_CHF_MASK);
	//使能通道事件中断
	REG_SET_MASK(FTM_CnSC_REG(ftm_ptr,ch), FTM_CnSC_CHIE_MASK);
	//允许接收该FTM模块中断请求
	ENABLE_IRQ(ftm_irq_table[mod]);
}

//==========================================================================
//函数名称: ftm_ch_disable_int
//函数返回: 无
//参数说明: mod:FTM模块号:
//             FTM_MODx，x为模块号;
//         ch:FTM模块的通道号:
//            FTM_CHx，x为通道号;
//功能概要: 关闭通道中断
//==========================================================================
void ftm_ch_disable_int(uint8 mod, uint8 ch) {
	//关闭通道事件中断，未禁止接收该FTM模块中断请求，因为可能有别的通道会产生中断请求
	REG_CLR_MASK(FTM_CnSC_REG(ftm_table[mod],ch), FTM_CnSC_CHIE_MASK);
}

//==========================================================================
//函数名称: ftm_ch_get_int
//函数返回: true:产生中断; false:未产生中断;
//参数说明: mod:FTM模块号:
//             FTM_MODx，x为模块号;
//         ch:FTM模块的通道号:
//            FTM_CHx，x为通道号;
//功能概要: 获取通道的中断标志
//==========================================================================
bool ftm_ch_get_int(uint8 mod, uint8 ch) {
	return (REG_GET_MASK(FTM_CnSC_REG(ftm_table[mod],ch), FTM_CnSC_CHF_MASK)) ?
			true : false;
}

//==========================================================================
//函数名称: ftm_ch_clear_int
//函数返回: 无
//参数说明: mod:FTM模块号:
//             FTM_MODx，x为模块号;
//         ch:FTM模块的通道号:
//            FTM_CHx，x为通道号;
//功能概要: 清除通道的中断标志
//==========================================================================
void ftm_ch_clear_int(uint8 mod, uint8 ch) {
	REG_CLR_MASK(FTM_CnSC_REG(ftm_table[mod],ch), FTM_CnSC_CHF_MASK);
}

//==========================================================================
//函数名称: ftm_qd_init
//函数返回: 无
//参数说明: mod:FTM模块号:
//             FTM_MODx，x为模块号;这里仅有FTM_MOD1和FTM_MOD2可以使用;
//         mode:正交解码功能模式:
//              FTM_QD_MODE_PHAB:           AB相编码器;
//              FTM_QD_MODE_COUNT_DIRECTION:方向-脉冲型编码器;
//         dir:正交解码功能方向(具体见备注):
//             FTM_QD_DIR_POSITIVE:正方向;
//             FTM_QD_DIR_NEGATIVE:负方向;
//功能概要: 初始化FTM模块为正交解码功能，已上拉引脚电阻
//备注: 相应FTM模块的计数器需运行在正交解码模式下;
//     引脚号可以通过宏定义选择;
//
//     若选择AB相编码器模式，两个引脚分别接在编码器的AB相上;此时测量的为编码器的4倍频，
//     即若编码器分辨率为500，即500个A相脉冲和500个B相脉冲每圈，计数器会计数2000次;
//     编码器旋转圈数=(计数器计数个数*x)/4/编码器分辨率，x为FTM_CLK_DIV_x的x;
//     当方向为正方向时，顺时针旋转时计数器增加，逆时针旋转时计数器减少;
//     当方向为负方向时，顺时针旋转时计数器减少，逆时针旋转时计数器增加;
//
//     若选择方向-脉冲型编码器模式，A相引脚定义计数率，B相引脚决定计数器的方向，
//     当B相引脚电平不改变时，编码器无论怎么旋转，计数器都是增加/减少的;
//     当方向为正方向时，B相引脚为高电平时计数器增加，为低电平时计数器减少;
//     当方向为负方向时，B相引脚为高电平时计数器减少，为低电平时计数器增加;
//     编码器旋转圈数=(计数器计数个数*x)/编码器分辨率，x为FTM_CLK_DIV_x的x;
//==========================================================================
void ftm_qd_init(uint8 mod, uint8 mode, uint8 dir) {
	FTM_Type * ftm_ptr;	//FTM基地址
	uint8 pha_port_pin, phb_port_pin;	//pha与phb的端口号与引脚号
	uint8 pcr_mux;	//PCR的MUX值

	//获取FTM基地址
	ftm_ptr = ftm_table[mod];
	//获取端口号与引脚号
	pha_port_pin = ftm_qd_pha_pin_table[mod - FTM_MOD1];
	phb_port_pin = ftm_qd_phb_pin_table[mod - FTM_MOD1];
	//获取PCR的MUX值
	pcr_mux = ftm_qd_pcr_mux_table[mod - FTM_MOD1];

	//设置引脚为FTM正交解码功能
	com_port_pin_set_mux(pha_port_pin, pcr_mux);
	com_port_pin_set_mux(phb_port_pin, pcr_mux);
	//上拉引脚电阻
	gpio_pull(pha_port_pin, GPIO_LEVEL_HIGH);
	gpio_pull(phb_port_pin, GPIO_LEVEL_HIGH);
	//配置正交解码模式
	if (mode == FTM_QD_MODE_PHAB) {
		//AB相
		REG_CLR_MASK(FTM_QDCTRL_REG(ftm_ptr), FTM_QDCTRL_QUADMODE_MASK);
		//根据方向配置极性
		if (dir == FTM_QD_DIR_POSITIVE) {
			//AB相均为正常极性
			REG_CLR_MASK(FTM_QDCTRL_REG(ftm_ptr),
					(FTM_QDCTRL_PHAPOL_MASK|FTM_QDCTRL_PHBPOL_MASK));
		} else {
			//A相为正常极性，B相为倒置极性
			REG_CLR_MASK(FTM_QDCTRL_REG(ftm_ptr), FTM_QDCTRL_PHAPOL_MASK);
			REG_SET_MASK(FTM_QDCTRL_REG(ftm_ptr), (FTM_QDCTRL_PHBPOL_MASK));
		}
	} else {
		//方向-脉冲型
		REG_SET_MASK(FTM_QDCTRL_REG(ftm_ptr), FTM_QDCTRL_QUADMODE_MASK);
		if (dir == FTM_QD_DIR_POSITIVE) {
			//AB相均为正常极性
			REG_CLR_MASK(FTM_QDCTRL_REG(ftm_ptr),
					(FTM_QDCTRL_PHAPOL_MASK|FTM_QDCTRL_PHBPOL_MASK));
		} else {
			//AB相均为倒置极性
			REG_SET_MASK(FTM_QDCTRL_REG(ftm_ptr),
					(FTM_QDCTRL_PHAPOL_MASK|FTM_QDCTRL_PHBPOL_MASK));
		}
	}
}

//==========================================================================
//函数名称: ftm_qd_get_count
//函数返回: 计数器计数个数
//参数说明: mod:FTM模块号:
//             FTM_MODx，x为模块号;这里仅有FTM_MOD1和FTM_MOD2可以使用;
//功能概要: 获取计数器当前计数个数
//备注: 计数个数带方向，正负分别代表不同方向，具体定义与设置有关
//==========================================================================
int16 ftm_qd_get_count(uint8 mod) {
	return FTM_CNT_REG(ftm_table[mod]);
}

//==========================================================================
//函数名称: ftm_qd_clear_count
//函数返回: 无
//参数说明: mod:FTM模块号:
//             FTM_MODx，x为模块号;这里仅有FTM_MOD1和FTM_MOD2可以使用;
//功能概要: 清除计数器计数个数
//==========================================================================
void ftm_qd_clear_count(uint8 mod) {
	REG_SET_VAL(FTM_CNT_REG(ftm_table[mod]), 0);
}

//==========================================================================
//函数名称: ftm_qd_get_and_clear_count
//函数返回: 计数器计数个数
//参数说明: mod:FTM模块号:
//             FTM_MODx，x为模块号;这里仅有FTM_MOD1和FTM_MOD2可以使用;
//功能概要: 获取计数器当前计数个数，并且清除计数个数
//备注: 计数个数带方向，正负分别代表不同方向，具体定义与设置有关
//==========================================================================
int16 ftm_qd_get_and_clear_count(uint8 mod) {
	FTM_Type * ftm_ptr;	//FTM基地址
	int16 count;

	//获取FTM基地址
	ftm_ptr = ftm_table[mod];

	count = FTM_CNT_REG(ftm_ptr);
	REG_SET_VAL(FTM_CNT_REG(ftm_ptr), 0);
	return count;
}
