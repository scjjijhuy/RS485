#ifndef _RX8025T_H_
#define _RX8025T_H_
#include "main.h"
#define RX8025T

#define Seconds_Addr    		0x00
#define Minutes_Addr    		0x01
#define Hours_Addr      		0x02
#define Weekdays_Addr   		0x03
#define Days_Addr       		0x04
#define Months_Addr     		0x05
#define Years_Addr      		0x06
#define Digital_Addr    		0x07
#define Alarm_W_Minute_Addr		0x08
#define Alarm_W_Hour_Addr		0x09
#define Alarm_W_Weekday_Addr	0x0A
#define Alarm_D_Minute_Addr		0x0B
#define Alarm_D_Hour_Addr		0x0C
#define Control_1_Addr			0x0E
#define Control_2_Addr			0x0F

uint32_t rx8025t_init(uint8_t u8Format);
int32_t rx8025t_SetTime(stc_rtc_time_t *pstcRtcTime);
int32_t rx8025t_SetDate(stc_rtc_date_t *pstcRtcDate);
int32_t rx8025t_GetDataTime(stc_rtc_date_t *pstcRtcDate,stc_rtc_time_t *pstcRtcTime);
void rx8025t_test(void);
#endif
