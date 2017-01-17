//==========================================================================
//�ļ����ƣ�crypto.c
//���ܸ�Ҫ���ӽ����������Դ�ļ�
//==========================================================================

#include "crypto.h"

//==========================================================================
//��������: crypto_get_blk_num
//��������: ����ķ�����
//����˵��: len:��Ϣ�ĳ���
//���ܸ�Ҫ: ��ȡ����ķ�������ÿ��64�ֽ�
//==========================================================================
static uint32 crypto_get_blk_num(uint32 len) {
	//ÿ64�ֽ�һ�飬��ʣ���ֽ������ڵ���56�����������һ��
	return (len & 0x3F) >= 56 ? (len >> 6) + 2 : (len >> 6) + 1;
}

//==========================================================================
//��������: crypto_pad_msg
//��������: ��
//����˵��: msg:��Ϣ���׵�ַ
//         len:��Ϣ�ĳ���
//         pad_msg:�����Ϣ���׵�ַ
//         pad_len:�����Ϣ�ĳ���
//         lsb:�����Ƿ�ΪС��
//���ܸ�Ҫ: �����Ϣ
//==========================================================================
static void crypto_pad_msg(uint8* msg, uint32 len, uint8* pad_msg,
		uint32 pad_len, bool lsb) {
	uint32 pad0_index;	//���0��λ��
	uint32 len_index;	//��䳤�ȵ�λ��
	uint8* len_ptr;		//ָ�򳤶ȵ�ָ��
	uint64 bit_num;		//��Ϣ��λ��

	pad0_index = len + 1;	//���0x80��ʼ���0
	len_index = pad_len - 8;	//���8���ֽ���䳤��
	len_ptr = pad_msg + len_index;	//��ȡ�����׵�ַ
	bit_num = len << 3;		//������Ϣλ��

	memcpy(pad_msg, msg, len);	//������Ϣ
	pad_msg[len] = 0x80;	//���0x80
	memset(pad_msg + pad0_index, 0, len_index - pad0_index);	//���0
	//��䳤�ȣ�������λ����ʾ
	if (lsb) {
		//С��
		*(uint64*) len_ptr = bit_num;
	} else {
		//���
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
//��������: crypto_msb_to_lsb
//��������: ��
//����˵��: lsb:����ֵ��׵�ַ
//���ܸ�Ҫ: �������ת��ΪС����
//==========================================================================
static void crypto_msb_to_lsb(unsigned int* msb) {
	uint8 temp;	//��ʱ����
	uint8* ptr;	//8λָ��

	ptr = (uint8*) msb;	//ת��Ϊ8λָ��
	//������β�ֽ�
	temp = ptr[0];
	ptr[0] = ptr[3];
	ptr[3] = temp;
	//�����м��ֽ�
	temp = ptr[1];
	ptr[1] = ptr[2];
	ptr[2] = temp;
}

//==========================================================================
//��������: crypto_xor
//��������: ��
//����˵��: b1:��������1���׵�ַ
//         b2:��������2���׵�ַ
//         len:���������ĳ���
//         dest:�洢������ĵ�ַ
//���ܸ�Ҫ: ��b1��b2������򣬽��������dest
//==========================================================================
static void crypto_xor(uint8* b1, uint8* b2, uint32 len, uint8* dest) {
	uint32 i;
	for (i = 0; i < len; i++) {
		*(dest + i) = *(b1 + i) ^ *(b2 + i);
	}
}

//==========================================================================
//��������: crypto_md5
//��������: ��
//����˵��: msg:��Ϣ���׵�ַ
//         len:��Ϣ�ĳ���
//         digest:�洢����ժҪ���׵�ַ
//���ܸ�Ҫ: ��һ�����ȵ���Ϣʹ��MD5ɢ�к���������128bit����ϢժҪ(��16�ֽ�)
//==========================================================================
void crypto_md5(uint8* msg, uint32 len, uint8* digest) {
	uint32 blk_num;	//��Ϣ������
	uint32 pad_len;	//������ܳ���

	blk_num = crypto_get_blk_num(len);	//��ȡ������
	pad_len = blk_num << 6;	//���������ܳ���
	//�������Ϣ��C99��׼�����ں�����ʼ��������������ʹ�ñ�����ʼ������
	uint8 pad_msg[pad_len];

	//�����Ϣ
	crypto_pad_msg(msg, len, pad_msg, pad_len, true);
	//��ʼ�����ӱ���
	mmcau_md5_initialize_output(digest);
	//�Ը����������ѭ��
	mmcau_md5_hash_n(pad_msg, blk_num, digest);
}

//==========================================================================
//��������: crypto_md5_string
//��������: ��
//����˵��: msg:��Ϣ�ַ������׵�ַ
//         digest:�洢����ժҪ���׵�ַ
//���ܸ�Ҫ: ����Ϣ�ַ���ʹ��MD5ɢ�к���������128bit����ϢժҪ(��16�ֽ�)
//==========================================================================
void crypto_md5_string(uint8* msg, uint8* digest) {
	crypto_md5(msg, strlen(msg), digest);
}

//==========================================================================
//��������: crypto_sha1
//��������: ��
//����˵��: msg:��Ϣ���׵�ַ
//         len:��Ϣ�ĳ���
//         digest:�洢����ժҪ���׵�ַ
//���ܸ�Ҫ: ��һ�����ȵ���Ϣʹ��SHA1ɢ�к���������160bit����ϢժҪ(��20�ֽ�)
//��ע: �����ɵ���ϢժҪת������С�˸�ʽ
//==========================================================================
void crypto_sha1(uint8* msg, uint32 len, uint8* digest) {
	uint32 blk_num;	//��Ϣ������
	uint32 pad_len;	//������ܳ���
	unsigned int* ptr32;	//32λ��ժҪָ��
	uint8 i;		//�α�

	blk_num = crypto_get_blk_num(len);	//��ȡ������
	pad_len = blk_num << 6;	//���������ܳ���
	ptr32 = (unsigned int*) digest;		//��ժҪָ��ת��Ϊ32λ
	//�������Ϣ��C99��׼�����ں�����ʼ��������������ʹ�ñ�����ʼ������
	uint8 pad_msg[pad_len];

	//�����Ϣ
	crypto_pad_msg(msg, len, pad_msg, pad_len, false);
	//��ʼ����ϢժҪ����
	mmcau_sha1_initialize_output(ptr32);
	//�Ը��������ѹ������
	mmcau_sha1_hash_n(pad_msg, blk_num, ptr32);
	//ת��ΪС�˸�ʽ
	for (i = 0; i < 5; i++) {
		crypto_msb_to_lsb(ptr32 + i);
	}
}

//==========================================================================
//��������: crypto_sha1_string
//��������: ��
//����˵��: msg:��Ϣ�ַ������׵�ַ
//         digest:�洢����ժҪ���׵�ַ
//���ܸ�Ҫ: ����Ϣ�ַ���ʹ��SHA1ɢ�к���������160bit����ϢժҪ(��20�ֽ�)
//��ע: �����ɵ���ϢժҪת������С�˸�ʽ
//==========================================================================
void crypto_sha1_string(uint8* msg, uint8* digest) {
	crypto_sha1(msg, strlen(msg), digest);
}

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
		uint32* cipher_len) {
	uint32 blk_num;		//������
	uint8 *ptr8, *ptr8c;	//8λָ��
	uint8 pad_num;		//Ҫ�����ֽ���
	uint8 v[8];			//64λ����

	//�����������
	if (padding == CRYPTO_PADDING_NONE) {
		//�����ĳ��Ȳ�Ϊ8�ı���������ʧ��
		if (plain_len & 0x7) {
			return false;
		}
		//���������
		blk_num = plain_len >> 3;
	} else {
		//���ʱ����������һ��
		blk_num = (plain_len >> 3) + 1;
	}
	//�������ĳ���
	*cipher_len = blk_num << 3;
	//��������ģ�C99��׼�����ں�����ʼ��������������ʹ�ñ�����ʼ������
	uint8 pad_plain[*cipher_len];
	//��������
	memcpy(pad_plain, plain, plain_len);
	//ָ�����λ��
	ptr8 = pad_plain + plain_len;
	//��������ֽ���
	pad_num = *cipher_len - plain_len;
	//������䷽ʽ�������
	switch (padding) {
	case CRYPTO_PADDING_PKCS7:
		//���n��n
		memset(ptr8, pad_num, pad_num);
		break;
	case CRYPTO_PADDING_ISO10126:
		//���n-1�������
		rng_next_bytes(ptr8, pad_num - 1);
		//���һ���ֽ����n
		pad_plain[*cipher_len - 1] = pad_num;
		break;
	case CRYPTO_PADDING_ANSIX923:
		//���n-1��0
		memset(ptr8, 0, pad_num - 1);
		//���һ���ֽ����n
		pad_plain[*cipher_len - 1] = pad_num;
		break;
	}
	//���ݹ���ģʽ����
	switch (mode) {
	case CRYPTO_MODE_ECB:	//ECB
		//�Ը��������
		for (ptr8 = pad_plain, ptr8c = cipher; blk_num > 0; --blk_num, ptr8 +=
				8, ptr8c += 8) {
			mmcau_des_encrypt(ptr8, key, ptr8c);
		}
		break;
	case CRYPTO_MODE_CBC:	//CBC
		//��������Ϊ0ʱ�����м���
		if (blk_num) {
			//�Ե�һ�����
			crypto_xor(iv, pad_plain, 8, v);
			mmcau_des_encrypt(v, key, cipher);
			//����ʣ����
			for (--blk_num, ptr8 = pad_plain, ptr8c = cipher; blk_num > 0;
					--blk_num) {
				crypto_xor(ptr8c, ptr8 += 8, 8, v);
				mmcau_des_encrypt(v, key, ptr8c += 8);
			}
		}
		break;
	case CRYPTO_MODE_CFB:	//CFB
		//��������Ϊ0ʱ�����м���
		if (blk_num) {
			//�Ե�һ�����
			mmcau_des_encrypt(iv, key, v);
			crypto_xor(v, pad_plain, 8, cipher);
			//����ʣ����
			for (--blk_num, ptr8 = pad_plain, ptr8c = cipher; blk_num > 0;
					--blk_num) {
				mmcau_des_encrypt(ptr8c, key, v);
				crypto_xor(v, ptr8 += 8, 8, ptr8c += 8);
			}
		}
		break;
	case CRYPTO_MODE_OFB:	//OFB
		//��������Ϊ0ʱ�����м���
		if (blk_num) {
			//�Ե�һ�����
			mmcau_des_encrypt(iv, key, v);
			crypto_xor(v, pad_plain, 8, cipher);
			//����ʣ����
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
		uint32* plain_len) {
	uint8 pad_plain[cipher_len];	//���������
	uint32 blk_num;		//������
	uint8 *ptr8, *ptr8c;	//8λָ��
	uint8 v[8];			//64λ����
	uint8 pad_num;		//�����ֽ���

	//�����ĳ��Ȳ�Ϊ8�ı���������ʧ��
	if (cipher_len & 0x7) {
		return false;
	}
	blk_num = cipher_len >> 3;	//���������
	//���ݹ���ģʽ����
	switch (mode) {
	case CRYPTO_MODE_ECB:	//ECB
		//�Ը��������
		for (ptr8c = cipher, ptr8 = pad_plain; blk_num > 0; --blk_num, ptr8c +=
				8, ptr8 += 8) {
			mmcau_des_decrypt(ptr8c, key, ptr8);
		}
		break;
	case CRYPTO_MODE_CBC:	//CBC
		//��������Ϊ0ʱ�����н���
		if (blk_num) {
			//�Ե�һ�����
			mmcau_des_decrypt(cipher, key, v);
			crypto_xor(v, iv, 8, pad_plain);
			//����ʣ����
			for (--blk_num, ptr8 = pad_plain, ptr8c = cipher; blk_num > 0;
					--blk_num, ptr8c += 8) {
				mmcau_des_decrypt(ptr8c + 8, key, v);
				crypto_xor(v, ptr8c, 8, ptr8 += 8);
			}
		}
		break;
	case CRYPTO_MODE_CFB:	//CFB
		//��������Ϊ0ʱ�����н���
		if (blk_num) {
			//�Ե�һ�����
			mmcau_des_encrypt(iv, key, v);
			crypto_xor(v, cipher, 8, pad_plain);
			//����ʣ����
			for (--blk_num, ptr8 = pad_plain, ptr8c = cipher; blk_num > 0;
					--blk_num) {
				mmcau_des_encrypt(ptr8c, key, v);
				crypto_xor(v, ptr8c += 8, 8, ptr8 += 8);
			}
		}
		break;
	case CRYPTO_MODE_OFB:	//OFB
		//��������Ϊ0ʱ�����н���
		if (blk_num) {
			//�Ե�һ�����
			mmcau_des_encrypt(iv, key, v);
			crypto_xor(v, cipher, 8, pad_plain);
			//����ʣ����
			for (--blk_num, ptr8 = pad_plain, ptr8c = cipher; blk_num > 0;
					--blk_num) {
				mmcau_des_encrypt(v, key, v);
				crypto_xor(v, ptr8c += 8, 8, ptr8 += 8);
			}
		}
		break;
	}
	//���ĳ��Ȳ�Ϊ0ʱ
	if (cipher_len) {
		//��δ�������
		if (padding == CRYPTO_PADDING_NONE) {
			//���ĳ��ȵ������ĳ���
			*plain_len = cipher_len;
		} else {
			blk_num = cipher_len - 1;	//blk_num������ʱ����
			pad_num = pad_plain[blk_num];	//��ȡ����ֽ���
			//���������Ƿ�Ϸ�
			if (pad_num == 0 || pad_num > 8) {
				return false;
			}
			//������䷽ʽ�������Ƿ�Ϸ�
			switch (padding) {
			case CRYPTO_PADDING_PKCS7:
				//����Ƿ�Ϊn-1��n
				for (ptr8c = pad_plain + blk_num, ptr8 = ptr8c - pad_num + 1;
						ptr8 < ptr8c;) {
					if (*ptr8++ != pad_num) {
						return false;
					}
				}
				break;
			case CRYPTO_PADDING_ANSIX923:
				//����Ƿ�Ϊn-1��0
				for (ptr8c = pad_plain + blk_num, ptr8 = ptr8c - pad_num + 1;
						ptr8 < ptr8c;) {
					if (*ptr8++) {
						return false;
					}
				}
				break;
			}
			//ȥ������ֽ���
			*plain_len = cipher_len - pad_num;
		}
		//��������
		memcpy(plain, pad_plain, *plain_len);
	} else {
		//���ĳ���Ϊ0
		*plain_len = 0;
	}
	return true;
}

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
		uint32* cipher_len) {
	uint32 blk_num;		//������
	uint8 *ptr8, *ptr8c;	//8λָ��
	uint8 pad_num;		//Ҫ�����ֽ���
	uint8 v[16];		//128λ����
	uint8 nr;			//AES����

	//�����������
	if (padding == CRYPTO_PADDING_NONE) {
		//�����ĳ��Ȳ�Ϊ16�ı���������ʧ��
		if (plain_len & 0xF) {
			return false;
		}
		//���������
		blk_num = plain_len >> 4;
	} else {
		//���ʱ����������һ��
		blk_num = (plain_len >> 4) + 1;
	}
	//�������ĳ���
	*cipher_len = blk_num << 4;
	//��������ģ�C99��׼�����ں�����ʼ��������������ʹ�ñ�����ʼ������
	uint8 pad_plain[*cipher_len];
	//��������
	memcpy(pad_plain, plain, plain_len);
	//ָ�����λ��
	ptr8 = pad_plain + plain_len;
	//��������ֽ���
	pad_num = *cipher_len - plain_len;
	//������䷽ʽ�������
	switch (padding) {
	case CRYPTO_PADDING_PKCS7:
		//���n��n
		memset(ptr8, pad_num, pad_num);
		break;
	case CRYPTO_PADDING_ISO10126:
		//���n-1�������
		rng_next_bytes(ptr8, pad_num - 1);
		//���һ���ֽ����n
		pad_plain[*cipher_len - 1] = pad_num;
		break;
	case CRYPTO_PADDING_ANSIX923:
		//���n-1��0
		memset(ptr8, 0, pad_num - 1);
		//���һ���ֽ����n
		pad_plain[*cipher_len - 1] = pad_num;
		break;
	}
	//������Կλ������AES����
	switch (key_size) {
	case 128:
		nr = 10;
		//����pad_num����ʱ����
		pad_num = 176;
		break;
	case 192:
		nr = 12;
		pad_num = 208;
		break;
	case 256:
		nr = 14;
		pad_num = 240;
		break;
	}
	uint8 key_sch[pad_num];	//����Կ���ɵ�key_sch
	//����Կ����key_sch
	mmcau_aes_set_key(key, key_size, key_sch);
	//���ݹ���ģʽ����
	switch (mode) {
	case CRYPTO_MODE_ECB:	//ECB
		//�Ը��������
		for (ptr8 = pad_plain, ptr8c = cipher; blk_num > 0; --blk_num, ptr8 +=
				16, ptr8c += 16) {
			mmcau_aes_encrypt(ptr8, key_sch, nr, ptr8c);
		}
		break;
	case CRYPTO_MODE_CBC:	//CBC
		//��������Ϊ0ʱ�����м���
		if (blk_num) {
			//�Ե�һ�����
			crypto_xor(iv, pad_plain, 16, v);
			mmcau_aes_encrypt(v, key_sch, nr, cipher);
			//����ʣ����
			for (--blk_num, ptr8 = pad_plain, ptr8c = cipher; blk_num > 0;
					--blk_num) {
				crypto_xor(ptr8c, ptr8 += 16, 16, v);
				mmcau_aes_encrypt(v, key_sch, nr, ptr8c += 16);
			}
		}
		break;
	case CRYPTO_MODE_CFB:	//CFB
		//��������Ϊ0ʱ�����м���
		if (blk_num) {
			//�Ե�һ�����
			mmcau_aes_encrypt(iv, key_sch, nr, v);
			crypto_xor(v, pad_plain, 16, cipher);
			//����ʣ����
			for (--blk_num, ptr8 = pad_plain, ptr8c = cipher; blk_num > 0;
					--blk_num) {
				mmcau_aes_encrypt(ptr8c, key_sch, nr, v);
				crypto_xor(v, ptr8 += 16, 16, ptr8c += 16);
			}
		}
		break;
	case CRYPTO_MODE_OFB:	//OFB
		//��������Ϊ0ʱ�����м���
		if (blk_num) {
			//�Ե�һ�����
			mmcau_aes_encrypt(iv, key_sch, nr, v);
			crypto_xor(v, pad_plain, 16, cipher);
			//����ʣ����
			for (--blk_num, ptr8 = pad_plain, ptr8c = cipher; blk_num > 0;
					--blk_num) {
				mmcau_aes_encrypt(v, key_sch, nr, v);
				crypto_xor(v, ptr8 += 16, 16, ptr8c += 16);
			}
		}
		break;
	}
	return true;
}

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
		uint32* plain_len) {
	uint8 pad_plain[cipher_len];	//���������
	uint32 blk_num;		//������
	uint8 *ptr8, *ptr8c;	//8λָ��
	uint8 v[16];			//128λ����
	uint8 pad_num;		//�����ֽ���
	uint8 nr;			//AES����

	//�����ĳ��Ȳ�Ϊ16�ı���������ʧ��
	if (cipher_len & 0xF) {
		return false;
	}
	blk_num = cipher_len >> 4;	//���������
	//������Կλ������AES����
	switch (key_size) {
	case 128:
		nr = 10;
		//����pad_num����ʱ����
		pad_num = 176;
		break;
	case 192:
		nr = 12;
		pad_num = 208;
		break;
	case 256:
		nr = 14;
		pad_num = 240;
		break;
	}
	uint8 key_sch[pad_num];	//����Կ���ɵ�key_sch
	//����Կ����key_sch
	mmcau_aes_set_key(key, key_size, key_sch);
	//���ݹ���ģʽ����
	switch (mode) {
	case CRYPTO_MODE_ECB:	//ECB
		//�Ը��������
		for (ptr8c = cipher, ptr8 = pad_plain; blk_num > 0; --blk_num, ptr8c +=
				16, ptr8 += 16) {
			mmcau_aes_decrypt(ptr8c, key_sch, nr, ptr8);
		}
		break;
	case CRYPTO_MODE_CBC:	//CBC
		//��������Ϊ0ʱ�����н���
		if (blk_num) {
			//�Ե�һ�����
			mmcau_aes_decrypt(cipher, key_sch, nr, v);
			crypto_xor(v, iv, 16, pad_plain);
			//����ʣ����
			for (--blk_num, ptr8 = pad_plain, ptr8c = cipher; blk_num > 0;
					--blk_num, ptr8c += 16) {
				mmcau_aes_decrypt(ptr8c + 16, key_sch, nr, v);
				crypto_xor(v, ptr8c, 16, ptr8 += 16);
			}
		}
		break;
	case CRYPTO_MODE_CFB:	//CFB
		//��������Ϊ0ʱ�����н���
		if (blk_num) {
			//�Ե�һ�����
			mmcau_aes_encrypt(iv, key_sch, nr, v);
			crypto_xor(v, cipher, 16, pad_plain);
			//����ʣ����
			for (--blk_num, ptr8 = pad_plain, ptr8c = cipher; blk_num > 0;
					--blk_num) {
				mmcau_aes_encrypt(ptr8c, key_sch, nr, v);
				crypto_xor(v, ptr8c += 16, 16, ptr8 += 16);
			}
		}
		break;
	case CRYPTO_MODE_OFB:	//OFB
		//��������Ϊ0ʱ�����н���
		if (blk_num) {
			//�Ե�һ�����
			mmcau_aes_encrypt(iv, key_sch, nr, v);
			crypto_xor(v, cipher, 16, pad_plain);
			//����ʣ����
			for (--blk_num, ptr8 = pad_plain, ptr8c = cipher; blk_num > 0;
					--blk_num) {
				mmcau_aes_encrypt(v, key_sch, nr, v);
				crypto_xor(v, ptr8c += 16, 16, ptr8 += 16);
			}
		}
		break;
	}
	//���ĳ��Ȳ�Ϊ0ʱ
	if (cipher_len) {
		//��δ�������
		if (padding == CRYPTO_PADDING_NONE) {
			//���ĳ��ȵ������ĳ���
			*plain_len = cipher_len;
		} else {
			blk_num = cipher_len - 1;	//blk_num������ʱ����
			pad_num = pad_plain[blk_num];	//��ȡ����ֽ���
			//���������Ƿ�Ϸ�
			if (pad_num == 0 || pad_num > 16) {
				return false;
			}
			//������䷽ʽ�������Ƿ�Ϸ�
			switch (padding) {
			case CRYPTO_PADDING_PKCS7:
				//����Ƿ�Ϊn-1��n
				for (ptr8c = pad_plain + blk_num, ptr8 = ptr8c - pad_num + 1;
						ptr8 < ptr8c;) {
					if (*ptr8++ != pad_num) {
						return false;
					}
				}
				break;
			case CRYPTO_PADDING_ANSIX923:
				//����Ƿ�Ϊn-1��0
				for (ptr8c = pad_plain + blk_num, ptr8 = ptr8c - pad_num + 1;
						ptr8 < ptr8c;) {
					if (*ptr8++) {
						return false;
					}
				}
				break;
			}
			//ȥ������ֽ���
			*plain_len = cipher_len - pad_num;
		}
		//��������
		memcpy(plain, pad_plain, *plain_len);
	} else {
		//���ĳ���Ϊ0
		*plain_len = 0;
	}
	return true;
}
