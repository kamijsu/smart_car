//==========================================================================
//�ļ����ƣ�menu.c
//���ܸ�Ҫ���û��Զ����������Դ�ļ�
//==========================================================================

#include "menu.h"
#include "oled.h"
#include "uart.h"
#include "stdlib.h"

static const uint8 menu_col = 0;	//�˵�����ʼ�к�
static const uint8 menu_page = 0;	//�˵�����ʼҳ��
static const uint8 max_row_number = 4;	//����к�

static const uint8* parameters[6] = {"speed", "angle", "radius",
									 "length", "width", "height"};
static float values[6] = {1.1, 1.2, 1.3, 1.4, 1.5, 1.6};
static float modified_val = 0.0;
static uint8 start_idx = 0;	//OLED��ʼ����ֵ����
static uint8 select_idx = 0; //OLEDѡ���־����

//==========================================================================
//��������: menu_oled_display
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: ��ʾ�˵�����
//==========================================================================
void menu_oled_display()
{
	uint8 i;	//�α�
	uint8 current_idx = 0; //OLED��ǰ����ֵ����

	//��0x00���������Ļ������Ļȫ��
	oled_fill(0x00);
	current_idx = start_idx;
	for (i = 0; i < max_row_number && current_idx < 6; i++)
	{
		//���������
		oled_printf(menu_col + 8, menu_page + i * 2, "%s:", parameters[current_idx]);
		//�������ֵ
		oled_printf(menu_col + 80, menu_page + i * 2, "%.1f", values[current_idx++]);
	}
	oled_printf(menu_col, menu_page + (select_idx - start_idx) * 2, "%c", '*');
}

//==========================================================================
//��������: menu_oled_next_item
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: �ƶ����˵���һѡ��
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
//��������: menu_oled_last_item
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: �ƶ����˵���һѡ��
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
//��������: menu_oled_select
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: ����˵�ѡ��ģʽ
//==========================================================================
void menu_oled_select()
{
	//��0x00���������Ļ������Ļȫ��
	oled_fill(0x00);
	//���������
	oled_printf(menu_col, menu_page, "%s:", parameters[select_idx]);
	//�������ֵ
	oled_printf(menu_col + 80, menu_page, "%.1f", values[select_idx]);
	oled_printf(menu_col + 8, (max_row_number - 1) * 2, "%s", "modify:");
}

//==========================================================================
//��������: menu_oled_modify
//��������: ��
//����˵��: buff[8]:����޸�ֵ�Ļ���������
//���ܸ�Ҫ: �޸�ѡ��Ĳ���ֵ
//==========================================================================
void menu_oled_modify(uint8 buff[8])
{
	modified_val = atof(buff);
	oled_printf(menu_col + 80, (max_row_number - 1) * 2, "%.1f", modified_val);
}

//==========================================================================
//��������: menu_oled_save
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: ����˵�ѡ��
//==========================================================================
void menu_oled_save()
{
	values[select_idx] = modified_val;
	menu_oled_display();
}

//==========================================================================
//��������: menu_oled_reset
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: ���ò˵���ѡ��
//==========================================================================
void menu_oled_reset()
{
	start_idx = 0;
	select_idx = 0;
	menu_oled_display();
}
