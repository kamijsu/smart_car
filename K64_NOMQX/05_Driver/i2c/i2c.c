//==========================================================================
//文件名称：i2c.c
//功能概要：K64 I2C底层驱动程序源文件
//==========================================================================

#include "i2c.h"
//包含gpio.h，因为要用到上拉引脚和开漏输出功能
#include "gpio.h"

//I2C各模块基地址
static I2C_Type * const i2c_table[] = I2C_BASE_PTRS;
//I2C各模块时钟信号的引脚号
static const uint8 i2c_scl_pin_table[] = { I2C_MOD0_SCL_PIN, I2C_MOD1_SCL_PIN,
I2C_MOD2_SCL_PIN };
//I2C各模块时钟信号的PCR的MUX值
static const uint8 i2c_scl_pcr_mux_table[] = { I2C_MOD0_SCL_PCR_MUX,
I2C_MOD1_SCL_PCR_MUX, I2C_MOD2_SCL_PCR_MUX };
//I2C各模块数据信号的引脚号
static const uint8 i2c_sda_pin_table[] = { I2C_MOD0_SDA_PIN, I2C_MOD1_SDA_PIN,
I2C_MOD2_SDA_PIN };
//I2C各模块数据信号的PCR的MUX值
static const uint8 i2c_sda_pcr_mux_table[] = { I2C_MOD0_SDA_PCR_MUX,
I2C_MOD1_SDA_PCR_MUX, I2C_MOD2_SDA_PCR_MUX };
//I2C各模块中断请求号
static const IRQn_Type i2c_irq_table[] = { I2C0_IRQn, I2C1_IRQn, I2C2_IRQn };

static void i2c_send_start(I2C_Type* i2c_ptr) {
	REG_SET_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_MST_MASK|I2C_C1_TX_MASK);
}

static void i2c_send_restart(I2C_Type* i2c_ptr) {
	REG_SET_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_RSTA_MASK);
}

static void i2c_send_stop(I2C_Type* i2c_ptr) {
	REG_CLR_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_MST_MASK|I2C_C1_TX_MASK);
}

static void i2c_set_ack(I2C_Type* i2c_ptr, bool ack) {
	if (ack) {
		REG_CLR_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_TXAK_MASK);
	} else {
		REG_SET_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_TXAK_MASK);
	}
}

#define I2C_DIR_WRITE	(0x0)
#define I2C_DIR_READ	(0x1)

static uint8 i2c_re_data(I2C_Type* i2c_ptr) {
	return I2C_D_REG(i2c_ptr);
}

static I2CResult i2c_wait_ack(I2C_Type* i2c_ptr) {
	uint8 status;	//状态寄存器的值
	uint32 timeout;	//从机响应超时数值

	//等待从机响应
	timeout = 0;
	do {
		//约4s，包括主机发送数据时间和从机响应时间，若为开始信号后首次发送，还会包括开始信号保持时间
		if (timeout++ >= 0x2000000u) {
			return I2CTimeout;
		}
		status = I2C_S_REG(i2c_ptr);
	} while (!REG_GET_MASK(status, I2C_S_IICIF_MASK));
	//清除中断标志
	REG_SET_VAL(I2C_S_REG(i2c_ptr), status);

	//若发生仲裁丢失
	if (REG_GET_MASK(status, I2C_S_ARBL_MASK)) {
		//返回仲裁丢失
		return I2CArbitrationLost;
	}
	//判断从机是否响应
	if (REG_GET_MASK(status, I2C_S_RXAK_MASK)) {
		//返回未响应
		return I2CNAck;
	} else {
		//返回发送成功
		return I2CSuccess;
	}
}

//icr:时钟速率
void i2c_init(uint8 mod, uint8 mul, uint8 icr, uint8 addr_mode, uint16 addr,
		bool enable_general_call_addr) {
	I2C_Type * i2c_ptr;	//I2C基地址
	uint8 scl_pin, sda_pin;	//时钟信号和数据信号引脚

	//获取I2C基地址
	i2c_ptr = i2c_table[mod];
	//获取时钟信号和数据信号引脚
	scl_pin = i2c_scl_pin_table[mod];
	sda_pin = i2c_sda_pin_table[mod];

	//开启相应I2C模块时钟门
	switch (mod) {
	case I2C_MOD0:
		REG_SET_MASK(SIM_SCGC4, SIM_SCGC4_I2C0_MASK);
		break;
	case I2C_MOD1:
		REG_SET_MASK(SIM_SCGC4, SIM_SCGC4_I2C1_MASK);
		break;
	case I2C_MOD2:
		REG_SET_MASK(SIM_SCGC1, SIM_SCGC1_I2C2_MASK);
		break;
	}

	//暂时禁用I2C模块，并关闭I2C中断，设置为从机接收模式，允许发送应答信号
	REG_CLR_MASK(I2C_C1_REG(i2c_ptr),
			I2C_C1_IICEN_MASK|I2C_C1_IICIE_MASK|I2C_C1_MST_MASK|I2C_C1_TX_MASK|I2C_C1_TXAK_MASK);

	//使能时钟信号引脚功能
	com_port_pin_set_mux(scl_pin, i2c_scl_pcr_mux_table[mod]);
	//使能数据信号引脚功能
	com_port_pin_set_mux(sda_pin, i2c_sda_pcr_mux_table[mod]);
	//设置引脚上拉电阻使能
	gpio_set_pull(scl_pin, GPIO_LEVEL_HIGH);
	gpio_set_pull(sda_pin, GPIO_LEVEL_HIGH);
	//设置引脚开漏输出使能
	gpio_set_open_drain(scl_pin, true);
	gpio_set_open_drain(sda_pin, true);

	//设置乘数因子和时钟速率
	REG_SET_VAL(I2C_F_REG(i2c_ptr), I2C_F_MULT(mul)|I2C_F_ICR(icr));

	//设置地址模式及地址
	if (addr_mode == I2C_ADDR_MODE_BITS_7) {
		//设置为7位地址模式
		REG_CLR_MASK(I2C_C2_REG(i2c_ptr), I2C_C2_ADEXT_MASK);
	} else {
		//设置为10位地址模式
		REG_SET_MASK(I2C_C2_REG(i2c_ptr), I2C_C2_ADEXT_MASK);
		//设置高3位地址
		REG_CLR_MASK(I2C_C2_REG(i2c_ptr), I2C_C2_AD_MASK);
		REG_SET_MASK(I2C_C2_REG(i2c_ptr), I2C_C2_AD(addr>>I2C_A1_AD_WIDTH));
	}
	//设置7位地址
	REG_SET_VAL(I2C_A1_REG(i2c_ptr), I2C_A1_AD(addr));

	if (enable_general_call_addr) {
		//使能广播呼叫地址
		REG_SET_MASK(I2C_C2_REG(i2c_ptr), I2C_C2_GCAEN_MASK);
	} else {
		//关闭广播呼叫地址
		REG_CLR_MASK(I2C_C2_REG(i2c_ptr), I2C_C2_GCAEN_MASK);
	}

	//使能I2C模块
	REG_SET_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_IICEN_MASK);
}

I2CResult i2c_master_send(uint8 mod, uint8 addr, uint8* data, uint32 len) {
	I2C_Type * i2c_ptr;	//I2C基地址
	I2CResult result;	//发送结果
	bool enable_int;	//是否使能接收中断

	//获取I2C基地址
	i2c_ptr = i2c_table[mod];

	//判断是否使能接收中断
	if (REG_GET_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_IICIE_MASK)) {
		//使能接收中断
		enable_int = true;
		//暂时关闭接收中断
		REG_CLR_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_IICIE_MASK);
	} else {
		//未使能接收中断
		enable_int = false;
	}

	//发送开始信号，并转换为发送模式
	REG_SET_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_MST_MASK|I2C_C1_TX_MASK);
	//等待开始信号生效
	while (!REG_GET_MASK(I2C_S_REG(i2c_ptr), I2C_S_BUSY_MASK)) {
	}

	//发送7位地址，且主机为写模式
	REG_SET_VAL(I2C_D_REG(i2c_ptr), I2C_D_DATA(addr<<1));
	//等待发送结果
	result = i2c_wait_ack(i2c_ptr);
	//若发送失败
	if (result != I2CSuccess) {
		//停止发送数据
		len = 0;
	}

	//发送所有数据
	while (len--) {
		//发送数据
		REG_SET_VAL(I2C_D_REG(i2c_ptr), I2C_D_DATA(*data++));
		//等待发送结果
		result = i2c_wait_ack(i2c_ptr);
		//若发送失败
		if (result != I2CSuccess) {
			//停止发送数据
			break;
		}
	}

	//发送停止信号，并转换为接收模式
	REG_CLR_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_MST_MASK|I2C_C1_TX_MASK);
	//恢复接收中断
	if (enable_int) {
		REG_SET_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_IICIE_MASK);
	}
	if (result == I2CSuccess || result == I2CNAck) {
		//等待停止信号生效
		while (REG_GET_MASK(I2C_S_REG(i2c_ptr), I2C_S_BUSY_MASK)) {
		}
	}
	//返回发送结果
	return result;
}

void i2c_slave_enable_int(uint8 mod) {
	REG_SET_MASK(I2C_C1_REG(i2c_table[mod]), I2C_C1_IICIE_MASK);
	//允许接收中断
	ENABLE_IRQ(i2c_irq_table[mod]);
}

void i2c_slave_disable_int(uint8 mod) {
	REG_CLR_MASK(I2C_C1_REG(i2c_table[mod]), I2C_C1_IICIE_MASK);
	//允许接收中断
	DISABLE_IRQ(i2c_irq_table[mod]);
}

//从机地址匹配时(包括广播地址)一定发送应答信号，匹配到广播地址后，接收的数据一定发送应答信号，更改应答信号类型后，下一次发生应答信号请求时生效
void i2c_slave_set_ack(uint8 mod, bool ack) {
	if (ack) {
		REG_CLR_MASK(I2C_C1_REG(i2c_table[mod]), I2C_C1_TXAK_MASK);
	} else {
		REG_SET_MASK(I2C_C1_REG(i2c_table[mod]), I2C_C1_TXAK_MASK);
	}
}

//从机中断发生时，应答信号已经按照设置发送出去，但未读/写数据寄存器时，总线会被从机占据，主机会在下一次传输数据时等待从机释放总线
I2CSlaveIntType i2c_slave_handle_int(uint8 mod) {
	I2C_Type * i2c_ptr;	//I2C基地址
	uint8 status;
	I2CSlaveIntType int_type;

	//获取I2C基地址
	i2c_ptr = i2c_table[mod];
	status = I2C_S_REG(i2c_ptr);

//	uart_printf(1, "从机状态:%X\r\n", status);
	if (!REG_GET_MASK(status, I2C_S_IICIF_MASK)) {
		int_type = I2CSlaveNoInt;
	} else if (REG_GET_MASK(status, I2C_S_ARBL_MASK)) {
		int_type = I2CSlaveArbitrationLostInt;
	} else if (REG_GET_MASK(status, I2C_S_IAAS_MASK)) {
//		REG_CLR_MASK(status, I2C_S_IAAS_MASK);
		//写C1寄存器可以清除IAAS标志
		if (REG_GET_MASK(status, I2C_S_SRW_MASK)) {
			int_type = I2CSlaveCalledSendInt;
			REG_SET_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_TX_MASK);
		} else {
			REG_CLR_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_TX_MASK);
			if (i2c_slave_re_data(mod) == 0) {
				int_type = I2CSlaveGeneralCalledInt;
			} else {
				int_type = I2CSlaveCalledReInt;
			}
		}
	} else {
		if (REG_GET_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_TX_MASK)) {
			int_type = I2CSlaveSendDataInt;
		} else {
			int_type = I2CSlaveReDataInt;
		}
	}

	REG_SET_VAL(I2C_S_REG(i2c_ptr), status);
	return int_type;
}

uint8 i2c_slave_re_data(uint8 mod) {
	return I2C_D_REG(i2c_table[mod]);
}
