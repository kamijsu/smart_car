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
//˵��: оƬ�ͺ�ΪMK64FX512VLL12��ӵ������flash�Լ�FlexRAM������flash����ǰ��
//     FlexRAM������Ϊ��ͨRAMʹ�ã�������flash����ռ���ΪEEPROM���ݣ�
//     ��FlexRAM���Ա���ΪEEPROMʹ��;
//     ��оƬ������flash�Լ�FlexRAM����ֻ����ʹ�ó���flash��FlexRAM��RAMģʽ
//ע��: ����flashд��Ͳ�������ʱ��Ϊ�˰�ȫ����ر����ж�
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
//��������: flash_init
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: ��ʼ��flashģ��
//��ע: �ú������ڵȴ�flash��λ��ϣ�Ϊ�˸��ֲ����İ�ȫ�ԣ���Ҫ���øú���
//==========================================================================
void flash_init();

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
//                ����flashʱ��ȡֵ��ΧΪ[0,31]�����������Ӧ���٣����������˵��;
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
//                ����flashʱ��ȡֵ��ΧΪ[0,31]�����������Ӧ���٣����������˵��;
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
//                ����flashʱ��ȡֵ��ΧΪ[0,31]�����������Ӧ���٣����������˵��;
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
//��ע: ������flash����ռ��EEPROM���ݣ��ú������޷�ʹ�ã�����ʱ���طǷ�����
//==========================================================================
FlashResult flash_erase_dflash();

//==========================================================================
//��������: flash_check_empty
//��������: �����:
//         FlashSuccess:        ���ɹ�����������ȫ��Ϊ0xFF;
//         FlashIllegalArgument:�Ƿ�����;
//         FlashVerifyFail:     ���ʧ�ܣ���������ȫΪ0xFF;
//����˵��: blk:flash��:
//             FLASH_BLK_PFLASH:����flash;
//             FLASH_BLK_DFLASH:����flash;
//         sector:������:
//                ����flashʱ��ȡֵ��ΧΪ[0,127];
//                ����flashʱ��ȡֵ��ΧΪ[0,31]�����������Ӧ���٣����������˵��;
//         offset:������ƫ�Ƶ�ַ������Ϊ16�ı���
//         num:�����ֽ���������Ϊ16�ı���
//���ܸ�Ҫ: �����Ӧflash���һƬ�����Ƿ�Ϊ�գ����������Ƿ�ȫΪ0xFF
//��ע: ������СΪ4KB����һ������������ʱ�����л�����һ����;
//     ����flash������flash������������flash������ʱ�����л�������flash
//==========================================================================
FlashResult flash_check_empty(uint8 blk, uint8 sector, uint16 offset,
		uint16 num);

//==========================================================================
//��������: flash_check_empty_dflash
//��������: �����:
//         FlashSuccess:        ���ɹ���������flashȫ��Ϊ0xFF;
//         FlashIllegalArgument:�Ƿ�����;
//         FlashVerifyFail:     ���ʧ�ܣ�������flash��ȫΪ0xFF;
//����˵��: ��
//���ܸ�Ҫ: �������flash�Ƿ�Ϊ�գ����Ƿ�ȫΪ0xFF
//��ע: ������flash����ռ��EEPROM���ݣ��ú������޷�ʹ�ã�����ʱ���طǷ�����
//==========================================================================
FlashResult flash_check_empty_dflash();

//==========================================================================
//��������: flash_protect
//��������: ��
//����˵��: blk:flash���FlexRAM��:
//             FLASH_BLK_PFLASH: ����flash;
//             FLASH_BLK_DFLASH: ����flash;
//             FLASH_BLK_FLEXRAM:FlexRAM;
//         region:�����:
//                ����flashʱ��ȡֵ��ΧΪ[0,31];
//                ����flashʱ��ȡֵ��ΧΪ[0,7];
//                FlexRAMʱ��ȡֵ��ΧΪ[0,7];
//���ܸ�Ҫ: ����flash���FlexRAM���һƬ���򣬱�����������޷�������д��
//��ע: ���������´θ�λ��ʧЧ�����븴λ����Ȼ�������������޸�����ʱflash������;
//
//     �Գ���flash��ÿ�����򱣻�4����������0�����򱣻�0-3��������
//     1�����򱣻�4-7���������Դ�����;
//
//     ������flash����������СΪ128K����96Kʱ��ÿ�����򱣻�4��������
//     �ҷ�����СΪ96Kʱ���������Ч��Χ��Ϊ[0,5]��
//     ������СΪ64Kʱ��ÿ�����򱣻�2��������
//     ������СΪ32Kʱ��ÿ�����򱣻�1��������
//     ������СΪ0ʱ���������û������;
//
//     ��FlexRAM����������FlexRAM��ΪEEPROMʱ��Ч;
//     ��FlexRAM�ķ�����Сƽ���ֳ�8�ݣ�ÿ�����򱣻���Ӧ��һ�ݣ�������Ŷ�Ӧ�������ַ
//==========================================================================
void flash_protect(uint8 blk, uint8 region);

//==========================================================================
//��������: flash_check_protected
//��������: true:�����򱻱���; false:������δ������
//����˵��: blk:flash���FlexRAM��:
//             FLASH_BLK_PFLASH: ����flash;
//             FLASH_BLK_DFLASH: ����flash;
//             FLASH_BLK_FLEXRAM:FlexRAM;
//         region:�����:
//                ����flashʱ��ȡֵ��ΧΪ[0,31];
//                ����flashʱ��ȡֵ��ΧΪ[0,7];
//                FlexRAMʱ��ȡֵ��ΧΪ[0,7];
//���ܸ�Ҫ: ���flash���FlexRAM���һƬ�����Ƿ񱻱���������˵������������
//==========================================================================
bool flash_check_protected(uint8 blk, uint8 region);

//==========================================================================
//��������: flash_partition
//��������: �������:
//         FlashSuccess:        �����ɹ�;
//         FlashIllegalArgument:�Ƿ�����;
//         FlashVerifyFail:     ������ϣ������������Ԥ�ڲ�ͬ;
//����˵��: dflash_size:����flash������С:
//                     FLASH_DFLASH_SIZE_128:����flash:128KB  EEPROM����:  0KB;
//                     FLASH_DFLASH_SIZE_96: ����flash: 96KB  EEPROM����: 32KB;
//                     FLASH_DFLASH_SIZE_64: ����flash: 64KB  EEPROM����: 64KB;
//                     FLASH_DFLASH_SIZE_32: ����flash: 32KB  EEPROM����: 96KB;
//                     FLASH_DFLASH_SIZE_0:  ����flash:  0KB  EEPROM����:128KB;
//         eeprom_size:EEPROM������С:
//                     FLASH_EEPROM_SIZE_4K: 4KB;
//                     FLASH_EEPROM_SIZE_2K: 2KB;
//                     FLASH_EEPROM_SIZE_1K: 1KB;
//                     FLASH_EEPROM_SIZE_512:512B;
//                     FLASH_EEPROM_SIZE_256:256B;
//                     FLASH_EEPROM_SIZE_128:128B;
//                     FLASH_EEPROM_SIZE_64: 64B;
//                     FLASH_EEPROM_SIZE_32: 32B;
//                     FLASH_EEPROM_SIZE_0:  0B;
//         eeprom_split:EEPROM��ϵͳ�ָ�����:
//                      FLASH_EEPROM_SPLIT_1_7:A��ϵͳ:EEPROM*1/8 B��ϵͳ:EEPROM*7/8;
//                      FLASH_EEPROM_SPLIT_1_3:A��ϵͳ:EEPROM*1/4 B��ϵͳ:EEPROM*3/4;
//                      FLASH_EEPROM_SPLIT_1_1:A��ϵͳ:EEPROM*1/2 B��ϵͳ:EEPROM*1/2;
//���ܸ�Ҫ: ������flash���з���������һ���ֿռ���ΪEEPROM���ݣ�
//         ������FlexRAM��EEPROMģʽʱ�Ŀ��ÿռ��С
//��ע: Ĭ������£�����flash������СΪ128K��EEPROM������СΪ0����ϵͳ�ָ�����Ϊ1:1;
//
//     ����flash����ռ��EEPROM����ʱ��EEPROM������С����Ϊ0;
//     EEPROM������С��Ϊ0ʱ������flash�������ռ��EEPROM����;
//     EEPROM������СΪ32ʱ����ϵͳ�ָ����Ӳ���Ϊ1:7;
//     EEPROM������СΪ4Kʱ������ϵͳ�ָ����Ӳ�Ϊ1:1����EEPROM���ݱ������32K;
//
//     ����flash�����󣬿�����������Ӧ�ļ��٣���������:
//     ������СΪ128Kʱ������������ΧΪ[0,31]��
//     ������СΪ96Kʱ������������ΧΪ[0,23]��
//     ������СΪ64Kʱ������������ΧΪ[0,15]��
//     ������СΪ32Kʱ������������ΧΪ[0,7]��
//     ������СΪ0ʱ���޿�������;
//
//     EEPROM���շָ����ӱ��ָ�ΪA��B��ϵͳ����ʹ��ʱ���迼����ϵͳ���
//     ����Ӱ��EEPROM��ʹ����������EEPROM������С��EEPROM���ݿռ��С֮��ԽСʱ��
//     ʹ������Խ��;
//     EEPROM���ݱ�ƽ���ֳɴ�С��ͬ�����ݣ�ΪA��B��ϵͳ�ṩ����;
//     EEPROM���ݿռ���Ӳ���Զ������������ֶ�����;
//
//     �ú���ִ�й����в��ܱ��жϣ���ִ�й��������ٴη���ʱ�᷵�طǷ�������
//     ��Ҫ�����������Ҫ�������;
//
//     ִ�й������󣬸�λоƬʱ��FlexRAM��Ĭ��ΪEEPROMģʽ
//==========================================================================
FlashResult flash_partition(uint8 dflash_size, uint8 eeprom_size,
		uint8 eeprom_split);

//==========================================================================
//��������: flash_partition_get_info
//��������: true:��ȡ��Ϣ�ɹ�; false:��ȡ��Ϣʧ��
//����˵��: dflash_size:�洢����flash������С�ĵ�ַ
//         eeprom_size:�洢EEPROM������С�ĵ�ַ
//         eeprom_split:�洢EEPROM��ϵͳ�ָ����ӵĵ�ַ
//���ܸ�Ҫ: ��ȡ��ǰ�ķ�����Ϣ���ɹ�ʱ�洢��Ӧ����������ֵ��궨��ֵ��ͬ
//==========================================================================
bool flash_partition_get_info(uint8* dflash_size, uint8* eeprom_size,
		uint8* eeprom_split);

//==========================================================================
//��������: flash_flexram_set_mode
//��������: ����ģʽ���:
//         FlashSuccess:        ���óɹ�;
//         FlashIllegalArgument:�Ƿ�����;
//����˵��: flexram_mode:FlexRAM��ģʽ:
//                      FLASH_FLEXRAM_MODE_RAM:   RAM;
//                      FLASH_FLEXRAM_MODE_EEPROM:EEPROM;
//���ܸ�Ҫ: ����FlexRAM��ģʽ
//��ע: ��EEPROM������СΪ0������EEPROMģʽʱ�����طǷ�����;
//     EEPROMģʽʱ��FlexRAM���ÿռ���EEPROM������С��������ʣ��ռ��޷�ʹ��;
//     RAMģʽʱ��FlexRAM���ÿռ�Ϊ4K����ÿ������ΪRAMģʽʱ�������ռ�ᱻ���0xFF
//==========================================================================
FlashResult flash_flexram_set_mode(uint8 flexram_mode);

//==========================================================================
//��������: flash_flexram_get_mode
//��������: FlexRAM��ģʽ:
//         FLASH_FLEXRAM_MODE_RAM:   RAM;
//         FLASH_FLEXRAM_MODE_EEPROM:EEPROM;
//����˵��: ��
//���ܸ�Ҫ: ��ȡ��ǰFlexRAM��ģʽ������ֵ��궨��ֵ��ͬ
//==========================================================================
uint8 flash_flexram_get_mode();

//==========================================================================
//��������: flash_flexram_write
//��������: true:д��ɹ�; false:д��ʧ�ܣ���EEPROMģʽʱ��д�������������������
//����˵��: offset:ƫ�Ƶ�ַ
//         num:д����ֽ���
//         data:д�����ݵ��׵�ַ
//���ܸ�Ҫ: ��һ������������д��FlexRAM��
//��ע: д��ǰ�����������д���������ܿ��ÿռ��С���ƣ����ÿռ��С������ģʽ˵��;
//     EEPROMģʽʱ�������ݵ���󲻻ᶪʧ���������ϵ�ʱ���EEPROM���ݿռ俽����FlexRAM;
//     ����������������ǰ�����ݻᱻд��FlexRAM�������������������������ʧ�ܣ�����д��;
//     RAMģʽʱ�������ݵ����ᶪʧ
//==========================================================================
bool flash_flexram_write(uint16 offset, uint16 num, uint8* data);

//==========================================================================
//��������: flash_flexram_read
//��������: ��
//����˵��: offset:ƫ�Ƶ�ַ
//         num:��ȡ���ֽ���
//         data:�洢��ȡ���ݵ��׵�ַ
//���ܸ�Ҫ: ��FlexRAM�ж�ȡһ�����������ݣ��洢��RAM��
//==========================================================================
void flash_flexram_read(uint16 offset, uint16 num, uint8* data);

#endif
