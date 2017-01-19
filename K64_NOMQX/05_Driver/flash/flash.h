//==========================================================================
//文件名称：flash.h
//功能概要：K64 Flash底层驱动程序头文件
//==========================================================================

#ifndef _FLASH_H
#define _FLASH_H

//包含公共头文件
#include "common.h"

//==========================================================================
//说明: 芯片型号为MK64FX512VLL12，拥有FlexNVM以及FlexRAM，即拥有数据flash以及
//     模拟EEPROM，可以使用程序flash、数据flash、模拟EEPROM和RAM(未作为EEPROM的部分)，
//     若芯片无FlexNVM以及FlexRAM，则只可以使用程序flash和RAM
//==========================================================================

//定义flash块
#define FLASH_BLK_PFLASH	(0)	//程序flash
#define FLASH_BLK_DFLASH	(1)	//数据flash

//定义flash块起始地址
#define FLASH_ADDR_PFLASH	(0x00000000)	//0x00000000-0x0007FFFF
#define FLASH_ADDR_DFLASH	(0x10000000)	//0x10000000-0x1001FFFF
#define FLASH_ADDR_RAM		(0x14000000)	//0x14000000-0x14000FFF

#endif
