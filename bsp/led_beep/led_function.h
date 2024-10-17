#ifndef _LED_FUNCTION_H_
#define _LED_FUNCTION_H_
#include "common_c.h"

/*
	run灯服务函数
*/
void led_run_blink(int *color, int *mode, int interval_ms);

/*
	gps灯服务函数
*/
void led_gps_blink(int *color, int *mode, int interval_ms);

/*
	gprs灯服务函数
*/
void led_gprs_blink(int *color, int *mode, int interval_ms);

/*
	run灯模式设定
*/
void led_run_mode(int color, int mode);

/*
	gps灯模式设定
*/
void led_gps_mode(int color, int mode);

/*
	gprs灯模式设定
*/
void led_gprs_mode(int color, int mode);

/*
	设备低电量报警
*/
void led_beep_alarm(void);

#endif
