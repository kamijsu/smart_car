//==========================================================================
//�ļ����ƣ�flash.h
//���ܸ�Ҫ��K64 Flash�ײ���������ͷ�ļ�
//==========================================================================

#ifndef _FLASH_H
#define _FLASH_H

//��������ͷ�ļ�
#include "common.h"

//==========================================================================
//˵��: оƬ�ͺ�ΪMK64FX512VLL12��ӵ��FlexNVM�Լ�FlexRAM����ӵ������flash�Լ�
//     ģ��EEPROM������ʹ�ó���flash������flash��ģ��EEPROM��RAM(δ��ΪEEPROM�Ĳ���)��
//     ��оƬ��FlexNVM�Լ�FlexRAM����ֻ����ʹ�ó���flash��RAM
//==========================================================================

//����flash��
#define FLASH_BLK_PFLASH	(0)	//����flash
#define FLASH_BLK_DFLASH	(1)	//����flash

//����flash����ʼ��ַ
#define FLASH_ADDR_PFLASH	(0x00000000)	//0x00000000-0x0007FFFF
#define FLASH_ADDR_DFLASH	(0x10000000)	//0x10000000-0x1001FFFF
#define FLASH_ADDR_RAM		(0x14000000)	//0x14000000-0x14000FFF

#endif
