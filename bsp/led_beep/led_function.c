/*
	led功能函数代码
	作者:lee
	时间：2019/5/17
*/

#include "main.h"

/*
	run灯服务函数
*/
void led_run_blink(int *color, int *mode, int interval_ms)
{
	//局部变量
	static int time_ms=0;
	static int count=0;
	
	time_ms+=interval_ms;
	if(0 == time_ms%100)
	{
		count+=1;
		if(11<count)
		{
			count=0;
			time_ms=0;
		}
	}
	
	if(*mode<=count)
	{
		led_run_control(LED_OFF);
		return;
	}

	if(0==time_ms%50 && 0!=time_ms%100)
	{
		led_run_control(LED_OFF);
	}
	else if(0==time_ms%50 && 0==time_ms%100)
	{
		led_run_control(*color);
	}
}

/*
	gps灯服务函数
*/
//void led_gps_blink(int *color, int *mode, int interval_ms)
//{
//	//局部变量
//	static int time_ms=0;
//	static int count=0;
//	
//	time_ms+=interval_ms;
//	if(0 == time_ms%100)
//	{
//		count+=1;
//		if(11<count)
//		{
//			count=0;
//			time_ms=0;
//		}
//	}
//	
//	if(*mode<=count)
//	{
//		led_gps_control(LED_OFF);
//		return;
//	}

//	if(0==time_ms%50 && 0!=time_ms%100)
//	{
//		led_gps_control(LED_OFF);
//	}
//	else if(0==time_ms%50 && 0==time_ms%100)
//	{
//		led_gps_control(*color);
//	}
//}

/*
	gprs灯服务函数
*/
//void led_gprs_blink(int *color, int *mode, int interval_ms)
//{
//	//局部变量
//	static int time_ms=0;
//	static int count=0;
//	
//	time_ms+=interval_ms;
//	if(0 == time_ms%100)
//	{
//		count+=1;
//		if(11<count)
//		{
//			count=0;
//			time_ms=0;
//		}
//	}
//	
//	if(*mode<=count)
//	{
//		led_gprs_control(LED_OFF);
//		return;
//	}

//	if(0==time_ms%50 && 0!=time_ms%100)
//	{
//		led_gprs_control(LED_OFF);
//	}
//	else if(0==time_ms%50 && 0==time_ms%100)
//	{
//		led_gprs_control(*color);
//	}
//}

/*
	run灯模式设定
*/
void led_run_mode(int color, int mode)
{
	global_led_run.color = color;
	global_led_run.mode = mode;
}

/*
	gps灯模式设定
*/
//void led_gps_mode(int color, int mode)
//{
//	global_led_gps.color = color;
//	global_led_gps.mode = mode;
//}

///*
//	gprs灯模式设定
//*/
//void led_gprs_mode(int color, int mode)
//{
//	global_led_gprs.color = color;
//	global_led_gprs.mode = mode;
//}

/*
	设备低电量报警
*/
void led_beep_alarm(void)
{
	int i=0;
	
	for(i=0;i<3;i++)
	{
		led_run_control(LED_RED);
//		led_gps_control(LED_RED);
//		led_gprs_control(LED_RED);
		beep(1);
		
		osDelay(100);
		
		led_run_control(LED_OFF);
//		led_gps_control(LED_OFF);
//		led_gprs_control(LED_OFF);
		beep(0);
		
		osDelay(100);
		
		led_run_control(LED_RED);
//		led_gps_control(LED_RED);
//		led_gprs_control(LED_RED);
		beep(1);
		
		osDelay(500);
		
		led_run_control(LED_OFF);
//		led_gps_control(LED_OFF);
//		led_gprs_control(LED_OFF);
		beep(0);
		
		osDelay(500);
	}
}
