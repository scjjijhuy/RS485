#ifndef __NET_POWER_H__
#define __NET_POWER_H__

#include "common_c.h"

/*
	EC20��������
	1:�������� 0:����������
*/
int net_power_config(int sleep_enable_in);

/*
	EC20����ʹ��
	1:�������� 2:�˳�����
*/
int net_power_sleep(int enable_in);

#endif

