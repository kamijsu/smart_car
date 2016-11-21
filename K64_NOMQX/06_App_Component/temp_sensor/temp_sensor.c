//==========================================================================
//文件名称：temp_sensor.c
//功能概要：温度传感器应用驱动程序源文件
//==========================================================================

#include "temp_sensor.h"

//==========================================================================
//函数名称: temp_sensor_init
//函数返回: true: 若使能校对功能，且校对功能配置成功，或者禁止校对功能;
//         false:若使能校对功能，且校对功能配置失败;
//参数说明: 无
//功能概要: 初始化温度传感器，相应配置在temp_sensor.h中
//备注: 若总线时钟频率为48MHz，使用总线时钟4分频，差分16位转换，硬件采样8个采样均值，
//     长采样时间额外12个ADCK周期，正常转换时序，则采样时间约为31.02μs
//==========================================================================
bool temp_sensor_init() {
	//按照配置初始化ADC模块
	return adc_init(TEMP_SENSOR_ADC_MOD, TEMP_SENSOR_ADC_CLK_DIV,
	TEMP_SENSOR_ADC_ACCURACY, TEMP_SENSOR_ADC_HARDWARE_AVG,
	TEMP_SENSOR_ADC_ADLSTS, TEMP_SENSOR_ADC_ADHSC, TEMP_SENSOR_ADC_CAL);
}

//==========================================================================
//函数名称: temp_sensor_get
//函数返回: 温度传感器所测温度，单位°C
//参数说明: 无
//功能概要: 获取温度传感器所测温度，单位°C
//备注: 当使用差分9位转换时，分辨率约为7.99°C，即温度变化约7.99°C时，才可以检测出来;
//     当使用差分11位转换时，分辨率约为1.99°C;
//     当使用差分13位转换时，分辨率约为0.50°C;
//     当使用差分16位转换时，分辨率约为0.06°C;
//==========================================================================
float temp_sensor_get() {
	int16 ad;		//采样AD值
	float vtemp;	//根据采样AD值转化出的电压值，单位mV
	float temp;		//实际温度，单位°C

	//获取采样AD值
	ad = adc_diff_get(TEMP_SENSOR_ADC_MOD, TEMP_SENSOR_ADC_DIFF_GROUP);
	//转化为电压值
	vtemp = ad * TEMP_SENSOR_ADC_VTG * 1.0f
			/ ((1 << TEMP_SENSOR_ADC_DATA_BITS) - 1);
	//转化为温度
	temp = 25 - ((vtemp - TEMP_SENSOR_VTEMP25) / TEMP_SENSOR_M);

	return temp;
}
