//==========================================================================
//文件名称：menu.c
//功能概要：用户自定义软件程序源文件
//==========================================================================

#include "menu.h"
#include "oled.h"
#include "uart.h"

static const uint8 menu_col = 0;	//"菜单"的起始列号
static const uint8 menu_page = 0;	//"菜单"的起始页号
static const uint8 max_row_number = 4;	//最大行号

static const char menu_name[] = "speed";
static const float option[6] = {1.1, 1.2, 1.3, 1.4, 1.5, 1.6};
static uint8 start_idx = 0;	//OLED起始参数值索引
static uint8 select_idx = 0; //OLED选择标志索引
static bool sel_flag = false; //选择标志位

void menu_oled_display()
{
	uint8 i;	//游标
	uint8 current_idx = 0; //OLED当前参数值索引

	//用0x00填充整个屏幕，即屏幕全暗
	oled_fill(0x00);
	current_idx = start_idx;
	//输出参数名
	oled_printf(menu_col, menu_page, "%s", menu_name);
	if (true == sel_flag)
		oled_printf(menu_col, menu_page + (select_idx - start_idx + 1) * 2, "%c", '*');
	for (i = 1; i < max_row_number && current_idx < 6; i++)
		oled_printf(menu_col + 8, menu_page + i * 2,
						"%.1f", option[current_idx++]);
}
void menu_oled_next_item()
{
	if (true == sel_flag && select_idx < 5)
	{
		select_idx += 1;
		if (select_idx - start_idx >= max_row_number - 1)
			start_idx += 1;
	}
	else if (start_idx < 5)
		start_idx += 1;
	else;
	menu_oled_display();
}
void menu_oled_last_item()
{
	if (true == sel_flag && select_idx > 0)
	{
		select_idx -= 1;
		if (select_idx < start_idx)
			start_idx = select_idx;
	}
	else if (start_idx > 0)
		start_idx -= 1;
	else;
	menu_oled_display();
}
void menu_oled_select()
{
	if (false == sel_flag)
	{
		sel_flag = true;
		start_idx = 0;
		select_idx = 0;
		menu_oled_display();
	}
}
void menu_oled_save()
{
	sel_flag = false;
	//用0x00填充整个屏幕，即屏幕全暗
	oled_fill(0x00);
	oled_printf(menu_col, menu_page, "%s = %.1f",
			menu_name, option[select_idx]);
}
void menu_oled_reset()
{
	sel_flag = false;
	start_idx = 0;
	select_idx = 0;
	menu_oled_display();
}
