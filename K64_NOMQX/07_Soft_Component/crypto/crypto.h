//==========================================================================
//�ļ����ƣ�crypto.h
//���ܸ�Ҫ���ӽ����������ͷ�ļ�
//==========================================================================

#ifndef _CRYPTO_H
#define _CRYPTO_H

#include "common.h"
#include "mmcau.h"
#include "rng.h"
#include <string.h>

//����DES��AES����ģʽ
#define CRYPTO_MODE_ECB		(0)		//���뱾
#define CRYPTO_MODE_CBC		(1)		//�����������
#define CRYPTO_MODE_CFB		(2)		//���뷴��
#define CRYPTO_MODE_OFB		(3)		//�������

//����DES��AES����㷨
#define CRYPTO_PADDING_PKCS7	(0)	//PKCS7��DES��AES�еȼ���PKCS5
#define CRYPTO_PADDING_ISO10126	(1)	//ISO 10126
#define CRYPTO_PADDING_ANSIX923	(2)	//ANSI X.923
#define CRYPTO_PADDING_NONE		(3)	//���������

//==========================================================================
//��������: crypto_md5
//��������: ��
//����˵��: msg:��Ϣ���׵�ַ
//         len:��Ϣ�ĳ���
//         digest:�洢����ժҪ���׵�ַ
//���ܸ�Ҫ: ��һ�����ȵ���Ϣʹ��MD5ɢ�к���������128bit����ϢժҪ(��16�ֽ�)
//==========================================================================
void crypto_md5(uint8* msg, uint32 len, uint8* digest);

//==========================================================================
//��������: crypto_md5_string
//��������: ��
//����˵��: msg:��Ϣ�ַ������׵�ַ
//         digest:�洢����ժҪ���׵�ַ
//���ܸ�Ҫ: ����Ϣ�ַ���ʹ��MD5ɢ�к���������128bit����ϢժҪ(��16�ֽ�)
//==========================================================================
void crypto_md5_string(uint8* msg, uint8* digest);

//==========================================================================
//��������: crypto_sha1
//��������: ��
//����˵��: msg:��Ϣ���׵�ַ
//         len:��Ϣ�ĳ���
//         digest:�洢����ժҪ���׵�ַ
//���ܸ�Ҫ: ��һ�����ȵ���Ϣʹ��SHA1ɢ�к���������160bit����ϢժҪ(��20�ֽ�)
//��ע: �����ɵ���ϢժҪת������С�˸�ʽ
//==========================================================================
void crypto_sha1(uint8* msg, uint32 len, uint8* digest);

//==========================================================================
//��������: crypto_sha1_string
//��������: ��
//����˵��: msg:��Ϣ�ַ������׵�ַ
//         digest:�洢����ժҪ���׵�ַ
//���ܸ�Ҫ: ����Ϣ�ַ���ʹ��SHA1ɢ�к���������160bit����ϢժҪ(��20�ֽ�)
//��ע: �����ɵ���ϢժҪת������С�˸�ʽ
//==========================================================================
void crypto_sha1_string(uint8* msg, uint8* digest);

//==========================================================================
//��������: crypto_des_encrypt
//��������: true:���ܳɹ�; false:����ʧ��
//����˵��: mode:����ģʽ:
//              CRYPTO_MODE_ECB:���뱾;
//              CRYPTO_MODE_CBC:�����������;
//              CRYPTO_MODE_CFB:���뷴��;
//              CRYPTO_MODE_OFB:�������;
//         padding:����㷨:
//                 CRYPTO_PADDING_PKCS7:   PKCS7;
//                 CRYPTO_PADDING_ISO10126:ISO 10126;
//                 CRYPTO_PADDING_ANSIX923:ANSI X.923;
//                 CRYPTO_PADDING_NONE:    ���������;
//         plain:���ĵ��׵�ַ
//         plain_len:���ĳ���
//         key:8�ֽ���Կ���׵�ַ
//         iv:8�ֽڳ�ʼ���������׵�ַ��������ģʽΪECB���ò�����Ч
//         cipher:�洢���ĵ��׵�ַ
//         cipher_len:�洢���ĳ��ȵĵ�ַ
//���ܸ�Ҫ: ������ʹ��DES�㷨���м��ܣ��ɹ�ʱ�洢��Ӧ������
//��ע: ��������㷨ΪNone�����ĳ��Ȳ�Ϊ8�ı���ʱ������ʧ��;
//     ������㷨ΪISO10126����Ҫ�ȳ�ʼ��RNGģ��;
//     ������㷨ΪNone�����ĳ��ȵ������ĳ��ȣ�
//     �������ĳ���Ϊ���ĳ��Ȳ���8�ı����������ĳ���Ϊ8�ı����������ⲹ8���ֽ�;
//     ���ĵ�ַ����Ϊ���ĵ�ַ
//==========================================================================
bool crypto_des_encrypt(uint8 mode, uint8 padding, uint8* plain,
		uint32 plain_len, uint8* key, uint8* iv, uint8* cipher,
		uint32* cipher_len);

//==========================================================================
//��������: crypto_des_decrypt
//��������: true:���ܳɹ�; false:����ʧ��
//����˵��: mode:����ģʽ:
//              CRYPTO_MODE_ECB:���뱾;
//              CRYPTO_MODE_CBC:�����������;
//              CRYPTO_MODE_CFB:���뷴��;
//              CRYPTO_MODE_OFB:�������;
//         padding:����㷨:
//                 CRYPTO_PADDING_PKCS7:   PKCS7;
//                 CRYPTO_PADDING_ISO10126:ISO 10126;
//                 CRYPTO_PADDING_ANSIX923:ANSI X.923;
//                 CRYPTO_PADDING_NONE:    ���������;
//         cipher:���ĵ��׵�ַ
//         cipher_len:���ĳ���
//         key:8�ֽ���Կ���׵�ַ
//         iv:8�ֽڳ�ʼ���������׵�ַ��������ģʽΪECB���ò�����Ч
//         plain:�洢���ĵ��׵�ַ
//         plain_len:�洢���ĳ��ȵĵ�ַ
//���ܸ�Ҫ: ������ʹ��DES�㷨���н��ܣ��ɹ�ʱ�洢��Ӧ������
//��ע: �����ĳ��Ȳ�Ϊ8�ı�������������������ʱ������ʧ��;
//     ���ĳ��ȴ��ڵ������ĳ���;
//     ���ĵ�ַ����Ϊ���ĵ�ַ
//==========================================================================
bool crypto_des_decrypt(uint8 mode, uint8 padding, uint8* cipher,
		uint32 cipher_len, uint8* key, uint8* iv, uint8* plain,
		uint32* plain_len);

//==========================================================================
//��������: crypto_aes_encrypt
//��������: true:���ܳɹ�; false:����ʧ��
//����˵��: mode:����ģʽ:
//              CRYPTO_MODE_ECB:���뱾;
//              CRYPTO_MODE_CBC:�����������;
//              CRYPTO_MODE_CFB:���뷴��;
//              CRYPTO_MODE_OFB:�������;
//         padding:����㷨:
//                 CRYPTO_PADDING_PKCS7:   PKCS7;
//                 CRYPTO_PADDING_ISO10126:ISO 10126;
//                 CRYPTO_PADDING_ANSIX923:ANSI X.923;
//                 CRYPTO_PADDING_NONE:    ���������;
//         plain:���ĵ��׵�ַ
//         plain_len:���ĳ���
//         key:��Կ���׵�ַ
//         key_size:��Կ��λ��:
//                  128 | 192 | 256
//         iv:16�ֽڳ�ʼ���������׵�ַ��������ģʽΪECB���ò�����Ч
//         cipher:�洢���ĵ��׵�ַ
//         cipher_len:�洢���ĳ��ȵĵ�ַ
//���ܸ�Ҫ: ������ʹ��AES�㷨���м��ܣ��ɹ�ʱ�洢��Ӧ������
//��ע: ��������㷨ΪNone�����ĳ��Ȳ�Ϊ16�ı���ʱ������ʧ��;
//     ������㷨ΪISO10126����Ҫ�ȳ�ʼ��RNGģ��;
//     ������㷨ΪNone�����ĳ��ȵ������ĳ��ȣ�
//     �������ĳ���Ϊ���ĳ��Ȳ���16�ı����������ĳ���Ϊ16�ı����������ⲹ16���ֽ�;
//     ���ĵ�ַ����Ϊ���ĵ�ַ
//==========================================================================
bool crypto_aes_encrypt(uint8 mode, uint8 padding, uint8* plain,
		uint32 plain_len, uint8* key, uint16 key_size, uint8* iv, uint8* cipher,
		uint32* cipher_len);

//==========================================================================
//��������: crypto_aes_decrypt
//��������: true:���ܳɹ�; false:����ʧ��
//����˵��: mode:����ģʽ:
//              CRYPTO_MODE_ECB:���뱾;
//              CRYPTO_MODE_CBC:�����������;
//              CRYPTO_MODE_CFB:���뷴��;
//              CRYPTO_MODE_OFB:�������;
//         padding:����㷨:
//                 CRYPTO_PADDING_PKCS7:   PKCS7;
//                 CRYPTO_PADDING_ISO10126:ISO 10126;
//                 CRYPTO_PADDING_ANSIX923:ANSI X.923;
//                 CRYPTO_PADDING_NONE:    ���������;
//         cipher:���ĵ��׵�ַ
//         cipher_len:���ĳ���
//         key:��Կ���׵�ַ
//         key_size:��Կ��λ��:
//                  128 | 192 | 256
//         iv:16�ֽڳ�ʼ���������׵�ַ��������ģʽΪECB���ò�����Ч
//         plain:�洢���ĵ��׵�ַ
//         plain_len:�洢���ĳ��ȵĵ�ַ
//���ܸ�Ҫ: ������ʹ��AES�㷨���н��ܣ��ɹ�ʱ�洢��Ӧ������
//��ע: �����ĳ��Ȳ�Ϊ16�ı�������������������ʱ������ʧ��;
//     ���ĳ��ȴ��ڵ������ĳ���;
//     ���ĵ�ַ����Ϊ���ĵ�ַ
//==========================================================================
bool crypto_aes_decrypt(uint8 mode, uint8 padding, uint8* cipher,
		uint32 cipher_len, uint8* key, uint16 key_size, uint8* iv, uint8* plain,
		uint32* plain_len);

#endif
