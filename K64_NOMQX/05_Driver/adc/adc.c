//==========================================================================
//文件名称：adc.c
//功能概要：K64 ADC底层驱动程序源文件
//==========================================================================

#include "adc.h"

//ADC各模块基地址
static ADC_Type * const adc_table[] = ADC_BASE_PTRS;

//==========================================================================
//函数名称: adc_cal
//函数返回: true: 校对功能配置成功;
//         false:校对功能配置失败;
//参数说明: mod:ADC模块号:
//             ADC_MODx，x为模块号;
//功能概要: 校对ADC模块
//==========================================================================
static bool adc_cal(uint8 mod) {
	ADC_Type * adc_ptr;	//ADC基地址
	uint16 cal_var;	//校对值临时变量

	//获取ADC基地址
	adc_ptr = adc_table[mod];

	//正常供电配置，选择输入时钟8分频，长时间采样，输入时钟为总线时钟/2
	REG_SET_VAL(ADC_CFG1_REG(adc_ptr),
			0|ADC_CFG1_ADIV(ADC_CLK_DIV_8)|ADC_CFG1_ADLSMP_MASK|ADC_CFG1_ADICLK(1));
	//高速转换，长时间采样20个额外ADCK周期
	REG_SET_VAL(ADC_CFG2_REG(adc_ptr), 0|ADC_CFG2_ADHSC_MASK);
	//选择软件触发，并设置参考电压为VREFH和VREFL
	REG_CLR_MASK(ADC_SC2_REG(adc_ptr), ADC_SC2_ADTRG_MASK|ADC_SC2_REFSEL_MASK);
	//设置单次转换，清除采样均值数
	REG_CLR_MASK(ADC_SC3_REG(adc_ptr), ADC_SC3_ADCO_MASK|ADC_SC3_AVGS_MASK);
	//使能硬件计算均值，并设置采样均值数为32
	REG_SET_MASK(ADC_SC3_REG(adc_ptr),
			ADC_SC3_AVGE_MASK|ADC_SC3_AVGS(ADC_HARDWARE_AVG_32));
	//开始校对
	REG_SET_MASK(ADC_SC3_REG(adc_ptr), ADC_SC3_CAL_MASK);
	//等待校对完成
	while (!REG_GET_MASK(ADC_SC1_REG(adc_ptr,0), ADC_SC1_COCO_MASK)) {
	}
	//校对失败
	if (REG_GET_MASK(ADC_SC3_REG(adc_ptr), ADC_SC3_CALF_MASK)) {
		return false;
	}
	//设置正向增益寄存器
	cal_var = ADC_CLP0_REG(adc_ptr);
	cal_var += ADC_CLP1_REG(adc_ptr);
	cal_var += ADC_CLP2_REG(adc_ptr);
	cal_var += ADC_CLP3_REG(adc_ptr);
	cal_var += ADC_CLP4_REG(adc_ptr);
	cal_var += ADC_CLPS_REG(adc_ptr);
	cal_var = 0x8000 | (cal_var >> 1);
	REG_SET_VAL(ADC_PG_REG(adc_ptr), cal_var);
	//设置负向增益寄存器
	cal_var = ADC_CLM0_REG(adc_ptr);
	cal_var += ADC_CLM1_REG(adc_ptr);
	cal_var += ADC_CLM2_REG(adc_ptr);
	cal_var += ADC_CLM3_REG(adc_ptr);
	cal_var += ADC_CLM4_REG(adc_ptr);
	cal_var += ADC_CLMS_REG(adc_ptr);
	cal_var = 0x8000 | (cal_var >> 1);
	REG_SET_VAL(ADC_MG_REG(adc_ptr), cal_var);
	//清除校对标志
	REG_CLR_MASK(ADC_SC3_REG(adc_ptr), ADC_SC3_CAL_MASK);
	return true;
}

//==========================================================================
//函数名称: adc_init
//函数返回: true: 若使能校对功能，且校对功能配置成功，或者禁止校对功能;
//         false:若使能校对功能，且校对功能配置失败;
//参数说明: mod:ADC模块号:
//             ADC_MODx，x为模块号;
//         clk_div:时钟分频数:
//                 ADC_CLK_DIV_x，x为分频数大小，x可取值为:
//                 1 | 2 | 4 | 8 | 16
//         accuracy:ADC采样精度:
//                  ADC_ACCURACY_SINGLE_8_DIFF_9:  单端8位转换，差分9位转换;
//                  ADC_ACCURACY_SINGLE_10_DIFF_11:单端10位转换，差分11位转换;
//                  ADC_ACCURACY_SINGLE_12_DIFF_13:单端12位转换，差分13位转换;
//                  ADC_ACCURACY_SINGLE_DIFF_16:   单端差分均16位转换;
//         hardware_avg:硬件采样均值数量:
//                      ADC_HARDWARE_AVG_4:      4个采样均值;
//                      ADC_HARDWARE_AVG_8:      8个采样均值;
//                      ADC_HARDWARE_AVG_16:     16个采样均值;
//                      ADC_HARDWARE_AVG_32:     32个采样均值;
//                      ADC_HARDWARE_AVG_DISABLE:禁止硬件计算均值;
//         adlsts:长采样时间:
//                ADC_ADLSTS_20:     额外20个ADCK周期;
//                ADC_ADLSTS_12:     额外12个ADCK周期;
//                ADC_ADLSTS_6:      额外6个ADCK周期;
//                ADC_ADLSTS_2:      额外2个ADCK周期;
//                ADC_ADLSTS_DISABLE:禁用长时间采样，即采用短时间采样;
//         adhsc:高速配置:
//               ADC_ADHSC_NORMAL:    正常转换时序;
//               ADC_ADHSC_HIGH_SPEED:高速转换时序;
//         cal:是否使能ADC校对功能:
//             ADC_CAL_ENABLE: 使能ADC校对功能;
//             ADC_CAL_DISABLE:禁止ADC校对功能;
//功能概要: 初始化ADC模块
//备注: 采样时间 = SFCAdder + AverageNum * (BCT + LSTAdder + HSCAdder);
//     SFCAdder:如果 adlsts == ADC_ADLSTS_DISABLE，
//              则 SFCAdder = 5个ADCK周期 + 5个总线时钟周期，
//              否则 SFCAdder = 3个ADCK周期 + 5个总线时钟周期;
//     AverageNum:ADC_HARDWARE_AVG_DISABLE:AverageNum = 1;
//                ADC_HARDWARE_AVG_4:      AverageNum = 4;
//                ADC_HARDWARE_AVG_8:      AverageNum = 8;
//                ADC_HARDWARE_AVG_16:     AverageNum = 16;
//                ADC_HARDWARE_AVG_32:     AverageNum = 32;
//     BCT:单端8位转换: BCT = 17个ADCK周期; 差分9位转换: BCT = 27个ADCK周期;
//         单端10位转换:BCT = 20个ADCK周期; 差分11位转换:BCT = 30个ADCK周期;
//         单端12位转换:BCT = 20个ADCK周期; 差分13位转换:BCT = 30个ADCK周期;
//         单端16位转换:BCT = 25个ADCK周期; 差分16位转换:BCT = 34个ADCK周期;
//     LSTAdder:ADC_ADLSTS_DISABLE:LSTAdder = 0 个ADCK周期;
//              ADC_ADLSTS_20:     LSTAdder = 20个ADCK周期;
//              ADC_ADLSTS_12:     LSTAdder = 12个ADCK周期;
//              ADC_ADLSTS_6:      LSTAdder = 6 个ADCK周期;
//              ADC_ADLSTS_2:      LSTAdder = 2 个ADCK周期;
//     HSCAdder:ADC_ADHSC_NORMAL:    HSCAdder = 0个ADCK周期;
//              ADC_ADHSC_HIGH_SPEED:HSCAdder = 2个ADCK周期;
//     其中，ADCK周期为总线时钟分频后的时钟周期，在这里，总线时钟频率为48MHz
//==========================================================================
bool adc_init(uint8 mod, uint8 clk_div, uint8 accuracy, uint8 hardware_avg,
		uint8 adlsts, uint8 adhsc, uint8 cal) {
	ADC_Type * adc_ptr;	//ADC基地址
	bool result;	//校对结果

	//获取ADC基地址
	adc_ptr = adc_table[mod];
	result = true;	//默认校对成功

	//开启相应ADC模块时钟门
	if (mod == ADC_MOD0) {
		REG_SET_MASK(SIM_SCGC6, SIM_SCGC6_ADC0_MASK);
	} else {
		REG_SET_MASK(SIM_SCGC3, SIM_SCGC3_ADC1_MASK);
	}
	//重置PG与MG的值
	REG_SET_VAL(ADC_PG_REG(adc_ptr), 0x8200);
	REG_SET_VAL(ADC_MG_REG(adc_ptr), 0x8200);
	//校对ADC模块
	if (cal == ADC_CAL_ENABLE) {
		result = adc_cal(mod);
	}
	//正常供电模式，并且清除时钟分频选择、采样时间配置(即选择短时间采样)、转换模式选择、输入时钟选择(即选择总线时钟)
	REG_SET_VAL(ADC_CFG1_REG(adc_ptr), 0);
	//配置时钟频率
	if (clk_div == ADC_CLK_DIV_16) {
		//输入时钟8分频，且输入时钟为总线时钟/2
		REG_SET_MASK(ADC_CFG1_REG(adc_ptr),
				ADC_CFG1_ADIV(ADC_CLK_DIV_8)|ADC_CFG1_ADICLK(1));
	} else {
		//配置分频数
		REG_SET_MASK(ADC_CFG1_REG(adc_ptr), ADC_CFG1_ADIV(clk_div));
	}
	//配置采样精度
	REG_SET_MASK(ADC_CFG1_REG(adc_ptr), ADC_CFG1_MODE(accuracy));
	//配置长时间采样
	if (adlsts != ADC_ADLSTS_DISABLE) {
		REG_SET_MASK(ADC_CFG1_REG(adc_ptr), ADC_CFG1_ADLSMP_MASK);
		REG_CLR_MASK(ADC_CFG2_REG(adc_ptr), ADC_CFG2_ADLSTS_MASK);
		REG_SET_MASK(ADC_CFG2_REG(adc_ptr), ADC_CFG2_ADLSTS(adlsts));
	}
	//配置高速配置
	if (adhsc == ADC_ADHSC_NORMAL) {
		REG_CLR_MASK(ADC_CFG2_REG(adc_ptr), ADC_CFG2_ADHSC_MASK);
	} else {
		REG_SET_MASK(ADC_CFG2_REG(adc_ptr), ADC_CFG2_ADHSC_MASK);
	}
	//选择软件触发，并设置参考电压为VREFH和VREFL
	REG_CLR_MASK(ADC_SC2_REG(adc_ptr), ADC_SC2_ADTRG_MASK|ADC_SC2_REFSEL_MASK);
	//配置硬件计算均值
	if (hardware_avg == ADC_HARDWARE_AVG_DISABLE) {
		//禁止硬件计算均值
		REG_CLR_MASK(ADC_SC3_REG(adc_ptr), ADC_SC3_AVGE_MASK);
	} else {
		//重置硬件计算均值数
		REG_CLR_MASK(ADC_SC3_REG(adc_ptr), ADC_SC3_AVGS_MASK);
		//使能硬件计算均值，选择计算均值，并使能持续转换
		REG_SET_MASK(ADC_SC3_REG(adc_ptr),
				ADC_SC3_AVGE_MASK|ADC_SC3_ADCO_MASK|ADC_SC3_AVGS(hardware_avg));
	}
	return result;
}

//==========================================================================
//函数名称: adc_single_get_ad
//函数返回: 采样AD值，无符号
//参数说明: mod:ADC模块号:
//             ADC_MODx，x为模块号;
//         se_ch:ADC单端输入通道号:
//               ADC_SEx，x为通道号;
//         se_sel:单端输入通道选择:
//                ADC_SE_SEL_A:选择A通道;
//                ADC_SE_SEL_B:选择B通道;
//功能概要: 阻塞式地获取单端输入通道的ADC转换结果(AD值)
//备注: 若通道无AB通道选择，则se_sel无效，选择任意通道均可;
//     未配置相应引脚控制寄存器的MUX值，因此相应引脚在使用前不能被配置为其他功能;
//     建议使用adc_single_get_vtg，效率较该函数高
//==========================================================================
uint16 adc_single_get_ad(uint8 mod, uint8 se_ch, uint8 se_sel) {
	ADC_Type * adc_ptr;	//ADC基地址

	//获取ADC基地址
	adc_ptr = adc_table[mod];

	//选择AB通道
	if (se_sel == ADC_SE_SEL_A) {
		REG_CLR_MASK(ADC_CFG2_REG(adc_ptr), ADC_CFG2_MUXSEL_MASK);
	} else {
		REG_SET_MASK(ADC_CFG2_REG(adc_ptr), ADC_CFG2_MUXSEL_MASK);
	}
	//配置输入通道，并设置单端输入
	REG_SET_VAL(ADC_SC1_REG(adc_ptr,0), ADC_SC1_ADCH(se_ch));
	//等待ADC转换完成
	while (!REG_GET_MASK(ADC_SC1_REG(adc_ptr,0), ADC_SC1_COCO_MASK)) {
	}
	//返回转换结果
	return ADC_R_REG(adc_ptr, 0);
}

//==========================================================================
//函数名称: adc_single_get_vtg
//函数返回: 采样电压值，单位mV
//参数说明: mod:ADC模块号:
//             ADC_MODx，x为模块号;
//         se_ch:ADC单端输入通道号:
//               ADC_SEx，x为通道号;
//         se_sel:单端输入通道选择:
//                ADC_SE_SEL_A:选择A通道;
//                ADC_SE_SEL_B:选择B通道;
//功能概要: 阻塞式地获取单端输入通道的ADC转换结果，并转化为电压值，单位mV
//备注: 若通道无AB通道选择，则se_sel无效，选择任意通道均可;
//     未配置相应引脚控制寄存器的MUX值，因此相应引脚在使用前不能被配置为其他功能
//==========================================================================
float adc_single_get_vtg(uint8 mod, uint8 se_ch, uint8 se_sel) {
	ADC_Type * adc_ptr;	//ADC基地址
	uint16 ad_val;		//采样AD值，无符号
	float ratio;		//AD值转电压时的比例系数
	uint8 mode;			//转换模式

	//获取ADC基地址
	adc_ptr = adc_table[mod];

	//选择AB通道
	if (se_sel == ADC_SE_SEL_A) {
		REG_CLR_MASK(ADC_CFG2_REG(adc_ptr), ADC_CFG2_MUXSEL_MASK);
	} else {
		REG_SET_MASK(ADC_CFG2_REG(adc_ptr), ADC_CFG2_MUXSEL_MASK);
	}
	//配置输入通道，并设置单端输入
	REG_SET_VAL(ADC_SC1_REG(adc_ptr,0), ADC_SC1_ADCH(se_ch));
	//等待转换完成期间计算比例系数
	//获取转换模式
	mode = REG_GET_MASK(ADC_CFG1_REG(adc_ptr),
			ADC_CFG1_MODE_MASK) >> ADC_CFG1_MODE_SHIFT;
	//根据转换模式计算比例系数，除数为(1<<数据位数)
	switch (mode) {
	case ADC_ACCURACY_SINGLE_8_DIFF_9:
		ratio = ADC_VTG / 256.0f;
		break;
	case ADC_ACCURACY_SINGLE_10_DIFF_11:
		ratio = ADC_VTG / 1024.0f;
		break;
	case ADC_ACCURACY_SINGLE_12_DIFF_13:
		ratio = ADC_VTG / 4096.0f;
		break;
	case ADC_ACCURACY_SINGLE_DIFF_16:
		ratio = ADC_VTG / 65536.0f;
		break;
	}
	//等待ADC转换完成
	while (!REG_GET_MASK(ADC_SC1_REG(adc_ptr,0), ADC_SC1_COCO_MASK)) {
	}
	//获取采样AD值
	ad_val = ADC_R_REG(adc_ptr, 0);
	//转化为电压值，并返回
	return ad_val * radio;
}

//==========================================================================
//函数名称: adc_diff_get_ad
//函数返回: 采样AD值，有符号
//参数说明: mod:ADC模块号:
//             ADC_MODx，x为模块号;
//         diff_group:ADC差分输入通道组号:
//                    ADC_DIFF_GROUPx，x为通道组号;
//功能概要: 阻塞式地获取差分输入通道组的ADC转换结果(AD值)
//备注: 未配置相应引脚控制寄存器的MUX值，因此相应引脚在使用前不能被配置为其他功能;
//     建议使用adc_diff_get_vtg，效率较该函数高
//==========================================================================
int16 adc_diff_get_ad(uint8 mod, uint8 diff_group) {
	ADC_Type * adc_ptr;	//ADC基地址

	//获取ADC基地址
	adc_ptr = adc_table[mod];

	//配置输入通道组，并设置差分输入
	REG_SET_VAL(ADC_SC1_REG(adc_ptr,0),
			ADC_SC1_ADCH(diff_group) | ADC_SC1_DIFF_MASK);
	//等待ADC转换完成
	while (!REG_GET_MASK(ADC_SC1_REG(adc_ptr,0), ADC_SC1_COCO_MASK)) {
	}
	//返回转换结果
	return ADC_R_REG(adc_ptr, 0);
}

//==========================================================================
//函数名称: adc_diff_get_vtg
//函数返回: 采样电压值，单位mV
//参数说明: mod:ADC模块号:
//             ADC_MODx，x为模块号;
//         diff_group:ADC差分输入通道组号:
//                    ADC_DIFF_GROUPx，x为通道组号;
//功能概要: 阻塞式地获取差分输入通道组的ADC转换结果，并转化为电压值，单位mV
//备注: 未配置相应引脚控制寄存器的MUX值，因此相应引脚在使用前不能被配置为其他功能
//==========================================================================
float adc_diff_get_vtg(uint8 mod, uint8 diff_group) {
	ADC_Type * adc_ptr;	//ADC基地址
	int16 ad_val;		//采样AD值，有符号
	float ratio;		//AD值转电压时的比例系数
	uint8 mode;			//转换模式

	//获取ADC基地址
	adc_ptr = adc_table[mod];

	//配置输入通道组，并设置差分输入
	REG_SET_VAL(ADC_SC1_REG(adc_ptr,0),
			ADC_SC1_ADCH(diff_group) | ADC_SC1_DIFF_MASK);
	//等待转换完成期间计算比例系数
	//获取转换模式
	mode = REG_GET_MASK(ADC_CFG1_REG(adc_ptr),
			ADC_CFG1_MODE_MASK) >> ADC_CFG1_MODE_SHIFT;
	//根据转换模式计算比例系数，除数为(1<<数据位数)
	switch (mode) {
	case ADC_ACCURACY_SINGLE_8_DIFF_9:
		ratio = ADC_VTG / 256.0f;
		break;
	case ADC_ACCURACY_SINGLE_10_DIFF_11:
		ratio = ADC_VTG / 1024.0f;
		break;
	case ADC_ACCURACY_SINGLE_12_DIFF_13:
		ratio = ADC_VTG / 4096.0f;
		break;
	case ADC_ACCURACY_SINGLE_DIFF_16:
		ratio = ADC_VTG / 32768.0f;
		break;
	}
	//等待ADC转换完成
	while (!REG_GET_MASK(ADC_SC1_REG(adc_ptr,0), ADC_SC1_COCO_MASK)) {
	}
	//获取采样AD值
	ad_val = ADC_R_REG(adc_ptr, 0);
	//转化为电压值，并返回
	return ad_val * ratio;
}
