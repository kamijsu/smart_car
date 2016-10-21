/*
 * includes.h
 *
 *  Created on: 2015年4月16日
 *      Author: SUMCU
 */

#ifndef INCLUDES_H_
#define INCLUDES_H_


//包含使用到的构件头文件
#include "common.h"
#include "light.h"
#include "uart.h"
#include "printf.h"
#include "mqxlite.h"

//防止全局变量重复声明的前缀处理方法
#ifdef GLOBLE_VAR                 //GLOBLE_VAR在main.c文件中宏定义
#define G_VAR_PREFIX          //main.c文件中使用全局变量不加“extern”前缀
#else
#define G_VAR_PREFIX  extern  //其他文件中使用全局变量自动加“extern”前缀
#endif

//定义宏常数
#define RUN_COUNTER_MAX   3500000ul   //小灯闪烁频率

#endif /* INCLUDES_H_ */
