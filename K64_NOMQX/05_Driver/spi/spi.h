//==========================================================================
//�ļ����ƣ�spi.h
//���ܸ�Ҫ��K64 SPI�ײ���������ͷ�ļ�
//==========================================================================

#ifndef _SPI_H
#define _SPI_H

//��������ͷ�ļ�
#include "common.h"

//����SPIģ���
#define SPI_MOD0	(0)
#define SPI_MOD1	(1)
#define SPI_MOD2	(2)

//����SPIƬѡ��
#define SPI_CS0		(0)
#define SPI_CS1		(1)
#define SPI_CS2		(2)
#define SPI_CS3		(3)
#define SPI_CS4		(4)
#define SPI_CS5		(5)

//����SPI���ú�
#define SPI_CONFIG0		(0)
#define SPI_CONFIG1		(1)

//����SPI��ģ����������ã�ͨ������COM_PORTx|x��x��ѡ�����ţ�
//SCK��SOUT��SIN��CSx�ֱ�Ϊʱ���źš�����źš������źš�Ƭѡ�ź�����
#define SPI_MOD0_SCK_PIN	(COM_PORTC|5)	//A15 C5  D1
#define SPI_MOD0_SOUT_PIN	(COM_PORTC|6)	//A16 C6  D2
#define SPI_MOD0_SIN_PIN	(COM_PORTC|7)	//A17 C7  D3
#define SPI_MOD0_CS0_PIN	(COM_PORTC|4)	//A14 C4  D0
#define SPI_MOD0_CS1_PIN	(COM_PORTC|3)	//    C3  D4
#define SPI_MOD0_CS2_PIN	(COM_PORTC|2)	//    C2  D5
#define SPI_MOD0_CS3_PIN	(COM_PORTC|1)	//    C1  D6
#define SPI_MOD0_CS4_PIN	(COM_PORTC|0)	//    C0
#define SPI_MOD0_CS5_PIN	(COM_PORTB|23)	//    B23

#define SPI_MOD1_SCK_PIN	(COM_PORTE|2)	//E2      B11 D5
#define SPI_MOD1_SOUT_PIN	(COM_PORTE|1)	//E1  E3  B16 D6
#define SPI_MOD1_SIN_PIN	(COM_PORTE|3)	//E3  E1  B17 D7
#define SPI_MOD1_CS0_PIN	(COM_PORTE|4)	//E4      B10 D4
#define SPI_MOD1_CS1_PIN	(COM_PORTE|0)	//E0      B9
#define SPI_MOD1_CS2_PIN	(COM_PORTE|5)	//E5
#define SPI_MOD1_CS3_PIN	(COM_PORTE|6)	//E6

#define SPI_MOD2_SCK_PIN	(COM_PORTB|21)	//B21 D12
#define SPI_MOD2_SOUT_PIN	(COM_PORTB|22)	//B22 D13
#define SPI_MOD2_SIN_PIN	(COM_PORTB|23)	//B23 D14
#define SPI_MOD2_CS0_PIN	(COM_PORTB|20)	//B20 D11
#define SPI_MOD2_CS1_PIN	(COM_PORTD|15)	//    D15

//����SPI��ģ��������ģʽ���Ƿ�����Ƭѡ�źţ�trueΪ���ã�falseΪ�����ã�
//���޸�Ƭѡ�źţ�������false;�ӻ�ģʽ�£�����������ã���������CS0
#define SPI_MOD0_CS0_ENABLE		true
#define SPI_MOD0_CS1_ENABLE		false
#define SPI_MOD0_CS2_ENABLE		false
#define SPI_MOD0_CS3_ENABLE		false
#define SPI_MOD0_CS4_ENABLE		false
#define SPI_MOD0_CS5_ENABLE		false

#define SPI_MOD1_CS0_ENABLE		true
#define SPI_MOD1_CS1_ENABLE		false
#define SPI_MOD1_CS2_ENABLE		false
#define SPI_MOD1_CS3_ENABLE		false
#define SPI_MOD1_CS4_ENABLE		false
#define SPI_MOD1_CS5_ENABLE		false

#define SPI_MOD2_CS0_ENABLE		true
#define SPI_MOD2_CS1_ENABLE		false
#define SPI_MOD2_CS2_ENABLE		false
#define SPI_MOD2_CS3_ENABLE		false
#define SPI_MOD2_CS4_ENABLE		false
#define SPI_MOD2_CS5_ENABLE		false

//����SPI�����ʷ�Ƶ���ӣ�SPI_BAUD_SCALER_x��xΪ��Ƶ���Ӵ�С
#define SPI_BAUD_SCALER_2		(SPI_CTAR_DBR_MASK|SPI_CTAR_PBR(0)|SPI_CTAR_BR(0))
#define SPI_BAUD_SCALER_3		(SPI_CTAR_DBR_MASK|SPI_CTAR_PBR(1)|SPI_CTAR_BR(0))
#define SPI_BAUD_SCALER_4		(SPI_CTAR_PBR(0)|SPI_CTAR_BR(0))
#define SPI_BAUD_SCALER_5		(SPI_CTAR_DBR_MASK|SPI_CTAR_PBR(2)|SPI_CTAR_BR(0))
#define SPI_BAUD_SCALER_6		(SPI_CTAR_PBR(1)|SPI_CTAR_BR(0))
#define SPI_BAUD_SCALER_7		(SPI_CTAR_DBR_MASK|SPI_CTAR_PBR(3)|SPI_CTAR_BR(0))
#define SPI_BAUD_SCALER_8		(SPI_CTAR_PBR(0)|SPI_CTAR_BR(1))
#define SPI_BAUD_SCALER_9		(SPI_CTAR_DBR_MASK|SPI_CTAR_PBR(1)|SPI_CTAR_BR(2))
#define SPI_BAUD_SCALER_10		(SPI_CTAR_PBR(2)|SPI_CTAR_BR(0))
#define SPI_BAUD_SCALER_12		(SPI_CTAR_PBR(0)|SPI_CTAR_BR(2))
#define SPI_BAUD_SCALER_14		(SPI_CTAR_PBR(3)|SPI_CTAR_BR(0))
#define SPI_BAUD_SCALER_15		(SPI_CTAR_DBR_MASK|SPI_CTAR_PBR(2)|SPI_CTAR_BR(2))
#define SPI_BAUD_SCALER_16		(SPI_CTAR_PBR(0)|SPI_CTAR_BR(3))
#define SPI_BAUD_SCALER_18		(SPI_CTAR_PBR(1)|SPI_CTAR_BR(2))
#define SPI_BAUD_SCALER_20		(SPI_CTAR_PBR(2)|SPI_CTAR_BR(1))
#define SPI_BAUD_SCALER_21		(SPI_CTAR_DBR_MASK|SPI_CTAR_PBR(3)|SPI_CTAR_BR(2))
#define SPI_BAUD_SCALER_24		(SPI_CTAR_PBR(1)|SPI_CTAR_BR(3))
#define SPI_BAUD_SCALER_28		(SPI_CTAR_PBR(3)|SPI_CTAR_BR(1))
#define SPI_BAUD_SCALER_30		(SPI_CTAR_PBR(2)|SPI_CTAR_BR(2))
#define SPI_BAUD_SCALER_32		(SPI_CTAR_PBR(0)|SPI_CTAR_BR(4))
#define SPI_BAUD_SCALER_40		(SPI_CTAR_PBR(2)|SPI_CTAR_BR(3))
#define SPI_BAUD_SCALER_42		(SPI_CTAR_PBR(3)|SPI_CTAR_BR(2))
#define SPI_BAUD_SCALER_48		(SPI_CTAR_PBR(1)|SPI_CTAR_BR(4))
#define SPI_BAUD_SCALER_56		(SPI_CTAR_PBR(3)|SPI_CTAR_BR(3))
#define SPI_BAUD_SCALER_64		(SPI_CTAR_PBR(0)|SPI_CTAR_BR(5))
#define SPI_BAUD_SCALER_80		(SPI_CTAR_PBR(2)|SPI_CTAR_BR(4))
#define SPI_BAUD_SCALER_96		(SPI_CTAR_PBR(1)|SPI_CTAR_BR(5))
#define SPI_BAUD_SCALER_112		(SPI_CTAR_PBR(3)|SPI_CTAR_BR(4))
#define SPI_BAUD_SCALER_128		(SPI_CTAR_PBR(0)|SPI_CTAR_BR(6))
#define SPI_BAUD_SCALER_160		(SPI_CTAR_PBR(2)|SPI_CTAR_BR(5))
#define SPI_BAUD_SCALER_192		(SPI_CTAR_PBR(1)|SPI_CTAR_BR(6))
#define SPI_BAUD_SCALER_224		(SPI_CTAR_PBR(3)|SPI_CTAR_BR(5))
#define SPI_BAUD_SCALER_256		(SPI_CTAR_PBR(0)|SPI_CTAR_BR(7))
#define SPI_BAUD_SCALER_320		(SPI_CTAR_PBR(2)|SPI_CTAR_BR(6))
#define SPI_BAUD_SCALER_384		(SPI_CTAR_PBR(1)|SPI_CTAR_BR(7))
#define SPI_BAUD_SCALER_448		(SPI_CTAR_PBR(3)|SPI_CTAR_BR(6))
#define SPI_BAUD_SCALER_512		(SPI_CTAR_PBR(0)|SPI_CTAR_BR(8))
#define SPI_BAUD_SCALER_640		(SPI_CTAR_PBR(2)|SPI_CTAR_BR(7))
#define SPI_BAUD_SCALER_768		(SPI_CTAR_PBR(1)|SPI_CTAR_BR(8))
#define SPI_BAUD_SCALER_896		(SPI_CTAR_PBR(3)|SPI_CTAR_BR(7))
#define SPI_BAUD_SCALER_1024	(SPI_CTAR_PBR(0)|SPI_CTAR_BR(9))
#define SPI_BAUD_SCALER_1280	(SPI_CTAR_PBR(2)|SPI_CTAR_BR(8))
#define SPI_BAUD_SCALER_1536	(SPI_CTAR_PBR(1)|SPI_CTAR_BR(9))
#define SPI_BAUD_SCALER_1792	(SPI_CTAR_PBR(3)|SPI_CTAR_BR(8))
#define SPI_BAUD_SCALER_2048	(SPI_CTAR_PBR(0)|SPI_CTAR_BR(10))
#define SPI_BAUD_SCALER_2560	(SPI_CTAR_PBR(2)|SPI_CTAR_BR(9))
#define SPI_BAUD_SCALER_3072	(SPI_CTAR_PBR(1)|SPI_CTAR_BR(10))
#define SPI_BAUD_SCALER_3584	(SPI_CTAR_PBR(3)|SPI_CTAR_BR(9))
#define SPI_BAUD_SCALER_4096	(SPI_CTAR_PBR(0)|SPI_CTAR_BR(11))
#define SPI_BAUD_SCALER_5120	(SPI_CTAR_PBR(2)|SPI_CTAR_BR(10))
#define SPI_BAUD_SCALER_6144	(SPI_CTAR_PBR(1)|SPI_CTAR_BR(11))
#define SPI_BAUD_SCALER_7168	(SPI_CTAR_PBR(3)|SPI_CTAR_BR(10))
#define SPI_BAUD_SCALER_8192	(SPI_CTAR_PBR(0)|SPI_CTAR_BR(12))
#define SPI_BAUD_SCALER_10240	(SPI_CTAR_PBR(2)|SPI_CTAR_BR(11))
#define SPI_BAUD_SCALER_12288	(SPI_CTAR_PBR(1)|SPI_CTAR_BR(12))
#define SPI_BAUD_SCALER_14336	(SPI_CTAR_PBR(3)|SPI_CTAR_BR(11))
#define SPI_BAUD_SCALER_16384	(SPI_CTAR_PBR(0)|SPI_CTAR_BR(13))
#define SPI_BAUD_SCALER_20480	(SPI_CTAR_PBR(2)|SPI_CTAR_BR(12))
#define SPI_BAUD_SCALER_24576	(SPI_CTAR_PBR(1)|SPI_CTAR_BR(13))
#define SPI_BAUD_SCALER_28672	(SPI_CTAR_PBR(3)|SPI_CTAR_BR(12))
#define SPI_BAUD_SCALER_32768	(SPI_CTAR_PBR(0)|SPI_CTAR_BR(14))
#define SPI_BAUD_SCALER_40960	(SPI_CTAR_PBR(2)|SPI_CTAR_BR(13))
#define SPI_BAUD_SCALER_49152	(SPI_CTAR_PBR(1)|SPI_CTAR_BR(14))
#define SPI_BAUD_SCALER_57344	(SPI_CTAR_PBR(3)|SPI_CTAR_BR(13))
#define SPI_BAUD_SCALER_65536	(SPI_CTAR_PBR(0)|SPI_CTAR_BR(15))
#define SPI_BAUD_SCALER_81920	(SPI_CTAR_PBR(2)|SPI_CTAR_BR(14))
#define SPI_BAUD_SCALER_98304	(SPI_CTAR_PBR(1)|SPI_CTAR_BR(15))
#define SPI_BAUD_SCALER_114688	(SPI_CTAR_PBR(3)|SPI_CTAR_BR(14))
#define SPI_BAUD_SCALER_163840	(SPI_CTAR_PBR(2)|SPI_CTAR_BR(15))
#define SPI_BAUD_SCALER_229376	(SPI_CTAR_PBR(3)|SPI_CTAR_BR(15))

//����SPI֡��ʽ
#define SPI_FORMAT_CPOL0_CPHA0	(0)	//ƽʱ�͵�ƽ����һ����ȡ�����ڶ�������������
#define SPI_FORMAT_CPOL0_CPHA1	(1)	//ƽʱ�͵�ƽ����һ�����������ߣ��ڶ�����ȡ��
#define SPI_FORMAT_CPOL1_CPHA0	(2)	//ƽʱ�ߵ�ƽ����һ����ȡ�����ڶ�������������
#define SPI_FORMAT_CPOL1_CPHA1	(3)	//ƽʱ�ߵ�ƽ����һ�����������ߣ��ڶ�����ȡ��

//����SPIģ��λ����˳��
#define SPI_BIT_ORDER_MSB		(0)		//�����Чλ�����ȷ������λ
#define SPI_BIT_ORDER_LSB		(1)		//�����Чλ�����ȷ������λ

//����SPI��ʱ��Ƶ���ӣ�SPI_DELAY_SCALER_x��xΪ��Ƶ���Ӵ�С��
//����ֵ��ЧλΪ6λ����2λΪԤ��Ƶ����ֵ����4λΪ��Ƶ����ֵ
#define SPI_DELAY_SCALER_2		(0x00)
#define SPI_DELAY_SCALER_4		(0x01)
#define SPI_DELAY_SCALER_6		(0x10)
#define SPI_DELAY_SCALER_8		(0x02)
#define SPI_DELAY_SCALER_10		(0x20)
#define SPI_DELAY_SCALER_12		(0x11)
#define SPI_DELAY_SCALER_14		(0x30)
#define SPI_DELAY_SCALER_16		(0x03)
#define SPI_DELAY_SCALER_20		(0x21)
#define SPI_DELAY_SCALER_24		(0x12)
#define SPI_DELAY_SCALER_28		(0x31)
#define SPI_DELAY_SCALER_32		(0x04)
#define SPI_DELAY_SCALER_40		(0x22)
#define SPI_DELAY_SCALER_48		(0x13)
#define SPI_DELAY_SCALER_56		(0x32)
#define SPI_DELAY_SCALER_64		(0x05)
#define SPI_DELAY_SCALER_80		(0x23)
#define SPI_DELAY_SCALER_96		(0x14)
#define SPI_DELAY_SCALER_112	(0x33)
#define SPI_DELAY_SCALER_128	(0x06)
#define SPI_DELAY_SCALER_160	(0x24)
#define SPI_DELAY_SCALER_192	(0x15)
#define SPI_DELAY_SCALER_224	(0x34)
#define SPI_DELAY_SCALER_256	(0x07)
#define SPI_DELAY_SCALER_320	(0x25)
#define SPI_DELAY_SCALER_384	(0x16)
#define SPI_DELAY_SCALER_448	(0x35)
#define SPI_DELAY_SCALER_512	(0x08)
#define SPI_DELAY_SCALER_640	(0x26)
#define SPI_DELAY_SCALER_768	(0x17)
#define SPI_DELAY_SCALER_896	(0x36)
#define SPI_DELAY_SCALER_1024	(0x09)
#define SPI_DELAY_SCALER_1280	(0x27)
#define SPI_DELAY_SCALER_1536	(0x18)
#define SPI_DELAY_SCALER_1792	(0x37)
#define SPI_DELAY_SCALER_2048	(0x0A)
#define SPI_DELAY_SCALER_2560	(0x28)
#define SPI_DELAY_SCALER_3072	(0x19)
#define SPI_DELAY_SCALER_3584	(0x38)
#define SPI_DELAY_SCALER_4096	(0x0B)
#define SPI_DELAY_SCALER_5120	(0x29)
#define SPI_DELAY_SCALER_6144	(0x1A)
#define SPI_DELAY_SCALER_7168	(0x39)
#define SPI_DELAY_SCALER_8192	(0x0C)
#define SPI_DELAY_SCALER_10240	(0x2A)
#define SPI_DELAY_SCALER_12288	(0x1B)
#define SPI_DELAY_SCALER_14336	(0x3A)
#define SPI_DELAY_SCALER_16384	(0x0D)
#define SPI_DELAY_SCALER_20480	(0x2B)
#define SPI_DELAY_SCALER_24576	(0x1C)
#define SPI_DELAY_SCALER_28672	(0x3B)
#define SPI_DELAY_SCALER_32768	(0x0E)
#define SPI_DELAY_SCALER_40960	(0x2C)
#define SPI_DELAY_SCALER_49152	(0x1D)
#define SPI_DELAY_SCALER_57344	(0x3C)
#define SPI_DELAY_SCALER_65536	(0x0F)
#define SPI_DELAY_SCALER_81920	(0x2D)
#define SPI_DELAY_SCALER_98304	(0x1E)
#define SPI_DELAY_SCALER_114688	(0x3D)
#define SPI_DELAY_SCALER_163840	(0x2E)
#define SPI_DELAY_SCALER_196608	(0x1F)
#define SPI_DELAY_SCALER_229376	(0x3E)
#define SPI_DELAY_SCALER_327680	(0x2F)
#define SPI_DELAY_SCALER_458752	(0x3F)

//��������ģʽ���Ƿ�ʹ����������(CONTΪcontinuous��д)
#define SPI_CONT_ENABLE			(1)		//ʹ����������
#define SPI_CONT_DISABLE		(0)		//�ر���������

//==========================================================================
//��������: spi_master_init
//��������: ��
//����˵��: mod:SPIģ���:
//             SPI_MODx��xΪģ���;
//         config:��ʼ��������ʹ�õ����ú�:
//                SPI_CONFIGx��xΪ���úţ�x��ȡֵΪ0��1;
//         baud_scaler:�����ʷ�Ƶ����:
//                     SPI_BAUD_SCALER_x��xΪ��Ƶ���Ӵ�С��x��ȡֵΪ:
//                     2  | 3  | 4  | 5  | 6  | 7  | 8  | 9   | 10  | 12  |
//                     14 | 15 | 16 | 18 | 20 | 21 | 24 | 28  | 30  | 32  |
//                     40 | 42 | 48 | 56 | 64 | 80 | 96 | 112 | 128 | 160 |
//                     192  | 224  | 256  | 320  | 384  | 448  | 512  | 640    |
//                     768  | 896  | 1024 | 1280 | 1536 | 1792 | 2048 | 2560   |
//                     3072 | 3584 | 4096 | 5120 | 6144 | 7168 | 8192 | 10240  |
//                     12288  | 14336 | 16384 | 20480 | 24576 | 28672 | 32768  |
//                     40960  | 49152 | 57344 | 65536 | 81920 | 98304 | 114688 |
//                     163840 | 229376
//         format:֡��ʽ:
//                SPI_FORMAT_CPOL0_CPHA0:ƽʱ�͵�ƽ����һ����ȡ�����ڶ�������������;
//                SPI_FORMAT_CPOL0_CPHA1:ƽʱ�͵�ƽ����һ�����������ߣ��ڶ�����ȡ��;
//                SPI_FORMAT_CPOL1_CPHA0:ƽʱ�ߵ�ƽ����һ����ȡ�����ڶ�������������;
//                SPI_FORMAT_CPOL1_CPHA1:ƽʱ�ߵ�ƽ����һ�����������ߣ��ڶ�����ȡ��;
//         data_size:һ֡������λ������ȡֵΪ[4,16]��һ��Ϊ8��16����ÿ�η���1��2���ֽ�
//         bit_order:λ����˳��:
//                   SPI_BIT_ORDER_MSB:�����Чλ�����ȷ������λ;
//                   SPI_BIT_ORDER_LSB:�����Чλ�����ȷ������λ;
//         csc_scaler:Ƭѡ�ź���Ч����һ��ʱ�������������ʱʱ��(CSC)�ķ�Ƶ����:
//                    SPI_DELAY_SCALER_x��xΪ��Ƶ���Ӵ�С��x��ȡֵΪ:
//                    2  | 4  | 6  | 8  | 10 | 12 | 14 | 16 | 20  | 24   |
//                    28 | 32 | 40 | 48 | 56 | 64 | 80 | 96 | 112 | 128  |
//                    160 | 192 | 224 | 256  | 320  | 384  | 448  | 512  |
//                    640 | 768 | 896 | 1024 | 1280 | 1536 | 1792 | 2048 |
//                    2560 | 3072  | 3584  | 4096  | 5120  | 6144  | 7168  |
//                    8192 | 10240 | 12288 | 14336 | 16384 | 20480 | 24576 |
//                    28672  | 32768 | 40960  | 49152  | 57344  | 65536  |
//                    81920  | 98304 | 114688 | 163840 | 196608 | 229376 |
//                    327680 | 458752
//         asc_scaler:���һ��ʱ���������䵽Ƭѡ�ź���Ч����ʱʱ��(ASC)�ķ�Ƶ���ӣ�
//                    ��ѡ����ͬcsc_scaler
//         dt_scaler:��֡�����Ƭѡ�ź���Ч��Ƭѡ�ź���Ч����ʱʱ��(DT)�ķ�Ƶ���ӣ�
//                    ��ѡ����ͬcsc_scaler
//���ܸ�Ҫ: ��ʼ��SPIģ�飬������Ϊ����ģʽ
//��ע: SPIģ�鱻����Ϊ����ģʽʱ�����Ա���ʼ�����Σ�ʹ�ò�ͬ�����úţ�
//     ���Ա������׷��Ͳ������ڷ���ʱѡ������һ��ʹ��;
//
//     Ϊ�˿�����ȷ�������ݣ�֡��ʽ��֡����λ����λ����˳������ӻ�һ��;
//
//     �����ʼ��㹫ʽ:����ʱ��Ƶ��/x��xΪSPI_BAUD_SCALER_x��x������ʱ��Ƶ�ʵ�λΪHz��
//     ������ʱ��Ƶ��Ϊ48MHz��xΪ4��������Ϊ12M����λΪbps;
//
//     ��ʱʱ��(CSC,ASC,DT)���㹫ʽ:x/����ʱ��Ƶ�ʣ�xΪSPI_DELAY_SCALER_x��x��
//     ����ʱ��Ƶ�ʵ�λΪHz��������ʱ��Ƶ��Ϊ48MHz��xΪ12������ʱʱ��Ϊ0.25��s
//==========================================================================
void spi_master_init(uint8 mod, uint8 config, uint32 baud_scaler, uint8 format,
		uint8 data_size, uint8 bit_order, uint8 csc_scaler, uint8 asc_scaler,
		uint8 dt_scaler);

//==========================================================================
//��������: spi_master_send
//��������: ���յ������ݣ���Ч����λ�����������õ�data_size����
//����˵��: mod:SPIģ���:
//             SPI_MODx��xΪģ���;
//         config:����ʱ��ʹ�õ����ú�:
//                SPI_CONFIGx��xΪ���úţ�x��ȡֵΪ0��1;
//         cs:����ʱ��Ƭѡ�ţ���ʹ�ܸ�Ƭѡ�ź�:
//            SPI_CSx��xΪƬѡ��;
//         data:Ҫ���͵����ݣ���Ч����λ�����������õ�data_size����
//         cont:�Ƿ�ʹ����������:
//              SPI_CONT_ENABLE: ʹ����������;
//              SPI_CONT_DISABLE:�ر���������;
//���ܸ�Ҫ: ������ģʽ�£���ӻ��������ݣ������ؽ��յ�������
//��ע: �������Ͷ���λ���ݣ����յ���ͬλ�����ݣ����յ������ݳ���Чλ���Ϊ0;
//
//     �ر���������ʱ��Ƭѡ�ź���Ч�󣬵ȴ�CSC��ʱʱ�䣬��ʼ�������ݣ�
//     һ֡���ݴ�����Ϻ󣬰�˳��ȴ�ASC+DT+CSC��ʱʱ�䣬��ʼ������һ֡���ݣ�
//     ����DT��ʱʱ���ڼ䣬Ƭѡ�źŽ�������Ч;
//
//     ������������ʱ�������ڼ�Ƭѡ�źŽ�һֱ������Ч��
//     ��֮֡���DT��ʱʱ�佫���õȴ�����ֻ��ȴ�ASC+CSC��ʱʱ�䣬
//     �����͵�ÿһ֡����ʹ����ͬ��Ƭѡ�źţ������л����úţ�
//     �����ü��֡����λ�����Բ�ͬ�⣬�������ñ�����ͬ��
//     �ڷ������һ֡ʱ������ر���������
//==========================================================================
uint16 spi_master_send(uint8 mod, uint8 config, uint8 cs, uint16 data,
		uint8 cont);

//==========================================================================
//��������: spi_slave_init
//��������: ��
//����˵��: mod:SPIģ���:
//             SPI_MODx��xΪģ���;
//         format:֡��ʽ:
//                SPI_FORMAT_CPOL0_CPHA0:ƽʱ�͵�ƽ����һ����ȡ�����ڶ�������������;
//                SPI_FORMAT_CPOL0_CPHA1:ƽʱ�͵�ƽ����һ�����������ߣ��ڶ�����ȡ��;
//                SPI_FORMAT_CPOL1_CPHA0:ƽʱ�ߵ�ƽ����һ����ȡ�����ڶ�������������;
//                SPI_FORMAT_CPOL1_CPHA1:ƽʱ�ߵ�ƽ����һ�����������ߣ��ڶ�����ȡ��;
//         data_size:һ֡������λ������ȡֵΪ[4,32]��һ��Ϊ8��16����ÿ�η���1��2���ֽ�
//���ܸ�Ҫ: ��ʼ��SPIģ�飬������Ϊ�ӻ�ģʽ
//��ע: SPIģ�鱻����Ϊ�ӻ�ģʽʱ�������Ա���ʼ��һ�Σ�Ĭ��ʹ��0������;
//     Ϊ�˿�����ȷ�������ݣ�֡��ʽ��֡����λ����������һ�£��Ҵӻ���֧��MSB
//==========================================================================
void spi_slave_init(uint8 mod, uint8 format, uint8 data_size);

//==========================================================================
//��������: spi_slave_send
//��������: true:���ͳɹ�; false:����ʧ�ܣ������Ͷ�������
//����˵��: mod:SPIģ���:
//             SPI_MODx��xΪģ���;
//         data:Ҫ���͵����ݣ���Ч����λ�����������õ�data_size����
//���ܸ�Ҫ: �ڴӻ�ģʽ�£���������������
//��ע: �������������յ��ӻ����͵����ݣ����ݽ����������Ͷ����У���˳��ȴ����ͣ�
//     ����������ӻ���������ʱ���ӻ������ݲŻᷢ��������;
//     ���Ͷ������Ϊ5���������5֡�ȴ�����;
//     ���Ͷ���������ʱ���������յ��������һ֡����ʼΪ0x00
//==========================================================================
bool spi_slave_send(uint8 mod, uint32 data);

//==========================================================================
//��������: spi_slave_re
//��������: true:���ճɹ�; false:����ʧ�ܣ������ն���Ϊ��
//����˵��: mod:SPIģ���:
//             SPI_MODx��xΪģ���;
//         data:����������ݵĵ�ַ����Ч����λ�����������õ�data_size����
//���ܸ�Ҫ: �ڴӻ�ģʽ�£������������͵�����
//��ע: �������͵����ݻᱻ���ڴӻ��Ľ��ն����У���˳��ȴ�ȡ��;
//     ���ն������Ϊ5���������5֡�ȴ�ȡ��;
//     �ӻ����յ�����λ���ݣ���ζ�Ŵӻ�����ͬλ�������ݷ���������
//==========================================================================
bool spi_slave_re(uint8 mod, uint32* data);

//==========================================================================
//��������: spi_slave_enable_re_int
//��������: ��
//����˵��: mod:SPIģ���:
//             SPI_MODx��xΪģ���;
//���ܸ�Ҫ: �ڴӻ�ģʽ�£�ʹ�ܽ����ж�
//��ע: �����ն��в�Ϊ��ʱ���������жϣ�ͨ���������ݿ�������жϱ�־
//==========================================================================
void spi_slave_enable_re_int(uint8 mod);

//==========================================================================
//��������: spi_slave_disable_re_int
//��������: ��
//����˵��: mod:SPIģ���:
//             SPI_MODx��xΪģ���;
//���ܸ�Ҫ: �ڴӻ�ģʽ�£��رս����ж�
//==========================================================================
void spi_slave_disable_re_int(uint8 mod);

//����SPIģ�������õ����źţ�������Ӧ��PCR��MUXֵ
#ifdef SPI_MOD0_SCK_PIN
#define SPI_MOD0_SCK_PCR_MUX	(2)
#endif

#ifdef SPI_MOD0_SOUT_PIN
#define SPI_MOD0_SOUT_PCR_MUX	(2)
#endif

#ifdef SPI_MOD0_SIN_PIN
#define SPI_MOD0_SIN_PCR_MUX	(2)
#endif

#ifdef SPI_MOD0_CS0_PIN
#define SPI_MOD0_CS0_PCR_MUX	(2)
#endif

#ifdef SPI_MOD0_CS1_PIN
#define SPI_MOD0_CS1_PCR_MUX	(2)
#endif

#ifdef SPI_MOD0_CS2_PIN
#define SPI_MOD0_CS2_PCR_MUX	(2)
#endif

#ifdef SPI_MOD0_CS3_PIN
#define SPI_MOD0_CS3_PCR_MUX	(2)
#endif

#ifdef SPI_MOD0_CS4_PIN
#define SPI_MOD0_CS4_PCR_MUX	(2)
#endif

#ifdef SPI_MOD0_CS5_PIN
#define SPI_MOD0_CS5_PCR_MUX	(3)
#endif

#ifdef SPI_MOD1_SCK_PIN
#if(SPI_MOD1_SCK_PIN == (COM_PORTD|5))
#define SPI_MOD1_SCK_PCR_MUX	(7)
#else
#define SPI_MOD1_SCK_PCR_MUX	(2)
#endif
#endif

#ifdef SPI_MOD1_SOUT_PIN
#if(SPI_MOD1_SOUT_PIN == (COM_PORTE|1) || SPI_MOD1_SOUT_PIN == (COM_PORTB|16))
#define SPI_MOD1_SOUT_PCR_MUX	(2)
#else
#define SPI_MOD1_SOUT_PCR_MUX	(7)
#endif
#endif

#ifdef SPI_MOD1_SIN_PIN
#if(SPI_MOD1_SIN_PIN == (COM_PORTE|3) || SPI_MOD1_SIN_PIN == (COM_PORTB|17))
#define SPI_MOD1_SIN_PCR_MUX	(2)
#else
#define SPI_MOD1_SIN_PCR_MUX	(7)
#endif
#endif

#ifdef SPI_MOD1_CS0_PIN
#if(SPI_MOD1_CS0_PIN == (COM_PORTD|4))
#define SPI_MOD1_CS0_PCR_MUX	(7)
#else
#define SPI_MOD1_CS0_PCR_MUX	(2)
#endif
#endif

#ifdef SPI_MOD1_CS1_PIN
#define SPI_MOD1_CS1_PCR_MUX	(2)
#endif

#ifdef SPI_MOD1_CS2_PIN
#define SPI_MOD1_CS2_PCR_MUX	(2)
#endif

#ifdef SPI_MOD1_CS3_PIN
#define SPI_MOD1_CS3_PCR_MUX	(2)
#endif

#ifdef SPI_MOD2_SCK_PIN
#define SPI_MOD2_SCK_PCR_MUX	(2)
#endif

#ifdef SPI_MOD2_SOUT_PIN
#define SPI_MOD2_SOUT_PCR_MUX	(2)
#endif

#ifdef SPI_MOD2_SIN_PIN
#define SPI_MOD2_SIN_PCR_MUX	(2)
#endif

#ifdef SPI_MOD2_CS0_PIN
#define SPI_MOD2_CS0_PCR_MUX	(2)
#endif

#ifdef SPI_MOD2_CS1_PIN
#define SPI_MOD2_CS1_PCR_MUX	(2)
#endif

#endif
