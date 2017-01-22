//==========================================================================
//文件名称：flash.h
//功能概要：K64 Flash底层驱动程序头文件
//==========================================================================

#ifndef _FLASH_H
#define _FLASH_H

//包含公共头文件
#include "common.h"
//需要用到memcpy
#include <string.h>

//==========================================================================
//说明: 芯片型号为MK64FX512VLL12，拥有FlexNVM以及FlexRAM，即拥有数据flash以及
//     模拟EEPROM，可以使用程序flash、数据flash、模拟EEPROM和RAM(未作为EEPROM的部分)，
//     若芯片无FlexNVM以及FlexRAM，则只可以使用程序flash和RAM
//==========================================================================

//定义flash块和FlexRAM块
#define FLASH_BLK_PFLASH	(0)	//程序flash
#define FLASH_BLK_DFLASH	(1)	//数据flash
#define FLASH_BLK_FLEXRAM	(2)	//FlexRAM

//定义flash块起始地址
#define FLASH_ADDR_PFLASH	(0x00000000)	//0x00000000-0x0007FFFF
#define FLASH_ADDR_DFLASH	(0x10000000)	//0x10000000-0x1001FFFF
#define FLASH_ADDR_FLEXRAM	(0x14000000)	//0x14000000-0x14000FFF

//定义数据flash分区大小
#define FLASH_DFLASH_SIZE_128	(0xF)	//数据flash:128KB  EEPROM备份:  0KB
#define FLASH_DFLASH_SIZE_96	(0x3)	//数据flash: 96KB  EEPROM备份: 32KB
#define FLASH_DFLASH_SIZE_64	(0x4)	//数据flash: 64KB  EEPROM备份: 64KB
#define FLASH_DFLASH_SIZE_32	(0xB)	//数据flash: 32KB  EEPROM备份: 96KB
#define FLASH_DFLASH_SIZE_0		(0x8)	//数据flash:  0KB  EEPROM备份:128KB

//定义EEPROM分区大小
#define FLASH_EEPROM_SIZE_4K	(0x2)	// 4KB
#define FLASH_EEPROM_SIZE_2K	(0x3)	// 2KB
#define FLASH_EEPROM_SIZE_1K	(0x4)	// 1KB
#define FLASH_EEPROM_SIZE_512	(0x5)	//512B
#define FLASH_EEPROM_SIZE_256	(0x6)	//256B
#define FLASH_EEPROM_SIZE_128	(0x7)	//128B
#define FLASH_EEPROM_SIZE_64	(0x8)	// 64B
#define FLASH_EEPROM_SIZE_32	(0x9)	// 32B
#define FLASH_EEPROM_SIZE_0		(0xF)	//  0B

//定义EEPROM子系统分割因子
#define FLASH_EEPROM_SPLIT_1_7	(0x0)	//A子系统:EEPROM*1/8 B子系统:EEPROM*7/8
#define FLASH_EEPROM_SPLIT_1_3	(0x1)	//A子系统:EEPROM*1/4 B子系统:EEPROM*3/4
#define FLASH_EEPROM_SPLIT_1_1	(0x3)	//A子系统:EEPROM*1/2 B子系统:EEPROM*1/2

//定义FlexRAM的模式
#define FLASH_FLEXRAM_MODE_RAM		(0xFF)	//RAM
#define FLASH_FLEXRAM_MODE_EEPROM	(0x00)	//EEPROM

//定义flash操作结果
typedef enum {
	FlashSuccess, 	//操作成功
	FlashIllegalArgument, //非法参数
	FlashProtected, //操作区域被保护
	FlashVerifyFail	//操作执行完毕，但确认操作结果时错误
} FlashResult;

//==========================================================================
//函数名称: flash_write
//函数返回: 写入结果:
//         FlashSuccess:        写入成功;
//         FlashIllegalArgument:非法参数;
//         FlashProtected:      写入区域被保护;
//         FlashVerifyFail:     写入完毕，但写入结果与预期不同;
//参数说明: blk:flash块:
//             FLASH_BLK_PFLASH:程序flash;
//             FLASH_BLK_DFLASH:数据flash;
//         sector:扇区号:
//                程序flash时，取值范围为[0,127];
//                数据flash时，取值范围为[0,31];
//         offset:扇区内偏移地址，必须为8的倍数
//         num:写入的字节数，必须为8的倍数
//         data:写入数据的首地址
//功能概要: 将一定数量的数据写入相应flash块的扇区中
//备注: 扇区大小为4KB，当一个扇区写满时，会切换到下一扇区;
//     程序flash与数据flash不连续，程序flash写满时不会切换至数据flash;
//     按顺序每次写入8B，若有一次不成功，则写入失败，并返回相应结果;
//     注意，若写入结果为FlashVerifyFail，说明写入前没有擦除扇区，
//     该flash块有一定几率无法读取，需要整体擦除
//==========================================================================
FlashResult flash_write(uint8 blk, uint8 sector, uint16 offset, uint16 num,
		uint8* data);

//==========================================================================
//函数名称: flash_read
//函数返回: 无
//参数说明: blk:flash块:
//             FLASH_BLK_PFLASH:程序flash;
//             FLASH_BLK_DFLASH:数据flash;
//         sector:扇区号:
//                程序flash时，取值范围为[0,127];
//                数据flash时，取值范围为[0,31];
//         offset:扇区内偏移地址
//         num:读取的字节数
//         data:存储读取数据的首地址
//功能概要: 从相应flash块的扇区中读取一定数量的数据，存储至RAM中
//备注: 偏移地址和读取字节数可以不为8的倍数;
//     扇区大小为4KB，当一个扇区读取完毕时，会切换到下一扇区;
//     程序flash与数据flash不连续，程序flash读取完毕时不会切换至数据flash
//==========================================================================
void flash_read(uint8 blk, uint8 sector, uint16 offset, uint16 num,
		uint8* data);

//==========================================================================
//函数名称: flash_erase_sector
//函数返回: 擦除结果:
//         FlashSuccess:        擦除成功;
//         FlashIllegalArgument:非法参数;
//         FlashProtected:      该扇区被保护;
//         FlashVerifyFail:     擦除完毕，但该扇区不全为0xFF;
//参数说明: blk:flash块:
//             FLASH_BLK_PFLASH:程序flash;
//             FLASH_BLK_DFLASH:数据flash;
//         sector:扇区号:
//                程序flash时，取值范围为[0,127];
//                数据flash时，取值范围为[0,31];
//功能概要: 擦除相应flash块的一块扇区
//备注: 擦除程序flash的0扇区时，会引起芯片加密，需要整体擦除
//==========================================================================
FlashResult flash_erase_sector(uint8 blk, uint8 sector);

//==========================================================================
//函数名称: flash_erase_dflash
//函数返回: 擦除结果:
//         FlashSuccess:        擦除成功;
//         FlashIllegalArgument:非法参数;
//         FlashProtected:      数据flash有区域被保护;
//         FlashVerifyFail:     擦除完毕，但数据flash不全为0xFF;
//参数说明: 无
//功能概要: 擦除数据flash
//备注: 若数据flash分配空间给EEPROM备份，擦除时，返回非法参数
//==========================================================================
FlashResult flash_erase_dflash();

FlashResult flash_check_empty(uint8 blk, uint8 sector, uint16 offset,
		uint16 num);

FlashResult flash_check_empty_dflash();

void flash_protect(uint8 blk, uint8 region);

bool flash_check_protected(uint8 blk, uint8 region);

FlashResult flash_partition(uint8 dflash_size, uint8 eeprom_size,
		uint8 eeprom_split);

bool flash_partition_get_info(uint8* dflash_size, uint8* eeprom_size,
		uint8* eeprom_split);

FlashResult flash_flexram_set_mode(uint8 flexram_mode);

uint8 flash_flexram_get_mode();

void flash_flexram_write(uint16 offset, uint16 num, uint8* data);

void flash_flexram_read(uint16 offset, uint16 num, uint8* data);

#endif
