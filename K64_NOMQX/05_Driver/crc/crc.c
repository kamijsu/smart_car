//==========================================================================
//�ļ����ƣ�crc.c
//���ܸ�Ҫ��K64 CRC�ײ���������Դ�ļ�
//==========================================================================

#include "crc.h"

//��ǰ��������
static uint32 current_seed;
//���������������Ŀ��ƼĴ���ֵ
static uint32 ctrl_seed;
//����У������ʱ�Ŀ��ƼĴ���ֵ
static uint32 ctrl_data;

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
	//����ctrl_seed��WAS��1
	REG_SET_VAL(ctrl_seed, CRC_CTRL_WAS_MASK);
	if (width == CRC_WIDTH_32) {
		//��Э����Ϊ32λ��TCRC��1
		REG_SET_MASK(ctrl_seed, CRC_CTRL_TCRC_MASK);
		//����ctrl_data������Э�����趨TCRC
		REG_SET_VAL(ctrl_data, CRC_CTRL_TCRC_MASK);
	} else {
		REG_SET_VAL(ctrl_data, 0);
	}
	//����ctrl_seedдת������
	REG_SET_MASK(ctrl_seed, CRC_CTRL_TOT(write_trans));
	//����ctrl_data��дת�����ͣ���Ϊ��32λУ�飬����дת��ȡ��
	REG_SET_MASK(ctrl_data,
			CRC_CTRL_TOT(~write_trans) | CRC_CTRL_TOTR(read_trans));
	//����ctrl_data����
	if (complement_read) {
		REG_SET_MASK(ctrl_data, CRC_CTRL_FXOR_MASK);
	}
	//���ÿ��ƼĴ���Ϊctrl_seed
	REG_SET_VAL(CRC_CTRL, ctrl_seed);
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
//��������: crc_cal
//��������: CRCУ���룬16λЭ��ʱ����16λ��Ч
//����˵��: data:У�����ݵ��׵�ַ
//         len:У�����ݵ��ֽ���
//���ܸ�Ҫ: У��һ�����ȵ����ݣ��������ɵ�CRCУ����
//��ע: У��������жϰ�ȫ��;
//     Ӳ��CRC���ʺϿ�ʽУ�����ݣ���ʽУ������ʱЧ�ʵ��£����������CRC������ʽУ��
//==========================================================================
uint32 crc_cal(uint8* data, uint32 len) {
	uint32 ctrl_old;	//�ɿ��ƼĴ���ֵ
	uint32 data_old;	//�����ݼĴ���ֵ
	uint32* data32;		//32λ����ָ�룬����32λУ������
	uint32 result;		//CRCУ����
	uint32 ctrl_backup;	//���ڶ�ȡ��д�����ݼĴ����Ŀ��ƼĴ���ֵ

	//WAS��TCRC��1
	ctrl_backup = CRC_CTRL_WAS_MASK | CRC_CTRL_TCRC_MASK;
	//��ȡ�ɿ��ƼĴ���ֵ
	ctrl_old = CRC_CTRL;
	//���ÿ��ƼĴ���Ϊctrl_backup
	REG_SET_VAL(CRC_CTRL, ctrl_backup);
	//��ȡ�����ݼĴ���ֵ
	data_old = CRC_DATA;
	//���ÿ��ƼĴ���Ϊctrl_seed
	REG_SET_VAL(CRC_CTRL, ctrl_seed);
	//����������
	REG_SET_VAL(CRC_DATA, current_seed);
	//���ÿ��ƼĴ���Ϊctrl_data
	REG_SET_VAL(CRC_CTRL, ctrl_data);
	//������ָ��ת��Ϊ32λ
	data32 = (uint32*) data;
	//32λУ������
	for (; len >= 4; len -= 4) {
		REG_SET_VAL(CRC_DATA, *(data32++));
	}
	//������ָ��ת��Ϊ8λ
	data = (uint8*) data32;
	//У��ʣ������
	switch (len) {
	case 3:	//ʣ��3�ֽڣ��Ȱ�16λУ�����ݣ��ٰ�8λУ������
		REG_SET_VAL(CRC_DATAL, *(uint16* )data);
		REG_SET_VAL(CRC_DATALL, *(data + 2));
		break;
	case 2:	//ʣ��2�ֽڣ���16λУ������
		REG_SET_VAL(CRC_DATAL, *(uint16* )data);
		break;
	case 1:	//ʣ��1�ֽڣ���8λУ������
		REG_SET_VAL(CRC_DATALL, *data);
		break;
	}
	//��ȡУ����
	if (REG_GET_MASK(ctrl_data, CRC_CTRL_TCRC_MASK)) {
		//32λЭ��ʱ��У����Ϊ���ݼĴ���ֵ
		result = CRC_DATA;
	} else {
		//16λЭ��ʱ�������Ƿ���й��ֽڶ��û���ѡ���ȡ���ݼĴ�����/�Ͱ���
		if (REG_GET_SHIFT(ctrl_data, (CRC_CTRL_TOTR_SHIFT + 1))) {
			//���й��ֽڶ��û�ʱ����ȡ�߰���
			result = CRC_DATAH;
		} else {
			//û�н��й��ֽڶ��û�ʱ����ȡ�Ͱ���
			result = CRC_DATAL;
		}
	}
	//���ÿ��ƼĴ���Ϊctrl_backup
	REG_SET_VAL(CRC_CTRL, ctrl_backup);
	//�ָ����ݼĴ�����ֵ
	REG_SET_VAL(CRC_DATA, data_old);
	//�ָ����ƼĴ�����ֵ
	REG_SET_VAL(CRC_CTRL, ctrl_old);
	//����У����
	return result;
}
