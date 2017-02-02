//==========================================================================
//文件名称：uart.c
//功能概要：K64 UART底层驱动程序源文件
//==========================================================================

#include "uart.h"
//使用可变参数
#include <stdarg.h>
//使用vasprintf
#include <stdio.h>
//使用free
#include <stdlib.h>

//各UART模块基地址
static UART_Type * const uart_table[] = UART_BASE_PTRS;
//UART各模块RX引脚号
static const uint8 uart_rx_pin_table[] =
{ UART_MOD0_RX_PIN, UART_MOD1_RX_PIN, UART_MOD2_RX_PIN,
  UART_MOD3_RX_PIN, UART_MOD4_RX_PIN, UART_MOD5_RX_PIN };
//UART各模块TX引脚号
static const uint8 uart_tx_pin_table[] =
{ UART_MOD0_TX_PIN, UART_MOD1_TX_PIN, UART_MOD2_TX_PIN,
  UART_MOD3_TX_PIN, UART_MOD4_TX_PIN, UART_MOD5_TX_PIN };
//UART各模块PCR的MUX值
static const uint8 uart_pcr_mux_table[] =
{ UART_MOD0_PCR_MUX, UART_MOD1_PCR_MUX, UART_MOD2_PCR_MUX,
  UART_MOD3_PCR_MUX, UART_MOD4_PCR_MUX, UART_MOD5_PCR_MUX };
//UART模块发送接收中断请求号
static const IRQn_Type uart_rx_tx_irq_table[] =
{ UART0_RX_TX_IRQn, UART1_RX_TX_IRQn, UART2_RX_TX_IRQn,
  UART3_RX_TX_IRQn, UART4_RX_TX_IRQn, UART5_RX_TX_IRQn };

//==========================================================================
//函数名称: uart_init
//函数返回: 无
//参数说明: mod:UART模块号:
//             UART_MODx，x为模块号;
//         baud:波特率:
//              (600) | 1200  | 2400  | 4800 | 9600 | 14400 | 19200 |
//              38400 | 56000 | 57600 | 115200
//         parity_mode:校验模式:
//                     UART_PARITY_DISABLED:不启用校验;
//                     UART_PARITY_ODD:     奇校验;
//                     UART_PARITY_EVEN:    偶校验;
//         stop_bit:停止位:
//                  UART_STOP_BIT_1:1位停止位;
//                  UART_STOP_BIT_2:2位停止位;
//         bit_order:位传输顺序:
//                   UART_BIT_ORDER_LSB:最低有效位;
//                   UART_BIT_ORDER_MSB:最高有效位;
//功能概要: 初始化UART模块
//备注: 波特率为600时，UART0与UART1无法使用;
//     位传输顺序仅影响数据位，对起始位、校验位、停止位均无影响
//==========================================================================
void uart_init(uint8 mod, uint32 baud, uint8 parity_mode, uint8 stop_bit,
		uint8 bit_order) {
	uint8 pcr_mux;	//PCR的MUX值
	UART_Type * uart_ptr;	//UART基地址
	uint16 sbr;	//波特率位，用来计算波特率
	uint8 brfa;	//波特率微调
	uint32 clk_freq;	//所用时钟频率

	//获取PCR的MUX值
	pcr_mux = uart_pcr_mux_table[mod];
	//获取UART基地址
	uart_ptr = uart_table[mod];

	//UART0和UART1使用系统时钟，其余UART模块使用总线时钟
	if (mod == UART_MOD0 || mod == UART_MOD1) {
		clk_freq = UART_WORK_FREQ1;
	} else {
		clk_freq = UART_WORK_FREQ2;
	}

	//使能引脚功能
	com_port_pin_set_mux(uart_rx_pin_table[mod], pcr_mux);
	com_port_pin_set_mux(uart_tx_pin_table[mod], pcr_mux);

	//开相应的UART模块时钟门
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

	//暂时关闭串口发送与接收功能
	REG_CLR_MASK(UART_C2_REG(uart_ptr), (UART_C2_TE_MASK | UART_C2_RE_MASK));

	//配置波特率，根据公式计算，UART波特率 = UART模块时钟/(16*(sbr+brfa/32))
	sbr = (uint16) (clk_freq / (baud << 4));
	brfa = ((clk_freq << 1) / baud) - (sbr << 5);
	//清空原值
	REG_CLR_MASK(UART_BDH_REG(uart_ptr), UART_BDH_SBR_MASK);
	REG_CLR_MASK(UART_BDL_REG(uart_ptr), UART_BDL_SBR_MASK);
	REG_CLR_MASK(UART_C4_REG(uart_ptr), UART_C4_BRFA_MASK);
	//设置计算出的值
	REG_SET_MASK(UART_BDH_REG(uart_ptr), UART_BDH_SBR(sbr >> 8));
	REG_SET_MASK(UART_BDL_REG(uart_ptr), UART_BDL_SBR(sbr));
	REG_SET_MASK(UART_C4_REG(uart_ptr), UART_C4_BRFA(brfa));

	//配置校验模式
	if (parity_mode == UART_PARITY_DISABLED) {
		REG_CLR_MASK(UART_C1_REG(uart_ptr), (UART_C1_M_MASK|UART_C1_PE_MASK));//8位数据，不开启校验
	} else {
		REG_SET_MASK(UART_C1_REG(uart_ptr), (UART_C1_M_MASK|UART_C1_PE_MASK));//9位数据(连同校验位)，开启校验
		if (parity_mode == UART_PARITY_ODD) {
			REG_SET_MASK(UART_C1_REG(uart_ptr), UART_C1_PT_MASK);	//开启奇校验
		} else {
			REG_CLR_MASK(UART_C1_REG(uart_ptr), UART_C1_PT_MASK);	//开启偶校验
		}
	}

	//设置位传输顺序
	if (bit_order == UART_BIT_ORDER_LSB) {
		REG_CLR_MASK(UART_S2_REG(uart_ptr), UART_S2_MSBF_MASK);
	} else {
		REG_SET_MASK(UART_S2_REG(uart_ptr), UART_S2_MSBF_MASK);
	}

	//设置停止位
	if (stop_bit == UART_STOP_BIT_1) {
		REG_CLR_MASK(UART_BDH_REG(uart_ptr), UART_BDH_SBNS_MASK);	//1位停止位
	} else {
		REG_SET_MASK(UART_BDH_REG(uart_ptr), UART_BDH_SBNS_MASK);	//2位停止位
	}

	//启动发送接收
	REG_SET_MASK(UART_C2_REG(uart_ptr), (UART_C2_TE_MASK | UART_C2_RE_MASK));
}

//==========================================================================
//函数名称: uart_send1
//函数返回: 无
//参数说明: mod:UART模块号:
//             UART_MODx，x为模块号;
//         byte:想要发送的字节数据
//功能概要: 阻塞式地发送1个字节数据
//==========================================================================
void uart_send1(uint8 mod, uint8 byte) {
	UART_Type * uart_ptr;	//UART基地址

	//获取UART基地址
	uart_ptr = uart_table[mod];

	//等待发送缓冲区为空
	while (!REG_GET_MASK(UART_S1_REG(uart_ptr), UART_S1_TDRE_MASK)) {
	}
	//发送该字节
	REG_SET_VAL(UART_D_REG(uart_ptr), byte);
}

//==========================================================================
//函数名称: uart_sendN
//函数返回: 无
//参数说明: mod:UART模块号:
//             UART_MODx，x为模块号;
//         buff:发送缓冲区首地址
//         len:发送的字节数
//功能概要: 阻塞式地发送N个字节数据
//==========================================================================
void uart_sendN(uint8 mod, uint8* buff, uint32 len) {
	uint32 i;

	//按顺序发送各字节
	for (i = 0; i < len; i++) {
		uart_send1(mod, buff[i]);
	}
}

//==========================================================================
//函数名称: uart_send_string
//函数返回: 无
//参数说明: mod:UART模块号:
//             UART_MODx，x为模块号;
//         str:发送字符串的首地址
//功能概要: 阻塞式地发送一个以'\0'结束的字符串，不会发送'\0'
//==========================================================================
void uart_send_string(uint8 mod, uint8* str) {
	//不为'\0'时，发送该字节
	while (*str != '\0') {
		uart_send1(mod, *str++);
	}
}

//==========================================================================
//函数名称: uart_printf
//函数返回: 发送的字符数，若小于0，说明内存不足，发送失败
//参数说明: mod:UART模块号:
//             UART_MODx，x为模块号;
//         fmt:格式控制字符串，用法同标准printf
//         ...:可变参数，类型需与格式控制字符串一致
//功能概要: 发送格式化后的字符串
//备注: 不支持float和double型的格式化，但可以按如下方法模拟发送格式化的浮点数:
//     float fval;		//要发送的浮点数
//     int32 d;			//浮点数整数部分
//     int32 temp;		//临时变量
//     uint32 f;		//浮点数小数部分
//     //获取浮点数整数部分
//     d = (int32) fval;
//     //获取浮点数小数部分，精度为3位
//     temp = (int32) ((fval * 1000) - d * 1000);
//     f = temp > 0 ? temp : -temp;
//     //当值为(-1.0f,0.0f)时，需要额外发送负号
//     if (fval < 0.0f && fval > -1.0f) {
//         uart_printf(UART_MOD1, "-");
//     }
//     //发送格式化后的浮点数
//     uart_printf(UART_MOD1, "%d.%03u", d, f);
//==========================================================================
int32 uart_printf(uint8 mod, uint8* fmt, ...) {
	va_list arg;	//指向可变参数的指针
	int32 result;	//发送的字符数
	char* ptr;		//指向格式化后的字符串的指针

	//获取可变参数列表的第一个参数的地址
	va_start(arg, fmt);
	//调用vasprintf，动态申请内存，格式化字符串，并将结果保存至ptr
	result = vasprintf(&ptr, fmt, arg);
	//结果大于等于0时，说明格式化成功
	if (result >= 0) {
		//发送格式化后的字符串
		uart_send_string(mod, ptr);
		//释放申请内存
		free(ptr);
	}
	//清空va_list可变参数列表
	va_end(arg);
	//返回发送的字符数
	return result;
}

//==========================================================================
//函数名称: uart_re1
//函数返回: true:接收成功; false:接收失败，即接收缓冲区无数据
//参数说明: mod:UART模块号:
//             UART_MODx，x为模块号;
//         byte:想要接收字节数据的地址
//功能概要: 接收1个字节数据，成功时存储该字节数据
//==========================================================================
bool uart_re1(uint8 mod, uint8* byte) {
	UART_Type * uart_ptr;	//UART基地址

	//获取UART基地址
	uart_ptr = uart_table[mod];

	//判断接收缓冲区是否满
	if (REG_GET_MASK(UART_S1_REG(uart_ptr), UART_S1_RDRF_MASK)) {
		//满时获取数据寄存器数据
		*byte = UART_D_REG(uart_ptr);
		return true;
	}
	return false;
}

//==========================================================================
//函数名称: uart_re1_parity
//函数返回: true:接收成功; false:接收失败，即接收缓冲区无数据
//参数说明: mod:UART模块号:
//             UART_MODx，x为模块号;
//         byte:想要接收字节数据的地址
//         err:接收成功时，奇偶校验有无错误，若未开启校验，则始终无错误
//功能概要: 接收1个字节数据，成功时存储该字节数据，并判断校验位有无错误
//==========================================================================
bool uart_re1_parity(uint8 mod, uint8* byte, bool* err) {
	UART_Type * uart_ptr;	//UART基地址

	//获取UART基地址
	uart_ptr = uart_table[mod];

	//判断接收缓冲区是否满
	if (REG_GET_MASK(UART_S1_REG(uart_ptr), UART_S1_RDRF_MASK)) {
		//查看奇偶校验错误标志
		*err = REG_GET_MASK(UART_S1_REG(uart_ptr), UART_S1_PF_MASK) ?
				true : false;
		//满时获取数据寄存器数据
		*byte = UART_D_REG(uart_ptr);
		return true;
	}
	return false;
}

//==========================================================================
//函数名称: uart_enable_re_int
//函数返回: 无
//参数说明: mod:UART模块号:
//             UART_MODx，x为模块号;
//功能概要: 使能串口模块接收中断，通过接收数据可以清除中断标志
//==========================================================================
void uart_enable_re_int(uint8 mod) {
	//允许发送接收中断
	REG_SET_MASK(UART_C2_REG(uart_table[mod]), UART_C2_RIE_MASK);
	//允许接收接收中断
	ENABLE_IRQ(uart_rx_tx_irq_table[mod]);
}

//==========================================================================
//函数名称: uart_disable_re_int
//函数返回: 无
//参数说明: mod:UART模块号:
//             UART_MODx，x为模块号;
//功能概要: 关闭串口模块接收中断
//==========================================================================
void uart_disable_re_int(uint8 mod) {
	//禁止发送接收中断
	REG_CLR_MASK(UART_C2_REG(uart_table[mod]), UART_C2_RIE_MASK);
	//禁止接收接收中断
	DISABLE_IRQ(uart_rx_tx_irq_table[mod]);
}
