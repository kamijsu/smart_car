//==========================================================================
//�ļ����ƣ�frame.c
//���ܸ�Ҫ����֡ͨ��Э���������Դ�ļ�
//==========================================================================

#include "frame.h"
#include "crc16_modbus.h"

//�ڲ�����֡���������нṹ��
typedef struct {
	FrameInfo buffer[FRAME_BUFFER_SIZE];	//֡������
	uint8 front;	//�����±�
	uint8 rear;		//��β�±�
	uint8 num;		//�����е�֡����
} FrameBufferQueue;

//��̬֡���������У���ʼ�����ס���β�±꣬������֡����Ϊ0
static FrameBufferQueue buffer_queue = { { }, 0, 0, 0 };

//==========================================================================
//��������: frame_init
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: ʹ����֡ͨ��Э�飬��Ӧ������frame.h��
//==========================================================================
void frame_init() {
	//�������ó�ʼ����Ӧ��UARTģ��
	uart_init(FRAME_UART_MOD, FRAME_UART_BAUD, FRAME_UART_PARITY,
	FRAME_UART_STOP_BIT, FRAME_UART_BIT_ORDER);
}

//==========================================================================
//��������: frame_enable_re_int
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: ʹ����֡ͨ��Э������жϣ�����Ӧ��UART�����жϣ�ͨ����֡��������жϱ�־
//==========================================================================
void frame_enable_re_int() {
	uart_enable_re_int(FRAME_UART_MOD);
}

//==========================================================================
//��������: frame_disable_re_int
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: �ر���֡ͨ��Э������жϣ�����Ӧ��UART�����ж�
//==========================================================================
void frame_disable_re_int() {
	uart_disable_re_int(FRAME_UART_MOD);
}

//==========================================================================
//��������: frame_send_info
//��������: ��
//����˵��: info:Ҫ���͵�֡��Ϣ�ṹ��
//���ܸ�Ҫ: ����֡��Ϣ�ṹ�壬�Զ����֡ͷ��֡У������
//==========================================================================
void frame_send_info(FrameInfo info) {
	uint16 info_len;	//֡��Ϣ�ṹ����ֽ���
	uint8* info_ptr;	//֡��Ϣ�ṹ���׵�ַ
	uint16 crc;			//CRC16У����
	//֡��Ϣ�ṹ���ֽ���Ϊ�����ֽ���+2
	info_len = info.len + 2;
	//��ȡ֡��Ϣ�ṹ���׵�ַ
	info_ptr = (uint8*) &info;
	//����CRC16У����
	crc = crc16_modbus_block(info_ptr, info_len);
	//��˳����֡���ֶ�
	uart_send1(FRAME_UART_MOD, FRAME_HEAD_LOW);
	uart_send1(FRAME_UART_MOD, FRAME_HEAD_HIGH);
	uart_sendN(FRAME_UART_MOD, info_ptr, info_len);
	uart_send1(FRAME_UART_MOD, (uint8) crc);
	uart_send1(FRAME_UART_MOD, crc >> 8);
}

//==========================================================================
//��������: frame_framing
//��������: ��֡���:
//         FramingNone:    δ���յ��ֽ�;
//         FramingFull:    ֡�����������ֽڱ�����;
//         FramingInvalid: �յ���Ч�ֽ�;
//         FramingHeadLow: �յ�֡ͷ���ֽ�;
//         FramingHeadHigh:�յ�֡ͷ���ֽ�;
//         FramingType:    �յ�֡����;
//         FramingLen:     �յ����ݳ���;
//         FramingData:    �յ�����;
//         FramingFCS:     �յ�FCS���ֽ�;
//         FramingSuccess: �յ�FCS���ֽڣ���У��ɹ�;
//         FramingFail:    �յ�FCS���ֽڣ���У��ʧ�ܣ�֡������;
//����˵��: ��
//���ܸ�Ҫ: ��UART���ջ������ж�ȡһ���ֽ����ݣ���������֡�����֡����֡��������
//==========================================================================
FrameFramingResult frame_framing() {
	//��־��λ0��ʾ�����յ�֡ͷ���ֽڣ�λ1��ʾ�����յ�֡ͷ���ֽڣ�
	//λ2��ʾ�����յ����ͣ�λ3��ʾ�����յ����ݳ��ȣ�λ4��ʾ�Ƿ���������ݣ�
	//λ5��ʾ�����յ�FCS���ֽڣ�λ6��ʾFCS���ֽ��Ƿ�У��ɹ�
	static uint8 flags = 0;
	//�Ѿ����յ����ݳ���
	static uint8 received_len = 0;
	//��һ�ε�CRCУ����
	static uint16 last_crc = CRC16_MODBUS_STREAM_INIT_CRC;
	uint8 byte;	//���յ����ֽ�
	FrameFramingResult result;	//��֡���

	//�����ֽ�
	if (uart_re1(FRAME_UART_MOD, &byte)) {
		//�����ֽڳɹ�ʱ�����֡�������Ƿ�����
		if (buffer_queue.num >= FRAME_BUFFER_SIZE) {
			//֡��������
			result = FramingFull;
		} else {
			//֡����������ʱ�����ݱ�־����Ӧ����
			switch (flags) {
			case 0:		//δ�յ�֡ͷ���ֽ�
				//���յ�֡ͷ���ֽ�
				if (byte == FRAME_HEAD_LOW) {
					//���ı�־
					flags = 1;
					result = FramingHeadLow;
				} else {
					//�յ���Ч�ֽ�
					result = FramingInvalid;
				}
				break;
			case 1:		//δ�յ�֡ͷ���ֽ�
				//���յ�֡ͷ���ֽ�
				if (byte == FRAME_HEAD_HIGH) {
					//���ı�־
					flags = 3;
					result = FramingHeadHigh;
				} else {
					//�յ���Ч�ֽڣ����ñ�־
					flags = 0;
					result = FramingInvalid;
				}
				break;
			case 3:		//δ�յ�֡����
				//�յ�֡����
				buffer_queue.buffer[buffer_queue.rear].type = byte;
				//����У��
				last_crc = crc16_modbus_stream(last_crc, byte);
				//���ı�־
				flags = 7;
				result = FramingType;
				break;
			case 7:		//δ�յ����ݳ���
				//�յ����ݳ���
				buffer_queue.buffer[buffer_queue.rear].len = byte;
				//����У��
				last_crc = crc16_modbus_stream(last_crc, byte);
				//�������ݳ��ȸ��ı�־
				flags = byte == 0 ? 31 : 15;
				result = FramingLen;
				break;
			case 15:	//δ����������
				//�յ�����
				buffer_queue.buffer[buffer_queue.rear].data[received_len++] =
						byte;
				//����У��
				last_crc = crc16_modbus_stream(last_crc, byte);
				//����Ƿ�������
				if (received_len
						>= buffer_queue.buffer[buffer_queue.rear].len) {
					//���ı�־
					flags = 31;
				}
				result = FramingData;
				break;
			case 31:	//δ�յ�FCS���ֽ�
				//���FCS���ֽ��Ƿ�У����ȷ
				if ((uint8) last_crc == byte) {
					//У��ɹ�
					flags = 127;
				} else {
					//У��ʧ��
					flags = 63;
				}
				//�����Ѿ����յ����ݳ���
				received_len = 0;
				result = FramingFCS;
				break;
			case 63:	//FCS���ֽ�У��ʧ��
				//��������FCS���ֽڣ����ø���̬����
				flags = 0;
				last_crc = CRC16_MODBUS_STREAM_INIT_CRC;
				result = FramingFail;
				break;
			case 127:	//FCS���ֽ�У��ɹ�
				//У��FCS���ֽ�
				if ((last_crc >> 8) == byte) {
					//У��ɹ������Ļ��������ж�β
					buffer_queue.rear = (buffer_queue.rear + 1)
							% FRAME_BUFFER_SIZE;
					buffer_queue.num++;
					result = FramingSuccess;
				} else {
					//У��ʧ��
					result = FramingFail;
				}
				//���ø���̬����
				flags = 0;
				last_crc = CRC16_MODBUS_STREAM_INIT_CRC;
				break;
			}
		}
	} else {
		//δ���յ��ֽ�
		result = FramingNone;
	}
	//���ؽ��
	return result;
}

//==========================================================================
//��������: frame_get_info
//��������: true:��ȡ�ɹ�; false:��ȡʧ��
//����˵��: info:�洢֡��Ϣ�ṹ��ĵ�ַ
//���ܸ�Ҫ: ֡��������Ϊ��ʱ��ȡ�����ȴ洢��һ֡
//==========================================================================
bool frame_get_info(FrameInfo* info) {
	uint8 len;	//���ݳ���
	uint8 i;	//�α�
	//��������Ϊ�գ���ȡʧ��
	if (buffer_queue.num == 0) {
		return false;
	}
	//��ȡ���ݳ���
	len = buffer_queue.buffer[buffer_queue.front].len;
	//��ֵ
	info->type = buffer_queue.buffer[buffer_queue.front].type;
	info->len = len;
	for (i = 0; i < len; i++) {
		info->data[i] = buffer_queue.buffer[buffer_queue.front].data[i];
	}
	//���Ļ��������ж���
	buffer_queue.num--;
	buffer_queue.front = (buffer_queue.front + 1) % FRAME_BUFFER_SIZE;
	//��ȡ�ɹ�
	return true;
}
