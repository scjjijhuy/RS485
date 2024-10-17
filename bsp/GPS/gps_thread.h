#ifndef _GPS_THREAD_H_
#define _GPS_THREAD_H_

/*
		初始化线程函数
*/
#include "main.h"
extern struct gps_raw_struct gps_raw;
extern uint32_t temp_gps,gps_time_out;
int Init_Thread_gps (void);
void restart_usart2_recv(void);
#endif
