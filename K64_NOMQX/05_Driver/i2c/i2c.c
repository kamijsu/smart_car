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
