/*
		网络线程源文件
		作者：lee
		时间：2019/12/23
*/

#include "main.h"
#define mavlink_test
void Thread_net (void const *argument);                             // thread function
osThreadId tid_Thread_net;                                          // thread id
osThreadDef (Thread_net, osPriorityNormal, 1, 0);                   // thread object

struct mqtt_message_struct mqtt_message = {0}; 
struct mqtt_storage_struct mqtt_storage_array[6] = {0};
char net_recv_buffer[1024];
extern char usart3_recv_buffer[1024];

//static char wave_file_name[128];
/*
		初始化线程函数
*/
int Init_Thread_net (void)
{
  tid_Thread_net = osThreadCreate (osThread(Thread_net), NULL);
  if (!tid_Thread_net) return(-1);
  
  return(0);
}

/*
	重新启动串口二接收
*/
void restart_usart3_recv(void)
{
    uint32_t num = 0;
	
	num = DMA_GetTransCount(CM_DMA1,DMA_CH3);
	if(num != 1023)
	{
		memset(usart3_recv_buffer,0,sizeof(usart3_recv_buffer));
		DMA_receive_IT(CM_USART3,CM_DMA1,DMA_CH3,(uint32_t)(&net_recv_buffer[0]),sizeof(net_recv_buffer)-1);
		(void)USART_ReadData(CM_USART3);
	}

}
void USART3_tx_live(void)
{
	  char temp_string[64]={0};
	  memset(net_send_buffer, 0, sizeof(net_send_buffer));
	  if(0==global_gps_out.valid)
	{
		if((gps_raw.s_enable_state[0] != '?') && (gps_raw.s_latitude[0] != '?')  &&
		   (gps_raw.s_NS[0] != '?')           && (gps_raw.s_longitude[0] != '?') &&
		   (gps_raw.s_EW[0] != '?')           && (gps_raw.s_speed[0] != '?'))
		{
			sprintf(temp_string,"lng%f%s",global_gps_out.longitude,global_gps_out.EW);
			strcat(net_send_buffer, temp_string);
			sprintf(temp_string,";");
			strcat(net_send_buffer, temp_string);
			sprintf(temp_string,"lat%f%s",global_gps_out.latitude,global_gps_out.NS);
			strcat(net_send_buffer, temp_string);
			
			sprintf(temp_string,";");
			strcat(net_send_buffer, temp_string);
		}
		else
		{
		//定位无效
			sprintf(temp_string,"null;null;");
			strcat(net_send_buffer, temp_string);
		}
	}
	else
	{
		  sprintf(temp_string,"lng%f%s",global_gps_out.longitude,global_gps_out.EW);
			strcat(net_send_buffer, temp_string);
			sprintf(temp_string,";");
			strcat(net_send_buffer, temp_string);
			sprintf(temp_string,"lat%f%s",global_gps_out.latitude,global_gps_out.NS);
			strcat(net_send_buffer, temp_string);
			
			sprintf(temp_string,";");
			strcat(net_send_buffer, temp_string);
	}
	
	sprintf(temp_string,"bat%d", global_upload_data.dump_energy);
	strcat(net_send_buffer, temp_string);
	sprintf(temp_string,";");
	strcat(net_send_buffer, temp_string);
	
	sprintf(temp_string,"%02d-%02d-%02d %02d:%02d:%02d",\
					global_device_info.device_time.year, global_device_info.device_time.month, global_device_info.device_time.day, \
					global_device_info.device_time.hour,global_device_info.device_time.minute,global_device_info.device_time.second);
	strcat(net_send_buffer, temp_string); 
}
/*
		线程函数
*/
uint32_t temp_thr = 0;
extern uint8_t mavlink_param_request_list_type;
extern float pos_e,pos_n,pos_u;
uint8_t mavlink_msg[256];
void Thread_net(void const *argument) 
{
	
	if(timer_0A == 0)
	{
		  NVIC_SystemReset();
	}
	while(1)
	{		
			temp_thr++;

		  osDelay(10);

	}
}

