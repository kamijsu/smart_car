//==========================================================================
//文件名称：flash.c
//功能概要：K64 Flash底层驱动程序源文件
//==========================================================================

#include "flash.h"
#include "uart.h"

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
	//各错误标志
	static const uint8 masks = FTFE_FSTAT_RDCOLERR_MASK | FTFE_FSTAT_ACCERR_MASK
			| FTFE_FSTAT_FPVIOL_MASK;
	uint8 temp;

	//去除各错误标志
	REG_SET_VAL(FTFE_FSTAT, masks);
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

static void flash_set_data(uint8 offset, uint8 num, volatile uint8* data) {
	//数据FCCOB寄存器基地址
	static uint8* const data_ptr = (uint8*) (FTFE_BASE + 0x8);
	volatile uint8* volatile ptr;
	for (ptr = data_ptr + offset; num > 0; --num) {
		*ptr++ = *data++;
	}
}

void show() {
	uart_send1(UART_MOD1, FTFE_FCCOB0);
	uart_send1(UART_MOD1, FTFE_FCCOB1);
	uart_send1(UART_MOD1, FTFE_FCCOB2);
	uart_send1(UART_MOD1, FTFE_FCCOB3);
	uart_send1(UART_MOD1, FTFE_FCCOB4);
	uart_send1(UART_MOD1, FTFE_FCCOB5);
	uart_send1(UART_MOD1, FTFE_FCCOB6);
	uart_send1(UART_MOD1, FTFE_FCCOB7);
	uart_send1(UART_MOD1, FTFE_FCCOB8);
	uart_send1(UART_MOD1, FTFE_FCCOB9);
	uart_send1(UART_MOD1, FTFE_FCCOBA);
	uart_send1(UART_MOD1, FTFE_FCCOBB);
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
//                数据flash时，取值范围为[0,31];
//         offset:扇区内偏移地址，必须为8的倍数，建议小于4096
//         num:写入的字节数，必须为8的倍数，建议小于等于4096
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
////	//数据FCCOB寄存器基地址
////	static uint8* const data_ptr = (uint8*) (FTFE_BASE + 0x8);
//	uint8 bn;		//字节数
//	uint32 addr;	//地址FCCOB寄存器设置的地址值
//	uint32 i;		//临时变量
//	uint16 j;		//临时变量
//	FlashResult result;	//写入结果
//
//	//偏移地址未满8字节部分
//	bn = offset & 0x7;
//	//flash块内偏移地址
//	i = (sector << 12) + offset - bn;
//	//第23位决定选择程序flash还是数据flash
//	addr = (flash_addr_table[blk] >> 5) + i;
//	//设置命令为写入8个字节
//	REG_SET_VAL(FTFE_FCCOB0, PGM8);
//	//处理偏移地址未满8字节情况
//	if (bn) {
//		//设置地址
//		flash_set_addr(addr);
//		//数据物理地址
//		i += flash_addr_table[blk];
//		//用原先数据填充数据FCCOB寄存器
//		flash_set_data(0, bn, (uint8*) i);
////		memcpy(data_ptr, (void*) i, bn);
//		//填充数量与写入总数之和
//		j = bn + num;
//		//判断能否满8字节
//		if (j >= 8) {
//			//写入数据数量
//			j = 8 - bn;
//			//能满时，设置剩余数据
//			flash_set_data(bn, j, data);
////			memcpy((void*) (data_ptr + bn), data, j);
//		} else {
//			//不能满时，设置数据后用原先数据填充FCCOB寄存器
//			flash_set_data(bn, num, data);
//			flash_set_data(j, 8 - j, (uint8*) (i + j));
////			memcpy((void*) (data_ptr + bn), data, num);
////			memcpy((void*) (data_ptr + j), (void*) (i + j), 8 - j);
//			//写入数据数量
//			j = num;
//		}
////		show();
//		//执行命令，并获取结果
//		result = flash_launch();
//		//写入失败，或写入完毕时，返回结果
//		if (result != FlashSuccess || (num -= j) == 0) {
//			return result;
//		}
//		//设置的地址值向后偏移8字节
//		addr += 8;
//		//写入数据首地址向后偏移
//		data += j;
//	}
//	//剩余写入数量未满8字节部分
//	bn = num & 0x7;
//	//写入满8字节部分
//	for (num -= bn; num > 0; num -= 8, data += 8, addr += 8) {
//		//设置地址
//		flash_set_addr(addr);
//		//设置数据
////		memcpy(data_ptr, data, 8);
//		REG_SET_VAL(FTFE_FCCOB7, data[0]);
//		REG_SET_VAL(FTFE_FCCOB6, data[1]);
//		REG_SET_VAL(FTFE_FCCOB5, data[2]);
//		REG_SET_VAL(FTFE_FCCOB4, data[3]);
//		REG_SET_VAL(FTFE_FCCOBB, data[4]);
//		REG_SET_VAL(FTFE_FCCOBA, data[5]);
//		REG_SET_VAL(FTFE_FCCOB9, data[6]);
//		REG_SET_VAL(FTFE_FCCOB8, data[7]);
//		//执行命令，并获取结果
//		result = flash_launch();
//		//写入失败时，返回失败原因
//		if (result != FlashSuccess) {
//			return result;
//		}
//	}
//	//处理剩余写入数量未满8字节情况
//	if (bn) {
//		//设置地址
//		flash_set_addr(addr);
//		//将地址第23位移至28位以获得物理地址
//		if (BGET(addr, 23)) {
//			BCLR(addr, 23);
//			BSET(addr, 28);
//		}
//		//设置剩余数据后用原先数据填充FCCOB寄存器
//		flash_set_data(0, bn, data);
//		flash_set_data(bn, 8 - bn, (uint8*) (addr + bn));
//		show();
//		uart_send1(1,addr>>24);
//		uart_send1(1,addr>>16);
//		uart_send1(1,addr>>8);
//		uart_send1(1,addr);
////		memcpy(data_ptr, data, bn);
////		memcpy((void*) (data_ptr + bn), (void*) (addr + bn), 8 - bn);
//		//执行命令，并获取结果
//		result = flash_launch();
//	}
//	//返回写入结果
//	return result;
}

//==========================================================================
//函数名称: flash_read
//函数返回: 无
//参数说明: blk:flash块:
//             FLASH_BLK_PFLASH:程序flash;
//             FLASH_BLK_DFLASH:数据flash;
//         sector:扇区号:
//                程序flash时，取值范围为[0,127];
//                数据flash时，取值范围为[0,31];
//         offset:扇区内偏移地址，建议小于4096
//         num:读取的字节数，建议小于等于4096
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
////	memcpy(data,(void*)(flash_addr_table[blk] + (sector << 12) + offset),num);
//	volatile uint32* volatile ptrr;	//不优化的32位指针
//	uint32* ptrw;	//32位指针
//
//	//将逻辑地址转换为物理地址
//	ptrr = (uint32*) (flash_addr_table[blk] + (sector << 12) + offset);
//	//每4字节读取
//	for (ptrw = (uint32*) data; num >= 4; num -= 4) {
//		*ptrw++ = *ptrr++;
//	}
//	//读取剩余字节
//	switch (num) {
//	case 3:
//		*(uint16*) ptrw = *(volatile uint16*) ptrr;
//		*((uint8*) ptrw + 2) = *((volatile uint8*) ptrr + 2);
//		break;
//	case 2:
//		*(uint16*) ptrw = *(volatile uint16*) ptrr;
//		break;
//	case 1:
//		*(uint8*) ptrw = *(volatile uint8*) ptrr;
//		break;
//	}
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
//                数据flash时，取值范围为[0,31];
//功能概要: 擦除一块扇区
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
//备注: 若数据flash分配空间给EEPROM备份，擦除时，返回非法参数
//==========================================================================
FlashResult flash_erase_dflash() {
	//设置命令为擦除一块flash
	REG_SET_VAL(FTFE_FCCOB0, ERSBLK);
	//设置地址
	flash_set_addr(flash_addr_table[FLASH_BLK_DFLASH] >> 5);
	//执行命令，并返回结果
	return flash_launch();
}
