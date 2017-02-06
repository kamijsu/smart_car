//==========================================================================
//�ļ����ƣ�custom.c
//���ܸ�Ҫ���û��Զ����������Դ�ļ�
//==========================================================================

#include "custom.h"
#include "oled.h"
#include "temp_sensor.h"

//"�¶�"����ģ���ݣ�����ȡģ���ֽڵ���;
//���ִ�СΪ16*16��ǰ32�ֽ�Ϊ"�¶�"�ϰ벿�֣���32�ֽ�Ϊ"�¶�"�°벿��
static const uint8 temp_char_lib[64] = { 0x10, 0x60, 0x02, 0x8C, 0x00, 0x00,
		0xFE, 0x92, 0x92, 0x92, 0x92, 0x92, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00,
		0xFC, 0x24, 0x24, 0x24, 0xFC, 0x25, 0x26, 0x24, 0xFC, 0x24, 0x24, 0x24,
		0x04, 0x00, 0x04, 0x04, 0x7E, 0x01, 0x40, 0x7E, 0x42, 0x42, 0x7E, 0x42,
		0x7E, 0x42, 0x42, 0x7E, 0x40, 0x00, 0x40, 0x30, 0x8F, 0x80, 0x84, 0x4C,
		0x55, 0x25, 0x25, 0x25, 0x55, 0x4C, 0x80, 0x80, 0x80, 0x00, };
//"��C"����ģ���ݣ�����ȡģ���ֽڵ��򣬷��Ŵ�СΪ16*16
static const uint8 degree_centigrade_char_lib[32] = { 0x06, 0x09, 0x09, 0xE6,
		0xF8, 0x0C, 0x04, 0x02, 0x02, 0x02, 0x02, 0x02, 0x04, 0x1E, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x07, 0x1F, 0x30, 0x20, 0x40, 0x40, 0x40, 0x40, 0x40,
		0x20, 0x10, 0x00, 0x00 };
static const uint8 temp_col = 0;	//"�¶�"����ʼ�к�
static const uint8 temp_page = 0;	//"�¶�"����ʼҳ��
static const uint8 temp_width = 4;	//�¶�ֵ�Ŀ��

//==========================================================================
//��������: custom_oled_display_init
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: ��ʼ��OLED��ʾ����
//==========================================================================
void custom_oled_display_init() {
	uint8 i;	//�α�

	//��ʾ"�¶�"
	oled_set_pos(temp_col, temp_page);
	for (i = 0; i < 32; i++) {
		oled_write_data(temp_char_lib[i]);
	}
	oled_set_pos(temp_col, temp_page + 1);
	for (; i < 64; i++) {
		oled_write_data(temp_char_lib[i]);
	}
	//��ʾ":"
	oled_display_str(temp_col + 32, temp_page, ":");
	//��ʾ"��C"
	oled_set_pos(temp_col + 40 + temp_width * 8, temp_page);
	for (i = 0; i < 16; i++) {
		oled_write_data(degree_centigrade_char_lib[i]);
	}
	oled_set_pos(temp_col + 40 + temp_width * 8, temp_page + 1);
	for (; i < 32; i++) {
		oled_write_data(degree_centigrade_char_lib[i]);
	}
	//��ʾ��ʼ���¶�ֵ
	custom_oled_update_temp();
}

//==========================================================================
//��������: custom_oled_update_temp
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: ����OLED��ʾ�¶�ֵ
//==========================================================================
void custom_oled_update_temp() {
	oled_printf(temp_col + 40, temp_page, "%*.1f", temp_width,
			temp_sensor_get_temp());
}
