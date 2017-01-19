//==========================================================================
//文件名称：crypto.h
//功能概要：加解密软件程序头文件
//==========================================================================

#ifndef _CRYPTO_H
#define _CRYPTO_H

#include "common.h"
#include "mmcau.h"
#include "rng.h"
#include <string.h>

//定义DES和AES工作模式
#define CRYPTO_MODE_ECB		(0)		//电码本
#define CRYPTO_MODE_CBC		(1)		//密码分组链接
#define CRYPTO_MODE_CFB		(2)		//密码反馈
#define CRYPTO_MODE_OFB		(3)		//输出反馈

//定义DES和AES填充算法
#define CRYPTO_PADDING_PKCS7	(0)	//PKCS7，DES和AES中等价于PKCS5
#define CRYPTO_PADDING_ISO10126	(1)	//ISO 10126
#define CRYPTO_PADDING_ANSIX923	(2)	//ANSI X.923
#define CRYPTO_PADDING_NONE		(3)	//不进行填充

//==========================================================================
//函数名称: crypto_md5
//函数返回: 无
//参数说明: msg:消息的首地址
//         len:消息的长度
//         digest:存储生成摘要的首地址
//功能概要: 对一定长度的消息使用MD5散列函数，生成128bit的消息摘要(即16字节)
//==========================================================================
void crypto_md5(uint8* msg, uint32 len, uint8* digest);

//==========================================================================
//函数名称: crypto_md5_string
//函数返回: 无
//参数说明: msg:消息字符串的首地址
//         digest:存储生成摘要的首地址
//功能概要: 对消息字符串使用MD5散列函数，生成128bit的消息摘要(即16字节)
//==========================================================================
void crypto_md5_string(uint8* msg, uint8* digest);

//==========================================================================
//函数名称: crypto_sha1
//函数返回: 无
//参数说明: msg:消息的首地址
//         len:消息的长度
//         digest:存储生成摘要的首地址
//功能概要: 对一定长度的消息使用SHA1散列函数，生成160bit的消息摘要(即20字节)
//备注: 将生成的消息摘要转换成了小端格式
//==========================================================================
void crypto_sha1(uint8* msg, uint32 len, uint8* digest);

//==========================================================================
//函数名称: crypto_sha1_string
//函数返回: 无
//参数说明: msg:消息字符串的首地址
//         digest:存储生成摘要的首地址
//功能概要: 对消息字符串使用SHA1散列函数，生成160bit的消息摘要(即20字节)
//备注: 将生成的消息摘要转换成了小端格式
//==========================================================================
void crypto_sha1_string(uint8* msg, uint8* digest);

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
		uint32* cipher_len);

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
//功能概要: 对密文使用DES算法进行解密，存储相应的明文
//备注: 当密文长度不为8的倍数或解出的明文填充错误时，解密失败;
//     密文长度大于等于明文长度;
//     仅当工作模式为ECB和OFB时，明文地址可以为密文地址;
//     密文长度合法时，无论填充是否正确，解出的明文均会被保存
//==========================================================================
bool crypto_des_decrypt(uint8 mode, uint8 padding, uint8* cipher,
		uint32 cipher_len, uint8* key, uint8* iv, uint8* plain,
		uint32* plain_len);

//==========================================================================
//函数名称: crypto_aes_encrypt
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
//         key:密钥的首地址
//         key_size:密钥的位数:
//                  128 | 192 | 256
//         iv:16字节初始化向量的首地址，若工作模式为ECB，该参数无效
//         cipher:存储密文的首地址
//         cipher_len:存储密文长度的地址
//功能概要: 对明文使用AES算法进行加密，成功时存储相应的密文
//备注: 仅当填充算法为None且明文长度不为16的倍数时，加密失败;
//     若填充算法为ISO10126，需要先初始化RNG模块;
//     若填充算法为None，密文长度等于明文长度，
//     否则密文长度为明文长度补至16的倍数，若明文长度为16的倍数，则会额外补16个字节;
//     密文地址可以为明文地址
//==========================================================================
bool crypto_aes_encrypt(uint8 mode, uint8 padding, uint8* plain,
		uint32 plain_len, uint8* key, uint16 key_size, uint8* iv, uint8* cipher,
		uint32* cipher_len);

//==========================================================================
//函数名称: crypto_aes_decrypt
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
//         key:密钥的首地址
//         key_size:密钥的位数:
//                  128 | 192 | 256
//         iv:16字节初始化向量的首地址，若工作模式为ECB，该参数无效
//         plain:存储明文的首地址
//         plain_len:存储明文长度的地址
//功能概要: 对密文使用AES算法进行解密，成功时存储相应的明文
//备注: 当密文长度不为16的倍数或解出的明文填充错误时，解密失败;
//     密文长度大于等于明文长度;
//     仅当工作模式为ECB和OFB时，明文地址可以为密文地址;
//     密文长度合法时，无论填充是否正确，解出的明文均会被保存
//==========================================================================
bool crypto_aes_decrypt(uint8 mode, uint8 padding, uint8* cipher,
		uint32 cipher_len, uint8* key, uint16 key_size, uint8* iv, uint8* plain,
		uint32* plain_len);

#endif
