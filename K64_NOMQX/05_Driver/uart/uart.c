//==========================================================================
//�ļ����ƣ�uart.c
//���ܸ�Ҫ��K64 UART�ײ���������Դ�ļ�
//==========================================================================

#include "uart.h"

//��UARTģ�����ַ
static UART_Type * const uart_table[] = { UART0, UART1, UART2, UART3, UART4,
UART5 };
//UARTģ�鷢�ͽ����ж������
static const IRQn_Type uart_rx_tx_irq_table[] = { UART0_RX_TX_IRQn,
		UART1_RX_TX_IRQn, UART2_RX_TX_IRQn, UART3_RX_TX_IRQn, UART4_RX_TX_IRQn,
		UART5_RX_TX_IRQn };

//==========================================================================
//��������: uart_init
//��������: ��
//����˵��: mod:UARTģ��ţ�UART_MODx��xΪģ���
//         baud:������:(600) | 1200 | 2400 | 4800 | 9600 | 14400 | 19200 |
//                     38400 | 56000 | 57600 | 115200
//         parity_mode:У��ģʽ��UART_PARITY_DISABLED:������У��;
//                     UART_PARITY_ODD:��У��; UART_PARITY_EVEN:żУ��
//         stop_bit:ֹͣλ��UART_STOP_BIT_1:1λֹͣλ; UART_STOP_BIT_2:2λֹͣλ
//���ܸ�Ҫ: ��ʼ��UARTģ��
//��ע: ������Ϊ600ʱ��UART0��UART1�޷�ʹ��
//==========================================================================
void uart_init(uint8 mod, uint32 baud, uint8 parity_mode, uint8 stop_bit) {
	uint16 sbr;	//������λ���������㲨����
	uint8 brfa;	//������΢��
	uint32 clk_freq;	//����ʱ��Ƶ��

	//UART0��UART1ʹ��ϵͳʱ�ӣ�����UARTģ��ʹ������ʱ��
	if (mod == UART_MOD0 || mod == UART_MOD1) {
		clk_freq = UART_WORK_FREQ1;
	} else {
		clk_freq = UART_WORK_FREQ2;
	}

	//ʹ�����Ź��ܲ�����Ӧ��UARTģ��ʱ����
	switch (mod) {
	case UART_MOD0:
		REG_CLR_MASK(UART_MOD0_TX_PCR, PORT_PCR_MUX_MASK);
		REG_CLR_MASK(UART_MOD0_RX_PCR, PORT_PCR_MUX_MASK);
#if(UART_MOD0_SETUP == 0)
		REG_SET_MASK(UART_MOD0_TX_PCR, PORT_PCR_MUX(2));
		REG_SET_MASK(UART_MOD0_RX_PCR, PORT_PCR_MUX(2));
#else
		REG_SET_MASK(UART_MOD0_TX_PCR, PORT_PCR_MUX(3));
		REG_SET_MASK(UART_MOD0_RX_PCR, PORT_PCR_MUX(3));
#endif
		REG_SET_MASK(SIM_SCGC4, SIM_SCGC4_UART0_MASK);
		break;
	case UART_MOD1:
		REG_CLR_MASK(UART_MOD1_TX_PCR, PORT_PCR_MUX_MASK);
		REG_CLR_MASK(UART_MOD1_RX_PCR, PORT_PCR_MUX_MASK);
		REG_SET_MASK(UART_MOD1_TX_PCR, PORT_PCR_MUX(3));
		REG_SET_MASK(UART_MOD1_RX_PCR, PORT_PCR_MUX(3));
		REG_SET_MASK(SIM_SCGC4, SIM_SCGC4_UART1_MASK);
		break;
	case UART_MOD2:
		REG_CLR_MASK(UART_MOD2_TX_PCR, PORT_PCR_MUX_MASK);
		REG_CLR_MASK(UART_MOD2_RX_PCR, PORT_PCR_MUX_MASK);
		REG_SET_MASK(UART_MOD2_TX_PCR, PORT_PCR_MUX(3));
		REG_SET_MASK(UART_MOD2_RX_PCR, PORT_PCR_MUX(3));
		REG_SET_MASK(SIM_SCGC4, SIM_SCGC4_UART2_MASK);
		break;
	case UART_MOD3:
		REG_CLR_MASK(UART_MOD3_TX_PCR, PORT_PCR_MUX_MASK);
		REG_CLR_MASK(UART_MOD3_RX_PCR, PORT_PCR_MUX_MASK);
		REG_SET_MASK(UART_MOD3_TX_PCR, PORT_PCR_MUX(3));
		REG_SET_MASK(UART_MOD3_RX_PCR, PORT_PCR_MUX(3));
		REG_SET_MASK(SIM_SCGC4, SIM_SCGC4_UART3_MASK);
		break;
	case UART_MOD4:
		REG_CLR_MASK(UART_MOD4_TX_PCR, PORT_PCR_MUX_MASK);
		REG_CLR_MASK(UART_MOD4_RX_PCR, PORT_PCR_MUX_MASK);
		REG_SET_MASK(UART_MOD4_TX_PCR, PORT_PCR_MUX(3));
		REG_SET_MASK(UART_MOD4_RX_PCR, PORT_PCR_MUX(3));
		REG_SET_MASK(SIM_SCGC1, SIM_SCGC1_UART4_MASK);
		break;
	case UART_MOD5:
		REG_CLR_MASK(UART_MOD5_TX_PCR, PORT_PCR_MUX_MASK);
		REG_CLR_MASK(UART_MOD5_RX_PCR, PORT_PCR_MUX_MASK);
		REG_SET_MASK(UART_MOD5_TX_PCR, PORT_PCR_MUX(3));
		REG_SET_MASK(UART_MOD5_RX_PCR, PORT_PCR_MUX(3));
		REG_SET_MASK(SIM_SCGC1, SIM_SCGC1_UART5_MASK);
		break;
	}

	//��ʱ�رմ��ڷ�������չ���
	REG_CLR_MASK(UART_C2_REG(uart_table[mod]),
			(UART_C2_TE_MASK | UART_C2_RE_MASK));

	//���ò����ʣ����ݹ�ʽ���㣬UART������ = UARTģ��ʱ��/(16*(sbr+brfa/32))
	sbr = (uint16) (clk_freq / (baud * 16));
	brfa = ((32 * clk_freq) / (baud * 16)) - 32 * sbr;
	//���ԭֵ
	REG_CLR_MASK(UART_BDH_REG(uart_table[mod]), UART_BDH_SBR_MASK);
	REG_CLR_MASK(UART_BDL_REG(uart_table[mod]), UART_BDL_SBR_MASK);
	REG_CLR_MASK(UART_C4_REG(uart_table[mod]), UART_C4_BRFA_MASK);
	//���ü������ֵ
	REG_SET_MASK(UART_BDH_REG(uart_table[mod]), UART_BDH_SBR(sbr >> 8));
	REG_SET_MASK(UART_BDL_REG(uart_table[mod]), UART_BDL_SBR(sbr));
	REG_SET_MASK(UART_C4_REG(uart_table[mod]), UART_C4_BRFA(brfa));

	//����У��ģʽ
	if (parity_mode == UART_PARITY_DISABLED) {
		REG_CLR_MASK(UART_C1_REG(uart_table[mod]), UART_C1_M_MASK);		//8λ����
		REG_CLR_MASK(UART_C1_REG(uart_table[mod]), UART_C1_PE_MASK);	//������У��
	} else {
		REG_SET_MASK(UART_C1_REG(uart_table[mod]), UART_C1_M_MASK);	//9λ����(��ͬУ��λ)
		REG_SET_MASK(UART_C1_REG(uart_table[mod]), UART_C1_PE_MASK);	//����У��
		if (parity_mode == UART_PARITY_ODD) {
			REG_SET_MASK(UART_C1_REG(uart_table[mod]), UART_C1_PT_MASK);//������У��
		} else {
			REG_CLR_MASK(UART_C1_REG(uart_table[mod]), UART_C1_PT_MASK);//����żУ��
		}
	}

	//��������λ˳����������ΪLSB����������ʼλ�������λ0
	REG_CLR_MASK(UART_S2_REG(uart_table[mod]), UART_S2_MSBF_MASK);

	//����ֹͣλ
	if (stop_bit == UART_STOP_BIT_1) {
		REG_CLR_MASK(UART_BDH_REG(uart_table[mod]), UART_BDH_SBNS_MASK);//1λֹͣλ
	} else {
		REG_SET_MASK(UART_BDH_REG(uart_table[mod]), UART_BDH_SBNS_MASK);//2λֹͣλ
	}

	//�������ͽ���
	REG_SET_MASK(UART_C2_REG(uart_table[mod]),
			(UART_C2_TE_MASK | UART_C2_RE_MASK));
}

//==========================================================================
//��������: uart_send1
//��������: true:���ͳɹ�; false:����ʧ��
//����˵��: mod:UARTģ��ţ�UART_MODx��xΪģ���
//         byte:��Ҫ���͵��ֽ�����
//���ܸ�Ҫ: ����1���ֽ�����
//==========================================================================
bool uart_send1(uint8 mod, uint8 byte) {
#if(UART_RP_TIME_SEND == UART_RP_TIME_INFINITY)
	for (;;) {
#else
	uint32 max = UART_RP_TIME_SEND;	//�����޴���ת��Ϊuint32����
	uint32 i;
	for (i = 0; i < max; i++) {
#endif
		//�жϷ��ͻ������Ƿ�Ϊ��
		if (REG_GET_MASK(UART_S1_REG(uart_table[mod]), UART_S1_TDRE_MASK)) {
			//Ϊ��ʱ���������ݼĴ���Ϊbyte
			REG_SET_VAL(UART_D_REG(uart_table[mod]), byte);
			return true;
		}
	}
	return false;
}

//============================================================================
//�������ƣ�uart_sendN
//����˵����uartNo: ���ں�:U_UART0��U_UART1��U_UART2��U_UART3��U_UART4��U_UART5
//          buff: ���ͻ�����
//          len:���ͳ���
//�������أ� ����ִ��״̬��0=������1=�쳣
//���ܸ�Ҫ������ ����n���ֽ�   
//============================================================================
uint8_t uart_sendN(uint8_t uartNo, uint16_t len, uint8_t* buff) {
	uint16_t i;
	for (i = 0; i < len; i++) {
		if (uart_send1(uartNo, buff[i])) //����һ���ֽ����ݣ�ʧ��������ѭ��
				{
			break;
		}
	}
	if (i < len)
		return 1;   //����len���ֽڳ���
	else
		return 0;   //����len���ֽڳɹ�
}

//============================================================================
//�������ƣ�uart_send_string
//����˵����uartNo:UARTģ���:U_UART0��U_UART1��U_UART2��U_UART3��U_UART4��U_UART5
//          buff:Ҫ���͵��ַ������׵�ַ
//�������أ� ����ִ��״̬��0=��������0=�쳣��
//���ܸ�Ҫ����ָ��UART�˿ڷ���һ����'\0'�������ַ���
//============================================================================
uint8_t uart_send_string(uint8_t uartNo, void *buff) {
	uint16_t i = 0;
	uint8_t *buff_ptr = (uint8_t *) buff;    //����ָ��ָ��Ҫ�����ַ����׵�ַ
	for (i = 0; buff_ptr[i] != '\0'; i++)  //�����ַ�������ַ�
			{
		if (uart_send1(uartNo, buff_ptr[i]))  //����ָ���Ӧ���ַ�
			return 1;  //����ʧ��
	}
	return 0;         //���ͳɹ�
}

//============================================================================
//�������ƣ�uart_re1
//����˵����uartNo: ���ں�:U_UART0��U_UART1��U_UART2��U_UART3��U_UART4��U_UART5
//          fp:���ճɹ���־��ָ��:*fp=0���ɹ����գ�*fp=1������ʧ��
//�������أ����շ����ֽ�
//���ܸ�Ҫ�����н���1���ֽ�
//============================================================================
uint8_t uart_re1(uint8_t uartNo, uint8_t *fp) {
	uint32_t t;
	uint8_t dat;
	UART_MemMapPtr uartch = uart_table[uartNo];         //��ȡUART1����2����ַ

	for (t = 0; t < 0xFBBB; t++)         //��ѯָ������
			{
		//�жϽ��ջ������Ƿ���
		if ((uartch->S1) & UART_S1_RDRF_MASK) {
			uint8_t dummy = uartch->S1;
			dummy++; /* For unused variable warning */
			dat = uartch->D; //��ȡ����
			*fp = 0;  //���ܳɹ�
			break;
		}

	}  //end for
	if (t >= 0xFBBB) {
		dat = 0xFF;
		*fp = 1;    //δ�յ�����
	}
	return dat;    //���ؽ��յ�������

}

//============================================================================
//�������ƣ�uart_reN
//����˵����uartNo: ���ں�:U_UART0��U_UART1��U_UART2��U_UART3��U_UART4��U_UART5
//          buff: ���ջ�����
//          len:���ճ���
//�������أ�����ִ��״̬ 0=����;��0=�쳣
//���ܸ�Ҫ������ ����n���ֽ�
//============================================================================
uint8_t uart_reN(uint8_t uartNo, uint16_t len, uint8_t* buff) {
	uint16_t i;
	uint8_t flag = 0;

	//�ж��Ƿ��ܽ�������
	for (i = 0; i < len && 0 == flag; i++) {
		buff[i] = uart_re1(uartNo, &flag); //��������
	}
	if (i < len)
		return 1; //����ʧ��
	else
		return 0; //���ճɹ�
}

//============================================================================
//�������ƣ�uart_enable_re_int
//����˵����uartNo: ���ں�:U_UART0��U_UART1��U_UART2��U_UART3��U_UART4��U_UART5
//�������أ���
//���ܸ�Ҫ�������ڽ����ж�
//============================================================================
void uart_enable_re_int(uint8_t uartNo) {
	UART_MemMapPtr uartch = uart_table[uartNo];
	uartch->C2 |= UART_C2_RIE_MASK;        //����UART�����ж�
	if (uartNo < 4)
		NVIC_EnableIRQ(UART0_RX_TX_IRQn + 2 * uartNo);   //���жϿ�����IRQ�ж�
	else
		NVIC_EnableIRQ(UART4_RX_TX_IRQn + 2 * (4 - uartNo));
}

//============================================================================
//�������ƣ�uart_disable_re_int
//����˵����uartNo: ���ں� :U_UART0��U_UART1��U_UART2��U_UART3��U_UART4��U_UART5
//�������أ���
//���ܸ�Ҫ���ش��ڽ����ж�
//============================================================================
void uart_disable_re_int(uint8_t uartNo) {
	UART_MemMapPtr uartch = uart_table[uartNo];
	uartch->C2 &= ~UART_C2_RIE_MASK;           //��ֹUART�����ж�
	if (uartNo < 4)
		NVIC_DisableIRQ(UART0_RX_TX_IRQn + 2 * uartNo);   //���жϿ�����IRQ�ж�
	else
		NVIC_DisableIRQ(UART4_RX_TX_IRQn + 2 * (4 - uartNo));
}

