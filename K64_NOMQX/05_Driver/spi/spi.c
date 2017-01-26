//==========================================================================
//文件名称：spi.c
//功能概要：K64 SPI底层驱动程序源文件
//==========================================================================

#include "spi.h"

//SPI各模块基地址
static SPI_Type * const spi_table[] = SPI_BASE_PTRS;
//SPI各模块时钟信号的引脚号
static const uint8 spi_sck_pin_table[] = { SPI_MOD0_SCK_PIN, SPI_MOD1_SCK_PIN,
SPI_MOD2_SCK_PIN };
//SPI各模块时钟信号的PCR的MUX值
static const uint8 spi_sck_pcr_mux_table[] = { SPI_MOD0_SCK_PCR_MUX,
SPI_MOD1_SCK_PCR_MUX, SPI_MOD2_SCK_PCR_MUX };
//SPI各模块输出信号的引脚号
static const uint8 spi_sout_pin_table[] = { SPI_MOD0_SOUT_PIN,
SPI_MOD1_SOUT_PIN, SPI_MOD2_SOUT_PIN };
//SPI各模块输出信号的PCR的MUX值
static const uint8 spi_sout_pcr_mux_table[] = { SPI_MOD0_SOUT_PCR_MUX,
SPI_MOD1_SOUT_PCR_MUX, SPI_MOD2_SOUT_PCR_MUX };
//SPI各模块输入信号的引脚号
static const uint8 spi_sin_pin_table[] = { SPI_MOD0_SIN_PIN, SPI_MOD1_SIN_PIN,
SPI_MOD2_SIN_PIN };
//SPI各模块输入信号的PCR的MUX值
static const uint8 spi_sin_pcr_mux_table[] = { SPI_MOD0_SIN_PCR_MUX,
SPI_MOD1_SIN_PCR_MUX, SPI_MOD2_SIN_PCR_MUX };
//SPI各模块片选信号的引脚号
static const uint8 spi_cs_pin_table[][6] = { { SPI_MOD0_CS0_PIN,
SPI_MOD0_CS1_PIN, SPI_MOD0_CS2_PIN, SPI_MOD0_CS3_PIN, SPI_MOD0_CS4_PIN,
SPI_MOD0_CS5_PIN }, { SPI_MOD1_CS0_PIN, SPI_MOD1_CS1_PIN,
SPI_MOD1_CS2_PIN, SPI_MOD1_CS3_PIN }, { SPI_MOD2_CS0_PIN,
SPI_MOD2_CS1_PIN } };
//SPI各模块片选信号的PCR的MUX值
static const uint8 spi_cs_pcr_mux_table[][6] = { { SPI_MOD0_CS0_PCR_MUX,
SPI_MOD0_CS1_PCR_MUX, SPI_MOD0_CS2_PCR_MUX, SPI_MOD0_CS3_PCR_MUX,
SPI_MOD0_CS4_PCR_MUX, SPI_MOD0_CS5_PCR_MUX }, { SPI_MOD1_CS0_PCR_MUX,
SPI_MOD1_CS1_PCR_MUX, SPI_MOD1_CS2_PCR_MUX, SPI_MOD1_CS3_PCR_MUX }, {
SPI_MOD2_CS0_PCR_MUX, SPI_MOD2_CS1_PCR_MUX } };
//主机模式下是否启用SPI各模块的片选信号
static const bool spi_cs_enable_table[][6] = { { SPI_MOD0_CS0_ENABLE,
SPI_MOD0_CS1_ENABLE, SPI_MOD0_CS2_ENABLE, SPI_MOD0_CS3_ENABLE,
SPI_MOD0_CS4_ENABLE, SPI_MOD0_CS5_ENABLE }, { SPI_MOD1_CS0_ENABLE,
SPI_MOD1_CS1_ENABLE, SPI_MOD1_CS2_ENABLE, SPI_MOD1_CS3_ENABLE,
SPI_MOD1_CS4_ENABLE, SPI_MOD1_CS5_ENABLE }, { SPI_MOD2_CS0_ENABLE,
SPI_MOD2_CS1_ENABLE, SPI_MOD2_CS2_ENABLE, SPI_MOD2_CS3_ENABLE,
SPI_MOD2_CS4_ENABLE, SPI_MOD2_CS5_ENABLE } };
//SPI各模块中断请求号
static const IRQn_Type spi_irq_table[] = { SPI0_IRQn, SPI1_IRQn, SPI2_IRQn };
//t CSC = (1/fP ) x PCSSCK x CSSCK
//t ASC = (1/fP) x PASC x ASC
//tDT = (1/fP ) x PDT x DT
// (fP /PBR) x [(1+DBR)/BR]
void spi_master_init(uint8 mod, uint8 config, uint32 baud_scaler, uint8 format,
		uint8 data_size, uint8 bit_order, uint8 csc_scaler, uint8 asc_scaler,
		uint8 dt_scaler) {
	SPI_Type * spi_ptr;	//SPI基地址
	uint8 i;	//游标

	//获取SPI基地址
	spi_ptr = spi_table[mod];

	//开启相应SPI模块时钟门
	switch (mod) {
	case SPI_MOD0:
		REG_SET_MASK(SIM_SCGC6, SIM_SCGC6_SPI0_MASK);
		break;
	case SPI_MOD1:
		REG_SET_MASK(SIM_SCGC6, SIM_SCGC6_SPI1_MASK);
		break;
	case SPI_MOD2:
		REG_SET_MASK(SIM_SCGC3, SIM_SCGC3_SPI2_MASK);
		break;
	}

	//暂时停止SPI传输
	REG_SET_MASK(SPI_MCR_REG(spi_ptr), SPI_MCR_HALT_MASK);

	//使能时钟信号引脚功能
	com_port_pin_set_mux(spi_sck_pin_table[mod], spi_sck_pcr_mux_table[mod]);
	//使能输出信号引脚功能
	com_port_pin_set_mux(spi_sout_pin_table[mod], spi_sout_pcr_mux_table[mod]);
	//使能输入信号引脚功能
	com_port_pin_set_mux(spi_sin_pin_table[mod], spi_sin_pcr_mux_table[mod]);
	//根据启用配置使能片选信号引脚功能
	for (i = SPI_CS0; i <= SPI_CS5; i++) {
		//若使能该片选信号，则使能引脚功能
		if (spi_cs_enable_table[mod][i]) {
			com_port_pin_set_mux(spi_cs_pin_table[mod][i],
					spi_cs_pcr_mux_table[mod][i]);
		}
	}

	//使能SPI时钟
	REG_CLR_MASK(SPI_MCR_REG(spi_ptr), SPI_MCR_MDIS_MASK);

	REG_SET_MASK(SPI_MCR_REG(spi_ptr), SPI_MCR_MSTR_MASK	//设置SPI模块为主机模式
			|SPI_MCR_PCSIS_MASK//设置各片选信号有效电平为低电平
			|SPI_MCR_DIS_TXF_MASK//关闭FIFO发送队列
			|SPI_MCR_DIS_RXF_MASK//关闭FIFO接收队列
			|SPI_MCR_CLR_TXF_MASK//清除FIFO发送计数器
			|SPI_MCR_CLR_RXF_MASK);//清除FIFO接收计数器

	//直接设置CTAR寄存器的值，节约了IO操作
	REG_SET_VAL(SPI_CTAR_REG(spi_ptr,config), baud_scaler//设置波特率分频因子
			|SPI_CTAR_FMSZ(data_size-1)//设置帧数据位数
			|SPI_CTAR_PCSSCK(csc_scaler>>4)//设置CSC预分频因子
			|SPI_CTAR_CSSCK(csc_scaler)//设置CSC分频因子
			|SPI_CTAR_PASC(asc_scaler>>4)//设置ASC预分频因子
			|SPI_CTAR_ASC(asc_scaler)//设置ASC分频因子
			|SPI_CTAR_PDT(dt_scaler>>4)//设置DT预分频因子
			|SPI_CTAR_DT(dt_scaler));//设置DT分频因子

	//设置帧格式，由于CTAR其他位的值全部置0，仅需置位即可
	switch (format) {
	case SPI_FORMAT_CPOL0_CPHA0:
		break;
	case SPI_FORMAT_CPOL0_CPHA1:
		REG_SET_MASK(SPI_CTAR_REG(spi_ptr,config), SPI_CTAR_CPHA_MASK);
		break;
	case SPI_FORMAT_CPOL1_CPHA0:
		REG_SET_MASK(SPI_CTAR_REG(spi_ptr,config), SPI_CTAR_CPOL_MASK);
		break;
	case SPI_FORMAT_CPOL1_CPHA1:
		REG_SET_MASK(SPI_CTAR_REG(spi_ptr,config),
				SPI_CTAR_CPOL_MASK|SPI_CTAR_CPHA_MASK);
		break;
	}

	//设置位传输顺序
	if (bit_order == SPI_BIT_ORDER_LSB) {
		REG_SET_MASK(SPI_CTAR_REG(spi_ptr,config), SPI_CTAR_LSBFE_MASK);
	}

	//清除所有状态标识
	REG_SET_VAL(SPI_SR_REG(spi_ptr), 0xFFFFFFFF);

	//开始传输
	REG_CLR_MASK(SPI_MCR_REG(spi_ptr), SPI_MCR_HALT_MASK);
}

uint16 spi_master_send(uint8 mod, uint8 config, uint8 cs, uint16 data,
		uint8 cont) {
	SPI_Type * spi_ptr;	//SPI基地址

	//获取SPI基地址
	spi_ptr = spi_table[mod];

	REG_SET_VAL(SPI_PUSHR_REG(spi_ptr), SPI_PUSHR_CTAS(config)//设置所用CTAR寄存器，即所用配置
			|SPI_PUSHR_PCS(1<<cs)//设置有效片选信号
			|SPI_PUSHR_TXDATA(data)//设置发送数据
			|SPI_PUSHR_CONT(cont));//设置是否使能连续发送
	//等待传输完成
	while (!REG_GET_MASK(SPI_SR_REG(spi_ptr), SPI_SR_TCF_MASK)) {
	}
	//清除传输完成标志
	REG_SET_MASK(SPI_SR_REG(spi_ptr), SPI_SR_TCF_MASK);
//	while(!REG_GET_MASK(SPI_SR_REG(spi_ptr), SPI_SR_RFDF_MASK));
//	REG_SET_MASK(SPI_SR_REG(spi_ptr), SPI_SR_RFDF_MASK);
	//返回接收到的数据
	return SPI_POPR_REG(spi_ptr);
}

//从机仅支持MSB，format、data_size要和主机一致
void spi_slave_init(uint8 mod, uint8 format, uint8 data_size) {
	SPI_Type * spi_ptr;	//SPI基地址

	//获取SPI基地址
	spi_ptr = spi_table[mod];

	//开启相应SPI模块时钟门
	switch (mod) {
	case SPI_MOD0:
		REG_SET_MASK(SIM_SCGC6, SIM_SCGC6_SPI0_MASK);
		break;
	case SPI_MOD1:
		REG_SET_MASK(SIM_SCGC6, SIM_SCGC6_SPI1_MASK);
		break;
	case SPI_MOD2:
		REG_SET_MASK(SIM_SCGC3, SIM_SCGC3_SPI2_MASK);
		break;
	}

	//暂时停止SPI传输
	REG_SET_MASK(SPI_MCR_REG(spi_ptr), SPI_MCR_HALT_MASK);

	//使能时钟信号引脚功能
	com_port_pin_set_mux(spi_sck_pin_table[mod], spi_sck_pcr_mux_table[mod]);
	//使能输出信号引脚功能
	com_port_pin_set_mux(spi_sout_pin_table[mod], spi_sout_pcr_mux_table[mod]);
	//使能输入信号引脚功能
	com_port_pin_set_mux(spi_sin_pin_table[mod], spi_sin_pcr_mux_table[mod]);
	//使能片选信号0引脚功能
	com_port_pin_set_mux(spi_cs_pin_table[mod][0],
			spi_cs_pcr_mux_table[mod][0]);

	//使能SPI时钟
	REG_CLR_MASK(SPI_MCR_REG(spi_ptr), SPI_MCR_MDIS_MASK);

	REG_CLR_MASK(SPI_MCR_REG(spi_ptr), SPI_MCR_MSTR_MASK	//设置SPI模块为从机模式
			|SPI_MCR_DIS_TXF_MASK//开启FIFO发送队列
			|SPI_MCR_DIS_RXF_MASK//开启FIFO接收队列
			|SPI_MCR_CLR_TXF_MASK//不清除FIFO发送计数器
			|SPI_MCR_CLR_RXF_MASK);//不清除FIFO接收计数器

	REG_SET_MASK(SPI_MCR_REG(spi_ptr), SPI_MCR_PCSIS_MASK);	//设置各片选信号有效电平为低电平
//			|SPI_MCR_DIS_TXF_MASK//关闭FIFO发送队列
//			|SPI_MCR_DIS_RXF_MASK//关闭FIFO接收队列
//			|SPI_MCR_CLR_TXF_MASK//清除FIFO发送计数器
//			|SPI_MCR_CLR_RXF_MASK);//清除FIFO接收计数器

	//设置帧数据位数，其他位置0
	REG_SET_VAL(SPI_CTAR_SLAVE_REG(spi_ptr,0),
			SPI_CTAR_SLAVE_FMSZ(data_size-1));

	//设置帧格式，由于CTAR其他位的值全部置0，仅需置位即可
	switch (format) {
	case SPI_FORMAT_CPOL0_CPHA0:
		break;
	case SPI_FORMAT_CPOL0_CPHA1:
		REG_SET_MASK(SPI_CTAR_SLAVE_REG(spi_ptr,0), SPI_CTAR_SLAVE_CPHA_MASK);
		break;
	case SPI_FORMAT_CPOL1_CPHA0:
		REG_SET_MASK(SPI_CTAR_SLAVE_REG(spi_ptr,0), SPI_CTAR_SLAVE_CPOL_MASK);
		break;
	case SPI_FORMAT_CPOL1_CPHA1:
		REG_SET_MASK(SPI_CTAR_SLAVE_REG(spi_ptr,0),
				SPI_CTAR_SLAVE_CPOL_MASK|SPI_CTAR_SLAVE_CPHA_MASK);
		break;
	}

	//清除所有状态标识
	REG_SET_VAL(SPI_SR_REG(spi_ptr), 0xFFFFFFFF);

	//开始传输
	REG_CLR_MASK(SPI_MCR_REG(spi_ptr), SPI_MCR_HALT_MASK);
}

bool spi_slave_send(uint8 mod, uint32 data) {
	SPI_Type * spi_ptr;	//SPI基地址

	//获取SPI基地址
	spi_ptr = spi_table[mod];

	if (REG_GET_MASK(SPI_SR_REG(spi_ptr), SPI_SR_TFFF_MASK)) {
		REG_SET_VAL(SPI_PUSHR_SLAVE_REG(spi_ptr), data);
		REG_SET_MASK(SPI_SR_REG(spi_ptr), SPI_SR_TFFF_MASK);
		return true;
	} else {
		return false;
	}
}

bool spi_slave_re(uint8 mod, uint32* data) {
	SPI_Type * spi_ptr;	//SPI基地址

	//获取SPI基地址
	spi_ptr = spi_table[mod];

	if (REG_GET_MASK(SPI_SR_REG(spi_ptr), SPI_SR_RFDF_MASK)) {
		*data = SPI_POPR_REG(spi_ptr);
		REG_SET_MASK(SPI_SR_REG(spi_ptr), SPI_SR_RFDF_MASK);
		return true;
	} else {
		return false;
	}
}

void spi_slave_enable_re_int(uint8 mod) {
	REG_SET_MASK(SPI_RSER_REG(spi_table[mod]), SPI_RSER_RFDF_RE_MASK);
	ENABLE_IRQ(spi_irq_table[mod]);
}

void spi_slave_disable_re_int(uint8 mod) {
	REG_CLR_MASK(SPI_RSER_REG(spi_table[mod]), SPI_RSER_RFDF_RE_MASK);
	DISABLE_IRQ(spi_irq_table[mod]);
}
