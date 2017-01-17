//==========================================================================
//�ļ����ƣ�frame_data.c
//���ܸ�Ҫ����֡ͨ��Э�������������Դ�ļ�
//==========================================================================

#include "frame_data.h"

//==========================================================================
//��������: frame_data_send
//��������: true:�������ݳɹ�; false:�Ƿ�������Ϣ����
//����˵��: data:Ҫ���͵�֡������Ϣ�ṹ��
//���ܸ�Ҫ: ����֡������Ϣ�ṹ�壬�Զ���װ֡��Ϣ�ṹ��
//��ע: ����ʹ����֡ͨ��Э��
//==========================================================================
bool frame_data_send(FrameDataInfo data) {
	FrameInfo info;	//Ҫ���͵�֡��Ϣ�ṹ��
	uint8 i;		//�α�
	//���������Ϣ�����Ƿ�Ϸ�
	if (data.len > 253) {
		return false;
	}
	//��������
	info.type = FRAME_DATA_TYPE;
	//���ó���
	info.len = data.len + 2;
	//��������
	info.data[0] = data.type;
	info.data[1] = data.len;
	for (i = 0; i < data.len; i++) {
		info.data[i + 2] = data.data[i];
	}
	//����֡��Ϣ�ṹ��
	frame_send_info(info);
	return true;
}

//==========================================================================
//��������: frame_data_parse
//��������: ���ݽ������:
//         DataParseSuccess:       ���ݽ����ɹ�;
//         DataParseWrongType:     ֡���ʹ���;
//         DataParseIllegalInfoLen:֡���ݳ��ȷǷ�;
//         DataParseIllegalDataLen:������Ϣ���ȷǷ�;
//����˵��: info:Ҫ������֡��Ϣ�ṹ��
//         data:�洢֡������Ϣ�ṹ����׵�ַ
//���ܸ�Ҫ: ����֡��Ϣ�ṹ�壬�����ɹ�ʱ���洢��Ӧ֡������Ϣ�ṹ��
//==========================================================================
FrameDataParseResult frame_data_parse(FrameInfo info, FrameDataInfo* data) {
	uint8 i;	//�α�

	//����֡�����Ƿ���ȷ
	if (info.type != FRAME_DATA_TYPE) {
		return DataParseWrongType;
	}
	//����֡���ݳ����Ƿ�Ϸ�
	if (info.len < 2) {
		return DataParseIllegalInfoLen;
	}
	//����������Ϣ�����Ƿ�Ϸ�
	if (info.data[1] != info.len - 2) {
		return DataParseIllegalDataLen;
	}
	//�洢֡������Ϣ�ṹ��
	data->type = info.data[0];
	data->len = info.data[1];
	for (i = 2; i < info.len; i++) {
		data->data[i - 2] = info.data[i];
	}
	return DataParseSuccess;
}
