/*
 * bsp.h
 *
 *  Created on: Dec 13, 2012
 *      Author: SY
 */

#ifndef BSP_H_
#define BSP_H_

#include "psptypes.h"
#include "uart.h"
#include "gpio.h"

#define BSP_CORE_CLOCK                  (DEFAULT_SYSTEM_CLOCK)  // �ں�ʱ��Ƶ��(Hz)
#define BSP_SYSTEM_CLOCK                (DEFAULT_SYSTEM_CLOCK/2)  // ϵͳʱ��Ƶ��(Hz)

// SYSTICK��Ƶ��
#define BSP_ALARM_FREQUENCY             (200)
// SYSTICK�����ڣ�ms��5ms/tick
#define BSP_ALARM_RESOLUTION            (1000 / BSP_ALARM_FREQUENCY)

// ����ϵͳ�δ�ʱ���ж�
#define BSP_TIMER_INTERRUPT_VECTOR      SysTick_IRQn+16   // ������оƬͷ�ļ�
#define BSP_TIMER_INTERRUPT_PRIORITY    0    // CM0�н��������ɱ�����ȼ�����

typedef void (*const tIsrFunc)(void);

extern uint_32 _bsp_enable_card ( void );

#endif /* BSP_H_ */
