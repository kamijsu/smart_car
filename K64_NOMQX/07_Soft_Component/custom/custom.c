//==========================================================================
//文件名称：custom.c
//功能概要：用户自定义软件程序源文件
//==========================================================================

#include "custom.h"
#include "oled.h"
#include "temp_sensor.h"
#include "camera.h"
#include "frame.h"
#include <string.h>

//"温度"的字模数据，纵向取模，字节倒序;
//汉字大小为16*16，前32字节为"温度"上半部分，后32字节为"温度"下半部分
static const uint8 temp_char_lib[64] = { 0x10, 0x60, 0x02, 0x8C, 0x00, 0x00,
		0xFE, 0x92, 0x92, 0x92, 0x92, 0x92, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00,
		0xFC, 0x24, 0x24, 0x24, 0xFC, 0x25, 0x26, 0x24, 0xFC, 0x24, 0x24, 0x24,
		0x04, 0x00, 0x04, 0x04, 0x7E, 0x01, 0x40, 0x7E, 0x42, 0x42, 0x7E, 0x42,
		0x7E, 0x42, 0x42, 0x7E, 0x40, 0x00, 0x40, 0x30, 0x8F, 0x80, 0x84, 0x4C,
		0x55, 0x25, 0x25, 0x25, 0x55, 0x4C, 0x80, 0x80, 0x80, 0x00, };
//"°C"的字模数据，纵向取模，字节倒序，符号大小为16*16
static const uint8 degree_centigrade_char_lib[32] = { 0x06, 0x09, 0x09, 0xE6,
		0xF8, 0x0C, 0x04, 0x02, 0x02, 0x02, 0x02, 0x02, 0x04, 0x1E, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x07, 0x1F, 0x30, 0x20, 0x40, 0x40, 0x40, 0x40, 0x40,
		0x20, 0x10, 0x00, 0x00 };
static const uint8 temp_col = 0;	//"温度"的起始列号
static const uint8 temp_page = 0;	//"温度"的起始页号
static const uint8 temp_width = 4;	//温度值的宽度

//==========================================================================
//函数名称: custom_oled_display_init
//函数返回: 无
//参数说明: 无
//功能概要: 初始化OLED显示内容
//==========================================================================
void custom_oled_display_init() {
	uint8 i;	//游标

	//显示"温度"
	oled_set_pos(temp_col, temp_page);
	for (i = 0; i < 32; i++) {
		oled_write_data(temp_char_lib[i]);
	}
	oled_set_pos(temp_col, temp_page + 1);
	for (; i < 64; i++) {
		oled_write_data(temp_char_lib[i]);
	}
	//显示":"
	oled_display_str(temp_col + 32, temp_page, ":");
	//显示"°C"
	oled_set_pos(temp_col + 40 + temp_width * 8, temp_page);
	for (i = 0; i < 16; i++) {
		oled_write_data(degree_centigrade_char_lib[i]);
	}
	oled_set_pos(temp_col + 40 + temp_width * 8, temp_page + 1);
	for (; i < 32; i++) {
		oled_write_data(degree_centigrade_char_lib[i]);
	}
	//显示初始的温度值
	custom_oled_update_temp();
}

//==========================================================================
//函数名称: custom_oled_update_temp
//函数返回: 无
//参数说明: 无
//功能概要: 更新OLED显示温度值
//==========================================================================
void custom_oled_update_temp() {
	oled_printf(temp_col + 40, temp_page, "%*.1f", temp_width,
			temp_sensor_get_temp());
}

//==========================================================================
//函数名称: custom_oled_show_img
//函数返回: 无
//参数说明: img:宽*高为80*60的图像首地址
//功能概要: 在OLED上显示一幅80*60的二值化图像
//备注: 显示的起始位置为第24列，第1页的第4行;
//     图像上方的4行被设置为全暗，但中间2列被设置为亮，用于指示图像的中心线;
//     显示一幅图像需1ms
//==========================================================================
void custom_oled_show_img(uint8 img[60][80]) {
	uint8 i, j, k;	//游标
	uint8 offset;	//行偏移量
	uint8 byte;		//要发送的字节

	//显示图像的最低4行，显示于OLED最低页的高4行
	//设置当前地址为起始列
	oled_set_pos(24, 0);
	//显示前39列
	for (i = 0; i < 39; i++) {
		//默认全暗
		byte = 0;
		//若4行中有白像素点，设置该位为亮
		for (j = 0; j < 4; j++) {
			if (img[j][i] != CAMERA_EXTRACT_BLACK_VAL) {
				byte |= 1 << j + 4;
			}
		}
		//发送该字节至OLED
		oled_write_data(byte);
	}

	//显示中间2列
	for (; i < 41; i++) {
		//默认OLED低4行为亮
		byte = 0xF;
		for (j = 0; j < 4; j++) {
			if (img[j][i] != CAMERA_EXTRACT_BLACK_VAL) {
				byte |= 1 << j + 4;
			}
		}
		oled_write_data(byte);
	}

	//显示后39列
	for (; i < 80; i++) {
		byte = 0;
		for (j = 0; j < 4; j++) {
			if (img[j][i] != CAMERA_EXTRACT_BLACK_VAL) {
				byte |= 1 << j + 4;
			}
		}
		oled_write_data(byte);
	}

	//显示剩余7页图像
	for (i = 0; i < 7; i++) {
		//计算行偏移量
		offset = 8 * i + 4;
		//设置当前地址为起始列
		oled_set_pos(24, i + 1);
		//显示该页的80列图像
		for (j = 0; j < 80; j++) {
			byte = 0;
			//若8行中有白像素点，设置该位为亮
			for (k = 0; k < 8; k++) {
				if (img[k + offset][j] != CAMERA_EXTRACT_BLACK_VAL) {
					byte |= 1 << k;
				}
			}
			oled_write_data(byte);
		}
	}
}

//==========================================================================
//函数名称: custom_send_string_to_host
//函数返回: true:发送成功; false:发送失败，即字符串长度大于等于255个字节
//参数说明: str:字符串的首地址
//功能概要: 发送字符串至上位机
//备注: 组帧发送
//==========================================================================
bool custom_send_string_to_host(const uint8* str) {
	FrameInfo info;		//帧信息结构体
	uint8 frame[263];	//帧字节
	uint16 frame_len;	//帧字节数
	uint32 str_len;		//字符串长度

	str_len = strlen(str);
	//判断字符串长度是否合法
	if (str_len >= 255) {
		return false;
	}
	//设置目的地址、源地址、类型、长度
	info.dest_addr = CUSTOM_HOST_ADDR;
	info.src_addr = frame_get_local_addr();
	info.type = CUSTOM_FRAME_TYPE_STRING;
	info.len = str_len;
	//拷贝字符串
	strcpy(info.data, str);
	//发送字符串帧
	frame_info_to_frame(&info, frame, &frame_len);
	uart_sendN(CUSTOM_UART_MOD, frame, frame_len);
	return true;
}

//==========================================================================
//函数名称: custom_send_param_to_host
//函数返回: 无
//参数说明: car:小车参数的地址
//功能概要: 发送小车参数至上位机
//备注: 组帧发送
//==========================================================================
void custom_send_param_to_host(const ParamCar* car) {
	FrameInfo info;		//帧信息结构体
	uint8 frame[263];	//帧字节
	uint16 frame_len;	//帧字节数

	//设置目的地址、源地址、类型、长度
	info.dest_addr = CUSTOM_HOST_ADDR;
	info.src_addr = frame_get_local_addr();
	info.type = CUSTOM_FRAME_TYPE_PARAM;
	info.len = sizeof(float) * 16 + sizeof(int16) * 5;
	//设置发送的数据
	memcpy(info.data, &car->angle.angle, sizeof(float));
	memcpy(info.data + sizeof(float), &car->angle.angle_speed, sizeof(float));
	memcpy(info.data + sizeof(float) * 2, &car->angle.last_angle_speed,
			sizeof(float));
	memcpy(info.data + sizeof(float) * 3, &car->angle.target_angle,
			sizeof(float));
	memcpy(info.data + sizeof(float) * 4, &car->angle.pid.p, sizeof(float));
	memcpy(info.data + sizeof(float) * 5, &car->angle.pid.d, sizeof(float));
	memcpy(info.data + sizeof(float) * 6, &car->angle.pwm.target_pwm,
			sizeof(int16));
	memcpy(info.data + sizeof(float) * 6 + sizeof(int16),
			&car->speed.left_speed, sizeof(float));
	memcpy(info.data + sizeof(float) * 7 + sizeof(int16),
			&car->speed.right_speed, sizeof(float));
	memcpy(info.data + sizeof(float) * 8 + sizeof(int16),
			&car->speed.target_speed, sizeof(float));
	memcpy(info.data + sizeof(float) * 9 + sizeof(int16),
			&car->speed.last_speed_err, sizeof(float));
	memcpy(info.data + sizeof(float) * 10 + sizeof(int16),
			&car->speed.distance_err, sizeof(float));
	memcpy(info.data + sizeof(float) * 11 + sizeof(int16), &car->speed.pid.p,
			sizeof(float));
	memcpy(info.data + sizeof(float) * 12 + sizeof(int16), &car->speed.pid.i,
			sizeof(float));
	memcpy(info.data + sizeof(float) * 13 + sizeof(int16), &car->speed.pid.d,
			sizeof(float));
	memcpy(info.data + sizeof(float) * 14 + sizeof(int16),
			&car->speed.pwm.target_pwm, sizeof(int16));
	memcpy(info.data + sizeof(float) * 14 + sizeof(int16) * 2, &car->turn.pid.p,
			sizeof(float));
	memcpy(info.data + sizeof(float) * 15 + sizeof(int16) * 2, &car->turn.pid.d,
			sizeof(float));
	memcpy(info.data + sizeof(float) * 16 + sizeof(int16) * 2,
			&car->turn.pwm.target_pwm, sizeof(int16));
	memcpy(info.data + sizeof(float) * 16 + sizeof(int16) * 3,
			&car->left_motor_pwm, sizeof(int16));
	memcpy(info.data + sizeof(float) * 16 + sizeof(int16) * 4,
			&car->right_motor_pwm, sizeof(int16));
	//发送帧
	frame_info_to_frame(&info, frame, &frame_len);
	uart_sendN(CUSTOM_UART_MOD, frame, frame_len);
}

//==========================================================================
//函数名称: custom_send_raw_img_to_host
//函数返回: 无
//参数说明: raw_img:600个字节的原始图片首地址
//功能概要: 发送原始图片至上位机
//备注: 组帧发送，共3帧
//==========================================================================
void custom_send_raw_img_to_host(const uint8 raw_img[600]) {
	FrameInfo info;		//帧信息结构体
	uint8 frame[263];	//帧字节
	uint16 frame_len;	//帧字节数

	//设置目的地址、源地址、长度
	info.dest_addr = CUSTOM_HOST_ADDR;
	info.src_addr = frame_get_local_addr();
	info.len = 200;

	//发送图片帧0
	info.type = CUSTOM_FRAME_TYPE_RAW_IMG0;
	memcpy(info.data, raw_img, 200);
	frame_info_to_frame(&info, frame, &frame_len);
	uart_sendN(CUSTOM_UART_MOD, frame, frame_len);
	//发送图片帧1
	info.type = CUSTOM_FRAME_TYPE_RAW_IMG1;
	memcpy(info.data, raw_img + 200, 200);
	frame_info_to_frame(&info, frame, &frame_len);
	uart_sendN(CUSTOM_UART_MOD, frame, frame_len);
	//发送图片帧2
	info.type = CUSTOM_FRAME_TYPE_RAW_IMG2;
	memcpy(info.data, raw_img + 400, 200);
	frame_info_to_frame(&info, frame, &frame_len);
	uart_sendN(CUSTOM_UART_MOD, frame, frame_len);
}

//==========================================================================
//函数名称: custom_send_mid_points_to_host
//函数返回: 无
//参数说明: mid_points:60个字节的中点数组的首地址
//         has_mid_points:60个字节的是否有中点数组的首地址
//功能概要: 发送中点信息至上位机
//备注: 组帧发送
//==========================================================================
void custom_send_mid_points_to_host(const uint8 mid_points[60],
		const bool has_mid_points[60]) {
	FrameInfo info;		//帧信息结构体
	uint8 frame[263];	//帧字节
	uint16 frame_len;	//帧字节数
	uint8 i;

	//设置目的地址、源地址、类型、长度
	info.dest_addr = CUSTOM_HOST_ADDR;
	info.src_addr = frame_get_local_addr();
	info.type = CUSTOM_FRAME_TYPE_MID_POINT;
	info.len = 0;
	//设置发送的数据，每个中点占两个字节，第一个为行号，第二个为列号
	for (i = 0; i < 60; ++i) {
		if (has_mid_points[i]) {
			info.data[info.len++] = i;
			info.data[info.len++] = mid_points[i];
		}
	}
	//发送帧
	frame_info_to_frame(&info, frame, &frame_len);
	uart_sendN(CUSTOM_UART_MOD, frame, frame_len);
}

//==========================================================================
//函数名称: custom_send_edges_to_host
//函数返回: 无
//参数说明: left_edges:60个字节的左边缘数组的首地址
//         has_left_edges:60个字节的是否有左边缘数组的首地址
//         right_edges:60个字节的右边缘数组的首地址
//         has_right_edges:60个字节的是否有右边缘数组的首地址
//功能概要: 发送边缘信息至上位机
//备注: 组帧发送
//==========================================================================
void custom_send_edges_to_host(const uint8 left_edges[60],
		const bool has_left_edges[60], const uint8 right_edges[60],
		const bool has_right_edges[60]) {
	FrameInfo info;		//帧信息结构体
	uint8 frame[263];	//帧字节
	uint16 frame_len;	//帧字节数
	uint8 i;

	//设置目的地址、源地址、类型、长度
	info.dest_addr = CUSTOM_HOST_ADDR;
	info.src_addr = frame_get_local_addr();
	info.type = CUSTOM_FRAME_TYPE_EDGE;
	info.len = 0;
	//设置发送的数据，每个边缘点占两个字节，第一个为行号，第二个为列号
	for (i = 0; i < 60; ++i) {
		if (has_left_edges[i]) {
			info.data[info.len++] = i;
			info.data[info.len++] = left_edges[i];
		}
		if (has_right_edges[i]) {
			info.data[info.len++] = i;
			info.data[info.len++] = right_edges[i];
		}
	}
	//发送帧
	frame_info_to_frame(&info, frame, &frame_len);
	uart_sendN(CUSTOM_UART_MOD, frame, frame_len);
}
