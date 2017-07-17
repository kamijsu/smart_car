//==========================================================================
//�ļ����ƣ�param.c
//���ܸ�Ҫ��С�������������Դ�ļ�
//==========================================================================

#include "param.h"
#include "flash.h"

//����洢��flash�еĽṹ������
typedef struct {
	float angle_target_angle;
	float angle_pid_p;
	float angle_pid_d;

	float speed_target_speed;
	float speed_pid_p;
	float speed_pid_i;
	float speed_pid_d;

	float turn_pid_p;
	float turn_pid_d;
	float target_mid_point;
} ParamInFlash;

//�����ֽںͽṹ��ת����������
typedef union {
	ParamInFlash param;
	uint8 param_bytes[PARAM_FLASH_NUM];
} ParamUnion;

//����һ����̬������
static ParamUnion param_union;

//==========================================================================
//��������: param_init
//��������: ��
//����˵��: param���洢����ֵ���׵�ַ
//���ܸ�Ҫ: ������ʼ������ʼ������Ҫ��flash�ж��Ĳ���
//==========================================================================
void param_init(ParamCarPtr param) {
	//��ʼ��С������
	param->angle.angle = 0;
	param->angle.last_angle_speed = 0;
	param->angle.pwm.target_pwm = 0;
	param->angle.pwm.output_pwm = 0;
	param->angle.pwm.period_num = 1;
	param->speed.last_speed_err = 0;
	param->speed.distance_err = 0;
	param->speed.pwm.target_pwm = 0;
	param->speed.pwm.output_pwm = 0;
	param->speed.pwm.period_num = 10;
	param->turn.pwm.target_pwm = 0;
	param->turn.pwm.output_pwm = 0;
	param->turn.pwm.period_num = 5;
	param->turn.last_mid_err = 0;
}

//==========================================================================
//��������: param_get
//��������: ��
//����˵��: param���洢����ֵ���׵�ַ
//         mode��ѡ���mode�ײ���
//���ܸ�Ҫ: ��flash�ж�ȡ���в���ֵ����PARAM_FLASH_SECTOR + mode��������
//         ƫ��PARAM_FLASH_OFFSET��ʼ����ŵ�mode�ײ�����
//         ÿ�ײ�������PARAM_FLASH_NUM�ֽ�
//==========================================================================
void param_get(ParamCarPtr param, uint8 mode) {
	//��flash�ж�ȡ����ֵ���ṹ��
	flash_read(FLASH_BLK_DFLASH,
	PARAM_FLASH_SECTOR + mode,
	PARAM_FLASH_OFFSET,
	PARAM_FLASH_NUM, param_union.param_bytes);
	//���Ƶ��ڴ���
	param->angle.target_angle = param_union.param.angle_target_angle;
	param->angle.pid.p = param_union.param.angle_pid_p;
	param->angle.pid.d = param_union.param.angle_pid_d;
	param->speed.target_speed = param_union.param.speed_target_speed;
	param->speed.pid.p = param_union.param.speed_pid_p;
	param->speed.pid.i = param_union.param.speed_pid_i;
	param->speed.pid.d = param_union.param.speed_pid_d;

	param->turn.pid.p = param_union.param.turn_pid_p;
	param->turn.pid.d = param_union.param.turn_pid_d;
	param->turn.target_mid_point = param_union.param.target_mid_point;
}

//==========================================================================
//��������: param_update
//��������: ��
//����˵��: param���洢����ֵ���׵�ַ
//         mode��ѡ���mode�ײ���
//���ܸ�Ҫ: ���²���ֵ�������������޸Ĺ��Ĳ������浽flash��
//         ��PARAM_FLASH_SECTOR + mode��������
//         ƫ��PARAM_FLASH_OFFSET��ʼ����ŵ�mode�ײ�����
//         ÿ�ײ�������PARAM_FLASH_NUM�ֽ�
//==========================================================================
void param_update(ParamCarPtr param, uint8 mode) {
	param_union.param.angle_target_angle = param->angle.target_angle;
	param_union.param.angle_pid_p = param->angle.pid.p;
	param_union.param.angle_pid_d = param->angle.pid.d;

	param_union.param.speed_target_speed = param->speed.target_speed;
	param_union.param.speed_pid_p = param->speed.pid.p;
	param_union.param.speed_pid_i = param->speed.pid.i;
	param_union.param.speed_pid_d = param->speed.pid.d;

	param_union.param.turn_pid_p = param->turn.pid.p;
	param_union.param.turn_pid_d = param->turn.pid.d;
	param_union.param.target_mid_point = param->turn.target_mid_point;

	//�ر����ж�
	DISABLE_INTERRUPTS;
	//��������
	flash_erase_sector(FLASH_BLK_DFLASH, PARAM_FLASH_SECTOR + mode);
	//���������Ĳ���ֵ���浽flash��
	flash_write(FLASH_BLK_DFLASH,
	PARAM_FLASH_SECTOR + mode,
	PARAM_FLASH_OFFSET,
	PARAM_FLASH_NUM, param_union.param_bytes);
	//�������ж�
	ENABLE_INTERRUPTS;
}
