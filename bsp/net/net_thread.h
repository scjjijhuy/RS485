#ifndef __NET_THREAD_H__
#define __NET_THREAD_H__

/*
		初始化线程函数
*/
int Init_Thread_net (void);
void restart_usart3_recv(void);
void rtc_config(void);
void USART3_tx_live(void);
#endif

