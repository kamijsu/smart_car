//==========================================================================
//文件名称：camera.c
//功能概要：摄像头应用驱动程序源文件
//==========================================================================

#include "camera.h"

static void camera_sccb_write(uint8* reg_val) {
	//发送寄存器-值对直到成功
	while (i2c_master_send(CAMERA_I2C_MOD, I2C_ADDR_MODE_BITS_7,
	CAMERA_I2C_ADDR, reg_val, 2) != I2CMasterSuccess) {
	}
}

static uint8 camera_sccb_read(uint8 reg) {
	uint8 val;	//寄存器值

	for (;;) {
		//发送寄存器失败时，重新发送
		if (i2c_master_send(CAMERA_I2C_MOD, I2C_ADDR_MODE_BITS_7,
		CAMERA_I2C_ADDR, &reg, 1) != I2CMasterSuccess) {
			continue;
		}
		//读取寄存器值成功时，准备返回值
		if (i2c_master_re(CAMERA_I2C_MOD, I2C_ADDR_MODE_BITS_7,
		CAMERA_I2C_ADDR, &val, 1) == I2CMasterSuccess) {
			break;
		}
	}
	//返回寄存器值
	return val;
}
