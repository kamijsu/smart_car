//==========================================================================
//�ļ����ƣ�param.h
//���ܸ�Ҫ��С�������������ͷ�ļ�
//==========================================================================

#ifndef _PARAM_H
#define _PARAM_H

#include "common.h"
#include "camera.h"

#define PARAM_FLASH_SECTOR (20) //flash�д�Ų�����������
#define PARAM_FLASH_OFFSET (0)  //flash�д�Ų�����������ƫ�Ƶ�ַ
#define PARAM_FLASH_NUM	   (40) //flash�в������ֽ���

//����PID�����ṹ��
typedef struct {
	float p;	//��������
	float i;	//���ֿ���
	float d;	//΢�ֿ���
} ParamPID, *ParamPIDPtr;

//����PWM�����ṹ��
typedef struct {
	int16 target_pwm;	//Ŀ��PWMֵ�����ʼ��
	int16 output_pwm;	//���������PWMֵ�����ʼ��
	uint8 period_counter;	//���ڼ�����
	uint8 period_num;	//���PWMֵ��ΪĿ��PWMֵ�����������������Ϊ1�����ʼ��
} ParamPWM, *ParamPWMPtr;

//����Ƕȿ���ģ�����
typedef struct {
	float angle;		//��ǰ�Ƕȣ���λ�㣬���ʼ��
	float angle_speed;	//��ǰ���ٶȣ���λ��/s
	float last_angle_speed;	//�ϴν��ٶȣ���λ��/s�����ʼ��
	float target_angle;	//Ŀ��Ƕȣ���λ�㣬���ʼ��
	ParamPID pid;		//PID���������ʼ��
	ParamPWM pwm;		//PWM���������ʼ��
} ParamAngle, *ParamAnglePtr;

//�����ٶȿ���ģ�����
typedef struct {
	float left_speed;	//�����ٶȣ���λm/s
	float right_speed;	//�����ٶȣ���λm/s
	float target_speed;	//Ŀ���ٶȣ���λm/s�����ʼ��
	float last_speed_err;	//�ϴ��ٶȲ�ֵ����λm/s�����ʼ��
	float distance_err;	//��������λm�����ʼ��
	ParamPID pid;		//PID���������ʼ��
	ParamPWM pwm;		//PWM���������ʼ��
} ParamSpeed, *ParamSpeedPtr;

//���巽�����ģ�����
typedef struct {
	uint8 raw_img[CAMERA_RAW_IMG_BYTES];	//ԭʼͼ��
	uint8 img[CAMERA_IMG_HEIGHT][CAMERA_IMG_WIDTH];	//��ѹ��ͼ������
	int16 mid_points[CAMERA_IMG_HEIGHT];
	int16 left_edges[CAMERA_IMG_HEIGHT];
	int16 right_edges[CAMERA_IMG_HEIGHT];
	bool valid_row[CAMERA_IMG_HEIGHT];
	float avg_mid_point;
	int16 returnBuff[6];
	float midpoint;
	float last_mid_err;	//���ʼ��
	ParamPID pid;		//PID���������ʼ��
	ParamPWM pwm;		//PWM���������ʼ��
} ParamTurn, *ParamTurnPtr;

//����С������
typedef struct {
	ParamAngle angle;	//�Ƕȿ��Ʋ���
	ParamSpeed speed;	//�ٶȿ��Ʋ���
	ParamTurn turn;	//������Ʋ���
	int16 left_motor_pwm;	//�������PWMֵ
	int16 right_motor_pwm;	//�ҵ�����PWMֵ
} ParamCar, *ParamCarPtr;

//==========================================================================
//��������: param_init
//��������: ��
//����˵��: param���洢����ֵ���׵�ַ
//���ܸ�Ҫ: ������ʼ������ʼ������Ҫ��flash�ж��Ĳ���
//==========================================================================
void param_init(ParamCarPtr param);

//==========================================================================
//��������: param_get
//��������: ��
//����˵��: param���洢����ֵ���׵�ַ
//         mode��ѡ���mode�ײ���
//���ܸ�Ҫ: ��flash�ж�ȡ���в���ֵ����PARAM_FLASH_SECTOR + mode��������
//         ƫ��PARAM_FLASH_OFFSET��ʼ����ŵ�mode�ײ�����
//         ÿ�ײ�������PARAM_FLASH_NUM�ֽ�
//==========================================================================
void param_get(ParamCarPtr param, uint8 mode);

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
void param_update(ParamCarPtr param, uint8 mode);

#endif
