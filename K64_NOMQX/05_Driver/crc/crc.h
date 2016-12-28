//==========================================================================
//�ļ����ƣ�crc.h
//���ܸ�Ҫ��K64 CRC�ײ���������ͷ�ļ�
//==========================================================================

#ifndef _CRC_H
#define _CRC_H

//��������ͷ�ļ�
#include "common.h"

//����CRCЭ����
#define CRC_WIDTH_16	(0)	//16λCRCЭ��
#define CRC_WIDTH_32	(1)	//32λCRCЭ��

//����CRCת������
#define CRC_TRANS_NO	(0)	//��ת�ã�[31:0]���[31:0]
#define CRC_TRANS_BITS	(1)	//����λת�ã�[31:0]���[24:31]��[16:23]��[8:15]��[0:7]
#define CRC_TRANS_BOTH	(2)	//��λ���ֽ�ת�ã�[31:0]���[0:31]
#define CRC_TRANS_BYTES	(3)	//�����ֽ�ת�ã�[31:0]���[7:0]��[15:8]��[23:16]��[31:24]

//����Ԥ���CRCЭ�飬��Э��ע������Ϊ�����������ɶ���ʽ��д����ת�����͡�������ת�����͡��Ƿ񲹶�
#define CRC_CRC16_IBM			(0)	//0x0000		0x8005		BITS	BOTH	false
#define CRC_CRC16_MAXIM			(1)	//0x0000		0x8005		BITS	BOTH	true
#define CRC_CRC16_USB			(2)	//0xFFFF		0x8005		BITS	BOTH	true
#define CRC_CRC16_MODBUS		(3)	//0xFFFF		0x8005		BITS	BOTH	false
#define CRC_CRC16_CCITT			(4)	//0x0000		0x1021		BITS	BOTH	false
#define CRC_CRC16_CCITT_FALSE	(5)	//0xFFFF		0x1021		NO		NO		false
#define CRC_CRC16_X25			(6)	//0xFFFF		0x1021		BITS	BOTH	true
#define CRC_CRC16_XMODEM		(7)	//0x0000		0x1021		NO		NO		false
#define CRC_CRC16_DNP			(8)	//0x0000		0x3D65		BITS	BOTH	true
#define CRC_CRC32				(9)	//0xFFFFFFFF	0x04C11DB7	BITS	BOTH	true
#define CRC_CRC32_MPEG2			(10)//0xFFFFFFFF	0x04C11DB7	NO		NO		false

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
		uint8 read_trans, bool complement_read);

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
void crc_init_protocol(uint8 protocol_type);

//==========================================================================
//��������: crc_cal_block
//��������: CRCУ���룬16λЭ��ʱ����16λ��Ч
//����˵��: data:У�����ݵ��׵�ַ
//         len:У�����ݵ��ֽ���
//���ܸ�Ҫ: У��һ�����ȵ����ݣ��������ɵ�CRCУ����
//==========================================================================
uint32 crc_cal_block(uint8* data, uint32 len);

#endif
