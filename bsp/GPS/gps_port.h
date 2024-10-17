#ifndef _GPS_PORT_H
#define _GPS_PORT_H

/*
	gps底层函数头文件
	作者：lee
	时间：2019/8/15
*/
struct gps_raw_struct
{
	//拆分后的字符串 
	char s_utc_time[32];
	char s_enable_state[2];
	char s_latitude[16];
	char s_NS[2];
	char s_longitude[16];
	char s_EW[2];
	char s_speed[16];
	char s_direction[16];
	char s_utc_date[32];
	char s_magnetic_declination[16];
	char s_magnetic_EW[2];
	char s_ADEN[2];
	char s_check_value[4];
};
/*
	gps开关控制
*/
extern uint8_t gps_data_switch;
int gps_power(int on_off);

int gps_analysis(char *recv_buf);

void gps_config(void);

#endif
