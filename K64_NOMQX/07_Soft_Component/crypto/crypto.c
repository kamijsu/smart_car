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
//函数名称: crypto_xor
//函数返回: 无
//参数说明: b1:异或操作数1的首地址
//         b2:异或操作数2的首地址
//         len:异或操作数的长度
//         dest:存储异或结果的地址
//功能概要: 对b1和b2进行异或，将结果存至dest
//==========================================================================
static void crypto_xor(uint8* b1, uint8* b2, uint32 len, uint8* dest) {
	uint32 i;
	for (i = 0; i < len; i++) {
		*(dest + i) = *(b1 + i) ^ *(b2 + i);
	}
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

//==========================================================================
//函数名称: crypto_des_encrypt
//函数返回: true:加密成功; false:加密失败
//参数说明: mode:工作模式:
//              CRYPTO_MODE_ECB:电码本;
//              CRYPTO_MODE_CBC:密码分组链接;
//              CRYPTO_MODE_CFB:密码反馈;
//              CRYPTO_MODE_OFB:输出反馈;
//         padding:填充算法:
//                 CRYPTO_PADDING_PKCS7:   PKCS7;
//                 CRYPTO_PADDING_ISO10126:ISO 10126;
//                 CRYPTO_PADDING_ANSIX923:ANSI X.923;
//                 CRYPTO_PADDING_NONE:    不进行填充;
//         plain:明文的首地址
//         plain_len:明文长度
//         key:8字节密钥的首地址
//         iv:8字节初始化向量的首地址，若工作模式为ECB，该参数无效
//         cipher:存储密文的首地址
//         cipher_len:存储密文长度的地址
//功能概要: 对明文使用DES算法进行加密，成功时存储相应的密文
//备注: 仅当填充算法为None且明文长度不为8的倍数时，加密失败;
//     若填充算法为ISO10126，需要先初始化RNG模块;
//     若填充算法为None，密文长度等于明文长度，
//     否则密文长度为明文长度补至8的倍数，若明文长度为8的倍数，则会额外补8个字节;
//     密文地址可以为明文地址
//==========================================================================
bool crypto_des_encrypt(uint8 mode, uint8 padding, uint8* plain,
		uint32 plain_len, uint8* key, uint8* iv, uint8* cipher,
		uint32* cipher_len) {
	uint32 blk_num;		//分组数
	uint8 *ptr8, *ptr8c;	//8位指针
	uint8 pad_num;		//要填充的字节数
	uint8 v[8];			//64位向量

	//若不进行填充
	if (padding == CRYPTO_PADDING_NONE) {
		//若明文长度不为8的倍数，加密失败
		if (plain_len & 0x7) {
			return false;
		}
		//计算分组数
		blk_num = plain_len >> 3;
	} else {
		//填充时，会额外添加一组
		blk_num = (plain_len >> 3) + 1;
	}
	//计算密文长度
	*cipher_len = blk_num << 3;
	//填充后的明文，C99标准允许不在函数起始处声明，并可以使用变量初始化数组
	uint8 pad_plain[*cipher_len];
	//复制明文
	memcpy(pad_plain, plain, plain_len);
	//指向填充位置
	ptr8 = pad_plain + plain_len;
	//计算填充字节数
	pad_num = *cipher_len - plain_len;
	//根据填充方式进行填充
	switch (padding) {
	case CRYPTO_PADDING_PKCS7:
		//填充n个n
		memset(ptr8, pad_num, pad_num);
		break;
	case CRYPTO_PADDING_ISO10126:
		//填充n-1个随机数
		rng_next_bytes(ptr8, pad_num - 1);
		//最后一个字节填充n
		pad_plain[*cipher_len - 1] = pad_num;
		break;
	case CRYPTO_PADDING_ANSIX923:
		//填充n-1个0
		memset(ptr8, 0, pad_num - 1);
		//最后一个字节填充n
		pad_plain[*cipher_len - 1] = pad_num;
		break;
	}
	//根据工作模式加密
	switch (mode) {
	case CRYPTO_MODE_ECB:	//ECB
		//对各分组加密
		for (ptr8 = pad_plain, ptr8c = cipher; blk_num > 0; --blk_num, ptr8 +=
				8, ptr8c += 8) {
			mmcau_des_encrypt(ptr8, key, ptr8c);
		}
		break;
	case CRYPTO_MODE_CBC:	//CBC
		//分组数不为0时，进行加密
		if (blk_num) {
			//对第一组加密
			crypto_xor(iv, pad_plain, 8, v);
			mmcau_des_encrypt(v, key, cipher);
			//加密剩余组
			for (--blk_num, ptr8 = pad_plain, ptr8c = cipher; blk_num > 0;
					--blk_num) {
				crypto_xor(ptr8c, ptr8 += 8, 8, v);
				mmcau_des_encrypt(v, key, ptr8c += 8);
			}
		}
		break;
	case CRYPTO_MODE_CFB:	//CFB
		//分组数不为0时，进行加密
		if (blk_num) {
			//对第一组加密
			mmcau_des_encrypt(iv, key, v);
			crypto_xor(v, pad_plain, 8, cipher);
			//加密剩余组
			for (--blk_num, ptr8 = pad_plain, ptr8c = cipher; blk_num > 0;
					--blk_num) {
				mmcau_des_encrypt(ptr8c, key, v);
				crypto_xor(v, ptr8 += 8, 8, ptr8c += 8);
			}
		}
		break;
	case CRYPTO_MODE_OFB:	//OFB
		//分组数不为0时，进行加密
		if (blk_num) {
			//对第一组加密
			mmcau_des_encrypt(iv, key, v);
			crypto_xor(v, pad_plain, 8, cipher);
			//加密剩余组
			for (--blk_num, ptr8 = pad_plain, ptr8c = cipher; blk_num > 0;
					--blk_num) {
				mmcau_des_encrypt(v, key, v);
				crypto_xor(v, ptr8 += 8, 8, ptr8c += 8);
			}
		}
		break;
	}
	return true;
}

//==========================================================================
//函数名称: crypto_des_decrypt
//函数返回: true:解密成功; false:解密失败
//参数说明: mode:工作模式:
//              CRYPTO_MODE_ECB:电码本;
//              CRYPTO_MODE_CBC:密码分组链接;
//              CRYPTO_MODE_CFB:密码反馈;
//              CRYPTO_MODE_OFB:输出反馈;
//         padding:填充算法:
//                 CRYPTO_PADDING_PKCS7:   PKCS7;
//                 CRYPTO_PADDING_ISO10126:ISO 10126;
//                 CRYPTO_PADDING_ANSIX923:ANSI X.923;
//                 CRYPTO_PADDING_NONE:    不进行填充;
//         cipher:密文的首地址
//         cipher_len:密文长度
//         key:8字节密钥的首地址
//         iv:8字节初始化向量的首地址，若工作模式为ECB，该参数无效
//         plain:存储明文的首地址
//         plain_len:存储明文长度的地址
//功能概要: 对密文使用DES算法进行解密，成功时存储相应的明文
//备注: 当密文长度不为8的倍数或解出的明文填充错误时，解密失败;
//     密文长度大于等于明文长度;
//     明文地址可以为密文地址
//==========================================================================
bool crypto_des_decrypt(uint8 mode, uint8 padding, uint8* cipher,
		uint32 cipher_len, uint8* key, uint8* iv, uint8* plain,
		uint32* plain_len) {
	uint8 pad_plain[cipher_len];	//填充后的明文
	uint32 blk_num;		//分组数
	uint8 *ptr8, *ptr8c;	//8位指针
	uint8 v[8];			//64位向量
	uint8 pad_num;		//填充的字节数

	//若密文长度不为8的倍数，解密失败
	if (cipher_len & 0x7) {
		return false;
	}
	blk_num = cipher_len >> 3;	//计算分组数
	//根据工作模式解密
	switch (mode) {
	case CRYPTO_MODE_ECB:	//ECB
		//对各分组解密
		for (ptr8c = cipher, ptr8 = pad_plain; blk_num > 0; --blk_num, ptr8c +=
				8, ptr8 += 8) {
			mmcau_des_decrypt(ptr8c, key, ptr8);
		}
		break;
	case CRYPTO_MODE_CBC:	//CBC
		//分组数不为0时，进行解密
		if (blk_num) {
			//对第一组解密
			mmcau_des_decrypt(cipher, key, v);
			crypto_xor(v, iv, 8, pad_plain);
			//解密剩余组
			for (--blk_num, ptr8 = pad_plain, ptr8c = cipher; blk_num > 0;
					--blk_num, ptr8c += 8) {
				mmcau_des_decrypt(ptr8c + 8, key, v);
				crypto_xor(v, ptr8c, 8, ptr8 += 8);
			}
		}
		break;
	case CRYPTO_MODE_CFB:	//CFB
		//分组数不为0时，进行解密
		if (blk_num) {
			//对第一组解密
			mmcau_des_encrypt(iv, key, v);
			crypto_xor(v, cipher, 8, pad_plain);
			//解密剩余组
			for (--blk_num, ptr8 = pad_plain, ptr8c = cipher; blk_num > 0;
					--blk_num) {
				mmcau_des_encrypt(ptr8c, key, v);
				crypto_xor(v, ptr8c += 8, 8, ptr8 += 8);
			}
		}
		break;
	case CRYPTO_MODE_OFB:	//OFB
		//分组数不为0时，进行解密
		if (blk_num) {
			//对第一组解密
			mmcau_des_encrypt(iv, key, v);
			crypto_xor(v, cipher, 8, pad_plain);
			//解密剩余组
			for (--blk_num, ptr8 = pad_plain, ptr8c = cipher; blk_num > 0;
					--blk_num) {
				mmcau_des_encrypt(v, key, v);
				crypto_xor(v, ptr8c += 8, 8, ptr8 += 8);
			}
		}
		break;
	}
	//密文长度不为0时
	if (cipher_len) {
		//若未进行填充
		if (padding == CRYPTO_PADDING_NONE) {
			//明文长度等于密文长度
			*plain_len = cipher_len;
		} else {
			blk_num = cipher_len - 1;	//blk_num当成临时变量
			pad_num = pad_plain[blk_num];	//获取填充字节数
			//检查填充数是否合法
			if (pad_num == 0 || pad_num > 8) {
				return false;
			}
			//根据填充方式检查填充是否合法
			switch (padding) {
			case CRYPTO_PADDING_PKCS7:
				//检查是否为n-1个n
				for (ptr8c = pad_plain + blk_num, ptr8 = ptr8c - pad_num + 1;
						ptr8 < ptr8c;) {
					if (*ptr8++ != pad_num) {
						return false;
					}
				}
				break;
			case CRYPTO_PADDING_ANSIX923:
				//检查是否为n-1个0
				for (ptr8c = pad_plain + blk_num, ptr8 = ptr8c - pad_num + 1;
						ptr8 < ptr8c;) {
					if (*ptr8++) {
						return false;
					}
				}
				break;
			}
			//去除填充字节数
			*plain_len = cipher_len - pad_num;
		}
		//拷贝明文
		memcpy(plain, pad_plain, *plain_len);
	} else {
		//明文长度为0
		*plain_len = 0;
	}
	return true;
}
