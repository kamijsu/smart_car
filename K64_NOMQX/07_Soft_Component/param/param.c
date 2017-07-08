//==========================================================================
//文件名称：param.c
//功能概要：小车参数软件程序源文件
//==========================================================================

#include "param.h"
#include "flash.h"

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
} ParamInFlash;
typedef union {
	ParamInFlash param;
	uint8 param_bytes[PARAM_FLASH_NUM];
} ParamUnion;
static ParamUnion param_union;

//==========================================================================
//函数名称: param_init
//函数返回: 无
//参数说明: param：存储参数值的首地址
//功能概要: 参数初始化，从flash中读取所有参数值
//==========================================================================
void param_init(ParamCarPtr param) {
	//初始化小车参数
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
//函数名称: param_get
//函数返回: 无
//参数说明: param：存储参数值的首地址
//         mode：选择第mode套参数
//功能概要: 从flash中读取所有参数值，第PARAM_FLASH_SECTOR + mode号扇区的
//         偏移PARAM_FLASH_OFFSET开始，存放第mode套参数，
//         每套参数长度PARAM_FLASH_NUM字节
//==========================================================================
void param_get(ParamCarPtr param, uint8 mode) {
	//从flash中读取参数值到结构体
	flash_read(FLASH_BLK_DFLASH,
	PARAM_FLASH_SECTOR + mode,
	PARAM_FLASH_OFFSET,
	PARAM_FLASH_NUM, param_union.param_bytes);
	//0m/s时pid参数，以下从flash读
	param->angle.target_angle = param_union.param.angle_target_angle;
	param->angle.pid.p = param_union.param.angle_pid_p;
	param->angle.pid.d = param_union.param.angle_pid_d;
	param->speed.target_speed = param_union.param.speed_target_speed;
	param->speed.pid.p = param_union.param.speed_pid_p;
	param->speed.pid.i = param_union.param.speed_pid_i;
	param->speed.pid.d = param_union.param.speed_pid_d;

	param->turn.pid.p = param_union.param.turn_pid_p;
	param->turn.pid.d = param_union.param.turn_pid_d;
}

//==========================================================================
//函数名称: param_update
//函数返回: 无
//参数说明: param：存储参数值的首地址
//         mode：选择第mode套参数
//功能概要: 更新参数值，将缓冲区中修改过的参数保存到flash中
//         第PARAM_FLASH_SECTOR + mode号扇区的
//         偏移PARAM_FLASH_OFFSET开始，存放第mode套参数，
//         每套参数长度PARAM_FLASH_NUM字节
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

	//关闭总中断
	DISABLE_INTERRUPTS;
	flash_erase_sector(FLASH_BLK_DFLASH, PARAM_FLASH_SECTOR + mode);
	//将缓冲区的参数值保存到flash中
	flash_write(FLASH_BLK_DFLASH,
	PARAM_FLASH_SECTOR + mode,
	PARAM_FLASH_OFFSET,
	PARAM_FLASH_NUM, param_union.param_bytes);
	ENABLE_INTERRUPTS;
}
