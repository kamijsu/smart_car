//==========================================================================
//�ļ����ƣ�gpio.c
//���ܸ�Ҫ��K64 GPIO�ײ���������Դ�ļ�
//==========================================================================

#include "gpio.h"

//GPIO�ڻ���ַ
static GPIO_Type * const gpio_table[] = { PTA, PTB, PTC, PTD, PTE };
//GPIOģ���ж������
static const IRQn_Type gpio_irq_table[] = { PORTA_IRQn, PORTB_IRQn, PORTC_IRQn,
		PORTD_IRQn, PORTE_IRQn };

//==========================================================================
//��������: gpio_init
//��������: ��
//����˵��: port_pin:(�˿ں�)|(���ź�)�������common.h�к궨��
//         dir:���ŷ���GPIO_INPUT:����Ϊ����; GPIO_OUTPUT:����Ϊ���
//         status:���ʱΪ���ŵ�ƽ״̬������ʱΪ����������״̬��GPIO_LEVEL_LOW:�͵�ƽ;
//                GPIO_LEVEL_HIGH:�ߵ�ƽ; GPIO_LEVEL_UNKNOWN:δ֪��ƽ����������Ϊ����ʱ��Ч
//���ܸ�Ҫ: ��ʼ��ָ���˿�����ΪGPIO����
//==========================================================================
void gpio_init(uint8 port_pin, uint8 dir, uint8 status) {
	uint8 port, pin;		//�˿ں������ź�

	//��ö˿ں������ź�
	com_port_pin_resolution(port_pin, &port, &pin);

	//ָ�������Ź���ΪGPIO
	REG_CLR_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_MUX_MASK);//���MUXλ
	REG_SET_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_MUX(1));//ʹMUX=0b001

	//�������ŷ���
	if (dir == GPIO_INPUT) {
		REG_CLR_SHIFT(GPIO_PDDR_REG(gpio_table[port]), pin);	//����Ϊ����
		gpio_pull(port_pin, status);  //�趨����������״̬
	} else {
		REG_SET_SHIFT(GPIO_PDDR_REG(gpio_table[port]), pin);	//����Ϊ���
		gpio_set(port_pin, status);  //�趨���ŵ�ƽ״̬
	}
}

//==========================================================================
//��������: gpio_set
//��������: ��
//����˵��: port_pin:(�˿ں�)|(���ź�)�������common.h�к궨��
//         status:���ŵ�ƽ״̬��GPIO_LEVEL_LOW:�͵�ƽ; GPIO_LEVEL_HIGH:�ߵ�ƽ
//���ܸ�Ҫ: ����������Ϊ���ʱ���趨����״̬Ϊָ��״̬
//==========================================================================
void gpio_set(uint8 port_pin, uint8 status) {
	uint8 port, pin;		//�˿ں������ź�

	//��ö˿ں������ź�
	com_port_pin_resolution(port_pin, &port, &pin);

	//�趨����״̬Ϊָ��״̬
	switch (status) {
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

//==========================================================================
//��������: gpio_reverse
//��������: ��
//����˵��: port_pin:(�˿ں�)|(���ź�)�������common.h�к궨��
//���ܸ�Ҫ: ����������Ϊ���ʱ����ת�����״̬
//==========================================================================
void gpio_reverse(uint8 port_pin) {
	uint8 port, pin;		//�˿ں������ź�

	//��ö˿ں������ź�
	com_port_pin_resolution(port_pin, &port, &pin);

	//��תָ���������״̬
	REG_SET_SHIFT(GPIO_PTOR_REG(gpio_table[port]), pin);
}

//==========================================================================
//��������: gpio_drive_strength
//��������: ��
//����˵��: port_pin:(�˿ں�)|(���ź�)�������common.h�к궨��
//         status:���ŵ�����������GPIO_DRIVE_LOW:������������(5mA);
//                GPIO_DRIVE_HIGH:����������(18mA)
//���ܸ�Ҫ: ����������Ϊ���ʱ���趨����������
//��ע:     K64оƬֻ��PTD7���Ա�����Ϊ����������
//==========================================================================
void gpio_drive_strength(uint8 port_pin, uint8 status) {
	uint8 port, pin;		//�˿ں������ź�

	//��ö˿ں������ź�
	com_port_pin_resolution(port_pin, &port, &pin);

	//�趨��������
	if (status == GPIO_DRIVE_LOW) {
		//������������
		REG_CLR_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_DSE_MASK);
	} else {
		//����������
		REG_SET_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_DSE_MASK);
	}
}

//==========================================================================
//��������: gpio_pull
//��������: ��
//����˵��: port_pin:(�˿ں�)|(���ź�)�������common.h�к궨��
//         status:����������״̬��GPIO_LEVEL_LOW:�͵�ƽ; GPIO_LEVEL_HIGH:�ߵ�ƽ;
//                GPIO_LEVEL_UNKNOWN:δ֪��ƽ�����ر�����������
//���ܸ�Ҫ: ����������Ϊ����ʱ���趨��������״̬
//==========================================================================
void gpio_pull(uint8 port_pin, uint8 status) {
	uint8 port, pin;		//�˿ں������ź�

	//��ö˿ں������ź�
	com_port_pin_resolution(port_pin, &port, &pin);

	if (status == GPIO_LEVEL_UNKNOWN) {
		REG_CLR_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_PE_MASK);	//�ر�����������
	} else {
		if (status == GPIO_LEVEL_LOW) {
			REG_CLR_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_PS_MASK);	//������������ʹ��
		} else {
			REG_SET_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_PS_MASK);	//������������ʹ��
		}
		REG_SET_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_PE_MASK);	//��������������
	}
}

//==========================================================================
//��������: gpio_get
//��������: GPIO_LEVEL_LOW(0):�͵�ƽ; GPIO_LEVEL_HIGH(1):�ߵ�ƽ
//����˵��: port_pin:(�˿ں�)|(���ź�)�������common.h�к궨��
//���ܸ�Ҫ: ����������Ϊ����ʱ����ȡ���ƽ״̬
//==========================================================================
uint8 gpio_get(uint8 port_pin) {
	uint8 port, pin;		//�˿ں������ź�

	//��ö˿ں������ź�
	com_port_pin_resolution(port_pin, &port, &pin);

	//�������ŵ�״̬
	return REG_GET_SHIFT(GPIO_PDIR_REG(gpio_table[port]), pin);
}

//==========================================================================
//��������: gpio_enable_int
//��������: ��
//����˵��: port_pin:(�˿ں�)|(���ź�)�������common.h�к궨��
//         int_type:�����ж����ͣ��������£�
//                  GPIO_INT_LOW_LEVEL		(8)		//�͵�ƽ����
//                  GPIO_INT_HIGH_LEVEL		(12)	//�ߵ�ƽ����
//                  GPIO_INT_RISING_EDGE	(9)		//�����ش���
//                  GPIO_INT_FALLING_EDGE	(10)	//�½��ش���
//                  GPIO_INT_DOUBLE_EDGE	(11)	//˫���ش���
//���ܸ�Ҫ: ����������Ϊ����ʱ�������ж����Ϳ����������ж�
//==========================================================================
void gpio_enable_int(uint8 port_pin, uint8 int_type) {
	uint8 port, pin;		//�˿ں������ź�

	//��ö˿ں������ź�
	com_port_pin_resolution(port_pin, &port, &pin);
	//��������жϱ�־
	REG_SET_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_ISF_MASK);
	//���������ж�����
	REG_CLR_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_IRQC_MASK);
	REG_SET_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_IRQC(int_type));
	//������ոö˿ڷ��͵��ж�����
	ENABLE_IRQ(gpio_irq_table[port]);
}

//==========================================================================
//��������: gpio_disable_int
//��������: ��
//����˵��: port_pin:(�˿ں�)|(���ź�)�������common.h�к궨��
//���ܸ�Ҫ: ����������Ϊ����ʱ���رո������ж�
//==========================================================================
void gpio_disable_int(uint8 port_pin) {
	uint8 port, pin;		//�˿ں������ź�

	//��ö˿ں������ź�
	com_port_pin_resolution(port_pin, &port, &pin);
	//��ֹ�����ŷ����ж���������û�н�ֹ�˿ڽ����ж�������Ϊ�ö˿ڿ����б�����ſ����ж�
	REG_CLR_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_IRQC_MASK);
}

//==========================================================================
//��������: gpio_get_int
//��������: true:�����ж�; false:δ�����ж�
//����˵��: port_pin:(�˿ں�)|(���ź�)�������common.h�к궨��
//���ܸ�Ҫ: ����������Ϊ����ʱ����ȡ���жϱ�־
//==========================================================================
bool gpio_get_int(uint8 port_pin) {
	uint8 port, pin;		//�˿ں������ź�

	//��ö˿ں������ź�
	com_port_pin_resolution(port_pin, &port, &pin);
	//��ȡ�жϱ�־
	return (REG_GET_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_ISF_MASK)) ?
			true : false;
}

//==========================================================================
//��������: gpio_clear_int
//��������: ��
//����˵��: port_pin:(�˿ں�)|(���ź�)�������common.h�к궨��
//���ܸ�Ҫ: ����������Ϊ����ʱ��������жϱ�־
//==========================================================================
void gpio_clear_int(uint8 port_pin) {
	uint8 port, pin;		//�˿ں������ź�

	//��ö˿ں������ź�
	com_port_pin_resolution(port_pin, &port, &pin);
	//����жϱ�־
	REG_SET_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_ISF_MASK);
}
