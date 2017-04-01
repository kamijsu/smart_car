//==========================================================================
//�ļ����ƣ�frame.c
//���ܸ�Ҫ����֡ͨ��Э���������Դ�ļ�
//==========================================================================

#include "frame.h"
#include "crc16_modbus.h"
#include <string.h>

//�ڲ�����֡���������нṹ��
typedef struct {
	FrameInfo buffer[FRAME_BUFFER_SIZE];	//֡������
	uint8 front;		//�����±�
	uint8 rear;			//��β�±�
	uint8 num;			//�����е�֡����
	uint8 flag;			//���н��ձ�־
	uint8 recv_len;		//�Ѿ����յ��ֽ���
	uint16 last_crc;	//��һ�ε�CRCУ����
} FrameBufferQueue, *FrameBufferQueuePtr;

//��̬֡����������
static FrameBufferQueue buffer_queue[FRAME_BUFFER_NUM];
//������ַ
static uint8 local_addr;

//==========================================================================
//��������: frame_init
//��������: ��
//����˵��: addr:������ַ��������㲥��ַ��ͬ
//���ܸ�Ҫ: ʹ����֡ͨ��Э�飬��Ӧ������frame.h��
//==========================================================================
void frame_init(uint8 addr) {
	uint8 i;

	//���ñ�����ַ
	local_addr = addr;
	//��ʼ����֡����������
	for (i = 0; i < FRAME_BUFFER_NUM; ++i) {
		buffer_queue[i].front = 0;
		buffer_queue[i].rear = 0;
		buffer_queue[i].num = 0;
		buffer_queue[i].flag = 0;
		buffer_queue[i].recv_len = 0;
		buffer_queue[i].last_crc = CRC16_MODBUS_STREAM_INIT_CRC;
	}
}

//==========================================================================
//��������: frame_get_local_addr
//��������: ������ַ
//����˵��: ��
//���ܸ�Ҫ: ��ȡ������ַ
//==========================================================================
uint8 frame_get_local_addr() {
	return local_addr;
}

//==========================================================================
//��������: frame_info_to_frame
//��������: ��
//����˵��: info:֡��Ϣ�ṹ���ַ
//         frame:�洢֡�ֽڵ��׵�ַ
//         frame_len:�洢֡�ֽڳ��ȵĵ�ַ
//���ܸ�Ҫ: ��֡��Ϣ�ṹ��ת��Ϊ֡�ֽڣ��Զ����֡ͷ��֡У������
//��ע: ֡�ֽ���󳤶�Ϊ263���ֽڣ���˽���frame���鳤�ȴ��ڵ���263
//==========================================================================
void frame_info_to_frame(const FrameInfo* info, uint8* frame, uint16* frame_len) {
	uint16 crc;			//CRC16У����

	//����֡�ֽڳ���
	*frame_len = info->len + 8;

	//���֡ͷ
	frame[0] = FRAME_HEAD_LOW;
	frame[1] = FRAME_HEAD_HIGH;
	//���Ŀ�ĵ�ַ
	frame[2] = info->dest_addr;
	//���Դ��ַ
	frame[3] = info->src_addr;
	//�������
	frame[4] = info->type;
	//������ݳ���
	frame[5] = info->len;
	//�������
	memcpy(frame + 6, info->data, info->len);
	//���֡У������
	crc = crc16_modbus_block(frame + 2, info->len + 4);
	frame[*frame_len - 2] = (uint8) crc;
	frame[*frame_len - 1] = crc >> 8;
}

//==========================================================================
//��������: frame_frame_to_info
//��������: true:ת���ɹ�; false:ת��ʧ�ܣ��Ƿ�֡�ֽ�
//����˵��: frame:֡�ֽڵ��׵�ַ
//         frame_len:֡�ֽڳ���
//         info:�洢֡��Ϣ�ṹ��ĵ�ַ
//���ܸ�Ҫ: ��֡�ֽ�ת��Ϊ֡��Ϣ�ṹ�壬����֡�ֽڳ��ȡ�֡ͷ��֡У������
//==========================================================================
bool frame_frame_to_info(const uint8* frame, uint16 frame_len, FrameInfo* info) {
	uint16 crc;			//CRC16У����
	uint8 len;			//���ݳ���

	//��ȡ���ݳ���
	len = frame[5];
	//����֡�ֽڳ����Ƿ�Ϸ�
	if (frame_len - 8 != len) {
		return false;
	}
	//����֡ͷ�Ƿ�Ϸ�
	if (frame[0] != FRAME_HEAD_LOW || frame[1] != FRAME_HEAD_HIGH) {
		return false;
	}
	//����֡У�������Ƿ�Ϸ�
	crc = crc16_modbus_block(frame + 2, len + 4);
	if (frame[frame_len - 2] != (uint8) crc
			|| frame[frame_len - 1] != crc >> 8) {
		return false;
	}
	//���Ŀ�ĵ�ַ
	info->dest_addr = frame[2];
	//���Դ��ַ
	info->src_addr = frame[3];
	//�������
	info->type = frame[4];
	//������ݳ���
	info->len = len;
	//�������
	memcpy(info->data, frame + 6, len);
	//ת���ɹ�
	return true;
}

//==========================================================================
//��������: frame_framing
//��������: ��֡���:
//         FramingInvalidBuffer:��Ч��֡��������;
//         FramingFull:         ֡�����������ֽڱ�����;
//         FramingInvalidByte:  �յ���Ч�ֽ�;
//         FramingHeadLow:      �յ�֡ͷ���ֽ�;
//         FramingHeadHigh:     �յ�֡ͷ���ֽ�;
//         FramingDestAddr:     �յ�Ŀ�ĵ�ַ;
//         FramingSrcAddr:      �յ�Դ��ַ;
//         FramingType:         �յ�֡����;
//         FramingLen:          �յ����ݳ���;
//         FramingData:         �յ�����;
//         FramingFCS:          �յ�FCS���ֽ�;
//         FramingFail:         �յ�FCS���ֽڣ�У��ʧ�ܣ�֡������;
//         FramingOthers:       �յ�FCS���ֽڣ�Ŀ���ַ��Ϊ������㲥��ַ��֡������;
//         FramingSuccess:      �յ�FCS���ֽڣ��Ҹ�֡�Ϸ����������֡��������
//����˵��: buffer:֡�������ţ�ȡֵ��ΧΪ[0, FRAME_BUFFER_NUM - 1]
//         byte:���յ����ֽ�
//���ܸ�Ҫ: ��ָ����֡�������У�������յ����ֽڽ�����֡
//==========================================================================
FrameFramingResult frame_framing(uint8 buffer, uint8 byte) {
	FrameBufferQueuePtr queue;	//֡����������ָ��
	FrameFramingResult result;	//��֡���

	//��Ч��֡��������
	if (buffer >= FRAME_BUFFER_NUM) {
		return FramingInvalidBuffer;
	}
	//��ȡ֡����������ָ��
	queue = buffer_queue + buffer;
	//֡��������
	if (queue->num >= FRAME_BUFFER_SIZE) {
		return FramingFull;
	}
	//֡����������ʱ�����ݶ��б�־����Ӧ����
	switch (queue->flag) {
	case 0:		//δ�յ�֡ͷ���ֽ�
		//���յ�֡ͷ���ֽ�
		if (byte == FRAME_HEAD_LOW) {
			//���ı�־
			queue->flag = 1;
			result = FramingHeadLow;
		} else {
			//�յ���Ч�ֽ�
			result = FramingInvalidByte;
		}
		break;
	case 1:		//δ�յ�֡ͷ���ֽ�
		//���յ�֡ͷ���ֽ�
		if (byte == FRAME_HEAD_HIGH) {
			//���ı�־
			queue->flag = 2;
			result = FramingHeadHigh;
		} else {
			//�յ���Ч�ֽڣ����ñ�־
			queue->flag = 0;
			result = FramingInvalidByte;
		}
		break;
	case 2:		//δ�յ�Ŀ�ĵ�ַ
		//�յ�Ŀ�ĵ�ַ
		queue->buffer[queue->rear].dest_addr = byte;
		//����У��
		queue->last_crc = crc16_modbus_stream(queue->last_crc, byte);
		//���ı�־
		queue->flag = 3;
		result = FramingDestAddr;
		break;
	case 3:		//δ�յ�Դ��ַ
		//�յ�Դ��ַ
		queue->buffer[queue->rear].src_addr = byte;
		//����У��
		queue->last_crc = crc16_modbus_stream(queue->last_crc, byte);
		//���ı�־
		queue->flag = 4;
		result = FramingSrcAddr;
		break;
	case 4:		//δ�յ�֡����
		//�յ�֡����
		queue->buffer[queue->rear].type = byte;
		//����У��
		queue->last_crc = crc16_modbus_stream(queue->last_crc, byte);
		//���ı�־
		queue->flag = 5;
		result = FramingType;
		break;
	case 5:		//δ�յ����ݳ���
		//�յ����ݳ���
		queue->buffer[queue->rear].len = byte;
		//����У��
		queue->last_crc = crc16_modbus_stream(queue->last_crc, byte);
		//�������ݳ��ȸ��ı�־
		queue->flag = byte == 0 ? 7 : 6;
		result = FramingLen;
		break;
	case 6:		//δ����������
		//�յ�����
		queue->buffer[queue->rear].data[queue->recv_len++] = byte;
		//����У��
		queue->last_crc = crc16_modbus_stream(queue->last_crc, byte);
		//����Ƿ�������
		if (queue->recv_len >= queue->buffer[queue->rear].len) {
			//���ı�־
			queue->flag = 7;
			//�����Ѿ����յ��ֽ���
			queue->recv_len = 0;
		}
		result = FramingData;
		break;
	case 7:		//δ�յ�FCS���ֽ�
		//���FCS���ֽ��Ƿ�У����ȷ
		if ((uint8) queue->last_crc == byte) {
			//У��ɹ�
			queue->flag = 9;
		} else {
			//У��ʧ��
			queue->flag = 8;
		}
		result = FramingFCS;
		break;
	case 8:		//FCS���ֽ�У��ʧ��
		//��������FCS���ֽڣ����ø���̬����
		queue->flag = 0;
		queue->last_crc = CRC16_MODBUS_STREAM_INIT_CRC;
		result = FramingFail;
		break;
	case 9:		//FCS���ֽ�У��ɹ�
		//У��FCS���ֽ�
		if ((queue->last_crc >> 8) == byte) {
			//У��ɹ������Ŀ�ĵ�ַ�Ƿ�Ϊ������㲥��ַ
			if (queue->buffer[queue->rear].dest_addr
					== local_addr|| queue->buffer[queue->rear].dest_addr == FRAME_BROADCAST_ADDR) {
				//���Ļ��������ж�β
				queue->rear = (queue->rear + 1) % FRAME_BUFFER_SIZE;
				queue->num++;
				result = FramingSuccess;
			} else {
				result = FramingOthers;
			}
		} else {
			//У��ʧ��
			result = FramingFail;
		}
		//���ø���̬����
		queue->flag = 0;
		queue->last_crc = CRC16_MODBUS_STREAM_INIT_CRC;
		break;
	}
	//���ؽ��
	return result;
}

//==========================================================================
//��������: frame_get_info
//��������: true:��ȡ�ɹ�; false:��ȡʧ��
//����˵��: buffer:֡�������ţ�ȡֵ��ΧΪ[0, FRAME_BUFFER_NUM - 1]
//         info:�洢֡��Ϣ�ṹ��ĵ�ַ
//���ܸ�Ҫ: ָ��֡��������Ϊ��ʱ��ȡ�����ȴ洢��һ֡
//==========================================================================
bool frame_get_info(uint8 buffer, FrameInfo* info) {
	FrameBufferQueuePtr queue;	//֡����������ָ��

	//��Ч��֡��������
	if (buffer >= FRAME_BUFFER_NUM) {
		return false;
	}
	//��ȡ֡����������ָ��
	queue = buffer_queue + buffer;
	//��������Ϊ�գ���ȡʧ��
	if (queue->num == 0) {
		return false;
	}
	//����֡��Ϣ�ṹ��
	memcpy(info, queue->buffer + queue->front, sizeof(FrameInfo));
	//���Ļ��������ж���
	queue->num--;
	queue->front = (queue->front + 1) % FRAME_BUFFER_SIZE;
	//��ȡ�ɹ�
	return true;
}
