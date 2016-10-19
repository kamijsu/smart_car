#include "breathe_light.h"

//======================================================================
//�������ƣ�breathe_light_init
//����������breath_light��BREATHE_LIGHT_GREEN|BREATHE_LIGHT_RED
//       duty��ռ�ձȣ�0-100��ֵԽ���Խ��
//�������أ���
//���ܸ�Ҫ��������������ʼ����
//======================================================================
void breathe_light_init(uint_16 breath_light, float duty)
{
	//��ʼ��TPMģ��
	tpm_timer_init(BREATHE_LIGHT_TPM, BREATHE_LIGHT_FREQ, BREATHE_LIGHT_CYCLE);
	//��ʼ��PWMͨ��
	pwm_init(breath_light, duty, BREATHE_LIGHT_ALIGN, BREATHE_LIGHT_POL);
}

//======================================================================
//�������ƣ�breathe_light_set
//����������breath_light��BREATHE_LIGHT_GREEN|BREATHE_LIGHT_RED
//       duty��ռ�ձȣ�0-100��ֵԽ���Խ��
//�������أ���
//���ܸ�Ҫ�����ú��������ȡ�
//======================================================================
void breathe_light_set(uint_16 breath_light, float duty)
{
	pwm_update(breath_light, duty);
}

//======================================================================
//�������ƣ�breathe_light_get
//����������breath_light��BREATHE_LIGHT_GREEN|BREATHE_LIGHT_RED
//�������أ�����������
//���ܸ�Ҫ����ȡ�����Ƶ�ǰ���ȡ�
//======================================================================
float breathe_light_get(uint_16 breath_light)
{
	return pwm_get(breath_light);
}
