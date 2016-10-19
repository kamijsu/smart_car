#ifndef _STRUCTS_H
#define _STRUCTS_H

#include "common.h"

//呼吸灯参数
typedef struct
{
	uint_8 is_start;	//是否开始呼吸标志，0为未开始，1为开始
	float duty;			//占空比
	uint_8 dir;			//变化方向，0为变暗，1为变亮
	float radio;		//每次变化的占空比大小
} BreatheLightParams, *BreatheLightParamsPtr;

//时间标志参数
typedef struct
{
	uint_8 f_50ms;
	uint_8 f_1s;
} TimeFlag, *TimeFlagPtr;

//时间计数器参数
typedef struct
{
	uint_8 c_50ms;
	uint_8 c_1s;
} TimeCounter, *TimeCounterPtr;

//消息队列
typedef struct
{
	uint_8 msg[16];		//存储消息的数组
	uint_16 front;		//队首下标
	uint_16 rear;		//队尾下标
} MsgQueue, *MsgQueuePtr;

#endif
