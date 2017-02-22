//==========================================================================
//�ļ����ƣ�camera.c
//���ܸ�Ҫ������ͷӦ����������Դ�ļ�
//==========================================================================

#include "camera.h"

//��������ͷ�ļĴ�����ַ��������������ͷ����
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

//����ͷ��ʼ���Ĵ������ñ�ǰ��Ϊ�Ĵ�����ַ������Ϊ�Ĵ���ֵ
static const uint8 camera_reg_init_table[][2] = {
	{ CAMERA_REG_COM7,      0x80 },	//��λ���Ĵ���ֵ

	//�������Ĵ�������֡����:
	// 50֡: COM4 = 0xC1��CLKRC = 0x02;
	// 75֡: COM4 = 0x41��CLKRC = 0x00;
	//112֡: COM4 = 0x81��CLKRC = 0x00;
	//150֡: COM4 = 0xC1��CLKRC = 0x00;
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
	{ CAMERA_REG_CNST,      0x80 },	//����Ĵ���������ֵ
	{ CAMERA_REG_SIGN,      0x06 },
	{ CAMERA_REG_UVADJ0,    0x11 },
	{ CAMERA_REG_UVADJ1,    0x02 }
};

//��Ҫ���õļĴ�������
static const uint8 reg_num = sizeof(camera_reg_init_table)
		/ sizeof(camera_reg_init_table[0]);

//==========================================================================
//��������: camera_sccb_write
//��������: ��
//����˵��: reg_val:�Ĵ�����ַ-�Ĵ���ֵ��
//���ܸ�Ҫ: ����ʽ�����üĴ���ֵ
//==========================================================================
static void camera_sccb_write(uint8* reg_val) {
	//���ͼĴ�����ַ-�Ĵ���ֵ��ֱ���ɹ�
	while (i2c_master_send(CAMERA_I2C_MOD, I2C_ADDR_MODE_BITS_7,
	CAMERA_ADDR, reg_val, 2) != I2CMasterSuccess) {
	}
}

//==========================================================================
//��������: camera_sccb_read
//��������: �Ĵ���ֵ
//����˵��: reg:�Ĵ�����ַ
//���ܸ�Ҫ: ����ʽ�ض�ȡ�Ĵ���ֵ
//==========================================================================
static uint8 camera_sccb_read(uint8 reg) {
	uint8 val;	//�Ĵ���ֵ

	//���ͼĴ�����ֱַ���ɹ�
	while (i2c_master_send(CAMERA_I2C_MOD, I2C_ADDR_MODE_BITS_7,
	CAMERA_ADDR, &reg, 1) != I2CMasterSuccess) {
	}
	//���ռĴ���ֱֵ���ɹ�
	while (i2c_master_re(CAMERA_I2C_MOD, I2C_ADDR_MODE_BITS_7,
	CAMERA_ADDR, &val, 1) != I2CMasterSuccess) {
	}
	//���ؼĴ���ֵ
	return val;
}

//==========================================================================
//��������: camera_init
//��������: ��
//����˵��: raw_img:ԭʼͼ���׵�ַ���ֽ���ΪCAMERA_RAW_IMG_BYTES��
//                 �ɼ���ԭʼͼ����Ϣ�洢�ڸ�������
//���ܸ�Ҫ: ��ʼ������ͷ����Ӧ������camera.h��
//��ע: ��Ҫ��ʹ���ṩ����ʱ����
//==========================================================================
void camera_init(uint8* raw_img) {
	uint8 i;	//�α�

	//��ʼ�������ź�����
	gpio_init(CAMERA_D0, GPIO_DIR_INPUT, GPIO_LEVEL_UNKNOWN);
	gpio_init(CAMERA_D1, GPIO_DIR_INPUT, GPIO_LEVEL_UNKNOWN);
	gpio_init(CAMERA_D2, GPIO_DIR_INPUT, GPIO_LEVEL_UNKNOWN);
	gpio_init(CAMERA_D3, GPIO_DIR_INPUT, GPIO_LEVEL_UNKNOWN);
	gpio_init(CAMERA_D4, GPIO_DIR_INPUT, GPIO_LEVEL_UNKNOWN);
	gpio_init(CAMERA_D5, GPIO_DIR_INPUT, GPIO_LEVEL_UNKNOWN);
	gpio_init(CAMERA_D6, GPIO_DIR_INPUT, GPIO_LEVEL_UNKNOWN);
	gpio_init(CAMERA_D7, GPIO_DIR_INPUT, GPIO_LEVEL_UNKNOWN);

	//��ʼ�����ж��ź����ţ���������
	gpio_init(CAMERA_VSYNC, GPIO_DIR_INPUT, GPIO_LEVEL_LOW);
	//�������ж��ź����ŵ���Դ�˲�
	gpio_set_passive_filter(CAMERA_VSYNC, true);

	//��ʼ��ʱ���ź����ţ���������
	gpio_init(CAMERA_PCLK, GPIO_DIR_INPUT, GPIO_LEVEL_LOW);

	//��ʼ��DMAͨ��
	dma_init(CAMERA_DMA_CH,			//����ͨ��
			CAMERA_DMA_REQ,			//��������Դ
			DMA_MODE_NORMAL,		//����Ϊ����ģʽ
			1,						//ÿ�θ�ѭ������1���ֽ�
			CAMERA_RAW_IMG_BYTES,	//��ѭ��������������ԭʼͼ���ֽ���
			CAMERA_DMA_SRC_ADDR,	//����Դ��ַ
			DMA_DATA_WIDTH_BYTE_1,	//ÿ�δ�Դ��ַ��ȡ1���ֽ�
			0,						//Դ��ַ����ȡ��ƫ��
			DMA_MODULO_DISABLED,	//����ģ������
			0,						//��ѭ����ɺ�Դ��ַ��ƫ��
			(uint32) raw_img,		//����Ŀ���ַ
			DMA_DATA_WIDTH_BYTE_1,	//ÿ����Ŀ���ַд��1���ֽ�
			1,						//Ŀ���ַ��д���ƫ��1���ֽ�
			DMA_MODULO_DISABLED,	//����ģ������
			-CAMERA_RAW_IMG_BYTES,	//��ѭ����ɺ�Ŀ���ַ�ָ�Ϊ��ʼĿ���ַ
			false);					//�ر���ѭ����ɺ��Զ�������DMA����
	//���ո�ͨ����DMA����
	dma_enable_req(CAMERA_DMA_CH);

	//��ʼ��I2Cģ��
	i2c_init(CAMERA_I2C_MOD, CAMERA_I2C_MUL, CAMERA_I2C_ICR,
	CAMERA_I2C_ADDR_MODE, CAMERA_I2C_ADDR, CAMERA_I2C_ENABLE_GENERAL_CALL_ADDR);

	//��λ���Ĵ���ֵ
	camera_sccb_write((uint8*) camera_reg_init_table[0]);
	//���ø��Ĵ���ֵ
	for (i = 1; i < reg_num; i++) {
		//���ļĴ���ֵ����1ms������Ĵ���ֵ�޷���������
		CAMERA_DELAY_US(1000);
		camera_sccb_write((uint8*) camera_reg_init_table[i]);
	}
}

//==========================================================================
//��������: camera_enable_vsync_int
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: ʹ�ܳ��жϣ���������ճ��ж��ź�
//��ע: ���ж��ź��������ڶ˿�ΪPORTCʱ���жϷ�����ΪPORTC_IRQHandler
//==========================================================================
void camera_enable_vsync_int() {
	//���ж��ź������ش������ж�
	gpio_enable_int(CAMERA_VSYNC, GPIO_INT_RISING_EDGE);
}

//==========================================================================
//��������: camera_disable_vsync_int
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: �رճ��жϣ�����������ճ��ж��ź�
//==========================================================================
void camera_disable_vsync_int() {
	gpio_disable_int(CAMERA_VSYNC);
}

//==========================================================================
//��������: camera_get_vsync_int
//��������: true:�����ж�; false:δ�����ж�
//����˵��: ��
//���ܸ�Ҫ: ��ȡ���жϵ��жϱ�־
//==========================================================================
bool camera_get_vsync_int() {
	return gpio_get_int(CAMERA_VSYNC);
}

//==========================================================================
//��������: camera_clear_vsync_int
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: ������жϵ��жϱ�־
//==========================================================================
void camera_clear_vsync_int() {
	gpio_clear_int(CAMERA_VSYNC);
}

//==========================================================================
//��������: camera_start_collecting
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: ��ʼ�ɼ�ԭʼͼ����Ϣ
//==========================================================================
void camera_start_collecting() {
	//ʱ���ź��½��ش���DMA����
	gpio_enable_dma(CAMERA_PCLK, GPIO_DMA_FALLING_EDGE);
}

//==========================================================================
//��������: camera_stop_collecting
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: ֹͣ�ɼ�ԭʼͼ����Ϣ
//==========================================================================
void camera_stop_collecting() {
	gpio_disable_dma(CAMERA_PCLK);
}

//==========================================================================
//��������: camera_enable_collect_done_int
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: ʹ��ԭʼͼ��ɼ�����ж�
//��ע: ��ʹ�õ�DMAͨ��ΪDMA_CH0ʱ���жϷ�����ΪDMA0_IRQHandler
//==========================================================================
void camera_enable_collect_done_int() {
	dma_enable_major_int(CAMERA_DMA_CH);
}

//==========================================================================
//��������: camera_disable_collect_done_int
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: �ر�ԭʼͼ��ɼ�����ж�
//==========================================================================
void camera_disable_collect_done_int() {
	dma_disable_major_int(CAMERA_DMA_CH);
}

//==========================================================================
//��������: camera_clear_collect_done_int
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: ���ԭʼͼ��ɼ�����жϵ��жϱ�־
//==========================================================================
void camera_clear_collect_done_int() {
	dma_clear_major_int(CAMERA_DMA_CH);
}

//==========================================================================
//��������: camera_extract_raw_img
//��������: ��
//����˵��: raw_img:ԭʼͼ���׵�ַ
//         img:��ѹ��ͼ���׵�ַ
//���ܸ�Ҫ: ��ѹԭʼͼ�񣬵õ���ѹ���ͼ��
//��ע: ��ѹ��ͼ������img[CAMERA_IMG_HEIGHT][CAMERA_IMG_WIDTH]��
//     ÿһ���ֽڱ�ʾһ�����ص㣬img[0][5]Ϊ��0�е�5�е����ص�
//==========================================================================
void camera_extract_raw_img(uint8* raw_img, uint8* img) {
	uint32 num;		//ԭʼͼ���ֽ���
	uint8 black_val, white_val;	//�ڰ����ص�ֵ

	num = CAMERA_RAW_IMG_BYTES;
	black_val = CAMERA_EXTRACT_BLACK_VAL;
	white_val = CAMERA_EXTRACT_WHITE_VAL;

	//��ѹԭʼͼ��ԭʼͼ��1��ʾ�ڣ�0��ʾ��
	while (num--) {
		*img++ = *raw_img & 0x80 ? black_val : white_val;
		*img++ = *raw_img & 0x40 ? black_val : white_val;
		*img++ = *raw_img & 0x20 ? black_val : white_val;
		*img++ = *raw_img & 0x10 ? black_val : white_val;
		*img++ = *raw_img & 0x08 ? black_val : white_val;
		*img++ = *raw_img & 0x04 ? black_val : white_val;
		*img++ = *raw_img & 0x02 ? black_val : white_val;
		*img++ = *raw_img++ & 0x01 ? black_val : white_val;
	}
}
