//============================================================================
//�ļ����ƣ�uart.c
//���ܸ�Ҫ��uart�ײ���������Դ�ļ�
//��Ȩ���У����ݴ�ѧ��˼����Ƕ��ʽ����(sumcu.suda.edu.cn)
//���¼�¼��2014-10-15   V2.0
//============================================================================

#include "uart.h"

//=====����0��1��2��3��4��5�Ŵ��ڵ�ַӳ��====
static const UART_MemMapPtr UART_ARR[6] =
{
UART0_BASE_PTR, UART1_BASE_PTR, UART2_BASE_PTR,
UART3_BASE_PTR, UART4_BASE_PTR, UART5_BASE_PTR };

//====================�ӿں���ʵ��============================================

//========================================================================
//�������ƣ�uart_init
//���ܸ�Ҫ����ʼ��uartģ��
//����˵����uartNo:���ںţ�U_UART0��U_UART1��U_UART2��U_UART3��U_UART4��U_UART5
//          sel_clk:ѡ�񴮿�ʱ��Դ:
//                �ں�ʱ��   (96000Khz)
//		  ����ʱ��    (48000khz)
//          baud:�����ʣ�300��600��1200��2400��4800��9600��19200��115200...
//�������أ���
//=========================================================================
uint8_t uart_init(uint8_t uartNo, uint32_t baud)
{
	register uint16_t sbr;
	uint16_t brfa;
	uint8_t temp;
	uint32_t sel_clk;
	UART_MemMapPtr uartch = UART_ARR[uartNo];

	//ʹ�����Ź��ܲ���������ʱ��
	switch (uartNo)
	{
	case 0:
		#if (U_UART0_GROUP==1)
				PORTA_PCR1 = PORT_PCR_MUX(0x2); //ʹ��UART0_TXD
				PORTA_PCR2 = PORT_PCR_MUX(0x2);//ʹ��UART0_RXD
		#endif

		#if (U_UART0_GROUP==2)
				PORTA_PCR14 = PORT_PCR_MUX(0x3); //ʹ��UART0_TXD
				PORTA_PCR15 = PORT_PCR_MUX(0x3);//ʹ��UART0_RXD
		#endif

		#if (U_UART0_GROUP==3)
				PORTB_PCR17 = PORT_PCR_MUX(0x3); //ʹ��UART0_TXD
				PORTB_PCR16 = PORT_PCR_MUX(0x3);//ʹ��UART0_RXD
		#endif

		#if (U_UART0_GROUP==4)
				PORTD_PCR7 = PORT_PCR_MUX(0x3); //ʹ��UART0_TXD
				PORTD_PCR6 = PORT_PCR_MUX(0x3);//ʹ��UART0_RXD
		#endif

		BSET(SIM_SCGC4_UART0_SHIFT, SIM_SCGC4); //��������0ʱ��
		break;
	case 1:
		#if (U_UART1_GROUP==1)
				PORTC_PCR4 = PORT_PCR_MUX(0x3); //ʹ��UART1_TXD
				PORTC_PCR3 = PORT_PCR_MUX(0x3);//ʹ��UART1_RXD
		#endif

		#if (U_UART1_GROUP==2)
				PORTE_PCR0 = PORT_PCR_MUX(0x3); //ʹ��UART1_TXD
				PORTE_PCR1 = PORT_PCR_MUX(0x3); //ʹ��UART1_RXD
		#endif

				BSET(SIM_SCGC4_UART1_SHIFT, SIM_SCGC4); //��������1ʱ��
				break;
			case 2:
		#if (U_UART2_GROUP==1)
				PORTD_PCR3 = PORT_PCR_MUX(0x3); //ʹ��UART2_TXD
				PORTD_PCR2 = PORT_PCR_MUX(0x3);//ʹ��UART2_RXD
		#endif

		BSET(SIM_SCGC4_UART2_SHIFT, SIM_SCGC4); //��������2ʱ��
		break;
	case 3:
		#if (U_UART3_GROUP==1)
				PORTB_PCR11 = PORT_PCR_MUX(0x3); //ʹ��UART3_TXD
				PORTB_PCR10 = PORT_PCR_MUX(0x3);//ʹ��UART3_RXD
		#endif

		#if (U_UART3_GROUP==2)
				PORTC_PCR17 = PORT_PCR_MUX(0x3); //ʹ��UART3_TXD
				PORTC_PCR16 = PORT_PCR_MUX(0x3);//ʹ��UART3_RXD
		#endif

		#if (U_UART3_GROUP==3)
				PORTE_PCR4 = PORT_PCR_MUX(0x3); //ʹ��UART3_TXD
				PORTE_PCR5 = PORT_PCR_MUX(0x3);//ʹ��UART3_RXD
		#endif

		BSET(SIM_SCGC4_UART3_SHIFT, SIM_SCGC4); //��������3ʱ��
		break;
	case 4:
		#if (U_UART4_GROUP==1)
				PORTC_PCR15 = PORT_PCR_MUX(0x3); //ʹ��UART4_TXD
				PORTC_PCR14 = PORT_PCR_MUX(0x3);//ʹ��UART4_RXD
		#endif

		#if (U_UART4_GROUP==2)
				PORTE_PCR24 = PORT_PCR_MUX(0x3); //ʹ��UART4_TXD
				PORTE_PCR25 = PORT_PCR_MUX(0x3);//ʹ��UART4_RXD
		#endif

		BSET(SIM_SCGC1_UART4_SHIFT, SIM_SCGC1); //��������4ʱ��
		break;
	case 5:
		#if (U_UART5_GROUP==1)
				PORTD_PCR9 = PORT_PCR_MUX(0x3); //ʹ��UART4_TXD
				PORTD_PCR8 = PORT_PCR_MUX(0x3);//ʹ��UART4_RXD
		#endif

		#if (U_UART5_GROUP==2)
				PORTE_PCR8 = PORT_PCR_MUX(0x3); //ʹ��UART5_TXD
				PORTE_PCR9 = PORT_PCR_MUX(0x3);//ʹ��UART5_RXD
		#endif

		BSET(SIM_SCGC1_UART5_SHIFT, SIM_SCGC1); //��������5ʱ��
		break;
	default:
		return 1;  //���δ��󣬷���
	}

	//���ô��ڹ���ģʽ,8λ��У��ģʽ
	uartch->C1 = 0;

	//��ʱ�رմ��ڷ�������չ���
	uartch->C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK);

	//���ò�����
	//����0��1ʹ�õ��ں�ʱ������������ʹ������ʱ��Ƶ�ʵ�2��
	if (U_UART0 == uartNo || U_UART1 == uartNo)
		sel_clk = SYSTEM_CLK_KHZ;
	else
		sel_clk = BUS_CLK_KHZ;

	sbr = (uint16_t) ((sel_clk * 1000) / (baud * 16));
	temp = UART_BDH_REG(uartch) & ~(UART_BDH_SBR(0x1F));
	UART_BDH_REG(uartch) = temp | UART_BDH_SBR(((sbr & 0x1F00) >> 8));
	UART_BDL_REG(uartch) = (uint8_t) (sbr & UART_BDL_SBR_MASK);

//	brfa = (32*sel_clk/(baud*16)) - 32*sbr;
//	uartch->C4 |= UART_C4_BRFA(brfa);
	//�˾�ŵ�λ����Ҫע��
	UART_BDH_REG(uartch) |= UART_BDH_SBNS_MASK;   //��λֹͣλ

	//��ʼ�����ƼĴ��������־λ
	uartch->C1 = 0x00;
	uartch->C3 = 0x00;
	uartch->S2 = 0x00;

	//�������ͽ���
	UART_C2_REG(uartch) |= (UART_C2_TE_MASK | UART_C2_RE_MASK);

	return 0;
}

//============================================================================
//�������ƣ�uart_send1
//����˵����uartNo: ���ں�:U_UART0��U_UART1��U_UART2��U_UART3��U_UART4��U_UART5
//          ch:Ҫ���͵��ֽ�
//�������أ�����ִ��״̬��0=��������0=�쳣��
//���ܸ�Ҫ�����з���1���ֽ�
//============================================================================
uint8_t uart_send1(uint8_t uartNo, uint8_t ch)
{
	uint32_t t;
	UART_MemMapPtr uartch = UART_ARR[uartNo]; //��ȡUART1����2����ַ

	for (t = 0; t < 0xFBBB; t++) //��ѯָ������
	{
		if ((uartch->S1) & UART_S1_TDRE_MASK) //�жϷ��ͻ������Ƿ�Ϊ��
		{
			uartch->D = ch;   //��ȡ���ݲ�����
			break;            //����ѭ��
		}
	}            //end for
	if (t >= 0xFBBB)
		return 1; //���ͳ�ʱ������ʧ��
	else
		return 0; //�ɹ�����

}

//============================================================================
//�������ƣ�uart_sendN
//����˵����uartNo: ���ں�:U_UART0��U_UART1��U_UART2��U_UART3��U_UART4��U_UART5
//          buff: ���ͻ�����
//          len:���ͳ���
//�������أ� ����ִ��״̬��0=������1=�쳣
//���ܸ�Ҫ������ ����n���ֽ�   
//============================================================================
uint8_t uart_sendN(uint8_t uartNo, uint16_t len, uint8_t* buff)
{
	uint16_t i;
	for (i = 0; i < len; i++)
	{
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
uint8_t uart_send_string(uint8_t uartNo, void *buff)
{
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
uint8_t uart_re1(uint8_t uartNo, uint8_t *fp)
{
	uint32_t t;
	uint8_t dat;
	UART_MemMapPtr uartch = UART_ARR[uartNo];         //��ȡUART1����2����ַ

	for (t = 0; t < 0xFBBB; t++)         //��ѯָ������
	{
		//�жϽ��ջ������Ƿ���
		if ((uartch->S1) & UART_S1_RDRF_MASK)
		{
			uint8_t dummy = uartch->S1;
			dummy++; /* For unused variable warning */
			dat = uartch->D; //��ȡ����
			*fp = 0;  //���ܳɹ�
			break;
		}

	}  //end for
	if (t >= 0xFBBB)
	{
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
uint8_t uart_reN(uint8_t uartNo, uint16_t len, uint8_t* buff)
{
	uint16_t i;
	uint8_t flag = 0;

	//�ж��Ƿ��ܽ�������
	for (i = 0; i < len && 0 == flag; i++)
	{
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
void uart_enable_re_int(uint8_t uartNo)
{
	UART_MemMapPtr uartch = UART_ARR[uartNo];
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
void uart_disable_re_int(uint8_t uartNo)
{
	UART_MemMapPtr uartch = UART_ARR[uartNo];
	uartch->C2 &= ~UART_C2_RIE_MASK;           //��ֹUART�����ж�
	if (uartNo < 4)
		NVIC_DisableIRQ(UART0_RX_TX_IRQn + 2 * uartNo);   //���жϿ�����IRQ�ж�
	else
		NVIC_DisableIRQ(UART4_RX_TX_IRQn + 2 * (4 - uartNo));
}

