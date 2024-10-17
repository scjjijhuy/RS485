#ifndef _STACK_H_
#define _STACK_H_

#include "common_c.h"

int stack_is_empty(void);
int stack_is_full(void);
int stack_push(void);
int stack_pop(void);
int stack_back(void);
void stack_clear(void);
int stack_len(void);
int stack_check(void);
int compare_vibration(const void* value1, const void* value2);

/*
	�ϴ����ݵ�װ��(�ڲ���)
*/
void load_upload_data(void);

/*
	��������־����
*/
int local_vibration_log_update(double vibration_x, double vibration_y, double vibration_z, struct time_struct time, double latitude, double longitude);

#endif
