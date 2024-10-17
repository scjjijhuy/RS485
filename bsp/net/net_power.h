#ifndef __NET_POWER_H__
#define __NET_POWER_H__

#include "common_c.h"

/*
	EC20休眠配置
	1:允许休眠 0:不允许休眠
*/
int net_power_config(int sleep_enable_in);

/*
	EC20休眠使能
	1:进入休眠 2:退出休眠
*/
int net_power_sleep(int enable_in);

#endif

