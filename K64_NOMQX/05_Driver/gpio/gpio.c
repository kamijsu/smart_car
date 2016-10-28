//==========================================================================
//�ļ����ƣ�gpio.c
//���ܸ�Ҫ��K64 GPIO�ײ���������Դ�ļ�
//==========================================================================

#include "gpio.h"

//���˿ڻ���ַ
static PORT_Type * const port_table[] = { PORTA, PORTB, PORTC, PORTD, PORTE };
//GPIO�ڻ���ַ
static GPIO_Type * const gpio_table[] = { PTA, PTB, PTC, PTD, PTE };

//===========================================================================
//��������: gpio_port_pin_resolution
//��������: ��
//����˵��: port_pin:(�˿ں�)|(���ź�)�������common.h�к궨��
//         port:�˿ں�
//	       pin:���ź�(0~31��ʵ��ȡֵ��оƬ���������ž���)
//���ܸ�Ҫ: ����������port_pin���н������ó�����˿ں������ź�
//===========================================================================
static void gpio_port_pin_resolution(uint16 port_pin, uint8* port, uint8* pin) {
	*port = port_pin >> 8;
	*pin = port_pin;
}
//==========================================================================

//==========================================================================
//��������: gpio_init
//��������: ��
//����˵��: port_pin:(�˿ں�)|(���ź�)�������common.h�к궨��
//         dir:���ŷ���GPIO_INPUT:����Ϊ����; GPIO_OUTPUT:����Ϊ���
//         state:���ʱΪ���ŵ�ƽ״̬������ʱΪ����������״̬��GPIO_LEVEL_LOW:�͵�ƽ;
//               GPIO_LEVEL_HIGH:�ߵ�ƽ; GPIO_LEVEL_UNKNOWN:δ֪��ƽ����������Ϊ����ʱ��Ч
//���ܸ�Ҫ: ��ʼ��ָ���˿�����ΪGPIO����
//==========================================================================
void gpio_init(uint16 port_pin, uint8 dir, uint8 state) {
	uint8 port;		//�˿ں�
	uint8 pin;		//���ź�

	//��ö˿ں������ź�
	gpio_port_pin_resolution(port_pin, &port, &pin);

	//ָ�������Ź���ΪGPIO
	REG_CLR_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_MUX_MASK);//���MUXλ
	REG_SET_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_MUX(1));//ʹMUX=0b001

	//�������ŷ���
	if (dir == GPIO_OUTPUT) {
		REG_SET_SHIFT(GPIO_PDDR_REG(gpio_table[port]), pin);	//����Ϊ���
		gpio_set(port_pin, state);  //�趨���ŵ�ƽ״̬
	} else {
		REG_CLR_SHIFT(GPIO_PDDR_REG(gpio_table[port]), pin);	//����Ϊ����
		gpio_pull(port_pin, state);  //�趨����������״̬
	}
}

//==========================================================================
//��������: gpio_set
//��������: ��
//����˵��: port_pin:(�˿ں�)|(���ź�)�������common.h�к궨��
//         state:���ŵ�ƽ״̬��GPIO_LEVEL_LOW:�͵�ƽ; GPIO_LEVEL_HIGH:�ߵ�ƽ
//���ܸ�Ҫ: ����������Ϊ���ʱ���趨����״̬Ϊָ��״̬
//==========================================================================
void gpio_set(uint16 port_pin, uint8 state) {
	uint8 port;		//�˿ں�
	uint8 pin;		//���ź�

	//��ö˿ں������ź�
	gpio_port_pin_resolution(port_pin, &port, &pin);

	//�趨����״̬Ϊָ��״̬
	switch (state) {
	case GPIO_LEVEL_LOW:
		REG_CLR_SHIFT(GPIO_PDOR_REG(gpio_table[port]), pin);	//�趨Ϊ�͵�ƽ
		break;
	case GPIO_LEVEL_HIGH:
		REG_SET_SHIFT(GPIO_PDOR_REG(gpio_table[port]), pin);	//�趨Ϊ�ߵ�ƽ
		break;
	default:
		break;
	}
}

//===========================================================================
//�������ƣ�gpio_get
//�������أ�ָ�����ŵ�״̬��1��0��
//����˵����port_pin���˿ں�|���źţ�����U_PORTD|(3) ��ʾΪD��3�Žţ�
//���ܸ�Ҫ����ȡָ�����ŵ�״̬��1��0��
//===========================================================================
uint8_t gpio_get(uint16_t port_pin) {
	GPIO_MemMapPtr gpio_ptr;    //����port_ptrΪGPIO�ṹ������ָ�루�׵�ַ��
	uint8_t port;
	uint8_t pin;
	gpio_port_pin_resolution(port_pin, &port, &pin);

	//���ݴ������port�����ֲ�����port_ptr��ֵ��GPIO����ַ��
	gpio_ptr = GPIO_ARR[port];

	//�������ŵ�״̬
	return ((REG_GET_SHIFT(gpio_ptr->PDIR, pin)) >= 1 ? 1 : 0);
}

//===========================================================================
//�������ƣ�gpio_reverse
//�������أ���
//����˵����port_pin���˿ں�|���źţ�����U_PORTD|(3) ��ʾΪD��3�Žţ�
//���ܸ�Ҫ����תָ���������״̬��
//===========================================================================
void gpio_reverse(uint16_t port_pin) {
	GPIO_MemMapPtr gpio_ptr;    //����port_ptrΪGPIO�ṹ������ָ�루�׵�ַ��
	uint8_t port;
	uint8_t pin;
	gpio_port_pin_resolution(port_pin, &port, &pin);

	//���ݴ������port�����ֲ�����port_ptr��ֵ��GPIO����ַ��
	gpio_ptr = GPIO_ARR[port];

	//��תָ���������״̬
	REG_SET_SHIFT(gpio_ptr->PTOR, pin);
}

//===========================================================================
//�������ƣ�gpio_pull
//�������أ���
//����˵����port_pin���˿ں�|���źţ�����U_PORTD|(3) ��ʾΪD��3�Žţ�
//        pullselect���������ߵ͵�ƽ�� 0=���͵�ƽ��1=���ߵ�ƽ��
//���ܸ�Ҫ��ʹָ�����������ߵ�ƽ�������͵�ƽ
//===========================================================================
void gpio_pull(uint16_t port_pin, uint8_t pullselect) {
	PORT_MemMapPtr port_ptr;    //����port_ptrΪPORT�ṹ������ָ��
	uint8_t port;
	uint8_t pin;
	gpio_port_pin_resolution(port_pin, &port, &pin);

	port_ptr = PORT_ARR[port];  //���PORTģ����Ӧ�ڻ���ַ

	port_ptr->PCR[pin] &= ~PORT_PCR_MUX_MASK;
	port_ptr->PCR[pin] |= PORT_PCR_MUX(1);

	REG_SET_SHIFT(port_ptr->PCR[pin], PORT_PCR_DSE_SHIFT);
	REG_SET_SHIFT(port_ptr->PCR[pin], PORT_PCR_PE_SHIFT);  //������������ʹ��

	//���ݴ������pullselect�������������߻�������
	if (1 == pullselect) {
		REG_SET_SHIFT(port_ptr->PCR[pin], PORT_PCR_PS_SHIFT);
	} else {
		REG_CLR_SHIFT(port_ptr->PCR[pin], PORT_PCR_PS_SHIFT);
	}
}

//===========================================================================
//�������ƣ�gpio_drive_strength
//�������أ���
//����˵����port_pin��(�˿ں�)|(���ź�)������U_PORTD|(3) ��ʾΪD��3�Žţ�
//        control���������ŵ�����������0=������������(5mA),Ĭ��1=����������(18mA).
//���ܸ�Ҫ�� ָ���˿����ű�����ΪGPIO���ܣ��趨������������
//===========================================================================
void gpio_drive_strength(uint16_t port_pin, uint8_t control) {
	//�ֲ���������
	PORT_MemMapPtr port_ptr;    //����port_ptrΪPORT�ṹ������ָ��
	uint8_t port;
	uint8_t pin;
	gpio_port_pin_resolution(port_pin, &port, &pin);

	//���ݴ������port�����ֲ�����port_ptr��ֵ
	port_ptr = PORT_ARR[port];

	//���ݴ������control����������Ϊ����ߵ���������������
	REG_CLR_SHIFT(port_ptr->PCR[pin], PORT_PCR_DSE_SHIFT);
	if (1 == control)
		REG_SET_SHIFT(port_ptr->PCR[pin], PORT_PCR_DSE_SHIFT);
	else
		REG_CLR_SHIFT(port_ptr->PCR[pin], PORT_PCR_DSE_SHIFT);
}

//===================================================================================
//�������ƣ�gpio_enable_int
//�������أ���
//����˵����port_pin��(�˿ں�)|(���ź�)������U_PORTD|(3) ��ʾΪD��3�Žţ�
//       irqtype�������ж����ͣ��ɺ궨��������ٴ��о����£�
//                  LOW_LEVEL    8      //�͵�ƽ����
//                  HIGH_LEVEL   12     //�ߵ�ƽ����
//                  RISING_EDGE  9      //�����ش���
//                  FALLING_EDGE 10     //�½��ش���
//                  DOUBLE_EDGE  11     //˫���ش���
//���ܸ�Ҫ��ָ���˿����ű�����ΪGPIO������Ϊ����ʱ�����������������жϣ��������жϴ���������
//=====================================================================================
void gpio_enable_int(uint16_t port_pin, uint8_t irqtype) {
	PORT_MemMapPtr port_ptr;    //����port_ptrΪPORT�ṹ������ָ��
	uint8_t port;
	uint8_t pin;
	gpio_port_pin_resolution(port_pin, &port, &pin);

	port_ptr = PORT_ARR[port];  //���PORTģ����Ӧ�ڻ���ַ

	port_ptr->PCR[pin] |= PORT_PCR_ISF_MASK;  //��������жϱ�־
	port_ptr->PCR[pin] |= PORT_PCR_IRQC(irqtype); //ʹ�������ж�

	switch (port) {
	case 0: //PTA
		NVIC_EnableIRQ(PORTA_IRQn);   //���жϿ�����IRQ�ж�
		break;
	case 1:   //PTB
		NVIC_EnableIRQ(PORTB_IRQn);   //���жϿ�����IRQ�ж�
		break;
	case 2:   //PTC
		NVIC_EnableIRQ(PORTC_IRQn);   //���жϿ�����IRQ�ж�
		break;
	case 3:   //PTD
		NVIC_EnableIRQ(PORTD_IRQn);   //���жϿ�����IRQ�ж�
		break;
	case 4:   //PTE
		NVIC_EnableIRQ(PORTE_IRQn);   //���жϿ�����IRQ�ж�
		break;
	default:
		;
	}
}

//===========================================================================
//�������ƣ�gpio_disable_int
//�������أ���
//����˵����port_pin��(�˿ں�)|(���ź�)������U_PORTD|(3) ��ʾΪD��3�Žţ�
//���ܸ�Ҫ����ָ���˿����ű�����ΪGPIO������Ϊ����ʱ���������ر������ж�
//===========================================================================
void gpio_disable_int(uint16_t port_pin) {
	PORT_MemMapPtr port_ptr;    //����port_ptrΪPORT�ṹ������ָ��
	uint8_t port;
	uint8_t pin;
	gpio_port_pin_resolution(port_pin, &port, &pin);

	port_ptr = PORT_ARR[port];  //���PORTģ����Ӧ�ڻ���ַ

	port_ptr->PCR[pin] &= ~PORT_PCR_IRQC_MASK;  //���������ж�

	switch (port) {
	case 0:  //PTA
		NVIC_DisableIRQ(PORTA_IRQn);   //���жϿ�����IRQ�ж�
		break;
	case 1:   //PTB
		NVIC_DisableIRQ(PORTB_IRQn);   //���жϿ�����IRQ�ж�
		break;
	case 2:   //PTC
		NVIC_DisableIRQ(PORTC_IRQn);   //���жϿ�����IRQ�ж�
		break;
	case 3:   //PTD
		NVIC_DisableIRQ(PORTD_IRQn);   //���жϿ�����IRQ�ж�
		break;
	case 4:   //PTE
		NVIC_DisableIRQ(PORTE_IRQn);   //���жϿ�����IRQ�ж�
		break;
	default:
		;
	}
}
//---------------------------�ⲿ�ӿں�������-------------------------------

//-------------------------------��      ע------------------------------------
//  (1)����������������
//		��������������ָ�����������������ĳ�������һ����mA��λ����
//		��KL25/KW01ֻ��PTB0,PTB1,PTD6,PTD7����,����K60ֻ��PTD7���䡣
//      ����K60оƬ��ֻ����CMT_IRO��ص����Ų��ܱ�����Ϊ������������
//      ��ͨ���鿴оƬ�ֲ�ȷ��ֻ��PTD7��CMT_IRO�ź���ء�
//
//  (2)GPIO���ж�����
//      KL25оƬ��ֻ��PORTA��PORTD�ھ���GPIO���жϹ���
//      KW01оƬ��ֻ��PORTA��PORTC��PORTD�ھ���GPIO���жϹ���
//		k60оƬ��PORTA��PORTB��PORTC��PORTD��PORTE������GPIO���жϹ���
//		ע:��ͬоƬ��ͬһ��GPIO�˿ڵ�IRQ��ͬ������KL25оƬPORTA��GPIO��
//         �ж�IRQ��30����K60оƬPORTA��GPIO���ж�IRQ��87����ͨ���鿴о
//         Ƭ�ֲ�ȷ����ӦоƬ�ĸ�GPIO�˿����ж�IRQ��
