#include "01_app_include.h"    //应用任务公共头文件

//===========================================================================
//任务名称：task_light
//功能概要：指示灯闪烁
//参数说明：未使用
//===========================================================================
void task_light(uint32_t initial_data)
{
    while (TRUE)
    {
    	light_change(LIGHT_RED);                  //红灯亮
    	printf("light red \r\n");
	    _time_delay_ticks(200);			          //延时1s，(放弃CPU控制权)
    }
}


