#ifndef _BREATHE_LIGHT_H
#define _BREATHE_LIGHT_H

#include "common.h"
#include "tpm.h"

//�������������TPMģ���
#define BREATHE_LIGHT_TPM		TPM_2

//�������������TPMģ��ļ�����Ƶ��
#define BREATHE_LIGHT_FREQ		(3000)

//�������������TPMģ��ļ������ڣ���λΪms
//����Ϊ�����жϣ�Ƶ�������ڲ���û��̫���壬������TPMģ���ģ���Ĵ���MOD����16λ������Ƶ�������ڳ˻����ܳ���65535��
#define BREATHE_LIGHT_CYCLE		(20)

//���������PWMͨ�����䷽ʽ
#define BREATHE_LIGHT_ALIGN		PWM_CENTER

//���������PWMͨ�����ԣ������ø����ԣ����Զ�Ӧ��ռ�ձ�Խ�ͣ���Խ����
#define BREATHE_LIGHT_POL		PWM_MINUS

//���������ռ�ձȵ����ֵ����Сֵ
#define BREATHE_LIGHT_MAX_DUTY	PWM_MAX
#define BREATHE_LIGHT_MIN_DUTY	PWM_MIN

//����ÿյ����������ͨ����
#define BREATHE_LIGHT_GREEN		TPM2_CH0
#define BREATHE_LIGHT_RED		TPM2_CH1

//======================================================================
//�������ƣ�breathe_light_init
//����������breath_light��BREATHE_LIGHT_GREEN|BREATHE_LIGHT_RED
//       duty��ռ�ձȣ�0-100��ֵԽ���Խ��
//�������أ���
//���ܸ�Ҫ��������������ʼ����
//======================================================================
void breathe_light_init(uint_16 breath_light, float duty);

//======================================================================
//�������ƣ�breathe_light_set
//����������breath_light��BREATHE_LIGHT_GREEN|BREATHE_LIGHT_RED
//       duty��ռ�ձȣ�0-100��ֵԽ���Խ��
//�������أ���
//���ܸ�Ҫ�����ú��������ȡ�
//======================================================================
void breathe_light_set(uint_16 breath_light, float duty);

//======================================================================
//�������ƣ�breathe_light_get
//����������breath_light��BREATHE_LIGHT_GREEN|BREATHE_LIGHT_RED
//�������أ�����������
//���ܸ�Ҫ����ȡ�����Ƶ�ǰ���ȡ�
//======================================================================
float breathe_light_get(uint_16 breath_light);

#endif
