//==========================================================================
//�ļ����ƣ�spi.c
//���ܸ�Ҫ��K64 SPI�ײ���������Դ�ļ�
//==========================================================================

#include "spi.h"

//SPI��ģ�����ַ
static SPI_Type * const spi_table[] = SPI_BASE_PTRS;
//SPI��ģ��ʱ���źŵ����ź�
static const uint8 spi_sck_pin_table[] = { SPI_MOD0_SCK_PIN, SPI_MOD1_SCK_PIN,
SPI_MOD2_SCK_PIN };
//SPI��ģ��ʱ���źŵ�PCR��MUXֵ
static const uint8 spi_sck_pcr_mux_table[] = { SPI_MOD0_SCK_PCR_MUX,
SPI_MOD1_SCK_PCR_MUX, SPI_MOD2_SCK_PCR_MUX };
//SPI��ģ������źŵ����ź�
static const uint8 spi_sout_pin_table[] = { SPI_MOD0_SOUT_PIN,
SPI_MOD1_SOUT_PIN, SPI_MOD2_SOUT_PIN };
//SPI��ģ������źŵ�PCR��MUXֵ
static const uint8 spi_sout_pcr_mux_table[] = { SPI_MOD0_SOUT_PCR_MUX,
SPI_MOD1_SOUT_PCR_MUX, SPI_MOD2_SOUT_PCR_MUX };
//SPI��ģ�������źŵ����ź�
static const uint8 spi_sin_pin_table[] = { SPI_MOD0_SIN_PIN, SPI_MOD1_SIN_PIN,
SPI_MOD2_SIN_PIN };
//SPI��ģ�������źŵ�PCR��MUXֵ
static const uint8 spi_sin_pcr_mux_table[] = { SPI_MOD0_SIN_PCR_MUX,
SPI_MOD1_SIN_PCR_MUX, SPI_MOD2_SIN_PCR_MUX };
//SPI��ģ��Ƭѡ�źŵ����ź�
static const uint8 spi_cs_pin_table[][6] = { { SPI_MOD0_CS0_PIN,
SPI_MOD0_CS1_PIN, SPI_MOD0_CS2_PIN, SPI_MOD0_CS3_PIN, SPI_MOD0_CS4_PIN,
SPI_MOD0_CS5_PIN }, { SPI_MOD1_CS0_PIN, SPI_MOD1_CS1_PIN,
SPI_MOD1_CS2_PIN, SPI_MOD1_CS3_PIN }, { SPI_MOD2_CS0_PIN,
SPI_MOD2_CS1_PIN } };
//SPI��ģ��Ƭѡ�źŵ�PCR��MUXֵ
static const uint8 spi_cs_pcr_mux_table[][6] = { { SPI_MOD0_CS0_PCR_MUX,
SPI_MOD0_CS1_PCR_MUX, SPI_MOD0_CS2_PCR_MUX, SPI_MOD0_CS3_PCR_MUX,
SPI_MOD0_CS4_PCR_MUX, SPI_MOD0_CS5_PCR_MUX }, { SPI_MOD1_CS0_PCR_MUX,
SPI_MOD1_CS1_PCR_MUX, SPI_MOD1_CS2_PCR_MUX, SPI_MOD1_CS3_PCR_MUX }, {
SPI_MOD2_CS0_PCR_MUX, SPI_MOD2_CS1_PCR_MUX } };
//����ģʽ���Ƿ�����SPI��ģ���Ƭѡ�ź�
static const bool spi_cs_enable_table[][6] = { { SPI_MOD0_CS0_ENABLE,
SPI_MOD0_CS1_ENABLE, SPI_MOD0_CS2_ENABLE, SPI_MOD0_CS3_ENABLE,
SPI_MOD0_CS4_ENABLE, SPI_MOD0_CS5_ENABLE }, { SPI_MOD1_CS0_ENABLE,
SPI_MOD1_CS1_ENABLE, SPI_MOD1_CS2_ENABLE, SPI_MOD1_CS3_ENABLE,
SPI_MOD1_CS4_ENABLE, SPI_MOD1_CS5_ENABLE }, { SPI_MOD2_CS0_ENABLE,
SPI_MOD2_CS1_ENABLE, SPI_MOD2_CS2_ENABLE, SPI_MOD2_CS3_ENABLE,
SPI_MOD2_CS4_ENABLE, SPI_MOD2_CS5_ENABLE } };
//SPI��ģ���ж������
static const IRQn_Type spi_irq_table[] = { SPI0_IRQn, SPI1_IRQn, SPI2_IRQn };
//t CSC = (1/fP ) x PCSSCK x CSSCK
//t ASC = (1/fP) x PASC x ASC
//tDT = (1/fP ) x PDT x DT
// (fP /PBR) x [(1+DBR)/BR]
void spi_init_master(uint8 mod, uint8 config, uint32 baud, uint8 format,
		uint8 data_size, uint8 bit_order) {
	SPI_Type * spi_ptr;	//SPI����ַ
	uint8 i;	//�α�

	//��ȡSPI����ַ
	spi_ptr = spi_table[mod];

	//������ӦSPIģ��ʱ����
	switch (mod) {
	case SPI_MOD0:
		REG_SET_MASK(SIM_SCGC6, SIM_SCGC6_SPI0_MASK);
		break;
	case SPI_MOD1:
		REG_SET_MASK(SIM_SCGC6, SIM_SCGC6_SPI1_MASK);
		break;
	case SPI_MOD2:
		REG_SET_MASK(SIM_SCGC3, SIM_SCGC3_SPI2_MASK);
		break;
	}

	//��ʱֹͣSPI����
	REG_SET_MASK(SPI_MCR_REG(spi_ptr), SPI_MCR_HALT_MASK);

	//ʹ��ʱ���ź����Ź���
	com_port_pin_set_mux(spi_sck_pin_table[mod], spi_sck_pcr_mux_table[mod]);
	//ʹ������ź����Ź���
	com_port_pin_set_mux(spi_sout_pin_table[mod], spi_sout_pcr_mux_table[mod]);
	//ʹ�������ź����Ź���
	com_port_pin_set_mux(spi_sin_pin_table[mod], spi_sin_pcr_mux_table[mod]);
	//������������ʹ��Ƭѡ�ź����Ź���
	for (i = SPI_CS0; i <= SPI_CS5; i++) {
		//��ʹ�ܸ�Ƭѡ�źţ���ʹ�����Ź���
		if (spi_cs_enable_table[mod][i]) {
			com_port_pin_set_mux(spi_cs_pin_table[mod][i],
					spi_cs_pcr_mux_table[mod][i]);
		}
	}

	//����SPIģ��Ϊ����ģʽ��Ƭѡ�ź���Ч��ƽΪ�͵�ƽ���ر�FIFO���У������FIFO������
	REG_SET_MASK(SPI_MCR_REG(spi_ptr),
			SPI_MCR_MSTR_MASK|SPI_MCR_PCSIS_MASK|SPI_MCR_DIS_TXF_MASK|SPI_MCR_DIS_RXF_MASK|SPI_MCR_CLR_TXF_MASK|SPI_MCR_CLR_RXF_MASK);

	//����֡��ʽ
	switch (format) {
	case SPI_FORMAT_CPOL0_CPHA0:
		REG_CLR_MASK(SPI_CTAR_REG(spi_ptr,config),
				SPI_CTAR_CPOL_MASK|SPI_CTAR_CPHA_MASK);
		break;
	case SPI_FORMAT_CPOL0_CPHA1:
		REG_CLR_MASK(SPI_CTAR_REG(spi_ptr,config), SPI_CTAR_CPOL_MASK);
		REG_SET_MASK(SPI_CTAR_REG(spi_ptr,config), SPI_CTAR_CPHA_MASK);
		break;
	case SPI_FORMAT_CPOL1_CPHA0:
		REG_SET_MASK(SPI_CTAR_REG(spi_ptr,config), SPI_CTAR_CPOL_MASK);
		REG_CLR_MASK(SPI_CTAR_REG(spi_ptr,config), SPI_CTAR_CPHA_MASK);
		break;
	case SPI_FORMAT_CPOL1_CPHA1:
		REG_SET_MASK(SPI_CTAR_REG(spi_ptr,config),
				SPI_CTAR_CPOL_MASK|SPI_CTAR_CPHA_MASK);
		break;
	}

	//����֡����λ��
	REG_CLR_MASK(SPI_CTAR_REG(spi_ptr,config), SPI_CTAR_FMSZ_MASK);
	REG_SET_MASK(SPI_CTAR_REG(spi_ptr,config), SPI_CTAR_FMSZ(data_size));

	//����λ����˳��
	if (bit_order == SPI_BIT_ORDER_LSB) {
		REG_SET_MASK(SPI_CTAR_REG(spi_ptr,config), SPI_CTAR_LSBFE_MASK);
	} else {
		REG_CLR_MASK(SPI_CTAR_REG(spi_ptr,config), SPI_CTAR_LSBFE_MASK);
	}
}
