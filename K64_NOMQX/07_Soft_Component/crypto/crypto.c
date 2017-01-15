//==========================================================================
//文件名称：crypto.c
//功能概要：加解密软件程序源文件
//==========================================================================

#include "crypto.h"

//==========================================================================
//函数名称: crypto_get_blk_num
//函数返回: 填充后的分组数
//参数说明: len:消息的长度
//功能概要: 获取填充后的分组数，每组64字节
//==========================================================================
static uint32 crypto_get_blk_num(uint32 len) {
	//每64字节一组，若剩余字节数大于等于56，则额外增加一组
	return (len & 0x3F) >= 56 ? (len >> 6) + 2 : (len >> 6) + 1;
}

//==========================================================================
//函数名称: crypto_pad_msg
//函数返回: 无
//参数说明: msg:消息的首地址
//         len:消息的长度
//         pad_msg:填充消息的首地址
//         pad_len:填充消息的长度
//         lsb:长度是否为小端
//功能概要: 填充消息
//==========================================================================
static void crypto_pad_msg(uint8* msg, uint32 len, uint8* pad_msg,
		uint32 pad_len, bool lsb) {
	uint32 pad0_index;	//填充0的位置
	uint32 len_index;	//填充长度的位置
	uint8* len_ptr;		//指向长度的指针
	uint64 bit_num;		//消息的位数

	pad0_index = len + 1;	//填充0x80后开始填充0
	len_index = pad_len - 8;	//最后8个字节填充长度
	len_ptr = pad_msg + len_index;	//获取长度首地址
	bit_num = len << 3;		//计算消息位数

	memcpy(pad_msg, msg, len);	//复制消息
	pad_msg[len] = 0x80;	//填充0x80
	memset(pad_msg + pad0_index, 0, len_index - pad0_index);	//填充0
	//填充长度，长度用位数表示
	if (lsb) {
		//小端
		*(uint64*) len_ptr = bit_num;
	} else {
		//大端
		len_ptr[0] = bit_num >> 56;
		len_ptr[1] = bit_num >> 48;
		len_ptr[2] = bit_num >> 40;
		len_ptr[3] = bit_num >> 32;
		len_ptr[4] = bit_num >> 24;
		len_ptr[5] = bit_num >> 16;
		len_ptr[6] = bit_num >> 8;
		len_ptr[7] = bit_num;
	}
}

//==========================================================================
//函数名称: crypto_msb_to_lsb
//函数返回: 无
//参数说明: lsb:大端字的首地址
//功能概要: 将大端字转换为小端字
//==========================================================================
static void crypto_msb_to_lsb(unsigned int* msb) {
	uint8 temp;	//临时数据
	uint8* ptr;	//8位指针

	ptr = (uint8*) msb;	//转换为8位指针
	//交换首尾字节
	temp = ptr[0];
	ptr[0] = ptr[3];
	ptr[3] = temp;
	//交换中间字节
	temp = ptr[1];
	ptr[1] = ptr[2];
	ptr[2] = temp;
}

//==========================================================================
//函数名称: crypto_md5
//函数返回: 无
//参数说明: msg:消息的首地址
//         len:消息的长度
//         digest:存储生成摘要的首地址
//功能概要: 对一定长度的消息使用MD5散列函数，生成128bit的消息摘要(即16字节)
//==========================================================================
void crypto_md5(uint8* msg, uint32 len, uint8* digest) {
	uint32 blk_num;	//消息分组数
	uint32 pad_len;	//填充后的总长度

	blk_num = crypto_get_blk_num(len);	//获取分组数
	pad_len = blk_num << 6;	//计算填充后总长度
	//填充后的消息，C99标准允许不在函数起始处声明，并可以使用变量初始化数组
	uint8 pad_msg[pad_len];

	//填充消息
	crypto_pad_msg(msg, len, pad_msg, pad_len, true);
	//初始化链接变量
	mmcau_md5_initialize_output(digest);
	//对各分组进行轮循环
	mmcau_md5_hash_n(pad_msg, blk_num, digest);
}

//==========================================================================
//函数名称: crypto_md5_string
//函数返回: 无
//参数说明: msg:消息字符串的首地址
//         digest:存储生成摘要的首地址
//功能概要: 对消息字符串使用MD5散列函数，生成128bit的消息摘要(即16字节)
//==========================================================================
void crypto_md5_string(uint8* msg, uint8* digest) {
	crypto_md5(msg, strlen(msg), digest);
}

//==========================================================================
//函数名称: crypto_sha1
//函数返回: 无
//参数说明: msg:消息的首地址
//         len:消息的长度
//         digest:存储生成摘要的首地址
//功能概要: 对一定长度的消息使用SHA1散列函数，生成160bit的消息摘要(即20字节)
//备注: 将生成的消息摘要转换成了小端格式
//==========================================================================
void crypto_sha1(uint8* msg, uint32 len, uint8* digest) {
	uint32 blk_num;	//消息分组数
	uint32 pad_len;	//填充后的总长度
	unsigned int* ptr32;	//32位的摘要指针
	uint8 i;		//游标

	blk_num = crypto_get_blk_num(len);	//获取分组数
	pad_len = blk_num << 6;	//计算填充后总长度
	ptr32 = (unsigned int*) digest;		//将摘要指针转化为32位
	//填充后的消息，C99标准允许不在函数起始处声明，并可以使用变量初始化数组
	uint8 pad_msg[pad_len];

	//填充消息
	crypto_pad_msg(msg, len, pad_msg, pad_len, false);
	//初始化消息摘要缓存
	mmcau_sha1_initialize_output(ptr32);
	//对各分组进行压缩函数
	mmcau_sha1_hash_n(pad_msg, blk_num, ptr32);
	//转化为小端格式
	for (i = 0; i < 5; i++) {
		crypto_msb_to_lsb(ptr32 + i);
	}
}

//==========================================================================
//函数名称: crypto_sha1_string
//函数返回: 无
//参数说明: msg:消息字符串的首地址
//         digest:存储生成摘要的首地址
//功能概要: 对消息字符串使用SHA1散列函数，生成160bit的消息摘要(即20字节)
//备注: 将生成的消息摘要转换成了小端格式
//==========================================================================
void crypto_sha1_string(uint8* msg, uint8* digest) {
	crypto_sha1(msg, strlen(msg), digest);
}
