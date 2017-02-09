//==========================================================================
//�ļ����ƣ�gpio.c
//���ܸ�Ҫ��K64 GPIO�ײ���������Դ�ļ�
//==========================================================================

#include "gpio.h"

//���˿ڻ���ַ
static PORT_Type * const port_table[] = PORT_BASE_PTRS;
//GPIO�ڻ���ַ
static GPIO_Type * const gpio_table[] = GPIO_BASE_PTRS;
//GPIOģ���ж������
static const IRQn_Type gpio_irq_table[] = { PORTA_IRQn, PORTB_IRQn, PORTC_IRQn,
		PORTD_IRQn, PORTE_IRQn };

//==========================================================================
//��������: gpio_init
//��������: ��
//����˵��: port_pin:(�˿ں�)|(���ź�):
//                  COM_PORTx|p��xΪ�˿ںţ�pΪ���źţ������common.h�к궨��;
//         dir:���ŷ���:
//             GPIO_DIR_INPUT: ����Ϊ����;
//             GPIO_DIR_OUTPUT:����Ϊ���;
//         level:���ʱΪ���ŵ�ƽ״̬������ʱΪ����������״̬:
//               GPIO_LEVEL_LOW:    �͵�ƽ;
//               GPIO_LEVEL_HIGH:   �ߵ�ƽ;
//               GPIO_LEVEL_UNKNOWN:δ֪��ƽ(���ر�����������)����������Ϊ����ʱ��Ч;
//���ܸ�Ҫ: ��ʼ��ָ���˿�����ΪGPIO����
//==========================================================================
void gpio_init(uint8 port_pin, uint8 dir, uint8 level) {
	uint8 port, pin;		//�˿ں������ź�
	GPIO_Type * gpio_ptr;	//GPIO����ַ

	//��ö˿ں������ź�
	com_port_pin_resolution(port_pin, &port, &pin);
	//��ȡGPIO����ַ
	gpio_ptr = gpio_table[port];

	//ָ�������Ź���ΪGPIO��ʹMUX=0b001
	com_port_pin_set_mux(port_pin, 1);

	//�������ŷ���
	if (dir == GPIO_DIR_INPUT) {
		REG_CLR_SHIFT(GPIO_PDDR_REG(gpio_ptr), pin);	//����Ϊ����
		gpio_set_pull(port_pin, level);  //�趨����������״̬
	} else {
		REG_SET_SHIFT(GPIO_PDDR_REG(gpio_ptr), pin);	//����Ϊ���
		gpio_set_level(port_pin, level);  //�趨���ŵ�ƽ״̬
	}
}

//==========================================================================
//��������: gpio_set_level
//��������: ��
//����˵��: port_pin:(�˿ں�)|(���ź�):
//                  COM_PORTx|p��xΪ�˿ںţ�pΪ���źţ������common.h�к궨��;
//         level:���ŵ�ƽ״̬:
//               GPIO_LEVEL_LOW: �͵�ƽ;
//               GPIO_LEVEL_HIGH:�ߵ�ƽ;
//���ܸ�Ҫ: ����������Ϊ���ʱ���趨���ŵ�ƽ״̬Ϊָ����ƽ
//==========================================================================
void gpio_set_level(uint8 port_pin, uint8 level) {
	uint8 port, pin;		//�˿ں������ź�
	GPIO_Type * gpio_ptr;	//GPIO����ַ

	//��ö˿ں������ź�
	com_port_pin_resolution(port_pin, &port, &pin);
	//��ȡ�ö˿�GPIO����ַ
	gpio_ptr = gpio_table[port];

	//�趨���ŵ�ƽ״̬Ϊָ����ƽ
	switch (level) {
	case GPIO_LEVEL_LOW:
		REG_CLR_SHIFT(GPIO_PDOR_REG(gpio_ptr), pin);	//�趨Ϊ�͵�ƽ
		break;
	case GPIO_LEVEL_HIGH:
		REG_SET_SHIFT(GPIO_PDOR_REG(gpio_ptr), pin);	//�趨Ϊ�ߵ�ƽ
		break;
	default:
		break;
	}
}

//==========================================================================
//��������: gpio_toggle_level
//��������: ��
//����˵��: port_pin:(�˿ں�)|(���ź�):
//                  COM_PORTx|p��xΪ�˿ںţ�pΪ���źţ������common.h�к궨��;
//���ܸ�Ҫ: ����������Ϊ���ʱ����ת�����״̬
//==========================================================================
void gpio_toggle_level(uint8 port_pin) {
	uint8 port, pin;		//�˿ں������ź�

	//��ö˿ں������ź�
	com_port_pin_resolution(port_pin, &port, &pin);

	//��תָ���������״̬
	REG_SET_SHIFT(GPIO_PTOR_REG(gpio_table[port]), pin);
}

//==========================================================================
//��������: gpio_set_drive_strength
//��������: ��
//����˵��: port_pin:(�˿ں�)|(���ź�):
//                  COM_PORTx|p��xΪ�˿ںţ�pΪ���źţ������common.h�к궨��;
//         enable:�Ƿ�ʹ�����ŵĸ���������:
//                true: ����������(18mA);
//                false:������������(5mA);
//���ܸ�Ҫ: ����������Ϊ���ʱ���趨�Ƿ�ʹ�����ŵĸ���������
//==========================================================================
void gpio_set_drive_strength(uint8 port_pin, bool enable) {
	uint8 port, pin;		//�˿ں������ź�

	//��ö˿ں������ź�
	com_port_pin_resolution(port_pin, &port, &pin);

	if (enable) {
		//����������
		REG_SET_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_DSE_MASK);
	} else {
		//������������
		REG_CLR_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_DSE_MASK);
	}
}

//==========================================================================
//��������: gpio_set_open_drain
//��������: ��
//����˵��: port_pin:(�˿ں�)|(���ź�):
//                  COM_PORTx|p��xΪ�˿ںţ�pΪ���źţ������common.h�к궨��;
//         enable:�Ƿ�ʹ�����ŵĿ�©���:
//                true: ʹ�ܿ�©���;
//                false:�رտ�©���;
//���ܸ�Ҫ: ����������Ϊ���ʱ���趨�Ƿ�ʹ�����ŵĿ�©���
//==========================================================================
void gpio_set_open_drain(uint8 port_pin, bool enable) {
	uint8 port, pin;		//�˿ں������ź�

	//��ö˿ں������ź�
	com_port_pin_resolution(port_pin, &port, &pin);

	if (enable) {
		//ʹ�ܿ�©���
		REG_SET_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_ODE_MASK);
	} else {
		//�رտ�©���
		REG_CLR_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_ODE_MASK);
	}
}

//==========================================================================
//��������: gpio_set_slew_rate
//��������: ��
//����˵��: port_pin:(�˿ں�)|(���ź�):
//                  COM_PORTx|p��xΪ�˿ںţ�pΪ���źţ������common.h�к궨��;
//         enable:�Ƿ�ʹ�����ŵĵ͵�ѹת������:
//                true: �͵�ѹת������;
//                false:�ߵ�ѹת������;
//���ܸ�Ҫ: ����������Ϊ���ʱ���趨�Ƿ�ʹ�����ŵĵ͵�ѹת������
//==========================================================================
void gpio_set_slew_rate(uint8 port_pin, bool enable) {
	uint8 port, pin;		//�˿ں������ź�

	//��ö˿ں������ź�
	com_port_pin_resolution(port_pin, &port, &pin);

	if (enable) {
		//�͵�ѹת������
		REG_SET_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_SRE_MASK);
	} else {
		//�ߵ�ѹת������
		REG_CLR_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_SRE_MASK);
	}
}

//==========================================================================
//��������: gpio_get_level
//��������: GPIO_LEVEL_LOW(0):�͵�ƽ; GPIO_LEVEL_HIGH(1):�ߵ�ƽ
//����˵��: port_pin:(�˿ں�)|(���ź�):
//                  COM_PORTx|p��xΪ�˿ںţ�pΪ���źţ������common.h�к궨��;
//���ܸ�Ҫ: ����������Ϊ����ʱ����ȡ���ƽ״̬
//==========================================================================
uint8 gpio_get_level(uint8 port_pin) {
	uint8 port, pin;		//�˿ں������ź�

	//��ö˿ں������ź�
	com_port_pin_resolution(port_pin, &port, &pin);

	//�������ŵ�״̬
	return REG_GET_SHIFT(GPIO_PDIR_REG(gpio_table[port]), pin);
}

//==========================================================================
//��������: gpio_set_pull
//��������: ��
//����˵��: port_pin:(�˿ں�)|(���ź�):
//                  COM_PORTx|p��xΪ�˿ںţ�pΪ���źţ������common.h�к궨��;
//         level:����������״̬:
//               GPIO_LEVEL_LOW:    �͵�ƽ;
//               GPIO_LEVEL_HIGH:   �ߵ�ƽ;
//               GPIO_LEVEL_UNKNOWN:δ֪��ƽ�����ر�����������;
//���ܸ�Ҫ: ����������Ϊ����ʱ���趨��������״̬
//==========================================================================
void gpio_set_pull(uint8 port_pin, uint8 level) {
	uint8 port, pin;		//�˿ں������ź�
	PORT_Type * port_ptr;	//PORT����ַ

	//��ö˿ں������ź�
	com_port_pin_resolution(port_pin, &port, &pin);
	//��ȡ�ö˿�PORT����ַ
	port_ptr = port_table[port];

	if (level == GPIO_LEVEL_UNKNOWN) {
		REG_CLR_MASK(PORT_PCR_REG(port_ptr,pin), PORT_PCR_PE_MASK);	//�ر�����������
	} else {
		if (level == GPIO_LEVEL_LOW) {
			REG_CLR_MASK(PORT_PCR_REG(port_ptr,pin), PORT_PCR_PS_MASK);	//������������ʹ��
		} else {
			REG_SET_MASK(PORT_PCR_REG(port_ptr,pin), PORT_PCR_PS_MASK);	//������������ʹ��
		}
		REG_SET_MASK(PORT_PCR_REG(port_ptr,pin), PORT_PCR_PE_MASK);	//��������������
	}
}

//==========================================================================
//��������: gpio_set_passive_filter
//��������: ��
//����˵��: port_pin:(�˿ں�)|(���ź�):
//                  COM_PORTx|p��xΪ�˿ںţ�pΪ���źţ������common.h�к궨��;
//         enable:�Ƿ�ʹ�����ŵ���Դ�˲�:
//                true: ʹ����Դ�˲�;
//                false:�ر���Դ�˲�;
//���ܸ�Ҫ: ����������Ϊ����ʱ���趨�Ƿ�ʹ�����ŵ���Դ�˲�
//==========================================================================
void gpio_set_passive_filter(uint8 port_pin, bool enable) {
	uint8 port, pin;		//�˿ں������ź�

	//��ö˿ں������ź�
	com_port_pin_resolution(port_pin, &port, &pin);

	if (enable) {
		//ʹ����Դ�˲�
		REG_SET_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_PFE_MASK);
	} else {
		//�ر���Դ�˲�
		REG_CLR_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_PFE_MASK);
	}
}

//==========================================================================
//��������: gpio_enable_int
//��������: ��
//����˵��: port_pin:(�˿ں�)|(���ź�):
//                  COM_PORTx|p��xΪ�˿ںţ�pΪ���źţ������common.h�к궨��;
//         int_type:�����ж����ͣ�
//                  GPIO_INT_LOW_LEVEL:   �͵�ƽ����;
//                  GPIO_INT_HIGH_LEVEL:  �ߵ�ƽ����;
//                  GPIO_INT_RISING_EDGE: �����ش���;
//                  GPIO_INT_FALLING_EDGE:�½��ش���;
//                  GPIO_INT_DOUBLE_EDGE: ˫���ش���;
//���ܸ�Ҫ: ����������Ϊ����ʱ�������ж����Ϳ����������ж�
//==========================================================================
void gpio_enable_int(uint8 port_pin, uint8 int_type) {
	uint8 port, pin;		//�˿ں������ź�
	PORT_Type * port_ptr;	//PORT����ַ

	//��ö˿ں������ź�
	com_port_pin_resolution(port_pin, &port, &pin);
	//��ȡ�ö˿�PORT����ַ
	port_ptr = port_table[port];

	//��������ж�����
	REG_CLR_MASK(PORT_PCR_REG(port_ptr,pin), PORT_PCR_IRQC_MASK);
	//��������жϱ�־�������������ж�����
	REG_SET_MASK(PORT_PCR_REG(port_ptr,pin),
			PORT_PCR_ISF_MASK|PORT_PCR_IRQC(int_type));
	//������ոö˿ڷ��͵��ж�����
	ENABLE_IRQ(gpio_irq_table[port]);
}

//==========================================================================
//��������: gpio_disable_int
//��������: ��
//����˵��: port_pin:(�˿ں�)|(���ź�):
//                  COM_PORTx|p��xΪ�˿ںţ�pΪ���źţ������common.h�к궨��;
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
//����˵��: port_pin:(�˿ں�)|(���ź�):
//                  COM_PORTx|p��xΪ�˿ںţ�pΪ���źţ������common.h�к궨��;
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
//����˵��: port_pin:(�˿ں�)|(���ź�):
//                  COM_PORTx|p��xΪ�˿ںţ�pΪ���źţ������common.h�к궨��;
//���ܸ�Ҫ: ����������Ϊ����ʱ��������жϱ�־
//==========================================================================
void gpio_clear_int(uint8 port_pin) {
	uint8 port, pin;		//�˿ں������ź�

	//��ö˿ں������ź�
	com_port_pin_resolution(port_pin, &port, &pin);
	//����жϱ�־
	REG_SET_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_ISF_MASK);
}
