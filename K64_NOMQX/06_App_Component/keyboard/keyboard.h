//==========================================================================
//文件名称：keyboard.h
//功能概要：键盘应用驱动程序头文件
//==========================================================================

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "common.h"
#include "gpio.h"
#include "pit.h"

//需要提供一个可用的延时函数
#define KEYBOARD_DELAY_MS(ms)		pit_delay_ms(PIT_CH1, ms)
//定义键盘读取成功之后的延时时间，单位ms
#define KEYBOARD_TIME_DELAY 		(200)

//定义键盘的行线引脚，作为GPIO输出
#define KEYBOARD_ROW0       (COM_PORTB | 10)
#define KEYBOARD_ROW1       (COM_PORTB | 9)
#define KEYBOARD_ROW2       (COM_PORTA | 4)
#define KEYBOARD_ROW3       (COM_PORTA | 5)
//定义键盘的列线引脚，作为GPIO输入，必须为同一端口下引脚
#define KEYBOARD_COL0       (COM_PORTA | 14)
#define KEYBOARD_COL1       (COM_PORTA | 15)
#define KEYBOARD_COL2       (COM_PORTA | 16)
#define KEYBOARD_COL3       (COM_PORTA | 17)

//定义键盘按键字符
#define KEYBOARD_KEY_ROW0_COL0	('7')
#define KEYBOARD_KEY_ROW0_COL1	('8')
#define KEYBOARD_KEY_ROW0_COL2	('9')
#define KEYBOARD_KEY_ROW0_COL3	('P')

#define KEYBOARD_KEY_ROW1_COL0	('4')
#define KEYBOARD_KEY_ROW1_COL1	('5')
#define KEYBOARD_KEY_ROW1_COL2	('6')
#define KEYBOARD_KEY_ROW1_COL3	('N')

#define KEYBOARD_KEY_ROW2_COL0	('1')
#define KEYBOARD_KEY_ROW2_COL1	('2')
#define KEYBOARD_KEY_ROW2_COL2	('3')
#define KEYBOARD_KEY_ROW2_COL3	('B')

#define KEYBOARD_KEY_ROW3_COL0	('0')
#define KEYBOARD_KEY_ROW3_COL1	('.')
#define KEYBOARD_KEY_ROW3_COL2	('C')
#define KEYBOARD_KEY_ROW3_COL3	('O')

//定义键盘扫描阈值，扫描相同键值数到达阈值时，获取键值
#define KEYBORAD_THRESHOLD	(5)

//==========================================================================
//函数名称: keyboard_init
//函数返回: 无
//参数说明: 无
//功能概要: 初始化键盘，相应配置在keyboard.h中
//备注: 已上拉列线引脚电阻
//==========================================================================
void keyboard_init();

//==========================================================================
//函数名称: keyboard_read
//函数返回: true:读取成功; false:读取失败，无键按下
//参数说明: key:存储键盘按键字符的地址
//功能概要: 读取键盘，成功时存储读取的键盘按键字符，并延时定义的时间
//==========================================================================
bool keyboard_read(uint8* key);

//==========================================================================
//函数名称: keyboard_enable_int
//函数返回: 无
//参数说明: 无
//功能概要: 使能键盘中断，当有按键被按下时，触发中断
//备注: 列线引脚所在端口为PORTA时，中断服务函数为PORTA_IRQHandler
//==========================================================================
void keyboard_enable_int();

//==========================================================================
//函数名称: keyboard_disable_int
//函数返回: 无
//参数说明: 无
//功能概要: 关闭键盘中断
//==========================================================================
void keyboard_disable_int();

//==========================================================================
//函数名称: keyboard_get_int
//函数返回: true:产生中断; false:未产生中断
//参数说明: 无
//功能概要: 获取键盘中断标志
//==========================================================================
bool keyboard_get_int();

//==========================================================================
//函数名称: keyboard_clear_int
//函数返回: 无
//参数说明: 无
//功能概要: 清除键盘中断标志
//备注: 在读取键盘后，再清除中断标志
//==========================================================================
void keyboard_clear_int();

#endif
