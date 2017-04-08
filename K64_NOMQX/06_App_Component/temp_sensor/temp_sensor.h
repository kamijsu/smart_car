//==========================================================================
//文件名称：temp_sensor.h
//功能概要：温度传感器应用驱动程序头文件
//==========================================================================

#ifndef _TEMP_SENSOR_H
#define _TEMP_SENSOR_H

#include "common.h"
#include "adc.h"

//定义温度传感器所使用的ADC模块配置，可选配置见adc.h(注意:若相同ADC模块被用于不同应用构件，请保证它们的配置相同)
//定义所使用的ADC模块
#define TEMP_SENSOR_ADC_MOD				ADC_MOD1						//ADC1
//定义ADC模块时钟分频数
#define TEMP_SENSOR_ADC_CLK_DIV			ADC_CLK_DIV_4					//总线时钟4分频
//定义ADC采样精度
#define TEMP_SENSOR_ADC_ACCURACY		ADC_ACCURACY_SINGLE_DIFF_16		//差分16位转换
//定义ADC硬件采样均值数量
#define TEMP_SENSOR_ADC_HARDWARE_AVG	ADC_HARDWARE_AVG_8				//8个采样均值
//定义ADC长采样时间
#define TEMP_SENSOR_ADC_ADLSTS			ADC_ADLSTS_12					//额外12个ADCK周期
//定义ADC高速配置
#define TEMP_SENSOR_ADC_ADHSC			ADC_ADHSC_NORMAL				//正常转换时序
//定义是否使能ADC校对功能
#define TEMP_SENSOR_ADC_ENABLE_CAL		true							//使能ADC校对功能
//定义所使用的ADC差分通道组(这里使用差分输入获取ADC转换结果)
#define TEMP_SENSOR_ADC_DIFF_GROUP		ADC_DIFF_GROUP_TEMP

//定义温度传感器硬件参数，温度 = 25 - ((VTEMP - VTEMP25) / m)
//温度传感器25°C时的电压值，单位mV
#define TEMP_SENSOR_VTEMP25				(716)
//温度传感器比例系数，单位mV/°C
#define TEMP_SENSOR_M					(1.62f)

//==========================================================================
//函数名称: temp_sensor_init
//函数返回: true: 若使能校对功能，且校对功能配置成功，或者禁止校对功能;
//         false:若使能校对功能，且校对功能配置失败;
//参数说明: 无
//功能概要: 初始化温度传感器，相应配置在temp_sensor.h中
//备注: 若总线时钟频率为48MHz，使用总线时钟4分频，差分16位转换，硬件采样8个采样均值，
//     长采样时间额外12个ADCK周期，正常转换时序，则采样时间约为31.02μs
//==========================================================================
bool temp_sensor_init();

//==========================================================================
//函数名称: temp_sensor_get_vtg
//函数返回: 当前ADC采样的电压值，单位mV
//参数说明: 无
//功能概要: 获取当前ADC采样的电压值，单位mV，相应配置在temp_sensor.h中
//备注: 可以用于设置TEMP_SENSOR_VTEMP25
//==========================================================================
float temp_sensor_get_vtg();

//==========================================================================
//函数名称: temp_sensor_get_temp
//函数返回: 温度传感器所测温度，单位°C
//参数说明: 无
//功能概要: 获取温度传感器所测温度，单位°C，相应配置在temp_sensor.h中
//备注: 当使用差分9位转换时，分辨率约为7.96°C，即温度变化约7.96°C时，才可以检测出来;
//     当使用差分11位转换时，分辨率约为1.99°C;
//     当使用差分13位转换时，分辨率约为0.50°C;
//     当使用差分16位转换时，分辨率约为0.06°C;
//==========================================================================
float temp_sensor_get_temp();

#endif
