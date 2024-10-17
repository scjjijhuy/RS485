/*
	led底层代码源文件
	作者：lee
	时间: 2019/5/13
*/

#include "main.h"

/*
	run灯控制函数
*/
void led_run_control(int color)
{
	if(LED_OFF == color)
	{
		LED_GREEN_OFF();
		LED_RED_OFF();
		LED_YELLOW_OFF();
	}
	if(LED_RED == color)
	{
		LED_GREEN_OFF();
		LED_RED_ON();
		LED_YELLOW_OFF();
	}
	if(LED_GREEN == color)
	{
		LED_GREEN_ON();
		LED_RED_OFF();
		LED_YELLOW_OFF();
	}
	if(LED_ORANGE == color)
	{
		LED_GREEN_OFF();
		LED_RED_OFF();
		LED_YELLOW_ON();
	}
}


void led_Power_indicator_close(void)
{
//	LED_BAT_1_OFF();
	LED_BAT_2_OFF();
//	LED_BAT_3_OFF();
//	LED_BAT_4_OFF();   
	LED_BAT_5_OFF();   
}
void led_Power_indicator(void)
{
//	if(global_device_info.dump_energy >= 99)
//	{
////		LED_BAT_1_OFF();
//		LED_BAT_2_ON();
////		LED_BAT_3_ON();
////		LED_BAT_4_ON();   
//		LED_BAT_5_ON(); 
//	}
//	else if((global_device_info.dump_energy >= 75) && (global_device_info.dump_energy < 99))
//	{
////		LED_BAT_1_OFF();
//		LED_BAT_2_ON();
////		LED_BAT_3_ON();
////		LED_BAT_4_ON();   
//		LED_BAT_5_OFF(); 
//	}
//	else if((global_device_info.dump_energy >= 50) && (global_device_info.dump_energy < 75))
//	{
////		LED_BAT_1_OFF();
//		LED_BAT_2_ON();
////		LED_BAT_3_ON();
////		LED_BAT_4_OFF();   
//		LED_BAT_5_OFF(); 
//	}
//	else if((global_device_info.dump_energy >= 25) && (global_device_info.dump_energy < 50))
//	{
////		LED_BAT_1_OFF();
//		LED_BAT_2_ON();
////		LED_BAT_3_OFF();
////		LED_BAT_4_OFF();   
//		LED_BAT_5_OFF(); 
//	}
//	else
//	{
////		LED_BAT_1_ON();
//		LED_BAT_2_OFF();
////		LED_BAT_3_OFF();
////		LED_BAT_4_OFF();   
//		LED_BAT_5_OFF(); 
//	}
	
}
