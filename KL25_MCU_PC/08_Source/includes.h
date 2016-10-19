//======================================================================
//文件名称：includes.h
//功能概要：应用工程总头文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//版本更新：2012-11-12  V2.0
//======================================================================

#ifndef _INCLUDES_H
#define _INCLUDES_H

#include "common.h"
#include "gpio.h"
#include "light.h"
#include "breathe_light.h"
#include "uart.h"
#include "tpm.h"
#include "pit.h"
#include "structs.h"
#include <stdio.h>
#include <string.h>

//防止全局变量重复声明的前缀处理方法
#ifdef GLOBLE_VAR                 //GLOBLE_VAR在main.c文件中宏定义
#define G_VAR_PREFIX          //main.c文件中使用全局变量不加“extern”前缀
#else
#define G_VAR_PREFIX  extern  //其他文件中使用全局变量自动加“extern”前缀
#endif

//定义全局变量
//消息队列
G_VAR_PREFIX MsgQueue g_msg;
//时间标志
G_VAR_PREFIX TimeFlag g_time_flag;

//定义小灯闪烁频率
#define RUN_COUNTER_MAX   3000000ul

//定义所使用的串口
#define UART_USE UART_2

#endif
