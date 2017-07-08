//==========================================================================
//�ļ����ƣ�menu.c
//���ܸ�Ҫ���û��Զ����������Դ�ļ�
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
	uint8 buf[MENU_BUF_SIZE];   // �����ַ�������
	uint8 start_index;          // OLED��ʼ����ֵ����
	uint8 select_index;         // OLEDѡ���־����
	uint8 mode;                 // �ڼ��ײ���
	Interface interface;             // �˵���ǰ״̬
	Item param_read[MENU_PARAM_NUM]; // ����
} Menu;

static Menu menu;
static ParamCarPtr param_ptr;

//�ڲ���������
static void menu_get(uint8 mode);
static void menu_display(void);
static void menu_prev(void);
static void menu_next(void);
static void menu_save(void);
static void menu_toggle(void);

//==========================================================================
//��������: menu_init
//��������: ��
//����˵��: param���洢����ֵ���׵�ַ
//���ܸ�Ҫ: �˵����ݳ�ʼ��
//==========================================================================
void menu_init(ParamCarPtr param) {
	menu.start_index = 0;
	menu.select_index = 0;
	menu.buf[0] = '\0';
	menu.pos = 0;
	menu.interface = IMAGE; // ��ʾͼ��
	param_ptr = param;
	menu_display();
}

//==========================================================================
//��������: menu_accept
//��������: ��
//����˵��: ch:������ַ�
//���ܸ�Ҫ: ����������ַ������뻺��������ʾ
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
//��������: menu_can_show_img
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: ��ʾ��Ļ�Ƿ������ʾͼ��true�����ڲ˵�ͼ��ģʽ��������ʾ��
//                                false���˵�Ϊ����ģʽ����������ʾ
//==========================================================================
bool menu_can_show_img() {
	return menu.interface == IMAGE ? true : false;
}

//==========================================================================
//��������: menu_get
//��������: ��
//����˵��: mode����������
//���ܸ�Ҫ: ���ݸ���mode�����ܴ�flash�ж�ȡ�Ĳ���
//==========================================================================
void menu_get(uint8 mode) {
	//flash�ж�ȡ��������param_ptrָ�������
	param_get(param_ptr, menu.mode);
	Item param_read[] = { { "TgtAgl", param_ptr->angle.target_angle }, {
			"AglPidP", param_ptr->angle.pid.p }, { "AglPidD",
			param_ptr->angle.pid.d }, { "TgtS", param_ptr->speed.target_speed },
			{ "SPidP", param_ptr->speed.pid.p }, { "SPidI",
					param_ptr->speed.pid.i },
			{ "SPidD", param_ptr->speed.pid.d }, { "TurnPidP",
					param_ptr->turn.pid.p },
			{ "TurnPidD", param_ptr->turn.pid.d } };
	//���Ƶ�menu��param_read����
	memcpy(menu.param_read, param_read, sizeof(Item) * MENU_PARAM_NUM);
}

//==========================================================================
//��������: menu_display
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: ��ʾ�˵�����
//==========================================================================
void menu_display() {
	uint8 i;
	uint8 cur_idx;

	switch (menu.interface) {
	case IMAGE:
		//��0x00���������Ļ������Ļȫ��
		oled_fill(0x00);
		break;
	case SELECT:
		//��0x00���������Ļ������Ļȫ��
		oled_fill(0x00);
		oled_printf(0, 0, " interface:");
		break;
	case READ:
		cur_idx = menu.start_index; //OLED��ǰ��д����ֵ����
		for (i = 0; i < MENU_MAX_ROW && cur_idx < MENU_PARAM_NUM; i++) {
			oled_clear_page(i * 2);
			//����������Ͳ���ֵ
			oled_printf(0, i * 2, " %-8s %.4f", menu.param_read[cur_idx].name,
					menu.param_read[cur_idx++].value);
		}
		oled_printf(0, (menu.select_index - menu.start_index) * 2, "%c",
				menu.mode + '0');
		break;
	case MODIFY:
		//��0x00���������Ļ������Ļȫ��
		oled_fill(0x00); //�����޸Ĳ����Ľ���
		//����������Ͳ���ֵ
		oled_printf(0, 0, "%-8s %.4f", menu.param_read[menu.select_index].name,
				menu.param_read[menu.select_index].value);
		oled_printf(0, 2, "new: %-8s", menu.buf);
		break;
	}
}

//==========================================================================
//��������: menu_next
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: �ƶ����˵���һѡ��
//==========================================================================
static void menu_next(void) {
	if (menu.interface == READ && menu.select_index < MENU_PARAM_NUM - 1) {
		menu.select_index += 1;
		if (menu.select_index - menu.start_index >= MENU_MAX_ROW)
			menu.start_index += 1;
	}
	menu_display();
}

//==========================================================================
//��������: menu_prev
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: �ƶ����˵���һѡ��
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
//��������: menu_save
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: ����˵�ѡ��flash������ʾ�²˵�����Ļ��
//==========================================================================
static void menu_save(void) {
	float modified_val;

	if (menu.pos != 0) {
		sscanf(menu.buf, "%f", &modified_val);
		menu.param_read[menu.select_index].value = modified_val;
	}
	//�������Ƶ�param_ptrָ��ĵ�ַ
	param_ptr->angle.target_angle = menu.param_read[0].value;
	param_ptr->angle.pid.p = menu.param_read[1].value;
	param_ptr->angle.pid.d = menu.param_read[2].value;
	param_ptr->speed.target_speed = menu.param_read[3].value;
	param_ptr->speed.pid.p = menu.param_read[4].value;
	param_ptr->speed.pid.i = menu.param_read[5].value;
	param_ptr->speed.pid.d = menu.param_read[6].value;
	param_ptr->turn.pid.p = menu.param_read[7].value;
	param_ptr->turn.pid.d = menu.param_read[8].value;
	//���µ�flash��
	param_update(param_ptr, menu.mode);
	//���õ�������ȡ״̬
	menu_toggle();
}

//==========================================================================
//��������: menu_toggle
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: �л����棬���ԭ����MODIFY״̬�����������
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
