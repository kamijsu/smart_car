//==========================================================================
//文件名称：camera.c
//功能概要：摄像头应用驱动程序源文件
//==========================================================================

#include "camera.h"

//定义摄像头的寄存器地址，用于配置摄像头属性
#define CAMERA_REG_GAIN      0x00
#define CAMERA_REG_BLUE      0x01
#define CAMERA_REG_RED       0x02
#define CAMERA_REG_GREEN     0x03
#define CAMERA_REG_BAVG      0x05
#define CAMERA_REG_GAVG      0x06
#define CAMERA_REG_RAVG      0x07
#define CAMERA_REG_AECH      0x08
#define CAMERA_REG_COM2      0x09
#define CAMERA_REG_PID       0x0A
#define CAMERA_REG_VER       0x0B
#define CAMERA_REG_COM3      0x0C
#define CAMERA_REG_COM4      0x0D
#define CAMERA_REG_COM5      0x0E
#define CAMERA_REG_COM6      0x0F
#define CAMERA_REG_AEC       0x10
#define CAMERA_REG_CLKRC     0x11
#define CAMERA_REG_COM7      0x12
#define CAMERA_REG_COM8      0x13
#define CAMERA_REG_COM9      0x14
#define CAMERA_REG_COM10     0x15
#define CAMERA_REG_REG16     0x16
#define CAMERA_REG_HSTART    0x17
#define CAMERA_REG_HSIZE     0x18
#define CAMERA_REG_VSTRT     0x19
#define CAMERA_REG_VSIZE     0x1A
#define CAMERA_REG_PSHFT     0x1B
#define CAMERA_REG_MIDH      0x1C
#define CAMERA_REG_MIDL      0x1D
#define CAMERA_REG_LAEC      0x1F
#define CAMERA_REG_COM11     0x20
#define CAMERA_REG_BDBase    0x22
#define CAMERA_REG_BDMStep   0x23
#define CAMERA_REG_AEW       0x24
#define CAMERA_REG_AEB       0x25
#define CAMERA_REG_VPT       0x26
#define CAMERA_REG_REG28     0x28
#define CAMERA_REG_HOutSize  0x29
#define CAMERA_REG_EXHCH     0x2A
#define CAMERA_REG_EXHCL     0x2B
#define CAMERA_REG_VOutSize  0x2C
#define CAMERA_REG_ADVFL     0x2D
#define CAMERA_REG_ADVFH     0x2E
#define CAMERA_REG_YAVE      0x2F
#define CAMERA_REG_LumHTh    0x30
#define CAMERA_REG_LumLTh    0x31
#define CAMERA_REG_HREF      0x32
#define CAMERA_REG_DM_LNL    0x33
#define CAMERA_REG_DM_LNH    0x34
#define CAMERA_REG_ADoff_B   0x35
#define CAMERA_REG_ADoff_R   0x36
#define CAMERA_REG_ADoff_Gb  0x37
#define CAMERA_REG_ADoff_Gr  0x38
#define CAMERA_REG_Off_B     0x39
#define CAMERA_REG_Off_R     0x3A
#define CAMERA_REG_Off_Gb    0x3B
#define CAMERA_REG_Off_Gr    0x3C
#define CAMERA_REG_COM12     0x3D
#define CAMERA_REG_COM13     0x3E
#define CAMERA_REG_COM14     0x3F
#define CAMERA_REG_COM16     0x41
#define CAMERA_REG_TGT_B     0x42
#define CAMERA_REG_TGT_R     0x43
#define CAMERA_REG_TGT_Gb    0x44
#define CAMERA_REG_TGT_Gr    0x45
#define CAMERA_REG_LC_CTR    0x46
#define CAMERA_REG_LC_XC     0x47
#define CAMERA_REG_LC_YC     0x48
#define CAMERA_REG_LC_COEF   0x49
#define CAMERA_REG_LC_RADI   0x4A
#define CAMERA_REG_LC_COEFB  0x4B
#define CAMERA_REG_LC_COEFR  0x4C
#define CAMERA_REG_FixGain   0x4D
#define CAMERA_REG_AREF1     0x4F
#define CAMERA_REG_AREF6     0x54
#define CAMERA_REG_UFix      0x60
#define CAMERA_REG_VFix      0x61
#define CAMERA_REG_AWBb_blk  0x62
#define CAMERA_REG_AWB_Ctrl0 0x63
#define CAMERA_REG_DSP_Ctrl1 0x64
#define CAMERA_REG_DSP_Ctrl2 0x65
#define CAMERA_REG_DSP_Ctrl3 0x66
#define CAMERA_REG_DSP_Ctrl4 0x67
#define CAMERA_REG_AWB_bias  0x68
#define CAMERA_REG_AWBCtrl1  0x69
#define CAMERA_REG_AWBCtrl2  0x6A
#define CAMERA_REG_AWBCtrl3  0x6B
#define CAMERA_REG_AWBCtrl4  0x6C
#define CAMERA_REG_AWBCtrl5  0x6D
#define CAMERA_REG_AWBCtrl6  0x6E
#define CAMERA_REG_AWBCtrl7  0x6F
#define CAMERA_REG_AWBCtrl8  0x70
#define CAMERA_REG_AWBCtrl9  0x71
#define CAMERA_REG_AWBCtrl10 0x72
#define CAMERA_REG_AWBCtrl11 0x73
#define CAMERA_REG_AWBCtrl12 0x74
#define CAMERA_REG_AWBCtrl13 0x75
#define CAMERA_REG_AWBCtrl14 0x76
#define CAMERA_REG_AWBCtrl15 0x77
#define CAMERA_REG_AWBCtrl16 0x78
#define CAMERA_REG_AWBCtrl17 0x79
#define CAMERA_REG_AWBCtrl18 0x7A
#define CAMERA_REG_AWBCtrl19 0x7B
#define CAMERA_REG_AWBCtrl20 0x7C
#define CAMERA_REG_AWBCtrl21 0x7D
#define CAMERA_REG_GAM1      0x7E
#define CAMERA_REG_GAM2      0x7F
#define CAMERA_REG_GAM3      0x80
#define CAMERA_REG_GAM4      0x81
#define CAMERA_REG_GAM5      0x82
#define CAMERA_REG_GAM6      0x83
#define CAMERA_REG_GAM7      0x84
#define CAMERA_REG_GAM8      0x85
#define CAMERA_REG_GAM9      0x86
#define CAMERA_REG_GAM10     0x87
#define CAMERA_REG_GAM11     0x88
#define CAMERA_REG_GAM12     0x89
#define CAMERA_REG_GAM13     0x8A
#define CAMERA_REG_GAM14     0x8B
#define CAMERA_REG_GAM15     0x8C
#define CAMERA_REG_SLOP      0x8D
#define CAMERA_REG_DNSTh     0x8E
#define CAMERA_REG_EDGE0     0x8F
#define CAMERA_REG_EDGE1     0x90
#define CAMERA_REG_DNSOff    0x91
#define CAMERA_REG_EDGE2     0x92
#define CAMERA_REG_EDGE3     0x93
#define CAMERA_REG_MTX1      0x94
#define CAMERA_REG_MTX2      0x95
#define CAMERA_REG_MTX3      0x96
#define CAMERA_REG_MTX4      0x97
#define CAMERA_REG_MTX5      0x98
#define CAMERA_REG_MTX6      0x99
#define CAMERA_REG_MTX_Ctrl  0x9A
#define CAMERA_REG_BRIGHT    0x9B
#define CAMERA_REG_CNST      0x9C
#define CAMERA_REG_UVADJ0    0x9E
#define CAMERA_REG_UVADJ1    0x9F
#define CAMERA_REG_SCAL0     0xA0
#define CAMERA_REG_SCAL1     0xA1
#define CAMERA_REG_SCAL2     0xA2
#define CAMERA_REG_SDE       0xA6
#define CAMERA_REG_USAT      0xA7
#define CAMERA_REG_VSAT      0xA8
#define CAMERA_REG_HUECOS    0xA9
#define CAMERA_REG_HUESIN    0xAA
#define CAMERA_REG_SIGN      0xAB
#define CAMERA_REG_DSPAuto   0xAC

//摄像头初始化寄存器配置表，前者为寄存器地址，后者为寄存器值
static const uint8 camera_reg_init_table[][2] = {
	{ CAMERA_REG_COM7,      0x80 },	//复位各寄存器值

	//这两个寄存器决定帧速率:
	// 50帧: COM4 = 0xC1，CLKRC = 0x02;
	// 75帧: COM4 = 0x41，CLKRC = 0x00;
	//112帧: COM4 = 0x81，CLKRC = 0x00;
	//150帧: COM4 = 0xC1，CLKRC = 0x00;
	{ CAMERA_REG_COM4,      0xC1 },
	{ CAMERA_REG_CLKRC,     0x02 },

	{ CAMERA_REG_COM2,      0x03 },
	{ CAMERA_REG_COM3,      0xD0 },
	{ CAMERA_REG_COM7,      0x40 },
	{ CAMERA_REG_COM10,     0x20 },
	{ CAMERA_REG_HSTART,    0x3F },
	{ CAMERA_REG_HSIZE,     0x50 },
	{ CAMERA_REG_VSTRT,     0x03 },
	{ CAMERA_REG_VSIZE,     0x78 },
	{ CAMERA_REG_HREF,      0x00 },
	{ CAMERA_REG_SCAL0,     0x0A },
	{ CAMERA_REG_AWB_Ctrl0, 0xE0 },
	{ CAMERA_REG_DSPAuto,   0xFF },
	{ CAMERA_REG_DSP_Ctrl2, 0x0C },
	{ CAMERA_REG_DSP_Ctrl3, 0x00 },
	{ CAMERA_REG_DSP_Ctrl4, 0x00 },

#if (CAMERA_IMG_WIDTH == 80)
	{ CAMERA_REG_HOutSize,  0x14 },
#elif (CAMERA_IMG_WIDTH == 160)
	{ CAMERA_REG_HOutSize,  0x28 },
#elif (CAMERA_IMG_WIDTH == 240)
	{ CAMERA_REG_HOutSize,  0x3C },
#elif (CAMERA_IMG_WIDTH == 320)
	{ CAMERA_REG_HOutSize,  0x50 },
#endif

#if (CAMERA_IMG_HEIGHT == 60 )
	{ CAMERA_REG_VOutSize,  0x1E },
#elif (CAMERA_IMG_HEIGHT == 120 )
	{ CAMERA_REG_VOutSize,  0x3C },
#elif (CAMERA_IMG_HEIGHT == 180 )
	{ CAMERA_REG_VOutSize,  0x5A },
#elif (CAMERA_IMG_HEIGHT == 240 )
	{ CAMERA_REG_VOutSize,  0x78 },
#endif

	{ CAMERA_REG_EXHCH,     0x00 },
	{ CAMERA_REG_GAM1,      0x0C },
	{ CAMERA_REG_GAM2,      0x16 },
	{ CAMERA_REG_GAM3,      0x2A },
	{ CAMERA_REG_GAM4,      0x4E },
	{ CAMERA_REG_GAM5,      0x61 },
	{ CAMERA_REG_GAM6,      0x6F },
	{ CAMERA_REG_GAM7,      0x7B },
	{ CAMERA_REG_GAM8,      0x86 },
	{ CAMERA_REG_GAM9,      0x8E },
	{ CAMERA_REG_GAM10,     0x97 },
	{ CAMERA_REG_GAM11,     0xA4 },
	{ CAMERA_REG_GAM12,     0xAF },
	{ CAMERA_REG_GAM13,     0xC5 },
	{ CAMERA_REG_GAM14,     0xD7 },
	{ CAMERA_REG_GAM15,     0xE8 },
	{ CAMERA_REG_SLOP,      0x20 },
	{ CAMERA_REG_LC_RADI,   0x00 },
	{ CAMERA_REG_LC_COEF,   0x13 },
	{ CAMERA_REG_LC_XC,     0x08 },
	{ CAMERA_REG_LC_COEFB,  0x14 },
	{ CAMERA_REG_LC_COEFR,  0x17 },
	{ CAMERA_REG_LC_CTR,    0x05 },
	{ CAMERA_REG_BDBase,    0x99 },
	{ CAMERA_REG_BDMStep,   0x03 },
	{ CAMERA_REG_SDE,       0x04 },
	{ CAMERA_REG_BRIGHT,    0x00 },
	{ CAMERA_REG_CNST,      0x38 },	//这个寄存器调节阈值，值越小画面越暗
	{ CAMERA_REG_SIGN,      0x06 },
	{ CAMERA_REG_UVADJ0,    0x11 },
	{ CAMERA_REG_UVADJ1,    0x02 }
};

//需要配置的寄存器数量
static const uint8 reg_num = sizeof(camera_reg_init_table)
		/ sizeof(camera_reg_init_table[0]);

//==========================================================================
//函数名称: camera_sccb_write
//函数返回: 无
//参数说明: reg_val:寄存器地址-寄存器值对
//功能概要: 阻塞式地配置寄存器值
//==========================================================================
static void camera_sccb_write(uint8* reg_val) {
	//发送寄存器地址-寄存器值对直到成功
	while (i2c_master_send(CAMERA_I2C_MOD, I2C_ADDR_MODE_BITS_7,
	CAMERA_ADDR, reg_val, 2) != I2CMasterSuccess) {
	}
}

//==========================================================================
//函数名称: camera_sccb_read
//函数返回: 寄存器值
//参数说明: reg:寄存器地址
//功能概要: 阻塞式地读取寄存器值
//==========================================================================
static uint8 camera_sccb_read(uint8 reg) {
	uint8 val;	//寄存器值

	//发送寄存器地址直到成功
	while (i2c_master_send(CAMERA_I2C_MOD, I2C_ADDR_MODE_BITS_7,
	CAMERA_ADDR, &reg, 1) != I2CMasterSuccess) {
	}
	//接收寄存器值直到成功
	while (i2c_master_re(CAMERA_I2C_MOD, I2C_ADDR_MODE_BITS_7,
	CAMERA_ADDR, &val, 1) != I2CMasterSuccess) {
	}
	//返回寄存器值
	return val;
}

//==========================================================================
//函数名称: camera_init
//函数返回: 无
//参数说明: raw_img:原始图像首地址，字节数为CAMERA_RAW_IMG_BYTES，
//                 采集的原始图像信息存储在该数组中
//功能概要: 初始化摄像头，相应配置在camera.h中
//备注: 需要先使能提供的延时函数
//==========================================================================
void camera_init(uint8* raw_img) {
	uint8 i;	//游标

	//初始化数据信号引脚
	gpio_init(CAMERA_D0, GPIO_DIR_INPUT, GPIO_LEVEL_UNKNOWN);
	gpio_init(CAMERA_D1, GPIO_DIR_INPUT, GPIO_LEVEL_UNKNOWN);
	gpio_init(CAMERA_D2, GPIO_DIR_INPUT, GPIO_LEVEL_UNKNOWN);
	gpio_init(CAMERA_D3, GPIO_DIR_INPUT, GPIO_LEVEL_UNKNOWN);
	gpio_init(CAMERA_D4, GPIO_DIR_INPUT, GPIO_LEVEL_UNKNOWN);
	gpio_init(CAMERA_D5, GPIO_DIR_INPUT, GPIO_LEVEL_UNKNOWN);
	gpio_init(CAMERA_D6, GPIO_DIR_INPUT, GPIO_LEVEL_UNKNOWN);
	gpio_init(CAMERA_D7, GPIO_DIR_INPUT, GPIO_LEVEL_UNKNOWN);

	//初始化场中断信号引脚，下拉电阻
	gpio_init(CAMERA_VSYNC, GPIO_DIR_INPUT, GPIO_LEVEL_LOW);
	//开启场中断信号引脚的无源滤波
	gpio_set_passive_filter(CAMERA_VSYNC, true);

	//初始化时钟信号引脚，下拉电阻
	gpio_init(CAMERA_PCLK, GPIO_DIR_INPUT, GPIO_LEVEL_LOW);

	//初始化DMA通道
	dma_init(CAMERA_DMA_CH,			//设置通道
			CAMERA_DMA_REQ,			//设置请求源
			DMA_MODE_NORMAL,		//设置为正常模式
			1,						//每次副循环传输1个字节
			CAMERA_RAW_IMG_BYTES,	//主循环迭代次数等于原始图像字节数
			CAMERA_DMA_SRC_ADDR,	//设置源地址
			DMA_DATA_WIDTH_BYTE_1,	//每次从源地址读取1个字节
			0,						//源地址被读取后不偏移
			DMA_MODULO_DISABLED,	//禁用模数功能
			0,						//主循环完成后源地址不偏移
			(uint32) raw_img,		//设置目标地址
			DMA_DATA_WIDTH_BYTE_1,	//每次向目标地址写入1个字节
			1,						//目标地址被写入后偏移1个字节
			DMA_MODULO_DISABLED,	//禁用模数功能
			-CAMERA_RAW_IMG_BYTES,	//主循环完成后目标地址恢复为初始目标地址
			false);					//关闭主循环完成后自动不接收DMA请求
	//接收该通道的DMA请求
	dma_enable_req(CAMERA_DMA_CH);

	//初始化I2C模块
	i2c_init(CAMERA_I2C_MOD, CAMERA_I2C_MUL, CAMERA_I2C_ICR,
	CAMERA_I2C_ADDR_MODE, CAMERA_I2C_ADDR, CAMERA_I2C_ENABLE_GENERAL_CALL_ADDR);

	//复位各寄存器值
	camera_sccb_write((uint8*) camera_reg_init_table[0]);
	//配置各寄存器值
	for (i = 1; i < reg_num; i++) {
		//更改寄存器值需间隔1ms，否则寄存器值无法正常配置
		CAMERA_DELAY_US(1000);
		camera_sccb_write((uint8*) camera_reg_init_table[i]);
	}
}

//==========================================================================
//函数名称: camera_enable_vsync_int
//函数返回: 无
//参数说明: 无
//功能概要: 使能场中断，即允许接收场中断信号
//备注: 场中断信号引脚所在端口为PORTC时，中断服务函数为PORTC_IRQHandler
//==========================================================================
void camera_enable_vsync_int() {
	//场中断信号上升沿触发场中断
	gpio_enable_int(CAMERA_VSYNC, GPIO_INT_RISING_EDGE);
}

//==========================================================================
//函数名称: camera_disable_vsync_int
//函数返回: 无
//参数说明: 无
//功能概要: 关闭场中断，即不允许接收场中断信号
//==========================================================================
void camera_disable_vsync_int() {
	gpio_disable_int(CAMERA_VSYNC);
}

//==========================================================================
//函数名称: camera_get_vsync_int
//函数返回: true:产生中断; false:未产生中断
//参数说明: 无
//功能概要: 获取场中断的中断标志
//==========================================================================
bool camera_get_vsync_int() {
	return gpio_get_int(CAMERA_VSYNC);
}

//==========================================================================
//函数名称: camera_clear_vsync_int
//函数返回: 无
//参数说明: 无
//功能概要: 清除场中断的中断标志
//==========================================================================
void camera_clear_vsync_int() {
	gpio_clear_int(CAMERA_VSYNC);
}

//==========================================================================
//函数名称: camera_start_collecting
//函数返回: 无
//参数说明: 无
//功能概要: 开始采集原始图像信息
//==========================================================================
void camera_start_collecting() {
	//时钟信号下降沿触发DMA请求
	gpio_enable_dma(CAMERA_PCLK, GPIO_DMA_FALLING_EDGE);
}

//==========================================================================
//函数名称: camera_stop_collecting
//函数返回: 无
//参数说明: 无
//功能概要: 停止采集原始图像信息
//==========================================================================
void camera_stop_collecting() {
	gpio_disable_dma(CAMERA_PCLK);
}

//==========================================================================
//函数名称: camera_enable_collect_done_int
//函数返回: 无
//参数说明: 无
//功能概要: 使能原始图像采集完毕中断
//备注: 所使用的DMA通道为DMA_CH0时，中断服务函数为DMA0_IRQHandler
//==========================================================================
void camera_enable_collect_done_int() {
	dma_enable_major_int(CAMERA_DMA_CH);
}

//==========================================================================
//函数名称: camera_disable_collect_done_int
//函数返回: 无
//参数说明: 无
//功能概要: 关闭原始图像采集完毕中断
//==========================================================================
void camera_disable_collect_done_int() {
	dma_disable_major_int(CAMERA_DMA_CH);
}

//==========================================================================
//函数名称: camera_clear_collect_done_int
//函数返回: 无
//参数说明: 无
//功能概要: 清除原始图像采集完毕中断的中断标志
//==========================================================================
void camera_clear_collect_done_int() {
	dma_clear_major_int(CAMERA_DMA_CH);
}

//==========================================================================
//函数名称: camera_extract_raw_img
//函数返回: 无
//参数说明: raw_img:原始图像首地址
//         img:解压后图像首地址
//功能概要: 解压原始图像，得到解压后的图像
//备注: 解压后图像形如img[CAMERA_IMG_HEIGHT][CAMERA_IMG_WIDTH]，
//     每一个字节表示一个像素点，img[0][5]为第0行第5列的像素点
//==========================================================================
void camera_extract_raw_img(uint8* raw_img, uint8* img) {
	uint32 num;		//原始图像字节数
	uint8 black_val, white_val;	//黑白像素点值
	uint8 temp;		//临时值

	num = CAMERA_RAW_IMG_BYTES;
	black_val = CAMERA_EXTRACT_BLACK_VAL;
	white_val = CAMERA_EXTRACT_WHITE_VAL;

	//解压原始图像，原始图像1表示黑，0表示白
	while (num--) {
		temp = *raw_img++;
		*img++ = temp & 0x80 ? black_val : white_val;
		*img++ = temp & 0x40 ? black_val : white_val;
		*img++ = temp & 0x20 ? black_val : white_val;
		*img++ = temp & 0x10 ? black_val : white_val;
		*img++ = temp & 0x08 ? black_val : white_val;
		*img++ = temp & 0x04 ? black_val : white_val;
		*img++ = temp & 0x02 ? black_val : white_val;
		*img++ = temp & 0x01 ? black_val : white_val;
	}
}
