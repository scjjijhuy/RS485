/*
	rtc底层代码
	作者：lee
	时间：2020/11/27
*/

#include "main.h"
uint8_t u8SecIntFlag = 0U;
void read_RTC_time(struct time_struct* time)
{
	stc_rtc_date_t stcCurrentDate;
    stc_rtc_time_t stcCurrentTime;
	if(LL_OK == RTC_GetDate(RTC_DATA_FMT_DEC, &stcCurrentDate))
	{
		time->year    = stcCurrentDate.u8Year;
		time->month   = stcCurrentDate.u8Month;
		time->day     = stcCurrentDate.u8Day;
		time->weekday = stcCurrentDate.u8Weekday;
		if(LL_OK == RTC_GetTime(RTC_DATA_FMT_DEC, &stcCurrentTime))
		{
			time->hour = stcCurrentTime.u8Hour;
			time->minute = stcCurrentTime.u8Minute;
			time->second = stcCurrentTime.u8Second;
		}
	}
	
}
//extern RTC_HandleTypeDef hrtc;
static void RTC_Period_IrqCallback(void)
{
    u8SecIntFlag = 1U;
//	global_timer.net_timer_ms++;
//	global_timer.gnss_timer_ms++; 
//	global_timer.system_timer++;
//	read_RTC_time(&global_device_info.device_time);
//    RTC_ClearStatus(RTC_INT_PERIOD);
}
static void RTC_CalendarConfig(void)
{
    stc_rtc_date_t stcRtcDate;
    stc_rtc_time_t stcRtcTime;

    /* Date configuration */
    stcRtcDate.u8Year    = 20U;
    stcRtcDate.u8Month   = RTC_MONTH_JANUARY;
    stcRtcDate.u8Day     = 1U;
    stcRtcDate.u8Weekday = RTC_WEEKDAY_WEDNESDAY;

    /* Time configuration */
    stcRtcTime.u8Hour   = 23U;
    stcRtcTime.u8Minute = 59U;
    stcRtcTime.u8Second = 55U;
    stcRtcTime.u8AmPm   = RTC_HOUR_12H_AM;

    if (LL_OK != RTC_SetDate(RTC_DATA_FMT_DEC, &stcRtcDate)) 
	{
		printf("Set Date failed!\r\n");
//        DDL_Printf("Set Date failed!\r\n");
    }

    if (LL_OK != RTC_SetTime(RTC_DATA_FMT_DEC, &stcRtcTime)) 
	{
//        DDL_Printf("Set Time failed!\r\n");
		printf("Set Date failed!\r\n");
    }
}
void RTC_Config(void)
{
    int32_t i32Ret;
    stc_rtc_init_t stcRtcInit;
    stc_irq_signin_config_t stcIrqSignConfig;

    /* RTC period interrupt configure */
    stcIrqSignConfig.enIntSrc    = INT_SRC_RTC_PRD;
    stcIrqSignConfig.enIRQn      = INT002_IRQn;
    stcIrqSignConfig.pfnCallback = &RTC_Period_IrqCallback;
    (void)INTC_IrqSignOut(stcIrqSignConfig.enIRQn);
    i32Ret = INTC_IrqSignIn(&stcIrqSignConfig);
    if (LL_OK != i32Ret) {
        /* check parameter */
        for (;;) {
        }
    }

    /* Clear pending */
    NVIC_ClearPendingIRQ(stcIrqSignConfig.enIRQn);
    /* Set priority */
    NVIC_SetPriority(stcIrqSignConfig.enIRQn, DDL_IRQ_PRIO_DEFAULT);
    /* Enable NVIC */
    NVIC_EnableIRQ(stcIrqSignConfig.enIRQn);

    /* RTC stopped */
    if (DISABLE == RTC_GetCounterState()) {
        /* Reset RTC counter */
        if (LL_ERR_TIMEOUT == RTC_DeInit()) {
//            DDL_Printf("Reset RTC failed!\r\n");
        } else {
            /* Configure structure initialization */
            (void)RTC_StructInit(&stcRtcInit);

            /* Configuration RTC structure */
            stcRtcInit.u8ClockSrc   = RTC_CLK_SRC_XTAL32;
            stcRtcInit.u8HourFormat = RTC_HOUR_FMT_24H;
            stcRtcInit.u8IntPeriod  = RTC_INT_PERIOD_PER_SEC;
            (void)RTC_Init(&stcRtcInit);

            /* Update date and time */
            RTC_CalendarConfig();
            /* Enable period interrupt */
            RTC_IntCmd(RTC_INT_PERIOD, ENABLE);
            /* Startup RTC count */
            RTC_Cmd(ENABLE);
        }
    }
}
/*
	读取rtc时间
*/
int rtc_get_time(struct time_struct *time_out)
{
	struct rtc_get_time_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}rtc_get_time_ret = {0,1};
	
	//局部变量
	stc_rtc_time_t myTime={0};
	stc_rtc_date_t myDate={0};
	
	//从RTC中获取时间
//	HAL_RTC_GetTime(&hrtc, &myTime, RTC_FORMAT_BIN);
//	HAL_RTC_GetDate(&hrtc, &myDate, RTC_FORMAT_BIN);
	if(LL_OK == RTC_GetDate(RTC_DATA_FMT_DEC, &myDate))
	{
		time_out->year    = myDate.u8Year;
		time_out->month   = myDate.u8Month;
		time_out->day     = myDate.u8Day;
		time_out->weekday = myDate.u8Weekday;
		if(LL_OK == RTC_GetTime(RTC_DATA_FMT_DEC, &myTime))
		{
			time_out->hour = myTime.u8Hour;
			time_out->minute = myTime.u8Minute;
			time_out->second = myTime.u8Second;
		}
	}
//	time_out->hour		= myTime.u8Hour%24;
//	time_out->minute	= myTime.u8Minute%60;
//	time_out->second	= myTime.u8Second%60;
//	time_out->year 		= myDate.u8Year;
//	time_out->month 	= myDate.u8Month;
//	time_out->day 		= myDate.u8Day;
//	time_out->weekday = myDate.u8Weekday;
	
	//函数返回
	return rtc_get_time_ret.RET_OK;
}
