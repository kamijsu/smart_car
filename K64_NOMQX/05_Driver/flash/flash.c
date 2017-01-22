//==========================================================================
//�ļ����ƣ�flash.c
//���ܸ�Ҫ��K64 Flash�ײ���������Դ�ļ�
//==========================================================================

#include "flash.h"
#include "uart.h"

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
	//�������־
	static const uint8 masks = FTFE_FSTAT_RDCOLERR_MASK | FTFE_FSTAT_ACCERR_MASK
			| FTFE_FSTAT_FPVIOL_MASK;
	uint8 temp;

	//ȥ���������־
	REG_SET_VAL(FTFE_FSTAT, masks);
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

static void flash_set_data(uint8 offset, uint8 num, volatile uint8* data) {
	//����FCCOB�Ĵ�������ַ
	static uint8* const data_ptr = (uint8*) (FTFE_BASE + 0x8);
	volatile uint8* volatile ptr;
	for (ptr = data_ptr + offset; num > 0; --num) {
		*ptr++ = *data++;
	}
}

void show() {
	uart_send1(UART_MOD1, FTFE_FCCOB0);
	uart_send1(UART_MOD1, FTFE_FCCOB1);
	uart_send1(UART_MOD1, FTFE_FCCOB2);
	uart_send1(UART_MOD1, FTFE_FCCOB3);
	uart_send1(UART_MOD1, FTFE_FCCOB4);
	uart_send1(UART_MOD1, FTFE_FCCOB5);
	uart_send1(UART_MOD1, FTFE_FCCOB6);
	uart_send1(UART_MOD1, FTFE_FCCOB7);
	uart_send1(UART_MOD1, FTFE_FCCOB8);
	uart_send1(UART_MOD1, FTFE_FCCOB9);
	uart_send1(UART_MOD1, FTFE_FCCOBA);
	uart_send1(UART_MOD1, FTFE_FCCOBB);
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
////	//����FCCOB�Ĵ�������ַ
////	static uint8* const data_ptr = (uint8*) (FTFE_BASE + 0x8);
//	uint8 bn;		//�ֽ���
//	uint32 addr;	//��ַFCCOB�Ĵ������õĵ�ֵַ
//	uint32 i;		//��ʱ����
//	uint16 j;		//��ʱ����
//	FlashResult result;	//д����
//
//	//ƫ�Ƶ�ַδ��8�ֽڲ���
//	bn = offset & 0x7;
//	//flash����ƫ�Ƶ�ַ
//	i = (sector << 12) + offset - bn;
//	//��23λ����ѡ�����flash��������flash
//	addr = (flash_addr_table[blk] >> 5) + i;
//	//��������Ϊд��8���ֽ�
//	REG_SET_VAL(FTFE_FCCOB0, PGM8);
//	//����ƫ�Ƶ�ַδ��8�ֽ����
//	if (bn) {
//		//���õ�ַ
//		flash_set_addr(addr);
//		//���������ַ
//		i += flash_addr_table[blk];
//		//��ԭ�������������FCCOB�Ĵ���
//		flash_set_data(0, bn, (uint8*) i);
////		memcpy(data_ptr, (void*) i, bn);
//		//���������д������֮��
//		j = bn + num;
//		//�ж��ܷ���8�ֽ�
//		if (j >= 8) {
//			//д����������
//			j = 8 - bn;
//			//����ʱ������ʣ������
//			flash_set_data(bn, j, data);
////			memcpy((void*) (data_ptr + bn), data, j);
//		} else {
//			//������ʱ���������ݺ���ԭ���������FCCOB�Ĵ���
//			flash_set_data(bn, num, data);
//			flash_set_data(j, 8 - j, (uint8*) (i + j));
////			memcpy((void*) (data_ptr + bn), data, num);
////			memcpy((void*) (data_ptr + j), (void*) (i + j), 8 - j);
//			//д����������
//			j = num;
//		}
////		show();
//		//ִ���������ȡ���
//		result = flash_launch();
//		//д��ʧ�ܣ���д�����ʱ�����ؽ��
//		if (result != FlashSuccess || (num -= j) == 0) {
//			return result;
//		}
//		//���õĵ�ֵַ���ƫ��8�ֽ�
//		addr += 8;
//		//д�������׵�ַ���ƫ��
//		data += j;
//	}
//	//ʣ��д������δ��8�ֽڲ���
//	bn = num & 0x7;
//	//д����8�ֽڲ���
//	for (num -= bn; num > 0; num -= 8, data += 8, addr += 8) {
//		//���õ�ַ
//		flash_set_addr(addr);
//		//��������
////		memcpy(data_ptr, data, 8);
//		REG_SET_VAL(FTFE_FCCOB7, data[0]);
//		REG_SET_VAL(FTFE_FCCOB6, data[1]);
//		REG_SET_VAL(FTFE_FCCOB5, data[2]);
//		REG_SET_VAL(FTFE_FCCOB4, data[3]);
//		REG_SET_VAL(FTFE_FCCOBB, data[4]);
//		REG_SET_VAL(FTFE_FCCOBA, data[5]);
//		REG_SET_VAL(FTFE_FCCOB9, data[6]);
//		REG_SET_VAL(FTFE_FCCOB8, data[7]);
//		//ִ���������ȡ���
//		result = flash_launch();
//		//д��ʧ��ʱ������ʧ��ԭ��
//		if (result != FlashSuccess) {
//			return result;
//		}
//	}
//	//����ʣ��д������δ��8�ֽ����
//	if (bn) {
//		//���õ�ַ
//		flash_set_addr(addr);
//		//����ַ��23λ����28λ�Ի�������ַ
//		if (BGET(addr, 23)) {
//			BCLR(addr, 23);
//			BSET(addr, 28);
//		}
//		//����ʣ�����ݺ���ԭ���������FCCOB�Ĵ���
//		flash_set_data(0, bn, data);
//		flash_set_data(bn, 8 - bn, (uint8*) (addr + bn));
//		show();
//		uart_send1(1,addr>>24);
//		uart_send1(1,addr>>16);
//		uart_send1(1,addr>>8);
//		uart_send1(1,addr);
////		memcpy(data_ptr, data, bn);
////		memcpy((void*) (data_ptr + bn), (void*) (addr + bn), 8 - bn);
//		//ִ���������ȡ���
//		result = flash_launch();
//	}
//	//����д����
//	return result;
}

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
void flash_read(uint8 blk, uint8 sector, uint16 offset, uint16 num, uint8* data) {
	//���߼���ַת��Ϊ�����ַ�������Ƶ��ڴ���
	memcpy(data, (uint8*) (flash_addr_table[blk] + (sector << 12) + offset),
			num);
////	memcpy(data,(void*)(flash_addr_table[blk] + (sector << 12) + offset),num);
//	volatile uint32* volatile ptrr;	//���Ż���32λָ��
//	uint32* ptrw;	//32λָ��
//
//	//���߼���ַת��Ϊ�����ַ
//	ptrr = (uint32*) (flash_addr_table[blk] + (sector << 12) + offset);
//	//ÿ4�ֽڶ�ȡ
//	for (ptrw = (uint32*) data; num >= 4; num -= 4) {
//		*ptrw++ = *ptrr++;
//	}
//	//��ȡʣ���ֽ�
//	switch (num) {
//	case 3:
//		*(uint16*) ptrw = *(volatile uint16*) ptrr;
//		*((uint8*) ptrw + 2) = *((volatile uint8*) ptrr + 2);
//		break;
//	case 2:
//		*(uint16*) ptrw = *(volatile uint16*) ptrr;
//		break;
//	case 1:
//		*(uint8*) ptrw = *(volatile uint8*) ptrr;
//		break;
//	}
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
//                ����flashʱ��ȡֵ��ΧΪ[0,31];
//���ܸ�Ҫ: ����һ������
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
//��ע: ������flash����ռ��EEPROM���ݣ�����ʱ�����طǷ�����
//==========================================================================
FlashResult flash_erase_dflash() {
	//��������Ϊ����һ��flash
	REG_SET_VAL(FTFE_FCCOB0, ERSBLK);
	//���õ�ַ
	flash_set_addr(flash_addr_table[FLASH_BLK_DFLASH] >> 5);
	//ִ����������ؽ��
	return flash_launch();
}
