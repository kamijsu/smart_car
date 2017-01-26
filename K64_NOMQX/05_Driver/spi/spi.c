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
void spi_init_master(uint8 mod, uint8 config, uint32 baud_scaler, uint8 format,
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

	//设置SPI模块为主机模式，片选信号有效电平为低电平，关闭FIFO队列，并清除FIFO计数器
	REG_SET_MASK(SPI_MCR_REG(spi_ptr),
			SPI_MCR_MSTR_MASK|SPI_MCR_PCSIS_MASK|SPI_MCR_DIS_TXF_MASK|SPI_MCR_DIS_RXF_MASK|SPI_MCR_CLR_TXF_MASK|SPI_MCR_CLR_RXF_MASK);

	//清空CTAR寄存器的值，节约了IO操作
	REG_SET_VAL(SPI_CTAR_REG(spi_ptr,config), 0);

	//设置帧格式，由于CTAR值全部置0，仅需置位即可
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

	//设置波特率分频因子，帧数据位数，各延时分频因子
	REG_SET_MASK(SPI_CTAR_REG(spi_ptr,config),
			baud_scaler |SPI_CTAR_FMSZ(data_size) |SPI_CTAR_PCSSCK(csc_scaler>>4) |SPI_CTAR_CSSCK(csc_scaler) |SPI_CTAR_PASC(asc_scaler>>4) |SPI_CTAR_ASC(asc_scaler) |SPI_CTAR_PDT(dt_scaler>>4) |SPI_CTAR_DT(dt_scaler));

	//设置位传输顺序
	if (bit_order == SPI_BIT_ORDER_LSB) {
		REG_SET_MASK(SPI_CTAR_REG(spi_ptr,config), SPI_CTAR_LSBFE_MASK);
	}

	//清除所有状态标识
	REG_SET_VAL(SPI_SR_REG(spi_ptr), 0xFFFFFFFF);

	//使能SPI时钟，并开始传输
	REG_CLR_MASK(SPI_MCR_REG(spi_ptr), SPI_MCR_MDIS_MASK|SPI_MCR_HALT_MASK);
}
