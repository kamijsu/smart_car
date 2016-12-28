//==========================================================================
//文件名称：crc.h
//功能概要：K64 CRC底层驱动程序头文件
//==========================================================================

#ifndef _CRC_H
#define _CRC_H

//包含公共头文件
#include "common.h"

//定义CRC协议宽度
#define CRC_WIDTH_16	(0)	//16位CRC协议
#define CRC_WIDTH_32	(1)	//32位CRC协议

//定义CRC转置类型
#define CRC_TRANS_NO	(0)	//不转置，[31:0]变成[31:0]
#define CRC_TRANS_BITS	(1)	//仅按位转置，[31:0]变成[24:31]，[16:23]，[8:15]，[0:7]
#define CRC_TRANS_BOTH	(2)	//按位和字节转置，[31:0]变成[0:31]
#define CRC_TRANS_BYTES	(3)	//仅按字节转置，[31:0]变成[7:0]，[15:8]，[23:16]，[31:24]

//定义预设的CRC协议，各协议注释依次为种子数、生成多项式、写数据转置类型、读数据转置类型、是否补读
#define CRC_CRC16_IBM			(0)	//0x0000		0x8005		BITS	BOTH	false
#define CRC_CRC16_MAXIM			(1)	//0x0000		0x8005		BITS	BOTH	true
#define CRC_CRC16_USB			(2)	//0xFFFF		0x8005		BITS	BOTH	true
#define CRC_CRC16_MODBUS		(3)	//0xFFFF		0x8005		BITS	BOTH	false
#define CRC_CRC16_CCITT			(4)	//0x0000		0x1021		BITS	BOTH	false
#define CRC_CRC16_CCITT_FALSE	(5)	//0xFFFF		0x1021		NO		NO		false
#define CRC_CRC16_X25			(6)	//0xFFFF		0x1021		BITS	BOTH	true
#define CRC_CRC16_XMODEM		(7)	//0x0000		0x1021		NO		NO		false
#define CRC_CRC16_DNP			(8)	//0x0000		0x3D65		BITS	BOTH	true
#define CRC_CRC32				(9)	//0xFFFFFFFF	0x04C11DB7	BITS	BOTH	true
#define CRC_CRC32_MPEG2			(10)//0xFFFFFFFF	0x04C11DB7	NO		NO		false

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
		uint8 read_trans, bool complement_read);

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
void crc_init_protocol(uint8 protocol_type);

//==========================================================================
//函数名称: crc_cal_block
//函数返回: CRC校验码，16位协议时仅低16位有效
//参数说明: data:校验数据的首地址
//         len:校验数据的字节数
//功能概要: 校验一定长度的数据，返回生成的CRC校验码
//==========================================================================
uint32 crc_cal_block(uint8* data, uint32 len);

#endif
