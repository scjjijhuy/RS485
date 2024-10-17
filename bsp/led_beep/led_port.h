#ifndef _LED_PORT_H_
#define _LED_PORT_H_

/*
	run灯控制函数
*/
void led_run_control(int color);

/*
	gps灯控制函数
*/
void led_gps_control(int color);

/*
	gprs灯控制函数
*/
void led_gprs_control(int color);

/*
	电量指示灯
*/
void led_Power_indicator(void);

void led_Power_indicator_close(void);

#endif

