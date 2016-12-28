//==========================================================================
//�ļ����ƣ�crc.c
//���ܸ�Ҫ��K64 CRC�ײ���������Դ�ļ�
//==========================================================================

#include "crc.h"

//��ǰ��������
static uint32 current_seed;
//���ڱ��ݺͻָ����ݼĴ���ʱ�Ŀ��ƼĴ���ֵ
static uint32 ctrl_backup;
//���������������Ŀ��ƼĴ���ֵ
static uint32 ctrl_seed;
//����У������ʱ�Ŀ��ƼĴ���ֵ
static uint32 ctrl_data;

//==========================================================================
//��������: crc_get
//��������: CRCУ���룬16λЭ��ʱ����16λ��Ч
//����˵��: ��
//���ܸ�Ҫ: ��ȡ��ǰ��CRCУ����
//==========================================================================
static uint32 crc_get() {
	uint32 result;	//��ǰCRCУ����
	if (REG_GET_MASK(CRC_CTRL, CRC_CTRL_TCRC_MASK)) {
		//32λЭ��ʱ��У����Ϊ���ݼĴ���ֵ
		result = CRC_DATA;
	} else {
		//16λЭ��ʱ�������Ƿ���й��ֽڶ��û���ѡ���ȡ���ݼĴ�����/�Ͱ���
		if (REG_GET_SHIFT(CRC_CTRL, (CRC_CTRL_TOTR_SHIFT + 1))) {
			//���й��ֽڶ��û�ʱ����ȡ�߰���
			result = CRC_DATAH;
		} else {
			//û�н��й��ֽڶ��û�ʱ����ȡ�Ͱ���
			result = CRC_DATAL;
		}
	}
	//����CRCУ����
	return result;
}

//==========================================================================
//��������: crc_init
//��������: ��
//����˵��: width:CRCЭ����:
//               CRC_WIDTH_16:16λCRCЭ��;
//               CRC_WIDTH_32:32λCRCЭ��;
//         seed:������������ʼCRCֵ
//         poly:���ɶ���ʽ
//         write_trans:д����ʱ��ת������:
//                     CRC_TRANS_NO:   ��ת��;
//                     CRC_TRANS_BITS: ����λת��;
//                     CRC_TRANS_BOTH: ��λ���ֽ�ת��;
//                     CRC_TRANS_BYTES:�����ֽ�ת��;
//         read_trans:������ʱ��ת�����ͣ���ѡ����ͬwrite_trans
//         complement_read:�Ƿ���в���
//���ܸ�Ҫ: ��ʼ��CRCģ��
//��ע: ��������У������0xFFFFFFFF��0xFFFF�����������
//==========================================================================
void crc_init(uint8 width, uint32 seed, uint32 poly, uint8 write_trans,
		uint8 read_trans, bool complement_read) {
	//��CRCģ��ʱ����
	REG_SET_MASK(SIM_SCGC6, SIM_SCGC6_CRC_MASK);
	//����ctrl_backup��WAS��1
	ctrl_backup = CRC_CTRL_WAS_MASK;
	if (width == CRC_WIDTH_32) {
		//��Э����Ϊ32λ��TCRC��1
		ctrl_backup |= CRC_CTRL_TCRC_MASK;
		//����ctrl_data������Э�����趨TCRC
		ctrl_data = CRC_CTRL_TCRC_MASK;
	} else {
		ctrl_data = 0;
	}
	//����ctrl_seed�������дת������
	ctrl_seed = ctrl_backup | CRC_CTRL_TOT(write_trans);
	//����ctrl_data��дת������
	ctrl_data |= CRC_CTRL_TOT(~write_trans) | CRC_CTRL_TOTR(read_trans);
	//����ctrl_data����
	if (complement_read) {
		ctrl_data |= CRC_CTRL_FXOR_MASK;
	}

	//���ÿ��ƼĴ���Ϊ
	//�������ɶ���ʽ
	REG_SET_VAL(CRC_GPOLY, poly);
	//����������
	current_seed = seed;
}

//==========================================================================
//��������: crc_init_protocol
//��������: ��
//����˵��: protocol_type:CRCЭ������:
//                       CRC_CRC16_IBM;
//                       CRC_CRC16_MAXIM;
//                       CRC_CRC16_USB;
//                       CRC_CRC16_MODBUS;
//                       CRC_CRC16_CCITT;
//                       CRC_CRC16_CCITT_FALSE;
//                       CRC_CRC16_X25;
//                       CRC_CRC16_XMODEM;
//                       CRC_CRC16_DNP;
//                       CRC_CRC32;
//                       CRC_CRC32_MPEG2;
//���ܸ�Ҫ: ��Ԥ���Э���ʼ��CRCģ��
//��ע: ��Э��������궨��
//==========================================================================
void crc_init_protocol(uint8 protocol_type) {
	//����Э�����ͳ�ʼ��CRCģ��
	switch (protocol_type) {
	case CRC_CRC16_IBM:
		crc_init(CRC_WIDTH_16, 0x0000, 0x8005, CRC_TRANS_BITS, CRC_TRANS_BOTH,
		false);
		break;
	case CRC_CRC16_MAXIM:
		crc_init(CRC_WIDTH_16, 0x0000, 0x8005, CRC_TRANS_BITS, CRC_TRANS_BOTH,
		true);
		break;
	case CRC_CRC16_USB:
		crc_init(CRC_WIDTH_16, 0xFFFF, 0x8005, CRC_TRANS_BITS, CRC_TRANS_BOTH,
		true);
		break;
	case CRC_CRC16_MODBUS:
		crc_init(CRC_WIDTH_16, 0xFFFF, 0x8005, CRC_TRANS_BITS, CRC_TRANS_BOTH,
		false);
		break;
	case CRC_CRC16_CCITT:
		crc_init(CRC_WIDTH_16, 0x0000, 0x1021, CRC_TRANS_BITS, CRC_TRANS_BOTH,
		false);
		break;
	case CRC_CRC16_CCITT_FALSE:
		crc_init(CRC_WIDTH_16, 0xFFFF, 0x1021, CRC_TRANS_NO, CRC_TRANS_NO,
		false);
		break;
	case CRC_CRC16_X25:
		crc_init(CRC_WIDTH_16, 0xFFFF, 0x1021, CRC_TRANS_BITS, CRC_TRANS_BOTH,
		true);
		break;
	case CRC_CRC16_XMODEM:
		crc_init(CRC_WIDTH_16, 0x0000, 0x1021, CRC_TRANS_NO, CRC_TRANS_NO,
		false);
		break;
	case CRC_CRC16_DNP:
		crc_init(CRC_WIDTH_16, 0x0000, 0x3D65, CRC_TRANS_BITS, CRC_TRANS_BOTH,
		true);
		break;
	case CRC_CRC32:
		crc_init(CRC_WIDTH_32, 0xFFFFFFFF, 0x04C11DB7, CRC_TRANS_BITS,
		CRC_TRANS_BOTH, true);
		break;
	case CRC_CRC32_MPEG2:
		crc_init(CRC_WIDTH_32, 0xFFFFFFFF, 0x04C11DB7, CRC_TRANS_NO,
		CRC_TRANS_NO, false);
		break;
	}
}

//==========================================================================
//��������: crc_cal_block
//��������: CRCУ���룬16λЭ��ʱ����16λ��Ч
//����˵��: data:У�����ݵ��׵�ַ
//         len:У�����ݵ��ֽ���
//���ܸ�Ҫ: У��һ�����ȵ����ݣ��������ɵ�CRCУ����
//==========================================================================
uint32 crc_cal_block(uint8* data, uint32 len) {

}
