//===========================================================================
//文件名称：pwm.c
//功能概要：pwm底层驱动构件源文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//更新记录：2013-5-5   V3.0
//===========================================================================

#include "pwm.h"

//定义三个指针数组保存 FTMn 的地址
static const FTM_MemMapPtr ftm_table[] = { FTM0, FTM1, FTM2, FTM3 };
//=========================================================================
//函数名称：pwm_init
//功能概要：pwm模块初始化。
//参数说明：ftmNo:模块号：U_FTM0 ~ U_FTM3
//		 channel:通道号:U_FTMCH0 ~ U_FTMCH7 详见pwm.h文件定义
//       period:周期 单位微秒（比如0.1ms为100,1ms为1000）
//       duty:占空比    0~PWM_ACCURACY(在PWM.h中定义)
//函数返回：	0	成功
//			1	模块号错误
//			2	通道号错误
//			3	频率大于时钟的二分之一
//=========================================================================
uint8_t pwm_init(uint8_t ftmNo, uint8_t channel, uint32_t period, uint16_t duty) {
	uint32_t clk_hz = (bus_clk_khz * 1000) >> 1;  //bus频率/2  bus_clk_khz = 50000
	uint16_t mod;
	uint8_t sc_ps;
	uint16_t cv;

	/******************* 开启时钟 和 复用IO口*******************/
	//注，这里代码虽然长，但编译时会删掉很多没用的部分，不影响速度
	switch (ftmNo) {
	case U_FTM0:
		SIM_SCGC6 |= SIM_SCGC6_FTM0_MASK;                           //使能U_FTM0时钟
		switch (channel) {
		case U_FTMCH0:
#if(U_FTM0_CH0 == (U_PORTA|3))
			SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
			PORTA_PCR3 = PORT_PCR_MUX(0x3);		// PTA3
#endif

#if(U_FTM0_CH0 == (U_PORTC|1))
			SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
			PORTC_PCR1 = PORT_PCR_MUX(0x4);		// PTC1
#endif

			break;

		case U_FTMCH1:
#if(U_FTM0_CH1 == (U_PORTA|4))
			SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
			PORTA_PCR4 = PORT_PCR_MUX(0x3);		// PTA4
#endif

#if(U_FTM0_CH1 == (U_PORTC|2))
			SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
			PORTC_PCR2 = PORT_PCR_MUX(0x4);		// PTC2
#endif

			break;

		case U_FTMCH2:
#if(U_FTM0_CH2 == (U_PORTA|5))
			SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
			PORTA_PCR5 = PORT_PCR_MUX(0x3);		// PTA5
#endif

#if(U_FTM0_CH2 == (U_PORTC|3))
			SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
			PORTC_PCR3 = PORT_PCR_MUX(0x4);		// PTC3
#endif

#if(U_FTM0_CH2 == (U_PORTC|5))
			SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
			PORTC_PCR5 = PORT_PCR_MUX(0x7);		// PTC5
#endif

			break;

		case U_FTMCH3:
#if(U_FTM0_CH3 == (U_PORTA|6))
			SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
			PORTA_PCR6 = PORT_PCR_MUX(0x3);		// PTA6
#endif

#if(U_FTM0_CH3 == (U_PORTC|4))
			SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
			PORTC_PCR4 = PORT_PCR_MUX(0x4);		// PTC4
#endif

			break;

		case U_FTMCH4:
#if(U_FTM0_CH4 == (U_PORTA|7))
			SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
			PORTA_PCR7 = PORT_PCR_MUX(0x3);		// PTA7
#endif

#if(U_FTM0_CH4 == (U_PORTB|12))
			SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
			PORTB_PCR12 = PORT_PCR_MUX(0x4);	// PTB12
#endif

#if(U_FTM0_CH4 == (U_PORTD|4))
			SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;
			PORTD_PCR4 = PORT_PCR_MUX(0x4);		// PTD4
#endif

			break;

		case U_FTMCH5:
#if(U_FTM0_CH5 == (U_PORTA|0))
			SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
			PORTA_PCR0 = PORT_PCR_MUX(0x3);		// PTA0
#endif

#if(U_FTM0_CH5 == (U_PORTB|13))
			SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
			PORTB_PCR13 = PORT_PCR_MUX(0x4);	// PTB13
#endif

#if(U_FTM0_CH5 == (U_PORTD|5))
			SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;
			PORTD_PCR5 = PORT_PCR_MUX(0x4);		// PTD5
#endif

			break;

		case U_FTMCH6:
#if(U_FTM0_CH6 == (U_PORTA|1))
			SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
			PORTA_PCR1 = PORT_PCR_MUX(0x3);		// PTA1
#endif

#if(U_FTM0_CH6 == (U_PORTD|6))
			SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;
			PORTD_PCR6 = PORT_PCR_MUX(0x4);		// PTD6
#endif

			break;

		case U_FTMCH7:
#if(U_FTM0_CH7 == (U_PORTA|2))
			SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
			PORTA_PCR2 = PORT_PCR_MUX(0x3);		// PTA2
#endif

#if(U_FTM0_CH7 == (U_PORTD|7))
			SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;
			PORTD_PCR7 = PORT_PCR_MUX(0x4);		// PTD7
#endif

			break;

		default:
			return 2;
		}
		break;

	case U_FTM1:
		SIM_SCGC6 |= SIM_SCGC6_FTM1_MASK;                           //使能U_FTM1时钟
		switch (channel) {
		case U_FTMCH0:
#if(U_FTM1_CH0 == (U_PORTA|8))
			SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
			PORTA_PCR8 = PORT_PCR_MUX(0x3);		// PTA8
#endif

#if(U_FTM1_CH0 == (U_PORTA|12))
			SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
			PORTA_PCR12 = PORT_PCR_MUX(0x3);	// PTA12
#endif

#if(U_FTM1_CH0 == (U_PORTB|0))
			SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
			PORTB_PCR0 = PORT_PCR_MUX(0x3);		// PTB0
#endif

#if(U_FTM1_CH0 == (U_PORTB|12))
			SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
			PORTB_PCR12 = PORT_PCR_MUX(0x3);	// PTB12
#endif

			break;

		case U_FTMCH1:
#if(U_FTM1_CH1 == (U_PORTA|9))
			SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
			PORTA_PCR9 = PORT_PCR_MUX(0x3);		// PTA9
#endif

#if(U_FTM1_CH1 == (U_PORTA|13))
			SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
			PORTA_PCR13 = PORT_PCR_MUX(0x3);	// PTA13
#endif

#if(U_FTM1_CH1 == (U_PORTB|1))
			SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
			PORTB_PCR1 = PORT_PCR_MUX(0x3);		// PTB1
#endif

#if(U_FTM1_CH1 == (U_PORTB|13))
			SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
			PORTB_PCR13 = PORT_PCR_MUX(0x3);	// PTB13
#endif

			break;

		default:
			return 2;
		}
		break;

	case U_FTM2:
		SIM_SCGC3 |= SIM_SCGC3_FTM2_MASK;                           //使能U_FTM2时钟
		switch (channel) {
		case U_FTMCH0:
#if(U_FTM2_CH0 == (U_PORTA|10))
			SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
			PORTA_PCR10 = PORT_PCR_MUX(0x3);	// PTA10
#endif

#if(U_FTM2_CH0 == (U_PORTB|18))
			SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
			PORTB_PCR18 = PORT_PCR_MUX(0x3);	// PTB18
#endif

			break;

		case U_FTMCH1:
#if(U_FTM2_CH1 == (U_PORTA|11))
			SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
			PORTA_PCR11 = PORT_PCR_MUX(0x3);	// PTA11
#endif

#if(U_FTM2_CH1 == (U_PORTB|19))
			SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
			PORTB_PCR19 = PORT_PCR_MUX(0x3);	// PTB19
#endif

			break;

		default:
			return 2;
		}
		break;

	case U_FTM3:
		SIM_SCGC3 |= SIM_SCGC3_FTM3_MASK;                           //使能U_FTM3时钟
		switch (channel) {
		case U_FTMCH0:
#if(U_FTM3_CH0 == (U_PORTD|0))
			SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;
			PORTD_PCR0 = PORT_PCR_MUX(0x4);		// PTD0
#endif

#if(U_FTM3_CH0 == (U_PORTE|5))
			SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;
			PORTE_PCR5 = PORT_PCR_MUX(0x6);		// PTE5
#endif

			break;

		case U_FTMCH1:
#if(U_FTM3_CH1 == (U_PORTD|1))
			SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;
			PORTD_PCR1 = PORT_PCR_MUX(0x4);		// PTD1
#endif

#if(U_FTM3_CH1 == (U_PORTE|6))
			SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;
			PORTE_PCR6 = PORT_PCR_MUX(0x6);		// PTE6
#endif

			break;

		case U_FTMCH2:
#if(U_FTM3_CH2 == (U_PORTD|2))
			SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;
			PORTD_PCR2 = PORT_PCR_MUX(0x4);		// PTD2
#endif

#if(U_FTM3_CH2 == (U_PORTE|7))
			SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;
			PORTE_PCR7 = PORT_PCR_MUX(0x6);		// PTE7
#endif

			break;

		case U_FTMCH3:
#if(U_FTM3_CH3 == (U_PORTD|3))
			SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;
			PORTD_PCR3 = PORT_PCR_MUX(0x4);		// PTD3
#endif

#if(U_FTM3_CH3 == (U_PORTE|8))
			SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;
			PORTE_PCR8 = PORT_PCR_MUX(0x6);		// PTE8
#endif

			break;

		case U_FTMCH4:
#if(U_FTM3_CH4 == (U_PORTC|8))
			SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
			PORTC_PCR8 = PORT_PCR_MUX(0x3);		// PTC8
#endif

#if(U_FTM3_CH4 == (U_PORTE|9))
			SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;
			PORTE_PCR9 = PORT_PCR_MUX(0x6);		// PTE9
#endif

			break;

		case U_FTMCH5:
#if(U_FTM3_CH5 == (U_PORTC|9))
			SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
			PORTC_PCR9 = PORT_PCR_MUX(0x3);		// PTC9
#endif

#if(U_FTM3_CH5 == (U_PORTE|10))
			SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;
			PORTE_PCR10 = PORT_PCR_MUX(0x6);	// PTE10
#endif

			break;

		case U_FTMCH6:
#if(U_FTM3_CH6 == (U_PORTC|10))
			SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
			PORTC_PCR10 = PORT_PCR_MUX(0x3);	// PTC10
#endif

#if(U_FTM3_CH6 == (U_PORTE|11))
			SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;
			PORTE_PCR11 = PORT_PCR_MUX(0x6);	// PTE11
#endif

			break;

		case U_FTMCH7:
#if(U_FTM3_CH7 == (U_PORTC|11))
			SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
			PORTC_PCR11 = PORT_PCR_MUX(0x3);	// PTC11
#endif

#if(U_FTM3_CH7 == (U_PORTE|12))
			SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;
			PORTE_PCR12 = PORT_PCR_MUX(0x6);	// PTE12
#endif

			break;

		default:
			return 2;
		}
		break;

	default:
		return 1;
	}

	//检测 频率 是否正常 ,频率必须小于时钟二分之一
	if (!(period <= (clk_hz >> 1)))
		return 3;		//频率大于时钟的二分之一

	/*       计算分频因子       */
	mod = (clk_hz >> 16) / period;
	for (sc_ps = 0; (mod >> sc_ps) >= 1; sc_ps++)
		;
	if (period < 1000)
		sc_ps++;

	mod = (clk_hz >> sc_ps) / period;                 //频率设置因子,clk_hz = 25000000

	cv = (duty * (mod - 0 + 1)) / PWM_ACCURACY;            //占空比设置因子

	/******************** 选择输出模式为 边沿对齐PWM *******************/
	//通道状态控制，根据模式来选择 边沿或电平
	FTM_CnSC_REG(FTMx[ftmNo], channel) &= ~FTM_CnSC_ELSA_MASK;
	FTM_CnSC_REG(FTMx[ftmNo], channel) = FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;
	//MSnB:MSnA = 1x
	// ELSnB:ELSnA = 10   High-true pulses  (clear Output on match)
	// ELSnB:ELSnA = 11   Low-true pulses   (set Output on match)
	// Edge-aligned PWM  边沿对齐PWM波   《k16 reference manual.pdf》  P944  或者  《K60P144M100SF2RM.pdf》P1001

	/******************** 配置时钟和分频 ********************/
	FTM_SC_REG(FTMx[ftmNo]) = FTM_SC_CPWMS_MASK
			| FTM_SC_PS(sc_ps) | FTM_SC_CLKS(1);  //没有使能溢出中断
	FTM_CNTIN_REG(FTMx[ftmNo]) = 0; // Channel (n) Value  。设置脉冲宽度：(CnV - CNTIN).
	FTM_MOD_REG(FTMx[ftmNo]) = mod; //Modulo value模数, EPWM的周期为 ：MOD - CNTIN + 0x0001
	FTM_CNTIN_REG(FTMx[ftmNo]) = 0;              //Counter Initial Value 计数器初始化值
	FTM_CnV_REG(FTMx[ftmNo], channel) = cv;
	FTM_CNT_REG(FTMx[ftmNo]) = 0;                                 //计数器。只有低16位可用
	FTM_OUTINIT_REG(FTMx[ftmNo]) = 0;
	return 0;
}

//=========================================================================
//函数名称：pwm_set
//功能概要：设置pwm模块。
//参数说明：ftmNo:模块号：U_FTM0 ~ U_FTM3
//		 channel:通道号:U_FTMCH0 ~ U_FTMCH7 详见pwm.h文件定义
//       period:周期 单位微秒（比如0.1ms为100,1ms为1000）
//       duty:占空比    0~PWM_ACCURACY(在PWM.h中定义)
//函数返回：	0	成功
//			1	模块号、通道号错误
//			2	频率大于时钟的二分之一
//			3	占空比错误
//=========================================================================
uint8_t pwm_set(uint8_t ftmNo, uint8_t channel, uint32_t period, uint16_t duty) {
	uint32_t clk_hz = (bus_clk_khz * 1000) >> 1;        //bus频率/2
	uint16_t cv;
	uint16_t mod;
	uint8_t sc_ps;

	if (!((ftmNo == U_FTM0) || (ftmNo == U_FTM3)
			|| ((ftmNo == U_FTM1 || ftmNo == U_FTM2) && (channel <= U_FTMCH1)))) //检查传递进来的通道是否正确
		return 1;
	if (!(period <= (clk_hz >> 1)))    //检测 频率 是否正常 ,频率必须小于时钟二分之一
		return 2;
	if (!(duty <= PWM_ACCURACY))     //检测 占空比是否合理
		return 3;

	//占空比 = (CnV-CNTIN)/(MOD-CNTIN+1)
	mod = FTM_MOD_REG(FTMx[ftmNo]);        //读取 MOD 的值

	// 配置FTM通道值
	//FTM_CnV_REG(FTMx[ftmNo], channel) = cv;

	/*       计算频率设置        */
	mod = (clk_hz >> 16) / period;
	for (sc_ps = 0; (mod >> sc_ps) >= 1; sc_ps++)
		;
	mod = (clk_hz >> sc_ps) / period;
	//if(period < 1000)sc_ps++;
	cv = (duty * (mod - 0 + 1)) / PWM_ACCURACY;

	/******************** 配置时钟和分频 ********************/
	FTM_SC_REG(FTMx[ftmNo]) = FTM_SC_CPWMS_MASK
			| FTM_SC_PS(sc_ps) | FTM_SC_CLKS(1);  //没有使能溢出中断
	FTM_CNTIN_REG(FTMx[ftmNo]) = 0; // Channel (n) Value  。设置脉冲宽度：(CnV - CNTIN).
	FTM_MOD_REG(FTMx[ftmNo]) = mod; //Modulo value模数, EPWM的周期为 ：MOD - CNTIN + 0x0001
	FTM_CNTIN_REG(FTMx[ftmNo]) = 0;              //Counter Initial Value 计数器初始化值
	FTM_CnV_REG(FTMx[ftmNo], channel) = cv;                       //计数器。只有低16位可用
	FTM_OUTINIT_REG(FTMx[ftmNo]) = 0;
	return 0;
}
