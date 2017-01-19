//==========================================================================
//文件名称：flash.c
//功能概要：K64 Flash底层驱动程序源文件
//==========================================================================

#include "flash.h"

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
//函数返回: 完成后状态寄存器的值
//参数说明: 无
//功能概要: 执行flash命令
//==========================================================================
static uint32 flash_launch() {
	uint32 temp;

	//去除各错误标志
	temp = FTFE_FSTAT_RDCOLERR_MASK | FTFE_FSTAT_ACCERR_MASK
			| FTFE_FSTAT_FPVIOL_MASK;
	REG_SET_VAL(FTFE_FSTAT, temp);
	//启动命令
	REG_SET_VAL(FTFE_FSTAT, FTFE_FSTAT_CCIF_MASK);
	//等待命令完成
	do {
		temp = FTFE_FSTAT;
	} while (!REG_GET_MASK(temp, FTFE_FSTAT_CCIF_MASK));
	//返回状态寄存器的值
	return temp;
}

//==========================================================================
//函数名称: flash_logical_to_physical
//函数返回: 物理地址
//参数说明: blk:flash块:
//             FLASH_BLK_PFLASH:程序flash;
//             FLASH_BLK_DFLASH:数据flash;
//         sector:扇区号:
//                程序flash时，取值范围为[0,127];
//                数据flash时，取值范围为[0,31];
//         offset:扇区偏移地址
//功能概要: 将逻辑地址转换为物理地址
//==========================================================================
static uint32 flash_logical_to_physical(uint8 blk, uint8 sector, uint16 offset) {
	return flash_addr_table[blk] + (sector << 12) + offset;
}

uint16 flash_write(uint8 blk, uint8 sector, uint16 offset, uint16 num,
		uint8* data) {
	uint32 addr;	//物理地址
	uint16 i,j;		//临时变量
	uint32 result;	//写入结果

	//获取偏移地址未满8字节部分
	i = offset&0x7;
	//获取起始物理地址
	addr = flash_logical_to_physical(blk, sector, offset - i);
	//设置命令为写入8个字节
	REG_SET_VAL(FTFE_FCCOB0, PGM8);
	//按顺序
	for (i = 0; i < num; i += 8) {

	}
}
