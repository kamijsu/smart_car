//===========================================================================
//�ļ����ƣ�gpio.c
//���ܸ�Ҫ��K60 GPIO�ײ����������ļ�
//��Ȩ���У����ݴ�ѧ��˼����Ƕ��ʽ����(sumcu.suda.edu.cn)
//�汾���£�2013-05-03  V1.0
//        2014-04-25  V2.0
//        2014-10-12  V3.0
//===========================================================================

#include "gpio.h"  //����������ͷ�ļ�

//----------------------------------����Ϊ�ڲ�������Ŵ�---------------------------------------
//===========================================================================
//�������ƣ�gpio_port_pin_resolution
//�������أ���
//����˵����port_pin���˿ں�|���źţ�����U_PORTD|(3) ��ʾΪB��5�Žţ�
//        port���˿ں�
//	      pin:���źţ�0~31��ʵ��ȡֵ��оƬ���������ž�����
//���ܸ�Ҫ������������port_pin���н������ó�����˿ں������źţ�����U_PORTD|(3)
//        ����ΪPORTD��3��������ֱ�ֵ��port��pin��
//===========================================================================
static void gpio_port_pin_resolution(uint16_t port_pin, uint8_t* port,
		uint8_t* pin)
{
	*port = port_pin >> 8;
	*pin = port_pin;
}
//==========================================================================
//------------------------------------�ڲ���������-----------------------------------------

//------------------------------------�ⲿ�ӿں�����ʼ------------------------------------
//==========================================================================================
//�������ƣ�gpio_init
//�������أ���
//����˵����port_pin��(�˿ں�)|(���ź�)������U_PORTD|(3) ��ʾΪD��3�Žţ�
//        dir�����ŷ���0=���룬1=���,�������ŷ���궨�壩
//        state���˿����ų�ʼ״̬��0=�͵�ƽ��1=�ߵ�ƽ��
//���ܸ�Ҫ����ʼ��ָ���˿�������ΪGPIO���Ź��ܣ�������Ϊ�������������������
//        ��ָ����ʼ״̬�ǵ͵�ƽ��ߵ�ƽ
//===========================================================================
void gpio_init(uint16_t port_pin, uint8_t dir, uint8_t state)
{
	//�ֲ���������
	PORT_MemMapPtr port_ptr;    //����port_ptrΪPORT�ṹ������ָ��
	GPIO_MemMapPtr gpio_ptr;    //����port_ptrΪGPIO�ṹ������ָ��
	uint8_t port;                //�˿ں�
	uint8_t pin;                 //���ź�
	//��ö˿ں������źţ��ֱ𸳸�port��pin
	gpio_port_pin_resolution(port_pin, &port, &pin);

	//���ݶ˿ں�port�����ֲ�����port_ptr��gpio_ptr��ֵ(�����������ַ)
	port_ptr = PORT_ARR[port];  //���PORTģ����Ӧ�ڻ���ַ
	gpio_ptr = GPIO_ARR[port];  //���GPIOģ����Ӧ�ڻ���ַ

	//�������ź�pin,ָ�������Ź���ΪGPIO����(�������ſ��ƼĴ�����MUX=0b001)
	port_ptr->PCR[pin] &= ~PORT_PCR_MUX_MASK;   //����MUX=0b000
	port_ptr->PCR[pin] |= PORT_PCR_MUX(1);      //����MUX=0b001

	//���ݴ������dir����������Ϊ�����������
	if (1 == dir)   //ϣ��Ϊ���
	{
		REG_SET_SHIFT(gpio_ptr->PDDR,pin);   //���ݷ���Ĵ�����pinλ=1������Ϊ���
		gpio_set(port_pin, state);  //����gpio_set�������趨���ų�ʼ״̬
	}
	else
		//ϣ��Ϊ����
		REG_CLR_SHIFT(gpio_ptr->PDDR,pin);    //���ݷ���Ĵ�����pinλ=0������Ϊ����
}

//===========================================================================
//�������ƣ�gpio_set
//�������أ���
//����˵����port_pin���˿ں�|���źţ�����U_PORTD|(3) ��ʾΪD��3�Žţ�
//		  state�����ų�ʼ״̬��0=�͵�ƽ��1=�ߵ�ƽ��
//���ܸ�Ҫ���趨����״̬Ϊ�͵�ƽ��ߵ�ƽ
//===========================================================================
void gpio_set(uint16_t port_pin, uint8_t state)
{
	GPIO_MemMapPtr gpio_ptr;    //����port_ptrΪGPIO�ṹ������ָ��
	uint8_t port;
	uint8_t pin;
	gpio_port_pin_resolution(port_pin, &port, &pin);

	//���ݴ������port�����ֲ�����gpio_ptr��ֵ��GPIO����ַ��
	gpio_ptr = GPIO_ARR[port];

	//���ݴ������state����������Ϊ���1����0
	if (1 == state)
	{
		REG_SET_SHIFT(gpio_ptr->PDOR,pin);
	}
	else
	{
		REG_CLR_SHIFT(gpio_ptr->PDOR,pin);
	}
}

//===========================================================================
//�������ƣ�gpio_get
//�������أ�ָ�����ŵ�״̬��1��0��
//����˵����port_pin���˿ں�|���źţ�����U_PORTD|(3) ��ʾΪD��3�Žţ�
//���ܸ�Ҫ����ȡָ�����ŵ�״̬��1��0��
//===========================================================================
uint8_t gpio_get(uint16_t port_pin)
{
	GPIO_MemMapPtr gpio_ptr;    //����port_ptrΪGPIO�ṹ������ָ�루�׵�ַ��
	uint8_t port;
	uint8_t pin;
	gpio_port_pin_resolution(port_pin, &port, &pin);

	//���ݴ������port�����ֲ�����port_ptr��ֵ��GPIO����ַ��
	gpio_ptr = GPIO_ARR[port];

	//�������ŵ�״̬
	return ((REG_GET_SHIFT(gpio_ptr->PDIR,pin)) >= 1 ? 1 : 0);
}

//===========================================================================
//�������ƣ�gpio_reverse
//�������أ���
//����˵����port_pin���˿ں�|���źţ�����U_PORTD|(3) ��ʾΪD��3�Žţ�
//���ܸ�Ҫ����תָ���������״̬��
//===========================================================================
void gpio_reverse(uint16_t port_pin)
{
	GPIO_MemMapPtr gpio_ptr;    //����port_ptrΪGPIO�ṹ������ָ�루�׵�ַ��
	uint8_t port;
	uint8_t pin;
	gpio_port_pin_resolution(port_pin, &port, &pin);

	//���ݴ������port�����ֲ�����port_ptr��ֵ��GPIO����ַ��
	gpio_ptr = GPIO_ARR[port];

	//��תָ���������״̬
	REG_SET_SHIFT(gpio_ptr->PTOR,pin);
}

//===========================================================================
//�������ƣ�gpio_pull
//�������أ���
//����˵����port_pin���˿ں�|���źţ�����U_PORTD|(3) ��ʾΪD��3�Žţ�
//        pullselect���������ߵ͵�ƽ�� 0=���͵�ƽ��1=���ߵ�ƽ��
//���ܸ�Ҫ��ʹָ�����������ߵ�ƽ�������͵�ƽ
//===========================================================================
void gpio_pull(uint16_t port_pin, uint8_t pullselect)
{
	PORT_MemMapPtr port_ptr;    //����port_ptrΪPORT�ṹ������ָ��
	uint8_t port;
	uint8_t pin;
	gpio_port_pin_resolution(port_pin, &port, &pin);

	port_ptr = PORT_ARR[port];  //���PORTģ����Ӧ�ڻ���ַ

	port_ptr->PCR[pin] &= ~PORT_PCR_MUX_MASK;
	port_ptr->PCR[pin] |= PORT_PCR_MUX(1);

	REG_SET_SHIFT(port_ptr->PCR[pin],PORT_PCR_DSE_SHIFT);
	REG_SET_SHIFT(port_ptr->PCR[pin],PORT_PCR_PE_SHIFT);  //������������ʹ��

	//���ݴ������pullselect�������������߻�������
	if (1 == pullselect)
	{
		REG_SET_SHIFT(port_ptr->PCR[pin],PORT_PCR_PS_SHIFT);
	}
	else
	{
		REG_CLR_SHIFT(port_ptr->PCR[pin],PORT_PCR_PS_SHIFT);
	}
}

//===========================================================================
//�������ƣ�gpio_drive_strength
//�������أ���
//����˵����port_pin��(�˿ں�)|(���ź�)������U_PORTD|(3) ��ʾΪD��3�Žţ�
//        control���������ŵ�����������0=������������(5mA),Ĭ��1=����������(18mA).
//���ܸ�Ҫ�� ָ���˿����ű�����ΪGPIO���ܣ��趨������������
//===========================================================================
void gpio_drive_strength(uint16_t port_pin, uint8_t control)
{
	//�ֲ���������
	PORT_MemMapPtr port_ptr;    //����port_ptrΪPORT�ṹ������ָ��
	uint8_t port;
	uint8_t pin;
	gpio_port_pin_resolution(port_pin, &port, &pin);

	//���ݴ������port�����ֲ�����port_ptr��ֵ
	port_ptr = PORT_ARR[port];

	//���ݴ������control����������Ϊ����ߵ���������������
	REG_CLR_SHIFT( port_ptr->PCR[pin],PORT_PCR_DSE_SHIFT);
	if (1 == control)
		REG_SET_SHIFT(port_ptr->PCR[pin],PORT_PCR_DSE_SHIFT);
	else
		REG_CLR_SHIFT(port_ptr->PCR[pin],PORT_PCR_DSE_SHIFT);
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
void gpio_enable_int(uint16_t port_pin, uint8_t irqtype)
{
	PORT_MemMapPtr port_ptr;    //����port_ptrΪPORT�ṹ������ָ��
	uint8_t port;
	uint8_t pin;
	gpio_port_pin_resolution(port_pin, &port, &pin);

	port_ptr = PORT_ARR[port];  //���PORTģ����Ӧ�ڻ���ַ

	port_ptr->PCR[pin] |= PORT_PCR_ISF_MASK;  //��������жϱ�־
	port_ptr->PCR[pin] |= PORT_PCR_IRQC(irqtype); //ʹ�������ж�

	switch (port)
	{
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
void gpio_disable_int(uint16_t port_pin)
{
	PORT_MemMapPtr port_ptr;    //����port_ptrΪPORT�ṹ������ָ��
	uint8_t port;
	uint8_t pin;
	gpio_port_pin_resolution(port_pin, &port, &pin);

	port_ptr = PORT_ARR[port];  //���PORTģ����Ӧ�ڻ���ַ

	port_ptr->PCR[pin] &= ~PORT_PCR_IRQC_MASK;  //���������ж�

	switch (port)
	{
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
