//==========================================================================
//文件名称：flash.c
//功能概要：K64 Flash底层驱动程序源文件
//==========================================================================

#include "flash.h"
//需要用到memcpy
#include <string.h>

//Flash命令宏定义，内部使用
#define RD1BLK	(0x00)	//检查一块flash是否全为1
#define RD1SEC	(0x01)	//检查一块区域是否全为1
#define PGMCHK	(0x02)	//检查4个字节是否与预期的一致
#define RDRSRC	(0x03)	//读资源信息
#define PGM8	(0x07)  //写入8个字节
#define ERSBLK	(0x08)	//擦除一块flash
#define ERSSCR	(0x09)	//擦除一块扇区
#define PGMSEC	(0x0B)	//写入一块扇区
#define RD1ALL	(0x40)	//检查所有flash是否全为1
#define RDONCE	(0x41)	//只读一次命令
#define PGMONCE	(0x43)	//只写一次命令
#define ERSALL	(0x44)	//擦除所有区域，包括FlexRAM
#define VFYKEY	(0x45)	//验证后门访问钥匙
#define PGMPART	(0x80)	//程序分区
#define SETRAM	(0x81)	//设定FlexRAM功能

//flash块基地址表
static const uint32 flash_addr_table[] =
		{ FLASH_ADDR_PFLASH, FLASH_ADDR_DFLASH };
//flash状态寄存器各错误标志
static const uint8 flash_error_masks = FTFE_FSTAT_RDCOLERR_MASK
		| FTFE_FSTAT_ACCERR_MASK | FTFE_FSTAT_FPVIOL_MASK;

//==========================================================================
//函数名称: flash_launch
//函数返回: 操作结果:
//         FlashSuccess:        操作成功;
//         FlashIllegalArgument:非法参数;
//         FlashProtected:      操作区域被保护;
//         FlashVerifyFail:     操作执行完毕，但确认操作结果时错误;
//参数说明: 无
//功能概要: 执行flash命令
//==========================================================================
static FlashResult flash_launch() {
	uint8 temp;

	//去除各错误标志
	REG_SET_VAL(FTFE_FSTAT, flash_error_masks);
	//启动命令
	REG_SET_VAL(FTFE_FSTAT, FTFE_FSTAT_CCIF_MASK);
	//等待命令完成
	do {
		temp = FTFE_FSTAT;
	} while (!REG_GET_MASK(temp, FTFE_FSTAT_CCIF_MASK));
	//结果出现异常时，具体检查是何种错误
	if (temp != FTFE_FSTAT_CCIF_MASK) {
		//非法参数
		if (REG_GET_MASK(temp, FTFE_FSTAT_ACCERR_MASK)) {
			return FlashIllegalArgument;
		}
		//操作区域被保护
		if (REG_GET_MASK(temp, FTFE_FSTAT_FPVIOL_MASK)) {
			return FlashProtected;
		}
		//操作执行完毕，但确认操作结果时错误
		if (REG_GET_MASK(temp, FTFE_FSTAT_MGSTAT0_MASK)) {
			return FlashVerifyFail;
		}
	}
	//操作成功
	return FlashSuccess;
}

//==========================================================================
//函数名称: flash_set_addr
//函数返回: 无
//参数说明: addr:地址
//功能概要: 设置地址
//==========================================================================
static void flash_set_addr(uint32 addr) {
	REG_SET_VAL(FTFE_FCCOB1, addr >> 16);
	REG_SET_VAL(FTFE_FCCOB2, addr >> 8);
	REG_SET_VAL(FTFE_FCCOB3, addr);
}

//==========================================================================
//函数名称: flash_init
//函数返回: 无
//参数说明: 无
//功能概要: 初始化flash模块
//备注: 该函数是在等待flash复位完毕，为了各种操作的安全性，需要调用该函数
//==========================================================================
void flash_init() {
	//等待flash初始化完成
	while (!REG_GET_MASK(FTFE_FSTAT, FTFE_FSTAT_CCIF_MASK)) {
	}
}

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
		uint8* data) {
	uint32 addr;		//物理地址
	FlashResult result;	//写入结果

	//写入数量不为8的倍数时，参数非法
	if (num & 0x7) {
		return FlashIllegalArgument;
	}
	//获取起始物理地址，第23位决定选择程序flash还是数据flash
	addr = (flash_addr_table[blk] >> 5) + (sector << 12) + offset;
	//设置命令为写入8个字节
	REG_SET_VAL(FTFE_FCCOB0, PGM8);
	//写入各字节
	for (; num > 0; num -= 8, data += 8, addr += 8) {
		//设置地址
		flash_set_addr(addr);
		//设置数据
		REG_SET_VAL(FTFE_FCCOB7, data[0]);
		REG_SET_VAL(FTFE_FCCOB6, data[1]);
		REG_SET_VAL(FTFE_FCCOB5, data[2]);
		REG_SET_VAL(FTFE_FCCOB4, data[3]);
		REG_SET_VAL(FTFE_FCCOBB, data[4]);
		REG_SET_VAL(FTFE_FCCOBA, data[5]);
		REG_SET_VAL(FTFE_FCCOB9, data[6]);
		REG_SET_VAL(FTFE_FCCOB8, data[7]);
		//执行命令，并获取结果
		result = flash_launch();
		//写入失败时，返回失败原因
		if (result != FlashSuccess) {
			return result;
		}
	}
	//写入成功
	return FlashSuccess;
}

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
void flash_read(uint8 blk, uint8 sector, uint16 offset, uint16 num, uint8* data) {
	//将逻辑地址转换为物理地址，并复制到内存中
	memcpy(data, (uint8*) (flash_addr_table[blk] + (sector << 12) + offset),
			num);
}

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
FlashResult flash_erase_sector(uint8 blk, uint8 sector) {
	//设置命令为擦除一块扇区
	REG_SET_VAL(FTFE_FCCOB0, ERSSCR);
	//设置地址
	flash_set_addr((flash_addr_table[blk] >> 5) + (sector << 12));
	//执行命令，并返回结果
	return flash_launch();
}

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
FlashResult flash_erase_dflash() {
	//设置命令为擦除一块flash
	REG_SET_VAL(FTFE_FCCOB0, ERSBLK);
	//设置地址
	flash_set_addr(flash_addr_table[FLASH_BLK_DFLASH] >> 5);
	//执行命令，并返回结果
	return flash_launch();
}

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
		uint16 num) {
	//检查数量不为16的倍数时，参数非法
	if (num & 0xF) {
		return FlashIllegalArgument;
	}
	//设置命令为检查一块区域是否全为1
	REG_SET_VAL(FTFE_FCCOB0, RD1SEC);
	//设置地址
	flash_set_addr((flash_addr_table[blk] >> 5) + (sector << 12) + offset);
	//设置检查128位块的数量
	REG_SET_VAL(FTFE_FCCOB4, num >> 12);
	REG_SET_VAL(FTFE_FCCOB5, num >> 4);
	//设置对齐等级为普通
	REG_SET_VAL(FTFE_FCCOB6, 0x00);
	//执行命令，并返回结果
	return flash_launch();
}

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
FlashResult flash_check_empty_dflash() {
	//设置命令为检查一块flash是否全为1
	REG_SET_VAL(FTFE_FCCOB0, RD1BLK);
	//设置地址
	flash_set_addr(flash_addr_table[FLASH_BLK_DFLASH] >> 5);
	//设置对齐等级为普通
	REG_SET_VAL(FTFE_FCCOB4, 0x00);
	//执行命令，并返回结果
	return flash_launch();
}

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
void flash_protect(uint8 blk, uint8 region) {
	uint8 shift;	//保护寄存器的偏移量

	//计算偏移量
	shift = region & 0x7;
	//等待命令完成
	while (!REG_GET_MASK(FTFE_FSTAT, FTFE_FSTAT_CCIF_MASK)) {
	}
	//根据所选块进行保护
	switch (blk) {
	case FLASH_BLK_PFLASH:	//程序flash
		//根据区域选择寄存器进行保护
		switch (region >> 3) {
		case 0:
			REG_CLR_SHIFT(FTFE_FPROT3, shift);
			break;
		case 1:
			REG_CLR_SHIFT(FTFE_FPROT2, shift);
			break;
		case 2:
			REG_CLR_SHIFT(FTFE_FPROT1, shift);
			break;
		case 3:
			REG_CLR_SHIFT(FTFE_FPROT0, shift);
			break;
		}
		break;
	case FLASH_BLK_DFLASH:	//数据flash
		REG_CLR_SHIFT(FTFE_FDPROT, shift);
		break;
	case FLASH_BLK_FLEXRAM:	//FlexRAM
		REG_CLR_SHIFT(FTFE_FEPROT, shift);
		break;
	}
}

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
bool flash_check_protected(uint8 blk, uint8 region) {
	uint8 shift;	//保护寄存器的偏移量
	uint8 bit;		//保护位的值

	//保护位的值默认为1，这样区域参数非法时会返回未保护
	bit = 1;
	//计算偏移量
	shift = region & 0x7;
	//根据所选块获取保护位的值
	switch (blk) {
	case FLASH_BLK_PFLASH:	//程序flash
		//根据区域选择相应寄存器读取保护位的值
		switch (region >> 3) {
		case 0:
			bit = REG_GET_SHIFT(FTFE_FPROT3, shift);
			break;
		case 1:
			bit = REG_GET_SHIFT(FTFE_FPROT2, shift);
			break;
		case 2:
			bit = REG_GET_SHIFT(FTFE_FPROT1, shift);
			break;
		case 3:
			bit = REG_GET_SHIFT(FTFE_FPROT0, shift);
			break;
		}
		break;
	case FLASH_BLK_DFLASH:	//数据flash
		bit = REG_GET_SHIFT(FTFE_FDPROT, shift);
		break;
	case FLASH_BLK_FLEXRAM:	//FlexRAM
		bit = REG_GET_SHIFT(FTFE_FEPROT, shift);
		break;
	}
	//若保护位的值为0，说明被保护
	return bit == 0 ? true : false;
}

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
		uint8 eeprom_split) {
	//设置命令为程序分区
	REG_SET_VAL(FTFE_FCCOB0, PGMPART);
	//设置EEPROM分区参数
	REG_SET_VAL(FTFE_FCCOB4, (eeprom_split << 4) + eeprom_size);
	//设置数据flash分区参数
	REG_SET_VAL(FTFE_FCCOB5, dflash_size);
	//执行命令，并返回结果
	return flash_launch();
}

//==========================================================================
//函数名称: flash_partition_get_info
//函数返回: true:获取信息成功; false:获取信息失败
//参数说明: dflash_size:存储数据flash分区大小的地址
//         eeprom_size:存储EEPROM分区大小的地址
//         eeprom_split:存储EEPROM子系统分割因子的地址
//功能概要: 获取当前的分区信息，成功时存储相应参数，参数值与宏定义值相同
//==========================================================================
bool flash_partition_get_info(uint8* dflash_size, uint8* eeprom_size,
		uint8* eeprom_split) {
	uint8 temp;	//临时变量

	//设置命令为读资源信息
	REG_SET_VAL(FTFE_FCCOB0, RDRSRC);
	//设置地址，为Data Flash 0 IFR最后8字节
	REG_SET_VAL(FTFE_FCCOB1, 0x80);
	REG_SET_VAL(FTFE_FCCOB2, 0x03);
	REG_SET_VAL(FTFE_FCCOB3, 0xF8);
	//设置资源选择码
	REG_SET_VAL(FTFE_FCCOB4, 0x00);
	//执行命令，并获取结果
	if (flash_launch() == FlashSuccess) {
		//执行命令成功时，读取资源成功
		//0x3FC，数据flash分区信息
		temp = FTFE_FCCOBB;
		*dflash_size = temp & 0xF;
		//0x3FD，EEPROM分区信息
		temp = FTFE_FCCOBA;
		*eeprom_size = temp & 0xF;
		*eeprom_split = (temp >> 4) & 0x3;
		return true;
	} else {
		//执行命令失败时，读取资源失败
		return false;
	}
}

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
FlashResult flash_flexram_set_mode(uint8 flexram_mode) {
	//设置命令为设定FlexRAM功能
	REG_SET_VAL(FTFE_FCCOB0, SETRAM);
	//设置FlexRAM的模式
	REG_SET_VAL(FTFE_FCCOB1, flexram_mode);
	//执行命令，并返回结果
	return flash_launch();
}

//==========================================================================
//函数名称: flash_flexram_get_mode
//函数返回: FlexRAM的模式:
//         FLASH_FLEXRAM_MODE_RAM:   RAM;
//         FLASH_FLEXRAM_MODE_EEPROM:EEPROM;
//参数说明: 无
//功能概要: 获取当前FlexRAM的模式，返回值与宏定义值相同
//==========================================================================
uint8 flash_flexram_get_mode() {
	uint8 fcnfg;	//flash配置寄存器的值
	for (;;) {
		//读取配置寄存器的值
		fcnfg = FTFE_FCNFG;
		//若EEPROM就绪，则返回EEPROM模式
		if (REG_GET_MASK(fcnfg, FTFE_FCNFG_EEERDY_MASK)) {
			return FLASH_FLEXRAM_MODE_EEPROM;
		}
		//若RAM就绪，则返回RAM模式
		if (REG_GET_MASK(fcnfg, FTFE_FCNFG_RAMRDY_MASK)) {
			return FLASH_FLEXRAM_MODE_RAM;
		}
		//否则继续等待配置完毕
	}
}

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
bool flash_flexram_write(uint16 offset, uint16 num, uint8* data) {
	uint8* ptr;	//8位指针
	uint8 i;	//临时变量

	//获取起始物理地址
	ptr = (uint8*) (FLASH_ADDR_FLEXRAM + offset);
	//获取FlexRAM模式
	if (flash_flexram_get_mode() == FLASH_FLEXRAM_MODE_EEPROM) {
		//EEPROM模式时，手动写入
		//地址没有4字节对齐时，每次写入1字节补至4字节对齐
		for (i = offset & 0x3; i > 0 && num > 0;) {
			//去除各错误标志
			REG_SET_VAL(FTFE_FSTAT, flash_error_masks);
			//写入1字节数据
			*ptr = *data;
			++ptr;
			++data;
			--num;
			i = (i + 1) & 0x3;
			//等待写入完成
			while (!REG_GET_MASK(FTFE_FCNFG, FTFE_FCNFG_EEERDY_MASK)) {
			}
			//若写入被保护区域，写入失败
			if (REG_GET_MASK(FTFE_FSTAT, FTFE_FSTAT_FPVIOL_MASK)) {
				return false;
			}
		}
		//地址4字节对齐后，每次写入4字节
		for (; num >= 4;) {
			//去除各错误标志
			REG_SET_VAL(FTFE_FSTAT, flash_error_masks);
			//写入4字节数据
			*(uint32*) ptr = *(uint32*) data;
			ptr += 4;
			data += 4;
			num -= 4;
			//等待写入完成
			while (!REG_GET_MASK(FTFE_FCNFG, FTFE_FCNFG_EEERDY_MASK)) {
			}
			//若写入被保护区域，写入失败
			if (REG_GET_MASK(FTFE_FSTAT, FTFE_FSTAT_FPVIOL_MASK)) {
				return false;
			}
		}
		//若存在剩余数据
		if (num) {
			//去除各错误标志
			REG_SET_VAL(FTFE_FSTAT, flash_error_masks);
			//处理剩余数据
			switch (num) {
			case 3:
				//剩余3字节，先写入2字节数据，再写入1字节数据
				*(uint16*) ptr = *(uint16*) data;
				ptr += 2;
				data += 2;
				//等待写入完成
				while (!REG_GET_MASK(FTFE_FCNFG, FTFE_FCNFG_EEERDY_MASK)) {
				}
				//若写入被保护区域，写入失败
				if (REG_GET_MASK(FTFE_FSTAT, FTFE_FSTAT_FPVIOL_MASK)) {
					return false;
				}
				//去除各错误标志
				REG_SET_VAL(FTFE_FSTAT, flash_error_masks);
				*ptr = *data;
				break;
			case 2:
				//剩余2字节，直接写入2字节数据
				*(uint16*) ptr = *(uint16*) data;
				break;
			default:
				//剩余1字节，直接写入1字节数据
				*ptr = *data;
				break;
			}
			//等待写入完成
			while (!REG_GET_MASK(FTFE_FCNFG, FTFE_FCNFG_EEERDY_MASK)) {
			}
			//若写入被保护区域，写入失败
			if (REG_GET_MASK(FTFE_FSTAT, FTFE_FSTAT_FPVIOL_MASK)) {
				return false;
			}
		}
	} else {
		//RAM模式时，直接调用memcpy
		memcpy(ptr, data, num);
	}
	//写入成功
	return true;
}

//==========================================================================
//函数名称: flash_flexram_read
//函数返回: 无
//参数说明: offset:偏移地址
//         num:读取的字节数
//         data:存储读取数据的首地址
//功能概要: 从FlexRAM中读取一定数量的数据，存储至RAM中
//==========================================================================
void flash_flexram_read(uint16 offset, uint16 num, uint8* data) {
	uint8* ptr;	//8位指针

	//获取起始物理地址
	ptr = (uint8*) (FLASH_ADDR_FLEXRAM + offset);
	//等待数据就绪
	while (!REG_GET_MASK(FTFE_FCNFG,
			FTFE_FCNFG_EEERDY_MASK|FTFE_FCNFG_RAMRDY_MASK)) {
	}
	//读取数据至内存
	memcpy(data, ptr, num);
}
