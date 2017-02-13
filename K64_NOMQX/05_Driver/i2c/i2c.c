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

static I2CMasterResult i2c_master_send_byte(I2C_Type* i2c_ptr, uint8 byte,
		uint8 old_c1) {
	uint8 status;	//״̬�Ĵ�����ֵ
	uint32 timeout;	//��Ӧ��ʱ��ֵ
	I2CMasterResult error_type;	//���ʹ�������

	//����һ�ֽ�
	REG_SET_VAL(I2C_D_REG(i2c_ptr), I2C_D_DATA(byte));

	//�ȴ��ӻ���Ӧ������������������ʱ��ʹӻ���Ӧʱ�䣬
	//��Ϊ�״η��ͣ���������ȴ���ʼ�ź���Чʱ��Ϳ�ʼ�źű���ʱ��
	timeout = 0;
	do {
		//���ӻ���Ӧ��ʱ
		if (timeout++ >= I2C_TIMEOUT_MAX) {
			//��ת��������
			error_type = I2CMasterTimeoutSlave;
			goto error_end;
		}
		//��ȡ״̬�Ĵ�����ֵ
		status = I2C_S_REG(i2c_ptr);
	} while (!REG_GET_MASK(status, I2C_S_IICIF_MASK));

	//����жϱ�־
	REG_SET_VAL(I2C_S_REG(i2c_ptr), status);

	//�������ٲö�ʧ
	if (REG_GET_MASK(status, I2C_S_ARBL_MASK)) {
		//��ת��������
		error_type = I2CMasterArbitrationLost;
		goto error_end;
	}

	//���ӻ�δ��Ӧ
	if (REG_GET_MASK(status, I2C_S_RXAK_MASK)) {
		//��ת��������δ��Ӧʱ��Ҫ����ֹͣ�ź�
		error_type = I2CMasterNAck;
		goto error_stop;
	}

	//���ط��ͳɹ�
	return I2CMasterSuccess;

	error_stop:
	//����ֹͣ�ź�
	REG_CLR_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_MST_MASK);
	//����timeout
	timeout = 0;
	//�ȴ�ֹͣ�ź���Ч
	while (REG_GET_MASK(I2C_S_REG(i2c_ptr), I2C_S_BUSY_MASK)) {
		//ֹͣ�ź���Ӧ��ʱ�����ӻ�δ��Ӧ�������һ���ֽ�����
		if (++timeout >= I2C_TIMEOUT_MAX) {
			error_type = I2CMasterTimeoutStop;
			break;
		}
	}

	error_end:
	//�ָ��ɿ��ƼĴ���C1��ֵ
	REG_SET_VAL(I2C_C1_REG(i2c_ptr), old_c1);
	//���ط��ʹ�������
	return error_type;
}

static I2CMasterResult i2c_master_re_byte(I2C_Type* i2c_ptr, uint8* byte,
		uint8 old_c1) {
	uint8 status;	//״̬�Ĵ�����ֵ
	uint32 timeout;	//��Ӧ��ʱ��ֵ
	I2CMasterResult error_type;	//���մ�������

	//����һ�ֽ�
	*byte = I2C_D_REG(i2c_ptr);

	//�ȴ��ӻ���Ӧ�������ӻ���Ӧʱ��ʹӻ���������ʱ��
	timeout = 0;
	do {
		//���ӻ���Ӧ��ʱ
		if (timeout++ >= I2C_TIMEOUT_MAX) {
			//��ת��������
			error_type = I2CMasterTimeoutSlave;
			goto error_end;
		}
		//��ȡ״̬�Ĵ�����ֵ
		status = I2C_S_REG(i2c_ptr);
	} while (!REG_GET_MASK(status, I2C_S_IICIF_MASK));

	//����жϱ�־
	REG_SET_VAL(I2C_S_REG(i2c_ptr), status);

	//�������ٲö�ʧ
	if (REG_GET_MASK(status, I2C_S_ARBL_MASK)) {
		//��ת��������
		error_type = I2CMasterArbitrationLost;
		goto error_end;
	}

	//���ؽ��ճɹ�
	return I2CMasterSuccess;

	error_end:
	//�ָ��ɿ��ƼĴ���C1��ֵ
	REG_SET_VAL(I2C_C1_REG(i2c_ptr), old_c1);
	//���ط��ʹ�������
	return error_type;
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

I2CMasterResult i2c_master_send(uint8 mod, uint8 addr_mode, uint16 addr,
		uint8* data, uint32 len) {
	I2C_Type * i2c_ptr;		//I2C����ַ
	I2CMasterResult result;	//���ͽ��
	uint8 old_c1;			//�ɿ��ƼĴ���C1��ֵ
	uint32 timeout;			//ֹͣ�ź���Ӧ��ʱ��ֵ

	//��ȡI2C����ַ
	i2c_ptr = i2c_table[mod];
	//��ȡ�ɿ��ƼĴ���C1��ֵ
	old_c1 = I2C_C1_REG(i2c_ptr);

	//����I2Cģ���Ѿ�������Ϊ�������������ڹ���
	if (REG_GET_MASK(old_c1, I2C_C1_MST_MASK)) {
		return I2CMasterIsBusy;
	}

	//��ʱ�ر��ж�
	REG_CLR_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_IICIE_MASK);

	//���Ϳ�ʼ�źţ���ת��Ϊ����ģʽ
	REG_SET_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_MST_MASK|I2C_C1_TX_MASK);

	//�жϵ�ַģʽ
	if (addr_mode == I2C_ADDR_MODE_BITS_10) {
		//���ձ�׼����10λ��ַ�ĸ�2λ��������Ϊдģʽ
		result = i2c_master_send_byte(i2c_ptr, 0xF0 | ((addr & 0x300) >> 7),
				old_c1);
		//ʧ��ʱ�����ط��ͽ��
		if (result != I2CMasterSuccess) {
			return result;
		}
		//����10λ��ַ�ĵ�8λ
		result = i2c_master_send_byte(i2c_ptr, addr, old_c1);
	} else {
		//����7λ��ַ��������Ϊдģʽ
		result = i2c_master_send_byte(i2c_ptr, addr << 1, old_c1);
	}
	//ʧ��ʱ�����ط��ͽ��
	if (result != I2CMasterSuccess) {
		return result;
	}

	//������������
	while (len--) {
		result = i2c_master_send_byte(i2c_ptr, *data++, old_c1);
		//ʧ��ʱ�����ط��ͽ��
		if (result != I2CMasterSuccess) {
			return result;
		}
	}

	//����ֹͣ�ź�
	REG_CLR_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_MST_MASK);
	//��ʼ��timeout
	timeout = 0;
	//�ȴ�ֹͣ�ź���Ч
	while (REG_GET_MASK(I2C_S_REG(i2c_ptr), I2C_S_BUSY_MASK)) {
		//ֹͣ�ź���Ӧ��ʱ�����ӻ�δ��Ӧ�������һ���ֽ�����
		if (++timeout >= I2C_TIMEOUT_MAX) {
			result = I2CMasterTimeoutStop;
			break;
		}
	}

	//�ָ��ɿ��ƼĴ���C1��ֵ
	REG_SET_VAL(I2C_C1_REG(i2c_ptr), old_c1);
	//���ط��ͽ��
	return result;
}

I2CMasterResult i2c_master_send_re(uint8 mod, uint8 addr_mode, uint16 addr,
		uint8* send_data, uint32 send_len, uint8* re_data, uint32 re_len) {
	I2C_Type * i2c_ptr;		//I2C����ַ
	I2CMasterResult result;	//���ͽ��ս��
	uint8 old_c1;			//�ɿ��ƼĴ���C1��ֵ
	uint32 timeout;			//ֹͣ�ź���Ӧ��ʱ��ֵ
	uint8 dummy;			//��Ч������

	//��ȡI2C����ַ
	i2c_ptr = i2c_table[mod];
	//��ȡ�ɿ��ƼĴ���C1��ֵ
	old_c1 = I2C_C1_REG(i2c_ptr);

	//����I2Cģ���Ѿ�������Ϊ�������������ڹ���
	if (REG_GET_MASK(old_c1, I2C_C1_MST_MASK)) {
		return I2CMasterIsBusy;
	}

	//��ʱ�ر��ж�
	REG_CLR_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_IICIE_MASK);

	//���Ϳ�ʼ�źţ���ת��Ϊ����ģʽ
	REG_SET_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_MST_MASK|I2C_C1_TX_MASK);

	//�жϵ�ַģʽ
	if (addr_mode == I2C_ADDR_MODE_BITS_10) {
		//���ձ�׼����10λ��ַ�ĸ�2λ��������Ϊдģʽ
		result = i2c_master_send_byte(i2c_ptr, 0xF0 | ((addr & 0x300) >> 7),
				old_c1);
		//ʧ��ʱ�����ؽ��
		if (result != I2CMasterSuccess) {
			return result;
		}
		//����10λ��ַ�ĵ�8λ
		result = i2c_master_send_byte(i2c_ptr, addr, old_c1);
	} else {
		//����7λ��ַ��������Ϊдģʽ
		result = i2c_master_send_byte(i2c_ptr, addr << 1, old_c1);
	}
	//ʧ��ʱ�����ؽ��
	if (result != I2CMasterSuccess) {
		return result;
	}

	//������������
	while (send_len--) {
		result = i2c_master_send_byte(i2c_ptr, *send_data++, old_c1);
		//ʧ��ʱ�����ؽ��
		if (result != I2CMasterSuccess) {
			return result;
		}
	}

	//�������¿�ʼ�ź�
	REG_SET_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_RSTA_MASK);

	//�жϵ�ַģʽ
	if (addr_mode == I2C_ADDR_MODE_BITS_10) {
		//���ձ�׼����10λ��ַ�ĸ�2λ��������Ϊ��ģʽ
		result = i2c_master_send_byte(i2c_ptr, 0xF1 | ((addr & 0x300) >> 7),
				old_c1);
	} else {
		//����7λ��ַ��������Ϊ��ģʽ
		result = i2c_master_send_byte(i2c_ptr, (addr << 1) | 1, old_c1);
	}
	//ʧ��ʱ�����ؽ��
	if (result != I2CMasterSuccess) {
		return result;
	}

	//����Ϊ����Ӧ���źţ���ת��Ϊ����ģʽ
	REG_CLR_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_TXAK_MASK|I2C_C1_TX_MASK);

	//�������������ڵ���2ʱ
	if (re_len >= 2) {
		//����1�ֽ���Ч����
		result = i2c_master_re_byte(i2c_ptr, &dummy, old_c1);
		//ʧ��ʱ�����ؽ��
		if (result != I2CMasterSuccess) {
			return result;
		}
		//����������ʣ��2�ֽ�����δ����
		for (; re_len > 2; --re_len) {
			result = i2c_master_re_byte(i2c_ptr, re_data++, old_c1);
			//ʧ��ʱ�����ؽ��
			if (result != I2CMasterSuccess) {
				return result;
			}
		}
	}

	//����Ϊ���ͷ�Ӧ���ź�
	REG_SET_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_TXAK_MASK);

	if (re_len <= 1) {
		//������������С�ڵ���1������1�ֽ���Ч����
		result = i2c_master_re_byte(i2c_ptr, &dummy, old_c1);
	} else {
		//������յ�����2���ֽ�
		result = i2c_master_re_byte(i2c_ptr, re_data++, old_c1);
	}
	//ʧ��ʱ�����ؽ��
	if (result != I2CMasterSuccess) {
		return result;
	}

	//����ֹͣ�ź�
	REG_CLR_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_MST_MASK);

	//��ȡ���һ���ֽ�����
	*re_data = I2C_D_REG(i2c_ptr);

	//��ʼ��timeout
	timeout = 0;
	//�ȴ�ֹͣ�ź���Ч
	while (REG_GET_MASK(I2C_S_REG(i2c_ptr), I2C_S_BUSY_MASK)) {
		//ֹͣ�ź���Ӧ��ʱ�����ӻ�δ��Ӧ������Ӧ���ź�
		if (++timeout >= I2C_TIMEOUT_MAX) {
			result = I2CMasterTimeoutStop;
			break;
		}
	}

	//�ָ��ɿ��ƼĴ���C1��ֵ
	REG_SET_VAL(I2C_C1_REG(i2c_ptr), old_c1);
	//���ؽ��
	return result;
}

I2CMasterResult i2c_master_re(uint8 mod, uint8 addr_mode, uint16 addr,
		uint8* data, uint32 len) {
	I2C_Type * i2c_ptr;		//I2C����ַ
	I2CMasterResult result;	//���ս��
	uint8 old_c1;			//�ɿ��ƼĴ���C1��ֵ
	uint32 timeout;			//ֹͣ�ź���Ӧ��ʱ��ֵ
	uint8 dummy;			//��Ч������

	//��ȡI2C����ַ
	i2c_ptr = i2c_table[mod];
	//��ȡ�ɿ��ƼĴ���C1��ֵ
	old_c1 = I2C_C1_REG(i2c_ptr);

	//����I2Cģ���Ѿ�������Ϊ�������������ڹ���
	if (REG_GET_MASK(old_c1, I2C_C1_MST_MASK)) {
		return I2CMasterIsBusy;
	}

	//��ʱ�ر��ж�
	REG_CLR_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_IICIE_MASK);

	//���Ϳ�ʼ�źţ���ת��Ϊ����ģʽ
	REG_SET_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_MST_MASK|I2C_C1_TX_MASK);

	//�жϵ�ַģʽ
	if (addr_mode == I2C_ADDR_MODE_BITS_10) {
		//���ձ�׼����10λ��ַ�ĸ�2λ��������Ϊдģʽ
		result = i2c_master_send_byte(i2c_ptr, 0xF0 | ((addr & 0x300) >> 7),
				old_c1);
		//ʧ��ʱ�����ؽ��
		if (result != I2CMasterSuccess) {
			return result;
		}
		//����10λ��ַ�ĵ�8λ
		result = i2c_master_send_byte(i2c_ptr, addr, old_c1);
		//ʧ��ʱ�����ؽ��
		if (result != I2CMasterSuccess) {
			return result;
		}

		//�������¿�ʼ�ź�
		REG_SET_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_RSTA_MASK);

		//���ձ�׼����10λ��ַ�ĸ�2λ��������Ϊ��ģʽ
		result = i2c_master_send_byte(i2c_ptr, 0xF1 | ((addr & 0x300) >> 7),
				old_c1);
	} else {
		//����7λ��ַ��������Ϊ��ģʽ
		result = i2c_master_send_byte(i2c_ptr, (addr << 1) | 1, old_c1);
	}
	//ʧ��ʱ�����ؽ��
	if (result != I2CMasterSuccess) {
		return result;
	}

	//����Ϊ����Ӧ���źţ���ת��Ϊ����ģʽ
	REG_CLR_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_TXAK_MASK|I2C_C1_TX_MASK);

	//�������������ڵ���2ʱ
	if (len >= 2) {
		//����1�ֽ���Ч����
		result = i2c_master_re_byte(i2c_ptr, &dummy, old_c1);
		//ʧ��ʱ�����ؽ��
		if (result != I2CMasterSuccess) {
			return result;
		}
		//����������ʣ��2�ֽ�����δ����
		for (; len > 2; --len) {
			result = i2c_master_re_byte(i2c_ptr, data++, old_c1);
			//ʧ��ʱ�����ؽ��
			if (result != I2CMasterSuccess) {
				return result;
			}
		}
	}

	//����Ϊ���ͷ�Ӧ���ź�
	REG_SET_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_TXAK_MASK);

	if (len <= 1) {
		//������������С�ڵ���1������1�ֽ���Ч����
		result = i2c_master_re_byte(i2c_ptr, &dummy, old_c1);
	} else {
		//������յ�����2���ֽ�
		result = i2c_master_re_byte(i2c_ptr, data++, old_c1);
	}
	//ʧ��ʱ�����ؽ��
	if (result != I2CMasterSuccess) {
		return result;
	}

	//����ֹͣ�ź�
	REG_CLR_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_MST_MASK);

	//��ȡ���һ���ֽ�����
	*data = I2C_D_REG(i2c_ptr);

	//��ʼ��timeout
	timeout = 0;
	//�ȴ�ֹͣ�ź���Ч
	while (REG_GET_MASK(I2C_S_REG(i2c_ptr), I2C_S_BUSY_MASK)) {
		//ֹͣ�ź���Ӧ��ʱ�����ӻ�δ��Ӧ������Ӧ���ź�
		if (++timeout >= I2C_TIMEOUT_MAX) {
			result = I2CMasterTimeoutStop;
			break;
		}
	}

	//�ָ��ɿ��ƼĴ���C1��ֵ
	REG_SET_VAL(I2C_C1_REG(i2c_ptr), old_c1);
	//���ؽ��
	return result;
}

void i2c_slave_enable_int(uint8 mod) {
	//ʹ�ܴӻ��ж�
	REG_SET_MASK(I2C_C1_REG(i2c_table[mod]), I2C_C1_IICIE_MASK);
	//��������ж�
	ENABLE_IRQ(i2c_irq_table[mod]);
}

void i2c_slave_disable_int(uint8 mod) {
	//�رմӻ��ж�
	REG_CLR_MASK(I2C_C1_REG(i2c_table[mod]), I2C_C1_IICIE_MASK);
	//��ֹ�����ж�
	DISABLE_IRQ(i2c_irq_table[mod]);
}

//�ӻ���ַƥ��ʱ(�����㲥��ַ)һ������Ӧ���źţ�ƥ�䵽�㲥��ַ�󣬽��յ�����һ������Ӧ���źţ�����Ӧ���ź����ͺ���һ�η���Ӧ���ź�����ʱ��Ч
void i2c_slave_set_ack(uint8 mod, bool ack) {
	if (ack) {
		//����Ӧ���ź�
		REG_CLR_MASK(I2C_C1_REG(i2c_table[mod]), I2C_C1_TXAK_MASK);
	} else {
		//���ͷ�Ӧ���ź�
		REG_SET_MASK(I2C_C1_REG(i2c_table[mod]), I2C_C1_TXAK_MASK);
	}
}

//�ӻ��жϷ���ʱ��Ӧ���ź��Ѿ��������÷��ͳ�ȥ����δ��/д���ݼĴ���ʱ�����߻ᱻ�ӻ�ռ�ݣ�����������һ�δ�������ʱ�ȴ��ӻ��ͷ�����
I2CSlaveIntType i2c_slave_handle_int(uint8 mod) {
	I2C_Type * i2c_ptr;	//I2C����ַ
	uint8 status;		//״̬�Ĵ�����ֵ
	volatile uint8 dummy;	//��Ч������

	//��ȡI2C����ַ
	i2c_ptr = i2c_table[mod];
	//��ȡ״̬�Ĵ�����ֵ
	status = I2C_S_REG(i2c_ptr);

	//�鿴�жϱ�־�Ƿ���λ
	if (!REG_GET_MASK(status, I2C_S_IICIF_MASK)) {
		//�������жϷ���
		return I2CSlaveNoInt;
	}

	//����жϱ�־
	REG_SET_VAL(I2C_S_REG(i2c_ptr), status);

	//�鿴�Ƿ�ѰַΪ�ӻ�
	if (REG_GET_MASK(status, I2C_S_IAAS_MASK)) {
		//дC1�Ĵ����������IAAS��־
		//��ѰַΪ�ӻ�ʱ���鿴�������ͷ���
		if (REG_GET_MASK(status, I2C_S_SRW_MASK)) {
			//������ȡ����ʱ���ӻ�ת��Ϊ����ģʽ
			REG_SET_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_TX_MASK);
			//���ر�Ѱַ�����ж�
			return I2CSlaveCalledSendInt;
		} else {
			//������������ʱ���ӻ�ת��Ϊ����ģʽ
			REG_CLR_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_TX_MASK);
			//�����յĵ�ַΪ0x00���Ҵӻ�Ϊ7λ��ַģʽ
			if (I2C_D_REG(i2c_table[mod])
					== 0&& !REG_GET_MASK(I2C_C2_REG(i2c_ptr),I2C_C2_ADEXT_MASK)) {
				//���ر��㲥Ѱַ�ж�
				return I2CSlaveCalledGeneralInt;
			} else {
				//���򷵻ر�Ѱַ�����ж�
				return I2CSlaveCalledReInt;
			}
		}
	}

	//�鿴�Ƿ��ٲö�ʧ
	if (REG_GET_MASK(status, I2C_S_ARBL_MASK)) {
		//�����ٲö�ʧ�ж�
		return I2CSlaveArbitrationLostInt;
	}

	//�鿴�ӻ����ݷ���
	if (REG_GET_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_TX_MASK)) {
		//��Ϊ����ģʽ���鿴�Ƿ���յ���Ӧ���ź�
		if (REG_GET_MASK(status, I2C_S_RXAK_MASK)) {
			//���յ���Ӧ���ź�ʱ��ת��Ϊ����ģʽ
			REG_CLR_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_TX_MASK);
			//��ȡһ�����ݼĴ���
			dummy = I2C_D_REG(i2c_table[mod]);
			//�������ݷ�������ж�
			return I2CSlaveDataSendOverInt;
		} else {
			//���յ�Ӧ���ź�ʱ���������ݷ����ж�
			return I2CSlaveDataSendInt;
		}
	} else {
		//��Ϊ����ģʽ���������ݽ����ж�
		return I2CSlaveDataReInt;
	}
}

uint8 i2c_slave_re(uint8 mod) {
	return I2C_D_REG(i2c_table[mod]);
}

void i2c_slave_send(uint8 mod, uint8 data) {
	REG_SET_VAL(I2C_D_REG(i2c_table[mod]), I2C_D_DATA(data));
}
