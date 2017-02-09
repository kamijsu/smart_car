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
#define I2C_MOD0_SCL_PIN	(COM_PORTB|0)	//B0  B2  D2  D8  E24
#define I2C_MOD0_SDA_PIN	(COM_PORTB|1)	//B1  B3  D3  D9  E25

#define I2C_MOD1_SCL_PIN	(COM_PORTC|10)	//C10 E1
#define I2C_MOD1_SDA_PIN	(COM_PORTC|11)	//C11 E0

#define I2C_MOD2_SCL_PIN	(COM_PORTA|14)	//A12 A14
#define I2C_MOD2_SDA_PIN	(COM_PORTA|13)	//A11 A13

//����I2C��������
#define I2C_MUL_1	(0)		//��������Ϊ1
#define I2C_MUL_2	(1)		//��������Ϊ2
#define I2C_MUL_4	(2)		//��������Ϊ4

//����I2C��ַģʽ
#define I2C_ADDR_MODE_BITS_7	(0)		//7λ��ַģʽ
#define I2C_ADDR_MODE_BITS_10	(1)		//10λ��ַģʽ

typedef enum {
	I2CSuccess, I2CNAck, I2CArbitrationLost
} I2CResult;

void i2c_init(uint8 mod, uint8 mul, uint8 icr, uint8 addr_mode, uint16 addr,
		bool enable_general_call_addr);

I2CResult i2c_master_send(uint8 mod, uint8 addr, uint8* data, uint32 len);

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
