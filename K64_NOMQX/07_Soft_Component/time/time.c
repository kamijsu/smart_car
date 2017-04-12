//==========================================================================
//�ļ����ƣ�time.c
//���ܸ�Ҫ��ʱ���������Դ�ļ�
//==========================================================================

#include "time.h"

//�����ʱ���ṹ��
typedef struct {
	uint8 flags;		//ͨ����־����0λ��ʾͨ������ע�ᣬ��1λ��ʾ��ʱ�����޵���ʱ��
	uint64 start_time;	//��ʱ����ʼʱ��
	uint64 set_time;	//��ʱ���趨�ļ�ʱʱ��
} TimeTimer;

//��ǰʱ��
static uint64 current_time;
//��ͨ����ʱ��
static TimeTimer timers[TIME_TIMER_CH_NUM];

//==========================================================================
//��������: time_init
//��������: ��
//����˵��: init_time:��ʼʱ�䣬��λms
//���ܸ�Ҫ: ʹ��ʱ��ģ��
//==========================================================================
void time_init(uint64 init_time) {
	uint8 i;	//�α�

	//�趨��ǰʱ��Ϊ��ʼʱ��
	current_time = init_time;
	//���ø�ͨ����δע��
	for (i = 0; i < TIME_TIMER_CH_NUM; ++i) {
		timers[i].flags &= ~0x3;
	}
}

//==========================================================================
//��������: time_update
//��������: ��
//����˵��: time:ʱ����������λms
//���ܸ�Ҫ: ���µ�ǰʱ��
//==========================================================================
void time_update(uint64 time) {
	uint8 i;	//�α�

	//���µ�ǰʱ��
	current_time += time;
	//���¸�ͨ����ʱ���ĵ������
	for (i = 0; i < TIME_TIMER_CH_NUM; ++i) {
		//��ͨ���ļ�ʱ����ע�ᣬ��δ����ʱ��
		if ((timers[i].flags & 0x3) == 0x1) {
			//���ʱ���Ƿ񵽴�
			if (current_time >= timers[i].start_time + timers[i].set_time) {
				timers[i].flags |= 0x2;
			}
		}
	}
}

//==========================================================================
//��������: time_get_time
//��������: ��ǰʱ�䣬��λms
//����˵��: ��
//���ܸ�Ҫ: ��ȡ��ǰʱ��
//==========================================================================
uint64 time_get_time() {
	//���ص�ǰʱ��
	return current_time;
}

//==========================================================================
//��������: time_register_timer
//��������: true:ע��ɹ�; false:ע��ʧ�ܣ�ͨ����Խ����߸�ͨ���Ѿ�ע��
//����˵��: timer_ch:��ʱ��ͨ���ţ�ȡֵ��ΧΪ[0, TIME_TIMER_CH_NUM - 1]
//         set_time:��ͨ����ʱ���ļ�ʱʱ�䣬��λms
//���ܸ�Ҫ: ע���ʱ��ͨ������ʹ��ͨ���ļ�ʱ����ʼ��ʱ
//==========================================================================
bool time_register_timer(uint8 timer_ch, uint64 set_time) {
	//���ͨ�����Ƿ�Խ��
	if (timer_ch >= TIME_TIMER_CH_NUM) {
		return false;
	}
	//���ͨ���Ƿ��Ѿ�ע��
	if ((timers[timer_ch].flags & 0x1) == 0x1) {
		return false;
	}
	//��ʱ��δ����ʱ��
	timers[timer_ch].flags &= ~0x2;
	//���ü�ʱʱ��
	timers[timer_ch].set_time = set_time;
	//���ÿ�ʼʱ��Ϊ��ǰʱ��
	timers[timer_ch].start_time = current_time;
	//��ͨ���Ѿ�ע��
	timers[timer_ch].flags |= 0x1;
	return true;
}

//==========================================================================
//��������: time_is_timer_arrived
//��������: true:�ѵ���ʱ��; false:δ����ʱ��
//����˵��: timer_ch:��ʱ��ͨ���ţ�ȡֵ��ΧΪ[0, TIME_TIMER_CH_NUM - 1]
//���ܸ�Ҫ: �鿴ָ��ͨ���ļ�ʱ���Ƿ��Ѿ�����ʱ�䣬����ʱ��󣬼�ʱ�����ټ�ʱ
//==========================================================================
bool time_is_timer_arrived(uint8 timer_ch) {
	//���ͨ�����Ƿ�Խ��
	if (timer_ch >= TIME_TIMER_CH_NUM) {
		return false;
	}
	//����ͨ���Ѿ�ע�ᣬ�Ҽ�ʱ���Ѿ�����ʱ�䣬����true�����򷵻�false
	if ((timers[timer_ch].flags & 0x3) == 0x3) {
		return true;
	} else {
		return false;
	}
}

//==========================================================================
//��������: time_restart_timer
//��������: true:�����ɹ�; false:����ʧ�ܣ�ͨ����Խ����߸�ͨ��δע��
//����˵��: timer_ch:��ʱ��ͨ���ţ�ȡֵ��ΧΪ[0, TIME_TIMER_CH_NUM - 1]
//���ܸ�Ҫ: ����ָ��ͨ���ļ�ʱ����ʹ�����¼�ʱ
//==========================================================================
bool time_restart_timer(uint8 timer_ch) {
	//���ͨ�����Ƿ�Խ��
	if (timer_ch >= TIME_TIMER_CH_NUM) {
		return false;
	}
	//���ͨ���Ƿ��Ѿ�ע��
	if ((timers[timer_ch].flags & 0x1) != 0x1) {
		return false;
	}
	//���ÿ�ʼʱ��Ϊ��ǰʱ��
	timers[timer_ch].start_time = current_time;
	//��ʱ��δ����ʱ��
	timers[timer_ch].flags &= ~0x2;
	return true;
}
