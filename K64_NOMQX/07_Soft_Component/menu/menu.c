//==========================================================================
//文件名称：menu.c
//功能概要：用户自定义软件程序源文件
//==========================================================================

#include "menu.h"
#include "oled.h"
#include "uart.h"
#include <stdio.h>
#include <string.h>

static uint8 buf[BUF_SIZE];   // 输入字符缓冲区
static uint8 pos = 0;

typedef struct
{
	uint8* name;
	float value;
} item;
typedef struct
{
	item items[6];
	uint8 num_of_params;
	uint8 srt_idx;      //OLED起始参数值索引
	uint8 sel_idx;      //OLED选择标志索引
	bool sel_flag;      //是否处于选择状态
} menu;
menu list;

//内部函数声明
void menu_oled_display_list(void);
void menu_oled_last_item(void);
void menu_oled_next_item(void);
void menu_oled_select(void);
void menu_oled_backspace();
void menu_oled_save(void);
void menu_oled_reset(void);

//==========================================================================
//函数名称: menu_oled_init
//函数返回: 无
//参数说明: 无
//功能概要: 菜单内容初始化
//==========================================================================
void menu_oled_init(void)
{
	item speed  = {"speed" , 1.1};
	item angle	= {"angle" , 1.2};
	item radius = {"radius", 1.3};
	item length = {"length", 1.4};
	item width	= {"width" , 1.5};
	item height = {"height", 1.6};

	list.items[0] = speed;
	list.items[1] = angle;
	list.items[2] = radius;
	list.items[3] = length;
	list.items[4] = width;
	list.items[5] = height;
	list.num_of_params = 6;
	list.srt_idx = 0;
	list.sel_idx = 0;
	list.sel_flag = false; //设置为非选择状态
	buf[0] = '\0';
	pos = 0;
	menu_oled_display_list();
}

//==========================================================================
//函数名称: menu_oled_display_list
//函数返回: 无
//参数说明: 无
//功能概要: 显示菜单内容
//==========================================================================
void menu_oled_display_list(void)
{
    uint8 i;    //游标
    uint8 cur_idx = list.srt_idx; //OLED当前参数值索引

    //用0x00填充整个屏幕，即屏幕全暗
    oled_fill(0x00);
    for (i = 0; i < MAX_ROW && cur_idx < list.num_of_params; i++)
    {
        //输出参数名
        oled_printf(STARTING_COL + 8, STARTING_PAGE + i * 2,
        		    "%s:", list.items[cur_idx].name);
        //输出参数值
        oled_printf(STARTING_COL + 80, STARTING_PAGE + i * 2,
        		    "%.4f", list.items[cur_idx++].value);
    }
    oled_printf(STARTING_COL, STARTING_PAGE + (list.sel_idx - list.srt_idx) * 2,
				"%c", '*');
}

//==========================================================================
//函数名称: menu_oled_display_item
//函数返回: 无
//参数说明: 无
//功能概要: 显示指定的选项的名称和值
//==========================================================================
void menu_oled_display_item(void)
{
	//用0x00填充整个屏幕，即屏幕全暗
	oled_fill(0x00);
	//输出参数名
	oled_printf(STARTING_COL, STARTING_PAGE,
				"%s:", list.items[list.sel_idx].name);
	//输出参数值
	oled_printf(STARTING_COL + 80, STARTING_PAGE,
				"%f", list.items[list.sel_idx].value);
	oled_printf(STARTING_COL, STARTING_PAGE + 2, "new: ");
}

//==========================================================================
//函数名称: menu_oled_next_item
//函数返回: 无
//参数说明: 无
//功能概要: 移动到菜单下一选项
//==========================================================================
void menu_oled_next_item(void)
{
	if (list.sel_flag == true) return; //处于选择状态则返回
    if (list.sel_idx < 5)
    {
    	list.sel_idx += 1;
        if (list.sel_idx - list.srt_idx >= MAX_ROW)
        	list.srt_idx += 1;
    }
    menu_oled_display_list();
}

//==========================================================================
//函数名称: menu_oled_last_item
//函数返回: 无
//参数说明: 无
//功能概要: 移动到菜单上一选项
//==========================================================================
void menu_oled_last_item(void)
{
	if (list.sel_flag == true) return; //处于选择状态则返回
    if (list.sel_idx > 0)
    {
    	list.sel_idx -= 1;
        if (list.sel_idx < list.srt_idx)
        	list.srt_idx = list.sel_idx;
    }
    menu_oled_display_list();
}

//==========================================================================
//函数名称: menu_oled_select
//函数返回: 无
//参数说明: 无
//功能概要: 进入菜单选择模式
//==========================================================================
void menu_oled_select(void)
{
	list.sel_flag = true; //设置为选择状态
	menu_oled_display_item();
}

//==========================================================================
//函数名称: menu_oled_accept
//函数返回: 无
//参数说明: ch:输入的字符
//功能概要: 接受输入的字符，放入缓冲区并显示
//==========================================================================
void menu_oled_accept(uint8 ch)
{
	switch (ch)
	{
	case 'N': // next
		menu_oled_next_item(); break;
	case 'L': // last
		menu_oled_last_item(); break;
	case 'O': // ok
	{
		if (list.sel_flag == false) menu_oled_select();
		else menu_oled_save();
		break;
	}
	case 'B': // backspace
		menu_oled_backspace(); break;
	case 'C': // cancel
		menu_oled_reset(); break;
	default:
	{
		if (list.sel_flag == true && pos < BUF_SIZE)
		{
			oled_printf(STARTING_COL + 80 + 8 * pos, STARTING_PAGE + 2,
						"%c", ch);
			buf[pos++] = ch;
			buf[pos] = '\0';
		}
		break;
	}
	}
}

//==========================================================================
//函数名称: menu_oled_backspace
//函数返回: 无
//参数说明: 无
//功能概要: 退格
//==========================================================================
void menu_oled_backspace()
{
	if (pos == 0) return;
	buf[pos--] = '\0';
	oled_display_str(STARTING_COL + 80 + 8 * pos, STARTING_PAGE + 2, " ");
}

//==========================================================================
//函数名称: menu_oled_save
//函数返回: 无
//参数说明: 无
//功能概要: 保存菜单选择
//==========================================================================
void menu_oled_save(void)
{
    float modified_val;

    if (pos != 0)
    {
    	sscanf(buf, "%f", &modified_val);
    	list.items[list.sel_idx].value = modified_val;
    }
    menu_oled_reset();
}

//==========================================================================
//函数名称: menu_oled_reset
//函数返回: 无
//参数说明: 无
//功能概要: 重置菜单和选择
//==========================================================================
void menu_oled_reset(void)
{
	list.srt_idx = 0;
	list.sel_idx = 0;
	buf[0] = '\0';
	pos = 0;
	list.sel_flag = false; //设置为非选择状态
	menu_oled_display_list();
}
