//==========================================================================
//文件名称：i2c.c
//功能概要：K64 I2C底层驱动程序源文件
//==========================================================================

#include "i2c.h"

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

void i2c_send_addr_7bits(I2C_Type* i2c_ptr, uint8 addr, uint8 dir) {
	REG_SET_VAL(I2C_D_REG(i2c_ptr), I2C_D_DATA((addr<<1)|dir));
}

static void i2c_send_data(I2C_Type* i2c_ptr, uint8 data) {
	REG_SET_VAL(I2C_D_REG(i2c_ptr), I2C_D_DATA(data));
}

static uint8 i2c_re_data(I2C_Type* i2c_ptr) {
	return I2C_D_REG(i2c_ptr);
}

static I2CResult i2c_wait_ack(I2C_Type* i2c_ptr) {
	uint8 status;
	do {
		status = I2C_S_REG(i2c_ptr);
	} while (!REG_GET_MASK(status, I2C_S_IICIF_MASK));
	REG_SET_VAL(I2C_S_REG(i2c_ptr), status);
	if (REG_GET_MASK(status, I2C_S_ARBL_MASK)) {
		return I2CArbitrationLost;
	}
	if (REG_GET_MASK(status, I2C_S_RXAK_MASK)) {
		return I2CNAck;
	} else {
		return I2CSuccess;
	}
}

//icr:时钟速率
void i2c_init(uint8 mod, uint8 mul, uint8 icr, uint8 addr_mode, uint16 addr,
		bool enable_general_call_addr) {
	I2C_Type * i2c_ptr;	//I2C基地址

	//获取I2C基地址
	i2c_ptr = i2c_table[mod];

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

	//暂时禁用I2C模块
	REG_CLR_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_IICEN_MASK);

	//使能时钟信号引脚功能
	com_port_pin_set_mux(i2c_scl_pin_table[mod], i2c_scl_pcr_mux_table[mod]);
	//使能数据信号引脚功能
	com_port_pin_set_mux(i2c_sda_pin_table[mod], i2c_sda_pcr_mux_table[mod]);

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
	uint8 status;
	I2CResult result;

	//获取I2C基地址
	i2c_ptr = i2c_table[mod];

	//关闭中断

	//发送开始信号
	REG_SET_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_MST_MASK);
	//获取状态寄存器状态
	status = I2C_S_REG(i2c_ptr);
	//若发生仲裁丢失，发送失败
	if (REG_GET_MASK(status, I2C_S_ARBL_MASK)) {
		//清除仲裁丢失和中断标志
		REG_SET_VAL(I2C_S_REG(i2c_ptr), status);
		return I2CArbitrationLost;
	}
	//转换为发送模式
	REG_SET_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_TX_MASK);
	//发送7位地址，且主机为写模式
	REG_SET_VAL(I2C_D_REG(i2c_ptr), I2C_D_DATA(addr<<1));
	//等待发送结果
	result = i2c_wait_ack(i2c_ptr);
	//若发送失败
	if (result != I2CSuccess) {
		//发送停止信号，并转换为接收模式，若为仲裁丢失，则不会发送停止信号
		REG_CLR_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_MST_MASK|I2C_C1_TX_MASK);
		//等待总线忙状态停止
		while (REG_GET_MASK(I2C_S_REG(i2c_ptr), I2C_S_BUSY_MASK)) {
		}
		//返回发送结果
		return result;
	}
	//发送所有数据
	while (len--) {
		//发送数据
		REG_SET_VAL(I2C_D_REG(i2c_ptr), I2C_D_DATA(*data++));
		//等待发送结果
		result = i2c_wait_ack(i2c_ptr);
		//若发送失败
		if (result != I2CSuccess) {
			//发送停止信号，并转换为接收模式，若为仲裁丢失，则不会发送停止信号
			REG_CLR_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_MST_MASK|I2C_C1_TX_MASK);
			//等待总线忙状态停止
			while (REG_GET_MASK(I2C_S_REG(i2c_ptr), I2C_S_BUSY_MASK)) {
			}
			//返回发送结果
			return result;
		}
	}
	//发送停止信号，并转换为接收模式
	REG_CLR_MASK(I2C_C1_REG(i2c_ptr), I2C_C1_MST_MASK|I2C_C1_TX_MASK);
	//等待总线忙状态停止
	while (REG_GET_MASK(I2C_S_REG(i2c_ptr), I2C_S_BUSY_MASK)) {
	}
	//发送成功
	return I2CSuccess;
}
