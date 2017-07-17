//==========================================================================
//文件名称：time.h
//功能概要：时间软件程序头文件
//==========================================================================

#ifndef _TIME_H
#define _TIME_H

#include "common.h"

//定义计时器通道的数量，计时器通道号为[0, TIME_TIMER_CH_NUM - 1]
#define TIME_TIMER_CH_NUM		(6)

//==========================================================================
//函数名称: time_init
//函数返回: 无
//参数说明: init_time:初始时间，单位ms
//功能概要: 使能时间模块
//==========================================================================
void time_init(uint64 init_time);

//==========================================================================
//函数名称: time_update
//函数返回: 无
//参数说明: time:时间增量，单位ms
//功能概要: 更新当前时间
//==========================================================================
void time_update(uint64 time);

//==========================================================================
//函数名称: time_get_time
//函数返回: 当前时间，单位ms
//参数说明: 无
//功能概要: 获取当前时间
//==========================================================================
uint64 time_get_time();

//==========================================================================
//函数名称: time_register_timer
//函数返回: true:注册成功; false:注册失败，通道号越界或者该通道已经注册
//参数说明: timer_ch:计时器通道号，取值范围为[0, TIME_TIMER_CH_NUM - 1]
//         set_time:该通道计时器的计时时间，单位ms
//功能概要: 注册计时器通道，并使该通道的计时器开始计时
//==========================================================================
bool time_register_timer(uint8 timer_ch, uint64 set_time);

//==========================================================================
//函数名称: time_is_timer_arrived
//函数返回: true:已到达时间; false:未到达时间
//参数说明: timer_ch:计时器通道号，取值范围为[0, TIME_TIMER_CH_NUM - 1]
//功能概要: 查看指定通道的计时器是否已经到达时间，到达时间后，计时器不再计时
//==========================================================================
bool time_is_timer_arrived(uint8 timer_ch);

//==========================================================================
//函数名称: time_restart_timer
//函数返回: true:重启成功; false:重启失败，通道号越界或者该通道未注册
//参数说明: timer_ch:计时器通道号，取值范围为[0, TIME_TIMER_CH_NUM - 1]
//功能概要: 重启指定通道的计时器，使其重新计时
//==========================================================================
bool time_restart_timer(uint8 timer_ch);

#endif
