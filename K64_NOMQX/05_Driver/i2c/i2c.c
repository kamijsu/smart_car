//==========================================================================
//�ļ����ƣ�i2c.c
//���ܸ�Ҫ��K64 I2C�ײ���������Դ�ļ�
//==========================================================================

#include "i2c.h"

//I2C��ģ�����ַ
static I2C_Type * const i2c_table[] = I2C_BASE_PTRS;
//I2C��ģ��ʱ���źŵ����ź�
static const uint8 i2c_scl_pin_table[] = { I2C_MOD0_SCL_PIN, I2C_MOD1_SCL_PIN,
I2C_MOD2_SCL_PIN };
//I2C��ģ��ʱ���źŵ�PCR��MUXֵ
static const uint8 i2c_scl_pcr_mux_table[] = { I2C_MOD0_SCL_PCR_MUX,
I2C_MOD1_SCL_PCR_MUX, I2C_MOD2_SCL_PCR_MUX };
//I2C��ģ�������źŵ����ź�
static const uint8 i2c_sda_pin_table[] = { I2C_MOD0_SDA_PIN, I2C_MOD1_SDA_PIN,
I2C_MOD2_SDA_PIN };
//I2C��ģ�������źŵ�PCR��MUXֵ
static const uint8 i2c_sda_pcr_mux_table[] = { I2C_MOD0_SDA_PCR_MUX,
I2C_MOD1_SDA_PCR_MUX, I2C_MOD2_SDA_PCR_MUX };
//I2C��ģ���ж������
static const IRQn_Type i2c_irq_table[] = { I2C0_IRQn, I2C1_IRQn, I2C2_IRQn };

static void i2c_send_start(I2C_Type* i2c_ptr) {
	REG_SET_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_MST_MASK|I2C_C1_TX_MASK);
}

static void i2c_send_restart(I2C_Type* i2c_ptr) {
	REG_SET_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_RSTA_MASK);
}

static void i2c_send_stop(I2C_Type* i2c_ptr) {
	REG_CLR_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_MST_MASK|I2C_C1_TX_MASK);
}

static void i2c_set_ack(I2C_Type* i2c_ptr, bool ack) {
	if (ack) {
		REG_CLR_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_TXAK_MASK);
	} else {
		REG_SET_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_TXAK_MASK);
	}
}

#define I2C_DIR_WRITE	(0x0)
#define I2C_DIR_READ	(0x1)

void i2c_send_addr_7bits(I2C_Type* i2c_ptr, uint8 addr, uint8 dir) {
	REG_SET_VAL(I2C_D_REG(i2c_ptr), I2C_D_DATA((addr<<1)|dir));
}

static void i2c_send_data(I2C_Type* i2c_ptr, uint8 data) {
	REG_SET_VAL(I2C_D_REG(i2c_ptr), I2C_D_DATA(data));
}

static uint8 i2c_re_data(I2C_Type* i2c_ptr) {
	return I2C_D_REG(i2c_ptr);
}

static I2CResult i2c_wait_ack(I2C_Type* i2c_ptr) {
	uint8 status;
	do {
		status = I2C_S_REG(i2c_ptr);
	} while (!REG_GET_MASK(status, I2C_S_IICIF_MASK));
	REG_SET_VAL(I2C_S_REG(i2c_ptr), status);
	if (REG_GET_MASK(status, I2C_S_ARBL_MASK)) {
		return I2CArbitrationLost;
	}
	if (REG_GET_MASK(status, I2C_S_RXAK_MASK)) {
		return I2CNAck;
	} else {
		return I2CSuccess;
	}
}

//icr:ʱ������
void i2c_init(uint8 mod, uint8 mul, uint8 icr, uint8 addr_mode, uint16 addr,
		bool enable_general_call_addr) {
	I2C_Type * i2c_ptr;	//I2C����ַ

	//��ȡI2C����ַ
	i2c_ptr = i2c_table[mod];

	//������ӦI2Cģ��ʱ����
	switch (mod) {
	case I2C_MOD0:
		REG_SET_MASK(SIM_SCGC4, SIM_SCGC4_I2C0_MASK);
		break;
	case I2C_MOD1:
		REG_SET_MASK(SIM_SCGC4, SIM_SCGC4_I2C1_MASK);
		break;
	case I2C_MOD2:
		REG_SET_MASK(SIM_SCGC1, SIM_SCGC1_I2C2_MASK);
		break;
	}

	//��ʱ����I2Cģ��
	REG_CLR_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_IICEN_MASK);

	//ʹ��ʱ���ź����Ź���
	com_port_pin_set_mux(i2c_scl_pin_table[mod], i2c_scl_pcr_mux_table[mod]);
	//ʹ�������ź����Ź���
	com_port_pin_set_mux(i2c_sda_pin_table[mod], i2c_sda_pcr_mux_table[mod]);

	//���ó������Ӻ�ʱ������
	REG_SET_VAL(I2C_F_REG(i2c_ptr), I2C_F_MULT(mul)|I2C_F_ICR(icr));

	//���õ�ַģʽ����ַ
	if (addr_mode == I2C_ADDR_MODE_BITS_7) {
		//����Ϊ7λ��ַģʽ
		REG_CLR_MASK(I2C_C2_REG(i2c_ptr), I2C_C2_ADEXT_MASK);
	} else {
		//����Ϊ10λ��ַģʽ
		REG_SET_MASK(I2C_C2_REG(i2c_ptr), I2C_C2_ADEXT_MASK);
		//���ø�3λ��ַ
		REG_CLR_MASK(I2C_C2_REG(i2c_ptr), I2C_C2_AD_MASK);
		REG_SET_MASK(I2C_C2_REG(i2c_ptr), I2C_C2_AD(addr>>I2C_A1_AD_WIDTH));
	}
	//����7λ��ַ
	REG_SET_VAL(I2C_A1_REG(i2c_ptr), I2C_A1_AD(addr));

	if (enable_general_call_addr) {
		//ʹ�ܹ㲥���е�ַ
		REG_SET_MASK(I2C_C2_REG(i2c_ptr), I2C_C2_GCAEN_MASK);
	} else {
		//�رչ㲥���е�ַ
		REG_CLR_MASK(I2C_C2_REG(i2c_ptr), I2C_C2_GCAEN_MASK);
	}

	//ʹ��I2Cģ��
	REG_SET_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_IICEN_MASK);
}

I2CResult i2c_master_send(uint8 mod, uint8 addr, uint8* data, uint32 len) {
	I2C_Type * i2c_ptr;	//I2C����ַ
	uint8 status;
	I2CResult result;

	//��ȡI2C����ַ
	i2c_ptr = i2c_table[mod];

	//�ر��ж�

	//���Ϳ�ʼ�ź�
	REG_SET_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_MST_MASK);
	//��ȡ״̬�Ĵ���״̬
	status = I2C_S_REG(i2c_ptr);
	//�������ٲö�ʧ������ʧ��
	if (REG_GET_MASK(status, I2C_S_ARBL_MASK)) {
		//����ٲö�ʧ���жϱ�־
		REG_SET_VAL(I2C_S_REG(i2c_ptr), status);
		return I2CArbitrationLost;
	}
	//ת��Ϊ����ģʽ
	REG_SET_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_TX_MASK);
	//����7λ��ַ��������Ϊдģʽ
	REG_SET_VAL(I2C_D_REG(i2c_ptr), I2C_D_DATA(addr<<1));
	//�ȴ����ͽ��
	result = i2c_wait_ack(i2c_ptr);
	//������ʧ��
	if (result != I2CSuccess) {
		//����ֹͣ�źţ���ת��Ϊ����ģʽ����Ϊ�ٲö�ʧ���򲻻ᷢ��ֹͣ�ź�
		REG_CLR_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_MST_MASK|I2C_C1_TX_MASK);
		//�ȴ�����æ״ֹ̬ͣ
		while (REG_GET_MASK(I2C_S_REG(i2c_ptr), I2C_S_BUSY_MASK)) {
		}
		//���ط��ͽ��
		return result;
	}
	//������������
	while (len--) {
		//��������
		REG_SET_VAL(I2C_D_REG(i2c_ptr), I2C_D_DATA(*data++));
		//�ȴ����ͽ��
		result = i2c_wait_ack(i2c_ptr);
		//������ʧ��
		if (result != I2CSuccess) {
			//����ֹͣ�źţ���ת��Ϊ����ģʽ����Ϊ�ٲö�ʧ���򲻻ᷢ��ֹͣ�ź�
			REG_CLR_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_MST_MASK|I2C_C1_TX_MASK);
			//�ȴ�����æ״ֹ̬ͣ
			while (REG_GET_MASK(I2C_S_REG(i2c_ptr), I2C_S_BUSY_MASK)) {
			}
			//���ط��ͽ��
			return result;
		}
	}
	//����ֹͣ�źţ���ת��Ϊ����ģʽ
	REG_CLR_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_MST_MASK|I2C_C1_TX_MASK);
	//�ȴ�����æ״ֹ̬ͣ
	while (REG_GET_MASK(I2C_S_REG(i2c_ptr), I2C_S_BUSY_MASK)) {
	}
	//���ͳɹ�
	return I2CSuccess;
}
