/*
 * tasks.c
 *
 *  Created on: Dec 13, 2012
 *      Author: SY
 */
#include "01_app_include.h"
// Ϊ����������ע������ջ
const uint_8 * mqx_task_stack_pointers[] = 
{
    task_main_stack,
    NULL
};
// ����ģ���б�
const TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
    // ������,  ������,   ����ջ��С,        ���ȼ�, ������,   ��������
    {TASK_MAIN,    task_main,   TASK_MAIN_STACK_SIZE,      7,   "main",MQX_AUTO_START_TASK},
    {TASK_LIGHT,  task_light,   TASK_LIGHT_STACK_SIZE,      9,   "task_light", 0},
    {0}
};
