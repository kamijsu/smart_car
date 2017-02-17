//==========================================================================
//文件名称：gpio.c
//功能概要：K64 GPIO底层驱动程序源文件
//==========================================================================

#include "gpio.h"

//各端口基地址
static PORT_Type * const port_table[] = PORT_BASE_PTRS;
//GPIO口基地址
static GPIO_Type * const gpio_table[] = GPIO_BASE_PTRS;
//GPIO模块中断请求号
static const IRQn_Type gpio_irq_table[] = { PORTA_IRQn, PORTB_IRQn, PORTC_IRQn,
		PORTD_IRQn, PORTE_IRQn };

//==========================================================================
//函数名称: gpio_init
//函数返回: 无
//参数说明: port_pin:(端口号)|(引脚号):
//                  COM_PORTx|p，x为端口号，p为引脚号，具体见common.h中宏定义;
//         dir:引脚方向:
//             GPIO_DIR_INPUT: 配置为输入;
//             GPIO_DIR_OUTPUT:配置为输出;
//         level:输出时为引脚电平状态，输入时为引脚上下拉状态:
//               GPIO_LEVEL_LOW:    低电平;
//               GPIO_LEVEL_HIGH:   高电平;
//               GPIO_LEVEL_UNKNOWN:未知电平(即关闭上下拉电阻)，仅在配置为输入时有效;
//功能概要: 初始化指定端口引脚为GPIO功能
//==========================================================================
void gpio_init(uint8 port_pin, uint8 dir, uint8 level) {
	uint8 port, pin;		//端口号与引脚号
	GPIO_Type * gpio_ptr;	//GPIO基地址

	//获得端口号与引脚号
	com_port_pin_resolution(port_pin, &port, &pin);
	//获取GPIO基地址
	gpio_ptr = gpio_table[port];

	//指定该引脚功能为GPIO，使MUX=0b001
	com_port_pin_set_mux(port_pin, 1);

	//配置引脚方向
	if (dir == GPIO_DIR_INPUT) {
		//配置为输入
		REG_CLR_SHIFT(GPIO_PDDR_REG(gpio_ptr), pin);
		//设定引脚上下拉状态
		gpio_set_pull(port_pin, level);
	} else {
		//配置为输出
		REG_SET_SHIFT(GPIO_PDDR_REG(gpio_ptr), pin);
		//设定引脚电平状态
		gpio_set_level(port_pin, level);
	}
}

//==========================================================================
//函数名称: gpio_set_level
//函数返回: 无
//参数说明: port_pin:(端口号)|(引脚号):
//                  COM_PORTx|p，x为端口号，p为引脚号，具体见common.h中宏定义;
//         level:引脚电平状态:
//               GPIO_LEVEL_LOW: 低电平;
//               GPIO_LEVEL_HIGH:高电平;
//功能概要: 当引脚配置为输出时，设定引脚电平状态为指定电平
//==========================================================================
void gpio_set_level(uint8 port_pin, uint8 level) {
	uint8 port, pin;		//端口号与引脚号
	GPIO_Type * gpio_ptr;	//GPIO基地址

	//获得端口号与引脚号
	com_port_pin_resolution(port_pin, &port, &pin);
	//获取该端口GPIO基地址
	gpio_ptr = gpio_table[port];

	//设定引脚电平状态为指定电平
	switch (level) {
	case GPIO_LEVEL_LOW:
		//设定为低电平
		REG_CLR_SHIFT(GPIO_PDOR_REG(gpio_ptr), pin);
		break;
	case GPIO_LEVEL_HIGH:
		//设定为高电平
		REG_SET_SHIFT(GPIO_PDOR_REG(gpio_ptr), pin);
		break;
	default:
		break;
	}
}

//==========================================================================
//函数名称: gpio_toggle_level
//函数返回: 无
//参数说明: port_pin:(端口号)|(引脚号):
//                  COM_PORTx|p，x为端口号，p为引脚号，具体见common.h中宏定义;
//功能概要: 当引脚配置为输出时，反转其输出状态
//==========================================================================
void gpio_toggle_level(uint8 port_pin) {
	uint8 port, pin;		//端口号与引脚号

	//获得端口号与引脚号
	com_port_pin_resolution(port_pin, &port, &pin);

	//反转指定引脚输出状态
	REG_SET_SHIFT(GPIO_PTOR_REG(gpio_table[port]), pin);
}

//==========================================================================
//函数名称: gpio_set_drive_strength
//函数返回: 无
//参数说明: port_pin:(端口号)|(引脚号):
//                  COM_PORTx|p，x为端口号，p为引脚号，具体见common.h中宏定义;
//         enable:是否使能引脚的高驱动能力:
//                true: 高驱动能力(18mA);
//                false:正常驱动能力(5mA);
//功能概要: 当引脚配置为输出时，设定是否使能引脚的高驱动能力
//==========================================================================
void gpio_set_drive_strength(uint8 port_pin, bool enable) {
	uint8 port, pin;		//端口号与引脚号

	//获得端口号与引脚号
	com_port_pin_resolution(port_pin, &port, &pin);

	if (enable) {
		//高驱动能力
		REG_SET_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_DSE_MASK);
	} else {
		//正常驱动能力
		REG_CLR_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_DSE_MASK);
	}
}

//==========================================================================
//函数名称: gpio_set_open_drain
//函数返回: 无
//参数说明: port_pin:(端口号)|(引脚号):
//                  COM_PORTx|p，x为端口号，p为引脚号，具体见common.h中宏定义;
//         enable:是否使能引脚的开漏输出:
//                true: 使能开漏输出;
//                false:关闭开漏输出;
//功能概要: 当引脚配置为输出时，设定是否使能引脚的开漏输出
//==========================================================================
void gpio_set_open_drain(uint8 port_pin, bool enable) {
	uint8 port, pin;		//端口号与引脚号

	//获得端口号与引脚号
	com_port_pin_resolution(port_pin, &port, &pin);

	if (enable) {
		//使能开漏输出
		REG_SET_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_ODE_MASK);
	} else {
		//关闭开漏输出
		REG_CLR_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_ODE_MASK);
	}
}

//==========================================================================
//函数名称: gpio_set_slew_rate
//函数返回: 无
//参数说明: port_pin:(端口号)|(引脚号):
//                  COM_PORTx|p，x为端口号，p为引脚号，具体见common.h中宏定义;
//         enable:是否使能引脚的低电压转换速率:
//                true: 低电压转换速率;
//                false:高电压转换速率;
//功能概要: 当引脚配置为输出时，设定是否使能引脚的低电压转换速率
//==========================================================================
void gpio_set_slew_rate(uint8 port_pin, bool enable) {
	uint8 port, pin;		//端口号与引脚号

	//获得端口号与引脚号
	com_port_pin_resolution(port_pin, &port, &pin);

	if (enable) {
		//低电压转换速率
		REG_SET_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_SRE_MASK);
	} else {
		//高电压转换速率
		REG_CLR_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_SRE_MASK);
	}
}

//==========================================================================
//函数名称: gpio_get_level
//函数返回: GPIO_LEVEL_LOW(0):低电平; GPIO_LEVEL_HIGH(1):高电平
//参数说明: port_pin:(端口号)|(引脚号):
//                  COM_PORTx|p，x为端口号，p为引脚号，具体见common.h中宏定义;
//功能概要: 当引脚配置为输入时，获取其电平状态
//==========================================================================
uint8 gpio_get_level(uint8 port_pin) {
	uint8 port, pin;		//端口号与引脚号

	//获得端口号与引脚号
	com_port_pin_resolution(port_pin, &port, &pin);

	//返回引脚的状态
	return REG_GET_SHIFT(GPIO_PDIR_REG(gpio_table[port]), pin);
}

//==========================================================================
//函数名称: gpio_set_pull
//函数返回: 无
//参数说明: port_pin:(端口号)|(引脚号):
//                  COM_PORTx|p，x为端口号，p为引脚号，具体见common.h中宏定义;
//         level:引脚上下拉状态:
//               GPIO_LEVEL_LOW:    低电平;
//               GPIO_LEVEL_HIGH:   高电平;
//               GPIO_LEVEL_UNKNOWN:未知电平，即关闭上下拉电阻;
//功能概要: 当引脚配置为输入时，设定其上下拉状态
//==========================================================================
void gpio_set_pull(uint8 port_pin, uint8 level) {
	uint8 port, pin;		//端口号与引脚号
	PORT_Type * port_ptr;	//PORT基地址

	//获得端口号与引脚号
	com_port_pin_resolution(port_pin, &port, &pin);
	//获取该端口PORT基地址
	port_ptr = port_table[port];

	if (level == GPIO_LEVEL_UNKNOWN) {
		//关闭上下拉电阻
		REG_CLR_MASK(PORT_PCR_REG(port_ptr,pin), PORT_PCR_PE_MASK);
	} else {
		if (level == GPIO_LEVEL_LOW) {
			//引脚下拉电阻使能
			REG_CLR_MASK(PORT_PCR_REG(port_ptr,pin), PORT_PCR_PS_MASK);
		} else {
			//引脚上拉电阻使能
			REG_SET_MASK(PORT_PCR_REG(port_ptr,pin), PORT_PCR_PS_MASK);
		}
		//开启上下拉电阻
		REG_SET_MASK(PORT_PCR_REG(port_ptr,pin), PORT_PCR_PE_MASK);
	}
}

//==========================================================================
//函数名称: gpio_set_passive_filter
//函数返回: 无
//参数说明: port_pin:(端口号)|(引脚号):
//                  COM_PORTx|p，x为端口号，p为引脚号，具体见common.h中宏定义;
//         enable:是否使能引脚的无源滤波:
//                true: 使能无源滤波;
//                false:关闭无源滤波;
//功能概要: 当引脚配置为输入时，设定是否使能引脚的无源滤波
//==========================================================================
void gpio_set_passive_filter(uint8 port_pin, bool enable) {
	uint8 port, pin;		//端口号与引脚号

	//获得端口号与引脚号
	com_port_pin_resolution(port_pin, &port, &pin);

	if (enable) {
		//使能无源滤波
		REG_SET_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_PFE_MASK);
	} else {
		//关闭无源滤波
		REG_CLR_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_PFE_MASK);
	}
}

//==========================================================================
//函数名称: gpio_enable_int
//函数返回: 无
//参数说明: port_pin:(端口号)|(引脚号):
//                  COM_PORTx|p，x为端口号，p为引脚号，具体见common.h中宏定义;
//         int_type:引脚中断类型：
//                  GPIO_INT_LOW_LEVEL:   低电平触发;
//                  GPIO_INT_HIGH_LEVEL:  高电平触发;
//                  GPIO_INT_RISING_EDGE: 上升沿触发;
//                  GPIO_INT_FALLING_EDGE:下降沿触发;
//                  GPIO_INT_DOUBLE_EDGE: 双边沿触发;
//功能概要: 当引脚配置为输入时，根据中断类型开启该引脚中断
//==========================================================================
void gpio_enable_int(uint8 port_pin, uint8 int_type) {
	uint8 port, pin;		//端口号与引脚号
	PORT_Type * port_ptr;	//PORT基地址

	//获得端口号与引脚号
	com_port_pin_resolution(port_pin, &port, &pin);
	//获取该端口PORT基地址
	port_ptr = port_table[port];

	//清除引脚中断类型
	REG_CLR_MASK(PORT_PCR_REG(port_ptr,pin), PORT_PCR_IRQC_MASK);
	//清除引脚中断标志，并设置引脚中断类型
	REG_SET_MASK(PORT_PCR_REG(port_ptr,pin),
			PORT_PCR_ISF_MASK|PORT_PCR_IRQC(int_type));
	//允许接收该端口发送的中断请求
	ENABLE_IRQ(gpio_irq_table[port]);
}

//==========================================================================
//函数名称: gpio_disable_int
//函数返回: 无
//参数说明: port_pin:(端口号)|(引脚号):
//                  COM_PORTx|p，x为端口号，p为引脚号，具体见common.h中宏定义;
//功能概要: 当引脚配置为输入时，关闭该引脚中断
//==========================================================================
void gpio_disable_int(uint8 port_pin) {
	uint8 port, pin;		//端口号与引脚号

	//获得端口号与引脚号
	com_port_pin_resolution(port_pin, &port, &pin);
	//禁止该引脚发送中断请求，这里没有禁止端口接收中断请求，因为该端口可能有别的引脚开启中断
	REG_CLR_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_IRQC_MASK);
}

//==========================================================================
//函数名称: gpio_get_int
//函数返回: true:产生中断; false:未产生中断
//参数说明: port_pin:(端口号)|(引脚号):
//                  COM_PORTx|p，x为端口号，p为引脚号，具体见common.h中宏定义;
//功能概要: 当引脚配置为输入时，获取其中断标志
//==========================================================================
bool gpio_get_int(uint8 port_pin) {
	uint8 port, pin;		//端口号与引脚号

	//获得端口号与引脚号
	com_port_pin_resolution(port_pin, &port, &pin);
	//获取中断标志
	return (REG_GET_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_ISF_MASK)) ?
			true : false;
}

//==========================================================================
//函数名称: gpio_clear_int
//函数返回: 无
//参数说明: port_pin:(端口号)|(引脚号):
//                  COM_PORTx|p，x为端口号，p为引脚号，具体见common.h中宏定义;
//功能概要: 当引脚配置为输入时，清除其中断标志
//==========================================================================
void gpio_clear_int(uint8 port_pin) {
	uint8 port, pin;		//端口号与引脚号

	//获得端口号与引脚号
	com_port_pin_resolution(port_pin, &port, &pin);
	//清除中断标志
	REG_SET_MASK(PORT_PCR_REG(port_table[port],pin), PORT_PCR_ISF_MASK);
}
