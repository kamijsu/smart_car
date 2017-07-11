//==========================================================================
//文件名称：menu.c
//功能概要：用户自定义软件程序源文件
//==========================================================================

#include <stdio.h>
#include <string.h>
#include "menu.h"

typedef enum {
	IMAGE, SELECT, READ, MODIFY
} Interface;
typedef struct {
	uint8* name;
	float value;
} Item;
typedef struct {
	uint8 pos;
	uint8 buf[MENU_BUF_SIZE];   // 输入字符缓冲区
	uint8 start_index;          // OLED起始参数值索引
	uint8 select_index;         // OLED选择标志索引
	uint8 mode;                 // 第几套参数
	Interface interface;             // 菜单当前状态
	Item param_read[MENU_PARAM_CNT]; // 参数
} Menu;

static Menu menu;
static ParamCarPtr param_ptr;

//内部函数声明
static void menu_get(uint8 mode);
static void menu_display(void);
static void menu_prev(void);
static void menu_next(void);
static void menu_save(void);
static void menu_toggle(void);

//==========================================================================
//函数名称: menu_init
//函数返回: 无
//参数说明: param：存储参数值的首地址
//功能概要: 菜单内容初始化
//==========================================================================
void menu_init(ParamCarPtr param) {
	menu.start_index = 0;
	menu.select_index = 0;
	menu.buf[0] = '\0';
	menu.pos = 0;
	menu.interface = IMAGE; // 显示图像
	param_ptr = param;
	menu_display();
}

//==========================================================================
//函数名称: menu_accept
//函数返回: 无
//参数说明: ch:输入的字符
//功能概要: 接受输入的字符，放入缓冲区并显示
//==========================================================================
void menu_accept(uint8 ch) {
	switch (ch) {
	case 'N': // next
		menu_next();
		break;
	case 'P': // prev
		menu_prev();
		break;
	case 'O': // ok
	{
		if (menu.interface == READ) {
			menu.interface = MODIFY;
			menu_display();
		} else if (menu.interface == MODIFY)
			menu_save();
		break;
	}
	case 'B': // backspace
		if (menu.interface == MODIFY) {
			if (menu.pos != 0)
				menu.buf[--menu.pos] = '\0';
			menu_display();
		} else if (menu.interface == IMAGE) {
			param_ptr->speed.distance_err = 0;
		}
		break;
	case 'C': // cancel
		menu_toggle();
		break;
	default:
		if (menu.interface == SELECT && ch >= '0' && ch < '4') {
			menu.mode = ch - '0';
			menu_get(menu.mode);
			menu.interface = READ;
		} else if (menu.interface == MODIFY && menu.pos < MENU_BUF_SIZE) {
			menu.buf[menu.pos++] = ch;
			menu.buf[menu.pos] = '\0';
		}
		menu_display();
		break;
	}
}

//==========================================================================
//函数名称: menu_can_show_img
//函数返回: 无
//参数说明: 无
//功能概要: 表示屏幕是否可以显示图像，true：处于菜单图像模式，可以显示；
//                                false：菜单为其他模式，不可以显示
//==========================================================================
bool menu_can_show_img() {
	return menu.interface == IMAGE ? true : false;
}

//==========================================================================
//函数名称: menu_get
//函数返回: 无
//参数说明: mode：参数套数
//功能概要: 根据给定mode，接受从flash中读取的参数
//==========================================================================
void menu_get(uint8 mode) {
	//flash中读取参数放入param_ptr指向的区域
	param_get(param_ptr, menu.mode);
	Item param_read[] = {
			{ "TgtAgl", param_ptr->angle.target_angle },
			{"AglPidP", param_ptr->angle.pid.p },
			{ "AglPidD", param_ptr->angle.pid.d },
			{ "TgtS", param_ptr->speed.target_speed },
			{ "SPidP", param_ptr->speed.pid.p },
			{ "SPidI", param_ptr->speed.pid.i },
			{ "SPidD", param_ptr->speed.pid.d },
			{ "TurnPidP", param_ptr->turn.pid.p },
			{ "TurnPidD", param_ptr->turn.pid.d },
			{ "TurnTMdP", param_ptr->turn.target_mid_point } };
	//复制到menu的param_read数组
	memcpy(menu.param_read, param_read, sizeof(Item) * MENU_PARAM_CNT);
}

//==========================================================================
//函数名称: menu_display
//函数返回: 无
//参数说明: 无
//功能概要: 显示菜单内容
//==========================================================================
void menu_display() {
	uint8 i;
	uint8 cur_idx;

	switch (menu.interface) {
	case IMAGE:
		//用0x00填充整个屏幕，即屏幕全暗
		oled_fill(0x00);
		break;
	case SELECT:
		//用0x00填充整个屏幕，即屏幕全暗
		oled_fill(0x00);
		oled_printf(0, 0, " interface:");
		break;
	case READ:
		cur_idx = menu.start_index; //OLED当前可写参数值索引
		for (i = 0; i < MENU_MAX_ROW && cur_idx < MENU_PARAM_CNT; i++) {
			oled_clear_page(i * 2);
			//输出参数名和参数值
			oled_printf(0, i * 2, " %-8s %.4f", menu.param_read[cur_idx].name,
					menu.param_read[cur_idx++].value);
		}
		oled_printf(0, (menu.select_index - menu.start_index) * 2, "%c",
				menu.mode + '0');
		break;
	case MODIFY:
		//用0x00填充整个屏幕，即屏幕全暗
		oled_fill(0x00); //换到修改参数的界面
		//输出参数名和参数值
		oled_printf(0, 0, "%-8s %.4f", menu.param_read[menu.select_index].name,
				menu.param_read[menu.select_index].value);
		oled_printf(0, 2, "new: %-8s", menu.buf);
		break;
	}
}

//==========================================================================
//函数名称: menu_next
//函数返回: 无
//参数说明: 无
//功能概要: 移动到菜单下一选项
//==========================================================================
static void menu_next(void) {
	if (menu.interface == READ && menu.select_index < MENU_PARAM_CNT - 1) {
		menu.select_index += 1;
		if (menu.select_index - menu.start_index >= MENU_MAX_ROW)
			menu.start_index += 1;
	}
	menu_display();
}

//==========================================================================
//函数名称: menu_prev
//函数返回: 无
//参数说明: 无
//功能概要: 移动到菜单上一选项
//==========================================================================
static void menu_prev(void) {
	if (menu.select_index > 0) {
		menu.select_index -= 1;
		if (menu.select_index < menu.start_index)
			menu.start_index = menu.select_index;
	}
	menu_display();
}

//==========================================================================
//函数名称: menu_save
//函数返回: 无
//参数说明: 无
//功能概要: 保存菜单选择到flash，并显示新菜单到屏幕上
//==========================================================================
static void menu_save(void) {
	float modified_val;

	if (menu.pos != 0) {
		sscanf(menu.buf, "%f", &modified_val);
		menu.param_read[menu.select_index].value = modified_val;
	}
	//参数复制到param_ptr指向的地址
	param_ptr->angle.target_angle = menu.param_read[0].value;
	param_ptr->angle.pid.p = menu.param_read[1].value;
	param_ptr->angle.pid.d = menu.param_read[2].value;
	param_ptr->speed.target_speed = menu.param_read[3].value;
	param_ptr->speed.pid.p = menu.param_read[4].value;
	param_ptr->speed.pid.i = menu.param_read[5].value;
	param_ptr->speed.pid.d = menu.param_read[6].value;
	param_ptr->turn.pid.p = menu.param_read[7].value;
	param_ptr->turn.pid.d = menu.param_read[8].value;
	param_ptr->turn.target_mid_point = menu.param_read[9].value;
	//更新到flash中
	param_update(param_ptr, menu.mode);
	//重置到参数读取状态
	menu_toggle();
}

//==========================================================================
//函数名称: menu_toggle
//函数返回: 无
//参数说明: 无
//功能概要: 切换界面，如果原本是MODIFY状态，则输入清空
//==========================================================================
static void menu_toggle(void) {
	switch (menu.interface) {
	case MODIFY:
		menu.buf[0] = '\0';
		menu.pos = 0;
		menu.interface = READ;
		break;
	case READ:
		menu.interface = SELECT;
		break;
	case SELECT:
		menu.interface = IMAGE;
		break;
	case IMAGE:
		menu.interface = SELECT;
		break;
	}
	menu_display();
}
