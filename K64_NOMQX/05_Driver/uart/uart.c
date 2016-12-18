//==========================================================================
//�ļ����ƣ�uart.c
//���ܸ�Ҫ��K64 UART�ײ���������Դ�ļ�
//==========================================================================

#include "uart.h"

//��UARTģ�����ַ
static UART_Type * const uart_table[] = { UART0, UART1, UART2, UART3, UART4,
UART5 };
//UART��ģ��RX���ź�
static const uint8 uart_rx_pin_table[] = { UART_MOD0_RX_PIN, UART_MOD1_RX_PIN,
UART_MOD2_RX_PIN, UART_MOD3_RX_PIN, UART_MOD4_RX_PIN, UART_MOD5_RX_PIN };
//UART��ģ��TX���ź�
static const uint8 uart_tx_pin_table[] = { UART_MOD0_TX_PIN, UART_MOD1_TX_PIN,
UART_MOD2_TX_PIN, UART_MOD3_TX_PIN, UART_MOD4_TX_PIN, UART_MOD5_TX_PIN };
//UART��ģ��PCR��MUXֵ
static const uint8 uart_pcr_mux_table[] = { UART_MOD0_PCR_MUX,
UART_MOD1_PCR_MUX, UART_MOD2_PCR_MUX, UART_MOD3_PCR_MUX,
UART_MOD4_PCR_MUX, UART_MOD5_PCR_MUX };
//UARTģ�鷢�ͽ����ж������
static const IRQn_Type uart_rx_tx_irq_table[] = { UART0_RX_TX_IRQn,
		UART1_RX_TX_IRQn, UART2_RX_TX_IRQn, UART3_RX_TX_IRQn, UART4_RX_TX_IRQn,
		UART5_RX_TX_IRQn };

//==========================================================================
//��������: uart_init
//��������: ��
//����˵��: mod:UARTģ���:
//             UART_MODx��xΪģ���;
//         baud:������:
//              (600) | 1200  | 2400  | 4800 | 9600 | 14400 | 19200 |
//              38400 | 56000 | 57600 | 115200
//         parity_mode:У��ģʽ:
//                     UART_PARITY_DISABLED:������У��;
//                     UART_PARITY_ODD:     ��У��;
//                     UART_PARITY_EVEN:    żУ��;
//         stop_bit:ֹͣλ:
//                  UART_STOP_BIT_1:1λֹͣλ;
//                  UART_STOP_BIT_2:2λֹͣλ;
//         bit_order:λ����˳��:
//                   UART_BIT_ORDER_LSB:�����Чλ;
//                   UART_BIT_ORDER_MSB:�����Чλ;
//���ܸ�Ҫ: ��ʼ��UARTģ��
//��ע: ������Ϊ600ʱ��UART0��UART1�޷�ʹ��;
//     λ����˳���Ӱ������λ������ʼλ��У��λ��ֹͣλ����Ӱ��
//==========================================================================
void uart_init(uint8 mod, uint32 baud, uint8 parity_mode, uint8 stop_bit,
		uint8 bit_order) {
	uint8 pcr_mux;	//PCR��MUXֵ
	UART_Type * uart_ptr;	//UART����ַ
	uint16 sbr;	//������λ���������㲨����
	uint8 brfa;	//������΢��
	uint32 clk_freq;	//����ʱ��Ƶ��

	//��ȡPCR��MUXֵ
	pcr_mux = uart_pcr_mux_table[mod];
	//��ȡUART����ַ
	uart_ptr = uart_table[mod];

	//UART0��UART1ʹ��ϵͳʱ�ӣ�����UARTģ��ʹ������ʱ��
	if (mod == UART_MOD0 || mod == UART_MOD1) {
		clk_freq = UART_WORK_FREQ1;
	} else {
		clk_freq = UART_WORK_FREQ2;
	}

	//ʹ�����Ź���
	com_port_pin_set_mux(uart_rx_pin_table[mod], pcr_mux);
	com_port_pin_set_mux(uart_tx_pin_table[mod], pcr_mux);

	//����Ӧ��UARTģ��ʱ����
	switch (mod) {
	case UART_MOD0:
		REG_SET_MASK(SIM_SCGC4, SIM_SCGC4_UART0_MASK);
		break;
	case UART_MOD1:
		REG_SET_MASK(SIM_SCGC4, SIM_SCGC4_UART1_MASK);
		break;
	case UART_MOD2:
		REG_SET_MASK(SIM_SCGC4, SIM_SCGC4_UART2_MASK);
		break;
	case UART_MOD3:
		REG_SET_MASK(SIM_SCGC4, SIM_SCGC4_UART3_MASK);
		break;
	case UART_MOD4:
		REG_SET_MASK(SIM_SCGC1, SIM_SCGC1_UART4_MASK);
		break;
	case UART_MOD5:
		REG_SET_MASK(SIM_SCGC1, SIM_SCGC1_UART5_MASK);
		break;
	}

	//��ʱ�رմ��ڷ�������չ���
	REG_CLR_MASK(UART_C2_REG(uart_ptr), (UART_C2_TE_MASK | UART_C2_RE_MASK));

	//���ò����ʣ����ݹ�ʽ���㣬UART������ = UARTģ��ʱ��/(16*(sbr+brfa/32))
	sbr = (uint16) (clk_freq / (baud << 4));
	brfa = ((clk_freq << 1) / baud) - (sbr << 5);
	//���ԭֵ
	REG_CLR_MASK(UART_BDH_REG(uart_ptr), UART_BDH_SBR_MASK);
	REG_CLR_MASK(UART_BDL_REG(uart_ptr), UART_BDL_SBR_MASK);
	REG_CLR_MASK(UART_C4_REG(uart_ptr), UART_C4_BRFA_MASK);
	//���ü������ֵ
	REG_SET_MASK(UART_BDH_REG(uart_ptr), UART_BDH_SBR(sbr >> 8));
	REG_SET_MASK(UART_BDL_REG(uart_ptr), UART_BDL_SBR(sbr));
	REG_SET_MASK(UART_C4_REG(uart_ptr), UART_C4_BRFA(brfa));

	//����У��ģʽ
	if (parity_mode == UART_PARITY_DISABLED) {
		REG_CLR_MASK(UART_C1_REG(uart_ptr), (UART_C1_M_MASK|UART_C1_PE_MASK));//8λ���ݣ�������У��
	} else {
		REG_SET_MASK(UART_C1_REG(uart_ptr), (UART_C1_M_MASK|UART_C1_PE_MASK));//9λ����(��ͬУ��λ)������У��
		if (parity_mode == UART_PARITY_ODD) {
			REG_SET_MASK(UART_C1_REG(uart_ptr), UART_C1_PT_MASK);	//������У��
		} else {
			REG_CLR_MASK(UART_C1_REG(uart_ptr), UART_C1_PT_MASK);	//����żУ��
		}
	}

	//����λ����˳��
	if (bit_order == UART_BIT_ORDER_LSB) {
		REG_CLR_MASK(UART_S2_REG(uart_ptr), UART_S2_MSBF_MASK);
	} else {
		REG_SET_MASK(UART_S2_REG(uart_ptr), UART_S2_MSBF_MASK);
	}

	//����ֹͣλ
	if (stop_bit == UART_STOP_BIT_1) {
		REG_CLR_MASK(UART_BDH_REG(uart_ptr), UART_BDH_SBNS_MASK);	//1λֹͣλ
	} else {
		REG_SET_MASK(UART_BDH_REG(uart_ptr), UART_BDH_SBNS_MASK);	//2λֹͣλ
	}

	//�������ͽ���
	REG_SET_MASK(UART_C2_REG(uart_ptr), (UART_C2_TE_MASK | UART_C2_RE_MASK));
}

//==========================================================================
//��������: uart_send1
//��������: true:���ͳɹ�; false:����ʧ��
//����˵��: mod:UARTģ���:
//             UART_MODx��xΪģ���;
//         byte:��Ҫ���͵��ֽ�����
//���ܸ�Ҫ: ����1���ֽ�����
//==========================================================================
bool uart_send1(uint8 mod, uint8 byte) {
	UART_Type * uart_ptr;	//UART����ַ
	uint32 max, i;

	//��ȡUART����ַ
	uart_ptr = uart_table[mod];
	max = UART_RP_TIME_SEND;	//�����޴���ת��Ϊuint32����

	for (i = 0; i < max; i++) {
		//�жϷ��ͻ������Ƿ�Ϊ��
		if (REG_GET_MASK(UART_S1_REG(uart_ptr), UART_S1_TDRE_MASK)) {
			//Ϊ��ʱ���������ݼĴ���Ϊbyte
			REG_SET_VAL(UART_D_REG(uart_ptr), byte);
			return true;
		}
	}
	return false;
}

//==========================================================================
//��������: uart_sendN
//��������: true:���ͳɹ�; false:����ʧ��
//����˵��: mod:UARTģ���:
//             UART_MODx��xΪģ���;
//         len:���͵��ֽ���
//         buff:���ͻ�����
//���ܸ�Ҫ: ����N���ֽ�����
//==========================================================================
bool uart_sendN(uint8 mod, uint32 len, uint8* buff) {
	uint32 i;

	for (i = 0; i < len; i++) {
		//����1���ֽ����ݣ�ʧ������ʧ��
		if (!uart_send1(mod, buff[i])) {
			return false;
		}
	}
	return true;
}

//==========================================================================
//��������: uart_send_string
//��������: true:���ͳɹ�; false:����ʧ��
//����˵��: mod:UARTģ���:
//             UART_MODx��xΪģ���;
//         str:�����ַ������׵�ַ
//���ܸ�Ҫ: ����һ����'\0'�������ַ��������ᷢ��'\0'
//==========================================================================
bool uart_send_string(uint8 mod, uint8* str) {
	while (*str != '\0') {
		//����1���ֽ����ݣ�ʧ������ʧ��
		if (!uart_send1(mod, *str++)) {
			return false;
		}
	}
	return true;
}

//==========================================================================
//��������: uart_re1
//��������: true:���ճɹ�; false:����ʧ��
//����˵��: mod:UARTģ���:
//             UART_MODx��xΪģ���;
//         byte:��Ҫ�����ֽ����ݵĵ�ַ
//���ܸ�Ҫ: ����1���ֽ�����
//==========================================================================
bool uart_re1(uint8 mod, uint8* byte) {
	UART_Type * uart_ptr;	//UART����ַ
	uint32 max, i;

	//��ȡUART����ַ
	uart_ptr = uart_table[mod];
	max = UART_RP_TIME_RECEIVE;	//�����޴���ת��Ϊuint32����

	for (i = 0; i < max; i++) {
		//�жϽ��ջ������Ƿ���
		if (REG_GET_MASK(UART_S1_REG(uart_ptr), UART_S1_RDRF_MASK)) {
			//��ʱ��ȡ���ݼĴ�������
			*byte = UART_D_REG(uart_ptr);
			return true;
		}
	}
	return false;
}

//==========================================================================
//��������: uart_re1_parity
//��������: true:���ճɹ�; false:����ʧ��
//����˵��: mod:UARTģ���:
//             UART_MODx��xΪģ���;
//         byte:��Ҫ�����ֽ����ݵĵ�ַ
//         err:���ճɹ�ʱ����żУ�����޴�����δ����У�飬��ʼ���޴���
//���ܸ�Ҫ: ����1���ֽ����ݣ����ж�У��λ���޴���
//==========================================================================
bool uart_re1_parity(uint8 mod, uint8* byte, bool* err) {
	UART_Type * uart_ptr;	//UART����ַ
	uint32 max, i;

	//��ȡUART����ַ
	uart_ptr = uart_table[mod];
	max = UART_RP_TIME_RECEIVE;	//�����޴���ת��Ϊuint32����

	for (i = 0; i < max; i++) {
		//�жϽ��ջ������Ƿ���
		if (REG_GET_MASK(UART_S1_REG(uart_ptr), UART_S1_RDRF_MASK)) {
			//�鿴��żУ������־
			*err = REG_GET_MASK(UART_S1_REG(uart_ptr), UART_S1_PF_MASK) ?
					true : false;
			//��ʱ��ȡ���ݼĴ�������
			*byte = UART_D_REG(uart_ptr);
			return true;
		}
	}
	return false;
}

//==========================================================================
//��������: uart_enable_re_int
//��������: ��
//����˵��: mod:UARTģ���:
//             UART_MODx��xΪģ���;
//���ܸ�Ҫ: ʹ�ܴ���ģ������жϣ�ͨ���������ݿ�������жϱ�־
//==========================================================================
void uart_enable_re_int(uint8 mod) {
	//�����ͽ����ж�
	REG_SET_MASK(UART_C2_REG(uart_table[mod]), UART_C2_RIE_MASK);
	//������ս����ж�
	ENABLE_IRQ(uart_rx_tx_irq_table[mod]);
}

//==========================================================================
//��������: uart_disable_re_int
//��������: ��
//����˵��: mod:UARTģ���:
//             UART_MODx��xΪģ���;
//���ܸ�Ҫ: �رմ���ģ������ж�
//==========================================================================
void uart_disable_re_int(uint8 mod) {
	//��ֹ���ͽ����ж�
	REG_CLR_MASK(UART_C2_REG(uart_table[mod]), UART_C2_RIE_MASK);
	//��ֹ���ս����ж�
	DISABLE_IRQ(uart_rx_tx_irq_table[mod]);
}
