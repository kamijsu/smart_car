//==========================================================================
//�ļ����ƣ�spi.c
//���ܸ�Ҫ��K64 SPI�ײ���������Դ�ļ�
//==========================================================================

#include "spi.h"

//SPI��ģ�����ַ
static SPI_Type * const spi_table[] = SPI_BASE_PTRS;
//SPI��ģ��ʱ���źŵ����ź�
static const uint8 spi_sck_pin_table[] =
{ SPI_MOD0_SCK_PIN, SPI_MOD1_SCK_PIN, SPI_MOD2_SCK_PIN };
//SPI��ģ��ʱ���źŵ�PCR��MUXֵ
static const uint8 spi_sck_pcr_mux_table[] =
{ SPI_MOD0_SCK_PCR_MUX, SPI_MOD1_SCK_PCR_MUX, SPI_MOD2_SCK_PCR_MUX };
//SPI��ģ������źŵ����ź�
static const uint8 spi_sout_pin_table[] =
{ SPI_MOD0_SOUT_PIN, SPI_MOD1_SOUT_PIN, SPI_MOD2_SOUT_PIN };
//SPI��ģ������źŵ�PCR��MUXֵ
static const uint8 spi_sout_pcr_mux_table[] =
{ SPI_MOD0_SOUT_PCR_MUX, SPI_MOD1_SOUT_PCR_MUX, SPI_MOD2_SOUT_PCR_MUX };
//SPI��ģ�������źŵ����ź�
static const uint8 spi_sin_pin_table[] =
{ SPI_MOD0_SIN_PIN, SPI_MOD1_SIN_PIN, SPI_MOD2_SIN_PIN };
//SPI��ģ�������źŵ�PCR��MUXֵ
static const uint8 spi_sin_pcr_mux_table[] =
{ SPI_MOD0_SIN_PCR_MUX, SPI_MOD1_SIN_PCR_MUX, SPI_MOD2_SIN_PCR_MUX };
//SPI��ģ��Ƭѡ�źŵ����ź�
static const uint8 spi_cs_pin_table[][6] = {
{ SPI_MOD0_CS0_PIN, SPI_MOD0_CS1_PIN, SPI_MOD0_CS2_PIN,
  SPI_MOD0_CS3_PIN, SPI_MOD0_CS4_PIN, SPI_MOD0_CS5_PIN },
{ SPI_MOD1_CS0_PIN, SPI_MOD1_CS1_PIN, SPI_MOD1_CS2_PIN,
  SPI_MOD1_CS3_PIN },
{ SPI_MOD2_CS0_PIN, SPI_MOD2_CS1_PIN }
};
//SPI��ģ��Ƭѡ�źŵ�PCR��MUXֵ
static const uint8 spi_cs_pcr_mux_table[][6] = {
{ SPI_MOD0_CS0_PCR_MUX, SPI_MOD0_CS1_PCR_MUX, SPI_MOD0_CS2_PCR_MUX,
  SPI_MOD0_CS3_PCR_MUX, SPI_MOD0_CS4_PCR_MUX, SPI_MOD0_CS5_PCR_MUX },
{ SPI_MOD1_CS0_PCR_MUX, SPI_MOD1_CS1_PCR_MUX, SPI_MOD1_CS2_PCR_MUX,
  SPI_MOD1_CS3_PCR_MUX },
{ SPI_MOD2_CS0_PCR_MUX, SPI_MOD2_CS1_PCR_MUX }
};
//����ģʽ���Ƿ�����SPI��ģ���Ƭѡ�ź�
static const bool spi_cs_enable_table[][6] = {
{ SPI_MOD0_CS0_ENABLE, SPI_MOD0_CS1_ENABLE, SPI_MOD0_CS2_ENABLE,
  SPI_MOD0_CS3_ENABLE, SPI_MOD0_CS4_ENABLE, SPI_MOD0_CS5_ENABLE },
{ SPI_MOD1_CS0_ENABLE, SPI_MOD1_CS1_ENABLE, SPI_MOD1_CS2_ENABLE,
  SPI_MOD1_CS3_ENABLE, SPI_MOD1_CS4_ENABLE, SPI_MOD1_CS5_ENABLE },
{ SPI_MOD2_CS0_ENABLE, SPI_MOD2_CS1_ENABLE, SPI_MOD2_CS2_ENABLE,
  SPI_MOD2_CS3_ENABLE, SPI_MOD2_CS4_ENABLE, SPI_MOD2_CS5_ENABLE }
};
//SPI��ģ���ж������
static const IRQn_Type spi_irq_table[] = { SPI0_IRQn, SPI1_IRQn, SPI2_IRQn };

//==========================================================================
//��������: spi_master_init
//��������: ��
//����˵��: mod:SPIģ���:
//             SPI_MODx��xΪģ���;
//         config:��ʼ��������ʹ�õ����ú�:
//                SPI_CONFIGx��xΪ���úţ�x��ȡֵΪ0��1;
//         baud_scaler:�����ʷ�Ƶ����:
//                     SPI_BAUD_SCALER_x��xΪ��Ƶ���Ӵ�С��x��ȡֵΪ:
//                     2  | 3  | 4  | 5  | 6  | 7  | 8  | 9   | 10  | 12  |
//                     14 | 15 | 16 | 18 | 20 | 21 | 24 | 28  | 30  | 32  |
//                     40 | 42 | 48 | 56 | 64 | 80 | 96 | 112 | 128 | 160 |
//                     192  | 224  | 256  | 320  | 384  | 448  | 512  | 640    |
//                     768  | 896  | 1024 | 1280 | 1536 | 1792 | 2048 | 2560   |
//                     3072 | 3584 | 4096 | 5120 | 6144 | 7168 | 8192 | 10240  |
//                     12288  | 14336 | 16384 | 20480 | 24576 | 28672 | 32768  |
//                     40960  | 49152 | 57344 | 65536 | 81920 | 98304 | 114688 |
//                     163840 | 229376
//         format:֡��ʽ:
//                SPI_FORMAT_CPOL0_CPHA0:ƽʱ�͵�ƽ����һ����ȡ�����ڶ�������������;
//                SPI_FORMAT_CPOL0_CPHA1:ƽʱ�͵�ƽ����һ�����������ߣ��ڶ�����ȡ��;
//                SPI_FORMAT_CPOL1_CPHA0:ƽʱ�ߵ�ƽ����һ����ȡ�����ڶ�������������;
//                SPI_FORMAT_CPOL1_CPHA1:ƽʱ�ߵ�ƽ����һ�����������ߣ��ڶ�����ȡ��;
//         data_size:һ֡������λ������ȡֵΪ[4,16]��һ��Ϊ8��16����ÿ�η���1��2���ֽ�
//         bit_order:λ����˳��:
//                   SPI_BIT_ORDER_MSB:�����Чλ�����ȷ������λ;
//                   SPI_BIT_ORDER_LSB:�����Чλ�����ȷ������λ;
//         csc_scaler:Ƭѡ�ź���Ч����һ��ʱ�������������ʱʱ��(CSC)�ķ�Ƶ����:
//                    SPI_DELAY_SCALER_x��xΪ��Ƶ���Ӵ�С��x��ȡֵΪ:
//                    2  | 4  | 6  | 8  | 10 | 12 | 14 | 16 | 20  | 24   |
//                    28 | 32 | 40 | 48 | 56 | 64 | 80 | 96 | 112 | 128  |
//                    160 | 192 | 224 | 256  | 320  | 384  | 448  | 512  |
//                    640 | 768 | 896 | 1024 | 1280 | 1536 | 1792 | 2048 |
//                    2560 | 3072  | 3584  | 4096  | 5120  | 6144  | 7168  |
//                    8192 | 10240 | 12288 | 14336 | 16384 | 20480 | 24576 |
//                    28672  | 32768 | 40960  | 49152  | 57344  | 65536  |
//                    81920  | 98304 | 114688 | 163840 | 196608 | 229376 |
//                    327680 | 458752
//         asc_scaler:���һ��ʱ���������䵽Ƭѡ�ź���Ч����ʱʱ��(ASC)�ķ�Ƶ���ӣ�
//                    ��ѡ����ͬcsc_scaler
//         dt_scaler:��֡�����Ƭѡ�ź���Ч��Ƭѡ�ź���Ч����ʱʱ��(DT)�ķ�Ƶ���ӣ�
//                    ��ѡ����ͬcsc_scaler
//���ܸ�Ҫ: ��ʼ��SPIģ�飬������Ϊ����ģʽ
//��ע: SPIģ�鱻����Ϊ����ģʽʱ�����Ա���ʼ�����Σ�ʹ�ò�ͬ�����úţ�
//     ���Ա������׷��Ͳ������ڷ���ʱѡ������һ��ʹ��;
//
//     Ϊ�˿�����ȷ�������ݣ�֡��ʽ��֡����λ����λ����˳������ӻ�һ��;
//
//     �����ʼ��㹫ʽ:����ʱ��Ƶ��/x��xΪSPI_BAUD_SCALER_x��x������ʱ��Ƶ�ʵ�λΪHz��
//     ������ʱ��Ƶ��Ϊ48MHz��xΪ4��������Ϊ12M����λΪbps;
//
//     ��ʱʱ��(CSC,ASC,DT)���㹫ʽ:x/����ʱ��Ƶ�ʣ�xΪSPI_DELAY_SCALER_x��x��
//     ����ʱ��Ƶ�ʵ�λΪHz��������ʱ��Ƶ��Ϊ48MHz��xΪ12������ʱʱ��Ϊ0.25��s
//==========================================================================
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

	REG_SET_MASK(SPI_MCR_REG(spi_ptr),
			 SPI_MCR_MSTR_MASK		//����SPIģ��Ϊ����ģʽ
			|SPI_MCR_PCSIS_MASK		//���ø�Ƭѡ�ź���Ч��ƽΪ�͵�ƽ
			|SPI_MCR_DIS_TXF_MASK	//�ر�FIFO���Ͷ���
			|SPI_MCR_DIS_RXF_MASK	//�ر�FIFO���ն���
			|SPI_MCR_CLR_TXF_MASK	//���FIFO���ͼ�����
			|SPI_MCR_CLR_RXF_MASK);	//���FIFO���ռ�����

	//ֱ������CTAR�Ĵ�����ֵ����Լ��IO����
	REG_SET_VAL(SPI_CTAR_REG(spi_ptr,config),
			 baud_scaler					//���ò����ʷ�Ƶ����
			|SPI_CTAR_FMSZ(data_size-1)		//����֡����λ��
			|SPI_CTAR_PCSSCK(csc_scaler>>4)	//����CSCԤ��Ƶ����
			|SPI_CTAR_CSSCK(csc_scaler)		//����CSC��Ƶ����
			|SPI_CTAR_PASC(asc_scaler>>4)	//����ASCԤ��Ƶ����
			|SPI_CTAR_ASC(asc_scaler)		//����ASC��Ƶ����
			|SPI_CTAR_PDT(dt_scaler>>4)		//����DTԤ��Ƶ����
			|SPI_CTAR_DT(dt_scaler));		//����DT��Ƶ����

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

//==========================================================================
//��������: spi_master_send
//��������: ���յ������ݣ���Ч����λ�����������õ�data_size����
//����˵��: mod:SPIģ���:
//             SPI_MODx��xΪģ���;
//         config:����ʱ��ʹ�õ����ú�:
//                SPI_CONFIGx��xΪ���úţ�x��ȡֵΪ0��1;
//         cs:����ʱ��Ƭѡ�ţ���ʹ�ܸ�Ƭѡ�ź�:
//            SPI_CSx��xΪƬѡ��;
//         data:Ҫ���͵����ݣ���Ч����λ�����������õ�data_size����
//         cont:�Ƿ�ʹ����������:
//              SPI_CONT_ENABLE: ʹ����������;
//              SPI_CONT_DISABLE:�ر���������;
//���ܸ�Ҫ: ������ģʽ�£���ӻ��������ݣ������ؽ��յ�������
//��ע: �������Ͷ���λ���ݣ����յ���ͬλ�����ݣ����յ������ݳ���Чλ���Ϊ0;
//
//     �ر���������ʱ��Ƭѡ�ź���Ч�󣬵ȴ�CSC��ʱʱ�䣬��ʼ�������ݣ�
//     һ֡���ݴ�����Ϻ󣬰�˳��ȴ�ASC+DT+CSC��ʱʱ�䣬��ʼ������һ֡���ݣ�
//     ����DT��ʱʱ���ڼ䣬Ƭѡ�źŽ�������Ч;
//
//     ������������ʱ�������ڼ�Ƭѡ�źŽ�һֱ������Ч��
//     ��֮֡���DT��ʱʱ�佫���õȴ�����ֻ��ȴ�ASC+CSC��ʱʱ�䣬
//     �����͵�ÿһ֡����ʹ����ͬ��Ƭѡ�źţ������л����úţ�
//     �����ü��֡����λ�����Բ�ͬ�⣬�������ñ�����ͬ��
//     �ڷ������һ֡ʱ������ر���������
//==========================================================================
uint16 spi_master_send(uint8 mod, uint8 config, uint8 cs, uint16 data,
		uint8 cont) {
	SPI_Type * spi_ptr;	//SPI����ַ

	//��ȡSPI����ַ
	spi_ptr = spi_table[mod];

	REG_SET_VAL(SPI_PUSHR_REG(spi_ptr),
			 SPI_PUSHR_CTAS(config)	//��������CTAR�Ĵ���������������
			|SPI_PUSHR_PCS(1<<cs)	//������ЧƬѡ�ź�
			|SPI_PUSHR_TXDATA(data)	//���÷�������
			|SPI_PUSHR_CONT(cont));	//�����Ƿ�ʹ����������
	//�ȴ��������
	while (!REG_GET_MASK(SPI_SR_REG(spi_ptr), SPI_SR_TCF_MASK)) {
	}
	//���������ɱ�־
	REG_SET_MASK(SPI_SR_REG(spi_ptr), SPI_SR_TCF_MASK);
	//���ؽ��յ�������
	return SPI_POPR_REG(spi_ptr);
}

//==========================================================================
//��������: spi_slave_init
//��������: ��
//����˵��: mod:SPIģ���:
//             SPI_MODx��xΪģ���;
//         format:֡��ʽ:
//                SPI_FORMAT_CPOL0_CPHA0:ƽʱ�͵�ƽ����һ����ȡ�����ڶ�������������;
//                SPI_FORMAT_CPOL0_CPHA1:ƽʱ�͵�ƽ����һ�����������ߣ��ڶ�����ȡ��;
//                SPI_FORMAT_CPOL1_CPHA0:ƽʱ�ߵ�ƽ����һ����ȡ�����ڶ�������������;
//                SPI_FORMAT_CPOL1_CPHA1:ƽʱ�ߵ�ƽ����һ�����������ߣ��ڶ�����ȡ��;
//         data_size:һ֡������λ������ȡֵΪ[4,32]��һ��Ϊ8��16����ÿ�η���1��2���ֽ�
//���ܸ�Ҫ: ��ʼ��SPIģ�飬������Ϊ�ӻ�ģʽ
//��ע: SPIģ�鱻����Ϊ�ӻ�ģʽʱ�������Ա���ʼ��һ�Σ�Ĭ��ʹ��0������;
//     Ϊ�˿�����ȷ�������ݣ�֡��ʽ��֡����λ����������һ�£��Ҵӻ���֧��MSB
//==========================================================================
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

	REG_CLR_MASK(SPI_MCR_REG(spi_ptr),
			 SPI_MCR_MSTR_MASK		//����SPIģ��Ϊ�ӻ�ģʽ
			|SPI_MCR_DIS_TXF_MASK	//����FIFO���Ͷ���
			|SPI_MCR_DIS_RXF_MASK	//����FIFO���ն���
			|SPI_MCR_CLR_TXF_MASK	//�����FIFO���ͼ�����
			|SPI_MCR_CLR_RXF_MASK);	//�����FIFO���ռ�����

	//���ø�Ƭѡ�ź���Ч��ƽΪ�͵�ƽ
	REG_SET_MASK(SPI_MCR_REG(spi_ptr), SPI_MCR_PCSIS_MASK);

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

//==========================================================================
//��������: spi_slave_send
//��������: true:���ͳɹ�; false:����ʧ�ܣ������Ͷ�������
//����˵��: mod:SPIģ���:
//             SPI_MODx��xΪģ���;
//         data:Ҫ���͵����ݣ���Ч����λ�����������õ�data_size����
//���ܸ�Ҫ: �ڴӻ�ģʽ�£���������������
//��ע: �������������յ��ӻ����͵����ݣ����ݽ����������Ͷ����У���˳��ȴ����ͣ�
//     ����������ӻ���������ʱ���ӻ������ݲŻᷢ��������;
//     ���Ͷ������Ϊ5���������5֡�ȴ�����;
//     ���Ͷ���������ʱ���������յ��������һ֡����ʼΪ0x00
//==========================================================================
bool spi_slave_send(uint8 mod, uint32 data) {
	SPI_Type * spi_ptr;	//SPI����ַ

	//��ȡSPI����ַ
	spi_ptr = spi_table[mod];

	//�жϷ��Ͷ����Ƿ�Ϊ��
	if (REG_GET_MASK(SPI_SR_REG(spi_ptr), SPI_SR_TFFF_MASK)) {
		//Ϊ��ʱ����ӷ�������
		REG_SET_VAL(SPI_PUSHR_SLAVE_REG(spi_ptr), data);
		//����������Ͷ��пձ�־�������в�Ϊ�գ����Ի���λ
		REG_SET_MASK(SPI_SR_REG(spi_ptr), SPI_SR_TFFF_MASK);
		//���ͳɹ�
		return true;
	} else {
		//��Ϊ��ʱ������ʧ��
		return false;
	}
}

//==========================================================================
//��������: spi_slave_re
//��������: true:���ճɹ�; false:����ʧ�ܣ������ն���Ϊ��
//����˵��: mod:SPIģ���:
//             SPI_MODx��xΪģ���;
//         data:����������ݵĵ�ַ����Ч����λ�����������õ�data_size����
//���ܸ�Ҫ: �ڴӻ�ģʽ�£������������͵�����
//��ע: �������͵����ݻᱻ���ڴӻ��Ľ��ն����У���˳��ȴ�ȡ��;
//     ���ն������Ϊ5���������5֡�ȴ�ȡ��;
//     �ӻ����յ�����λ���ݣ���ζ�Ŵӻ�����ͬλ�������ݷ���������
//==========================================================================
bool spi_slave_re(uint8 mod, uint32* data) {
	SPI_Type * spi_ptr;	//SPI����ַ

	//��ȡSPI����ַ
	spi_ptr = spi_table[mod];

	//�жϽ��ն����Ƿ�Ϊ��
	if (REG_GET_MASK(SPI_SR_REG(spi_ptr), SPI_SR_RFDF_MASK)) {
		//��Ϊ��ʱ���ӽ��ն���ȡ�������ܵ�������
		*data = SPI_POPR_REG(spi_ptr);
		//����������ն��в�Ϊ�ձ�־�������в�Ϊ�գ����Ի���λ
		REG_SET_MASK(SPI_SR_REG(spi_ptr), SPI_SR_RFDF_MASK);
		//���ճɹ�
		return true;
	} else {
		//Ϊ��ʱ������ʧ��
		return false;
	}
}

//==========================================================================
//��������: spi_slave_enable_re_int
//��������: ��
//����˵��: mod:SPIģ���:
//             SPI_MODx��xΪģ���;
//���ܸ�Ҫ: �ڴӻ�ģʽ�£�ʹ�ܽ����ж�
//��ע: �����ն��в�Ϊ��ʱ���������жϣ�ͨ���������ݿ�������жϱ�־
//==========================================================================
void spi_slave_enable_re_int(uint8 mod) {
	//ʹ�ܽ��ն��в�Ϊ���ж�
	REG_SET_MASK(SPI_RSER_REG(spi_table[mod]), SPI_RSER_RFDF_RE_MASK);
	//��������ж�
	ENABLE_IRQ(spi_irq_table[mod]);
}

//==========================================================================
//��������: spi_slave_disable_re_int
//��������: ��
//����˵��: mod:SPIģ���:
//             SPI_MODx��xΪģ���;
//���ܸ�Ҫ: �ڴӻ�ģʽ�£��رս����ж�
//==========================================================================
void spi_slave_disable_re_int(uint8 mod) {
	//�رս��ն��в�Ϊ���ж�
	REG_CLR_MASK(SPI_RSER_REG(spi_table[mod]), SPI_RSER_RFDF_RE_MASK);
	//��ֹ�����ж�
	DISABLE_IRQ(spi_irq_table[mod]);
}
