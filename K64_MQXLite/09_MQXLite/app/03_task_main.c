#define GLOBLE_VAR

#include "01_app_include.h"

//===========================================================================
//�������ƣ�task_main
//���ܸ�Ҫ��MQX������������Ҫʵ��ȫ�ֱ�����ʼ���������������񡢰�װ�û�ISR
//����˵����δʹ��
//===========================================================================
void task_main(uint32_t initial_data)
{
    //1.ȫ�ֱ�����ʼ��
    _lwevent_create(&lwevent_group,0);  //�����������¼���

    //2. �����ж�
    DISABLE_INTERRUPTS;
    //3.�����ʼ��
    light_init(LIGHT_RED,LIGHT_OFF);     //��Ƴ�ʼ��
    uart_init(UART_Debug,9600);        //��ʼ��"���Դ���"
    printf("MQX Lite test!\r\n");            //���ڷ��ͳ�ʼ����ʾ

    //4.������������
    _task_create_at(0, TASK_LIGHT, 0, task_light_stack, TASK_LIGHT_STACK_SIZE);

    //5.��װ�û�ISR
    _int_install_isr(UART0_RX_TX_IRQn+UART_Debug*2+16,UART1_RX_ISR,NULL); //ע����Դ��ڵ�ISR

    //6.ʹ��ģ���жϼ����ж�
    uart_enable_re_int(UART_Debug);   //ʹ�ܵ��Դ��ڽ����ж�

    //7.�����ж�
    ENABLE_INTERRUPTS;
    
    //------------ִ����ϣ��������������̬-----------------------------------
    _task_block();
}
