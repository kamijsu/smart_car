//==========================================================================
//文件名称：gpio.c
//功能概要：K64 GPIO底层驱动程序源文件
//==========================================================================

#include "gpio.h"

//各端口基地址
static PORT_Type * const port_table[] = { PORTA, PORTB, PORTC, PORTD, PORTE };
//GPIO口基地址
static GPIO_Type * const gpio_table[] = { PTA, PTB, PTC, PTD, PTE };
//GPIO模块中断请求号
static const IRQn_Type gpio_irq_table[] = { PORTA_IRQn, PORTB_IRQn, PORTC_IRQn,
		PORTD_IRQn, PORTE_IRQn };

//===========================================================================
//函数名称: gpio_port_pin_resolution
//函数返回: 无
//参数说明: port_pin:(端口号)|(引脚号)，具体见common.h中宏定义
//         port:端口号
//	       pin:引脚号(0~31，实际取值由芯片的物理引脚决定)
//功能概要: 将传进参数port_pin进行解析，得出具体端口号与引脚号
//===========================================================================
static void gpio_port_pin_resolution(uint16 port_pin, uint8* port, uint8* pin) {
	*port = port_pin >> 8;
	*pin = port_pin;
}

//==========================================================================
//函数名称: gpio_init
//函数返回: 无
//参数说明: port_pin:(端口号)|(引脚号)，具体见common.h中宏定义
//         dir:引脚方向，GPIO_INPUT:配置为输入; GPIO_OUTPUT:配置为输出
//         state:输出时为引脚电平状态，输入时为引脚上下拉状态，GPIO_LEVEL_LOW:低电平;
//               GPIO_LEVEL_HIGH:高电平; GPIO_LEVEL_UNKNOWN:未知电平，仅在配置为输入时有效
//功能概要: 初始化指定端口引脚为GPIO功能
//==========================================================================
void gpio_init(uint16 port_pin, uint8 dir, uint8 state) {
	uint8 port, pin;		//端口号与引脚号

	//获得端口号与引脚号
	gpio_port_pin_resolution(port_pin, &port, &pin);

	//指定该引脚功能为GPIO
	REG_CLR_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_MUX_MASK);//清空MUX位
	REG_SET_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_MUX(1));//使MUX=0b001

	//配置引脚方向
	if (dir == GPIO_INPUT) {
		REG_CLR_SHIFT(GPIO_PDDR_REG(gpio_table[port]), pin);	//配置为输入
		gpio_pull(port_pin, state);  //设定引脚上下拉状态
	} else {
		REG_SET_SHIFT(GPIO_PDDR_REG(gpio_table[port]), pin);	//配置为输出
		gpio_set(port_pin, state);  //设定引脚电平状态
	}
}

//==========================================================================
//函数名称: gpio_set
//函数返回: 无
//参数说明: port_pin:(端口号)|(引脚号)，具体见common.h中宏定义
//         state:引脚电平状态，GPIO_LEVEL_LOW:低电平; GPIO_LEVEL_HIGH:高电平
//功能概要: 当引脚配置为输出时，设定引脚状态为指定状态
//==========================================================================
void gpio_set(uint16 port_pin, uint8 state) {
	uint8 port, pin;		//端口号与引脚号

	//获得端口号与引脚号
	gpio_port_pin_resolution(port_pin, &port, &pin);

	//设定引脚状态为指定状态
	switch (state) {
	case GPIO_LEVEL_LOW:
		REG_CLR_SHIFT(GPIO_PDOR_REG(gpio_table[port]), pin);	//设定为低电平
		break;
	case GPIO_LEVEL_HIGH:
		REG_SET_SHIFT(GPIO_PDOR_REG(gpio_table[port]), pin);	//设定为高电平
		break;
	default:
		break;
	}
}

//==========================================================================
//函数名称: gpio_reverse
//函数返回: 无
//参数说明: port_pin:(端口号)|(引脚号)，具体见common.h中宏定义
//功能概要: 当引脚配置为输出时，反转其输出状态
//==========================================================================
void gpio_reverse(uint16 port_pin) {
	uint8 port, pin;		//端口号与引脚号

	//获得端口号与引脚号
	gpio_port_pin_resolution(port_pin, &port, &pin);

	//反转指定引脚输出状态
	REG_SET_SHIFT(GPIO_PTOR_REG(gpio_table[port]), pin);
}

//==========================================================================
//函数名称: gpio_drive_strength
//函数返回: 无
//参数说明: port_pin:(端口号)|(引脚号)，具体见common.h中宏定义
//         state:引脚的驱动能力，GPIO_DRIVE_LOW:正常驱动能力(5mA);
//               GPIO_DRIVE_HIGH:高驱动能力(18mA)
//功能概要: 当引脚配置为输出时，设定其驱动能力
//备注:     K64芯片只有PTD7可以被设置为高驱动能力
//==========================================================================
void gpio_drive_strength(uint16 port_pin, uint8 state) {
	uint8 port, pin;		//端口号与引脚号

	//获得端口号与引脚号
	gpio_port_pin_resolution(port_pin, &port, &pin);

	//设定驱动能力
	if (state == GPIO_DRIVE_LOW) {
		//正常驱动能力
		REG_CLR_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_DSE_MASK);
	} else {
		//高驱动能力
		REG_SET_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_DSE_MASK);
	}
}

//==========================================================================
//函数名称: gpio_pull
//函数返回: 无
//参数说明: port_pin:(端口号)|(引脚号)，具体见common.h中宏定义
//         state:引脚上下拉状态，GPIO_LEVEL_LOW:低电平; GPIO_LEVEL_HIGH:高电平;
//               GPIO_LEVEL_UNKNOWN:未知电平，即关闭上下拉电阻
//功能概要: 当引脚配置为输入时，设定其上下拉状态
//==========================================================================
void gpio_pull(uint16 port_pin, uint8 state) {
	uint8 port, pin;		//端口号与引脚号

	//获得端口号与引脚号
	gpio_port_pin_resolution(port_pin, &port, &pin);

	if (state == GPIO_LEVEL_UNKNOWN) {
		REG_CLR_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_PE_MASK);	//关闭上下拉电阻
	} else {
		if (state == GPIO_LEVEL_LOW) {
			REG_CLR_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_PS_MASK);	//引脚下拉电阻使能
		} else {
			REG_SET_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_PS_MASK);	//引脚上拉电阻使能
		}
		REG_SET_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_PE_MASK);	//开启上下拉电阻
	}
}

//==========================================================================
//函数名称: gpio_get
//函数返回: GPIO_LEVEL_LOW(0):低电平; GPIO_LEVEL_HIGH(1):高电平
//参数说明: port_pin:(端口号)|(引脚号)，具体见common.h中宏定义
//功能概要: 当引脚配置为输入时，获取其电平状态
//==========================================================================
uint8 gpio_get(uint16 port_pin) {
	uint8 port, pin;		//端口号与引脚号

	//获得端口号与引脚号
	gpio_port_pin_resolution(port_pin, &port, &pin);

	//返回引脚的状态
	return REG_GET_SHIFT(GPIO_PDIR_REG(gpio_table[port]), pin);
}

//==========================================================================
//函数名称: gpio_enable_int
//函数返回: 无
//参数说明: port_pin:(端口号)|(引脚号)，具体见common.h中宏定义
//         int_type:引脚中断类型，具体如下：
//                  GPIO_INT_LOW_LEVEL		(8)		//低电平触发
//                  GPIO_INT_HIGH_LEVEL		(12)	//高电平触发
//                  GPIO_INT_RISING_EDGE	(9)		//上升沿触发
//                  GPIO_INT_FALLING_EDGE	(10)	//下降沿触发
//                  GPIO_INT_DOUBLE_EDGE	(11)	//双边沿触发
//功能概要: 当引脚配置为输入时，根据中断类型开启该引脚中断
//==========================================================================
void gpio_enable_int(uint16 port_pin, uint8 int_type) {
	uint8 port, pin;		//端口号与引脚号

	//获得端口号与引脚号
	gpio_port_pin_resolution(port_pin, &port, &pin);
	//清除引脚中断标志
	REG_SET_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_ISF_MASK);
	//设置引脚中断类型
	REG_CLR_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_IRQC_MASK);
	REG_SET_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_IRQC(int_type));
	//允许接收该端口发送的中断请求
	ENABLE_IRQ(gpio_irq_table[port]);
}

//==========================================================================
//函数名称: gpio_disable_int
//函数返回: 无
//参数说明: port_pin:(端口号)|(引脚号)，具体见common.h中宏定义
//功能概要: 当引脚配置为输入时，关闭该引脚中断
//==========================================================================
void gpio_disable_int(uint16 port_pin) {
	uint8 port, pin;		//端口号与引脚号

	//获得端口号与引脚号
	gpio_port_pin_resolution(port_pin, &port, &pin);
	//禁止该引脚发送中断请求，这里没有禁止端口接收中断请求，因为该端口可能有别的引脚开启中断
	REG_CLR_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_IRQC_MASK);
}

//==========================================================================
//函数名称: gpio_get_int
//函数返回: GPIO_INT_TRUE(1):产生中断; GPIO_INT_FALSE(0):未产生中断
//参数说明: port_pin:(端口号)|(引脚号)，具体见common.h中宏定义
//功能概要: 当引脚配置为输入时，获取其中断标志
//==========================================================================
uint8 gpio_get_int(uint16 port_pin) {
	uint8 port, pin;		//端口号与引脚号

	//获得端口号与引脚号
	gpio_port_pin_resolution(port_pin, &port, &pin);
	//获取中断标志
	return REG_GET_SHIFT(PORT_PCR_REG(port_table[port],pin), PORT_PCR_ISF_SHIFT);
}

//==========================================================================
//函数名称: gpio_clear_int
//函数返回: 无
//参数说明: port_pin:(端口号)|(引脚号)，具体见common.h中宏定义
//功能概要: 当引脚配置为输入时，清除其中断标志
//==========================================================================
void gpio_clear_int(uint16 port_pin) {
	uint8 port, pin;		//端口号与引脚号

	//获得端口号与引脚号
	gpio_port_pin_resolution(port_pin, &port, &pin);
	//清除中断标志
	REG_SET_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_ISF_MASK);
}
