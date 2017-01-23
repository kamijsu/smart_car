//==========================================================================
//�ļ����ƣ�flash.c
//���ܸ�Ҫ��K64 Flash�ײ���������Դ�ļ�
//==========================================================================

#include "flash.h"

//Flash����궨�壬�ڲ�ʹ��
#define RD1BLK	(0x00)	//���һ��flash�Ƿ�ȫΪ1
#define RD1SEC	(0x01)	//���һ�������Ƿ�ȫΪ1
#define PGMCHK	(0x02)	//���4���ֽ��Ƿ���Ԥ�ڵ�һ��
#define RDRSRC	(0x03)	//����Դ��Ϣ
#define PGM8	(0x07)  //д��8���ֽ�
#define ERSBLK	(0x08)	//����һ��flash
#define ERSSCR	(0x09)	//����һ������
#define PGMSEC	(0x0B)	//д��һ������
#define RD1ALL	(0x40)	//�������flash�Ƿ�ȫΪ1
#define RDONCE	(0x41)	//ֻ��һ������
#define PGMONCE	(0x43)	//ֻдһ������
#define ERSALL	(0x44)	//�����������򣬰���FlexRAM
#define VFYKEY	(0x45)	//��֤���ŷ���Կ��
#define PGMPART	(0x80)	//�������
#define SETRAM	(0x81)	//�趨FlexRAM����

//flash�����ַ��
static const uint32 flash_addr_table[] =
		{ FLASH_ADDR_PFLASH, FLASH_ADDR_DFLASH };
//flash״̬�Ĵ����������־
static const uint8 flash_error_masks = FTFE_FSTAT_RDCOLERR_MASK
		| FTFE_FSTAT_ACCERR_MASK | FTFE_FSTAT_FPVIOL_MASK;

//==========================================================================
//��������: flash_launch
//��������: �������:
//         FlashSuccess:        �����ɹ�;
//         FlashIllegalArgument:�Ƿ�����;
//         FlashProtected:      �������򱻱���;
//         FlashVerifyFail:     ����ִ����ϣ���ȷ�ϲ������ʱ����;
//����˵��: ��
//���ܸ�Ҫ: ִ��flash����
//==========================================================================
static FlashResult flash_launch() {
	uint8 temp;

	//ȥ���������־
	REG_SET_VAL(FTFE_FSTAT, flash_error_masks);
	//��������
	REG_SET_VAL(FTFE_FSTAT, FTFE_FSTAT_CCIF_MASK);
	//�ȴ��������
	do {
		temp = FTFE_FSTAT;
	} while (!REG_GET_MASK(temp, FTFE_FSTAT_CCIF_MASK));
	//��������쳣ʱ���������Ǻ��ִ���
	if (temp != FTFE_FSTAT_CCIF_MASK) {
		//�Ƿ�����
		if (REG_GET_MASK(temp, FTFE_FSTAT_ACCERR_MASK)) {
			return FlashIllegalArgument;
		}
		//�������򱻱���
		if (REG_GET_MASK(temp, FTFE_FSTAT_FPVIOL_MASK)) {
			return FlashProtected;
		}
		//����ִ����ϣ���ȷ�ϲ������ʱ����
		if (REG_GET_MASK(temp, FTFE_FSTAT_MGSTAT0_MASK)) {
			return FlashVerifyFail;
		}
	}
	//�����ɹ�
	return FlashSuccess;
}

//==========================================================================
//��������: flash_set_addr
//��������: ��
//����˵��: addr:��ַ
//���ܸ�Ҫ: ���õ�ַ
//==========================================================================
static void flash_set_addr(uint32 addr) {
	REG_SET_VAL(FTFE_FCCOB1, addr >> 16);
	REG_SET_VAL(FTFE_FCCOB2, addr >> 8);
	REG_SET_VAL(FTFE_FCCOB3, addr);
}

//==========================================================================
//��������: flash_init
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: ��ʼ��flashģ��
//��ע: �ú������ڵȴ�flash��λ��ϣ�Ϊ�˸��ֲ����İ�ȫ�ԣ���Ҫ���øú���
//==========================================================================
void flash_init() {
	//�ȴ�flash��ʼ�����
	while (!REG_GET_MASK(FTFE_FSTAT, FTFE_FSTAT_CCIF_MASK)) {
	}
}

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
		uint8* data) {
	uint32 addr;		//�����ַ
	FlashResult result;	//д����

	//д��������Ϊ8�ı���ʱ�������Ƿ�
	if (num & 0x7) {
		return FlashIllegalArgument;
	}
	//��ȡ��ʼ�����ַ����23λ����ѡ�����flash��������flash
	addr = (flash_addr_table[blk] >> 5) + (sector << 12) + offset;
	//��������Ϊд��8���ֽ�
	REG_SET_VAL(FTFE_FCCOB0, PGM8);
	//д����ֽ�
	for (; num > 0; num -= 8, data += 8, addr += 8) {
		//���õ�ַ
		flash_set_addr(addr);
		//��������
		REG_SET_VAL(FTFE_FCCOB7, data[0]);
		REG_SET_VAL(FTFE_FCCOB6, data[1]);
		REG_SET_VAL(FTFE_FCCOB5, data[2]);
		REG_SET_VAL(FTFE_FCCOB4, data[3]);
		REG_SET_VAL(FTFE_FCCOBB, data[4]);
		REG_SET_VAL(FTFE_FCCOBA, data[5]);
		REG_SET_VAL(FTFE_FCCOB9, data[6]);
		REG_SET_VAL(FTFE_FCCOB8, data[7]);
		//ִ���������ȡ���
		result = flash_launch();
		//д��ʧ��ʱ������ʧ��ԭ��
		if (result != FlashSuccess) {
			return result;
		}
	}
	//д��ɹ�
	return FlashSuccess;
}

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
void flash_read(uint8 blk, uint8 sector, uint16 offset, uint16 num, uint8* data) {
	//���߼���ַת��Ϊ�����ַ�������Ƶ��ڴ���
	memcpy(data, (uint8*) (flash_addr_table[blk] + (sector << 12) + offset),
			num);
}

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
FlashResult flash_erase_sector(uint8 blk, uint8 sector) {
	//��������Ϊ����һ������
	REG_SET_VAL(FTFE_FCCOB0, ERSSCR);
	//���õ�ַ
	flash_set_addr((flash_addr_table[blk] >> 5) + (sector << 12));
	//ִ����������ؽ��
	return flash_launch();
}

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
FlashResult flash_erase_dflash() {
	//��������Ϊ����һ��flash
	REG_SET_VAL(FTFE_FCCOB0, ERSBLK);
	//���õ�ַ
	flash_set_addr(flash_addr_table[FLASH_BLK_DFLASH] >> 5);
	//ִ����������ؽ��
	return flash_launch();
}

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
		uint16 num) {
	//���������Ϊ16�ı���ʱ�������Ƿ�
	if (num & 0xF) {
		return FlashIllegalArgument;
	}
	//��������Ϊ���һ�������Ƿ�ȫΪ1
	REG_SET_VAL(FTFE_FCCOB0, RD1SEC);
	//���õ�ַ
	flash_set_addr((flash_addr_table[blk] >> 5) + (sector << 12) + offset);
	//���ü��128λ�������
	REG_SET_VAL(FTFE_FCCOB4, num >> 12);
	REG_SET_VAL(FTFE_FCCOB5, num >> 4);
	//���ö���ȼ�Ϊ��ͨ
	REG_SET_VAL(FTFE_FCCOB6, 0x00);
	//ִ����������ؽ��
	return flash_launch();
}

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
FlashResult flash_check_empty_dflash() {
	//��������Ϊ���һ��flash�Ƿ�ȫΪ1
	REG_SET_VAL(FTFE_FCCOB0, RD1BLK);
	//���õ�ַ
	flash_set_addr(flash_addr_table[FLASH_BLK_DFLASH] >> 5);
	//���ö���ȼ�Ϊ��ͨ
	REG_SET_VAL(FTFE_FCCOB4, 0x00);
	//ִ����������ؽ��
	return flash_launch();
}

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
void flash_protect(uint8 blk, uint8 region) {
	uint8 shift;	//�����Ĵ�����ƫ����

	//����ƫ����
	shift = region & 0x7;
	//�ȴ��������
	while (!REG_GET_MASK(FTFE_FSTAT, FTFE_FSTAT_CCIF_MASK)) {
	}
	//������ѡ����б���
	switch (blk) {
	case FLASH_BLK_PFLASH:	//����flash
		//��������ѡ��Ĵ������б���
		switch (region >> 3) {
		case 0:
			REG_CLR_SHIFT(FTFE_FPROT3, shift);
			break;
		case 1:
			REG_CLR_SHIFT(FTFE_FPROT2, shift);
			break;
		case 2:
			REG_CLR_SHIFT(FTFE_FPROT1, shift);
			break;
		case 3:
			REG_CLR_SHIFT(FTFE_FPROT0, shift);
			break;
		}
		break;
	case FLASH_BLK_DFLASH:	//����flash
		REG_CLR_SHIFT(FTFE_FDPROT, shift);
		break;
	case FLASH_BLK_FLEXRAM:	//FlexRAM
		REG_CLR_SHIFT(FTFE_FEPROT, shift);
		break;
	}
}

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
bool flash_check_protected(uint8 blk, uint8 region) {
	uint8 shift;	//�����Ĵ�����ƫ����
	uint8 bit;		//����λ��ֵ

	//����λ��ֵĬ��Ϊ1��������������Ƿ�ʱ�᷵��δ����
	bit = 1;
	//����ƫ����
	shift = region & 0x7;
	//������ѡ���ȡ����λ��ֵ
	switch (blk) {
	case FLASH_BLK_PFLASH:	//����flash
		//��������ѡ����Ӧ�Ĵ�����ȡ����λ��ֵ
		switch (region >> 3) {
		case 0:
			bit = REG_GET_SHIFT(FTFE_FPROT3, shift);
			break;
		case 1:
			bit = REG_GET_SHIFT(FTFE_FPROT2, shift);
			break;
		case 2:
			bit = REG_GET_SHIFT(FTFE_FPROT1, shift);
			break;
		case 3:
			bit = REG_GET_SHIFT(FTFE_FPROT0, shift);
			break;
		}
		break;
	case FLASH_BLK_DFLASH:	//����flash
		bit = REG_GET_SHIFT(FTFE_FDPROT, shift);
		break;
	case FLASH_BLK_FLEXRAM:	//FlexRAM
		bit = REG_GET_SHIFT(FTFE_FEPROT, shift);
		break;
	}
	//������λ��ֵΪ0��˵��������
	return bit == 0 ? true : false;
}

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
		uint8 eeprom_split) {
	//��������Ϊ�������
	REG_SET_VAL(FTFE_FCCOB0, PGMPART);
	//����EEPROM��������
	REG_SET_VAL(FTFE_FCCOB4, (eeprom_split << 4) + eeprom_size);
	//��������flash��������
	REG_SET_VAL(FTFE_FCCOB5, dflash_size);
	//ִ����������ؽ��
	return flash_launch();
}

//==========================================================================
//��������: flash_partition_get_info
//��������: true:��ȡ��Ϣ�ɹ�; false:��ȡ��Ϣʧ��
//����˵��: dflash_size:�洢����flash������С�ĵ�ַ
//         eeprom_size:�洢EEPROM������С�ĵ�ַ
//         eeprom_split:�洢EEPROM��ϵͳ�ָ����ӵĵ�ַ
//���ܸ�Ҫ: ��ȡ��ǰ�ķ�����Ϣ���ɹ�ʱ�洢��Ӧ����������ֵ��궨��ֵ��ͬ
//==========================================================================
bool flash_partition_get_info(uint8* dflash_size, uint8* eeprom_size,
		uint8* eeprom_split) {
	uint8 temp;	//��ʱ����

	//��������Ϊ����Դ��Ϣ
	REG_SET_VAL(FTFE_FCCOB0, RDRSRC);
	//���õ�ַ��ΪData Flash 0 IFR���8�ֽ�
	REG_SET_VAL(FTFE_FCCOB1, 0x80);
	REG_SET_VAL(FTFE_FCCOB2, 0x03);
	REG_SET_VAL(FTFE_FCCOB3, 0xF8);
	//������Դѡ����
	REG_SET_VAL(FTFE_FCCOB4, 0x00);
	//ִ���������ȡ���
	if (flash_launch() == FlashSuccess) {
		//ִ������ɹ�ʱ����ȡ��Դ�ɹ�
		//0x3FC������flash������Ϣ
		temp = FTFE_FCCOBB;
		*dflash_size = temp & 0xF;
		//0x3FD��EEPROM������Ϣ
		temp = FTFE_FCCOBA;
		*eeprom_size = temp & 0xF;
		*eeprom_split = (temp >> 4) & 0x3;
		return true;
	} else {
		//ִ������ʧ��ʱ����ȡ��Դʧ��
		return false;
	}
}

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
FlashResult flash_flexram_set_mode(uint8 flexram_mode) {
	//��������Ϊ�趨FlexRAM����
	REG_SET_VAL(FTFE_FCCOB0, SETRAM);
	//����FlexRAM��ģʽ
	REG_SET_VAL(FTFE_FCCOB1, flexram_mode);
	//ִ����������ؽ��
	return flash_launch();
}

//==========================================================================
//��������: flash_flexram_get_mode
//��������: FlexRAM��ģʽ:
//         FLASH_FLEXRAM_MODE_RAM:   RAM;
//         FLASH_FLEXRAM_MODE_EEPROM:EEPROM;
//����˵��: ��
//���ܸ�Ҫ: ��ȡ��ǰFlexRAM��ģʽ������ֵ��궨��ֵ��ͬ
//==========================================================================
uint8 flash_flexram_get_mode() {
	uint8 fcnfg;	//flash���üĴ�����ֵ
	for (;;) {
		//��ȡ���üĴ�����ֵ
		fcnfg = FTFE_FCNFG;
		//��EEPROM�������򷵻�EEPROMģʽ
		if (REG_GET_MASK(fcnfg, FTFE_FCNFG_EEERDY_MASK)) {
			return FLASH_FLEXRAM_MODE_EEPROM;
		}
		//��RAM�������򷵻�RAMģʽ
		if (REG_GET_MASK(fcnfg, FTFE_FCNFG_RAMRDY_MASK)) {
			return FLASH_FLEXRAM_MODE_RAM;
		}
		//��������ȴ��������
	}
}

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
bool flash_flexram_write(uint16 offset, uint16 num, uint8* data) {
	uint8* ptr;	//8λָ��
	uint8 i;	//��ʱ����

	//��ȡ��ʼ�����ַ
	ptr = (uint8*) (FLASH_ADDR_FLEXRAM + offset);
	//��ȡFlexRAMģʽ
	if (flash_flexram_get_mode() == FLASH_FLEXRAM_MODE_EEPROM) {
		//EEPROMģʽʱ���ֶ�д��
		//��ַû��4�ֽڶ���ʱ��ÿ��д��1�ֽڲ���4�ֽڶ���
		for (i = offset & 0x3; i > 0 && num > 0;) {
			//ȥ���������־
			REG_SET_VAL(FTFE_FSTAT, flash_error_masks);
			//д��1�ֽ�����
			*ptr = *data;
			++ptr;
			++data;
			--num;
			i = (i + 1) & 0x3;
			//�ȴ�д�����
			while (!REG_GET_MASK(FTFE_FCNFG, FTFE_FCNFG_EEERDY_MASK)) {
			}
			//��д�뱻��������д��ʧ��
			if (REG_GET_MASK(FTFE_FSTAT, FTFE_FSTAT_FPVIOL_MASK)) {
				return false;
			}
		}
		//��ַ4�ֽڶ����ÿ��д��4�ֽ�
		for (; num >= 4;) {
			//ȥ���������־
			REG_SET_VAL(FTFE_FSTAT, flash_error_masks);
			//д��4�ֽ�����
			*(uint32*) ptr = *(uint32*) data;
			ptr += 4;
			data += 4;
			num -= 4;
			//�ȴ�д�����
			while (!REG_GET_MASK(FTFE_FCNFG, FTFE_FCNFG_EEERDY_MASK)) {
			}
			//��д�뱻��������д��ʧ��
			if (REG_GET_MASK(FTFE_FSTAT, FTFE_FSTAT_FPVIOL_MASK)) {
				return false;
			}
		}
		//������ʣ������
		if (num) {
			//ȥ���������־
			REG_SET_VAL(FTFE_FSTAT, flash_error_masks);
			//����ʣ������
			switch (num) {
			case 3:
				//ʣ��3�ֽڣ���д��2�ֽ����ݣ���д��1�ֽ�����
				*(uint16*) ptr = *(uint16*) data;
				ptr += 2;
				data += 2;
				//�ȴ�д�����
				while (!REG_GET_MASK(FTFE_FCNFG, FTFE_FCNFG_EEERDY_MASK)) {
				}
				//��д�뱻��������д��ʧ��
				if (REG_GET_MASK(FTFE_FSTAT, FTFE_FSTAT_FPVIOL_MASK)) {
					return false;
				}
				//ȥ���������־
				REG_SET_VAL(FTFE_FSTAT, flash_error_masks);
				*ptr = *data;
				break;
			case 2:
				//ʣ��2�ֽڣ�ֱ��д��2�ֽ�����
				*(uint16*) ptr = *(uint16*) data;
				break;
			default:
				//ʣ��1�ֽڣ�ֱ��д��1�ֽ�����
				*ptr = *data;
				break;
			}
			//�ȴ�д�����
			while (!REG_GET_MASK(FTFE_FCNFG, FTFE_FCNFG_EEERDY_MASK)) {
			}
			//��д�뱻��������д��ʧ��
			if (REG_GET_MASK(FTFE_FSTAT, FTFE_FSTAT_FPVIOL_MASK)) {
				return false;
			}
		}
	} else {
		//RAMģʽʱ��ֱ�ӵ���memcpy
		memcpy(ptr, data, num);
	}
	//д��ɹ�
	return true;
}

//==========================================================================
//��������: flash_flexram_read
//��������: ��
//����˵��: offset:ƫ�Ƶ�ַ
//         num:��ȡ���ֽ���
//         data:�洢��ȡ���ݵ��׵�ַ
//���ܸ�Ҫ: ��FlexRAM�ж�ȡһ�����������ݣ��洢��RAM��
//==========================================================================
void flash_flexram_read(uint16 offset, uint16 num, uint8* data) {
	uint8* ptr;	//8λָ��

	//��ȡ��ʼ�����ַ
	ptr = (uint8*) (FLASH_ADDR_FLEXRAM + offset);
	//�ȴ����ݾ���
	while (!REG_GET_MASK(FTFE_FCNFG,
			FTFE_FCNFG_EEERDY_MASK|FTFE_FCNFG_RAMRDY_MASK)) {
	}
	//��ȡ�������ڴ�
	memcpy(data, ptr, num);
}
