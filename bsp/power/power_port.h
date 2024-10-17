#ifndef _POWER_PORT_H_
#define _POWER_PORT_H_

/*
	读取电池电量百分比
*/
int power_battery_percent(int *percent_out, double *voltage_out);
int power_battery_percent_once(int *percent_out, double *voltage_out);
/*
	检查设备是否需要进入休眠
*/
int power_sleep_check(void);

/*
	检查设备是否需要关机
*/
int power_down_check(void);

#endif
