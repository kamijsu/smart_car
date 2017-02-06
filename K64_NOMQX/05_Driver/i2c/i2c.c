//==========================================================================
//�ļ����ƣ�i2c.c
//���ܸ�Ҫ��K64 I2C�ײ���������Դ�ļ�
//==========================================================================

#include "i2c.h"

//I2C��ģ�����ַ
static I2C_Type * const i2c_table[] = I2C_BASE_PTRS;
//I2C��ģ��ʱ���źŵ����ź�
static const uint8 i2c_scl_pin_table[] = { I2C_MOD0_SCL_PIN, I2C_MOD1_SCL_PIN,
I2C_MOD2_SCL_PIN };
//I2C��ģ��ʱ���źŵ�PCR��MUXֵ
static const uint8 i2c_scl_pcr_mux_table[] = { I2C_MOD0_SCL_PCR_MUX,
I2C_MOD1_SCL_PCR_MUX, I2C_MOD2_SCL_PCR_MUX };
//I2C��ģ�������źŵ����ź�
static const uint8 i2c_sda_pin_table[] = { I2C_MOD0_SDA_PIN, I2C_MOD1_SDA_PIN,
I2C_MOD2_SDA_PIN };
//I2C��ģ�������źŵ�PCR��MUXֵ
static const uint8 i2c_sda_pcr_mux_table[] = { I2C_MOD0_SDA_PCR_MUX,
I2C_MOD1_SDA_PCR_MUX, I2C_MOD2_SDA_PCR_MUX };
