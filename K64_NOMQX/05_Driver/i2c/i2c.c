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

static I2CMasterResult i2c_master_send_byte(I2C_Type* i2c_ptr, uint8 byte,
		uint8 old_c1) {
	uint8 status;	//状态寄存器的值
	uint32 timeout;	//响应超时数值
	I2CMasterResult error_type;	//发送错误类型

	//发送一字节
	REG_SET_VAL(I2C_D_REG(i2c_ptr), I2C_D_DATA(byte));

	//等待从机响应，包括主机发送数据时间和从机响应时间，
	//若为首次发送，还会包括等待开始信号生效时间和开始信号保持时间
	timeout = 0;
	do {
		//若从机响应超时
		if (timeout++ >= I2C_TIMEOUT_MAX) {
			//跳转至错误处理
			error_type = I2CMasterTimeoutSlave;
			goto error_end;
		}
		//获取状态寄存器的值
		status = I2C_S_REG(i2c_ptr);
	} while (!REG_GET_MASK(status, I2C_S_IICIF_MASK));

	//清除中断标志
	REG_SET_VAL(I2C_S_REG(i2c_ptr), status);

	//若发生仲裁丢失
	if (REG_GET_MASK(status, I2C_S_ARBL_MASK)) {
		//跳转至错误处理
		error_type = I2CMasterArbitrationLost;
		goto error_end;
	}

	//若从机未响应
	if (REG_GET_MASK(status, I2C_S_RXAK_MASK)) {
		//跳转至错误处理，未响应时需要发送停止信号
		error_type = I2CMasterNAck;
		goto error_stop;
	}

	//返回发送成功
	return I2CMasterSuccess;

	error_stop:
	//发送停止信号
	REG_CLR_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_MST_MASK);
	//重置timeout
	timeout = 0;
	//等待停止信号生效
	while (REG_GET_MASK(I2C_S_REG(i2c_ptr), I2C_S_BUSY_MASK)) {
		//停止信号响应超时，即从机未响应主机最后一个字节数据
		if (++timeout >= I2C_TIMEOUT_MAX) {
			error_type = I2CMasterTimeoutStop;
			break;
		}
	}

	error_end:
	//恢复旧控制寄存器C1的值
	REG_SET_VAL(I2C_C1_REG(i2c_ptr), old_c1);
	//返回发送错误类型
	return error_type;
}

static I2CMasterResult i2c_master_re_byte(I2C_Type* i2c_ptr, uint8* byte,
		uint8 old_c1) {
	uint8 status;	//状态寄存器的值
	uint32 timeout;	//响应超时数值
	I2CMasterResult error_type;	//接收错误类型

	//接收一字节
	*byte = I2C_D_REG(i2c_ptr);

	//等待从机响应，包括从机响应时间和从机发送数据时间
	timeout = 0;
	do {
		//若从机响应超时
		if (timeout++ >= I2C_TIMEOUT_MAX) {
			//跳转至错误处理
			error_type = I2CMasterTimeoutSlave;
			goto error_end;
		}
		//获取状态寄存器的值
		status = I2C_S_REG(i2c_ptr);
	} while (!REG_GET_MASK(status, I2C_S_IICIF_MASK));

	//清除中断标志
	REG_SET_VAL(I2C_S_REG(i2c_ptr), status);

	//若发生仲裁丢失
	if (REG_GET_MASK(status, I2C_S_ARBL_MASK)) {
		//跳转至错误处理
		error_type = I2CMasterArbitrationLost;
		goto error_end;
	}

	//返回接收成功
	return I2CMasterSuccess;

	error_end:
	//恢复旧控制寄存器C1的值
	REG_SET_VAL(I2C_C1_REG(i2c_ptr), old_c1);
	//返回发送错误类型
	return error_type;
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

I2CMasterResult i2c_master_send(uint8 mod, uint8 addr_mode, uint16 addr,
		uint8* data, uint32 len) {
	I2C_Type * i2c_ptr;		//I2C基地址
	I2CMasterResult result;	//发送结果
	uint8 old_c1;			//旧控制寄存器C1的值
	uint32 timeout;			//停止信号响应超时数值

	//获取I2C基地址
	i2c_ptr = i2c_table[mod];
	//获取旧控制寄存器C1的值
	old_c1 = I2C_C1_REG(i2c_ptr);

	//若该I2C模块已经被设置为主机，返回正在工作
	if (REG_GET_MASK(old_c1, I2C_C1_MST_MASK)) {
		return I2CMasterIsBusy;
	}

	//暂时关闭中断
	REG_CLR_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_IICIE_MASK);

	//发送开始信号，并转换为发送模式
	REG_SET_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_MST_MASK|I2C_C1_TX_MASK);

	//判断地址模式
	if (addr_mode == I2C_ADDR_MODE_BITS_10) {
		//按照标准发送10位地址的高2位，且主机为写模式
		result = i2c_master_send_byte(i2c_ptr, 0xF0 | ((addr & 0x300) >> 7),
				old_c1);
		//失败时，返回发送结果
		if (result != I2CMasterSuccess) {
			return result;
		}
		//发送10位地址的低8位
		result = i2c_master_send_byte(i2c_ptr, addr, old_c1);
	} else {
		//发送7位地址，且主机为写模式
		result = i2c_master_send_byte(i2c_ptr, addr << 1, old_c1);
	}
	//失败时，返回发送结果
	if (result != I2CMasterSuccess) {
		return result;
	}

	//发送所有数据
	while (len--) {
		result = i2c_master_send_byte(i2c_ptr, *data++, old_c1);
		//失败时，返回发送结果
		if (result != I2CMasterSuccess) {
			return result;
		}
	}

	//发送停止信号
	REG_CLR_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_MST_MASK);
	//初始化timeout
	timeout = 0;
	//等待停止信号生效
	while (REG_GET_MASK(I2C_S_REG(i2c_ptr), I2C_S_BUSY_MASK)) {
		//停止信号响应超时，即从机未响应主机最后一个字节数据
		if (++timeout >= I2C_TIMEOUT_MAX) {
			result = I2CMasterTimeoutStop;
			break;
		}
	}

	//恢复旧控制寄存器C1的值
	REG_SET_VAL(I2C_C1_REG(i2c_ptr), old_c1);
	//返回发送结果
	return result;
}

I2CMasterResult i2c_master_send_re(uint8 mod, uint8 addr_mode, uint16 addr,
		uint8* send_data, uint32 send_len, uint8* re_data, uint32 re_len) {
	I2C_Type * i2c_ptr;		//I2C基地址
	I2CMasterResult result;	//发送接收结果
	uint8 old_c1;			//旧控制寄存器C1的值
	uint32 timeout;			//停止信号响应超时数值
	uint8 dummy;			//无效读数据

	//获取I2C基地址
	i2c_ptr = i2c_table[mod];
	//获取旧控制寄存器C1的值
	old_c1 = I2C_C1_REG(i2c_ptr);

	//若该I2C模块已经被设置为主机，返回正在工作
	if (REG_GET_MASK(old_c1, I2C_C1_MST_MASK)) {
		return I2CMasterIsBusy;
	}

	//暂时关闭中断
	REG_CLR_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_IICIE_MASK);

	//发送开始信号，并转换为发送模式
	REG_SET_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_MST_MASK|I2C_C1_TX_MASK);

	//判断地址模式
	if (addr_mode == I2C_ADDR_MODE_BITS_10) {
		//按照标准发送10位地址的高2位，且主机为写模式
		result = i2c_master_send_byte(i2c_ptr, 0xF0 | ((addr & 0x300) >> 7),
				old_c1);
		//失败时，返回结果
		if (result != I2CMasterSuccess) {
			return result;
		}
		//发送10位地址的低8位
		result = i2c_master_send_byte(i2c_ptr, addr, old_c1);
	} else {
		//发送7位地址，且主机为写模式
		result = i2c_master_send_byte(i2c_ptr, addr << 1, old_c1);
	}
	//失败时，返回结果
	if (result != I2CMasterSuccess) {
		return result;
	}

	//发送所有数据
	while (send_len--) {
		result = i2c_master_send_byte(i2c_ptr, *send_data++, old_c1);
		//失败时，返回结果
		if (result != I2CMasterSuccess) {
			return result;
		}
	}

	//发送重新开始信号
	REG_SET_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_RSTA_MASK);

	//判断地址模式
	if (addr_mode == I2C_ADDR_MODE_BITS_10) {
		//按照标准发送10位地址的高2位，且主机为读模式
		result = i2c_master_send_byte(i2c_ptr, 0xF1 | ((addr & 0x300) >> 7),
				old_c1);
	} else {
		//发送7位地址，且主机为读模式
		result = i2c_master_send_byte(i2c_ptr, (addr << 1) | 1, old_c1);
	}
	//失败时，返回结果
	if (result != I2CMasterSuccess) {
		return result;
	}

	//设置为发送应答信号，并转换为接收模式
	REG_CLR_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_TXAK_MASK|I2C_C1_TX_MASK);

	//接收数据量大于等于2时
	if (re_len >= 2) {
		//接收1字节无效数据
		result = i2c_master_re_byte(i2c_ptr, &dummy, old_c1);
		//失败时，返回结果
		if (result != I2CMasterSuccess) {
			return result;
		}
		//接收数据至剩余2字节数据未接收
		for (; re_len > 2; --re_len) {
			result = i2c_master_re_byte(i2c_ptr, re_data++, old_c1);
			//失败时，返回结果
			if (result != I2CMasterSuccess) {
				return result;
			}
		}
	}

	//设置为发送非应答信号
	REG_SET_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_TXAK_MASK);

	if (re_len <= 1) {
		//若接收数据量小于等于1，接收1字节无效数据
		result = i2c_master_re_byte(i2c_ptr, &dummy, old_c1);
	} else {
		//否则接收倒数第2个字节
		result = i2c_master_re_byte(i2c_ptr, re_data++, old_c1);
	}
	//失败时，返回结果
	if (result != I2CMasterSuccess) {
		return result;
	}

	//发送停止信号
	REG_CLR_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_MST_MASK);

	//读取最后一个字节数据
	*re_data = I2C_D_REG(i2c_ptr);

	//初始化timeout
	timeout = 0;
	//等待停止信号生效
	while (REG_GET_MASK(I2C_S_REG(i2c_ptr), I2C_S_BUSY_MASK)) {
		//停止信号响应超时，即从机未响应主机非应答信号
		if (++timeout >= I2C_TIMEOUT_MAX) {
			result = I2CMasterTimeoutStop;
			break;
		}
	}

	//恢复旧控制寄存器C1的值
	REG_SET_VAL(I2C_C1_REG(i2c_ptr), old_c1);
	//返回结果
	return result;
}

I2CMasterResult i2c_master_re(uint8 mod, uint8 addr_mode, uint16 addr,
		uint8* data, uint32 len) {
	I2C_Type * i2c_ptr;		//I2C基地址
	I2CMasterResult result;	//接收结果
	uint8 old_c1;			//旧控制寄存器C1的值
	uint32 timeout;			//停止信号响应超时数值
	uint8 dummy;			//无效读数据

	//获取I2C基地址
	i2c_ptr = i2c_table[mod];
	//获取旧控制寄存器C1的值
	old_c1 = I2C_C1_REG(i2c_ptr);

	//若该I2C模块已经被设置为主机，返回正在工作
	if (REG_GET_MASK(old_c1, I2C_C1_MST_MASK)) {
		return I2CMasterIsBusy;
	}

	//暂时关闭中断
	REG_CLR_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_IICIE_MASK);

	//发送开始信号，并转换为发送模式
	REG_SET_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_MST_MASK|I2C_C1_TX_MASK);

	//判断地址模式
	if (addr_mode == I2C_ADDR_MODE_BITS_10) {
		//按照标准发送10位地址的高2位，且主机为写模式
		result = i2c_master_send_byte(i2c_ptr, 0xF0 | ((addr & 0x300) >> 7),
				old_c1);
		//失败时，返回结果
		if (result != I2CMasterSuccess) {
			return result;
		}
		//发送10位地址的低8位
		result = i2c_master_send_byte(i2c_ptr, addr, old_c1);
		//失败时，返回结果
		if (result != I2CMasterSuccess) {
			return result;
		}

		//发送重新开始信号
		REG_SET_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_RSTA_MASK);

		//按照标准发送10位地址的高2位，且主机为读模式
		result = i2c_master_send_byte(i2c_ptr, 0xF1 | ((addr & 0x300) >> 7),
				old_c1);
	} else {
		//发送7位地址，且主机为读模式
		result = i2c_master_send_byte(i2c_ptr, (addr << 1) | 1, old_c1);
	}
	//失败时，返回结果
	if (result != I2CMasterSuccess) {
		return result;
	}

	//设置为发送应答信号，并转换为接收模式
	REG_CLR_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_TXAK_MASK|I2C_C1_TX_MASK);

	//接收数据量大于等于2时
	if (len >= 2) {
		//接收1字节无效数据
		result = i2c_master_re_byte(i2c_ptr, &dummy, old_c1);
		//失败时，返回结果
		if (result != I2CMasterSuccess) {
			return result;
		}
		//接收数据至剩余2字节数据未接收
		for (; len > 2; --len) {
			result = i2c_master_re_byte(i2c_ptr, data++, old_c1);
			//失败时，返回结果
			if (result != I2CMasterSuccess) {
				return result;
			}
		}
	}

	//设置为发送非应答信号
	REG_SET_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_TXAK_MASK);

	if (len <= 1) {
		//若接收数据量小于等于1，接收1字节无效数据
		result = i2c_master_re_byte(i2c_ptr, &dummy, old_c1);
	} else {
		//否则接收倒数第2个字节
		result = i2c_master_re_byte(i2c_ptr, data++, old_c1);
	}
	//失败时，返回结果
	if (result != I2CMasterSuccess) {
		return result;
	}

	//发送停止信号
	REG_CLR_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_MST_MASK);

	//读取最后一个字节数据
	*data = I2C_D_REG(i2c_ptr);

	//初始化timeout
	timeout = 0;
	//等待停止信号生效
	while (REG_GET_MASK(I2C_S_REG(i2c_ptr), I2C_S_BUSY_MASK)) {
		//停止信号响应超时，即从机未响应主机非应答信号
		if (++timeout >= I2C_TIMEOUT_MAX) {
			result = I2CMasterTimeoutStop;
			break;
		}
	}

	//恢复旧控制寄存器C1的值
	REG_SET_VAL(I2C_C1_REG(i2c_ptr), old_c1);
	//返回结果
	return result;
}

void i2c_slave_enable_int(uint8 mod) {
	//使能从机中断
	REG_SET_MASK(I2C_C1_REG(i2c_table[mod]), I2C_C1_IICIE_MASK);
	//允许接收中断
	ENABLE_IRQ(i2c_irq_table[mod]);
}

void i2c_slave_disable_int(uint8 mod) {
	//关闭从机中断
	REG_CLR_MASK(I2C_C1_REG(i2c_table[mod]), I2C_C1_IICIE_MASK);
	//禁止接收中断
	DISABLE_IRQ(i2c_irq_table[mod]);
}

//从机地址匹配时(包括广播地址)一定发送应答信号，匹配到广播地址后，接收的数据一定发送应答信号，更改应答信号类型后，下一次发生应答信号请求时生效
void i2c_slave_set_ack(uint8 mod, bool ack) {
	if (ack) {
		//发送应答信号
		REG_CLR_MASK(I2C_C1_REG(i2c_table[mod]), I2C_C1_TXAK_MASK);
	} else {
		//发送非应答信号
		REG_SET_MASK(I2C_C1_REG(i2c_table[mod]), I2C_C1_TXAK_MASK);
	}
}

//从机中断发生时，应答信号已经按照设置发送出去，但未读/写数据寄存器时，总线会被从机占据，主机会在下一次传输数据时等待从机释放总线
I2CSlaveIntType i2c_slave_handle_int(uint8 mod) {
	I2C_Type * i2c_ptr;	//I2C基地址
	uint8 status;		//状态寄存器的值
	volatile uint8 dummy;	//无效读数据

	//获取I2C基地址
	i2c_ptr = i2c_table[mod];
	//获取状态寄存器的值
	status = I2C_S_REG(i2c_ptr);

	//查看中断标志是否置位
	if (!REG_GET_MASK(status, I2C_S_IICIF_MASK)) {
		//返回无中断发送
		return I2CSlaveNoInt;
	}

	//清除中断标志
	REG_SET_VAL(I2C_S_REG(i2c_ptr), status);

	//查看是否被寻址为从机
	if (REG_GET_MASK(status, I2C_S_IAAS_MASK)) {
		//写C1寄存器可以清除IAAS标志
		//被寻址为从机时，查看主机发送方向
		if (REG_GET_MASK(status, I2C_S_SRW_MASK)) {
			//主机读取数据时，从机转换为发送模式
			REG_SET_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_TX_MASK);
			//返回被寻址发送中断
			return I2CSlaveCalledSendInt;
		} else {
			//主机发送数据时，从机转换为接收模式
			REG_CLR_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_TX_MASK);
			//若接收的地址为0x00，且从机为7位地址模式
			if (I2C_D_REG(i2c_table[mod])
					== 0&& !REG_GET_MASK(I2C_C2_REG(i2c_ptr),I2C_C2_ADEXT_MASK)) {
				//返回被广播寻址中断
				return I2CSlaveCalledGeneralInt;
			} else {
				//否则返回被寻址接收中断
				return I2CSlaveCalledReInt;
			}
		}
	}

	//查看是否仲裁丢失
	if (REG_GET_MASK(status, I2C_S_ARBL_MASK)) {
		//返回仲裁丢失中断
		return I2CSlaveArbitrationLostInt;
	}

	//查看从机数据方向
	if (REG_GET_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_TX_MASK)) {
		//若为发送模式，查看是否接收到非应答信号
		if (REG_GET_MASK(status, I2C_S_RXAK_MASK)) {
			//接收到非应答信号时，转换为接收模式
			REG_CLR_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_TX_MASK);
			//读取一次数据寄存器
			dummy = I2C_D_REG(i2c_table[mod]);
			//返回数据发送完毕中断
			return I2CSlaveDataSendOverInt;
		} else {
			//接收到应答信号时，返回数据发送中断
			return I2CSlaveDataSendInt;
		}
	} else {
		//若为接收模式，返回数据接收中断
		return I2CSlaveDataReInt;
	}
}

uint8 i2c_slave_re(uint8 mod) {
	return I2C_D_REG(i2c_table[mod]);
}

void i2c_slave_send(uint8 mod, uint8 data) {
	REG_SET_VAL(I2C_D_REG(i2c_table[mod]), I2C_D_DATA(data));
}
