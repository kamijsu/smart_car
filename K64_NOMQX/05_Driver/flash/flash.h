//==========================================================================
//�ļ����ƣ�flash.h
//���ܸ�Ҫ��K64 Flash�ײ���������ͷ�ļ�
//==========================================================================

#ifndef _FLASH_H
#define _FLASH_H

//��������ͷ�ļ�
#include "common.h"
//��Ҫ�õ�memcpy
#include <string.h>

//==========================================================================
//˵��: оƬ�ͺ�ΪMK64FX512VLL12��ӵ��FlexNVM�Լ�FlexRAM����ӵ������flash�Լ�
//     ģ��EEPROM������ʹ�ó���flash������flash��ģ��EEPROM��RAM(δ��ΪEEPROM�Ĳ���)��
//     ��оƬ��FlexNVM�Լ�FlexRAM����ֻ����ʹ�ó���flash��RAM
//==========================================================================

//����flash��
#define FLASH_BLK_PFLASH	(0)	//����flash
#define FLASH_BLK_DFLASH	(1)	//����flash

//����flash����ʼ��ַ
#define FLASH_ADDR_PFLASH	(0x00000000)	//0x00000000-0x0007FFFF
#define FLASH_ADDR_DFLASH	(0x10000000)	//0x10000000-0x1001FFFF
#define FLASH_ADDR_RAM		(0x14000000)	//0x14000000-0x14000FFF

//����flash�������
typedef enum {
	FlashSuccess, 	//�����ɹ�
	FlashIllegalArgument, //�Ƿ�����
	FlashProtected, //�������򱻱���
	FlashVerifyFail	//����ִ����ϣ���ȷ�ϲ������ʱ����
} FlashResult;

//==========================================================================
//��������: flash_write
//��������: д����:
//         FlashSuccess:        д��ɹ�;
//         FlashIllegalArgument:�Ƿ�����;
//         FlashProtected:      д�����򱻱���;
//         FlashVerifyFail:     д����ϣ���д������Ԥ�ڲ�ͬ;
//����˵��: blk:flash��:
//             FLASH_BLK_PFLASH:����flash;
//             FLASH_BLK_DFLASH:����flash;
//         sector:������:
//                ����flashʱ��ȡֵ��ΧΪ[0,127];
//                ����flashʱ��ȡֵ��ΧΪ[0,31];
//         offset:������ƫ�Ƶ�ַ������Ϊ8�ı���������С��4096
//         num:д����ֽ���������Ϊ8�ı���������С�ڵ���4096
//         data:д�����ݵ��׵�ַ
//���ܸ�Ҫ: ��һ������������д����Ӧflash���������
//��ע: ������СΪ4KB����һ������д��ʱ�����л�����һ����;
//     ����flash������flash������������flashд��ʱ�����л�������flash;
//     ��˳��ÿ��д��8B������һ�β��ɹ�����д��ʧ�ܣ���������Ӧ���;
//     ע�⣬��д����ΪFlashVerifyFail��˵��д��ǰû�в���������
//     ��flash����һ�������޷���ȡ����Ҫ�������
//==========================================================================
FlashResult flash_write(uint8 blk, uint8 sector, uint16 offset, uint16 num,
		uint8* data);

//==========================================================================
//��������: flash_read
//��������: ��
//����˵��: blk:flash��:
//             FLASH_BLK_PFLASH:����flash;
//             FLASH_BLK_DFLASH:����flash;
//         sector:������:
//                ����flashʱ��ȡֵ��ΧΪ[0,127];
//                ����flashʱ��ȡֵ��ΧΪ[0,31];
//         offset:������ƫ�Ƶ�ַ������С��4096
//         num:��ȡ���ֽ���������С�ڵ���4096
//         data:�洢��ȡ���ݵ��׵�ַ
//���ܸ�Ҫ: ����Ӧflash��������ж�ȡһ�����������ݣ��洢��RAM��
//��ע: ƫ�Ƶ�ַ�Ͷ�ȡ�ֽ������Բ�Ϊ8�ı���;
//     ������СΪ4KB����һ��������ȡ���ʱ�����л�����һ����;
//     ����flash������flash������������flash��ȡ���ʱ�����л�������flash
//==========================================================================
void flash_read(uint8 blk, uint8 sector, uint16 offset, uint16 num,
		uint8* data);

#endif