//==========================================================================
//文件名称：spi.c
//功能概要：K64 SPI底层驱动程序源文件
//==========================================================================

#include "spi.h"

//SPI各模块基地址
static SPI_Type * const spi_table[] = SPI_BASE_PTRS;
//SPI各模块时钟信号的引脚号
static const uint8 spi_sck_pin_table[] =
{ SPI_MOD0_SCK_PIN, SPI_MOD1_SCK_PIN, SPI_MOD2_SCK_PIN };
//SPI各模块时钟信号的PCR的MUX值
static const uint8 spi_sck_pcr_mux_table[] =
{ SPI_MOD0_SCK_PCR_MUX, SPI_MOD1_SCK_PCR_MUX, SPI_MOD2_SCK_PCR_MUX };
//SPI各模块输出信号的引脚号
static const uint8 spi_sout_pin_table[] =
{ SPI_MOD0_SOUT_PIN, SPI_MOD1_SOUT_PIN, SPI_MOD2_SOUT_PIN };
//SPI各模块输出信号的PCR的MUX值
static const uint8 spi_sout_pcr_mux_table[] =
{ SPI_MOD0_SOUT_PCR_MUX, SPI_MOD1_SOUT_PCR_MUX, SPI_MOD2_SOUT_PCR_MUX };
//SPI各模块输入信号的引脚号
static const uint8 spi_sin_pin_table[] =
{ SPI_MOD0_SIN_PIN, SPI_MOD1_SIN_PIN, SPI_MOD2_SIN_PIN };
//SPI各模块输入信号的PCR的MUX值
static const uint8 spi_sin_pcr_mux_table[] =
{ SPI_MOD0_SIN_PCR_MUX, SPI_MOD1_SIN_PCR_MUX, SPI_MOD2_SIN_PCR_MUX };
//SPI各模块片选信号的引脚号
static const uint8 spi_cs_pin_table[][6] = {
{ SPI_MOD0_CS0_PIN, SPI_MOD0_CS1_PIN, SPI_MOD0_CS2_PIN,
  SPI_MOD0_CS3_PIN, SPI_MOD0_CS4_PIN, SPI_MOD0_CS5_PIN },
{ SPI_MOD1_CS0_PIN, SPI_MOD1_CS1_PIN, SPI_MOD1_CS2_PIN,
  SPI_MOD1_CS3_PIN },
{ SPI_MOD2_CS0_PIN, SPI_MOD2_CS1_PIN }
};
//SPI各模块片选信号的PCR的MUX值
static const uint8 spi_cs_pcr_mux_table[][6] = {
{ SPI_MOD0_CS0_PCR_MUX, SPI_MOD0_CS1_PCR_MUX, SPI_MOD0_CS2_PCR_MUX,
  SPI_MOD0_CS3_PCR_MUX, SPI_MOD0_CS4_PCR_MUX, SPI_MOD0_CS5_PCR_MUX },
{ SPI_MOD1_CS0_PCR_MUX, SPI_MOD1_CS1_PCR_MUX, SPI_MOD1_CS2_PCR_MUX,
  SPI_MOD1_CS3_PCR_MUX },
{ SPI_MOD2_CS0_PCR_MUX, SPI_MOD2_CS1_PCR_MUX }
};
//主机模式下是否启用SPI各模块的片选信号
static const bool spi_cs_enable_table[][6] = {
{ SPI_MOD0_CS0_ENABLE, SPI_MOD0_CS1_ENABLE, SPI_MOD0_CS2_ENABLE,
  SPI_MOD0_CS3_ENABLE, SPI_MOD0_CS4_ENABLE, SPI_MOD0_CS5_ENABLE },
{ SPI_MOD1_CS0_ENABLE, SPI_MOD1_CS1_ENABLE, SPI_MOD1_CS2_ENABLE,
  SPI_MOD1_CS3_ENABLE, SPI_MOD1_CS4_ENABLE, SPI_MOD1_CS5_ENABLE },
{ SPI_MOD2_CS0_ENABLE, SPI_MOD2_CS1_ENABLE, SPI_MOD2_CS2_ENABLE,
  SPI_MOD2_CS3_ENABLE, SPI_MOD2_CS4_ENABLE, SPI_MOD2_CS5_ENABLE }
};
//SPI各模块中断请求号
static const IRQn_Type spi_irq_table[] = { SPI0_IRQn, SPI1_IRQn, SPI2_IRQn };

//==========================================================================
//函数名称: spi_master_init
//函数返回: 无
//参数说明: mod:SPI模块号:
//             SPI_MODx，x为模块号;
//         config:初始化参数所使用的配置号:
//                SPI_CONFIGx，x为配置号，x可取值为0和1;
//         baud_scaler:波特率分频因子:
//                     SPI_BAUD_SCALER_x，x为分频因子大小，x可取值为:
//                     2  | 3  | 4  | 5  | 6  | 7  | 8  | 9   | 10  | 12  |
//                     14 | 15 | 16 | 18 | 20 | 21 | 24 | 28  | 30  | 32  |
//                     40 | 42 | 48 | 56 | 64 | 80 | 96 | 112 | 128 | 160 |
//                     192  | 224  | 256  | 320  | 384  | 448  | 512  | 640    |
//                     768  | 896  | 1024 | 1280 | 1536 | 1792 | 2048 | 2560   |
//                     3072 | 3584 | 4096 | 5120 | 6144 | 7168 | 8192 | 10240  |
//                     12288  | 14336 | 16384 | 20480 | 24576 | 28672 | 32768  |
//                     40960  | 49152 | 57344 | 65536 | 81920 | 98304 | 114688 |
//                     163840 | 229376
//         format:帧格式:
//                SPI_FORMAT_CPOL0_CPHA0:平时低电平，第一跳变取数，第二跳变数据上线;
//                SPI_FORMAT_CPOL0_CPHA1:平时低电平，第一跳变数据上线，第二跳变取数;
//                SPI_FORMAT_CPOL1_CPHA0:平时高电平，第一跳变取数，第二跳变数据上线;
//                SPI_FORMAT_CPOL1_CPHA1:平时高电平，第一跳变数据上线，第二跳变取数;
//         data_size:一帧的数据位数，可取值为[4,16]，一般为8或16，即每次发送1或2个字节
//         bit_order:位传输顺序:
//                   SPI_BIT_ORDER_MSB:最高有效位，即先发送最高位;
//                   SPI_BIT_ORDER_LSB:最低有效位，即先发送最低位;
//         csc_scaler:片选信号有效到第一个时钟周期跳变的延时时间(CSC)的分频因子:
//                    SPI_DELAY_SCALER_x，x为分频因子大小，x可取值为:
//                    2  | 4  | 6  | 8  | 10 | 12 | 14 | 16 | 20  | 24   |
//                    28 | 32 | 40 | 48 | 56 | 64 | 80 | 96 | 112 | 128  |
//                    160 | 192 | 224 | 256  | 320  | 384  | 448  | 512  |
//                    640 | 768 | 896 | 1024 | 1280 | 1536 | 1792 | 2048 |
//                    2560 | 3072  | 3584  | 4096  | 5120  | 6144  | 7168  |
//                    8192 | 10240 | 12288 | 14336 | 16384 | 20480 | 24576 |
//                    28672  | 32768 | 40960  | 49152  | 57344  | 65536  |
//                    81920  | 98304 | 114688 | 163840 | 196608 | 229376 |
//                    327680 | 458752
//         asc_scaler:最后一个时钟周期跳变到片选信号无效的延时时间(ASC)的分频因子，
//                    可选参数同csc_scaler
//         dt_scaler:两帧传输间片选信号无效到片选信号有效的延时时间(DT)的分频因子，
//                    可选参数同csc_scaler
//功能概要: 初始化SPI模块，并设置为主机模式
//备注: SPI模块被设置为主机模式时，可以被初始化两次，使用不同的配置号，
//     可以保存两套发送参数，在发送时选择其中一套使用;
//
//     为了可以正确传输数据，帧格式、帧数据位数、位传输顺序需与从机一致;
//
//     波特率计算公式:总线时钟频率/x，x为SPI_BAUD_SCALER_x的x，总线时钟频率单位为Hz，
//     若总线时钟频率为48MHz，x为4，则波特率为12M，单位为bps;
//
//     延时时间(CSC,ASC,DT)计算公式:x/总线时钟频率，x为SPI_DELAY_SCALER_x的x，
//     总线时钟频率单位为Hz，若总线时钟频率为48MHz，x为12，则延时时间为0.25μs
//==========================================================================
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

	REG_SET_MASK(SPI_MCR_REG(spi_ptr),
			 SPI_MCR_MSTR_MASK		//设置SPI模块为主机模式
			|SPI_MCR_PCSIS_MASK		//设置各片选信号有效电平为低电平
			|SPI_MCR_DIS_TXF_MASK	//关闭FIFO发送队列
			|SPI_MCR_DIS_RXF_MASK	//关闭FIFO接收队列
			|SPI_MCR_CLR_TXF_MASK	//清除FIFO发送计数器
			|SPI_MCR_CLR_RXF_MASK);	//清除FIFO接收计数器

	//直接设置CTAR寄存器的值，节约了IO操作
	REG_SET_VAL(SPI_CTAR_REG(spi_ptr,config),
			 baud_scaler					//设置波特率分频因子
			|SPI_CTAR_FMSZ(data_size-1)		//设置帧数据位数
			|SPI_CTAR_PCSSCK(csc_scaler>>4)	//设置CSC预分频因子
			|SPI_CTAR_CSSCK(csc_scaler)		//设置CSC分频因子
			|SPI_CTAR_PASC(asc_scaler>>4)	//设置ASC预分频因子
			|SPI_CTAR_ASC(asc_scaler)		//设置ASC分频因子
			|SPI_CTAR_PDT(dt_scaler>>4)		//设置DT预分频因子
			|SPI_CTAR_DT(dt_scaler));		//设置DT分频因子

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

//==========================================================================
//函数名称: spi_master_send
//函数返回: 接收到的数据，有效数据位数由所用配置的data_size决定
//参数说明: mod:SPI模块号:
//             SPI_MODx，x为模块号;
//         config:发送时所使用的配置号:
//                SPI_CONFIGx，x为配置号，x可取值为0和1;
//         cs:发送时的片选号，即使能该片选信号:
//            SPI_CSx，x为片选号;
//         data:要发送的数据，有效数据位数由所用配置的data_size决定
//         cont:是否使能连续发送:
//              SPI_CONT_ENABLE: 使能连续发送;
//              SPI_CONT_DISABLE:关闭连续发送;
//功能概要: 在主机模式下，向从机发送数据，并返回接收到的数据
//备注: 主机发送多少位数据，将收到相同位的数据，接收到的数据除有效位外均为0;
//
//     关闭连续发送时，片选信号有效后，等待CSC延时时间，开始传输数据，
//     一帧数据传输完毕后，按顺序等待ASC+DT+CSC延时时间，开始传输下一帧数据，
//     且在DT延时时间期间，片选信号将保持无效;
//
//     开启连续发送时，传输期间片选信号将一直保持有效，
//     两帧之间的DT延时时间将不用等待，即只需等待ASC+CSC延时时间，
//     但发送的每一帧必须使用相同的片选信号，且若切换配置号，
//     两配置间除帧数据位数可以不同外，其他配置必须相同，
//     在发送最后一帧时，必须关闭连续发送
//==========================================================================
uint16 spi_master_send(uint8 mod, uint8 config, uint8 cs, uint16 data,
		uint8 cont) {
	SPI_Type * spi_ptr;	//SPI基地址

	//获取SPI基地址
	spi_ptr = spi_table[mod];

	REG_SET_VAL(SPI_PUSHR_REG(spi_ptr),
			 SPI_PUSHR_CTAS(config)	//设置所用CTAR寄存器，即所用配置
			|SPI_PUSHR_PCS(1<<cs)	//设置有效片选信号
			|SPI_PUSHR_TXDATA(data)	//设置发送数据
			|SPI_PUSHR_CONT(cont));	//设置是否使能连续发送
	//等待传输完成
	while (!REG_GET_MASK(SPI_SR_REG(spi_ptr), SPI_SR_TCF_MASK)) {
	}
	//清除传输完成标志
	REG_SET_VAL(SPI_SR_REG(spi_ptr), SPI_SR_TCF_MASK);
	//返回接收到的数据
	return SPI_POPR_REG(spi_ptr);
}

//==========================================================================
//函数名称: spi_slave_init
//函数返回: 无
//参数说明: mod:SPI模块号:
//             SPI_MODx，x为模块号;
//         format:帧格式:
//                SPI_FORMAT_CPOL0_CPHA0:平时低电平，第一跳变取数，第二跳变数据上线;
//                SPI_FORMAT_CPOL0_CPHA1:平时低电平，第一跳变数据上线，第二跳变取数;
//                SPI_FORMAT_CPOL1_CPHA0:平时高电平，第一跳变取数，第二跳变数据上线;
//                SPI_FORMAT_CPOL1_CPHA1:平时高电平，第一跳变数据上线，第二跳变取数;
//         data_size:一帧的数据位数，可取值为[4,32]，一般为8或16，即每次发送1或2个字节
//功能概要: 初始化SPI模块，并设置为从机模式
//备注: SPI模块被设置为从机模式时，仅可以被初始化一次，默认使用0号配置;
//     为了可以正确传输数据，帧格式、帧数据位数需与主机一致，且从机仅支持MSB
//==========================================================================
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

	REG_CLR_MASK(SPI_MCR_REG(spi_ptr),
			 SPI_MCR_MSTR_MASK		//设置SPI模块为从机模式
			|SPI_MCR_DIS_TXF_MASK	//开启FIFO发送队列
			|SPI_MCR_DIS_RXF_MASK	//开启FIFO接收队列
			|SPI_MCR_CLR_TXF_MASK	//不清除FIFO发送计数器
			|SPI_MCR_CLR_RXF_MASK);	//不清除FIFO接收计数器

	//设置各片选信号有效电平为低电平
	REG_SET_MASK(SPI_MCR_REG(spi_ptr), SPI_MCR_PCSIS_MASK);

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

//==========================================================================
//函数名称: spi_slave_send
//函数返回: true:发送成功; false:发送失败，即发送队列已满
//参数说明: mod:SPI模块号:
//             SPI_MODx，x为模块号;
//         data:要发送的数据，有效数据位数由所用配置的data_size决定
//功能概要: 在从机模式下，向主机发送数据
//备注: 主机不会立即收到从机发送的数据，数据将被放至发送队列中，按顺序等待发送，
//     仅当主机向从机发送数据时，从机的数据才会发送至主机;
//     发送队列深度为5，即最多有5帧等待发送;
//     发送队列无数据时，主机将收到最后放入的一帧，初始为0x00
//==========================================================================
bool spi_slave_send(uint8 mod, uint32 data) {
	SPI_Type * spi_ptr;	//SPI基地址

	//获取SPI基地址
	spi_ptr = spi_table[mod];

	//判断发送队列是否为空
	if (REG_GET_MASK(SPI_SR_REG(spi_ptr), SPI_SR_TFFF_MASK)) {
		//为空时，添加发送数据
		REG_SET_VAL(SPI_PUSHR_SLAVE_REG(spi_ptr), data);
		//尝试清除发送队列空标志，若队列不为空，则仍会置位
		REG_SET_VAL(SPI_SR_REG(spi_ptr), SPI_SR_TFFF_MASK);
		//发送成功
		return true;
	} else {
		//不为空时，发送失败
		return false;
	}
}

//==========================================================================
//函数名称: spi_slave_re
//函数返回: true:接收成功; false:接收失败，即接收队列为空
//参数说明: mod:SPI模块号:
//             SPI_MODx，x为模块号;
//         data:储存接收数据的地址，有效数据位数由所用配置的data_size决定
//功能概要: 在从机模式下，接收主机发送的数据
//备注: 主机发送的数据会被放在从机的接收队列中，按顺序等待取出;
//     接收队列深度为5，即最多有5帧等待取出;
//     从机接收到多少位数据，意味着从机有相同位数的数据发送至主机
//==========================================================================
bool spi_slave_re(uint8 mod, uint32* data) {
	SPI_Type * spi_ptr;	//SPI基地址

	//获取SPI基地址
	spi_ptr = spi_table[mod];

	//判断接收队列是否为空
	if (REG_GET_MASK(SPI_SR_REG(spi_ptr), SPI_SR_RFDF_MASK)) {
		//不为空时，从接收队列取出最先受到的数据
		*data = SPI_POPR_REG(spi_ptr);
		//尝试清除接收队列不为空标志，若队列不为空，则仍会置位
		REG_SET_VAL(SPI_SR_REG(spi_ptr), SPI_SR_RFDF_MASK);
		//接收成功
		return true;
	} else {
		//为空时，接收失败
		return false;
	}
}

//==========================================================================
//函数名称: spi_slave_enable_re_int
//函数返回: 无
//参数说明: mod:SPI模块号:
//             SPI_MODx，x为模块号;
//功能概要: 在从机模式下，使能接收中断
//备注: 当接收队列不为空时，将发生中断，通过接收数据可以清除中断标志
//==========================================================================
void spi_slave_enable_re_int(uint8 mod) {
	//使能接收队列不为空中断
	REG_SET_MASK(SPI_RSER_REG(spi_table[mod]), SPI_RSER_RFDF_RE_MASK);
	//允许接收中断
	ENABLE_IRQ(spi_irq_table[mod]);
}

//==========================================================================
//函数名称: spi_slave_disable_re_int
//函数返回: 无
//参数说明: mod:SPI模块号:
//             SPI_MODx，x为模块号;
//功能概要: 在从机模式下，关闭接收中断
//==========================================================================
void spi_slave_disable_re_int(uint8 mod) {
	//关闭接收队列不为空中断
	REG_CLR_MASK(SPI_RSER_REG(spi_table[mod]), SPI_RSER_RFDF_RE_MASK);
	//禁止接收中断
	DISABLE_IRQ(spi_irq_table[mod]);
}
