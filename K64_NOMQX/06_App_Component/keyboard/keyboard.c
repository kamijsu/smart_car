//==========================================================================
//�ļ����ƣ�keyboard.c
//���ܸ�Ҫ������Ӧ����������Դ�ļ�
//==========================================================================

#include "keyboard.h"

//��ֵ������ַ���ӳ���
static const uint8 keyboard_map_table[] = {
KEYBOARD_KEY_ROW0_COL0, KEYBOARD_KEY_ROW0_COL1, KEYBOARD_KEY_ROW0_COL2, KEYBOARD_KEY_ROW0_COL3,
KEYBOARD_KEY_ROW1_COL0, KEYBOARD_KEY_ROW1_COL1, KEYBOARD_KEY_ROW1_COL2, KEYBOARD_KEY_ROW1_COL3,
KEYBOARD_KEY_ROW2_COL0, KEYBOARD_KEY_ROW2_COL1, KEYBOARD_KEY_ROW2_COL2, KEYBOARD_KEY_ROW2_COL3,
KEYBOARD_KEY_ROW3_COL0, KEYBOARD_KEY_ROW3_COL1, KEYBOARD_KEY_ROW3_COL2, KEYBOARD_KEY_ROW3_COL3
};

//�����������ű�
static uint8 keyboard_row_table[] = {
KEYBOARD_ROW0, KEYBOARD_ROW1, KEYBOARD_ROW2, KEYBOARD_ROW3
};
//�����������ű�
static uint8 keyboard_col_table[] = {
KEYBOARD_COL0, KEYBOARD_COL1, KEYBOARD_COL2, KEYBOARD_COL3
};

//==========================================================================
//��������: keyboard_scan1
//��������: ��ֵ���޼����·���0xFF
//����˵��: ��
//���ܸ�Ҫ: ɨ��һ�μ��̣����ؼ�ֵ
//==========================================================================
static uint8 keyboard_scan1() {
	uint8 i, j;		//�α�

	//�Ƚ���һ�����ͣ������������
	gpio_set_level(KEYBOARD_ROW0, GPIO_LEVEL_LOW);
	gpio_set_level(KEYBOARD_ROW1, GPIO_LEVEL_HIGH);
	gpio_set_level(KEYBOARD_ROW2, GPIO_LEVEL_HIGH);
	gpio_set_level(KEYBOARD_ROW3, GPIO_LEVEL_HIGH);

	//ɨ����У��ҵ��͵�ƽ��ʱ���ؼ�ֵ
	for (i = 0; i < 4; ++i) {
		if (gpio_get_level(keyboard_col_table[i]) == GPIO_LEVEL_LOW) {
			return i;
		}
	}

	//ɨ���������
	for (i = 1; i < 4; ++i) {
		//��һ������
		gpio_set_level(keyboard_row_table[i - 1], GPIO_LEVEL_HIGH);
		//ɨ��������
		gpio_set_level(keyboard_row_table[i], GPIO_LEVEL_LOW);
		//ɨ����У��ҵ��͵�ƽ��ʱ���ؼ�ֵ
		for (j = 0; j < 4; ++j) {
			if (gpio_get_level(keyboard_col_table[j]) == GPIO_LEVEL_LOW) {
				return (i << 2) + j;
			}
		}
	}

	//�޼����£�����0xFF
	return 0xFF;
}

//==========================================================================
//��������: keyboard_scanN
//��������: ��ֵ���޼����·���0xFF
//����˵��: ��
//���ܸ�Ҫ: ɨ����̣��ﵽ��ֵ����ʱ�����ؼ�ֵ
//==========================================================================
static uint8 keyboard_scanN() {
	uint8 scan_cnt;		//�Ѿ�ɨ��Ĵ���
	uint8 last_val;		//��һ�εļ�ֵ
	uint8 current_val;	//��ǰ��ֵ

	//�Ѿ�ɨ��0��
	scan_cnt = 0;
	//��һ���޼�����
	last_val = 0xFF;

	//δ�ﵽ��ֵ����ʱ��ɨ�����
	while (scan_cnt < KEYBORAD_THRESHOLD) {
		//��ȡ��ǰ��ֵ
		current_val = keyboard_scan1();
		//��һ�μ�ֵ�뵱ǰ��ֵ���ʱ�������Ѿ�ɨ��Ĵ���
		if (current_val == last_val) {
			++scan_cnt;
		} else {
			//��ֵ��ͬʱ��������һ�μ�ֵ
			last_val = current_val;
			scan_cnt = 1;
		}
	}

	//�����������ͣ��ָ���ʼ״̬
	gpio_set_level(KEYBOARD_ROW0, GPIO_LEVEL_LOW);
	gpio_set_level(KEYBOARD_ROW1, GPIO_LEVEL_LOW);
	gpio_set_level(KEYBOARD_ROW2, GPIO_LEVEL_LOW);
	gpio_set_level(KEYBOARD_ROW3, GPIO_LEVEL_LOW);

	//���ؼ�ֵ
	return last_val;
}

//==========================================================================
//��������: keyboard_init
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: ��ʼ�����̣���Ӧ������keyboard.h��
//��ע: �������������ŵ���
//==========================================================================
void keyboard_init() {
	//��������͵�ƽ
	gpio_init(KEYBOARD_ROW0, GPIO_DIR_OUTPUT, GPIO_LEVEL_LOW);
	gpio_init(KEYBOARD_ROW1, GPIO_DIR_OUTPUT, GPIO_LEVEL_LOW);
	gpio_init(KEYBOARD_ROW2, GPIO_DIR_OUTPUT, GPIO_LEVEL_LOW);
	gpio_init(KEYBOARD_ROW3, GPIO_DIR_OUTPUT, GPIO_LEVEL_LOW);
	//�����������ŵ���
	gpio_init(KEYBOARD_COL0, GPIO_DIR_INPUT, GPIO_LEVEL_HIGH);
	gpio_init(KEYBOARD_COL1, GPIO_DIR_INPUT, GPIO_LEVEL_HIGH);
	gpio_init(KEYBOARD_COL2, GPIO_DIR_INPUT, GPIO_LEVEL_HIGH);
	gpio_init(KEYBOARD_COL3, GPIO_DIR_INPUT, GPIO_LEVEL_HIGH);
}

//==========================================================================
//��������: keyboard_read
//��������: true:��ȡ�ɹ�; false:��ȡʧ�ܣ��޼�����
//����˵��: key:�洢���̰����ַ��ĵ�ַ
//���ܸ�Ҫ: ��ȡ���̣��ɹ�ʱ�洢��ȡ�ļ��̰����ַ�������ʱ�����ʱ��
//==========================================================================
bool keyboard_read(uint8* key) {
	uint8 key_val;

	//ɨ����̣���ȡ��ֵ
	key_val = keyboard_scanN();

	//�ж��Ƿ��м�����
	if (key_val == 0xFF) {
		//�޼����£���ȡʧ��
		return false;
	} else {
		//�м����£��洢���̰����ַ�
		*key = keyboard_map_table[key_val];
		//��ʱ�����ʱ��
		KEYBOARD_DELAY_MS(KEYBOARD_TIME_DELAY);
		//��ȡ�ɹ�
		return true;
	}
}

//==========================================================================
//��������: keyboard_enable_int
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: ʹ�ܼ����жϣ����а���������ʱ�������ж�
//��ע: �����������ڶ˿�ΪPORTAʱ���жϷ�����ΪPORTA_IRQHandler
//==========================================================================
void keyboard_enable_int() {
	//ʹ�����������жϣ��͵�ƽ�����ж�
	gpio_enable_int(KEYBOARD_COL0, GPIO_INT_LOW_LEVEL);
	gpio_enable_int(KEYBOARD_COL1, GPIO_INT_LOW_LEVEL);
	gpio_enable_int(KEYBOARD_COL2, GPIO_INT_LOW_LEVEL);
	gpio_enable_int(KEYBOARD_COL3, GPIO_INT_LOW_LEVEL);
}

//==========================================================================
//��������: keyboard_disable_int
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: �رռ����ж�
//==========================================================================
void keyboard_disable_int() {
	//�ر����������ж�
	gpio_disable_int(KEYBOARD_COL0);
	gpio_disable_int(KEYBOARD_COL1);
	gpio_disable_int(KEYBOARD_COL2);
	gpio_disable_int(KEYBOARD_COL3);
}

//==========================================================================
//��������: keyboard_get_int
//��������: true:�����ж�; false:δ�����ж�
//����˵��: ��
//���ܸ�Ҫ: ��ȡ�����жϱ�־
//==========================================================================
bool keyboard_get_int() {
	uint8 i;

	//���������Ų����ж�ʱ�����̲����ж�
	for (i = 0; i < 4; ++i) {
		if (gpio_get_int(keyboard_col_table[i])) {
			return true;
		}
	}
	//����δ�����ж�
	return false;
}

//==========================================================================
//��������: keyboard_clear_int
//��������: ��
//����˵��: ��
//���ܸ�Ҫ: ��������жϱ�־
//��ע: �ڶ�ȡ���̺�������жϱ�־
//==========================================================================
void keyboard_clear_int() {
	//������������жϱ�־
	gpio_clear_int(KEYBOARD_COL0);
	gpio_clear_int(KEYBOARD_COL1);
	gpio_clear_int(KEYBOARD_COL2);
	gpio_clear_int(KEYBOARD_COL3);
}
