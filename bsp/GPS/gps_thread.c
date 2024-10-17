/*
		gps线程源文件
		作者：lee
		时间：2019/8/15
*/
#include "main.h"

void Thread_gps (void const *argument);                             // thread function
osThreadId tid_Thread_gps;                                          // thread id
osThreadDef (Thread_gps, osPriorityNormal, 1, 0);                   // thread object
uint8_t gps_Factory_settings_state = 0;
char gps_recv_buf[1024];
/*
		初始化线程函数
*/
int Init_Thread_gps (void)
{

  tid_Thread_gps = osThreadCreate (osThread(Thread_gps), NULL);
  if (!tid_Thread_gps) return(-1);
  
  return(0);
}
void restart_usart2_recv(void)
{
	uint32_t num = 0;
	
	num = DMA_GetTransCount(CM_DMA1,DMA_CH2);
	if(num != 1024)
	{
		memset(gps_recv_buf,0,sizeof(gps_recv_buf));
		DMA_receive_IT(CM_USART2,CM_DMA1,DMA_CH2,(uint32_t)(&gps_recv_buf[0]),1024);
		(void)USART_ReadData(CM_USART2);
	}
}
/*
		线程函数
*/
uint32_t temp_gps = 0,gps_time_out = 0;
void Thread_gps (void const *argument) 
{
	
	while (1)
	{
		temp_gps++;
//		if(global_flag.sleep_flag == 1)
//		{
//			gps_power_switch(DISABLE);
//			osDelay(200);
//			continue;
//		}
//		else
//		{
//			
//		}
		if(1==global_flag.sleep_flag)
		{
			gps_power_switch(DISABLE);
			global_sleep.gps_thread_sleep_done = 1;
			printf("[gps]休眠准备完毕\n");
			
			while(1==global_flag.sleep_flag)
			{
				global_timer.net_timer_ms = 0;
				osDelay(100);
			}
			
			global_sleep.gps_thread_sleep_done = 0;
			printf("[gps]唤醒\n");
		}
//		restart_usart2_recv();
		gps_power_switch(ENABLE);
		gps_analysis(gps_recv_buf);
		if(0!=global_gps_out.valid)
		{

		}
		else
		{
			global_gps_out.latitude = 0;
			global_gps_out.longitude = 0;
			global_gps_out.direction = 0;
			global_gps_out.speed = 0;
			global_gps_out.valid=0;
		}
		
		if(global_gps_out.max_speed <= global_gps_out.speed && 200>global_gps_out.speed)
		{
			global_gps_out.max_speed = global_gps_out.speed;
		}
		
		global_gps_out.last_speed = global_gps_out.speed;
		
		osDelay(200);
		if(gps_Factory_settings_state == 1)
		{
			gps_Factory_settings_state = 0;
			gps_config();
		}
	}
}
