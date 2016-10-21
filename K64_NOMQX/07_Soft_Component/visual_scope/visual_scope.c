//==============================================================
//�ļ����ƣ�visual_scope.c
//���ܸ�Ҫ�����ڵ��Թ���Դ�ļ�
//��Ȩ���У����ݴ�ѧ��˼����Ƕ��ʽ����(sumcu.suda.edu.cn)
//==============================================================

#include "visual_scope.h"

static unsigned short CRC_CHECK(unsigned char *Buf, unsigned char CRC_CNT) //����CRCУ��
{
	unsigned short CRC_Temp;
	unsigned char i, j;
	CRC_Temp = 0xffff;

	for (i = 0; i < CRC_CNT; i++)
	{
		CRC_Temp ^= Buf[i];
		for (j = 0; j < 8; j++)
		{
			if (CRC_Temp & 0x01)
				CRC_Temp = (CRC_Temp >> 1) ^ 0xa001;
			else
				CRC_Temp = CRC_Temp >> 1;
		}
	}
	return (CRC_Temp);
}

//============================================================================
//�������ƣ�visual_scope_output
//����˵����uartNo:���͵Ĵ��ں�
//		 data_out:��Ҫ���͵�4��ͨ��������ָ��
//�������أ���
//���ܸ�Ҫ�����ĸ�ͨ�����ݼ���У�鷢�ͳ�ȥ
//============================================================================
void visual_scope_output(uint8_t uartNo, float data_out[4])
{
	int32_t temp[4] = { 0 };

	uint32_t temp1[4] = { 0 };

	unsigned char databuf[10] = { 0 };
	unsigned char i;
	unsigned short CRC16 = 0;
	for (i = 0; i < 4; i++)
	{
		temp[i] = (int32_t) data_out[i];
		temp1[i] = (uint32_t) temp[i];
	}

	for (i = 0; i < 4; i++)
	{
		databuf[i * 2] = (unsigned char) (temp1[i] % 256);
		databuf[i * 2 + 1] = (unsigned char) (temp1[i] / 256);
	}

	CRC16 = CRC_CHECK(databuf, 8);
	databuf[8] = CRC16 % 256;
	databuf[9] = CRC16 / 256;

	for (i = 0; i < 10; i++)
		uart_send1(uartNo, databuf[i]);
}
