#ifndef _POWER_PORT_H_
#define _POWER_PORT_H_

/*
	��ȡ��ص����ٷֱ�
*/
int power_battery_percent(int *percent_out, double *voltage_out);
int power_battery_percent_once(int *percent_out, double *voltage_out);
/*
	����豸�Ƿ���Ҫ��������
*/
int power_sleep_check(void);

/*
	����豸�Ƿ���Ҫ�ػ�
*/
int power_down_check(void);

#endif
