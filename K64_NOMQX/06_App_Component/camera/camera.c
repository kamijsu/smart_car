//==========================================================================
//文件名称：camera.c
//功能概要：摄像头应用驱动程序源文件
//==========================================================================

#include "camera.h"

//定义摄像头的寄存器地址，用于配置摄像头属性
#define OV7725_GAIN      0x00
#define OV7725_BLUE      0x01
#define OV7725_RED       0x02
#define OV7725_GREEN     0x03
#define OV7725_BAVG      0x05
#define OV7725_GAVG      0x06
#define OV7725_RAVG      0x07
#define OV7725_AECH      0x08
#define OV7725_COM2      0x09
#define OV7725_PID       0x0A
#define OV7725_VER       0x0B
#define OV7725_COM3      0x0C
#define OV7725_COM4      0x0D
#define OV7725_COM5      0x0E
#define OV7725_COM6      0x0F
#define OV7725_AEC       0x10
#define OV7725_CLKRC     0x11
#define OV7725_COM7      0x12
#define OV7725_COM8      0x13
#define OV7725_COM9      0x14
#define OV7725_COM10     0x15
#define OV7725_REG16     0x16
#define OV7725_HSTART    0x17
#define OV7725_HSIZE     0x18
#define OV7725_VSTRT     0x19
#define OV7725_VSIZE     0x1A
#define OV7725_PSHFT     0x1B
#define OV7725_MIDH      0x1C
#define OV7725_MIDL      0x1D
#define OV7725_LAEC      0x1F
#define OV7725_COM11     0x20
#define OV7725_BDBase    0x22
#define OV7725_BDMStep   0x23
#define OV7725_AEW       0x24
#define OV7725_AEB       0x25
#define OV7725_VPT       0x26
#define OV7725_REG28     0x28
#define OV7725_HOutSize  0x29
#define OV7725_EXHCH     0x2A
#define OV7725_EXHCL     0x2B
#define OV7725_VOutSize  0x2C
#define OV7725_ADVFL     0x2D
#define OV7725_ADVFH     0x2E
#define OV7725_YAVE      0x2F
#define OV7725_LumHTh    0x30
#define OV7725_LumLTh    0x31
#define OV7725_HREF      0x32
#define OV7725_DM_LNL    0x33
#define OV7725_DM_LNH    0x34
#define OV7725_ADoff_B   0x35
#define OV7725_ADoff_R   0x36
#define OV7725_ADoff_Gb  0x37
#define OV7725_ADoff_Gr  0x38
#define OV7725_Off_B     0x39
#define OV7725_Off_R     0x3A
#define OV7725_Off_Gb    0x3B
#define OV7725_Off_Gr    0x3C
#define OV7725_COM12     0x3D
#define OV7725_COM13     0x3E
#define OV7725_COM14     0x3F
#define OV7725_COM16     0x41
#define OV7725_TGT_B     0x42
#define OV7725_TGT_R     0x43
#define OV7725_TGT_Gb    0x44
#define OV7725_TGT_Gr    0x45
#define OV7725_LC_CTR    0x46
#define OV7725_LC_XC     0x47
#define OV7725_LC_YC     0x48
#define OV7725_LC_COEF   0x49
#define OV7725_LC_RADI   0x4A
#define OV7725_LC_COEFB  0x4B
#define OV7725_LC_COEFR  0x4C
#define OV7725_FixGain   0x4D
#define OV7725_AREF1     0x4F
#define OV7725_AREF6     0x54
#define OV7725_UFix      0x60
#define OV7725_VFix      0x61
#define OV7725_AWBb_blk  0x62
#define OV7725_AWB_Ctrl0 0x63
#define OV7725_DSP_Ctrl1 0x64
#define OV7725_DSP_Ctrl2 0x65
#define OV7725_DSP_Ctrl3 0x66
#define OV7725_DSP_Ctrl4 0x67
#define OV7725_AWB_bias  0x68
#define OV7725_AWBCtrl1  0x69
#define OV7725_AWBCtrl2  0x6A
#define OV7725_AWBCtrl3  0x6B
#define OV7725_AWBCtrl4  0x6C
#define OV7725_AWBCtrl5  0x6D
#define OV7725_AWBCtrl6  0x6E
#define OV7725_AWBCtrl7  0x6F
#define OV7725_AWBCtrl8  0x70
#define OV7725_AWBCtrl9  0x71
#define OV7725_AWBCtrl10 0x72
#define OV7725_AWBCtrl11 0x73
#define OV7725_AWBCtrl12 0x74
#define OV7725_AWBCtrl13 0x75
#define OV7725_AWBCtrl14 0x76
#define OV7725_AWBCtrl15 0x77
#define OV7725_AWBCtrl16 0x78
#define OV7725_AWBCtrl17 0x79
#define OV7725_AWBCtrl18 0x7A
#define OV7725_AWBCtrl19 0x7B
#define OV7725_AWBCtrl20 0x7C
#define OV7725_AWBCtrl21 0x7D
#define OV7725_GAM1      0x7E
#define OV7725_GAM2      0x7F
#define OV7725_GAM3      0x80
#define OV7725_GAM4      0x81
#define OV7725_GAM5      0x82
#define OV7725_GAM6      0x83
#define OV7725_GAM7      0x84
#define OV7725_GAM8      0x85
#define OV7725_GAM9      0x86
#define OV7725_GAM10     0x87
#define OV7725_GAM11     0x88
#define OV7725_GAM12     0x89
#define OV7725_GAM13     0x8A
#define OV7725_GAM14     0x8B
#define OV7725_GAM15     0x8C
#define OV7725_SLOP      0x8D
#define OV7725_DNSTh     0x8E
#define OV7725_EDGE0     0x8F
#define OV7725_EDGE1     0x90
#define OV7725_DNSOff    0x91
#define OV7725_EDGE2     0x92
#define OV7725_EDGE3     0x93
#define OV7725_MTX1      0x94
#define OV7725_MTX2      0x95
#define OV7725_MTX3      0x96
#define OV7725_MTX4      0x97
#define OV7725_MTX5      0x98
#define OV7725_MTX6      0x99
#define OV7725_MTX_Ctrl  0x9A
#define OV7725_BRIGHT    0x9B
#define OV7725_CNST      0x9C
#define OV7725_UVADJ0    0x9E
#define OV7725_UVADJ1    0x9F
#define OV7725_SCAL0     0xA0
#define OV7725_SCAL1     0xA1
#define OV7725_SCAL2     0xA2
#define OV7725_SDE       0xA6
#define OV7725_USAT      0xA7
#define OV7725_VSAT      0xA8
#define OV7725_HUECOS    0xA9
#define OV7725_HUESIN    0xAA
#define OV7725_SIGN      0xAB
#define OV7725_DSPAuto   0xAC

static void camera_sccb_write(uint8* reg_val) {
	//发送寄存器-值对直到成功
	while (i2c_master_send(CAMERA_I2C_MOD, I2C_ADDR_MODE_BITS_7,
	CAMERA_ADDR, reg_val, 2) != I2CMasterSuccess) {
	}
}

uint8 camera_sccb_read(uint8 reg) {
	I2CMasterResult result;
	uint8 val;	//寄存器值

	for (;;) {
		//发送寄存器失败时，重新发送
		if ((result = i2c_master_send(CAMERA_I2C_MOD, I2C_ADDR_MODE_BITS_7,
		CAMERA_ADDR, &reg, 1)) != I2CMasterSuccess) {
			uart_printf(1,"警告，发送错误:%d\r\n",result);
			continue;
		}
		//读取寄存器值成功时，准备返回值
		if ((result = i2c_master_re(CAMERA_I2C_MOD, I2C_ADDR_MODE_BITS_7,
		CAMERA_ADDR, &val, 1)) == I2CMasterSuccess) {
			break;
		}
		uart_printf(1,"警告，接收错误:%d\r\n",result);
	}
	//返回寄存器值
	return val;
}

void camera_init(uint8* raw_img) {
	//初始化数据信号引脚
	gpio_init(CAMERA_D0, GPIO_DIR_INPUT, GPIO_LEVEL_UNKNOWN);
	gpio_init(CAMERA_D1, GPIO_DIR_INPUT, GPIO_LEVEL_UNKNOWN);
	gpio_init(CAMERA_D2, GPIO_DIR_INPUT, GPIO_LEVEL_UNKNOWN);
	gpio_init(CAMERA_D3, GPIO_DIR_INPUT, GPIO_LEVEL_UNKNOWN);
	gpio_init(CAMERA_D4, GPIO_DIR_INPUT, GPIO_LEVEL_UNKNOWN);
	gpio_init(CAMERA_D5, GPIO_DIR_INPUT, GPIO_LEVEL_UNKNOWN);
	gpio_init(CAMERA_D6, GPIO_DIR_INPUT, GPIO_LEVEL_UNKNOWN);
	gpio_init(CAMERA_D7, GPIO_DIR_INPUT, GPIO_LEVEL_UNKNOWN);

	//初始化场中断信号引脚
	gpio_init(CAMERA_VSYNC, GPIO_DIR_INPUT, GPIO_LEVEL_LOW);
	//开启场中断信号引脚的无源滤波
	gpio_set_passive_filter(CAMERA_VSYNC, true);

	//初始化时钟信号引脚
	gpio_init(CAMERA_PCLK, GPIO_DIR_INPUT, GPIO_LEVEL_LOW);
	//时钟信号上升沿触发DMA请求
	gpio_enable_dma(CAMERA_PCLK, GPIO_DMA_RISING_EDGE);

	//初始化DMA通道
	dma_init(CAMERA_DMA_CH,			//设置通道
			CAMERA_DMA_REQ,			//设置请求源
			DMA_MODE_NORMAL,		//设置为正常模式
			1,						//每次副循环传输1个字节
			CAMERA_RAW_IMG_BYTES,	//主循环迭代次数等于原始图像字节数
			CAMERA_DMA_SRC_ADDR,	//设置源地址
			1,						//每次从源地址读取1个字节
			0,						//源地址被读取后不偏移
			DMA_MODULO_DISABLED,	//禁用模数功能
			0,						//主循环完成后源地址不偏移
			(uint32) raw_img,		//设置目标地址
			1,						//每次向目标地址写入1个字节
			1,						//目标地址被写入后偏移1个字节
			DMA_MODULO_DISABLED,	//禁用模数功能
			-CAMERA_RAW_IMG_BYTES,	//主循环完成后目标地址恢复为初始目标地址
			true);					//使能主循环完成后自动不接收DMA请求

	//初始化I2C模块
	i2c_init(CAMERA_I2C_MOD, CAMERA_I2C_MUL, CAMERA_I2C_ICR,
	CAMERA_I2C_ADDR_MODE, CAMERA_I2C_ADDR, CAMERA_I2C_ENABLE_GENERAL_CALL_ADDR);
}
