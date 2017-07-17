//============================================================================
//�ļ����ƣ�includes.h
//���ܸ�Ҫ��Ӧ�ù�����ͷ�ļ�
//��Ȩ���У����ݴ�ѧ��˼����Ƕ��ʽ����(sumcu.suda.edu.cn)
//�汾���£�2012-11-12  V2.0
//          2014-11-9  V3.0
//============================================================================

#ifndef _INCLUDES_H
#define _INCLUDES_H

#include "common.h"
#include <stdlib.h>
#include <stdio.h>
#include "adc.h"
#include "dac.h"
#include "gpio.h"
#include "pit.h"
#include "uart.h"
#include "encoder.h"
#include "keyboard.h"
#include "gyro_acce.h"
#include "light.h"
#include "motor.h"
#include "reed_switch.h"
#include "control.h"
#include "visual_scope.h"
#include "ftm.h"
#include "temp_sensor.h"
#include "crc.h"
#include "crc16_modbus.h"
#include "frame.h"
#include <math.h>
#include "rng.h"
#include "arm_math.h"
#include "mmcau.h"
#include "crypto.h"
#include "flash.h"
#include "spi.h"
#include "custom.h"
#include "oled.h"
#include "i2c.h"
#include "dma.h"
#include "camera.h"
#include "vcan.h"
#include "menu.h"
#include "time.h"
#include "param.h"

//��ֹȫ�ֱ����ظ�������ǰ׺���������������volatileǰ׺��
//��Ϊȫ�ֱ����ᱻ���.c�ļ�ʹ�ã�����ʹ��volatileǰ׺���򲻻�ÿ��ȥ�������ڵ�ַ��ֵ��
//�����ĳ��.c�ļ��޸ı����󣬶Ա��.c�ļ����ɼ����⣬��.c�ļ��еľ�̬���������ڸ����⣬
//��Ϊ���и�.c�ļ����Է��ʸþ�̬���������þ�̬����ʼ�նԸ�.c�ļ��ǿɼ���
#ifdef GLOBLE_VAR				//GLOBLE_VAR��main.c�ļ��к궨��
#define G_VAR_PREFIX volatile		//main.c�ļ���ʹ��ȫ�ֱ������ӡ�extern��ǰ׺
#else
#define G_VAR_PREFIX extern volatile	//�����ļ���ʹ��ȫ�ֱ����Զ��ӡ�extern��ǰ׺
#endif

//����ȫ�ֱ���
G_VAR_PREFIX bool raw_img_done;

//����ʹ�õĵ��Ժ�
#define UART_USE  UART_MOD0

#endif
