#ifndef _LED_FUNCTION_H_
#define _LED_FUNCTION_H_
#include "common_c.h"

/*
	run�Ʒ�����
*/
void led_run_blink(int *color, int *mode, int interval_ms);

/*
	gps�Ʒ�����
*/
void led_gps_blink(int *color, int *mode, int interval_ms);

/*
	gprs�Ʒ�����
*/
void led_gprs_blink(int *color, int *mode, int interval_ms);

/*
	run��ģʽ�趨
*/
void led_run_mode(int color, int mode);

/*
	gps��ģʽ�趨
*/
void led_gps_mode(int color, int mode);

/*
	gprs��ģʽ�趨
*/
void led_gprs_mode(int color, int mode);

/*
	�豸�͵�������
*/
void led_beep_alarm(void);

#endif
