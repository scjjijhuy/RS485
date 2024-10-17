/*
	电源管理模块底层代码
	作者：lee
	时间：2019/5/15
*/
#include "main.h"
static uint16_t tx8025t_s_addr = 0x0032;
uint8_t Weekday_conv(uint8_t week_day)
{
	uint8_t iLoop = 0;
	for(iLoop = 0;iLoop < 7;iLoop++)
	{
		if((week_day & 0x01) != 0)
		{
			return iLoop;
		}
		week_day = week_day >> 1;
	}
	return iLoop;
}

int32_t rx8025t_SetDate(stc_rtc_date_t *pstcRtcDate)
{
	int32_t i32Ret = LL_OK;
	
	uint8_t w_data[2] = {0};
	
	w_data[0] = Years_Addr;
	w_data[1] = pstcRtcDate->u8Year;
	I2C_Master_Transmit(tx8025t_s_addr,w_data,2,10000);
	
	w_data[0] = Months_Addr;
	w_data[1] = pstcRtcDate->u8Month;
	I2C_Master_Transmit(tx8025t_s_addr,w_data,2,10000);
	
	w_data[0] = Days_Addr;
	w_data[1] = pstcRtcDate->u8Day;
	I2C_Master_Transmit(tx8025t_s_addr,w_data,2,10000);
	
	w_data[0] = Weekdays_Addr;
	w_data[1] = 1 << (pstcRtcDate->u8Weekday);
	I2C_Master_Transmit(tx8025t_s_addr,w_data,2,10000);
	
	return i32Ret;
}
//时间
int32_t rx8025t_GetDataTime(stc_rtc_date_t *pstcRtcDate,stc_rtc_time_t *pstcRtcTime)
{
	int32_t i32Ret = LL_OK;
	uint8_t r_data[8] = {0};
	
	r_data[0] = Seconds_Addr;
	I2C_Master_Receive_Simplified(tx8025t_s_addr,r_data[0],r_data+1,7,TIMEOUT);
	pstcRtcTime->u8Second = RTC_BCD2DEC(r_data[1]);
	pstcRtcTime->u8Minute = RTC_BCD2DEC(r_data[2]);
	pstcRtcTime->u8Hour = RTC_BCD2DEC(r_data[3]);
	pstcRtcDate->u8Weekday = Weekday_conv(r_data[4]);
	pstcRtcDate->u8Day = RTC_BCD2DEC(r_data[5]);
	pstcRtcDate->u8Month = RTC_BCD2DEC(r_data[6]);
	pstcRtcDate->u8Year = RTC_BCD2DEC(r_data[7]);
	return i32Ret;
}
int32_t rx8025t_SetTime(stc_rtc_time_t *pstcRtcTime)
{
	int32_t i32Ret = LL_OK;
	
	uint8_t w_data[2] = {0};
	
	w_data[0] = Hours_Addr;
	w_data[1] = pstcRtcTime->u8Hour;
	I2C_Master_Transmit(tx8025t_s_addr,w_data,2,10000);
	
	w_data[0] = Minutes_Addr;
	w_data[1] = pstcRtcTime->u8Minute;
	I2C_Master_Transmit(tx8025t_s_addr,w_data,2,10000);
	
	w_data[0] = Seconds_Addr;
	w_data[1] = pstcRtcTime->u8Second;
	I2C_Master_Transmit(tx8025t_s_addr,w_data,2,10000);
	return i32Ret;
}
uint32_t rx8025t_init(uint8_t u8Format)
{
	int32_t i32Ret = LL_OK;

	return i32Ret;
}

void rx8025t_test(void)
{
//	uint8_t r_data[8] = {0};
//	r_data[0] = Seconds_Addr;
//	I2C_Master_Receive_Simplified(tx8025t_s_addr,r_data[0],r_data+1,7,TIMEOUT);

}
