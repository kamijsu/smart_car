//==========================================================================
//�ļ����ƣ�menu.c
//���ܸ�Ҫ���û��Զ����������Դ�ļ�
//==========================================================================

#include "menu.h"
#include "oled.h"
#include "uart.h"

<<<<<<< HEAD
static const uint8 menu_col = 0;	//�˵�����ʼ�к�
static const uint8 menu_page = 0;	//�˵�����ʼҳ��
=======
static const uint8 menu_col = 0;	//"�˵�"����ʼ�к�
static const uint8 menu_page = 0;	//"�˵�"����ʼҳ��
>>>>>>> 7c960ba4d227b4170e5985f89be3ff29308a367b
static const uint8 max_row_number = 4;	//����к�

static const char menu_name[] = "speed";
static const float option[6] = {1.1, 1.2, 1.3, 1.4, 1.5, 1.6};
static uint8 start_idx = 0;	//OLED��ʼ����ֵ����
static uint8 select_idx = 0; //OLEDѡ���־����
static bool sel_flag = false; //ѡ���־λ

<<<<<<< HEAD
//==========================================================================
//��������: menu_oled_display
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: ��ʾ�˵�����
//==========================================================================
=======
>>>>>>> 7c960ba4d227b4170e5985f89be3ff29308a367b
void menu_oled_display()
{
	uint8 i;	//�α�
	uint8 current_idx = 0; //OLED��ǰ����ֵ����

	//��0x00���������Ļ������Ļȫ��
	oled_fill(0x00);
	current_idx = start_idx;
	//���������
	oled_printf(menu_col, menu_page, "%s", menu_name);
	if (true == sel_flag)
		oled_printf(menu_col, menu_page + (select_idx - start_idx + 1) * 2, "%c", '*');
	for (i = 1; i < max_row_number && current_idx < 6; i++)
		oled_printf(menu_col + 8, menu_page + i * 2,
						"%.1f", option[current_idx++]);
}
<<<<<<< HEAD

//==========================================================================
//��������: menu_oled_next_item
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: �ƶ����˵���һѡ��
//==========================================================================
=======
>>>>>>> 7c960ba4d227b4170e5985f89be3ff29308a367b
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
<<<<<<< HEAD

//==========================================================================
//��������: menu_oled_last_item
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: �ƶ����˵���һѡ��
//==========================================================================
=======
>>>>>>> 7c960ba4d227b4170e5985f89be3ff29308a367b
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
<<<<<<< HEAD

//==========================================================================
//��������: menu_oled_select
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: ����˵�ѡ��ģʽ
//==========================================================================
=======
>>>>>>> 7c960ba4d227b4170e5985f89be3ff29308a367b
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
<<<<<<< HEAD

//==========================================================================
//��������: menu_oled_save
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: ����˵�ѡ��
//==========================================================================
=======
>>>>>>> 7c960ba4d227b4170e5985f89be3ff29308a367b
void menu_oled_save()
{
	sel_flag = false;
	//��0x00���������Ļ������Ļȫ��
	oled_fill(0x00);
	oled_printf(menu_col, menu_page, "%s = %.1f",
			menu_name, option[select_idx]);
}
<<<<<<< HEAD

//==========================================================================
//��������: menu_oled_reset
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: ���ò˵���ѡ��
//==========================================================================
=======
>>>>>>> 7c960ba4d227b4170e5985f89be3ff29308a367b
void menu_oled_reset()
{
	sel_flag = false;
	start_idx = 0;
	select_idx = 0;
	menu_oled_display();
}
