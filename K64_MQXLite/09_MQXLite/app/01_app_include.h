#ifndef __APP_INC_H_
#define __APP_INC_H_
//---------------------------------------------------------------------------

//1.包含头文件
#include "bsp.h"
#include "mqxlite.h"
#include "mqx_inc.h"
#include "light.h"
#include "lwevent.h"
#include "printf.h"
#include "string.h"



//2.声明全局变量
//防止全局变量重复声明的前缀处理方法
#ifdef GLOBLE_VAR             //GLOBLE_VAR在task_main.c文件中宏定义
#define G_VAR_PREFIX          //task_main.c文件中使用全局变量不加“extern”前缀
#else
#define G_VAR_PREFIX  extern  //其他文件中使用全局变量自动加“extern”前缀
#endif
//声明全局变量（全局变量类型前一律前缀G_VAR_PREFIX）

//G_VAR_PREFIX LWEVENT_STRUCT  lwevent_group1;      //轻量级事件组

#define DELAY_TIMES    200           // 每个tick对应5ms

//1.4 声明轻量级事件组
G_VAR_PREFIX LWEVENT_STRUCT  lwevent_group;  //轻量级事件组

//3.登记任务模板编号
#define TASK_MAIN                     1
#define TASK_LIGHT                    2

//4.为任务创建任务栈
#define TASK_MAIN_STACK_SIZE      (sizeof(TD_STRUCT) + 2048 + PSP_STACK_ALIGNMENT + 1)
#define TASK_LIGHT_STACK_SIZE     (sizeof(TD_STRUCT) + 512 + PSP_STACK_ALIGNMENT + 1)

G_VAR_PREFIX uint_8 task_main_stack[TASK_MAIN_STACK_SIZE];
G_VAR_PREFIX uint_8 task_light_stack[TASK_LIGHT_STACK_SIZE];

//5.声明任务函数
void task_main(uint32_t initial_data);
void task_light(uint32_t initial_data);

//6.声明中断处理函数
void UART1_RX_ISR();


//---------------------------------------------------------------------------
#endif    //app_inc.h
