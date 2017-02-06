//==========================================================================
//文件名称：custom.c
//功能概要：用户自定义软件程序源文件
//==========================================================================

#include "custom.h"
#include "oled.h"
#include "temp_sensor.h"

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
