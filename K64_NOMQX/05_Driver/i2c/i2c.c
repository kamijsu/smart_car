//==========================================================================
//�ļ����ƣ�i2c.c
//���ܸ�Ҫ��K64 I2C�ײ���������Դ�ļ�
//==========================================================================

#include "i2c.h"
//����gpio.h����ΪҪ�õ��������źͿ�©�������
#include "gpio.h"

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

static uint8 i2c_re_data(I2C_Type* i2c_ptr) {
	return I2C_D_REG(i2c_ptr);
}

static I2CResult i2c_wait_ack(I2C_Type* i2c_ptr) {
	uint8 status;	//״̬�Ĵ�����ֵ
	uint32 timeout;	//�ӻ���Ӧ��ʱ��ֵ

	//�ȴ��ӻ���Ӧ
	timeout = 0;
	do {
		//Լ4s������������������ʱ��ʹӻ���Ӧʱ�䣬��Ϊ��ʼ�źź��״η��ͣ����������ʼ�źű���ʱ��
		if (timeout++ >= 0x2000000u) {
			return I2CTimeout;
		}
		status = I2C_S_REG(i2c_ptr);
	} while (!REG_GET_MASK(status, I2C_S_IICIF_MASK));
	//����жϱ�־
	REG_SET_VAL(I2C_S_REG(i2c_ptr), status);

	//�������ٲö�ʧ
	if (REG_GET_MASK(status, I2C_S_ARBL_MASK)) {
		//�����ٲö�ʧ
		return I2CArbitrationLost;
	}
	//�жϴӻ��Ƿ���Ӧ
	if (REG_GET_MASK(status, I2C_S_RXAK_MASK)) {
		//����δ��Ӧ
		return I2CNAck;
	} else {
		//���ط��ͳɹ�
		return I2CSuccess;
	}
}

//icr:ʱ������
void i2c_init(uint8 mod, uint8 mul, uint8 icr, uint8 addr_mode, uint16 addr,
		bool enable_general_call_addr) {
	I2C_Type * i2c_ptr;	//I2C����ַ
	uint8 scl_pin, sda_pin;	//ʱ���źź������ź�����

	//��ȡI2C����ַ
	i2c_ptr = i2c_table[mod];
	//��ȡʱ���źź������ź�����
	scl_pin = i2c_scl_pin_table[mod];
	sda_pin = i2c_sda_pin_table[mod];

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

	//��ʱ����I2Cģ�飬���ر�I2C�жϣ�����Ϊ�ӻ�����ģʽ��������Ӧ���ź�
	REG_CLR_MASK(I2C_C1_REG(i2c_ptr),
			I2C_C1_IICEN_MASK|I2C_C1_IICIE_MASK|I2C_C1_MST_MASK|I2C_C1_TX_MASK|I2C_C1_TXAK_MASK);

	//ʹ��ʱ���ź����Ź���
	com_port_pin_set_mux(scl_pin, i2c_scl_pcr_mux_table[mod]);
	//ʹ�������ź����Ź���
	com_port_pin_set_mux(sda_pin, i2c_sda_pcr_mux_table[mod]);
	//����������������ʹ��
	gpio_set_pull(scl_pin, GPIO_LEVEL_HIGH);
	gpio_set_pull(sda_pin, GPIO_LEVEL_HIGH);
	//�������ſ�©���ʹ��
	gpio_set_open_drain(scl_pin, true);
	gpio_set_open_drain(sda_pin, true);

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
	I2CResult result;	//���ͽ��
	bool enable_int;	//�Ƿ�ʹ�ܽ����ж�

	//��ȡI2C����ַ
	i2c_ptr = i2c_table[mod];

	//�ж��Ƿ�ʹ�ܽ����ж�
	if (REG_GET_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_IICIE_MASK)) {
		//ʹ�ܽ����ж�
		enable_int = true;
		//��ʱ�رս����ж�
		REG_CLR_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_IICIE_MASK);
	} else {
		//δʹ�ܽ����ж�
		enable_int = false;
	}

	//���Ϳ�ʼ�źţ���ת��Ϊ����ģʽ
	REG_SET_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_MST_MASK|I2C_C1_TX_MASK);
	//�ȴ���ʼ�ź���Ч
	while (!REG_GET_MASK(I2C_S_REG(i2c_ptr), I2C_S_BUSY_MASK)) {
	}

	//����7λ��ַ��������Ϊдģʽ
	REG_SET_VAL(I2C_D_REG(i2c_ptr), I2C_D_DATA(addr<<1));
	//�ȴ����ͽ��
	result = i2c_wait_ack(i2c_ptr);
	//������ʧ��
	if (result != I2CSuccess) {
		//ֹͣ��������
		len = 0;
	}

	//������������
	while (len--) {
		//��������
		REG_SET_VAL(I2C_D_REG(i2c_ptr), I2C_D_DATA(*data++));
		//�ȴ����ͽ��
		result = i2c_wait_ack(i2c_ptr);
		//������ʧ��
		if (result != I2CSuccess) {
			//ֹͣ��������
			break;
		}
	}

	//����ֹͣ�źţ���ת��Ϊ����ģʽ
	REG_CLR_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_MST_MASK|I2C_C1_TX_MASK);
	//�ָ������ж�
	if (enable_int) {
		REG_SET_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_IICIE_MASK);
	}
	if (result == I2CSuccess || result == I2CNAck) {
		//�ȴ�ֹͣ�ź���Ч
		while (REG_GET_MASK(I2C_S_REG(i2c_ptr), I2C_S_BUSY_MASK)) {
		}
	}
	//���ط��ͽ��
	return result;
}

void i2c_slave_enable_int(uint8 mod) {
	REG_SET_MASK(I2C_C1_REG(i2c_table[mod]), I2C_C1_IICIE_MASK);
	//��������ж�
	ENABLE_IRQ(i2c_irq_table[mod]);
}

void i2c_slave_disable_int(uint8 mod) {
	REG_CLR_MASK(I2C_C1_REG(i2c_table[mod]), I2C_C1_IICIE_MASK);
	//��������ж�
	DISABLE_IRQ(i2c_irq_table[mod]);
}

//�ӻ���ַƥ��ʱ(�����㲥��ַ)һ������Ӧ���źţ�ƥ�䵽�㲥��ַ�󣬽��յ�����һ������Ӧ���źţ�����Ӧ���ź����ͺ���һ�η���Ӧ���ź�����ʱ��Ч
void i2c_slave_set_ack(uint8 mod, bool ack) {
	if (ack) {
		REG_CLR_MASK(I2C_C1_REG(i2c_table[mod]), I2C_C1_TXAK_MASK);
	} else {
		REG_SET_MASK(I2C_C1_REG(i2c_table[mod]), I2C_C1_TXAK_MASK);
	}
}

//�ӻ��жϷ���ʱ��Ӧ���ź��Ѿ��������÷��ͳ�ȥ����δ��/д���ݼĴ���ʱ�����߻ᱻ�ӻ�ռ�ݣ�����������һ�δ�������ʱ�ȴ��ӻ��ͷ�����
I2CSlaveIntType i2c_slave_handle_int(uint8 mod) {
	I2C_Type * i2c_ptr;	//I2C����ַ
	uint8 status;
	I2CSlaveIntType int_type;

	//��ȡI2C����ַ
	i2c_ptr = i2c_table[mod];
	status = I2C_S_REG(i2c_ptr);

//	uart_printf(1, "�ӻ�״̬:%X\r\n", status);
	if (!REG_GET_MASK(status, I2C_S_IICIF_MASK)) {
		int_type = I2CSlaveNoInt;
	} else if (REG_GET_MASK(status, I2C_S_ARBL_MASK)) {
		int_type = I2CSlaveArbitrationLostInt;
	} else if (REG_GET_MASK(status, I2C_S_IAAS_MASK)) {
//		REG_CLR_MASK(status, I2C_S_IAAS_MASK);
		//дC1�Ĵ����������IAAS��־
		if (REG_GET_MASK(status, I2C_S_SRW_MASK)) {
			int_type = I2CSlaveCalledSendInt;
			REG_SET_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_TX_MASK);
		} else {
			REG_CLR_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_TX_MASK);
			if (i2c_slave_re_data(mod) == 0) {
				int_type = I2CSlaveGeneralCalledInt;
			} else {
				int_type = I2CSlaveCalledReInt;
			}
		}
	} else {
		if (REG_GET_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_TX_MASK)) {
			int_type = I2CSlaveSendDataInt;
		} else {
			int_type = I2CSlaveReDataInt;
		}
	}

	REG_SET_VAL(I2C_S_REG(i2c_ptr), status);
	return int_type;
}

uint8 i2c_slave_re_data(uint8 mod) {
	return I2C_D_REG(i2c_table[mod]);
}
