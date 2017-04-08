//==========================================================================
//文件名称：gyro_acce.h
//功能概要：陀螺仪加速度传感器应用驱动程序头文件
//==========================================================================

#ifndef  _GYRO_ACCE_H
#define  _GYRO_ACCE_H

#include "common.h"
#include "adc.h"

//定义陀螺仪加速度传感器所使用的ADC模块配置，可选配置见adc.h(注意:若相同ADC模块被用于不同应用构件，请保证它们的配置相同)
//定义所使用的ADC模块
#define GYRO_ACCE_ADC_MOD				ADC_MOD0						//ADC0
//定义ADC模块时钟分频数
#define GYRO_ACCE_ADC_CLK_DIV			ADC_CLK_DIV_4					//总线时钟4分频
//定义ADC采样精度
#define GYRO_ACCE_ADC_ACCURACY			ADC_ACCURACY_SINGLE_12_DIFF_13	//单端12位转换
//定义ADC硬件采样均值数量
#define GYRO_ACCE_ADC_HARDWARE_AVG		ADC_HARDWARE_AVG_8				//8个采样均值
//定义ADC长采样时间
#define GYRO_ACCE_ADC_ADLSTS			ADC_ADLSTS_12					//额外12个ADCK周期
//定义ADC高速配置
#define GYRO_ACCE_ADC_ADHSC				ADC_ADHSC_NORMAL				//正常转换时序
//定义是否使能ADC校对功能
#define GYRO_ACCE_ADC_ENABLE_CAL		true							//使能ADC校对功能
//定义所使用的ADC单端通道号
//定义陀螺仪X轴角速度引脚，这里使用PTE24
#define GYRO_ADC_SE						ADC_SE17						//17号通道
#define GYRO_ADC_SE_SEL					ADC_SE_SEL_A					//选择A通道
//定义加速度传感器Z轴加速度引脚，这里使用PTE25
#define ACCE_ADC_SE						ADC_SE18						//18号通道
#define ACCE_ADC_SE_SEL					ADC_SE_SEL_A					//选择A通道

//定义陀螺仪硬件参数
//定义陀螺仪运算放大器倍数，无单位
#define GYRO_MTP						(5.1f)
//定义陀螺仪比例因子，单位mV/°/s
#define GYRO_SF							(0.67f)
//定义X轴角速度为0°/s时的电压值，单位mV
#define GYRO_ZERO_VTG					(1320)

//定义加速度传感器硬件参数
//定义加速度传感器分辨率，单位mV/g
#define ACCE_RES						(800)
//定义Z轴加速度为0g时的电压值，单位mV
#define ACCE_ZERO_VTG					(1550)

//定义陀螺仪加速度传感器采样的周期，单位s
#define GYRO_ACCE_PERIOD				(0.005f)						//5ms

//定义卡尔曼滤波因子
#define GYRO_ACCE_Q_ANGLE         (0.001f)		//由角速度积分获得的角度的不确定度，值越大波形越与实际值贴合
#define GYRO_ACCE_Q_GYRO          (0.003f)		//角速度漂移值的不确定度，值越大向真实值回归的速度越快
#define GYRO_ACCE_R_ANGLE         (0.5f)		//由加速度获得的角度的不确定度，值越小波形越与实际值贴合

//==========================================================================
//函数名称: gyro_acce_init
//函数返回: true: 若使能校对功能，且校对功能配置成功，或者禁止校对功能;
//         false:若使能校对功能，且校对功能配置失败;
//参数说明: 无
//功能概要: 初始化陀螺仪加速度传感器，相应配置在gyro_acce.h中
//==========================================================================
bool gyro_acce_init();

//==========================================================================
//函数名称: gyro_acce_get_vtg
//函数返回: 无
//参数说明: gyro_vtg:存储陀螺仪X轴角速度电压值的地址，单位mV，
//                  往前倾值减小，往后倾值增大
//         acce_vtg:存储加速度传感器Z轴加速度电压值的地址，单位mV，
//                  往前倾值增大，往后倾值减小
//功能概要: 获取采集到的X轴角速度和Z轴加速度的电压值
//备注: 可以用于设置GYRO_ZERO_VTG和ACCE_ZERO_VTG
//==========================================================================
void gyro_acce_get_vtg(float* gyro_vtg, float* acce_vtg);

//==========================================================================
//函数名称: gyro_acce_get_raw_phy
//函数返回: 无
//参数说明: raw_angle_speed:存储X轴角速度的地址，单位°/s，
//                         往前倾值增大，往后倾值减小
//         raw_acce_speed:存储Z轴加速度的地址，单位g，
//                        往前倾值增大，往后倾值减小
//功能概要: 获取采集到的X轴角速度和Z轴加速度
//==========================================================================
void gyro_acce_get_raw_phy(float* raw_angle_speed, float* raw_acce_speed);

//==========================================================================
//函数名称: gyro_acce_get_phy
//函数返回: 无
//参数说明: angle:存储角度的地址，单位°，往前倾值增大，往后倾值减小
//         angle_speed:存储角速度的地址，单位°/s，往前倾值增大，往后倾值减小
//功能概要: 获取卡尔曼滤波后的角度和角速度
//备注: 该函数需每 GYRO_ACCE_PERIOD s调用一次，否则获取的角度和角速度不准确;
//     angle参数为上一次滤波后的角度，初始值为0
//==========================================================================
void gyro_acce_get_phy(float* angle, float* angle_speed);

#endif
