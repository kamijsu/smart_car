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
//#include "ems.h"
#include "encoder.h"
//#include "gyro_acce.h"
#include "light.h"
#include "motor.h"
#include "reed_switch.h"
#include "control.h"
#include "structs.h"
#include "visual_scope.h"
#include "ftm.h"
#include "temp_sensor.h"
#include "crc.h"
#include "crc16_modbus.h"
#include "frame.h"
#include "frame_string.h"
#include "frame_cmd.h"
#include "frame_data.h"
#include "frame_bytes.h"
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

//防止全局变量重复声明的前缀处理方法，并且添加volatile前缀，
//因为全局变量会被多个.c文件使用，若不使用volatile前缀，则不会每次去变量所在地址读值，
//会造成某个.c文件修改变量后，对别的.c文件不可见问题，而.c文件中的静态变量不存在该问题，
//因为仅有该.c文件可以访问该静态变量，即该静态变量始终对该.c文件是可见的
#ifdef GLOBLE_VAR				//GLOBLE_VAR在main.c文件中宏定义
#define G_VAR_PREFIX volatile		//main.c文件中使用全局变量不加“extern”前缀
#else
#define G_VAR_PREFIX extern volatile	//其他文件中使用全局变量自动加“extern”前缀
#endif

G_VAR_PREFIX bool raw_img_done;

//定义全局变量
//时间标志参数
G_VAR_PREFIX TimeFlag time0_flag;

//编码器脉冲计数器
G_VAR_PREFIX EncoderCounter encoder_counter;

//小车标志参数
G_VAR_PREFIX CarFlag car_flag;

//定义小灯闪烁频率
#define RUN_COUNTER_MAX   3500000ul

//定义使用的调试号
#define UART_USE  UART_MOD0

/* 引脚
 * UART_USE:    PTE0   TX;                PTE1   RX
 * 串口接线:		white  RX;                green  TX
 * MOTOR1:      PTC1(purple) PWM1;        PTC2(grey)    PWM2
 * MOTOR2:      PTC3(purple) PWM3;        PTC4(grey)    PWM4
 * gyro_acce:   PTE24(green) X轴角速度(AR2); PTE25(blue)   Z轴加速度(Zout)
 * SPEED1:		PTD0(black);       5V(brown);       GND(blue)
 * SPEED2:		PTD1(white);       5V(brown);       GND(blue)
 * REED_SWITCH_INPUT  PTD2;
 * REED_SWITCH_OUTPUT PTD3;
 * EMS_CH0:		PTB0(green)  OUTA
 * EMS_CH1:		PTB1(blue)   OUTC
 * EMS_CH2:		PTB2(green)  OUTB
 * EMS_CH3:		PTB3(blue)   OUTD
 * */

#endif
