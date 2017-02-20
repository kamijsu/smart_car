//==========================================================================
//�ļ����ƣ�camera.c
//���ܸ�Ҫ������ͷӦ����������Դ�ļ�
//==========================================================================

#include "camera.h"

static void camera_sccb_write(uint8* reg_val) {
	//���ͼĴ���-ֵ��ֱ���ɹ�
	while (i2c_master_send(CAMERA_I2C_MOD, I2C_ADDR_MODE_BITS_7,
	CAMERA_I2C_ADDR, reg_val, 2) != I2CMasterSuccess) {
	}
}

static uint8 camera_sccb_read(uint8 reg) {
	uint8 val;	//�Ĵ���ֵ

	for (;;) {
		//���ͼĴ���ʧ��ʱ�����·���
		if (i2c_master_send(CAMERA_I2C_MOD, I2C_ADDR_MODE_BITS_7,
		CAMERA_I2C_ADDR, &reg, 1) != I2CMasterSuccess) {
			continue;
		}
		//��ȡ�Ĵ���ֵ�ɹ�ʱ��׼������ֵ
		if (i2c_master_re(CAMERA_I2C_MOD, I2C_ADDR_MODE_BITS_7,
		CAMERA_I2C_ADDR, &val, 1) == I2CMasterSuccess) {
			break;
		}
	}
	//���ؼĴ���ֵ
	return val;
}
