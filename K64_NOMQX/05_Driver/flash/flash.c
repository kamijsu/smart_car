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

//==========================================================================
//��������: flash_launch
//��������: ��ɺ�״̬�Ĵ�����ֵ
//����˵��: ��
//���ܸ�Ҫ: ִ��flash����
//==========================================================================
static uint32 flash_launch() {
	uint32 temp;

	//ȥ���������־
	temp = FTFE_FSTAT_RDCOLERR_MASK | FTFE_FSTAT_ACCERR_MASK
			| FTFE_FSTAT_FPVIOL_MASK;
	REG_SET_VAL(FTFE_FSTAT, temp);
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
//��������: flash_logical_to_physical
//��������: �����ַ
//����˵��: blk:flash��:
//             FLASH_BLK_PFLASH:����flash;
//             FLASH_BLK_DFLASH:����flash;
//         sector:������:
//                ����flashʱ��ȡֵ��ΧΪ[0,127];
//                ����flashʱ��ȡֵ��ΧΪ[0,31];
//         offset:����ƫ�Ƶ�ַ
//���ܸ�Ҫ: ���߼���ַת��Ϊ�����ַ
//==========================================================================
static uint32 flash_logical_to_physical(uint8 blk, uint8 sector, uint16 offset) {
	return flash_addr_table[blk] + (sector << 12) + offset;
}

uint16 flash_write(uint8 blk, uint8 sector, uint16 offset, uint16 num,
		uint8* data) {
	uint32 addr;	//�����ַ
	uint16 i,j;		//��ʱ����
	uint32 result;	//д����

	//��ȡƫ�Ƶ�ַδ��8�ֽڲ���
	i = offset&0x7;
	//��ȡ��ʼ�����ַ
	addr = flash_logical_to_physical(blk, sector, offset - i);
	//��������Ϊд��8���ֽ�
	REG_SET_VAL(FTFE_FCCOB0, PGM8);
	//��˳��
	for (i = 0; i < num; i += 8) {

	}
}
