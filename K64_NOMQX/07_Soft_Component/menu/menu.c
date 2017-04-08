//==========================================================================
//�ļ����ƣ�menu.c
//���ܸ�Ҫ���û��Զ����������Դ�ļ�
//==========================================================================

#include "menu.h"
#include "oled.h"
#include "uart.h"
#include <stdio.h>
#include <string.h>

static uint8 buf[BUF_SIZE];   // �����ַ�������
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
	uint8 srt_idx;      //OLED��ʼ����ֵ����
	uint8 sel_idx;      //OLEDѡ���־����
	bool sel_flag;      //�Ƿ���ѡ��״̬
} menu;
menu list;

//�ڲ���������
void menu_oled_display_list(void);
void menu_oled_last_item(void);
void menu_oled_next_item(void);
void menu_oled_select(void);
void menu_oled_backspace();
void menu_oled_save(void);
void menu_oled_reset(void);

//==========================================================================
//��������: menu_oled_init
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: �˵����ݳ�ʼ��
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
	list.sel_flag = false; //����Ϊ��ѡ��״̬
	buf[0] = '\0';
	pos = 0;
	menu_oled_display_list();
}

//==========================================================================
//��������: menu_oled_display_list
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: ��ʾ�˵�����
//==========================================================================
void menu_oled_display_list(void)
{
    uint8 i;    //�α�
    uint8 cur_idx = list.srt_idx; //OLED��ǰ����ֵ����

    //��0x00���������Ļ������Ļȫ��
    oled_fill(0x00);
    for (i = 0; i < MAX_ROW && cur_idx < list.num_of_params; i++)
    {
        //���������
        oled_printf(STARTING_COL + 8, STARTING_PAGE + i * 2,
        		    "%s:", list.items[cur_idx].name);
        //�������ֵ
        oled_printf(STARTING_COL + 80, STARTING_PAGE + i * 2,
        		    "%.4f", list.items[cur_idx++].value);
    }
    oled_printf(STARTING_COL, STARTING_PAGE + (list.sel_idx - list.srt_idx) * 2,
				"%c", '*');
}

//==========================================================================
//��������: menu_oled_display_item
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: ��ʾָ����ѡ������ƺ�ֵ
//==========================================================================
void menu_oled_display_item(void)
{
	//��0x00���������Ļ������Ļȫ��
	oled_fill(0x00);
	//���������
	oled_printf(STARTING_COL, STARTING_PAGE,
				"%s:", list.items[list.sel_idx].name);
	//�������ֵ
	oled_printf(STARTING_COL + 80, STARTING_PAGE,
				"%f", list.items[list.sel_idx].value);
	oled_printf(STARTING_COL, STARTING_PAGE + 2, "new: ");
}

//==========================================================================
//��������: menu_oled_next_item
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: �ƶ����˵���һѡ��
//==========================================================================
void menu_oled_next_item(void)
{
	if (list.sel_flag == true) return; //����ѡ��״̬�򷵻�
    if (list.sel_idx < 5)
    {
    	list.sel_idx += 1;
        if (list.sel_idx - list.srt_idx >= MAX_ROW)
        	list.srt_idx += 1;
    }
    menu_oled_display_list();
}

//==========================================================================
//��������: menu_oled_last_item
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: �ƶ����˵���һѡ��
//==========================================================================
void menu_oled_last_item(void)
{
	if (list.sel_flag == true) return; //����ѡ��״̬�򷵻�
    if (list.sel_idx > 0)
    {
    	list.sel_idx -= 1;
        if (list.sel_idx < list.srt_idx)
        	list.srt_idx = list.sel_idx;
    }
    menu_oled_display_list();
}

//==========================================================================
//��������: menu_oled_select
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: ����˵�ѡ��ģʽ
//==========================================================================
void menu_oled_select(void)
{
	list.sel_flag = true; //����Ϊѡ��״̬
	menu_oled_display_item();
}

//==========================================================================
//��������: menu_oled_accept
//��������: ��
//����˵��: ch:������ַ�
//���ܸ�Ҫ: ����������ַ������뻺��������ʾ
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
//��������: menu_oled_backspace
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: �˸�
//==========================================================================
void menu_oled_backspace()
{
	if (pos == 0) return;
	buf[pos--] = '\0';
	oled_display_str(STARTING_COL + 80 + 8 * pos, STARTING_PAGE + 2, " ");
}

//==========================================================================
//��������: menu_oled_save
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: ����˵�ѡ��
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
//��������: menu_oled_reset
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: ���ò˵���ѡ��
//==========================================================================
void menu_oled_reset(void)
{
	list.srt_idx = 0;
	list.sel_idx = 0;
	buf[0] = '\0';
	pos = 0;
	list.sel_flag = false; //����Ϊ��ѡ��״̬
	menu_oled_display_list();
}
