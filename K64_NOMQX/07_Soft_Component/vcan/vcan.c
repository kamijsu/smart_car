//==========================================================================
//�ļ����ƣ�vcan.c
//���ܸ�Ҫ��ɽ��๦�ܵ��������������Դ�ļ�
//==========================================================================

#include "vcan.h"
#include "camera.h"

//==========================================================================
//��������: vcan_send_raw_img
//��������: ��
//����˵��: raw_img:ԭʼͼ���׵�ַ
//���ܸ�Ҫ: ��ɽ��๦�ܵ�����������ʾ�ɼ���ͼ����Ϣ
//==========================================================================
void vcan_send_raw_img(uint8* raw_img) {
	//ɽ��๦�ܵ���������ʹ�õ�����
	uint8 cmdf[2] = { VCAN_CMD_IMG, ~VCAN_CMD_IMG };
	uint8 cmdr[2] = { ~VCAN_CMD_IMG, VCAN_CMD_IMG };

	//��������
	uart_sendN(VCAN_UART_MOD, cmdf, sizeof(cmdf));
	//����ͼ��
	uart_sendN(VCAN_UART_MOD, raw_img, CAMERA_RAW_IMG_BYTES);
	//��������
	uart_sendN(VCAN_UART_MOD, cmdr, sizeof(cmdr));
}

//==========================================================================
//��������: vcan_send_img
//��������: ��
//����˵��: img:ͼ���׵�ַ
//���ܸ�Ҫ: ��ɽ��๦�ܵ�����������ʾͼ����Ϣ
//==========================================================================
void vcan_send_img(uint8* img) {
	//ɽ��๦�ܵ���������ʹ�õ�����
	uint8 cmdf[2] = { VCAN_CMD_IMG, ~VCAN_CMD_IMG };
	uint8 cmdr[2] = { ~VCAN_CMD_IMG, VCAN_CMD_IMG };

	//��������
	uart_sendN(VCAN_UART_MOD, cmdf, sizeof(cmdf));
	//����ͼ��
	uart_sendN(VCAN_UART_MOD, img, CAMERA_IMG_HEIGHT * CAMERA_IMG_WIDTH);
	//��������
	uart_sendN(VCAN_UART_MOD, cmdr, sizeof(cmdr));
}
