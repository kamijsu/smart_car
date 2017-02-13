//==========================================================================
//�ļ����ƣ�i2c.h
//���ܸ�Ҫ��K64 I2C�ײ���������ͷ�ļ�
//==========================================================================

#ifndef _I2C_H
#define _I2C_H

//��������ͷ�ļ�
#include "common.h"

//����I2Cģ���
#define I2C_MOD0	(0)
#define I2C_MOD1	(1)
#define I2C_MOD2	(2)

//����I2C��ģ����������ã�ͨ������COM_PORTx|x��x��ѡ�����ţ�
//SCL��SDA�ֱ�Ϊʱ���źš������ź�����
#define I2C_MOD0_SCL_PIN	(COM_PORTB|2)	//B0  B2  D2  D8  E24
#define I2C_MOD0_SDA_PIN	(COM_PORTB|3)	//B1  B3  D3  D9  E25

#define I2C_MOD1_SCL_PIN	(COM_PORTC|10)	//C10 E1
#define I2C_MOD1_SDA_PIN	(COM_PORTC|11)	//C11 E0

#define I2C_MOD2_SCL_PIN	(COM_PORTA|14)	//A12 A14
#define I2C_MOD2_SDA_PIN	(COM_PORTA|13)	//A11 A13

//����I2C��������
#define I2C_MUL_1	(0)		//��������Ϊ1
#define I2C_MUL_2	(1)		//��������Ϊ2
#define I2C_MUL_4	(2)		//��������Ϊ4

//����I2C��ַģʽ
#define I2C_ADDR_MODE_BITS_7	(7)		//7λ��ַģʽ
#define I2C_ADDR_MODE_BITS_10	(10)	//10λ��ַģʽ

#define I2C_TIMEOUT_MAX		(0x2000000u)

typedef enum {
	I2CMasterSuccess,
	I2CMasterNAck,
	I2CMasterArbitrationLost,
	I2CMasterTimeoutSlave,
	I2CMasterTimeoutStop,
	I2CMasterIsBusy
} I2CMasterResult;

typedef enum {
	I2CSlaveNoInt,
	I2CSlaveCalledGeneralInt,
	I2CSlaveCalledReInt,
	I2CSlaveCalledSendInt,
	I2CSlaveDataReInt,
	I2CSlaveDataSendInt,
	I2CSlaveDataSendOverInt,
	I2CSlaveArbitrationLostInt
} I2CSlaveIntType;

void i2c_init(uint8 mod, uint8 mul, uint8 icr, uint8 addr_mode, uint16 addr,
		bool enable_general_call_addr);

I2CMasterResult i2c_master_send(uint8 mod, uint8 addr_mode, uint16 addr,
		uint8* data, uint32 len);

I2CMasterResult i2c_master_send_re(uint8 mod, uint8 addr_mode, uint16 addr,
		uint8* send_data, uint32 send_len, uint8* re_data, uint32 re_len);

I2CMasterResult i2c_master_re(uint8 mod, uint8 addr_mode, uint16 addr,
		uint8* data, uint32 len);

void i2c_slave_enable_int(uint8 mod);

void i2c_slave_disable_int(uint8 mod);

void i2c_slave_set_ack(uint8 mod, bool ack);

I2CSlaveIntType i2c_slave_handle_int(uint8 mod);

uint8 i2c_slave_re(uint8 mod);

void i2c_slave_send(uint8 mod, uint8 data);

//����I2Cģ�������õ����źţ�������Ӧ��PCR��MUXֵ
#ifdef I2C_MOD0_SCL_PIN
#if(I2C_MOD0_SCL_PIN == (COM_PORTE|24))
#define I2C_MOD0_SCL_PCR_MUX	(5)
#elif(I2C_MOD0_SCL_PIN == (COM_PORTD|2))
#define I2C_MOD0_SCL_PCR_MUX	(7)
#else
#define I2C_MOD0_SCL_PCR_MUX	(2)
#endif
#endif

#ifdef I2C_MOD0_SDA_PIN
#if(I2C_MOD0_SDA_PIN == (COM_PORTE|25))
#define I2C_MOD0_SDA_PCR_MUX	(5)
#elif(I2C_MOD0_SDA_PIN == (COM_PORTD|3))
#define I2C_MOD0_SDA_PCR_MUX	(7)
#else
#define I2C_MOD0_SDA_PCR_MUX	(2)
#endif
#endif

#ifdef I2C_MOD1_SCL_PIN
#if(I2C_MOD1_SCL_PIN == (COM_PORTE|1))
#define I2C_MOD1_SCL_PCR_MUX	(6)
#else
#define I2C_MOD1_SCL_PCR_MUX	(2)
#endif
#endif

#ifdef I2C_MOD1_SDA_PIN
#if(I2C_MOD1_SDA_PIN == (COM_PORTE|0))
#define I2C_MOD1_SDA_PCR_MUX	(6)
#else
#define I2C_MOD1_SDA_PCR_MUX	(2)
#endif
#endif

#ifdef I2C_MOD2_SCL_PIN
#define I2C_MOD2_SCL_PCR_MUX	(5)
#endif

#ifdef I2C_MOD2_SDA_PIN
#define I2C_MOD2_SDA_PCR_MUX	(5)
#endif

#endif
