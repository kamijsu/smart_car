#include "includes.h"

void UART1_RX_ISR()
{
    uint8_t ch;
    uint8_t flag = 1;
    //关中断
    DISABLE_INTERRUPTS;
    ch = uart_re1(UART_1, &flag);
    if (0 == flag)
    {
        uart_send1(UART_1, ch);
    }
    //开中断
    ENABLE_INTERRUPTS;
}


