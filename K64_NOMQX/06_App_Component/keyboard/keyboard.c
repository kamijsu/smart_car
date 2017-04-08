//==========================================================================
//文件名称：keyboard.c
//功能概要：键盘应用驱动程序源文件
//==========================================================================

#include "keyboard.h"

//键值与键盘字符的映射表
static const uint8 keyboard_map_table[] = {
KEYBOARD_KEY_ROW0_COL0, KEYBOARD_KEY_ROW0_COL1, KEYBOARD_KEY_ROW0_COL2, KEYBOARD_KEY_ROW0_COL3,
KEYBOARD_KEY_ROW1_COL0, KEYBOARD_KEY_ROW1_COL1, KEYBOARD_KEY_ROW1_COL2, KEYBOARD_KEY_ROW1_COL3,
KEYBOARD_KEY_ROW2_COL0, KEYBOARD_KEY_ROW2_COL1, KEYBOARD_KEY_ROW2_COL2, KEYBOARD_KEY_ROW2_COL3,
KEYBOARD_KEY_ROW3_COL0, KEYBOARD_KEY_ROW3_COL1, KEYBOARD_KEY_ROW3_COL2, KEYBOARD_KEY_ROW3_COL3
};

//键盘行线引脚表
static uint8 keyboard_row_table[] = {
KEYBOARD_ROW0, KEYBOARD_ROW1, KEYBOARD_ROW2, KEYBOARD_ROW3
};
//键盘列线引脚表
static uint8 keyboard_col_table[] = {
KEYBOARD_COL0, KEYBOARD_COL1, KEYBOARD_COL2, KEYBOARD_COL3
};

//==========================================================================
//函数名称: keyboard_scan1
//函数返回: 键值，无键按下返回0xFF
//参数说明: 无
//功能概要: 扫描一次键盘，返回键值
//==========================================================================
static uint8 keyboard_scan1() {
	uint8 i, j;		//游标

	//先将第一行拉低，其余各行拉高
	gpio_set_level(KEYBOARD_ROW0, GPIO_LEVEL_LOW);
	gpio_set_level(KEYBOARD_ROW1, GPIO_LEVEL_HIGH);
	gpio_set_level(KEYBOARD_ROW2, GPIO_LEVEL_HIGH);
	gpio_set_level(KEYBOARD_ROW3, GPIO_LEVEL_HIGH);

	//扫描各列，找到低电平列时返回键值
	for (i = 0; i < 4; ++i) {
		if (gpio_get_level(keyboard_col_table[i]) == GPIO_LEVEL_LOW) {
			return i;
		}
	}

	//扫描其余各行
	for (i = 1; i < 4; ++i) {
		//上一行拉高
		gpio_set_level(keyboard_row_table[i - 1], GPIO_LEVEL_HIGH);
		//扫描行拉低
		gpio_set_level(keyboard_row_table[i], GPIO_LEVEL_LOW);
		//扫描各列，找到低电平列时返回键值
		for (j = 0; j < 4; ++j) {
			if (gpio_get_level(keyboard_col_table[j]) == GPIO_LEVEL_LOW) {
				return (i << 2) + j;
			}
		}
	}

	//无键按下，返回0xFF
	return 0xFF;
}

//==========================================================================
//函数名称: keyboard_scanN
//函数返回: 键值，无键按下返回0xFF
//参数说明: 无
//功能概要: 扫描键盘，达到阈值次数时，返回键值
//==========================================================================
static uint8 keyboard_scanN() {
	uint8 scan_cnt;		//已经扫描的次数
	uint8 last_val;		//上一次的键值
	uint8 current_val;	//当前键值

	//已经扫描0次
	scan_cnt = 0;
	//上一次无键按下
	last_val = 0xFF;

	//未达到阈值次数时，扫描键盘
	while (scan_cnt < KEYBORAD_THRESHOLD) {
		//获取当前键值
		current_val = keyboard_scan1();
		//上一次键值与当前键值相等时，增加已经扫描的次数
		if (current_val == last_val) {
			++scan_cnt;
		} else {
			//键值不同时，更改上一次键值
			last_val = current_val;
			scan_cnt = 1;
		}
	}

	//将各行线拉低，恢复初始状态
	gpio_set_level(KEYBOARD_ROW0, GPIO_LEVEL_LOW);
	gpio_set_level(KEYBOARD_ROW1, GPIO_LEVEL_LOW);
	gpio_set_level(KEYBOARD_ROW2, GPIO_LEVEL_LOW);
	gpio_set_level(KEYBOARD_ROW3, GPIO_LEVEL_LOW);

	//返回键值
	return last_val;
}

//==========================================================================
//函数名称: keyboard_init
//函数返回: 无
//参数说明: 无
//功能概要: 初始化键盘，相应配置在keyboard.h中
//备注: 已上拉列线引脚电阻
//==========================================================================
void keyboard_init() {
	//行线输出低电平
	gpio_init(KEYBOARD_ROW0, GPIO_DIR_OUTPUT, GPIO_LEVEL_LOW);
	gpio_init(KEYBOARD_ROW1, GPIO_DIR_OUTPUT, GPIO_LEVEL_LOW);
	gpio_init(KEYBOARD_ROW2, GPIO_DIR_OUTPUT, GPIO_LEVEL_LOW);
	gpio_init(KEYBOARD_ROW3, GPIO_DIR_OUTPUT, GPIO_LEVEL_LOW);
	//列线上拉引脚电阻
	gpio_init(KEYBOARD_COL0, GPIO_DIR_INPUT, GPIO_LEVEL_HIGH);
	gpio_init(KEYBOARD_COL1, GPIO_DIR_INPUT, GPIO_LEVEL_HIGH);
	gpio_init(KEYBOARD_COL2, GPIO_DIR_INPUT, GPIO_LEVEL_HIGH);
	gpio_init(KEYBOARD_COL3, GPIO_DIR_INPUT, GPIO_LEVEL_HIGH);
}

//==========================================================================
//函数名称: keyboard_read
//函数返回: true:读取成功; false:读取失败，无键按下
//参数说明: key:存储键盘按键字符的地址
//功能概要: 读取键盘，成功时存储读取的键盘按键字符，并延时定义的时间
//==========================================================================
bool keyboard_read(uint8* key) {
	uint8 key_val;

	//扫描键盘，获取键值
	key_val = keyboard_scanN();

	//判断是否有键按下
	if (key_val == 0xFF) {
		//无键按下，读取失败
		return false;
	} else {
		//有键按下，存储键盘按键字符
		*key = keyboard_map_table[key_val];
		//延时定义的时间
		KEYBOARD_DELAY_MS(KEYBOARD_TIME_DELAY);
		//读取成功
		return true;
	}
}

//==========================================================================
//函数名称: keyboard_enable_int
//函数返回: 无
//参数说明: 无
//功能概要: 使能键盘中断，当有按键被按下时，触发中断
//备注: 列线引脚所在端口为PORTA时，中断服务函数为PORTA_IRQHandler
//==========================================================================
void keyboard_enable_int() {
	//使能列线引脚中断，低电平触发中断
	gpio_enable_int(KEYBOARD_COL0, GPIO_INT_LOW_LEVEL);
	gpio_enable_int(KEYBOARD_COL1, GPIO_INT_LOW_LEVEL);
	gpio_enable_int(KEYBOARD_COL2, GPIO_INT_LOW_LEVEL);
	gpio_enable_int(KEYBOARD_COL3, GPIO_INT_LOW_LEVEL);
}

//==========================================================================
//函数名称: keyboard_disable_int
//函数返回: 无
//参数说明: 无
//功能概要: 关闭键盘中断
//==========================================================================
void keyboard_disable_int() {
	//关闭列线引脚中断
	gpio_disable_int(KEYBOARD_COL0);
	gpio_disable_int(KEYBOARD_COL1);
	gpio_disable_int(KEYBOARD_COL2);
	gpio_disable_int(KEYBOARD_COL3);
}

//==========================================================================
//函数名称: keyboard_get_int
//函数返回: true:产生中断; false:未产生中断
//参数说明: 无
//功能概要: 获取键盘中断标志
//==========================================================================
bool keyboard_get_int() {
	uint8 i;

	//当列线引脚产生中断时，键盘产生中断
	for (i = 0; i < 4; ++i) {
		if (gpio_get_int(keyboard_col_table[i])) {
			return true;
		}
	}
	//键盘未产生中断
	return false;
}

//==========================================================================
//函数名称: keyboard_clear_int
//函数返回: 无
//参数说明: 无
//功能概要: 清除键盘中断标志
//备注: 在读取键盘后，再清除中断标志
//==========================================================================
void keyboard_clear_int() {
	//清除列线引脚中断标志
	gpio_clear_int(KEYBOARD_COL0);
	gpio_clear_int(KEYBOARD_COL1);
	gpio_clear_int(KEYBOARD_COL2);
	gpio_clear_int(KEYBOARD_COL3);
}
