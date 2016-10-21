#define GLOBLE_VAR

#include "01_app_include.h"

//===========================================================================
//任务名称：task_main
//功能概要：MQX自启动任务，主要实现全局变量初始化、创建其他任务、安装用户ISR
//参数说明：未使用
//===========================================================================
void task_main(uint32_t initial_data)
{
    //1.全局变量初始化
    _lwevent_create(&lwevent_group,0);  //创建轻量级事件组

    //2. 关总中断
    DISABLE_INTERRUPTS;
    //3.外设初始化
    light_init(LIGHT_RED,LIGHT_OFF);     //红灯初始化
    uart_init(UART_Debug,9600);        //初始化"调试串口"
    printf("MQX Lite test!\r\n");            //串口发送初始化提示

    //4.创建其他任务
    _task_create_at(0, TASK_LIGHT, 0, task_light_stack, TASK_LIGHT_STACK_SIZE);

    //5.安装用户ISR
    _int_install_isr(UART0_RX_TX_IRQn+UART_Debug*2+16,UART1_RX_ISR,NULL); //注册调试串口的ISR

    //6.使能模块中断及总中断
    uart_enable_re_int(UART_Debug);   //使能调试串口接收中断

    //7.开总中断
    ENABLE_INTERRUPTS;
    
    //------------执行完毕，本任务进入阻塞态-----------------------------------
    _task_block();
}
