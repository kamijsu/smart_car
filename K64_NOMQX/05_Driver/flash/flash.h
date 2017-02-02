//==========================================================================
//文件名称：flash.h
//功能概要：K64 Flash底层驱动程序头文件
//==========================================================================

#ifndef _FLASH_H
#define _FLASH_H

//包含公共头文件
#include "common.h"

//==========================================================================
//说明: 芯片型号为MK64FX512VLL12，拥有数据flash以及FlexRAM，数据flash分区前，
//     FlexRAM仅能作为普通RAM使用，若数据flash分配空间作为EEPROM备份，
//     则FlexRAM可以被作为EEPROM使用;
//     若芯片无数据flash以及FlexRAM，则只可以使用程序flash和FlexRAM的RAM模式
//注意: 进行flash写入和擦除操作时，为了安全，请关闭总中断
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
//函数名称: flash_init
//函数返回: 无
//参数说明: 无
//功能概要: 初始化flash模块
//备注: 该函数是在等待flash复位完毕，为了各种操作的安全性，需要调用该函数
//==========================================================================
void flash_init();

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
//                数据flash时，取值范围为[0,31]，分区后会相应减少，具体见分区说明;
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
//                数据flash时，取值范围为[0,31]，分区后会相应减少，具体见分区说明;
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
//                数据flash时，取值范围为[0,31]，分区后会相应减少，具体见分区说明;
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
//备注: 若数据flash分配空间给EEPROM备份，该函数将无法使用，调用时返回非法参数
//==========================================================================
FlashResult flash_erase_dflash();

//==========================================================================
//函数名称: flash_check_empty
//函数返回: 检查结果:
//         FlashSuccess:        检查成功，即该区域全部为0xFF;
//         FlashIllegalArgument:非法参数;
//         FlashVerifyFail:     检查失败，即该区域不全为0xFF;
//参数说明: blk:flash块:
//             FLASH_BLK_PFLASH:程序flash;
//             FLASH_BLK_DFLASH:数据flash;
//         sector:扇区号:
//                程序flash时，取值范围为[0,127];
//                数据flash时，取值范围为[0,31]，分区后会相应减少，具体见分区说明;
//         offset:扇区内偏移地址，必须为16的倍数
//         num:检查的字节数，必须为16的倍数
//功能概要: 检查相应flash块的一片区域是否为空，即该区域是否全为0xFF
//备注: 扇区大小为4KB，当一个扇区检查完毕时，会切换到下一扇区;
//     程序flash与数据flash不连续，程序flash检查完毕时不会切换至数据flash
//==========================================================================
FlashResult flash_check_empty(uint8 blk, uint8 sector, uint16 offset,
		uint16 num);

//==========================================================================
//函数名称: flash_check_empty_dflash
//函数返回: 检查结果:
//         FlashSuccess:        检查成功，即数据flash全部为0xFF;
//         FlashIllegalArgument:非法参数;
//         FlashVerifyFail:     检查失败，即数据flash不全为0xFF;
//参数说明: 无
//功能概要: 检查数据flash是否为空，即是否全为0xFF
//备注: 若数据flash分配空间给EEPROM备份，该函数将无法使用，调用时返回非法参数
//==========================================================================
FlashResult flash_check_empty_dflash();

//==========================================================================
//函数名称: flash_protect
//函数返回: 无
//参数说明: blk:flash块或FlexRAM块:
//             FLASH_BLK_PFLASH: 程序flash;
//             FLASH_BLK_DFLASH: 数据flash;
//             FLASH_BLK_FLEXRAM:FlexRAM;
//         region:区域号:
//                程序flash时，取值范围为[0,31];
//                数据flash时，取值范围为[0,7];
//                FlexRAM时，取值范围为[0,7];
//功能概要: 保护flash块或FlexRAM块的一片区域，保护后该区域无法擦除或写入
//备注: 保护会在下次复位后失效，若想复位后依然被保护，可以修改启动时flash的配置;
//
//     对程序flash，每个区域保护4个扇区，即0号区域保护0-3号扇区，
//     1号区域保护4-7号扇区，以此类推;
//
//     对数据flash，当分区大小为128K或者96K时，每个区域保护4个扇区，
//     且分区大小为96K时，区域号有效范围变为[0,5]，
//     分区大小为64K时，每个区域保护2个扇区，
//     分区大小为32K时，每个区域保护1个扇区，
//     分区大小为0时，保护变得没有意义;
//
//     对FlexRAM，保护仅在FlexRAM作为EEPROM时有效;
//     将FlexRAM的分区大小平均分成8份，每个区域保护对应的一份，低区域号对应低物理地址
//==========================================================================
void flash_protect(uint8 blk, uint8 region);

//==========================================================================
//函数名称: flash_check_protected
//函数返回: true:该区域被保护; false:该区域未被保护
//参数说明: blk:flash块或FlexRAM块:
//             FLASH_BLK_PFLASH: 程序flash;
//             FLASH_BLK_DFLASH: 数据flash;
//             FLASH_BLK_FLEXRAM:FlexRAM;
//         region:区域号:
//                程序flash时，取值范围为[0,31];
//                数据flash时，取值范围为[0,7];
//                FlexRAM时，取值范围为[0,7];
//功能概要: 检查flash块或FlexRAM块的一片区域是否被保护，区域说明见保护函数
//==========================================================================
bool flash_check_protected(uint8 blk, uint8 region);

//==========================================================================
//函数名称: flash_partition
//函数返回: 分区结果:
//         FlashSuccess:        分区成功;
//         FlashIllegalArgument:非法参数;
//         FlashVerifyFail:     分区完毕，但分区结果与预期不同;
//参数说明: dflash_size:数据flash分区大小:
//                     FLASH_DFLASH_SIZE_128:数据flash:128KB  EEPROM备份:  0KB;
//                     FLASH_DFLASH_SIZE_96: 数据flash: 96KB  EEPROM备份: 32KB;
//                     FLASH_DFLASH_SIZE_64: 数据flash: 64KB  EEPROM备份: 64KB;
//                     FLASH_DFLASH_SIZE_32: 数据flash: 32KB  EEPROM备份: 96KB;
//                     FLASH_DFLASH_SIZE_0:  数据flash:  0KB  EEPROM备份:128KB;
//         eeprom_size:EEPROM分区大小:
//                     FLASH_EEPROM_SIZE_4K: 4KB;
//                     FLASH_EEPROM_SIZE_2K: 2KB;
//                     FLASH_EEPROM_SIZE_1K: 1KB;
//                     FLASH_EEPROM_SIZE_512:512B;
//                     FLASH_EEPROM_SIZE_256:256B;
//                     FLASH_EEPROM_SIZE_128:128B;
//                     FLASH_EEPROM_SIZE_64: 64B;
//                     FLASH_EEPROM_SIZE_32: 32B;
//                     FLASH_EEPROM_SIZE_0:  0B;
//         eeprom_split:EEPROM子系统分割因子:
//                      FLASH_EEPROM_SPLIT_1_7:A子系统:EEPROM*1/8 B子系统:EEPROM*7/8;
//                      FLASH_EEPROM_SPLIT_1_3:A子系统:EEPROM*1/4 B子系统:EEPROM*3/4;
//                      FLASH_EEPROM_SPLIT_1_1:A子系统:EEPROM*1/2 B子系统:EEPROM*1/2;
//功能概要: 对数据flash进行分区，将其一部分空间作为EEPROM备份，
//         并定义FlexRAM在EEPROM模式时的可用空间大小
//备注: 默认情况下，数据flash分区大小为128K，EEPROM分区大小为0，子系统分割因子为1:1;
//
//     数据flash分配空间给EEPROM备份时，EEPROM分区大小不能为0;
//     EEPROM分区大小不为0时，数据flash必须分配空间给EEPROM备份;
//     EEPROM分区大小为32时，子系统分割因子不能为1:7;
//     EEPROM分区大小为4K时，若子系统分割因子不为1:1，则EEPROM备份必须大于32K;
//
//     数据flash分区后，可用扇区会相应的减少，具体如下:
//     分区大小为128K时，可用扇区范围为[0,31]，
//     分区大小为96K时，可用扇区范围为[0,23]，
//     分区大小为64K时，可用扇区范围为[0,15]，
//     分区大小为32K时，可用扇区范围为[0,7]，
//     分区大小为0时，无可用扇区;
//
//     EEPROM按照分割因子被分割为A、B子系统，在使用时无需考虑子系统差别，
//     它仅影响EEPROM的使用寿命，当EEPROM分区大小与EEPROM备份空间大小之比越小时，
//     使用寿命越长;
//     EEPROM备份被平均分成大小相同的两份，为A、B子系统提供备份;
//     EEPROM备份空间由硬件自动操作，无需手动管理;
//
//     该函数执行过程中不能被中断，且执行过分区后，再次分区时会返回非法参数，
//     若要清除分区，需要整体擦除;
//
//     执行过分区后，复位芯片时，FlexRAM会默认为EEPROM模式
//==========================================================================
FlashResult flash_partition(uint8 dflash_size, uint8 eeprom_size,
		uint8 eeprom_split);

//==========================================================================
//函数名称: flash_partition_get_info
//函数返回: true:获取信息成功; false:获取信息失败
//参数说明: dflash_size:存储数据flash分区大小的地址
//         eeprom_size:存储EEPROM分区大小的地址
//         eeprom_split:存储EEPROM子系统分割因子的地址
//功能概要: 获取当前的分区信息，成功时存储相应参数，参数值与宏定义值相同
//==========================================================================
bool flash_partition_get_info(uint8* dflash_size, uint8* eeprom_size,
		uint8* eeprom_split);

//==========================================================================
//函数名称: flash_flexram_set_mode
//函数返回: 设置模式结果:
//         FlashSuccess:        设置成功;
//         FlashIllegalArgument:非法参数;
//参数说明: flexram_mode:FlexRAM的模式:
//                      FLASH_FLEXRAM_MODE_RAM:   RAM;
//                      FLASH_FLEXRAM_MODE_EEPROM:EEPROM;
//功能概要: 设置FlexRAM的模式
//备注: 若EEPROM分区大小为0，设置EEPROM模式时，返回非法参数;
//     EEPROM模式时，FlexRAM可用空间由EEPROM分区大小决定，且剩余空间无法使用;
//     RAM模式时，FlexRAM可用空间为4K，且每次设置为RAM模式时，整个空间会被填充0xFF
//==========================================================================
FlashResult flash_flexram_set_mode(uint8 flexram_mode);

//==========================================================================
//函数名称: flash_flexram_get_mode
//函数返回: FlexRAM的模式:
//         FLASH_FLEXRAM_MODE_RAM:   RAM;
//         FLASH_FLEXRAM_MODE_EEPROM:EEPROM;
//参数说明: 无
//功能概要: 获取当前FlexRAM的模式，返回值与宏定义值相同
//==========================================================================
uint8 flash_flexram_get_mode();

//==========================================================================
//函数名称: flash_flexram_write
//函数返回: true:写入成功; false:写入失败，即EEPROM模式时，写入区域包含被保护区域
//参数说明: offset:偏移地址
//         num:写入的字节数
//         data:写入数据的首地址
//功能概要: 将一定数量的数据写入FlexRAM中
//备注: 写入前无需擦除，且写入数量仅受可用空间大小限制，可用空间大小见设置模式说明;
//     EEPROM模式时，该数据掉电后不会丢失，且重新上电时会从EEPROM备份空间拷贝至FlexRAM;
//     在遇到被保护区域前的数据会被写入FlexRAM，遇到被保护区域后，立即返回失败，放弃写入;
//     RAM模式时，该数据掉电后会丢失
//==========================================================================
bool flash_flexram_write(uint16 offset, uint16 num, uint8* data);

//==========================================================================
//函数名称: flash_flexram_read
//函数返回: 无
//参数说明: offset:偏移地址
//         num:读取的字节数
//         data:存储读取数据的首地址
//功能概要: 从FlexRAM中读取一定数量的数据，存储至RAM中
//==========================================================================
void flash_flexram_read(uint16 offset, uint16 num, uint8* data);

#endif
