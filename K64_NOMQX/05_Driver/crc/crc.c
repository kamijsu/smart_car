//==========================================================================
//文件名称：crc.c
//功能概要：K64 CRC底层驱动程序源文件
//==========================================================================

#include "crc.h"

//当前的种子数
static uint32 current_seed;
//用于设置种子数的控制寄存器值
static uint32 ctrl_seed;
//用于校验数据时的控制寄存器值
static uint32 ctrl_data;

//==========================================================================
//函数名称: crc_init
//函数返回: 无
//参数说明: width:CRC协议宽度:
//               CRC_WIDTH_16:16位CRC协议;
//               CRC_WIDTH_32:32位CRC协议;
//         seed:种子数，即初始CRC值
//         poly:生成多项式
//         write_trans:写数据时的转置类型:
//                     CRC_TRANS_NO:   不转置;
//                     CRC_TRANS_BITS: 仅按位转置;
//                     CRC_TRANS_BOTH: 按位和字节转置;
//                     CRC_TRANS_BYTES:仅按字节转置;
//         read_trans:读数据时的转置类型，可选参数同write_trans
//         complement_read:是否进行补读
//功能概要: 初始化CRC模块
//备注: 补读即将校验结果与0xFFFFFFFF或0xFFFF进行异或运算
//==========================================================================
void crc_init(uint8 width, uint32 seed, uint32 poly, uint8 write_trans,
		uint8 read_trans, bool complement_read) {
	//开CRC模块时钟门
	REG_SET_MASK(SIM_SCGC6, SIM_SCGC6_CRC_MASK);
	//设置ctrl_seed，WAS置1
	REG_SET_VAL(ctrl_seed, CRC_CTRL_WAS_MASK);
	if (width == CRC_WIDTH_32) {
		//若协议宽度为32位，TCRC置1
		REG_SET_MASK(ctrl_seed, CRC_CTRL_TCRC_MASK);
		//设置ctrl_data，根据协议宽度设定TCRC
		REG_SET_VAL(ctrl_data, CRC_CTRL_TCRC_MASK);
	} else {
		REG_SET_VAL(ctrl_data, 0);
	}
	//设置ctrl_seed写转置类型
	REG_SET_MASK(ctrl_seed, CRC_CTRL_TOT(write_trans));
	//设置ctrl_data读写转置类型，因为用32位校验，所以写转置取反
	REG_SET_MASK(ctrl_data,
			CRC_CTRL_TOT(~write_trans) | CRC_CTRL_TOTR(read_trans));
	//设置ctrl_data补读
	if (complement_read) {
		REG_SET_MASK(ctrl_data, CRC_CTRL_FXOR_MASK);
	}
	//设置控制寄存器为ctrl_seed
	REG_SET_VAL(CRC_CTRL, ctrl_seed);
	//设置生成多项式
	REG_SET_VAL(CRC_GPOLY, poly);
	//保存种子数
	current_seed = seed;
}

//==========================================================================
//函数名称: crc_init_protocol
//函数返回: 无
//参数说明: protocol_type:CRC协议类型:
//                       CRC_CRC16_IBM;
//                       CRC_CRC16_MAXIM;
//                       CRC_CRC16_USB;
//                       CRC_CRC16_MODBUS;
//                       CRC_CRC16_CCITT;
//                       CRC_CRC16_CCITT_FALSE;
//                       CRC_CRC16_X25;
//                       CRC_CRC16_XMODEM;
//                       CRC_CRC16_DNP;
//                       CRC_CRC32;
//                       CRC_CRC32_MPEG2;
//功能概要: 以预设的协议初始化CRC模块
//备注: 各协议参数见宏定义
//==========================================================================
void crc_init_protocol(uint8 protocol_type) {
	//根据协议类型初始化CRC模块
	switch (protocol_type) {
	case CRC_CRC16_IBM:
		crc_init(CRC_WIDTH_16, 0x0000, 0x8005, CRC_TRANS_BITS, CRC_TRANS_BOTH,
		false);
		break;
	case CRC_CRC16_MAXIM:
		crc_init(CRC_WIDTH_16, 0x0000, 0x8005, CRC_TRANS_BITS, CRC_TRANS_BOTH,
		true);
		break;
	case CRC_CRC16_USB:
		crc_init(CRC_WIDTH_16, 0xFFFF, 0x8005, CRC_TRANS_BITS, CRC_TRANS_BOTH,
		true);
		break;
	case CRC_CRC16_MODBUS:
		crc_init(CRC_WIDTH_16, 0xFFFF, 0x8005, CRC_TRANS_BITS, CRC_TRANS_BOTH,
		false);
		break;
	case CRC_CRC16_CCITT:
		crc_init(CRC_WIDTH_16, 0x0000, 0x1021, CRC_TRANS_BITS, CRC_TRANS_BOTH,
		false);
		break;
	case CRC_CRC16_CCITT_FALSE:
		crc_init(CRC_WIDTH_16, 0xFFFF, 0x1021, CRC_TRANS_NO, CRC_TRANS_NO,
		false);
		break;
	case CRC_CRC16_X25:
		crc_init(CRC_WIDTH_16, 0xFFFF, 0x1021, CRC_TRANS_BITS, CRC_TRANS_BOTH,
		true);
		break;
	case CRC_CRC16_XMODEM:
		crc_init(CRC_WIDTH_16, 0x0000, 0x1021, CRC_TRANS_NO, CRC_TRANS_NO,
		false);
		break;
	case CRC_CRC16_DNP:
		crc_init(CRC_WIDTH_16, 0x0000, 0x3D65, CRC_TRANS_BITS, CRC_TRANS_BOTH,
		true);
		break;
	case CRC_CRC32:
		crc_init(CRC_WIDTH_32, 0xFFFFFFFF, 0x04C11DB7, CRC_TRANS_BITS,
		CRC_TRANS_BOTH, true);
		break;
	case CRC_CRC32_MPEG2:
		crc_init(CRC_WIDTH_32, 0xFFFFFFFF, 0x04C11DB7, CRC_TRANS_NO,
		CRC_TRANS_NO, false);
		break;
	}
}

//==========================================================================
//函数名称: crc_cal
//函数返回: CRC校验码，16位协议时仅低16位有效
//参数说明: data:校验数据的首地址
//         len:校验数据的字节数
//功能概要: 校验一定长度的数据，返回生成的CRC校验码
//备注: 校验过程是中断安全的;
//     硬件CRC仅适合块式校验数据，流式校验数据时效率低下，建议用软件CRC进行流式校验
//==========================================================================
uint32 crc_cal(uint8* data, uint32 len) {
	//用于读取和写入数据寄存器的控制寄存器值，即WAS和TCRC置1
	static const uint32 ctrl_backup = CRC_CTRL_WAS_MASK | CRC_CTRL_TCRC_MASK;
	uint32 ctrl_old;	//旧控制寄存器值
	uint32 data_old;	//旧数据寄存器值
	uint32* data32;		//32位数据指针，用于32位校验数据
	uint32 result;		//CRC校验结果

	//读取旧控制寄存器值
	ctrl_old = CRC_CTRL;
	//设置控制寄存器为ctrl_backup
	REG_SET_VAL(CRC_CTRL, ctrl_backup);
	//读取旧数据寄存器值
	data_old = CRC_DATA;
	//设置控制寄存器为ctrl_seed
	REG_SET_VAL(CRC_CTRL, ctrl_seed);
	//设置种子数
	REG_SET_VAL(CRC_DATA, current_seed);
	//设置控制寄存器为ctrl_data
	REG_SET_VAL(CRC_CTRL, ctrl_data);
	//将数据指针转化为32位
	data32 = (uint32*) data;
	//32位校验数据
	for (; len >= 4; len -= 4) {
		REG_SET_VAL(CRC_DATA, *(data32++));
	}
	//将数据指针转化为8位
	data = (uint8*) data32;
	//校验剩余数据
	switch (len) {
	case 3:	//剩余3字节，先按16位校验数据，再按8位校验数据
		REG_SET_VAL(CRC_DATAL, *(uint16* )data);
		REG_SET_VAL(CRC_DATALL, *(data + 2));
		break;
	case 2:	//剩余2字节，按16位校验数据
		REG_SET_VAL(CRC_DATAL, *(uint16* )data);
		break;
	case 1:	//剩余1字节，按8位校验数据
		REG_SET_VAL(CRC_DATALL, *data);
		break;
	}
	//获取校验结果
	if (REG_GET_MASK(ctrl_data, CRC_CTRL_TCRC_MASK)) {
		//32位协议时，校验码为数据寄存器值
		result = CRC_DATA;
	} else {
		//16位协议时，根据是否进行过字节读置换而选择读取数据寄存器高/低半字
		if (REG_GET_SHIFT(ctrl_data, (CRC_CTRL_TOTR_SHIFT + 1))) {
			//进行过字节读置换时，读取高半字
			result = CRC_DATAH;
		} else {
			//没有进行过字节读置换时，读取低半字
			result = CRC_DATAL;
		}
	}
	//设置控制寄存器为ctrl_backup
	REG_SET_VAL(CRC_CTRL, ctrl_backup);
	//恢复数据寄存器旧值
	REG_SET_VAL(CRC_DATA, data_old);
	//恢复控制寄存器旧值
	REG_SET_VAL(CRC_CTRL, ctrl_old);
	//返回校验结果
	return result;
}
