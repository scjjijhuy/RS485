#ifndef __NET_LOG_H__
#define __NET_LOG_H__

/*
	����־�ļ����һ����¼
*/
int net_log_write(const char *buffer_in, ...);

/*
	���沨������
*/
int net_wave_write(void);

/*
	�������������־
*/
int net_log_print(void);

#endif

