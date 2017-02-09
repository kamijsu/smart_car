//==========================================================================
//文件名称：light.c
//功能概要：小灯应用驱动程序源文件
//==========================================================================

#include "light.h"

//==========================================================================
//函数名称: light_init
//函数返回: 无
//参数说明: light:小灯:
//               LIGHT_RED:  红灯;
//               LIGHT_GREEN:绿灯;
//               LIGHT_BLUE: 蓝灯;
//         state:小灯初始状态:
//               LIGHT_ON: 灯亮;
//               LIGHT_OFF:灯暗;
//功能概要: 初始化小灯，并设置初始状态
//==========================================================================
void light_init(uint8 light, uint8 state) {
	//配置为输出
	gpio_init(light, GPIO_DIR_OUTPUT, state);
}

//==========================================================================
//函数名称: light_set
//函数返回: 无
//参数说明: light:小灯:
//               LIGHT_RED:  红灯;
//               LIGHT_GREEN:绿灯;
//               LIGHT_BLUE: 蓝灯;
//         state:小灯状态:
//               LIGHT_ON: 灯亮;
//               LIGHT_OFF:灯暗;
//功能概要: 设置小灯状态，小灯需要被初始化
//==========================================================================
void light_set(uint8 light, uint8 state) {
	gpio_set_level(light, state);
}

//==========================================================================
//函数名称: light_change
//函数返回: 无
//参数说明: light:小灯:
//               LIGHT_RED:  红灯;
//               LIGHT_GREEN:绿灯;
//               LIGHT_BLUE: 蓝灯;
//功能概要: 切换小灯状态，小灯需要被初始化
//==========================================================================
void light_change(uint8 light) {
	gpio_toggle_level(light);
}

