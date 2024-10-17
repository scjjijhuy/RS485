#ifndef __NET_LOG_H__
#define __NET_LOG_H__

/*
	向日志文件添加一条记录
*/
int net_log_write(const char *buffer_in, ...);

/*
	保存波形数据
*/
int net_wave_write(void);

/*
	串口输出运行日志
*/
int net_log_print(void);

#endif

