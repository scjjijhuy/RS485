#ifndef __RTC_PORT_H__
#define __RTC_PORT_H__

#include "main.h"

/*
	��ȡrtcʱ��
*/
int rtc_get_time(struct time_struct *time_out);
void RTC_Config(void);
#endif
