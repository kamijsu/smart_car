//==========================================================================
//文件名称：adc.h
//功能概要：K64 ADC底层驱动程序头文件
//==========================================================================

#ifndef _ADC_H
#define _ADC_H

//包含公共头文件
#include "common.h"

//定义ADC模块号
#define ADC_MOD0	(0)
#define ADC_MOD1	(1)

//定义ADC差分输入通道组号，由于使用的K64封装类型为100引脚LQFP，有些引脚号使用数字表示，还有些该封装无该通道的用??表示
#define ADC_DIFF_GROUP0		(0)		//ADC0_DP0:18    ADC0_DM0:19    ADC1_DP0:20    ADC1_DM0:21
#define ADC_DIFF_GROUP1		(1)		//ADC0_DP1:14    ADC0_DM1:15    ADC1_DP1:16    ADC1_DM1:17
#define ADC_DIFF_GROUP2		(2)		//ADC0_DP2:E2    ADC0_DM2:E3
#define ADC_DIFF_GROUP3		(3)		//ADC0_DP3:20    ADC0_DM3:21    ADC1_DP3:18    ADC1_DM3:19
#define ADC_DIFF_GROUP_TEMP	(26)	//内置温度传感器，无引脚

//定义ADC单端输入通道号，引脚号说明同上
#define ADC_SE4				(4)		//ADC0_SE4b:C2   ADC1_SE4a:E0   ADC1_SE4b:C8
#define ADC_SE5				(5)		//ADC0_SE5b:D1   ADC1_SE5a:E1   ADC1_SE5b:C9
#define ADC_SE6				(6)		//ADC0_SE6b:D5   ADC1_SE6a:E2   ADC1_SE6b:C10
#define ADC_SE7				(7)		//ADC0_SE7b:D6   ADC1_SE7a:E3   ADC1_SE7b:C11
#define ADC_SE8				(8)		//ADC0_SE8: B0   ADC1_SE8: B0
#define ADC_SE9				(9)		//ADC0_SE9: B1   ADC1_SE9: B1
#define ADC_SE10			(10)	//ADC0_SE10:A7   ADC1_SE10:B4
#define ADC_SE11			(11)	//ADC0_SE11:A8   ADC1_SE11:B5
#define ADC_SE12			(12)	//ADC0_SE12:B2   ADC1_SE12:B6
#define ADC_SE13			(13)	//ADC0_SE13:B3   ADC1_SE13:B7
#define ADC_SE14			(14)	//ADC0_SE14:C0   ADC1_SE14:B10
#define ADC_SE15			(15)	//ADC0_SE15:C1   ADC1_SE15:B11
#define ADC_SE16			(16)	//ADC0_SE16:??   ADC1_SE16:??
#define ADC_SE17			(17)	//ADC0_SE17:E24  ADC1_SE17:A17
#define ADC_SE18			(18)	//ADC0_SE18:E25  ADC1_SE18:26
#define ADC_SE21			(21)	//ADC0_SE21:??
#define ADC_SE22			(22)	//ADC0_SE22:??
#define ADC_SE23			(23)	//ADC0_SE23:27   ADC1_SE23:??
#define ADC_SE_TEMP			(26)	//内置温度传感器，无引脚

//定义ADC单端输入通道选择
#define ADC_SE_SEL_A		(0)		//选择A通道
#define ADC_SE_SEL_B		(1)		//选择B通道

//定义ADC参考电压，单位mV，这里默认选择VREFH和VREFL(VTG为voltage简写)
#define ADC_VTG		(3300.0f)

//定义ADC模块时钟分频数，这里选择总线时钟
#define ADC_CLK_DIV_1		(0)		//总线时钟1分频
#define ADC_CLK_DIV_2		(1)		//总线时钟2分频
#define ADC_CLK_DIV_4		(2)		//总线时钟4分频
#define ADC_CLK_DIV_8		(3)		//总线时钟8分频
#define ADC_CLK_DIV_16		(4)		//总线时钟16分频(选择16分频时，输入时钟为总线时钟/2)

//定义ADC采样精度，这里默认选择软件触发采样
#define ADC_ACCURACY_SINGLE_8_DIFF_9	(0)		//单端8位转换，差分9位转换
#define ADC_ACCURACY_SINGLE_10_DIFF_11	(2)		//单端10位转换，差分11位转换
#define ADC_ACCURACY_SINGLE_12_DIFF_13	(1)		//单端12位转换，差分13位转换
#define ADC_ACCURACY_SINGLE_DIFF_16		(3)		//单端差分均16位转换

//定义ADC硬件采样均值数量(若使用硬件计算均值，则默认使能持续转换)
#define ADC_HARDWARE_AVG_4			(0)		//4个采样均值
#define ADC_HARDWARE_AVG_8			(1)		//8个采样均值
#define ADC_HARDWARE_AVG_16			(2)		//16个采样均值
#define ADC_HARDWARE_AVG_32			(3)		//32个采样均值
#define ADC_HARDWARE_AVG_DISABLE	(4)		//禁止硬件计算均值

//定义ADC长采样时间
#define ADC_ADLSTS_20		(0)		//额外20个ADCK周期
#define ADC_ADLSTS_12		(1)		//额外12个ADCK周期
#define ADC_ADLSTS_6		(2)		//额外6个ADCK周期
#define ADC_ADLSTS_2		(3)		//额外2个ADCK周期
#define ADC_ADLSTS_DISABLE	(4)		//禁用长时间采样，即采用短时间采样

//定义ADC高速配置
#define ADC_ADHSC_NORMAL		(0)	//正常转换时序
#define ADC_ADHSC_HIGH_SPEED	(1)	//高速转换时序

//定义是否使能ADC校对功能
#define ADC_CAL_ENABLE			(1)	//使能ADC校对功能
#define ADC_CAL_DISABLE			(0)	//禁止ADC校对功能

//==========================================================================
//函数名称: adc_init
//函数返回: true: 若使能校对功能，且校对功能配置成功，或者禁止校对功能;
//         false:若使能校对功能，且校对功能配置失败;
//参数说明: mod:ADC模块号:
//             ADC_MODx，x为模块号;
//         clk_div:时钟分频数:
//                 ADC_CLK_DIV_x，x为分频数大小;
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
		uint8 adlsts, uint8 adhsc, uint8 cal);

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
uint16 adc_single_get_ad(uint8 mod, uint8 se_ch, uint8 se_sel);

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
float adc_single_get_vtg(uint8 mod, uint8 se_ch, uint8 se_sel);

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
int16 adc_diff_get_ad(uint8 mod, uint8 diff_group);

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
float adc_diff_get_vtg(uint8 mod, uint8 diff_group);

#endif
