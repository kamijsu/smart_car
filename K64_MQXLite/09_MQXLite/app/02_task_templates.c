/*
 * tasks.c
 *
 *  Created on: Dec 13, 2012
 *      Author: SY
 */
#include "01_app_include.h"
// 为自启动任务注册任务栈
const uint_8 * mqx_task_stack_pointers[] = 
{
    task_main_stack,
    NULL
};
// 任务模板列表
const TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
    // 任务编号,  任务函数,   任务栈大小,        优先级, 任务名,   任务属性
    {TASK_MAIN,    task_main,   TASK_MAIN_STACK_SIZE,      7,   "main",MQX_AUTO_START_TASK},
    {TASK_LIGHT,  task_light,   TASK_LIGHT_STACK_SIZE,      9,   "task_light", 0},
    {0}
};
