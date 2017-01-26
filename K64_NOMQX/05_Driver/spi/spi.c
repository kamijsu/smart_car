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
void spi_master_init(uint8 mod, uint8 config, uint32 baud_scaler, uint8 format,
		uint8 data_size, uint8 bit_order, uint8 csc_scaler, uint8 asc_scaler,
		uint8 dt_scaler) {
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

	//ʹ��SPIʱ��
	REG_CLR_MASK(SPI_MCR_REG(spi_ptr), SPI_MCR_MDIS_MASK);

	REG_SET_MASK(SPI_MCR_REG(spi_ptr), SPI_MCR_MSTR_MASK	//����SPIģ��Ϊ����ģʽ
			|SPI_MCR_PCSIS_MASK//���ø�Ƭѡ�ź���Ч��ƽΪ�͵�ƽ
			|SPI_MCR_DIS_TXF_MASK//�ر�FIFO���Ͷ���
			|SPI_MCR_DIS_RXF_MASK//�ر�FIFO���ն���
			|SPI_MCR_CLR_TXF_MASK//���FIFO���ͼ�����
			|SPI_MCR_CLR_RXF_MASK);//���FIFO���ռ�����

	//ֱ������CTAR�Ĵ�����ֵ����Լ��IO����
	REG_SET_VAL(SPI_CTAR_REG(spi_ptr,config), baud_scaler//���ò����ʷ�Ƶ����
			|SPI_CTAR_FMSZ(data_size-1)//����֡����λ��
			|SPI_CTAR_PCSSCK(csc_scaler>>4)//����CSCԤ��Ƶ����
			|SPI_CTAR_CSSCK(csc_scaler)//����CSC��Ƶ����
			|SPI_CTAR_PASC(asc_scaler>>4)//����ASCԤ��Ƶ����
			|SPI_CTAR_ASC(asc_scaler)//����ASC��Ƶ����
			|SPI_CTAR_PDT(dt_scaler>>4)//����DTԤ��Ƶ����
			|SPI_CTAR_DT(dt_scaler));//����DT��Ƶ����

	//����֡��ʽ������CTAR����λ��ֵȫ����0��������λ����
	switch (format) {
	case SPI_FORMAT_CPOL0_CPHA0:
		break;
	case SPI_FORMAT_CPOL0_CPHA1:
		REG_SET_MASK(SPI_CTAR_REG(spi_ptr,config), SPI_CTAR_CPHA_MASK);
		break;
	case SPI_FORMAT_CPOL1_CPHA0:
		REG_SET_MASK(SPI_CTAR_REG(spi_ptr,config), SPI_CTAR_CPOL_MASK);
		break;
	case SPI_FORMAT_CPOL1_CPHA1:
		REG_SET_MASK(SPI_CTAR_REG(spi_ptr,config),
				SPI_CTAR_CPOL_MASK|SPI_CTAR_CPHA_MASK);
		break;
	}

	//����λ����˳��
	if (bit_order == SPI_BIT_ORDER_LSB) {
		REG_SET_MASK(SPI_CTAR_REG(spi_ptr,config), SPI_CTAR_LSBFE_MASK);
	}

	//�������״̬��ʶ
	REG_SET_VAL(SPI_SR_REG(spi_ptr), 0xFFFFFFFF);

	//��ʼ����
	REG_CLR_MASK(SPI_MCR_REG(spi_ptr), SPI_MCR_HALT_MASK);
}

uint16 spi_master_send(uint8 mod, uint8 config, uint8 cs, uint16 data,
		uint8 cont) {
	SPI_Type * spi_ptr;	//SPI����ַ

	//��ȡSPI����ַ
	spi_ptr = spi_table[mod];

	REG_SET_VAL(SPI_PUSHR_REG(spi_ptr), SPI_PUSHR_CTAS(config)//��������CTAR�Ĵ���������������
			|SPI_PUSHR_PCS(1<<cs)//������ЧƬѡ�ź�
			|SPI_PUSHR_TXDATA(data)//���÷�������
			|SPI_PUSHR_CONT(cont));//�����Ƿ�ʹ����������
	//�ȴ��������
	while (!REG_GET_MASK(SPI_SR_REG(spi_ptr), SPI_SR_TCF_MASK)) {
	}
	//���������ɱ�־
	REG_SET_MASK(SPI_SR_REG(spi_ptr), SPI_SR_TCF_MASK);
//	while(!REG_GET_MASK(SPI_SR_REG(spi_ptr), SPI_SR_RFDF_MASK));
//	REG_SET_MASK(SPI_SR_REG(spi_ptr), SPI_SR_RFDF_MASK);
	//���ؽ��յ�������
	return SPI_POPR_REG(spi_ptr);
}

//�ӻ���֧��MSB��format��data_sizeҪ������һ��
void spi_slave_init(uint8 mod, uint8 format, uint8 data_size) {
	SPI_Type * spi_ptr;	//SPI����ַ

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
	//ʹ��Ƭѡ�ź�0���Ź���
	com_port_pin_set_mux(spi_cs_pin_table[mod][0],
			spi_cs_pcr_mux_table[mod][0]);

	//ʹ��SPIʱ��
	REG_CLR_MASK(SPI_MCR_REG(spi_ptr), SPI_MCR_MDIS_MASK);

	REG_CLR_MASK(SPI_MCR_REG(spi_ptr), SPI_MCR_MSTR_MASK	//����SPIģ��Ϊ�ӻ�ģʽ
			|SPI_MCR_DIS_TXF_MASK//����FIFO���Ͷ���
			|SPI_MCR_DIS_RXF_MASK//����FIFO���ն���
			|SPI_MCR_CLR_TXF_MASK//�����FIFO���ͼ�����
			|SPI_MCR_CLR_RXF_MASK);//�����FIFO���ռ�����

	REG_SET_MASK(SPI_MCR_REG(spi_ptr), SPI_MCR_PCSIS_MASK);	//���ø�Ƭѡ�ź���Ч��ƽΪ�͵�ƽ
//			|SPI_MCR_DIS_TXF_MASK//�ر�FIFO���Ͷ���
//			|SPI_MCR_DIS_RXF_MASK//�ر�FIFO���ն���
//			|SPI_MCR_CLR_TXF_MASK//���FIFO���ͼ�����
//			|SPI_MCR_CLR_RXF_MASK);//���FIFO���ռ�����

	//����֡����λ��������λ��0
	REG_SET_VAL(SPI_CTAR_SLAVE_REG(spi_ptr,0),
			SPI_CTAR_SLAVE_FMSZ(data_size-1));

	//����֡��ʽ������CTAR����λ��ֵȫ����0��������λ����
	switch (format) {
	case SPI_FORMAT_CPOL0_CPHA0:
		break;
	case SPI_FORMAT_CPOL0_CPHA1:
		REG_SET_MASK(SPI_CTAR_SLAVE_REG(spi_ptr,0), SPI_CTAR_SLAVE_CPHA_MASK);
		break;
	case SPI_FORMAT_CPOL1_CPHA0:
		REG_SET_MASK(SPI_CTAR_SLAVE_REG(spi_ptr,0), SPI_CTAR_SLAVE_CPOL_MASK);
		break;
	case SPI_FORMAT_CPOL1_CPHA1:
		REG_SET_MASK(SPI_CTAR_SLAVE_REG(spi_ptr,0),
				SPI_CTAR_SLAVE_CPOL_MASK|SPI_CTAR_SLAVE_CPHA_MASK);
		break;
	}

	//�������״̬��ʶ
	REG_SET_VAL(SPI_SR_REG(spi_ptr), 0xFFFFFFFF);

	//��ʼ����
	REG_CLR_MASK(SPI_MCR_REG(spi_ptr), SPI_MCR_HALT_MASK);
}

bool spi_slave_send(uint8 mod, uint32 data) {
	SPI_Type * spi_ptr;	//SPI����ַ

	//��ȡSPI����ַ
	spi_ptr = spi_table[mod];

	if (REG_GET_MASK(SPI_SR_REG(spi_ptr), SPI_SR_TFFF_MASK)) {
		REG_SET_VAL(SPI_PUSHR_SLAVE_REG(spi_ptr), data);
		REG_SET_MASK(SPI_SR_REG(spi_ptr), SPI_SR_TFFF_MASK);
		return true;
	} else {
		return false;
	}
}

bool spi_slave_re(uint8 mod, uint32* data) {
	SPI_Type * spi_ptr;	//SPI����ַ

	//��ȡSPI����ַ
	spi_ptr = spi_table[mod];

	if (REG_GET_MASK(SPI_SR_REG(spi_ptr), SPI_SR_RFDF_MASK)) {
		*data = SPI_POPR_REG(spi_ptr);
		REG_SET_MASK(SPI_SR_REG(spi_ptr), SPI_SR_RFDF_MASK);
		return true;
	} else {
		return false;
	}
}

void spi_slave_enable_re_int(uint8 mod) {
	REG_SET_MASK(SPI_RSER_REG(spi_table[mod]), SPI_RSER_RFDF_RE_MASK);
	ENABLE_IRQ(spi_irq_table[mod]);
}

void spi_slave_disable_re_int(uint8 mod) {
	REG_CLR_MASK(SPI_RSER_REG(spi_table[mod]), SPI_RSER_RFDF_RE_MASK);
	DISABLE_IRQ(spi_irq_table[mod]);
}
