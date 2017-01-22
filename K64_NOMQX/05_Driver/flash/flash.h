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

//����flash���FlexRAM��
#define FLASH_BLK_PFLASH	(0)	//����flash
#define FLASH_BLK_DFLASH	(1)	//����flash
#define FLASH_BLK_FLEXRAM	(2)	//FlexRAM

//����flash����ʼ��ַ
#define FLASH_ADDR_PFLASH	(0x00000000)	//0x00000000-0x0007FFFF
#define FLASH_ADDR_DFLASH	(0x10000000)	//0x10000000-0x1001FFFF
#define FLASH_ADDR_FLEXRAM	(0x14000000)	//0x14000000-0x14000FFF

//��������flash������С
#define FLASH_DFLASH_SIZE_128	(0xF)	//����flash:128KB  EEPROM����:  0KB
#define FLASH_DFLASH_SIZE_96	(0x3)	//����flash: 96KB  EEPROM����: 32KB
#define FLASH_DFLASH_SIZE_64	(0x4)	//����flash: 64KB  EEPROM����: 64KB
#define FLASH_DFLASH_SIZE_32	(0xB)	//����flash: 32KB  EEPROM����: 96KB
#define FLASH_DFLASH_SIZE_0		(0x8)	//����flash:  0KB  EEPROM����:128KB

//����EEPROM������С
#define FLASH_EEPROM_SIZE_4K	(0x2)	// 4KB
#define FLASH_EEPROM_SIZE_2K	(0x3)	// 2KB
#define FLASH_EEPROM_SIZE_1K	(0x4)	// 1KB
#define FLASH_EEPROM_SIZE_512	(0x5)	//512B
#define FLASH_EEPROM_SIZE_256	(0x6)	//256B
#define FLASH_EEPROM_SIZE_128	(0x7)	//128B
#define FLASH_EEPROM_SIZE_64	(0x8)	// 64B
#define FLASH_EEPROM_SIZE_32	(0x9)	// 32B
#define FLASH_EEPROM_SIZE_0		(0xF)	//  0B

//����EEPROM��ϵͳ�ָ�����
#define FLASH_EEPROM_SPLIT_1_7	(0x0)	//A��ϵͳ:EEPROM*1/8 B��ϵͳ:EEPROM*7/8
#define FLASH_EEPROM_SPLIT_1_3	(0x1)	//A��ϵͳ:EEPROM*1/4 B��ϵͳ:EEPROM*3/4
#define FLASH_EEPROM_SPLIT_1_1	(0x3)	//A��ϵͳ:EEPROM*1/2 B��ϵͳ:EEPROM*1/2

//����FlexRAM��ģʽ
#define FLASH_FLEXRAM_MODE_RAM		(0xFF)	//RAM
#define FLASH_FLEXRAM_MODE_EEPROM	(0x00)	//EEPROM

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
//         offset:������ƫ�Ƶ�ַ������Ϊ8�ı���
//         num:д����ֽ���������Ϊ8�ı���
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
//         offset:������ƫ�Ƶ�ַ
//         num:��ȡ���ֽ���
//         data:�洢��ȡ���ݵ��׵�ַ
//���ܸ�Ҫ: ����Ӧflash��������ж�ȡһ�����������ݣ��洢��RAM��
//��ע: ƫ�Ƶ�ַ�Ͷ�ȡ�ֽ������Բ�Ϊ8�ı���;
//     ������СΪ4KB����һ��������ȡ���ʱ�����л�����һ����;
//     ����flash������flash������������flash��ȡ���ʱ�����л�������flash
//==========================================================================
void flash_read(uint8 blk, uint8 sector, uint16 offset, uint16 num,
		uint8* data);

//==========================================================================
//��������: flash_erase_sector
//��������: �������:
//         FlashSuccess:        �����ɹ�;
//         FlashIllegalArgument:�Ƿ�����;
//         FlashProtected:      ������������;
//         FlashVerifyFail:     ������ϣ�����������ȫΪ0xFF;
//����˵��: blk:flash��:
//             FLASH_BLK_PFLASH:����flash;
//             FLASH_BLK_DFLASH:����flash;
//         sector:������:
//                ����flashʱ��ȡֵ��ΧΪ[0,127];
//                ����flashʱ��ȡֵ��ΧΪ[0,31];
//���ܸ�Ҫ: ������Ӧflash���һ������
//��ע: ��������flash��0����ʱ��������оƬ���ܣ���Ҫ�������
//==========================================================================
FlashResult flash_erase_sector(uint8 blk, uint8 sector);

//==========================================================================
//��������: flash_erase_dflash
//��������: �������:
//         FlashSuccess:        �����ɹ�;
//         FlashIllegalArgument:�Ƿ�����;
//         FlashProtected:      ����flash�����򱻱���;
//         FlashVerifyFail:     ������ϣ�������flash��ȫΪ0xFF;
//����˵��: ��
//���ܸ�Ҫ: ��������flash
//��ע: ������flash����ռ��EEPROM���ݣ�����ʱ�����طǷ�����
//==========================================================================
FlashResult flash_erase_dflash();

FlashResult flash_check_empty(uint8 blk, uint8 sector, uint16 offset,
		uint16 num);

FlashResult flash_check_empty_dflash();

void flash_protect(uint8 blk, uint8 region);

bool flash_check_protected(uint8 blk, uint8 region);

FlashResult flash_partition(uint8 dflash_size, uint8 eeprom_size,
		uint8 eeprom_split);

bool flash_partition_get_info(uint8* dflash_size, uint8* eeprom_size,
		uint8* eeprom_split);

FlashResult flash_flexram_set_mode(uint8 flexram_mode);

uint8 flash_flexram_get_mode();

void flash_flexram_write(uint16 offset, uint16 num, uint8* data);

void flash_flexram_read(uint16 offset, uint16 num, uint8* data);

#endif
