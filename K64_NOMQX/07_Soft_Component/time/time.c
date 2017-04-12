//==========================================================================
//文件名称：time.c
//功能概要：时间软件程序源文件
//==========================================================================

#include "time.h"

//定义计时器结构体
typedef struct {
	uint8 flags;		//通道标志，第0位表示通道有无注册，第1位表示计时器有无到达时间
	uint64 start_time;	//计时器开始时间
	uint64 set_time;	//计时器设定的计时时间
} TimeTimer;

//当前时间
static uint64 current_time;
//各通道计时器
static TimeTimer timers[TIME_TIMER_CH_NUM];

//==========================================================================
//函数名称: time_init
//函数返回: 无
//参数说明: init_time:初始时间，单位ms
//功能概要: 使能时间模块
//==========================================================================
void time_init(uint64 init_time) {
	uint8 i;	//游标

	//设定当前时间为初始时间
	current_time = init_time;
	//设置各通道尚未注册
	for (i = 0; i < TIME_TIMER_CH_NUM; ++i) {
		timers[i].flags &= ~0x3;
	}
}

//==========================================================================
//函数名称: time_update
//函数返回: 无
//参数说明: time:时间增量，单位ms
//功能概要: 更新当前时间
//==========================================================================
void time_update(uint64 time) {
	uint8 i;	//游标

	//更新当前时间
	current_time += time;
	//更新各通道计时器的到达情况
	for (i = 0; i < TIME_TIMER_CH_NUM; ++i) {
		//该通道的计时器已注册，但未到达时间
		if ((timers[i].flags & 0x3) == 0x1) {
			//检查时间是否到达
			if (current_time >= timers[i].start_time + timers[i].set_time) {
				timers[i].flags |= 0x2;
			}
		}
	}
}

//==========================================================================
//函数名称: time_get_time
//函数返回: 当前时间，单位ms
//参数说明: 无
//功能概要: 获取当前时间
//==========================================================================
uint64 time_get_time() {
	//返回当前时间
	return current_time;
}

//==========================================================================
//函数名称: time_register_timer
//函数返回: true:注册成功; false:注册失败，通道号越界或者该通道已经注册
//参数说明: timer_ch:计时器通道号，取值范围为[0, TIME_TIMER_CH_NUM - 1]
//         set_time:该通道计时器的计时时间，单位ms
//功能概要: 注册计时器通道，并使该通道的计时器开始计时
//==========================================================================
bool time_register_timer(uint8 timer_ch, uint64 set_time) {
	//检查通道号是否越界
	if (timer_ch >= TIME_TIMER_CH_NUM) {
		return false;
	}
	//检查通道是否已经注册
	if ((timers[timer_ch].flags & 0x1) == 0x1) {
		return false;
	}
	//计时器未到达时间
	timers[timer_ch].flags &= ~0x2;
	//设置计时时间
	timers[timer_ch].set_time = set_time;
	//设置开始时间为当前时间
	timers[timer_ch].start_time = current_time;
	//该通道已经注册
	timers[timer_ch].flags |= 0x1;
	return true;
}

//==========================================================================
//函数名称: time_is_timer_arrived
//函数返回: true:已到达时间; false:未到达时间
//参数说明: timer_ch:计时器通道号，取值范围为[0, TIME_TIMER_CH_NUM - 1]
//功能概要: 查看指定通道的计时器是否已经到达时间，到达时间后，计时器不再计时
//==========================================================================
bool time_is_timer_arrived(uint8 timer_ch) {
	//检查通道号是否越界
	if (timer_ch >= TIME_TIMER_CH_NUM) {
		return false;
	}
	//若该通道已经注册，且计时器已经到达时间，返回true，否则返回false
	if ((timers[timer_ch].flags & 0x3) == 0x3) {
		return true;
	} else {
		return false;
	}
}

//==========================================================================
//函数名称: time_restart_timer
//函数返回: true:重启成功; false:重启失败，通道号越界或者该通道未注册
//参数说明: timer_ch:计时器通道号，取值范围为[0, TIME_TIMER_CH_NUM - 1]
//功能概要: 重启指定通道的计时器，使其重新计时
//==========================================================================
bool time_restart_timer(uint8 timer_ch) {
	//检查通道号是否越界
	if (timer_ch >= TIME_TIMER_CH_NUM) {
		return false;
	}
	//检查通道是否已经注册
	if ((timers[timer_ch].flags & 0x1) != 0x1) {
		return false;
	}
	//设置开始时间为当前时间
	timers[timer_ch].start_time = current_time;
	//计时器未到达时间
	timers[timer_ch].flags &= ~0x2;
	return true;
}
