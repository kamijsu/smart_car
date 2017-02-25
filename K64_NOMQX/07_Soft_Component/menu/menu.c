//==========================================================================
//文件名称：menu.c
//功能概要：用户自定义软件程序源文件
//==========================================================================

#include "menu.h"
#include "oled.h"
#include "uart.h"
#include "stdlib.h"

static const uint8 menu_col = 0;	//菜单的起始列号
static const uint8 menu_page = 0;	//菜单的起始页号
static const uint8 max_row_number = 4;	//最大行号

static const uint8* parameters[6] = {"speed", "angle", "radius",
									 "length", "width", "height"};
static float values[6] = {1.1, 1.2, 1.3, 1.4, 1.5, 1.6};
static float modified_val = 0.0;
static uint8 start_idx = 0;	//OLED起始参数值索引
static uint8 select_idx = 0; //OLED选择标志索引

//==========================================================================
//函数名称: menu_oled_display
//函数返回: 无
//参数说明: 无
//功能概要: 显示菜单内容
//==========================================================================
void menu_oled_display()
{
	uint8 i;	//游标
	uint8 current_idx = 0; //OLED当前参数值索引

	//用0x00填充整个屏幕，即屏幕全暗
	oled_fill(0x00);
	current_idx = start_idx;
	for (i = 0; i < max_row_number && current_idx < 6; i++)
	{
		//输出参数名
		oled_printf(menu_col + 8, menu_page + i * 2, "%s:", parameters[current_idx]);
		//输出参数值
		oled_printf(menu_col + 80, menu_page + i * 2, "%.1f", values[current_idx++]);
	}
	oled_printf(menu_col, menu_page + (select_idx - start_idx) * 2, "%c", '*');
}

//==========================================================================
//函数名称: menu_oled_next_item
//函数返回: 无
//参数说明: 无
//功能概要: 移动到菜单下一选项
//==========================================================================
void menu_oled_next_item()
{
	if (select_idx < 5)
	{
		select_idx += 1;
		if (select_idx - start_idx >= max_row_number)
			start_idx += 1;
	}
	else;
	menu_oled_display();
}

//==========================================================================
//函数名称: menu_oled_last_item
//函数返回: 无
//参数说明: 无
//功能概要: 移动到菜单上一选项
//==========================================================================
void menu_oled_last_item()
{
	if (select_idx > 0)
	{
		select_idx -= 1;
		if (select_idx < start_idx)
			start_idx = select_idx;
	}
	else;
	menu_oled_display();
}

//==========================================================================
//函数名称: menu_oled_select
//函数返回: 无
//参数说明: 无
//功能概要: 进入菜单选择模式
//==========================================================================
void menu_oled_select()
{
	//用0x00填充整个屏幕，即屏幕全暗
	oled_fill(0x00);
	//输出参数名
	oled_printf(menu_col, menu_page, "%s:", parameters[select_idx]);
	//输出参数值
	oled_printf(menu_col + 80, menu_page, "%.1f", values[select_idx]);
	oled_printf(menu_col + 8, (max_row_number - 1) * 2, "%s", "modify:");
}

//==========================================================================
//函数名称: menu_oled_modify
//函数返回: 无
//参数说明: buff[8]:存放修改值的缓冲区数组
//功能概要: 修改选择的参数值
//==========================================================================
void menu_oled_modify(uint8 buff[8])
{
	modified_val = atof(buff);
	oled_printf(menu_col + 80, (max_row_number - 1) * 2, "%.1f", modified_val);
}

//==========================================================================
//函数名称: menu_oled_save
//函数返回: 无
//参数说明: 无
//功能概要: 保存菜单选择
//==========================================================================
void menu_oled_save()
{
	values[select_idx] = modified_val;
	menu_oled_display();
}

//==========================================================================
//函数名称: menu_oled_reset
//函数返回: 无
//参数说明: 无
//功能概要: 重置菜单和选择
//==========================================================================
void menu_oled_reset()
{
	start_idx = 0;
	select_idx = 0;
	menu_oled_display();
}
