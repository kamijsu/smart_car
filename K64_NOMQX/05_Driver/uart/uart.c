//==========================================================================
//文件名称：uart.c
//功能概要：K64 UART底层驱动程序源文件
//==========================================================================

#include "uart.h"

//各UART模块基地址
static UART_Type * const uart_table[] = { UART0, UART1, UART2, UART3, UART4,
UART5 };
//UART模块发送接收中断请求号
static const IRQn_Type uart_rx_tx_irq_table[] = { UART0_RX_TX_IRQn,
		UART1_RX_TX_IRQn, UART2_RX_TX_IRQn, UART3_RX_TX_IRQn, UART4_RX_TX_IRQn,
		UART5_RX_TX_IRQn };

//==========================================================================
//函数名称: uart_init
//函数返回: 无
//参数说明: mod:UART模块号，UART_MODx，x为模块号
//         baud:波特率:(600) | 1200 | 2400 | 4800 | 9600 | 14400 | 19200 |
//                     38400 | 56000 | 57600 | 115200 | 128000 | 256000
//         parity_mode:校验模式，UART_PARITY_DISABLED:不启用校验;
//                     UART_PARITY_ODD:奇校验; UART_PARITY_EVEN:偶校验
//         stop_bit:停止位，UART_STOP_BIT_1:1位停止位; UART_STOP_BIT_2:2位停止位
//功能概要: 初始化UART模块
//备注: 波特率为600时，UART0与UART1无法使用
//==========================================================================
void uart_init(uint8 mod, uint32 baud, uint8 parity_mode, uint8 stop_bit) {
	uint16 sbr;	//波特率位，用来计算波特率
	uint8 brfa;	//波特率微调
	uint32 clk_freq;	//所用时钟频率

	//UART0和UART1使用系统时钟，其余UART模块使用总线时钟
	if (mod == UART_MOD0 || mod == UART_MOD1) {
		clk_freq = UART_WORK_FREQ1;
	} else {
		clk_freq = UART_WORK_FREQ2;
	}

	//使能引脚功能并开相应的UART模块时钟门
	switch (mod) {
	case UART_MOD0:
		REG_CLR_MASK(UART_MOD0_TX_PCR, PORT_PCR_MUX_MASK);
		REG_CLR_MASK(UART_MOD0_RX_PCR, PORT_PCR_MUX_MASK);
#if(UART_MOD0_SETUP == 0)
		REG_SET_MASK(UART_MOD0_TX_PCR, PORT_PCR_MUX(2));
		REG_SET_MASK(UART_MOD0_RX_PCR, PORT_PCR_MUX(2));
#else
		REG_SET_MASK(UART_MOD0_TX_PCR,PORT_PCR_MUX(3));
		REG_SET_MASK(UART_MOD0_RX_PCR,PORT_PCR_MUX(3));
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

	//暂时关闭串口发送与接收功能
	REG_CLR_MASK(UART_C2_REG(uart_table[mod]),
			(UART_C2_TE_MASK | UART_C2_RE_MASK));

	//配置波特率，根据公式计算，UART波特率 = UART模块时钟/(16*(sbr+brfa/32))
	sbr = (uint16) (clk_freq / (baud * 16));
	brfa = ((32 * clk_freq) / (baud * 16)) - 32 * sbr;
	//清空原值
	REG_CLR_MASK(UART_BDH_REG(uart_table[mod]), UART_BDH_SBR_MASK);
	REG_CLR_MASK(UART_BDL_REG(uart_table[mod]), UART_BDL_SBR_MASK);
	REG_CLR_MASK(UART_C4_REG(uart_table[mod]), UART_C4_BRFA_MASK);
	//设置计算出的值
	REG_SET_MASK(UART_BDH_REG(uart_table[mod]), UART_BDH_SBR(sbr >> 8));
	REG_SET_MASK(UART_BDL_REG(uart_table[mod]), UART_BDL_SBR(sbr));
	REG_SET_MASK(UART_C4_REG(uart_table[mod]), UART_C4_BRFA(brfa));

	//配置校验模式
	if (parity_mode == UART_PARITY_DISABLED) {
		REG_CLR_MASK(UART_C1_REG(uart_table[mod]), UART_C1_M_MASK);		//8位数据
		REG_CLR_MASK(UART_C1_REG(uart_table[mod]), UART_C1_PE_MASK);	//不开启校验
	} else {
		REG_SET_MASK(UART_C1_REG(uart_table[mod]), UART_C1_M_MASK);	//9位数据(连同校验位)
		REG_SET_MASK(UART_C1_REG(uart_table[mod]), UART_C1_PE_MASK);	//开启校验
		if (parity_mode == UART_PARITY_ODD) {
			REG_SET_MASK(UART_C1_REG(uart_table[mod]), UART_C1_PT_MASK);//开启奇校验
		} else {
			REG_CLR_MASK(UART_C1_REG(uart_table[mod]), UART_C1_PT_MASK);//开启偶校验
		}
	}

	//设置数据位顺序，这里设置为LSB，即紧跟起始位传输的是位0
	REG_CLR_MASK(UART_S2_REG(uart_table[mod]), UART_S2_MSBF_MASK);

	//设置停止位
	if (stop_bit == UART_STOP_BIT_1) {
		REG_CLR_MASK(UART_BDH_REG(uart_table[mod]), UART_BDH_SBNS_MASK);//1位停止位
	} else {
		REG_SET_MASK(UART_BDH_REG(uart_table[mod]), UART_BDH_SBNS_MASK);//2位停止位
	}

	//启动发送接收
	REG_SET_MASK(UART_C2_REG(uart_table[mod]),
			(UART_C2_TE_MASK | UART_C2_RE_MASK));
}

//============================================================================
//函数名称：uart_send1
//参数说明：uartNo: 串口号:U_UART0、U_UART1、U_UART2、U_UART3、U_UART4、U_UART5
//          ch:要发送的字节
//函数返回：函数执行状态：0=正常；非0=异常。
//功能概要：串行发送1个字节
//============================================================================
uint8_t uart_send1(uint8_t uartNo, uint8_t ch) {
	uint32_t t;
	UART_MemMapPtr uartch = uart_table[uartNo]; //获取UART1或者2基地址

	for (t = 0; t < 0xFBBB; t++) //查询指定次数
			{
		if ((uartch->S1) & UART_S1_TDRE_MASK) //判断发送缓冲区是否为空
		{
			uartch->D = ch;   //获取数据并发送
			break;            //跳出循环
		}
	}            //end for
	if (t >= 0xFBBB)
		return 1; //发送超时，发送失败
	else
		return 0; //成功发送

}

//============================================================================
//函数名称：uart_sendN
//参数说明：uartNo: 串口号:U_UART0、U_UART1、U_UART2、U_UART3、U_UART4、U_UART5
//          buff: 发送缓冲区
//          len:发送长度
//函数返回： 函数执行状态：0=正常；1=异常
//功能概要：串行 接收n个字节   
//============================================================================
uint8_t uart_sendN(uint8_t uartNo, uint16_t len, uint8_t* buff) {
	uint16_t i;
	for (i = 0; i < len; i++) {
		if (uart_send1(uartNo, buff[i])) //发送一个字节数据，失败则跳出循环
				{
			break;
		}
	}
	if (i < len)
		return 1;   //发送len个字节出错
	else
		return 0;   //发送len个字节成功
}

//============================================================================
//函数名称：uart_send_string
//参数说明：uartNo:UART模块号:U_UART0、U_UART1、U_UART2、U_UART3、U_UART4、U_UART5
//          buff:要发送的字符串的首地址
//函数返回： 函数执行状态：0=正常；非0=异常。
//功能概要：从指定UART端口发送一个以'\0'结束的字符串
//============================================================================
uint8_t uart_send_string(uint8_t uartNo, void *buff) {
	uint16_t i = 0;
	uint8_t *buff_ptr = (uint8_t *) buff;    //定义指针指向要发送字符串首地址
	for (i = 0; buff_ptr[i] != '\0'; i++)  //遍历字符串里的字符
			{
		if (uart_send1(uartNo, buff_ptr[i]))  //发送指针对应的字符
			return 1;  //发送失败
	}
	return 0;         //发送成功
}

//============================================================================
//函数名称：uart_re1
//参数说明：uartNo: 串口号:U_UART0、U_UART1、U_UART2、U_UART3、U_UART4、U_UART5
//          fp:接收成功标志的指针:*fp=0，成功接收；*fp=1，接收失败
//函数返回：接收返回字节
//功能概要：串行接收1个字节
//============================================================================
uint8_t uart_re1(uint8_t uartNo, uint8_t *fp) {
	uint32_t t;
	uint8_t dat;
	UART_MemMapPtr uartch = uart_table[uartNo];         //获取UART1或者2基地址

	for (t = 0; t < 0xFBBB; t++)         //查询指定次数
			{
		//判断接收缓冲区是否满
		if ((uartch->S1) & UART_S1_RDRF_MASK) {
			uint8_t dummy = uartch->S1;
			dummy++; /* For unused variable warning */
			dat = uartch->D; //获取数据
			*fp = 0;  //接受成功
			break;
		}

	}  //end for
	if (t >= 0xFBBB) {
		dat = 0xFF;
		*fp = 1;    //未收到数据
	}
	return dat;    //返回接收到的数据

}

//============================================================================
//函数名称：uart_reN
//参数说明：uartNo: 串口号:U_UART0、U_UART1、U_UART2、U_UART3、U_UART4、U_UART5
//          buff: 接收缓冲区
//          len:接收长度
//函数返回：函数执行状态 0=正常;非0=异常
//功能概要：串行 接收n个字节
//============================================================================
uint8_t uart_reN(uint8_t uartNo, uint16_t len, uint8_t* buff) {
	uint16_t i;
	uint8_t flag = 0;

	//判断是否能接受数据
	for (i = 0; i < len && 0 == flag; i++) {
		buff[i] = uart_re1(uartNo, &flag); //接受数据
	}
	if (i < len)
		return 1; //接收失败
	else
		return 0; //接收成功
}

//============================================================================
//函数名称：uart_enable_re_int
//参数说明：uartNo: 串口号:U_UART0、U_UART1、U_UART2、U_UART3、U_UART4、U_UART5
//函数返回：无
//功能概要：开串口接收中断
//============================================================================
void uart_enable_re_int(uint8_t uartNo) {
	UART_MemMapPtr uartch = uart_table[uartNo];
	uartch->C2 |= UART_C2_RIE_MASK;        //开放UART接收中断
	if (uartNo < 4)
		NVIC_EnableIRQ(UART0_RX_TX_IRQn + 2 * uartNo);   //开中断控制器IRQ中断
	else
		NVIC_EnableIRQ(UART4_RX_TX_IRQn + 2 * (4 - uartNo));
}

//============================================================================
//函数名称：uart_disable_re_int
//参数说明：uartNo: 串口号 :U_UART0、U_UART1、U_UART2、U_UART3、U_UART4、U_UART5
//函数返回：无
//功能概要：关串口接收中断
//============================================================================
void uart_disable_re_int(uint8_t uartNo) {
	UART_MemMapPtr uartch = uart_table[uartNo];
	uartch->C2 &= ~UART_C2_RIE_MASK;           //禁止UART接收中断
	if (uartNo < 4)
		NVIC_DisableIRQ(UART0_RX_TX_IRQn + 2 * uartNo);   //开中断控制器IRQ中断
	else
		NVIC_DisableIRQ(UART4_RX_TX_IRQn + 2 * (4 - uartNo));
}

