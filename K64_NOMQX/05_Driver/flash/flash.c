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
//��������: ��ɺ�״̬�Ĵ�����ֵ
//����˵��: ��
//���ܸ�Ҫ: ִ��flash����
//==========================================================================
static uint8 flash_launch() {
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
	//����״̬�Ĵ�����ֵ
	return temp;
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
	uint32 addr;	//�����ַ
	uint8 result;	//д����

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
		//��������쳣ʱ���������Ǻ��ִ���
		if (result != FTFE_FSTAT_CCIF_MASK) {
			//�Ƿ�����
			if (REG_GET_MASK(result, FTFE_FSTAT_ACCERR_MASK)) {
				return FlashIllegalArgument;
			}
			//д�����򱻱���
			if (REG_GET_MASK(result, FTFE_FSTAT_FPVIOL_MASK)) {
				return FlashProtected;
			}
			//д����ϣ���д������Ԥ�ڲ�ͬ
			if (REG_GET_MASK(result, FTFE_FSTAT_MGSTAT0_MASK)) {
				return FlashVerifyFail;
			}
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
}
