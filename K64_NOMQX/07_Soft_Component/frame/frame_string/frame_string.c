//==========================================================================
//�ļ����ƣ�frame_string.c
//���ܸ�Ҫ����֡ͨ��Э���ַ����������Դ�ļ�
//==========================================================================

#include "frame_string.h"

//==========================================================================
//��������: frame_string_send
//��������: �ַ������ͽ��:
//         StringSendSuccess:      �����ַ����ɹ�;
//         StringSendIllegalString:�Ƿ��ַ��������ַ������ȳ���255�ֽ�;
//         StringSendInfoFail:     ����֡��Ϣ�ṹ��ʧ��;
//����˵��: str:Ҫ���͵��ַ����׵�ַ
//���ܸ�Ҫ: ������'\0'�������ַ������Զ���װ֡��Ϣ�ṹ��
//��ע: ����ʹ����֡ͨ��Э��;�ַ������Ȳ��ܳ���255�ֽڣ�������'\0'��������'\0'
//==========================================================================
FrameStringSendResult frame_string_send(uint8* str) {
	FrameInfo info;	//Ҫ���͵�֡��Ϣ�ṹ��

	//��������
	info.type = FRAME_STRING_TYPE;
	//�����ַ�������ȡ���ȣ�����ֵ
	for (info.len = 0; str[info.len] != '\0' && info.len < 255; info.len++) {
		info.data[info.len] = str[info.len];
	}
	//����ַ��������Ƿ񳬹�255�ֽ�
	if (info.len == 255 && str[255] != '\0') {
		return StringSendIllegalString;
	}
	//����֡��Ϣ�ṹ��
	if (frame_send_info(info)) {
		//���ͳɹ�
		return StringSendSuccess;
	} else {
		//����ʧ��
		return StringSendInfoFail;
	}
}

//==========================================================================
//��������: frame_string_parse
//��������: �ַ����������:
//         StringParseSuccess:      �ַ��������ɹ�;
//         StringParseWrongType:    ֡���ʹ���;
//         StringParseIllegalString:�ַ����Ƿ�������ָ֡ʾ���ַ��������ڳ���'\0';
//����˵��: info:Ҫ������֡��Ϣ�ṹ��
//         str:�洢�ַ������׵�ַ
//���ܸ�Ҫ: ����֡��Ϣ�ṹ�壬�����ɹ�ʱ���洢��Ӧ�ַ���
//��ע: ���Զ����ַ�������ʱ���'\0'
//==========================================================================
FrameStringParseResult frame_string_parse(FrameInfo info, uint8* str) {
	uint8 i;	//�α�
	//����֡�����Ƿ���ȷ
	if (info.type != FRAME_STRING_TYPE) {
		return StringParseWrongType;
	}
	//�����Ƿ�Ƿ��ַ���
	for (i = 0; i < info.len; i++) {
		if (info.data[i] == '\0') {
			return StringParseIllegalString;
		}
	}
	//�洢�ַ���
	for (i = 0; i < info.len; i++) {
		str[i] = info.data[i];
	}
	//Ϊ�ַ������'\0';
	str[i] = '\0';
	return StringParseSuccess;
}
