/*
		ָʾ�ƺͷ������߳�Դ�ļ�
		���ߣ�lee
		ʱ�䣺2019/4/17
*/
#include "main.h"

void Thread_led_beep (void const *argument);                             // thread function
osThreadId tid_Thread_led_beep;                                          // thread id
osThreadDef (Thread_led_beep, osPriorityNormal, 1, 0);                   // thread object
extern uint32_t led_Power_time;
/*
		��ʼ���̺߳���
*/
int Init_Thread_led_beep (void)
{

  tid_Thread_led_beep = osThreadCreate (osThread(Thread_led_beep), NULL);
  if (!tid_Thread_led_beep) return(-1);
  
  return(0);
}
/*
		�̺߳���
*/
uint32_t temp_led = 0;
void Thread_led_beep (void const *argument)  
{
	//�ֲ���������
	
	while(1)
	{
		temp_led++;
		if((BATTERY_LOW_V+100) > global_device_info.battery_voltage*1000)
		{
			if(global_flag.battery_low_flag == 0)
			{
				global_flag.battery_low_flag = 2;
			}
		}
		if(BATTERY_LOW_V > global_device_info.battery_voltage*1000)
		{
			led_run_mode(LED_RED, 1);
			printf("[power]��ص�ѹ����,����,��ѹΪ%fV\n",global_device_info.battery_voltage);
			led_beep_alarm();
			global_flag.battery_low_flag = 1;
		}
		else if((BATTERY_LOW_V+100) > global_device_info.battery_voltage*1000)
		{
			if(global_flag.battery_low_flag == 0)
			{
				global_flag.battery_low_flag = 2;
			}
			led_run_mode(LED_RED, 1);
		}
		else
		{
			led_run_mode(LED_GREEN, 1);
			global_flag.battery_low_flag = 0;
		}
		if((global_timer.system_timer_sec - led_Power_time) < 3)
		{
			led_Power_indicator();
		}
		else
		{
			led_Power_indicator_close();
		}
		if(1==global_flag.sleep_flag)
		{
			led_run_control(LED_OFF);
			global_sleep.led_beep_thread_sleep_done = 1;
			printf("[led beep]����׼�����\n");
			
			while(1==global_flag.sleep_flag)
			{
				osDelay(100);
			}
			
			led_run_control(LED_GREEN);
			global_sleep.led_beep_thread_sleep_done = 0;
			printf("[led beep]����\n");
		}
		
		if(0==global_flag.led_lock_flag)
		{
			led_run_blink(&global_led_run.color, &global_led_run.mode, 50);
		}
		osDelay(100);
	}
}
