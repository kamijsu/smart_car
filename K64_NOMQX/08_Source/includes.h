//============================================================================
//文件名称：includes.h
//功能概要：应用工程总头文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//版本更新：2012-11-12  V2.0
//          2014-11-9  V3.0
//============================================================================

#ifndef _INCLUDES_H
#define _INCLUDES_H

#include "common.h"
#include <stdlib.h>
#include <stdio.h>
#include "adc.h"
#include "dac.h"
#include "gpio.h"
#include "pit.h"
#include "uart.h"
#include "encoder.h"
#include "keyboard.h"
#include "gyro_acce.h"
#include "light.h"
#include "motor.h"
#include "reed_switch.h"
#include "control.h"
#include "visual_scope.h"
#include "ftm.h"
#include "temp_sensor.h"
#include "crc.h"
#include "crc16_modbus.h"
#include "frame.h"
#include <math.h>
#include "rng.h"
#include "arm_math.h"
#include "mmcau.h"
#include "crypto.h"
#include "flash.h"
#include "spi.h"
#include "custom.h"
#include "oled.h"
#include "i2c.h"
#include "dma.h"
#include "camera.h"
#include "vcan.h"
#include "menu.h"
#include "time.h"
#include "param.h"

//防止全局变量重复声明的前缀处理方法，并且添加volatile前缀，
//因为全局变量会被多个.c文件使用，若不使用volatile前缀，则不会每次去变量所在地址读值，
//会造成某个.c文件修改变量后，对别的.c文件不可见问题，而.c文件中的静态变量不存在该问题，
//因为仅有该.c文件可以访问该静态变量，即该静态变量始终对该.c文件是可见的
#ifdef GLOBLE_VAR				//GLOBLE_VAR在main.c文件中宏定义
#define G_VAR_PREFIX volatile		//main.c文件中使用全局变量不加“extern”前缀
#else
#define G_VAR_PREFIX extern volatile	//其他文件中使用全局变量自动加“extern”前缀
#endif

//定义全局变量
G_VAR_PREFIX bool raw_img_done;

//定义使用的调试号
#define UART_USE  UART_MOD0

#endif
