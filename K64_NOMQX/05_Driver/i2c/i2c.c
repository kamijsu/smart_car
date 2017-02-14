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
static const uint8 i2c_scl_pin_table[] =
{ I2C_MOD0_SCL_PIN, I2C_MOD1_SCL_PIN, I2C_MOD2_SCL_PIN };
//I2C��ģ��ʱ���źŵ�PCR��MUXֵ
static const uint8 i2c_scl_pcr_mux_table[] =
{ I2C_MOD0_SCL_PCR_MUX, I2C_MOD1_SCL_PCR_MUX, I2C_MOD2_SCL_PCR_MUX };
//I2C��ģ�������źŵ����ź�
static const uint8 i2c_sda_pin_table[] =
{ I2C_MOD0_SDA_PIN, I2C_MOD1_SDA_PIN, I2C_MOD2_SDA_PIN };
//I2C��ģ�������źŵ�PCR��MUXֵ
static const uint8 i2c_sda_pcr_mux_table[] =
{ I2C_MOD0_SDA_PCR_MUX, I2C_MOD1_SDA_PCR_MUX, I2C_MOD2_SDA_PCR_MUX };
//I2C��ģ���ж������
static const IRQn_Type i2c_irq_table[] = { I2C0_IRQn, I2C1_IRQn, I2C2_IRQn };

//==========================================================================
//��������: i2c_master_send_byte
//��������: ����ͨ�Ž��:
//         I2CMasterSuccess:        ͨ�ųɹ�;
//         I2CMasterNAck:           �ӻ�δ��Ӧ;
//         I2CMasterArbitrationLost:�����ٲö�ʧ;
//         I2CMasterTimeoutSlave:   �ӻ���Ӧ��ʱ;
//         I2CMasterTimeoutStop:    ֹͣ�ź���Ӧ��ʱ;
//����˵��: i2c_ptr:I2Cģ�����ַ
//         byte:Ҫ���͵��ֽ�
//         old_c1:�ɿ��ƼĴ���C1��ֵ
//���ܸ�Ҫ: I2Cģ��������ģʽ�·���һ���ֽ����ݣ���������ʱ�����д�����
//==========================================================================
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

//==========================================================================
//��������: i2c_master_re_byte
//��������: ����ͨ�Ž��:
//         I2CMasterSuccess:        ͨ�ųɹ�;
//         I2CMasterArbitrationLost:�����ٲö�ʧ;
//         I2CMasterTimeoutSlave:   �ӻ���Ӧ��ʱ;
//����˵��: i2c_ptr:I2Cģ�����ַ
//         byte:�洢���յ��ֽڵĵ�ַ
//         old_c1:�ɿ��ƼĴ���C1��ֵ
//���ܸ�Ҫ: I2Cģ��������ģʽ�½���һ���ֽ����ݣ���������ʱ�����д�����
//==========================================================================
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

//==========================================================================
//��������: i2c_init
//��������: ��
//����˵��: mod:I2Cģ���:
//             I2C_MODx��xΪģ���;
//         mul:��������:
//             I2C_MUL_x��xΪ�������Ӵ�С��x��ȡֵΪ:
//             1 | 2 | 4
//         icr:ʱ�����ʣ�ȡֵ��ΧΪ[0x00,0x3F]��������ͼ���ע
//         addr_mode:��I2Cģ���ַģʽ:
//                   I2C_ADDR_MODE_BITS_7: 7λ��ַģʽ;
//                   I2C_ADDR_MODE_BITS_10:10λ��ַģʽ;
//         addr:��I2Cģ���ַ:
//              7λ��ַģʽʱ��ȡֵ��ΧΪ[0x08,0x77]�������ַ������
//              10λ��ַģʽʱ��ȡֵ��ΧΪ(0x000,0x3FF]
//         enable_general_call_addr:�Ƿ�ʹ�ܸ�ģ��Ĺ㲥���е�ַ:
//                                  true: ʹ�ܹ㲥���е�ַ����ģ����Ա��㲥Ѱַ;
//                                  false:�رչ㲥���е�ַ;
//���ܸ�Ҫ: ��ʼ��I2Cģ�飬Ϊ�ӻ�����ģʽ��Ĭ�Ϲرմӻ��жϣ���������Ӧ���ź�
//��ע: ��ʹ�����ŵĿ�©�������������;
//
//     ��I2Cģ�鲨���� = ����ʱ��Ƶ��(Hz) / (mul * SCL divider)��
//     �����ʲ�Ҫ����400Kbps������ͨ�����ܵ�����;
//     SDA����ʱ�� = ����ʱ������(s) * mul * SDA hold value��
//     SDA����ʱ��ΪSCL�ź��½����������SDA�źű仯��ʱ��;
//     SCL��ʼ����ʱ�� = ����ʱ������(s) * mul * SCL start hold value��
//     SCL��ʼ����ʱ��Ϊ��ʼ�źŷ�������SCL�źŵ�һ���½��������ʱ��;
//     SCLֹͣ����ʱ�� = ����ʱ������(s) * mul * SCL stop hold value��
//     SCLֹͣ����ʱ��ΪSCL�ź����һ���������������ֹͣ�źŷ�����ʱ��;
//
//     icr��������SCL divider��SDA hold value��SCL start hold value��
//     SCL stop hold value�������ϵ���±���ʾ:
//
//     |icr |SCL divider|SDA hold value|SCL start |SCL stop  |
//     |    |           |              |hold value|hold value|
//     |0x00|         20|             7|         6|        11|
//     |0x01|         22|             7|         7|        12|
//     |0x02|         24|             8|         8|        13|
//     |0x03|         26|             8|         9|        14|
//     |0x04|         28|             9|        10|        15|
//     |0x05|         30|             9|        11|        16|
//     |0x06|         34|            10|        13|        18|
//     |0x07|         40|            10|        16|        21|
//     |0x08|         28|             7|        10|        15|
//     |0x09|         32|             7|        12|        17|
//     |0x0A|         36|             9|        14|        19|
//     |0x0B|         40|             9|        16|        21|
//     |0x0C|         44|            11|        18|        23|
//     |0x0D|         48|            11|        20|        25|
//     |0x0E|         56|            13|        24|        29|
//     |0x0F|         68|            13|        30|        35|
//     |0x10|         48|             9|        18|        25|
//     |0x11|         56|             9|        22|        29|
//     |0x12|         64|            13|        26|        33|
//     |0x13|         72|            13|        30|        37|
//     |0x14|         80|            17|        34|        41|
//     |0x15|         88|            17|        38|        45|
//     |0x16|        104|            21|        46|        53|
//     |0x17|        128|            21|        58|        65|
//     |0x18|         80|             9|        38|        41|
//     |0x19|         96|             9|        46|        49|
//     |0x1A|        112|            17|        54|        57|
//     |0x1B|        128|            17|        62|        65|
//     |0x1C|        144|            25|        70|        73|
//     |0x1D|        160|            25|        78|        81|
//     |0x1E|        192|            33|        94|        97|
//     |0x1F|        240|            33|       118|       121|
//     |0x20|        160|            17|        78|        81|
//     |0x21|        192|            17|        94|        97|
//     |0x22|        224|            33|       110|       113|
//     |0x23|        256|            33|       126|       129|
//     |0x24|        288|            49|       142|       145|
//     |0x25|        320|            49|       158|       161|
//     |0x26|        384|            65|       190|       193|
//     |0x27|        480|            65|       238|       241|
//     |0x28|        320|            33|       158|       161|
//     |0x29|        384|            33|       190|       193|
//     |0x2A|        448|            65|       222|       225|
//     |0x2B|        512|            65|       254|       257|
//     |0x2C|        576|            97|       286|       289|
//     |0x2D|        640|            97|       318|       321|
//     |0x2E|        768|           129|       382|       385|
//     |0x2F|        960|           129|       478|       481|
//     |0x30|        640|            65|       318|       321|
//     |0x31|        768|            65|       382|       385|
//     |0x32|        896|           129|       446|       449|
//     |0x33|       1024|           129|       510|       513|
//     |0x34|       1152|           193|       574|       577|
//     |0x35|       1280|           193|       638|       641|
//     |0x36|       1536|           257|       766|       769|
//     |0x37|       1920|           257|       958|       961|
//     |0x38|       1280|           129|       638|       641|
//     |0x39|       1536|           129|       766|       769|
//     |0x3A|       1792|           257|       894|       897|
//     |0x3B|       2048|           257|      1022|      1025|
//     |0x3C|       2304|           385|      1150|      1153|
//     |0x3D|       2560|           385|      1278|      1281|
//     |0x3E|       3072|           513|      1534|      1537|
//     |0x3F|       3840|           513|      1918|      1921|
//
//     ������ʱ��Ƶ��Ϊ48MHz��mulΪI2C_MUL_2��icrΪ0x20��������Ϊ150Kbps��
//     SDA����ʱ��Ϊ0.71��s��SCL��ʼ����ʱ��Ϊ3.25��s��SCLֹͣ����ʱ��Ϊ3.375��s
//==========================================================================
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

	REG_CLR_MASK(I2C_C1_REG(i2c_ptr),
			 I2C_C1_IICEN_MASK		//��ʱ����I2Cģ��
			|I2C_C1_IICIE_MASK		//�ر�I2C�ж�
			|I2C_C1_MST_MASK		//����Ϊ�ӻ�ģʽ
			|I2C_C1_TX_MASK			//����Ϊ����ģʽ
			|I2C_C1_TXAK_MASK);		//������Ӧ���ź�

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

//==========================================================================
//��������: i2c_master_send
//��������: ����ͨ�Ž��:
//         I2CMasterSuccess:        ͨ�ųɹ�;
//         I2CMasterNAck:           �ӻ�δ��Ӧ;
//         I2CMasterArbitrationLost:�����ٲö�ʧ;
//         I2CMasterTimeoutSlave:   �ӻ���Ӧ��ʱ;
//         I2CMasterTimeoutStop:    ֹͣ�ź���Ӧ��ʱ;
//         I2CMasterIsBusy:         �������ڹ���;
//����˵��: mod:I2Cģ���:
//             I2C_MODx��xΪģ���;
//         addr_mode:ͨ�Ŵӻ�����ĵ�ַģʽ:
//                   I2C_ADDR_MODE_BITS_7: 7λ��ַģʽ;
//                   I2C_ADDR_MODE_BITS_10:10λ��ַģʽ;
//         addr:ͨ�Ŵӻ�����ĵ�ַ;
//         data:Ҫ�������ݵ��׵�ַ;
//         len:Ҫ�������ݵ��ֽ���;
//���ܸ�Ҫ: ��I2Cģ��ת��Ϊ����ģʽ����ָ���ӻ��������ݺ󣬻ָ�Ϊ�ӻ�ģʽ
//��ע: addr_mode��addr����ӻ��йأ��ͱ����޹أ���ͬ��ַģʽ��I2Cģ������໥ͨ�ţ�
//     ͨ�Ų������ɱ�������;
//     ����ַģʽΪ7λ��ַģʽ���ҵ�ַΪ0x00ʱ��Ϊ�㲥���е�ַ;
//
//     ע�⣬��������Ӧ��ʱ(�ӻ���ֹͣ�ź�)���ӻ��п����Ѿ���������Ҫ��λ�ӻ�;
//     ��ͬʱ�ж�������������ݣ���������ʧ��ʱ���ᷢ���ٲö�ʧ��
//     ��ӻ����������ͷ�����ʱ��Ҳ�ᷢ���ٲö�ʧ
//==========================================================================
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

//==========================================================================
//��������: i2c_master_send_re
//��������: ����ͨ�Ž��:
//         I2CMasterSuccess:        ͨ�ųɹ�;
//         I2CMasterNAck:           �ӻ�δ��Ӧ;
//         I2CMasterArbitrationLost:�����ٲö�ʧ;
//         I2CMasterTimeoutSlave:   �ӻ���Ӧ��ʱ;
//         I2CMasterTimeoutStop:    ֹͣ�ź���Ӧ��ʱ;
//         I2CMasterIsBusy:         �������ڹ���;
//����˵��: mod:I2Cģ���:
//             I2C_MODx��xΪģ���;
//         addr_mode:ͨ�Ŵӻ�����ĵ�ַģʽ:
//                   I2C_ADDR_MODE_BITS_7: 7λ��ַģʽ;
//                   I2C_ADDR_MODE_BITS_10:10λ��ַģʽ;
//         addr:ͨ�Ŵӻ�����ĵ�ַ;
//         send_data:Ҫ�������ݵ��׵�ַ;
//         send_len:Ҫ�������ݵ��ֽ���;
//         re_data:�洢�������ݵ��׵�ַ;
//         re_len:Ҫ�������ݵ��ֽ���;
//���ܸ�Ҫ: ��I2Cģ��ת��Ϊ����ģʽ����ָ���ӻ��������ݣ����������ݺ󣬻ָ�Ϊ�ӻ�ģʽ
//��ע: addr_mode��addr����ӻ��йأ��ͱ����޹أ���ͬ��ַģʽ��I2Cģ������໥ͨ�ţ�
//     ͨ�Ų������ɱ�������;
//     �޷�ʹ�ù㲥���е�ַ����Ϊ�㲥���е�ַ�������ڷ�������;
//
//     ע�⣬��������Ӧ��ʱ(�ӻ���ֹͣ�ź�)���ӻ��п����Ѿ���������Ҫ��λ�ӻ�;
//     ��ͬʱ�ж�������������ݣ���������ʧ��ʱ���ᷢ���ٲö�ʧ��
//     ��ӻ����������ͷ�����ʱ��Ҳ�ᷢ���ٲö�ʧ
//==========================================================================
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

//==========================================================================
//��������: i2c_master_re
//��������: ����ͨ�Ž��:
//         I2CMasterSuccess:        ͨ�ųɹ�;
//         I2CMasterNAck:           �ӻ�δ��Ӧ;
//         I2CMasterArbitrationLost:�����ٲö�ʧ;
//         I2CMasterTimeoutSlave:   �ӻ���Ӧ��ʱ;
//         I2CMasterTimeoutStop:    ֹͣ�ź���Ӧ��ʱ;
//         I2CMasterIsBusy:         �������ڹ���;
//����˵��: mod:I2Cģ���:
//             I2C_MODx��xΪģ���;
//         addr_mode:ͨ�Ŵӻ�����ĵ�ַģʽ:
//                   I2C_ADDR_MODE_BITS_7: 7λ��ַģʽ;
//                   I2C_ADDR_MODE_BITS_10:10λ��ַģʽ;
//         addr:ͨ�Ŵӻ�����ĵ�ַ;
//         data:�洢�������ݵ��׵�ַ;
//         len:Ҫ�������ݵ��ֽ���;
//���ܸ�Ҫ: ��I2Cģ��ת��Ϊ����ģʽ����ָ���ӻ��������ݺ󣬻ָ�Ϊ�ӻ�ģʽ
//��ע: addr_mode��addr����ӻ��йأ��ͱ����޹أ���ͬ��ַģʽ��I2Cģ������໥ͨ�ţ�
//     ͨ�Ų������ɱ�������;
//     �޷�ʹ�ù㲥���е�ַ����Ϊ�㲥���е�ַ�������ڷ�������;
//
//     ע�⣬��������Ӧ��ʱ(�ӻ���ֹͣ�ź�)���ӻ��п����Ѿ���������Ҫ��λ�ӻ�;
//     ��ͬʱ�ж�������������ݣ���������ʧ��ʱ���ᷢ���ٲö�ʧ��
//     ��ӻ����������ͷ�����ʱ��Ҳ�ᷢ���ٲö�ʧ
//==========================================================================
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

//==========================================================================
//��������: i2c_slave_enable_int
//��������: ��
//����˵��: mod:I2Cģ���:
//             I2C_MODx��xΪģ���;
//���ܸ�Ҫ: ʹ�ܴӻ��жϣ�������ͨ��ʱ���ᷢ���ж�
//==========================================================================
void i2c_slave_enable_int(uint8 mod) {
	//ʹ�ܴӻ��ж�
	REG_SET_MASK(I2C_C1_REG(i2c_table[mod]), I2C_C1_IICIE_MASK);
	//��������ж�
	ENABLE_IRQ(i2c_irq_table[mod]);
}

//==========================================================================
//��������: i2c_slave_disable_int
//��������: ��
//����˵��: mod:I2Cģ���:
//             I2C_MODx��xΪģ���;
//���ܸ�Ҫ: �رմӻ��ж�
//==========================================================================
void i2c_slave_disable_int(uint8 mod) {
	//�رմӻ��ж�
	REG_CLR_MASK(I2C_C1_REG(i2c_table[mod]), I2C_C1_IICIE_MASK);
	//��ֹ�����ж�
	DISABLE_IRQ(i2c_irq_table[mod]);
}

//==========================================================================
//��������: i2c_slave_set_ack
//��������: ��
//����˵��: mod:I2Cģ���:
//             I2C_MODx��xΪģ���;
//         ack:�Ƿ���Ӧ���ź�:
//             true: ����Ӧ���ź�;
//             false:���ͷ�Ӧ���ź�;
//���ܸ�Ҫ: ���ôӻ�Ӧ���ź�����
//��ע: �ӻ���ַƥ��ʱ(�����㲥��ַ)��һ������Ӧ���źţ�
//     ƥ�䵽�㲥��ַ�󣬽��յ�����һ������Ӧ���ź�;
//     ����Ӧ���ź����ͺ���һ�η���Ӧ���ź�����ʱ��Ч
//==========================================================================
void i2c_slave_set_ack(uint8 mod, bool ack) {
	if (ack) {
		//����Ӧ���ź�
		REG_CLR_MASK(I2C_C1_REG(i2c_table[mod]), I2C_C1_TXAK_MASK);
	} else {
		//���ͷ�Ӧ���ź�
		REG_SET_MASK(I2C_C1_REG(i2c_table[mod]), I2C_C1_TXAK_MASK);
	}
}

//==========================================================================
//��������: i2c_slave_handle_int
//��������: �ӻ��ж�����:
//         I2CSlaveNoInt:             ���жϷ���;
//         I2CSlaveCalledGeneralInt:  ���㲥Ѱַ�ж�;
//         I2CSlaveCalledReInt:       ��Ѱַ�����ж�;
//         I2CSlaveCalledSendInt:     ��Ѱַ�����ж�;
//         I2CSlaveDataReInt:         ���ݽ����ж�;
//         I2CSlaveDataSendInt:       ���ݷ����ж�;
//         I2CSlaveDataSendOverInt:   ���ݷ�������ж�;
//         I2CSlaveArbitrationLostInt:�ٲö�ʧ�ж�;
//����˵��: mod:I2Cģ���:
//             I2C_MODx��xΪģ���;
//���ܸ�Ҫ: ����ӻ��жϣ��������ж�����
//��ע: ���жϷ��������У���Ҫ���øú��������⣬
//     �ж�����ΪI2CSlaveDataReIntʱ����Ҫ����һ�ֽ����ݣ�
//     �ж�����ΪI2CSlaveCalledSendInt��I2CSlaveDataSendIntʱ����Ҫ����һ�ֽ�����;
//     �������ջ������ݣ����߻ᱻ�ӻ�ռ�ݣ�����������һ�δ�������ʱ�ȴ��ӻ��ͷ�����;
//
//     ��������n���ֽ�����ʱ���ӻ��жϹ�������:
//     [I2CSlaveDataReInt] - I2CSlaveCalledReInt - I2CSlaveDataReInt * n;
//
//     �����㲥����n���ֽ�����ʱ���ӻ��жϹ�������:
//     I2CSlaveCalledGeneralInt - I2CSlaveDataReInt * n;
//
//     ��������n���ֽ����ݣ�����m���ֽ�����ʱ���ӻ��жϹ�������:
//     [I2CSlaveDataReInt] - I2CSlaveCalledReInt - I2CSlaveDataReInt * n -
//     I2CSlaveCalledSendInt - I2CSlaveDataSendInt * (m-1) -
//     I2CSlaveDataSendOverInt;
//
//     ��������n���ֽ�����ʱ���ӻ��жϹ�������:
//     [I2CSlaveDataReInt] - [I2CSlaveCalledReInt] - I2CSlaveCalledSendInt -
//     I2CSlaveDataSendInt * (n-1) - I2CSlaveDataSendOverInt;
//
//     ��[]���ж�Ϊ�ӻ���10λ��ַģʽʱ������ⷢ�����жϣ��ӻ��ڱ�Ѱַ����ǰ��
//     ����������һ���ֽ����ݣ�Ϊ0b1111 0xx0��xxΪ�ӻ���2λ��ַ��
//     ���ӻ���ַΪ0x100�������յ�0xF2����Ҫ���н���������Ϊ��Ч����;
//
//     �жϷ���ʱ��Ӧ���ź��Ѿ��������÷��ͳ�ȥ
//==========================================================================
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
			//�����յĵ�ַΪ0x00
			if (I2C_D_REG(i2c_table[mod]) == 0) {
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

//==========================================================================
//��������: i2c_slave_re
//��������: �ӻ����յ�������
//����˵��: mod:I2Cģ���:
//             I2C_MODx��xΪģ���;
//���ܸ�Ҫ: �ӻ�����һ���ֽ�����
//==========================================================================
uint8 i2c_slave_re(uint8 mod) {
	return I2C_D_REG(i2c_table[mod]);
}

//==========================================================================
//��������: i2c_slave_send
//��������: ��
//����˵��: mod:I2Cģ���:
//             I2C_MODx��xΪģ���;
//         data:Ҫ���͵�����
//���ܸ�Ҫ: �ӻ�����һ���ֽ�����
//==========================================================================
void i2c_slave_send(uint8 mod, uint8 data) {
	REG_SET_VAL(I2C_D_REG(i2c_table[mod]), I2C_D_DATA(data));
}
