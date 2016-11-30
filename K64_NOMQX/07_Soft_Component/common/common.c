//==========================================================================
//文件名称：common.c
//功能概要：通用文件的源文件
//==========================================================================

#include "common.h"

//===========================================================================
//函数名称: com_port_pin_resolution
//函数返回: 无
//参数说明: port_pin:(端口号)|(引脚号):
//                  COM_PORTx|p，x为端口号，p为引脚号，具体见common.h中宏定义;
//         port:获得端口号的变量
//	       pin:获得引脚号的变量(0~31，实际取值由芯片的物理引脚决定)
//功能概要: 将传进参数port_pin进行解析，得出具体端口号与引脚号
//===========================================================================
void com_port_pin_resolution(uint8 port_pin, uint8* port, uint8* pin) {
	*port = port_pin >> 5;		//端口号为高3位
	*pin = port_pin & 0x1F;		//引脚号为低5位
}

//===========================================================================
//函数名称: com_port_pin_set_mux
//函数返回: 无
//参数说明: port_pin:(端口号)|(引脚号):
//                  COM_PORTx|p，x为端口号，p为引脚号，具体见common.h中宏定义;
//         mux:相应引脚控制寄存器所要设置的MUX值
//功能概要: 设置相应引脚控制寄存器的MUX值
//===========================================================================
void com_port_pin_set_mux(uint8 port_pin, uint8 mux) {
	uint8 port, pin;	//端口号与引脚号
	PORT_Type * port_ptr;	//PORT基地址

	//获取端口号与引脚号
	com_port_pin_resolution(port_pin, &port, &pin);
	//获取PORT基地址
	port_ptr = port_table[port];

	//设置MUX值
	REG_CLR_MASK(PORT_PCR_REG(port_ptr,pin), PORT_PCR_MUX_MASK);
	REG_SET_MASK(PORT_PCR_REG(port_ptr,pin), PORT_PCR_MUX(mux));
}
