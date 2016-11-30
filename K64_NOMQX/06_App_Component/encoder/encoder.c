//==========================================================================
//�ļ����ƣ�encoder.c
//���ܸ�Ҫ��������Ӧ����������Դ�ļ�
//==========================================================================

#include "encoder.h"

//������������FTMģ���
static const encoder_ftm_mod_table[] = { ENCODER0_FTM_MOD, ENCODER1_FTM_MOD };
//������������
static const encoder_dir_table[] = { ENCODER0_DIR, ENCODER1_DIR };

//==========================================================================
//��������: encoder_init
//��������: ��
//����˵��: encoder:��������:
//                 ENCODERx��xΪ��������;
//���ܸ�Ҫ: ��ʼ������������Ӧ������encoder.h��
//==========================================================================
void encoder_init(uint8 encoder) {
	uint8 mod;	//FTMģ���

	//��ȡFTMģ���
	mod = encoder_ftm_mod_table[encoder];

	//��ʼ��FTMģ�������Ϊ��������ģʽ
	ftm_init(mod, ENCODER_FTM_CLK_DIV, FTM_COUNTER_MODE_QD, 0);
	//��ʼ���������빦��
	ftm_qd_init(mod, ENCODER_MODE, encoder_dir_table[encoder]);
}

//==========================================================================
//��������: encoder_get
//��������: ��ǰ�ٶȣ���λm/s
//����˵��: encoder:��������:
//                 ENCODERx��xΪ��������;
//���ܸ�Ҫ: ��ȡ��������Ӧ��̥�ĵ�ǰ�ٶȣ���λm/s����Ӧ������encoder.h��
//��ע: �ú�����ÿ ENCODER_PERIOD ms����һ�Σ������ȡ���ٶȲ�׼ȷ;
//     �������������������
//==========================================================================
float encoder_get(uint8 encoder) {
	int16 count;	//��������������

	//��ȡ�������������������
	count = ftm_qd_get_and_clear_count(encoder_ftm_mod_table[encoder]);

	// ���ݳ��ְ뾶����ٶ�
//	*speed = (conter / (ENCODER_RES * 1.0))
//			* (6.28 * ENCODER_GEAR_R)/ ENCODER_PERIOD;			//2 * PI = 6.28
	return 0;
}
