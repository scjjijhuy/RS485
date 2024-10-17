/*
		rtc�߳�Դ�ļ�
		���ߣ�lee
		ʱ�䣺2019/4/17
*/
#include "main.h"

void Thread_rtc (void const *argument);                             // thread function
osThreadId tid_Thread_rtc;                                          // thread id
osThreadDef (Thread_rtc, osPriorityNormal, 1, 0);                   // thread object


/*
		��ʼ���̺߳���
*/
int Init_Thread_rtc (void)
{

  tid_Thread_rtc = osThreadCreate (osThread(Thread_rtc), NULL);
  if (!tid_Thread_rtc) return(-1);
  
  return(0);
}

/*
		�̺߳���
*/
uint32_t temp_rtc = 0;
void Thread_rtc (void const *argument) 
{
	stc_rtc_time_t myTime;
	stc_rtc_date_t myDate;
	struct tm time_tm={0};
	struct time_struct last_time={0};
	while(1)
	{
		temp_rtc++;
		
//		if(1==global_flag.sleep_flag)
//		{
//			global_sleep.rtc_thread_sleep_done = 1;
//			printf("[rtc]����׼�����\n");
//			
//			while(1==global_flag.sleep_flag)
//			{
//				osDelay(100);
//			}
//			
//			global_sleep.rtc_thread_sleep_done = 0;
//			printf("[rtc]����\n");
//		}
		
		//����Ƿ���Ҫ����ʱ��
		if(1 == global_rtc_set_time.need_set)
		{
			global_rtc_set_time.need_set = 0;
			
			printf("\n[rtc]��Ҫ��ʱ������Ϊ%04d-%02d-%02d %02d:%02d:%02d ����%d\n",\
							global_rtc_set_time.time_for_set.year+2000,\
							global_rtc_set_time.time_for_set.month,\
							global_rtc_set_time.time_for_set.day,\
							global_rtc_set_time.time_for_set.hour,\
							global_rtc_set_time.time_for_set.minute,\
							global_rtc_set_time.time_for_set.second,\
							global_rtc_set_time.time_for_set.weekday);
			
			myDate.u8Year = global_rtc_set_time.time_for_set.year % 100;
			myDate.u8Month = global_rtc_set_time.time_for_set.month % 13;
			myDate.u8Day = global_rtc_set_time.time_for_set.day % 32;
			myDate.u8Weekday = global_rtc_set_time.time_for_set.weekday % 8;
			if(myDate.u8Weekday == 7)
			{
				myDate.u8Weekday = 0;
			}
			myTime.u8Hour = global_rtc_set_time.time_for_set.hour % 24;
			myTime.u8Minute = global_rtc_set_time.time_for_set.minute % 60;
			myTime.u8Second = global_rtc_set_time.time_for_set.second % 60;
			myTime.u8AmPm   = RTC_HOUR_24H;
//			HAL_RTC_SetTime(&hrtc, &myTime, RTC_FORMAT_BIN);
//			HAL_RTC_SetDate(&hrtc, &myDate, RTC_FORMAT_BIN);
//			printf("\n[rtc]�ѽ�ʱ������Ϊ%04d-%02d-%02d %02d:%02d:%02d ����%d\n",\
//							myDate.Year+2000, myDate.Month, myDate.Date, myTime.Hours, myTime.Minutes, myTime.Seconds, myDate.WeekDay);
			if (LL_OK != RTC_SetDate(RTC_DATA_FMT_DEC, &myDate)) {
//				DDL_Printf("Set Date failed!\r\n");
				printf("Set Date failed!\r\n");
			}

			if (LL_OK != RTC_SetTime(RTC_DATA_FMT_DEC, &myTime)) {
//				DDL_Printf("Set Time failed!\r\n");
				printf("Set Date failed!\r\n");
			}
			rx8025t_SetDate(&myDate);
			rx8025t_SetTime(&myTime);
		}
		RTC_GetDate(RTC_DATA_FMT_DEC, &myDate);
		RTC_GetTime(RTC_DATA_FMT_DEC, &myTime);
		if(myDate.u8Year < 21)
		{
			rx8025t_GetDataTime(&myDate,&myTime);
		}
		//��RTC�л�ȡʱ��
//		HAL_RTC_GetTime(&hrtc, &myTime, RTC_FORMAT_BIN);
//		HAL_RTC_GetDate(&hrtc, &myDate, RTC_FORMAT_BIN);
		
		//�ж�Сʱ���Ƿ����24,stm32l476��RTC��BUG���Դ˹��
		if(24<=myTime.u8Hour)
		{
			global_flag.net_get_time_flag=1;
			continue;
		}
		//�ж�Сʱ���Ƿ��12������1�㣬�Դ˹��
		if(12==last_time.hour && 1==myTime.u8Hour)
		{
			global_flag.net_get_time_flag=1;
			//continue;
		}
		//�ж�12�㵽13�������Ƿ��һ�죬�Դ˹��
		if(last_time.day != myDate.u8Day && 12==(myTime.u8Hour%24))
		{
			global_flag.net_get_time_flag=1;
			//continue;
		}
		
		global_device_info.device_time.hour		= myTime.u8Hour % 24;
		global_device_info.device_time.minute	= myTime.u8Minute % 60;
		global_device_info.device_time.second	= myTime.u8Second % 60;
		global_device_info.device_time.year 	= myDate.u8Year % 100;
		global_device_info.device_time.month 	= myDate.u8Month % 13;
		global_device_info.device_time.day 		= myDate.u8Day % 32;
		global_device_info.device_time.weekday=	myDate.u8Weekday % 8;
		
		//ת��������
		time_tm.tm_year = global_device_info.device_time.year +2000 - 1900;
		time_tm.tm_mon = global_device_info.device_time.month-1;
		time_tm.tm_mday = global_device_info.device_time.day;
		time_tm.tm_hour = global_device_info.device_time.hour;
		time_tm.tm_min = global_device_info.device_time.minute;
		time_tm.tm_sec = global_device_info.device_time.second;
		
		global_device_info.time_stamp = mktime(&time_tm);
		
		last_time = global_device_info.device_time;
		osDelay(1000);
		rx8025t_test();
	}
}

