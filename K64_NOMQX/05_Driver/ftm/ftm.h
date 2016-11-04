//==========================================================================
//文件名称：ftm.h
//功能概要：K64 FTM底层驱动程序头文件
//==========================================================================

#ifndef _FTM_H
#define _FTM_H

//包含公共头文件
#include "common.h"

//==========================================================================
//说明: 在同一FTM模块内相互兼容的功能:
//     (边沿对齐的PWM波、输入捕捉、输出比较、计时)
//     (联合模式PWM波、互补模式PWM波)
//     除这两组外任何功能无法兼容
//==========================================================================

//定义FTM模块号
#define FTM_MOD0	(0)
#define FTM_MOD1	(1)
#define FTM_MOD2	(2)
#define FTM_MOD3	(3)

//定义FTM模块通道号
#define FTM_CH0		(0)
#define FTM_CH1		(1)
#define FTM_CH2		(2)
#define FTM_CH3		(3)
#define FTM_CH4		(4)
#define FTM_CH5		(5)
#define FTM_CH6		(6)
#define FTM_CH7		(7)

//定义FTM模块通道组号
#define FTM_CH_GROUP0	(0)		//CH0和CH1
#define FTM_CH_GROUP1	(1)		//CH2和CH3
#define FTM_CH_GROUP2	(2)		//CH4和CH5
#define FTM_CH_GROUP3	(3)		//CH6和CH7

//FTM各模块各通道的引脚设置，通过更改COM_PORTx|x的x以选择引脚，
//可选择的引脚注释在相应通道后方。注意：B12|B13不要重复定义
#define FTM_MOD0_CH0_PIN	(COM_PORTC|1)	//A3   C1
#define FTM_MOD0_CH1_PIN	(COM_PORTC|2)	//A2   C2
#define FTM_MOD0_CH2_PIN	(COM_PORTC|3)	//A5   C3   C5
#define FTM_MOD0_CH3_PIN	(COM_PORTC|4)	//A6   C4
#define FTM_MOD0_CH4_PIN	(COM_PORTD|4)	//A7   B12  D4
#define FTM_MOD0_CH5_PIN	(COM_PORTD|5)	//A0   B13  D5
#define FTM_MOD0_CH6_PIN	(COM_PORTA|1)	//A1        D6
#define FTM_MOD0_CH7_PIN	(COM_PORTA|2)	//A2        D7

#define FTM_MOD1_CH0_PIN	(COM_PORTB|0)	//A8   A12  B0   B12
#define FTM_MOD1_CH1_PIN	(COM_PORTB|1)	//A9   A13  B1   B13

#define FTM_MOD2_CH0_PIN	(COM_PORTA|10)	//A10  B18
#define FTM_MOD2_CH1_PIN	(COM_PORTA|11)	//A11  B19

#define FTM_MOD3_CH0_PIN	(COM_PORTD|0)	//D0   E5
#define FTM_MOD3_CH1_PIN	(COM_PORTD|1)	//D1   E6
#define FTM_MOD3_CH2_PIN	(COM_PORTD|2)	//D2   E7
#define FTM_MOD3_CH3_PIN	(COM_PORTD|3)	//D3   E8
#define FTM_MOD3_CH4_PIN	(COM_PORTC|8)	//C8   E9
#define FTM_MOD3_CH5_PIN	(COM_PORTC|9)	//C9   E10
#define FTM_MOD3_CH6_PIN	(COM_PORTC|10)	//C10  E11
#define FTM_MOD3_CH7_PIN	(COM_PORTC|11)	//C11  E12

//定义FTM模块正交解码功能所用引脚组，通过更改宏定义的值以选择引脚组，引脚组前者为PHA，后者为PHB
//FTM1_QD:  0:A8-A9;   1:A12-A13; 2:B0-B1; 3:B12-B13
#define FTM_MOD1_QD_SETUP	(2)
//FTM2_QD:  0:A10-A11; 1:B18-B19;
#define FTM_MOD2_QD_SETUP	(0)

//FTM模块所用时钟频率，这里使用的是总线时钟，单位kHz
#define FTM_CLK_FREQ	BUS_CLK_KHZ

//FTM模块时钟分频因子，DIV后的数字代表分频因子，其工作频率=时钟频率/分频因子
#define FTM_CLK_DIV_1		(0)
#define FTM_CLK_DIV_2		(1)
#define FTM_CLK_DIV_4		(2)
#define FTM_CLK_DIV_8		(3)
#define FTM_CLK_DIV_16		(4)
#define FTM_CLK_DIV_32		(5)
#define FTM_CLK_DIV_64		(6)
#define FTM_CLK_DIV_128		(7)

//定义FTM模块计数器模式
#define FTM_COUNTER_MODE_UP				(0)	//向上计数
#define FTM_COUNTER_MODE_UP_DOWN		(1)	//上下计数
#define FTM_COUNTER_MODE_QD				(2)	//正交解码

//定义FTM模块PWM功能的模式
#define FTM_PWM_MODE_EDGE_ALIGNED	(0)	//单通道，PWM波边沿对齐
#define FTM_PWM_MODE_CENTER_ALIGNED	(1)	//单通道，PWM波中心对齐
#define FTM_PWM_MODE_COMBINE		(2)	//双通道，两通道的PWM波相同
#define FTM_PWM_MODE_COMPLEMENTARY	(3)	//双通道，两通道的PWM波互补

//定义FTM模块PWM功能的极性（POL为polarity简写）
#define FTM_PWM_POL_POSITIVE		(1)	//正极性，即占空比越大，高电平所占比例越高
#define FTM_PWM_POL_NEGATIVE		(0)	//负极性，即占空比越大，低电平所占比例越高

//定义FTM模块的计数周期精度，(duty/FTM_PERIOD_ACCURACY)才为实际占空比，这里姑且称duty为占空比，回避了百分数，是为了避免浮点数运算
#define FTM_PERIOD_ACCURACY			(10000u)
//定义FTM模块占空比的最大值与最小值
#define FTM_DUTY_MIN				(0u)
#define FTM_DUTY_MAX				FTM_PERIOD_ACCURACY

//定义FTM模块捕捉模式
#define FTM_IC_MODE_RISING_EDGE		(0)		//上升沿捕捉
#define FTM_IC_MODE_FALLING_EDGE	(1)		//下降沿捕捉
#define FTM_IC_MODE_DOUBLE_EDGE		(2)		//双边沿捕捉

//定义FTM模块输出比较功能模式
#define FTM_OC_MODE_TOGGLE		(0)		//比较成功后反转电平
#define FTM_OC_MODE_SET			(1)		//比较成功后置高电平
#define FTM_OC_MODE_CLEAR		(2)		//比较成功后置低电平

//定义FTM模块正交解码功能模式
#define FTM_QD_MODE_PHAB				(0)		//AB相编码器
#define FTM_QD_MODE_COUNT_DIRECTION		(1)		//方向-脉冲型编码器

//定义FTM模块正交解码功能方向
#define FTM_QD_DIR_POSITIVE		(0)		//正方向
#define FTM_QD_DIR_NEGATIVE		(1)		//负方向

//==========================================================================
//函数名称: ftm_init
//函数返回: 无
//参数说明: mod:FTM模块号:
//             FTM_MODx，x为模块号;
//         clk_div:时钟分频因子:
//                 FTM_CLK_DIV_x，x为分频因子大小;
//         counter_mode:计数器模式:
//                      FTM_COUNTER_MODE_UP:          向上计数;
//                      FTM_COUNTER_MODE_UP_DOWN:     上下计数;
//                      FTM_COUNTER_MODE_QD:          正交解码;
//         counter_period:见备注
//功能概要: 初始化FTM模块，默认未开启中断
//备注: 当选择向上计数模式或上下计数模式时，counter_period为计数器计数周期，单位ms，
//     需满足48000/x*counter_period<=num，48000为这里使用的总线时钟频率，单位kHz，
//     x为FTM_CLK_DIV_x的x，向上计数模式时num为65536，上下计数模式时num为65534，
//     另外，上下计数模式时，若选择128分频，周期需为偶数(否则计数精度会丢失);
//
//     当选择正交解码模式时，counter_period无效，且此时clk_div代表计数器分频数，
//     (计数器计数个数*x)为正常计数个数，x为FTM_CLK_DIV_x的x;
//==========================================================================
void ftm_init(uint8 mod, uint8 clk_div, uint8 counter_mode,
		uint8 counter_period);

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
void ftm_timer_enable_int(uint8 mod, uint8 time);

//==========================================================================
//函数名称: ftm_timer_disable_int
//函数返回: 无
//参数说明: mod:FTM模块号:
//             FTM_MODx，x为模块号;
//功能概要: 关闭FTM模块计时中断
//==========================================================================
void ftm_timer_disable_int(uint8 mod);

//==========================================================================
//函数名称: ftm_timer_get_int
//函数返回: true:产生中断; false:未产生中断
//参数说明: mod:FTM模块号:
//             FTM_MODx，x为模块号;
//功能概要: 获取FTM模块计时中断标志
//==========================================================================
bool ftm_timer_get_int(uint8 mod);

//==========================================================================
//函数名称: ftm_timer_clear_int
//函数返回: 无
//参数说明: mod:FTM模块号:
//             FTM_MODx，x为模块号;
//功能概要: 清除FTM模块计时中断标志
//==========================================================================
void ftm_timer_clear_int(uint8 mod);

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
//     PWM波的频率为(1000/counter_period)，单位Hz，
//     counter_period为相应FTM模块的计数周期，单位ms
//==========================================================================
void ftm_pwm_single_init(uint8 mod, uint8 ch, uint8 mode, uint8 pol,
		uint16 duty);

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
void ftm_pwm_single_set(uint8 mod, uint8 ch, uint16 duty);

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
//     PWM波的频率为(1000/counter_period)，单位Hz，
//     counter_period为相应FTM模块的计数周期，单位ms;
//     偶数通道输出的PWM波，无论选择哪种模式，都是一样的，即互补模式只针对奇数通道
//==========================================================================
void ftm_pwm_combine_init(uint8 mod, uint8 ch_group, uint8 mode, uint8 pol,
		uint16 duty1, uint16 duty2);

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
void ftm_pwm_combine_set(uint8 mod, uint8 ch_group, uint16 duty1, uint16 duty2);

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
void ftm_ic_init(uint8 mod, uint8 ch, uint8 mode);

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
uint16 ftm_ic_get_ratio(uint8 mod, uint8 ch);

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
void ftm_oc_init(uint8 mod, uint8 ch, uint8 mode, uint16 ratio);

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
void ftm_oc_change_mode(uint8 mod, uint8 ch, uint8 mode);

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
void ftm_oc_set_ratio(uint8 mod, uint8 ch, uint16 ratio);

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
void ftm_ch_enable_int(uint8 mod, uint8 ch);

//==========================================================================
//函数名称: ftm_ch_disable_int
//函数返回: 无
//参数说明: mod:FTM模块号:
//             FTM_MODx，x为模块号;
//         ch:FTM模块的通道号:
//            FTM_CHx，x为通道号;
//功能概要: 关闭通道中断
//==========================================================================
void ftm_ch_disable_int(uint8 mod, uint8 ch);

//==========================================================================
//函数名称: ftm_ch_get_int
//函数返回: 无
//参数说明: mod:FTM模块号:
//             FTM_MODx，x为模块号;
//         ch:FTM模块的通道号:
//            FTM_CHx，x为通道号;
//功能概要: 获取通道的中断标志
//==========================================================================
bool ftm_ch_get_int(uint8 mod, uint8 ch);

//==========================================================================
//函数名称: ftm_ch_clear_int
//函数返回: 无
//参数说明: mod:FTM模块号:
//             FTM_MODx，x为模块号;
//         ch:FTM模块的通道号:
//            FTM_CHx，x为通道号;
//功能概要: 清除通道的中断标志
//==========================================================================
void ftm_ch_clear_int(uint8 mod, uint8 ch);

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
void ftm_qd_init(uint8 mod, uint8 mode, uint8 dir);

//==========================================================================
//函数名称: ftm_qd_get_count
//函数返回: 计数器计数个数
//参数说明: mod:FTM模块号:
//             FTM_MODx，x为模块号;这里仅有FTM_MOD1和FTM_MOD2可以使用;
//功能概要: 获取计数器当前计数个数
//备注: 计数个数带方向，正负分别代表不同方向，具体定义与设置有关
//==========================================================================
int16 ftm_qd_get_count(uint8 mod);

//==========================================================================
//函数名称: ftm_qd_clear_count
//函数返回: 无
//参数说明: mod:FTM模块号:
//             FTM_MODx，x为模块号;这里仅有FTM_MOD1和FTM_MOD2可以使用;
//功能概要: 清除计数器计数个数
//==========================================================================
void ftm_qd_clear_count(uint8 mod);

//==========================================================================
//函数名称: ftm_qd_get_and_clear_count
//函数返回: 计数器计数个数
//参数说明: mod:FTM模块号:
//             FTM_MODx，x为模块号;这里仅有FTM_MOD1和FTM_MOD2可以使用;
//功能概要: 获取计数器当前计数个数，并且清除计数个数
//备注: 计数个数带方向，正负分别代表不同方向，具体定义与设置有关
//==========================================================================
int16 ftm_qd_get_and_clear_count(uint8 mod);

//根据通道所设置的引脚号，定义相应的PCR的MUX值
#ifdef FTM_MOD0_CH0_PIN
#if(FTM_MOD0_CH0_PIN == (COM_PORTA|3))
#define FTM_MOD0_CH0_PCR_MUX	(3)
#elif(FTM_MOD0_CH0_PIN == (COM_PORTC|1))
#define FTM_MOD0_CH0_PCR_MUX	(4)
#endif
#endif

#ifdef FTM_MOD0_CH1_PIN
#if(FTM_MOD0_CH1_PIN == (COM_PORTA|4))
#define FTM_MOD0_CH1_PCR_MUX	(3)
#elif(FTM_MOD0_CH1_PIN == (COM_PORTC|2))
#define FTM_MOD0_CH1_PCR_MUX	(4)
#endif
#endif

#ifdef FTM_MOD0_CH2_PIN
#if(FTM_MOD0_CH2_PIN == (COM_PORTA|5))
#define FTM_MOD0_CH2_PCR_MUX	(3)
#elif(FTM_MOD0_CH2_PIN == (COM_PORTC|3))
#define FTM_MOD0_CH2_PCR_MUX	(4)
#elif(FTM_MOD0_CH2_PIN == (COM_PORTC|5))
#define FTM_MOD0_CH2_PCR_MUX	(7)
#endif
#endif

#ifdef FTM_MOD0_CH3_PIN
#if(FTM_MOD0_CH3_PIN == (COM_PORTA|6))
#define FTM_MOD0_CH3_PCR_MUX	(3)
#elif(FTM_MOD0_CH3_PIN == (COM_PORTC|4))
#define FTM_MOD0_CH3_PCR_MUX	(4)
#endif
#endif

#ifdef FTM_MOD0_CH4_PIN
#if(FTM_MOD0_CH4_PIN == (COM_PORTA|7))
#define FTM_MOD0_CH4_PCR_MUX	(3)
#elif(FTM_MOD0_CH4_PIN == (COM_PORTB|12) || FTM_MOD0_CH4_PIN == (COM_PORTD|4))
#define FTM_MOD0_CH4_PCR_MUX	(4)
#endif
#endif

#ifdef FTM_MOD0_CH5_PIN
#if(FTM_MOD0_CH5_PIN == (COM_PORTA|0))
#define FTM_MOD0_CH5_PCR_MUX	(3)
#elif(FTM_MOD0_CH5_PIN == (COM_PORTB|13) || FTM_MOD0_CH5_PIN == (COM_PORTD|5))
#define FTM_MOD0_CH5_PCR_MUX	(4)
#endif
#endif

#ifdef FTM_MOD0_CH6_PIN
#if(FTM_MOD0_CH6_PIN == (COM_PORTA|1))
#define FTM_MOD0_CH6_PCR_MUX	(3)
#elif(FTM_MOD0_CH6_PIN == (COM_PORTD|6))
#define FTM_MOD0_CH6_PCR_MUX	(4)
#endif
#endif

#ifdef FTM_MOD0_CH7_PIN
#if(FTM_MOD0_CH7_PIN == (COM_PORTA|2))
#define FTM_MOD0_CH7_PCR_MUX	(3)
#elif(FTM_MOD0_CH7_PIN == (COM_PORTD|7))
#define FTM_MOD0_CH7_PCR_MUX	(4)
#endif
#endif

#ifdef FTM_MOD1_CH0_PIN
#if(FTM_MOD1_CH0_PIN == (COM_PORTA|8) || FTM_MOD1_CH0_PIN == (COM_PORTA|12) || FTM_MOD1_CH0_PIN == (COM_PORTB|0)||FTM_MOD1_CH0_PIN == (COM_PORTB|12))
#define FTM_MOD1_CH0_PCR_MUX	(3)
#endif
#endif

#ifdef FTM_MOD1_CH1_PIN
#if(FTM_MOD1_CH1_PIN == (COM_PORTA|9) || FTM_MOD1_CH1_PIN == (COM_PORTA|13) || FTM_MOD1_CH1_PIN == (COM_PORTB|1) || FTM_MOD1_CH1_PIN == (COM_PORTB|13))
#define FTM_MOD1_CH1_PCR_MUX	(3)
#endif
#endif

#ifdef FTM_MOD2_CH0_PIN
#if(FTM_MOD2_CH0_PIN == (COM_PORTA|10) || FTM_MOD2_CH0_PIN == (COM_PORTB|18))
#define FTM_MOD2_CH0_PCR_MUX	(3)
#endif
#endif

#ifdef FTM_MOD2_CH1_PIN
#if(FTM_MOD2_CH1_PIN == (COM_PORTA|11) || FTM_MOD2_CH1_PIN == (COM_PORTB|19))
#define FTM_MOD2_CH1_PCR_MUX	(3)
#endif
#endif

#ifdef FTM_MOD3_CH0_PIN
#if(FTM_MOD3_CH0_PIN == (COM_PORTD|0))
#define FTM_MOD3_CH0_PCR_MUX	(4)
#elif(FTM_MOD3_CH0_PIN == (COM_PORTE|5))
#define FTM_MOD3_CH0_PCR_MUX	(6)
#endif
#endif

#ifdef FTM_MOD3_CH1_PIN
#if(FTM_MOD3_CH1_PIN == (COM_PORTD|1))
#define FTM_MOD3_CH1_PCR_MUX	(4)
#elif(FTM_MOD3_CH1_PIN == (COM_PORTE|6))
#define FTM_MOD3_CH1_PCR_MUX	(6)
#endif
#endif

#ifdef FTM_MOD3_CH2_PIN
#if(FTM_MOD3_CH2_PIN == (COM_PORTD|2))
#define FTM_MOD3_CH2_PCR_MUX	(4)
#elif(FTM_MOD3_CH2_PIN == (COM_PORTE|7))
#define FTM_MOD3_CH2_PCR_MUX	(6)
#endif
#endif

#ifdef FTM_MOD3_CH3_PIN
#if(FTM_MOD3_CH3_PIN == (COM_PORTD|3))
#define FTM_MOD3_CH3_PCR_MUX	(4)
#elif(FTM_MOD3_CH3_PIN == (COM_PORTE|8))
#define FTM_MOD3_CH3_PCR_MUX	(6)
#endif
#endif

#ifdef FTM_MOD3_CH4_PIN
#if(FTM_MOD3_CH4_PIN == (COM_PORTC|8))
#define FTM_MOD3_CH4_PCR_MUX	(3)
#elif(FTM_MOD3_CH4_PIN == (COM_PORTE|9))
#define FTM_MOD3_CH4_PCR_MUX	(6)
#endif
#endif

#ifdef FTM_MOD3_CH5_PIN
#if(FTM_MOD3_CH5_PIN == (COM_PORTC|9))
#define FTM_MOD3_CH5_PCR_MUX	(3)
#elif(FTM_MOD3_CH5_PIN == (COM_PORTE|10))
#define FTM_MOD3_CH5_PCR_MUX	(6)
#endif
#endif

#ifdef FTM_MOD3_CH6_PIN
#if(FTM_MOD3_CH6_PIN == (COM_PORTC|10))
#define FTM_MOD3_CH6_PCR_MUX	(3)
#elif(FTM_MOD3_CH6_PIN == (COM_PORTE|11))
#define FTM_MOD3_CH6_PCR_MUX	(6)
#endif
#endif

#ifdef FTM_MOD3_CH7_PIN
#if(FTM_MOD3_CH7_PIN == (COM_PORTC|11))
#define FTM_MOD3_CH7_PCR_MUX	(3)
#elif(FTM_MOD3_CH7_PIN == (COM_PORTE|12))
#define FTM_MOD3_CH7_PCR_MUX	(6)
#endif
#endif

//根据正交解码功能所选引脚组，设置PHA与PHB的引脚以及相应的PCR的MUX值
#ifdef FTM_MOD1_QD_SETUP
#if(FTM_MOD1_QD_SETUP == 0)
#define FTM_MOD1_QD_PHA_PIN		(COM_PORTA|8)
#define FTM_MOD1_QD_PHB_PIN		(COM_PORTA|9)
#elif(FTM_MOD1_QD_SETUP == 1)
#define FTM_MOD1_QD_PHA_PIN		(COM_PORTA|12)
#define FTM_MOD1_QD_PHB_PIN		(COM_PORTA|13)
#elif(FTM_MOD1_QD_SETUP == 2)
#define FTM_MOD1_QD_PHA_PIN		(COM_PORTB|0)
#define FTM_MOD1_QD_PHB_PIN		(COM_PORTB|1)
#elif(FTM_MOD1_QD_SETUP == 3)
#define FTM_MOD1_QD_PHA_PIN		(COM_PORTB|12)
#define FTM_MOD1_QD_PHB_PIN		(COM_PORTB|13)
#endif
#if(FTM_MOD1_QD_SETUP == 1)
#define FTM_MOD1_QD_PCR_MUX		(7)
#else
#define FTM_MOD1_QD_PCR_MUX		(6)
#endif
#endif

#ifdef FTM_MOD2_QD_SETUP
#if(FTM_MOD2_QD_SETUP == 0)
#define FTM_MOD2_QD_PHA_PIN		(COM_PORTA|10)
#define FTM_MOD2_QD_PHB_PIN		(COM_PORTA|11)
#elif(FTM_MOD2_QD_SETUP == 1)
#define FTM_MOD2_QD_PHA_PIN		(COM_PORTB|18)
#define FTM_MOD2_QD_PHB_PIN		(COM_PORTB|19)
#endif
#define FTM_MOD2_QD_PCR_MUX		(6)
#endif

#endif
