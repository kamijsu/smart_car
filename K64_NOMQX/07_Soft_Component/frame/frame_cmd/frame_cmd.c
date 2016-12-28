//==========================================================================
//�ļ����ƣ�frame_cmd.c
//���ܸ�Ҫ����֡ͨ��Э�������������Դ�ļ�
//==========================================================================

#include "frame_cmd.h"

//==========================================================================
//��������: frame_cmd_send
//��������: ����ͽ��:
//         CmdSendSuccess:   ��������ɹ�;
//         CmdSendIllegalLen:�Ƿ�������Ϣ����;
//         CmdSendInfoFail:  ����֡��Ϣ�ṹ��ʧ��;
//����˵��: cmd:Ҫ���͵�֡������Ϣ�ṹ��
//���ܸ�Ҫ: ����֡������Ϣ�ṹ�壬�Զ���װ֡��Ϣ�ṹ��
//��ע: ����ʹ����֡ͨ��Э��
//==========================================================================
FrameCmdSendResult frame_cmd_send(FrameCmdInfo cmd) {
	FrameInfo info;	//Ҫ���͵�֡��Ϣ�ṹ��
	uint8 i;		//�α�
	//���������Ϣ�����Ƿ�Ϸ�
	if (cmd.len > 253) {
		return CmdSendIllegalLen;
	}
	//��������
	info.type = FRAME_CMD_TYPE;
	//���ó���
	info.len = cmd.len + 2;
	//��������
	info.data[0] = cmd.type;
	info.data[1] = cmd.len;
	for (i = 0; i < cmd.len; i++) {
		info.data[i + 2] = cmd.data[i];
	}
	//����֡��Ϣ�ṹ��
	if (frame_send_info(info)) {
		//���ͳɹ�
		return CmdSendSuccess;
	} else {
		//����ʧ��
		return CmdSendInfoFail;
	}
}

//==========================================================================
//��������: frame_cmd_parse
//��������: ����������:
//         CmdParseSuccess:       ��������ɹ�;
//         CmdParseWrongType:     ֡���ʹ���;
//         CmdParseIllegalInfoLen:֡���ݳ��ȷǷ�;
//         CmdParseIllegalCmdLen: ������Ϣ���ȷǷ�;
//����˵��: info:Ҫ������֡��Ϣ�ṹ��
//         cmd:�洢֡������Ϣ�ṹ����׵�ַ
//���ܸ�Ҫ: ����֡��Ϣ�ṹ�壬�����ɹ�ʱ���洢��Ӧ֡������Ϣ�ṹ��
//==========================================================================
FrameCmdParseResult frame_cmd_parse(FrameInfo info, FrameCmdInfo* cmd) {
	uint8 i;	//�α�

	//����֡�����Ƿ���ȷ
	if (info.type != FRAME_CMD_TYPE) {
		return CmdParseWrongType;
	}
	//����֡���ݳ����Ƿ�Ϸ�
	if (info.len < 2) {
		return CmdParseIllegalInfoLen;
	}
	//����������Ϣ�����Ƿ�Ϸ�
	if (info.data[1] != info.len - 2) {
		return CmdParseIllegalCmdLen;
	}
	//�洢֡������Ϣ�ṹ��
	cmd->type = info.data[0];
	cmd->len = info.data[1];
	for (i = 2; i < info.len; i++) {
		cmd->data[i - 2] = info.data[i];
	}
	return CmdParseSuccess;
}
