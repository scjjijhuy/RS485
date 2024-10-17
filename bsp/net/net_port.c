/*
	网络模块底层头文件
	作者：lee
	时间：2019/12/23
*/

#include "main.h"

//extern UART_HandleTypeDef huart4;
//extern DMA_HandleTypeDef hdma_uart4_rx;
//extern DMA_HandleTypeDef hdma_uart4_tx;

extern char net_recv_buffer[1024];
extern char net_send_buffer[1024];

/*
	EC20重启
*/
int net_ec20_restart(int timeout_ms)
{
	struct net_ec20_restart_ret_struct
	{
		const int RET_OK;
		const int RET_TIMEOUT;
	}net_ec20_restart_ret = {0,1};
	
	//局部变量
	int i=0;
	int timeout_timer=0;
	
	//控制重启引脚
	printf("[net]EC20重启\n");
	net_ec20_power(0);
	net_ec20_power(1);
	
	//准备接收RDY
	restart_usart3_recv();
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&net_send_buffer[0]),strlen(net_send_buffer));
	
	//检查返回值
	for(timeout_timer=0;timeout_timer<timeout_ms;timeout_timer+=10)
	{
		osDelay(10);
		
		//去掉开头的无效字符
		for(i=0;i<sizeof(net_recv_buffer);i++)
		{
			if(NULL!=strstr(net_recv_buffer+i,"RDY"))
			{
				printf("%s\n",net_recv_buffer+i);
				printf("[net]EC20重启完毕\n");
				return net_ec20_restart_ret.RET_OK;
			}
		}
	}
	
	//运行到这则超时
	printf("[net]EC20重启超时，没有返回RDY\n");
	printf("%s\n",net_recv_buffer);
	
	return net_ec20_restart_ret.RET_TIMEOUT;
}

/*
	EC20开关
*/
int net_ec20_power(int on_off)
{
	struct net_ec20_power_ret_struct
	{
		const int RET_OK;
	}net_ec20_power_ret = {0};
	
	if(1==on_off)
	{
		restart_usart3_recv();
		NET_POWER_ON();
		NET_DTR_OFF();//关闭休眠状态
		printf("打开EC20\n");
		global_flag.net_power_on = 1;
	}
	else
	{
		global_flag.net_power_on = 0;
		NET_POWER_OFF();
		NET_DTR_OFF();//这个控制休眠的引脚漏电，所以关EC20时候必须把它置低
		osDelay(5000);
		printf("关闭EC20\n");
	}
	
	//函数返回
	return net_ec20_power_ret.RET_OK;
}

/*
	发送指令并检查是否返回OK
*/
int net_at_send(char *net_send_buffer ,int timeout_ms)
{
	struct net_at_send_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
		const int RET_TIMEOUT;
	}net_at_send_ret = {0,1,2};
	
	//局部变量
	int timeout_timer=0;
	
	//检查指针
	if(NULL == net_send_buffer)
	{
		printf("[net] net_at_send error. file=%s, line=%d\n",__FILE__,__LINE__);
		return net_at_send_ret.RET_OK;
	}
	
	//清空接收缓冲区
	restart_usart3_recv();
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&net_send_buffer[0]),strlen(net_send_buffer));
	
	for(timeout_timer=0;timeout_timer<timeout_ms;timeout_timer+=10)
	{
		osDelay(10);
		if(NULL!=strstr(net_recv_buffer,"OK"))
		{
			printf("%s\n",net_recv_buffer);
			return net_at_send_ret.RET_OK;
		}
		if(NULL!=strstr(net_recv_buffer,"ERROR"))
		{
			printf("%s\n",net_recv_buffer);
			return net_at_send_ret.RET_ERROR;
		}
	}
	return net_at_send_ret.RET_TIMEOUT;
	//运行到这则超时
	
}

/*
	AT+QMTCFG 配置MQTT
*/
int net_mqtt_configrate(int client_idx, struct mqtt_config_struct *mqtt_config)
{
	struct net_mqtt_configrate_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_mqtt_configrate_ret = {0,1};
	
	//局部变量
	int ret=0;
	
	//先检查指针
	if(NULL == mqtt_config)
	{
		printf("[net] net_mqtt_configrate error. file=%s, line=%d",__FILE__,__LINE__);
		return net_mqtt_configrate_ret.RET_ERROR;
	}
	
	//检查client_idx
	if(0>client_idx || 5<client_idx)
	{
		printf("[net] net_mqtt_configrate error. file=%s, line=%d",__FILE__,__LINE__);
		return net_mqtt_configrate_ret.RET_ERROR;
	}
	
	//配置MQTT版本
	if(3!=global_mqtt_config.mqtt_version && 4!=global_mqtt_config.mqtt_version)
	{
		printf("[net] net_mqtt_configrate error. file=%s, line=%d",__FILE__,__LINE__);
		return net_mqtt_configrate_ret.RET_ERROR;
	}
	
	sprintf(net_send_buffer,"AT+QMTCFG=\"version\",%d,%d\r",client_idx,global_mqtt_config.mqtt_version);
	
	ret = net_at_send(net_send_buffer,1000);
	if(0!=ret)
	{
		return net_mqtt_configrate_ret.RET_ERROR;
	}
	
	//配置pdpcid
	if(1>global_mqtt_config.pdpcid || 16<global_mqtt_config.pdpcid)
	{
		printf("[net] net_mqtt_configrate error. file=%s, line=%d",__FILE__,__LINE__);
		return net_mqtt_configrate_ret.RET_ERROR;
	}
	
	sprintf(net_send_buffer,"AT+QMTCFG=\"pdpcid\",%d,%d\r",client_idx,global_mqtt_config.pdpcid);
	
	ret = net_at_send(net_send_buffer,1000);
	if(0!=ret)
	{
		return net_mqtt_configrate_ret.RET_ERROR;
	}
	
	//配置超时时间
	if(1>global_mqtt_config.pkt_timeout || 60<global_mqtt_config.pkt_timeout)
	{
		printf("[net] net_mqtt_configrate error. file=%s, line=%d",__FILE__,__LINE__);
		return net_mqtt_configrate_ret.RET_ERROR;
	}
	
	if(0>global_mqtt_config.retry_times || 10<global_mqtt_config.retry_times)
	{
		printf("[net] net_mqtt_configrate error. file=%s, line=%d",__FILE__,__LINE__);
		return net_mqtt_configrate_ret.RET_ERROR;
	}
	
	if(0>global_mqtt_config.timeout_notice || 1<global_mqtt_config.timeout_notice)
	{
		printf("[net] net_mqtt_configrate error. file=%s, line=%d",__FILE__,__LINE__);
		return net_mqtt_configrate_ret.RET_ERROR;
	}
	
	sprintf(net_send_buffer,"AT+QMTCFG=\"timeout\",%d,%d,%d,%d\r",\
		client_idx,\
		global_mqtt_config.pkt_timeout,\
		global_mqtt_config.retry_times,\
		global_mqtt_config.timeout_notice);
	
	ret = net_at_send(net_send_buffer,1000);
	if(0!=ret)
	{
		return net_mqtt_configrate_ret.RET_ERROR;
	}
	
	//配置会话保持
	if(0>global_mqtt_config.clean_session || 1<global_mqtt_config.clean_session)
	{
		printf("[net] net_mqtt_configrate error. file=%s, line=%d",__FILE__,__LINE__);
		return net_mqtt_configrate_ret.RET_ERROR;
	}
	
	sprintf(net_send_buffer,"AT+QMTCFG=\"session\",%d,%d\r", client_idx, global_mqtt_config.clean_session);
	
	ret = net_at_send(net_send_buffer,1000);
	if(0!=ret)
	{
		return net_mqtt_configrate_ret.RET_ERROR;
	}
	
	//配置心跳超时时间
	if(0>global_mqtt_config.keep_alive_time || 3600<global_mqtt_config.keep_alive_time)
	{
		printf("[net] net_mqtt_configrate error. file=%s, line=%d",__FILE__,__LINE__);
		return net_mqtt_configrate_ret.RET_ERROR;
	}
	
	sprintf(net_send_buffer,"AT+QMTCFG=\"keepalive\",%d,%d\r", client_idx, global_mqtt_config.keep_alive_time);
	
	ret = net_at_send(net_send_buffer,1000);
	if(0!=ret)
	{
		return net_mqtt_configrate_ret.RET_ERROR;
	}
	
	//配置SSL
	if(0>global_mqtt_config.ssl_enable || 1<global_mqtt_config.ssl_enable)
	{
		printf("[net] net_mqtt_configrate error. file=%s, line=%d",__FILE__,__LINE__);
		return net_mqtt_configrate_ret.RET_ERROR;
	}
	
	if(0>global_mqtt_config.ssl_ctx_idx || 5<global_mqtt_config.ssl_ctx_idx)
	{
		printf("[net] net_mqtt_configrate error. file=%s, line=%d",__FILE__,__LINE__);
		return net_mqtt_configrate_ret.RET_ERROR;
	}
	
	if(0==global_mqtt_config.ssl_enable)
	{
		sprintf(net_send_buffer,"AT+QMTCFG=\"ssl\",%d,%d\r", client_idx, global_mqtt_config.ssl_enable);
	}
	else
	{
		sprintf(net_send_buffer,"AT+QMTCFG=\"ssl\",%d,%d,%d\r",\
			client_idx,\
			global_mqtt_config.ssl_enable,\
			global_mqtt_config.ssl_ctx_idx);
	}
	
	ret = net_at_send(net_send_buffer,1000);
	if(0!=ret)
	{
		return net_mqtt_configrate_ret.RET_ERROR;
	}
	
	//配置接收模式
	if(0>global_mqtt_config.msg_recv_mode || 1<global_mqtt_config.msg_recv_mode)
	{
		printf("[net] net_mqtt_configrate error. file=%s, line=%d",__FILE__,__LINE__);
		return net_mqtt_configrate_ret.RET_ERROR;
	}
	
	if(0>global_mqtt_config.msg_len_enable || 1<global_mqtt_config.msg_len_enable)
	{
		printf("[net] net_mqtt_configrate error. file=%s, line=%d",__FILE__,__LINE__);
		return net_mqtt_configrate_ret.RET_ERROR;
	}
	
	sprintf(net_send_buffer,"AT+QMTCFG=\"recv/mode\",%d,%d,%d\r",\
		client_idx,\
		global_mqtt_config.msg_recv_mode,\
		global_mqtt_config.msg_len_enable);
	
	ret = net_at_send(net_send_buffer,1000);
	if(0!=ret)
	{
		return net_mqtt_configrate_ret.RET_ERROR;
	}
	
	//函数返回
	return net_mqtt_configrate_ret.RET_OK;
}

/*
	打开MQTT网络连接函数
*/
int net_mqtt_open(int client_idx,char *host_name, int port)
{
	struct net_mqtt_open_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
		const int RET_TIMEOUT;
	}net_mqtt_open_ret = {0,1,2};
	
	//局部变量
	int ret=0;
	int timeout_timer=0;
	char *p=NULL;
	int result_client_idx=0;
	int result=0;
	
	//检查指针
	if(NULL == host_name)
	{
		printf("[net] net_mqtt_open error. file=%s, line=%d",__FILE__,__LINE__);
		return net_mqtt_open_ret.RET_ERROR;
	}
	
	//检查client_idx
	if(0>client_idx || 5<client_idx)
	{
		printf("[net] net_mqtt_open error. file=%s, line=%d",__FILE__,__LINE__);
		return net_mqtt_open_ret.RET_ERROR;
	}

	//准备指令
	sprintf(net_send_buffer,"AT+QMTOPEN=%d,\"%s\",%d\r",client_idx,host_name,port);
	
	//清空接收缓冲区
	restart_usart3_recv();
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&net_send_buffer[0]),strlen(net_send_buffer));
	
	for(timeout_timer=0; ;timeout_timer+=10)
	{
		osDelay(10);
		if(NULL!=strstr(net_recv_buffer,"+QMTOPEN:"))
		{
			common_wait_uart_receive(net_recv_buffer);
			printf("%s\n",net_recv_buffer);
			break;
		}
		if(NULL!=strstr(net_recv_buffer,"ERROR"))
		{
			printf("%s\n",net_recv_buffer);
			return net_mqtt_open_ret.RET_ERROR;
		}
		if(30000 < timeout_timer)
		{
			printf("%s\n",net_recv_buffer);
			return net_mqtt_open_ret.RET_TIMEOUT;
		}
	}
	
	//检查返回值
	p = strstr(net_recv_buffer,"+QMTOPEN:");
	ret = sscanf(p,"+QMTOPEN:%d,%d",&result_client_idx,&result);
	if(2!=ret)
	{
		printf("[net]sscanf error code=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
		return net_mqtt_open_ret.RET_ERROR;
	}
	
	//判断返回的idx是否正确
	if(client_idx != result_client_idx)
	{
		printf("[net] net_mqtt_open error. file=%s, line=%d",__FILE__,__LINE__);
		return net_mqtt_open_ret.RET_ERROR;
	}
	
	//判断返回值类型
	switch(result)
	{
		case -1:
			printf("[net] net_mqtt_open() Failed to open network.\n");
			return net_mqtt_open_ret.RET_ERROR;
			//break;
		
		case 0:
			printf("[net] net_mqtt_open() Network opened successfully.\n");
			return net_mqtt_open_ret.RET_OK;
			//break;
		
		case 1:
			printf("[net] net_mqtt_open() Wrong parameter.\n");
			return net_mqtt_open_ret.RET_ERROR;
			//break;
		
		case 2:
			printf("[net] net_mqtt_open() MQTT identifier is occupied.\n");
			//global_flag.net_reconnect_flag = 1;
			//global_mqtt_config.client_idx = (global_mqtt_config.client_idx+1)%6;
			return net_mqtt_open_ret.RET_ERROR;
			//break;
		
		case 3:
			printf("[net] net_mqtt_open() Failed to activate PDP.\n");
			return net_mqtt_open_ret.RET_ERROR;
			//break;
		
		case 4:
			printf("[net] net_mqtt_open() Failed to parse domain name.\n");
			return net_mqtt_open_ret.RET_ERROR;
			//break;
		
		case 5:
			printf("[net] net_mqtt_open() Network connection error.\n");
			return net_mqtt_open_ret.RET_ERROR;
			//break;
		
		default:
			printf("[net] net_mqtt_open() result code error.\n");
			return net_mqtt_open_ret.RET_ERROR;
			//break;
	}
}

/*
	关闭MQTT连接函数
*/
int net_mqtt_close(int client_idx)
{
	struct net_mqtt_close_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
		const int RET_TIMEOUT;
	}net_mqtt_close_ret = {0,1,2};
	
	//局部变量
	int ret=0;
	int timeout_timer=0;
	char *p=NULL;
	int result_client_idx=0;
	int result=0;
	
	//检查client_idx
	if(0>client_idx || 5<client_idx)
	{
		printf("[net] net_mqtt_open error. file=%s, line=%d",__FILE__,__LINE__);
		return net_mqtt_close_ret.RET_ERROR;
	}
	
	//准备指令
	sprintf(net_send_buffer,"AT+QMTCLOSE=%d\r",client_idx);
	
	//清空接收缓冲区
	restart_usart3_recv();
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&net_send_buffer[0]),strlen(net_send_buffer));
	
	for(timeout_timer=0; ;timeout_timer+=10)
	{
		osDelay(10);
		if(NULL!=strstr(net_recv_buffer,"+QMTCLOSE:"))
		{
			common_wait_uart_receive(net_recv_buffer);
			printf("%s\n",net_recv_buffer);
			break;
		}
		if(NULL!=strstr(net_recv_buffer,"ERROR"))
		{
			printf("%s\n",net_recv_buffer);
			return net_mqtt_close_ret.RET_ERROR;
		}
		if(10000 < timeout_timer)
		{
			printf("%s\n",net_recv_buffer);
			return net_mqtt_close_ret.RET_TIMEOUT;
		}
	}
	
	//检查返回值
	p = strstr(net_recv_buffer,"+QMTCLOSE:");
	ret = sscanf(p,"+QMTCLOSE:%d,%d",&result_client_idx,&result);
	if(2!=ret)
	{
		printf("[net]sscanf error code=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
		return net_mqtt_close_ret.RET_ERROR;
	}
	
	//判断返回的idx是否正确
	if(client_idx != result_client_idx)
	{
		printf("[net] net_mqtt_close error. file=%s, line=%d\n",__FILE__,__LINE__);
		return net_mqtt_close_ret.RET_ERROR;
	}
	
	//判断返回值类型
	switch(result)
	{
		case -1:
			printf("[net] net_mqtt_close() Failed to close network.\n");
			return net_mqtt_close_ret.RET_ERROR;
			//break;
		
		case 0:
			printf("[net] net_mqtt_close() Network closed successfully.\n");
			return net_mqtt_close_ret.RET_OK;
			//break;
		
		default:
			printf("[net] net_mqtt_close() result code error.\n");
			return net_mqtt_close_ret.RET_ERROR;
			//break;
	}
}

/*
	连接一个客户端到MQTT服务器
*/
int net_mqtt_connect(int client_idx, char *client_ID, char *username, char *password)
{
	struct net_mqtt_connect_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
		const int RET_TIMEOUT;
	}net_mqtt_connect_ret = {0,1,2};
	
	//局部变量
	int ret=0;
	int timeout_timer=0;
	char *p=NULL;
	int result_client_idx=0;
	int result=0;
	int ret_code=0;
	int error=0;
	
	
	//检查client_idx
	if(0>client_idx || 5<client_idx)
	{
		printf("[net] net_mqtt_connect error. file=%s, line=%d",__FILE__,__LINE__);
		return net_mqtt_connect_ret.RET_ERROR;
	}
	
	//准备指令
	sprintf(net_send_buffer,"AT+QMTCONN=%d,\"%s\",\"%s\",\"%s\"\r\n",client_idx,client_ID,username,password);
	
	//清空接收缓冲区
	restart_usart3_recv();
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&net_send_buffer[0]),strlen(net_send_buffer));
	
	for(timeout_timer=0; ;timeout_timer+=10)
	{
		osDelay(10);
		if(NULL!=strstr(net_recv_buffer,"+QMTCONN:"))
		{
			common_wait_uart_receive(net_recv_buffer);
			printf("%s\n",net_recv_buffer);
			break;
		}
		if(NULL!=strstr(net_recv_buffer,"ERROR"))
		{
			printf("%s\n",net_recv_buffer);
			return net_mqtt_connect_ret.RET_ERROR;
		}
		if(10000 < timeout_timer)
		{
			printf("%s\n",net_recv_buffer);
			return net_mqtt_connect_ret.RET_TIMEOUT;
		}
	}

	//检查返回值
	p = strstr(net_recv_buffer,"+QMTCONN:");
	ret = sscanf(p,"+QMTCONN:%d,%d,%d",&result_client_idx,&result,&ret_code);
	if(2>ret || 3<ret)
	{
		printf("[net]sscanf error code=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
		return net_mqtt_connect_ret.RET_ERROR;
	}
	
	//判断返回的idx是否正确
	if(client_idx != result_client_idx)
	{
		printf("[net] net_mqtt_connect error. file=%s, line=%d\n",__FILE__,__LINE__);
		return net_mqtt_connect_ret.RET_ERROR;
	}
	
	//判断返回值类型
	switch(result)
	{
		case 0:
			printf("[net] net_mqtt_connect() Packet sent successfully and ACK received from server.\n");
			error=0;
			break;
		
		case 1:
			printf("[net] net_mqtt_connect() Packet retransmission.\n");
			error=1;
			break;
		
		case 2:
			printf("[net] net_mqtt_connect() Failed to send packet.\n");
			error=1;
			break;
		
		default:
			printf("[net] net_mqtt_connect() result code error.\n");
			error=1;
			break;
	}
	
	//如果返回三个参数
	if(3==ret)
	{
		switch(ret_code)
		{
			case 0:
				printf("[net] net_mqtt_connect() Connection Accepted.\n");
				error=0;
				break;
			
			case 1:
				printf("[net] net_mqtt_connect() Connection Refused: Unacceptable Protocol Version.\n");
				error=1;
				break;
			
			case 2:
				printf("[net] net_mqtt_connect() Connection Refused: Identifier Rejected.\n");
				error=1;
				break;
			
			case 3:
				printf("[net] net_mqtt_connect() Connection Refused: Server Unavailable.\n");
				error=1;
				break;
			
			case 4:
				printf("[net] net_mqtt_connect() Connection Refused: Bad User Name or Password.\n");
				error=1;
				break;
			
			case 5:
				printf("[net] net_mqtt_connect() Connection Refused: Not Authorized.\n");
				error=1;
				break;
			
			default:
				printf("[net] net_mqtt_connect() ret_code error.\n");
				error=1;
				break;
		}
	}

	//函数返回
	if(0==error)
	{
		return net_mqtt_connect_ret.RET_OK;
	}
	else
	{
		return net_mqtt_connect_ret.RET_ERROR;
	}
}

/*
	从MQTT服务器断开一个客户端
*/
int net_mqtt_disconnect(int client_idx)
{
	struct net_mqtt_disconnect_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
		const int RET_TIMEOUT;
	}net_mqtt_disconnect_ret = {0,1,2};
	
	//局部变量
	int ret=0;
	int timeout_timer=0;
	char *p=NULL;
	int result_client_idx=0;
	int result=0;
	
	//检查client_idx
	if(0>client_idx || 5<client_idx)
	{
		printf("[net] net_mqtt_disconnect error. file=%s, line=%d",__FILE__,__LINE__);
		return net_mqtt_disconnect_ret.RET_ERROR;
	}
	
	//准备指令
	sprintf(net_send_buffer,"AT+QMTDISC=%d\r",client_idx);
	
	//清空接收缓冲区
	restart_usart3_recv();
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&net_send_buffer[0]),strlen(net_send_buffer));
	
	for(timeout_timer=0; ;timeout_timer+=10)
	{
		osDelay(10);
		if(NULL!=strstr(net_recv_buffer,"+QMTDISC:"))
		{
			common_wait_uart_receive(net_recv_buffer);
			printf("%s\n",net_recv_buffer);
			break;
		}
		/*
		if(NULL!=strstr(net_recv_buffer,"OK"))
		{
			printf("%s\n",net_recv_buffer);
			return net_mqtt_disconnect_ret.RET_OK;
		}
		*/
		if(NULL!=strstr(net_recv_buffer,"ERROR"))
		{
			printf("%s\n",net_recv_buffer);
			return net_mqtt_disconnect_ret.RET_ERROR;
		}
		if(10000 < timeout_timer)
		{
			printf("%s\n",net_recv_buffer);
			return net_mqtt_disconnect_ret.RET_TIMEOUT;
		}
	}
	
	//检查返回值
	p = strstr(net_recv_buffer,"+QMTDISC:");
	ret = sscanf(p,"+QMTDISC:%d,%d",&result_client_idx,&result);
	if(2!=ret)
	{
		printf("[net]sscanf error code=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
		return net_mqtt_disconnect_ret.RET_ERROR;
	}
	
	//判断返回的idx是否正确
	if(client_idx != result_client_idx)
	{
		printf("[net] net_mqtt_disconnect error. file=%s, line=%d\n",__FILE__,__LINE__);
		return net_mqtt_disconnect_ret.RET_ERROR;
	}
	
	//判断返回值类型
	switch(result)
	{
		case -1:
			printf("[net] net_mqtt_disconnect() Failed to close connection.\n");
			return net_mqtt_disconnect_ret.RET_ERROR;
			//break;
		
		case 0:
			printf("[net] net_mqtt_disconnect() Connection closed successfully.\n");
			return net_mqtt_disconnect_ret.RET_OK;
			//break;
		
		default:
			printf("[net] net_mqtt_disconnect() result code error.\n");
			return net_mqtt_disconnect_ret.RET_ERROR;
			//break;
	}
	
}

/*
	订阅频道
*/
int net_mqtt_subscribe(int client_idx, int msgID, int topic_amount, struct topic_qos_struct *topic_qos_array)
{
	struct net_mqtt_subscribe_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
		const int RET_TIMEOUT;
	}net_mqtt_subscribe_ret = {0,1,2};
	
	//局部变量
	int ret=0;
	char temp_buffer[256]={0};
	int timeout_timer=0;
	char *p=NULL;
	int result_client_idx=0;
	int result_msg_id=0;
	int result=0;
	int value=0;
	int i=0;
	int error=0;
	
	//检查参数
	if(0>client_idx || 5<client_idx)
	{
		printf("[net] net_mqtt_subscribe error. file=%s, line=%d",__FILE__,__LINE__);
		return net_mqtt_subscribe_ret.RET_ERROR;
	}
	
	if(1>topic_amount)
	{
		printf("[net] net_mqtt_subscribe error. file=%s, line=%d",__FILE__,__LINE__);
		return net_mqtt_subscribe_ret.RET_ERROR;
	}
	
	if(1>msgID || 65535<msgID)
	{
		printf("[net] net_mqtt_subscribe error. file=%s, line=%d",__FILE__,__LINE__);
		return net_mqtt_subscribe_ret.RET_ERROR;
	}

	if(NULL == topic_qos_array)
	{
		printf("[net] net_mqtt_subscribe error. file=%s, line=%d",__FILE__,__LINE__);
		return net_mqtt_subscribe_ret.RET_ERROR;
	}
	
	//准备指令
	sprintf(net_send_buffer,"AT+QMTSUB=%d,%d", client_idx, msgID);
	for(i=0;i<topic_amount;i++)
	{
		sprintf(temp_buffer, ",\"%s\",%d", topic_qos_array[i].topic, topic_qos_array[i].qos);
		strcat(net_send_buffer,temp_buffer);
	}
	strcat(net_send_buffer,"\r");
	
	//清空接收缓冲区
	restart_usart3_recv();
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&net_send_buffer[0]),strlen(net_send_buffer));
	
	for(timeout_timer=0; ;timeout_timer+=10)
	{
		osDelay(10);
		if(NULL!=strstr(net_recv_buffer,"+QMTSUB:"))
		{
			common_wait_uart_receive(net_recv_buffer);
			printf("%s\n",net_recv_buffer);
			break;
		}
		if(NULL!=strstr(net_recv_buffer,"ERROR"))
		{
			printf("%s\n",net_recv_buffer);
			return net_mqtt_subscribe_ret.RET_ERROR;
		}
		if(15000 < timeout_timer)
		{
			printf("%s\n",net_recv_buffer);
			return net_mqtt_subscribe_ret.RET_TIMEOUT;
		}
	}
	
	//检查返回值
	p = strstr(net_recv_buffer,"+QMTSUB:");
	ret = sscanf(p,"+QMTSUB:%d,%d,%d,%d",&result_client_idx,&result_msg_id, &result, &value);
	if(3>ret || 4<ret)
	{
		printf("[net]sscanf error code=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
		return net_mqtt_subscribe_ret.RET_ERROR;
	}
	
	//判断返回的idx是否正确
	if(client_idx != result_client_idx)
	{
		printf("[net] net_mqtt_subscribe error. file=%s, line=%d\n",__FILE__,__LINE__);
		return net_mqtt_subscribe_ret.RET_ERROR;
	}
	
	//判断返回的msgID是否正确
	if(msgID != result_msg_id)
	{
		printf("[net] net_mqtt_subscribe error. file=%s, line=%d\n",__FILE__,__LINE__);
		return net_mqtt_subscribe_ret.RET_ERROR;
	}
	
	//判断返回值类型
	switch(result)
	{
		case 0:
			printf("[net] net_mqtt_subscribe() Sent packet successfully and received ACK from server.\n");
			error=0;
			break;
		
		case 1:
			printf("[net] net_mqtt_subscribe() Packet retransmission.\n");
			error=1;
			break;
		
		case 2:
			printf("[net] net_mqtt_subscribe() Failed to send packet.\n");
			error=1;
			break;
		
		default:
			printf("[net] net_mqtt_subscribe() result code error.\n");
			error=1;
			break;
	}
	
	//如果返回4个参数
	if(4 == ret)
	{
		switch(value)
		{
			case 0:
				printf("[net] net_mqtt_subscribe()  it is a vector of granted QoS levels.\n");
				break;
			
			case 1:
				printf("[net] net_mqtt_subscribe() it means the times of packet retransmission.\n");
				break;
			
			case 2:
				printf("[net] net_mqtt_subscribe() it will not be presented.\n");
				break;
			
			default:
				printf("[net] net_mqtt_subscribe() result code error.\n");
				error=1;
				break;
		}
	}
	
	//函数返回
	if(0==error)
	{
		return net_mqtt_subscribe_ret.RET_OK;
	}
	else
	{
		return net_mqtt_subscribe_ret.RET_ERROR;
	}
	
}

/*
	取消订阅
*/
int net_mqtt_unsubscribe(int client_idx,int msgID, int topic_amount, struct topic_struct *topic_array)
{
	struct net_mqtt_unsubscribe_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
		const int RET_TIMEOUT;
	}net_mqtt_unsubscribe_ret = {0,1,2};
	
	//局部变量
	int ret=0;
	char temp_buffer[256]={0};
	int timeout_timer=0;
	char *p=NULL;
	int result_client_idx=0;
	int result_msg_id=0;
	int result=0;
	int value=0;
	int i=0;
	int error=0;
	
	//检查参数
	if(0>client_idx || 5<client_idx)
	{
		printf("[net] net_mqtt_unsubscribe error. file=%s, line=%d",__FILE__,__LINE__);
		return net_mqtt_unsubscribe_ret.RET_ERROR;
	}
	
	if(1>msgID || 65535<msgID)
	{
		printf("[net] net_mqtt_unsubscribe error. file=%s, line=%d",__FILE__,__LINE__);
		return net_mqtt_unsubscribe_ret.RET_ERROR;
	}
	
	if(1>topic_amount)
	{
		printf("[net] net_mqtt_unsubscribe error. file=%s, line=%d",__FILE__,__LINE__);
		return net_mqtt_unsubscribe_ret.RET_ERROR;
	}

	if(NULL == topic_array)
	{
		printf("[net] net_mqtt_unsubscribe error. file=%s, line=%d",__FILE__,__LINE__);
		return net_mqtt_unsubscribe_ret.RET_ERROR;
	}
	
	//准备指令
	sprintf(net_send_buffer,"AT+QMTUNS=%d,%d", client_idx, msgID);
	for(i=0;i<topic_amount;i++)
	{
		sprintf(temp_buffer, ",\"%s\"", topic_array[i].topic);
		strcat(net_send_buffer,temp_buffer);
	}
	strcat(net_send_buffer,"\r");
	
	//清空接收缓冲区
	restart_usart3_recv();
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&net_send_buffer[0]),strlen(net_send_buffer));
	
	for(timeout_timer=0; ;timeout_timer+=10)
	{
		osDelay(10);
		if(NULL!=strstr(net_recv_buffer,"+QMTUNS:"))
		{
			common_wait_uart_receive(net_recv_buffer);
			printf("%s\n",net_recv_buffer);
			break;
		}
		if(NULL!=strstr(net_recv_buffer,"ERROR"))
		{
			printf("%s\n",net_recv_buffer);
			return net_mqtt_unsubscribe_ret.RET_ERROR;
		}
		if(10000 < timeout_timer)
		{
			printf("%s\n",net_recv_buffer);
			return net_mqtt_unsubscribe_ret.RET_TIMEOUT;
		}
	}
	
	//检查返回值
	p = strstr(net_recv_buffer,"+QMTUNS:");
	ret = sscanf(p,"+QMTUNS:%d,%d,%d,%d",&result_client_idx,&result_msg_id, &result, &value);
	if(3>ret || 4<ret)
	{
		printf("[net]sscanf error code=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
		return net_mqtt_unsubscribe_ret.RET_ERROR;
	}
	
	//判断返回的idx是否正确
	if(client_idx != result_client_idx)
	{
		printf("[net] net_mqtt_unsubscribe error. file=%s, line=%d\n",__FILE__,__LINE__);
		return net_mqtt_unsubscribe_ret.RET_ERROR;
	}
	
	//判断返回的msgID是否正确
	if(msgID != result_msg_id)
	{
		printf("[net] net_mqtt_unsubscribe error. file=%s, line=%d\n",__FILE__,__LINE__);
		return net_mqtt_unsubscribe_ret.RET_ERROR;
	}
	
	//判断返回值类型
	switch(result)
	{
		case 0:
			printf("[net] net_mqtt_unsubscribe() Sent packet successfully and received ACK from server.\n");
			error=0;
			break;
		
		case 1:
			printf("[net] net_mqtt_unsubscribe() Packet retransmission.\n");
			error=1;
			break;
		
		case 2:
			printf("[net] net_mqtt_unsubscribe() Failed to send packet.\n");
			error=1;
			break;
		
		default:
			printf("[net] net_mqtt_unsubscribe() result code error.\n");
			error=1;
			break;
	}
	
	//如果返回4个参数
	if(4 == ret)
	{
		switch(value)
		{
			case 0:
				printf("[net] net_mqtt_unsubscribe()  it is a vector of granted QoS levels.\n");
				break;
			
			case 1:
				printf("[net] net_mqtt_unsubscribe() it means the times of packet retransmission.\n");
				break;
			
			case 2:
				printf("[net] net_mqtt_unsubscribe() it will not be presented.\n");
				break;
			
			default:
				printf("[net] net_mqtt_unsubscribe() result code error.\n");
				error=1;
				break;
		}
	}
	
	//函数返回
	if(0==error)
	{
		return net_mqtt_unsubscribe_ret.RET_OK;
	}
	else
	{
		return net_mqtt_unsubscribe_ret.RET_ERROR;
	}
}

/*
	发布消息
*/
int net_mqtt_publish(int client_idx, int msgID, int qos, int retain, char *topic, int msg_length , char *payload, int beep_in)
{
	struct net_mqtt_publish_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
		const int RET_TIMEOUT;
	}net_mqtt_publish_ret = {0,1,2};
	
	//局部变量
	int ret=0;
	int timeout_timer=0;
	char *p=NULL;
	int result_client_idx=0;
	int result_msg_id=0;
	int result=0;
	int value=0;
	int error=0;
	
	
	
	//检查参数
	if(0>client_idx || 5<client_idx)
	{
		printf("[net] net_mqtt_publish error. file=%s, line=%d\n",__FILE__,__LINE__);
		beep(0);
		return net_mqtt_publish_ret.RET_ERROR;
	}
	
	if(0>msgID || 65535<msgID)
	{
		printf("[net] net_mqtt_publish error. file=%s, line=%d\n",__FILE__,__LINE__);
		beep(0);
		return net_mqtt_publish_ret.RET_ERROR;
	}
	
	if(0>qos || 2<qos)
	{
		printf("[net] net_mqtt_publish error. file=%s, line=%d\n",__FILE__,__LINE__);
		beep(0);
		return net_mqtt_publish_ret.RET_ERROR;
	}
	
	if(0>retain || 1<retain)
	{
		printf("[net] net_mqtt_publish error. file=%s, line=%d\n",__FILE__,__LINE__);
		beep(0);
		return net_mqtt_publish_ret.RET_ERROR;
	}
	
	if(NULL == topic)
	{
		printf("[net] net_mqtt_publish error. file=%s, line=%d\n",__FILE__,__LINE__);
		beep(0);
		return net_mqtt_publish_ret.RET_ERROR;
	}
	
	if(0>msg_length || 1024<msg_length)
	{
		printf("[net] net_mqtt_publish error. file=%s, line=%d\n",__FILE__,__LINE__);
		beep(0);
		return net_mqtt_publish_ret.RET_ERROR;
	}

	if(0==msgID && 0!=qos)
	{
		printf("[net]  msgID will be 0 only when <qos>=0. file=%s, line=%d\n",__FILE__,__LINE__);
		beep(0);
		return net_mqtt_publish_ret.RET_ERROR;
	}
	
	//准备指令
	sprintf(net_send_buffer,"AT+QMTPUBEX=%d,%d,%d,%d,\"%s\",%d\r", client_idx, msgID, qos, retain, topic, msg_length);
	
	//清空接收缓冲区
	restart_usart3_recv();
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&net_send_buffer[0]),strlen(net_send_buffer));
	
	for(timeout_timer=0; ;timeout_timer+=10)
	{
		osDelay(10);
		if(NULL!=strstr(net_recv_buffer,">"))
		{
			break;
		}
		if(NULL!=strstr(net_recv_buffer,"ERROR"))
		{
			printf("%s\n",net_recv_buffer);
			beep(0);
			return net_mqtt_publish_ret.RET_ERROR;
		}
		if(10000 < timeout_timer)
		{
			printf("%s\n",net_recv_buffer);
			beep(0);
			return net_mqtt_publish_ret.RET_TIMEOUT;
		}
	}
	
	//beep 响
	if(1 == beep_in)
	{
		beep(1);
	}
	
	//发送payload
    restart_usart3_recv();
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&payload[0]),strlen(payload));	
	
	for(timeout_timer=0; ;timeout_timer+=10)
	{
		osDelay(10);
		if(NULL!=strstr(net_recv_buffer,"+QMTPUBEX:"))
		{
			common_wait_uart_receive(net_recv_buffer);
			printf("%s\n",net_recv_buffer);
			break;
		}
		if(NULL!=strstr(net_recv_buffer,"ERROR"))
		{
			printf("%s\n",net_recv_buffer);
			beep(0);
			return net_mqtt_publish_ret.RET_ERROR;
		}
		if(3000 < timeout_timer)
		{
			printf("%s\n",net_recv_buffer);
			beep(0);
			return net_mqtt_publish_ret.RET_TIMEOUT;
		}
	}

	//检查返回值
	p = strstr(net_recv_buffer,"+QMTPUBEX:");
	ret = sscanf(p,"+QMTPUBEX:%d,%d,%d,%d",&result_client_idx,&result_msg_id, &result, &value);
	if(3>ret || 4<ret)
	{
		printf("[net]sscanf error code=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
		beep(0);
		return net_mqtt_publish_ret.RET_ERROR;
	}
	
	//判断返回的idx是否正确
	if(client_idx != result_client_idx)
	{
		printf("[net] net_mqtt_publish error. file=%s, line=%d\n",__FILE__,__LINE__);
		beep(0);
		return net_mqtt_publish_ret.RET_ERROR;
	}
	
	//判断返回的msgID是否正确
	if(msgID != result_msg_id)
	{
		printf("[net] net_mqtt_publish error. file=%s, line=%d\n",__FILE__,__LINE__);
		beep(0);
		return net_mqtt_publish_ret.RET_ERROR;
	}
	
	//判断返回值类型
	switch(result)
	{
		case 0:
			printf("[net] net_mqtt_publish() Packet sent successfully and ACK received from server.\n");
			error=0;
			break;
		
		case 1:
			printf("[net] net_mqtt_publish() Packet retransmission.\n");
			error=1;
			break;
		
		case 2:
			printf("[net] net_mqtt_publish() Failed to send packet.\n");
			error=1;
			break;
		
		default:
			printf("[net] net_mqtt_publish() result code error.\n");
			error=1;
			break;
	}
	
	//如果返回4个参数
	if(4 == ret)
	{
		switch(value)
		{
			case 0:
				printf("[net] net_mqtt_publish()  it will not be presented.\n");
				break;
			
			case 1:
				printf("[net] net_mqtt_publish() it means the times of packet retransmission.\n");
				break;
			
			case 2:
				printf("[net] net_mqtt_publish() it will not be presented.\n");
				break;
			
			default:
				printf("[net] net_mqtt_publish() result code error.\n");
				error=1;
				break;
		}
	}
	
	//函数返回
	if(0==error)
	{
		beep(0);
		return net_mqtt_publish_ret.RET_OK;
	}
	else
	{
		printf("[net]发送失败，重新连接, file=%s, line=%d\n", __FILE__, __LINE__);
		global_flag.net_reconnect_flag = 1;
		beep(0);
		return net_mqtt_publish_ret.RET_ERROR;
	}
}

/*
	查看保存的信息数量
*/
int net_mqtt_receive_check(int *amount_out, struct mqtt_storage_struct mqtt_storage_array_out[6])
{
	struct net_mqtt_receive_check_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
		const int RET_TIMEOUT;
	}net_mqtt_receive_check_ret = {0,1,2};
	
	//局部变量
	int ret=0;
	int timeout_timer=0;
	char *p=NULL;
	int result_client_idx=0;
	int result_storage_flag[5]={0};
	int i=0;
	int amount=0;
	char temp_buffer[64]={0};
	int j=0;
	
	//检查参数
	if(NULL == amount_out)
	{
		printf("[net] net_mqtt_receive_check error. file=%s, line=%d\n",__FILE__,__LINE__);
		return net_mqtt_receive_check_ret.RET_ERROR;
	}
	
	//准备指令
	sprintf(net_send_buffer,"AT+QMTRECV?\r");
	
	//清空接收缓冲区
	restart_usart3_recv();
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&net_send_buffer[0]),strlen(net_send_buffer));
	
	for(timeout_timer=0; ;timeout_timer+=10)
	{
		osDelay(10);
		if(NULL!=strstr(net_recv_buffer,"AT+QMTRECV?\r\r\n+QMTRECV:"))
		{
			common_wait_uart_receive(net_recv_buffer);
			printf("%s\n",net_recv_buffer);
			break;
		}
		if(NULL!=strstr(net_recv_buffer,"ERROR"))
		{
			printf("%s\n",net_recv_buffer);
			return net_mqtt_receive_check_ret.RET_ERROR;
		}
		if(1000 < timeout_timer)
		{
			printf("%s\n",net_recv_buffer);
			return net_mqtt_receive_check_ret.RET_TIMEOUT;
		}
	}
	
	//检查返回值（共有几条）
	for(i=0;i<5;i++)
	{
		sprintf(temp_buffer, "+QMTRECV: %d,",i);
		p = strstr(net_recv_buffer,temp_buffer);
		if(NULL!=p)
		{
			ret = sscanf(p,"+QMTRECV:%d,%d,%d,%d,%d,%d",\
			&result_client_idx,\
			&result_storage_flag[0],\
			&result_storage_flag[1],\
			&result_storage_flag[2],\
			&result_storage_flag[3],\
			&result_storage_flag[4]);
			
			if(6==ret)
			{
				mqtt_storage_array_out[i].client_idx = result_client_idx;
				mqtt_storage_array_out[i].storage_flag[0]=result_storage_flag[0];
				mqtt_storage_array_out[i].storage_flag[1]=result_storage_flag[1];
				mqtt_storage_array_out[i].storage_flag[2]=result_storage_flag[2];
				mqtt_storage_array_out[i].storage_flag[3]=result_storage_flag[3];
				mqtt_storage_array_out[i].storage_flag[4]=result_storage_flag[4];
				
				for(j=0;j<5;j++)
				{
					if(1==result_storage_flag[j])
					{
						amount+=1;
					}
				}
			}
		}
	}

	//保存总数
	*amount_out = amount;
	
	//函数返回
	return net_mqtt_receive_check_ret.RET_OK;
}

/*
	读取一条信息
*/
int net_mqtt_receive_read(int client_idx, struct mqtt_message_struct *mqtt_message_out)
{
	struct net_mqtt_receive_read_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
		const int RET_TIMEOUT;
	}net_mqtt_receive_read_ret = {0,1,2};
	
	//静态变量
	static int index=0;
	
	//局部变量
	int ret=0;
	int timeout_timer=0;
	char *p=NULL;
	int result_client_idx=0;
	int i=0;
	int amount=0;
	struct mqtt_storage_struct mqtt_storage_array[6]={0};
	int message_exist_flag=0;
	int result_msg_id=0;
	char result_topic[256]={0};
	int result_payload_len=0;
	char *begin=NULL;
	char *end=NULL;
	char temp_string[32]={0};
	
	//检查参数
	if(0>client_idx || 5<client_idx)
	{
		printf("[net] net_mqtt_receive_read error. file=%s, line=%d\n",__FILE__,__LINE__);
		return net_mqtt_receive_read_ret.RET_ERROR;
	}
	
	if(NULL == mqtt_message_out)
	{
		printf("[net] net_mqtt_receive_read error. file=%s, line=%d\n",__FILE__,__LINE__);
		return net_mqtt_receive_read_ret.RET_ERROR;
	}
	
	//查看这个client_idx的第几个存储空间有消息
	ret = net_mqtt_receive_check(&amount, mqtt_storage_array);
	if(0!=ret)
	{
		printf("[net] net_mqtt_receive_read error. file=%s, line=%d\n",__FILE__,__LINE__);
		return net_mqtt_receive_read_ret.RET_ERROR;
	}
	
	//从下个index遍历
	//index=(index+1)%5;
	
	for(i=0;i<5;i++)
	{
		if(1==mqtt_storage_array[client_idx].storage_flag[index])
		{
			message_exist_flag=1;
			break;
		}
		else
		{
			index=(index+1)%5;
		}
	}
	
	if(0==message_exist_flag)
	{
		printf("[net] this index have no message received. file=%s, line=%d\n",__FILE__,__LINE__);
		return net_mqtt_receive_read_ret.RET_ERROR;
	}
	
	//读出当前INDEX的消息
	printf("[net]index=%d\n",index);
//********************************EC20F/EC200U********************************		
	//准备指令
	if(strstr(global_device_info.device_ID,"EC20F") != NULL)
	{
		sprintf(net_send_buffer,"AT+QMTRECV=%d,%d\r",client_idx,index);
	}
	else if(strstr(global_device_info.device_ID,"EC200U") != NULL)
	{
		  sprintf(net_send_buffer,"AT+QMTRECV=%d,%d\r\r\n",client_idx,4-index);
	}
	else
	{
		sprintf(net_send_buffer,"AT+QMTRECV=%d,%d\r",client_idx,index);
	}
//***************************************************************************	
	//清空接收缓冲区
//	memset(net_recv_buffer,0,sizeof(net_recv_buffer));
//	DMA_receive_IT(CM_USART2,CM_DMA1,DMA_CH2,(uint32_t)(&net_recv_buffer[0]),sizeof(net_recv_buffer)-1);
	restart_usart3_recv();
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&net_send_buffer[0]),strlen(net_send_buffer));

	
	//由于此时收到消息也会弹出+QMTRECV，影响命令接收，所以做特殊处理
	strcpy(temp_string,net_send_buffer);
	strcat(temp_string,"+QMTRECV:");
	
	for(timeout_timer=0; ;timeout_timer+=10)
	{
		DDL_DelayMS(10);
		if(NULL!=strstr(net_recv_buffer,temp_string))
		{
			common_wait_uart_receive(net_recv_buffer);
			printf("%s\n",net_recv_buffer);
			break;
		}
		if(NULL!=strstr(net_recv_buffer,"ERROR"))
		{
			printf("%s\n",net_recv_buffer);
			return net_mqtt_receive_read_ret.RET_ERROR;
		}
		if(1000 < timeout_timer)
		{
			printf("接收超时,接收的内容为:\n");
			printf("%s\n",net_recv_buffer);
			printf("内容输出完毕\n");
			return net_mqtt_receive_read_ret.RET_TIMEOUT;
		}
	}
	
	//检查返回值
	//此处特殊处理
	p = strstr(net_recv_buffer,temp_string);
	p = strstr(p,"+QMTRECV:");
	ret = sscanf(p,"+QMTRECV:%d,%d,\"%[^\"]\",%d,",&result_client_idx,&result_msg_id,result_topic,&result_payload_len);
	if(3>ret || 4<ret)
	{
		printf("[net] sscanf error. file=%s, line=%d\n",__FILE__,__LINE__);
		return net_mqtt_receive_read_ret.RET_ERROR;
	}

	if(client_idx != result_client_idx)
	{
		printf("[net] net_mqtt_receive_read error. file=%s, line=%d\n",__FILE__,__LINE__);
		return net_mqtt_receive_read_ret.RET_ERROR;
	}
	
	//解析msg_id
	mqtt_message_out->msg_id = result_msg_id;
	
	//解析频道
	strcpy(mqtt_message_out->topic, result_topic);
	
	//解析长度
	if(4==ret)
	{
		mqtt_message_out->payload_len = result_payload_len;
	}
	
	//解析内容（注意内容有可能为空，只有两个双引号存在）
	p = strstr(net_recv_buffer,"+QMTRECV:");
	if(3==ret)
	{
		//数3个逗号,找到起始端双引号
		p = strstr(p+1,",");
		p = strstr(p+1,",");
		p = strstr(p+1,",");
	}
	else if(4==ret)
	{
		//数4个逗号,找到起始端双引号
		p = strstr(p+1,",");
		p = strstr(p+1,",");
		p = strstr(p+1,",");
		p = strstr(p+1,",");
	}
	else
	{
		printf("[net] net_mqtt_receive_read error. file=%s, line=%d\n",__FILE__,__LINE__);
		return net_mqtt_receive_read_ret.RET_ERROR;
	}
	
	if('\"' != *(p+1))
	{
		printf("[net] net_mqtt_receive_read error. file=%s, line=%d\n",__FILE__,__LINE__);
		return net_mqtt_receive_read_ret.RET_ERROR;
	}
	begin = p+1;

	//找末尾最后一个双引号
	for(i=strlen(net_recv_buffer); i>0; i--)
	{
		if('\"' == net_recv_buffer[i-1])
		{
			break;
		}
	}
	end = &net_recv_buffer[i-1];
	
	if(end <= begin)
	{
		printf("[net] net_mqtt_receive_read error. file=%s, line=%d\n",__FILE__,__LINE__);
		return net_mqtt_receive_read_ret.RET_ERROR;
	}
	
	//截取内容
	strncpy(mqtt_message_out->payload, begin+1, end-begin-1);
	
	//计算长度
	if(3==ret)
	{
		mqtt_message_out->payload_len = strlen(mqtt_message_out->payload);
	}
	
	//函数返回
	return net_mqtt_receive_read_ret.RET_OK;
}

/*
	URC函数，主循环
*/
int net_mqtt_urc(char *buffer_in, struct mqtt_urc_struct *mqtt_urc_out)
{
	struct net_mqtt_urc_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_mqtt_urc_ret = {0,1};
	
	//局部变量
	int ret=0;
	char *p=NULL;
	int result_stat_client_idx=0;
	int result_stat_err_code=0;
	int result_recv_client_idx=0;
	int result_recv_msgID=0;
	int result_recv_payload_len=0;
	char *begin=NULL;
	char *end=NULL;
	int i=0;
	int storage_id=0;
	int result_ping_client_idx=0;
	int result_ping_result=0;
	
	//检查参数
	if(NULL==buffer_in)
	{
		printf("[net]net_mqtt_urc error, file=%s, line=%d\n", __FILE__, __LINE__);
		return net_mqtt_urc_ret.RET_ERROR;
	}
	if(NULL==mqtt_urc_out)
	{
		printf("[net]net_mqtt_urc error, file=%s, line=%d\n", __FILE__, __LINE__);
		return net_mqtt_urc_ret.RET_ERROR;
	}

	//清空原内容
	memset(mqtt_urc_out, 0, sizeof(struct mqtt_urc_struct));
	
	//解析内容 QMTSTAT
	p = strstr(buffer_in, "+QMTSTAT:");
	if(NULL != p)
	{
		ret = sscanf(p, "+QMTSTAT:%d,%d", &result_stat_client_idx, &result_stat_err_code);
		if(2!=ret)
		{
			printf("[net]net_mqtt_urc error, file=%s, line=%d\n", __FILE__, __LINE__);
			return net_mqtt_urc_ret.RET_ERROR;
		}
		
		//保存输出
		mqtt_urc_out->state_flag = 1;
		mqtt_urc_out->state_client_idx = result_stat_client_idx;
		mqtt_urc_out->state_code = result_stat_err_code;
		
		switch(result_stat_err_code)
		{
			case 1:
				printf("[net] +QMTSTAT : Connection is closed or reset by peer.\n");
				break;
			
			case 2:
				printf("[net] +QMTSTAT : Sending PINGREQ packet timed out or failed.\n");
				break;
			
			case 3:
				printf("[net] +QMTSTAT : Sending CONNECT packet timed out or failed.\n");
				break;
			
			case 4:
				printf("[net] +QMTSTAT : Receiving CONNECK packet timed out or failed.\n");
				break;
			
			case 5:
				printf("[net] +QMTSTAT : The client sends DISCONNECT packet to sever and the server is initiative to close MQTT connection.\n");
				break;
			
			case 6:
				printf("[net] +QMTSTAT : The client is initiative to close MQTT connection due to packet sending failure all the time.\n");
				break;
			
			case 7:
				printf("[net] +QMTSTAT : The link is not alive or the server is unavailable.\n");
				break;
			
			default:
				printf("[net] +QMTSTAT : result_stat_err_code = %d , error\n", result_stat_err_code);
				break;
		}
	}
	
	//解析内容 QMTRECV
	p = strstr(buffer_in, "+QMTRECV:");
	if(NULL!=p)
	{
		//先检查是第几种接收方式
		//寻找双引号，如果有则是直接显示的方式
		if(NULL!=strstr(p,"\""))
		{
			//判断是否显示长度信息
			ret = sscanf(p,"+QMTRECV:%d,%d,\"%[^\"]\",%d,", &result_recv_client_idx, &result_recv_msgID, mqtt_urc_out->receive_mqtt_message.topic, &result_recv_payload_len);
			if(3>ret || 4<ret)
			{
				printf("[net]net_mqtt_urc error, file=%s, line=%d\n", __FILE__, __LINE__);
				return net_mqtt_urc_ret.RET_ERROR;
			}
			
			mqtt_urc_out->receive_recv_mode = 0;
			mqtt_urc_out->receive_flag = 1;
			mqtt_urc_out->receive_client_idx = result_recv_client_idx;
			//strcpy(mqtt_urc_out->receive_mqtt_message.topic, result_recv_topic_buffer.p);
			
			if(3==ret)
			{
				//数3个逗号,找到起始端双引号
				p = strstr(p+1,",");
				p = strstr(p+1,",");
				p = strstr(p+1,",");
			}
			else if(4==ret)
			{
				//数4个逗号,找到起始端双引号
				p = strstr(p+1,",");
				p = strstr(p+1,",");
				p = strstr(p+1,",");
				p = strstr(p+1,",");
			}
			else
			{
				printf("[net] net_mqtt_urc error. file=%s, line=%d\n",__FILE__,__LINE__);
				return net_mqtt_urc_ret.RET_ERROR;
			}
			
			if('\"' != *(p+1))
			{
				printf("[net] net_mqtt_urc error. file=%s, line=%d\n",__FILE__,__LINE__);
				return net_mqtt_urc_ret.RET_ERROR;
			}
			begin = p+1;

			//找末尾最后一个双引号
			for(i=strlen(buffer_in); i>0; i--)
			{
				if('\"' == buffer_in[i-1])
				{
					break;
				}
			}
			end = &buffer_in[i-1];
			
			if(end <= begin)
			{
				printf("[net] net_mqtt_urc error. file=%s, line=%d\n",__FILE__,__LINE__);
				return net_mqtt_urc_ret.RET_ERROR;
			}
			
			//截取内容
			strncpy(mqtt_urc_out->receive_mqtt_message.payload, begin+1, end-begin-1);
			
			//计算长度
			if(3==ret)
			{
				mqtt_urc_out->receive_mqtt_message.payload_len = strlen(mqtt_urc_out->receive_mqtt_message.payload);
			}
			else if(4==ret)
			{
				mqtt_urc_out->receive_mqtt_message.payload_len = result_recv_payload_len;
			}
		}
		else
		{
			//非直接显示的方式
			ret = sscanf(p,"+QMTRECV:%d,%d",\
			&result_recv_client_idx,\
			&storage_id);
			
			if(2!=ret)
			{
				printf("[net] net_mqtt_urc error. file=%s, line=%d\n",__FILE__,__LINE__);
				return net_mqtt_urc_ret.RET_ERROR;
			}
			
			mqtt_urc_out->receive_flag = 1;
			mqtt_urc_out->receive_recv_mode = 1;
			mqtt_urc_out->receive_client_idx = result_recv_client_idx;
			mqtt_urc_out->receive_storage_id = storage_id;
		}
	}
	
	//解析内容 QMTPING
	p = strstr(buffer_in, "+QMTPING:");
	if(NULL!=p)
	{
		ret = sscanf(p, "+QMTPING:%d,%d", &result_ping_client_idx, &result_ping_result);
		if(2!=ret)
		{
			printf("[net] net_mqtt_urc error. file=%s, line=%d\n",__FILE__,__LINE__);
			return net_mqtt_urc_ret.RET_ERROR;
		}
		
		mqtt_urc_out->ping_flag = 1;
		mqtt_urc_out->ping_client_idx = result_ping_client_idx;
		mqtt_urc_out->ping_code = result_ping_result;
		
		if(1==result_ping_result)
		{
			printf("[net] client_idx %d ping FAIL.\n",result_ping_client_idx);
		}
	}
	
	//函数返回
	return net_mqtt_urc_ret.RET_OK;
}

/*
	获取网络时间
*/
int net_get_time(struct time_struct *time_out)
{
	struct net_get_time_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
		const int RET_TIMEOUT;
	}net_get_time_ret = {0,1,2};
	
	//局部变量
	int ret=0;
	int timeout_timer=0;
	char *p=NULL;
	int temp_year=0;
	int temp_month=0;
	int temp_day=0;
	int temp_hour=0;
	int temp_minute=0;
	int temp_second=0;
	
	//检查参数
	if(NULL == time_out)
	{
		printf("[net]net_get_time error, file=%s, line=%d\n", __FILE__, __LINE__);
		return net_get_time_ret.RET_ERROR;
	}
	
	//发送指令，设置自动切换时区
	ret = net_at_send("AT+CTZU=3\r",5000);
	if(0!=ret)
	{
		printf("[net]net_get_time error, file=%s, line=%d\n", __FILE__, __LINE__);
		return net_get_time_ret.RET_ERROR;
	}
	
	//发送指令，获取当地时间
	sprintf(net_send_buffer,"AT+CCLK?\r");
	
	//清空接收缓冲区
	restart_usart3_recv();
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&net_send_buffer[0]),strlen(net_send_buffer));
	
	for(timeout_timer=0; ;timeout_timer+=10)
	{
		osDelay(10);
		if(NULL!=strstr(net_recv_buffer,"+CCLK:"))
		{
			common_wait_uart_receive(net_recv_buffer);
			printf("%s\n",net_recv_buffer);
			break;
		}
		if(NULL!=strstr(net_recv_buffer,"ERROR"))
		{
			printf("%s\n",net_recv_buffer);
			return net_get_time_ret.RET_ERROR;
		}
		if(1000 < timeout_timer)
		{
			printf("%s\n",net_recv_buffer);
			return net_get_time_ret.RET_TIMEOUT;
		}
	}
	
	
	
	//检查返回值
	p = strstr(net_recv_buffer,"+CCLK: ");
	ret = sscanf(p,"+CCLK: \"%d/%d/%d,%d:%d:%d", &temp_year, &temp_month, &temp_day, &temp_hour, &temp_minute, &temp_second);
	if(6!=ret)
	{
		printf("[net] sscanf error. file=%s, line=%d\n",__FILE__,__LINE__);
		return net_get_time_ret.RET_ERROR;
	}
	
	//检查年份
	if(2000<temp_year && 2255>temp_year)
	{
		temp_year-=2000;
	}
	else if(2255 < temp_year || 0 > temp_year)
	{
		printf("[net]net_get_time error, file=%s, line=%d\n", __FILE__, __LINE__);
		return net_get_time_ret.RET_ERROR;
	}
	
	//检查月份
	if(1>temp_month || 12<temp_month)
	{
		printf("[net]net_get_time error, file=%s, line=%d\n", __FILE__, __LINE__);
		return net_get_time_ret.RET_ERROR;
	}
	
	//检查日期
	if(1>temp_day || 31<temp_day)
	{
		printf("[net]net_get_time error, file=%s, line=%d\n", __FILE__, __LINE__);
		return net_get_time_ret.RET_ERROR;
	}
	
	//检查小时
	if(0>temp_hour || 23<temp_hour)
	{
		printf("[net]net_get_time error, file=%s, line=%d\n", __FILE__, __LINE__);
		return net_get_time_ret.RET_ERROR;
	}
	
	//检查分钟
	if(0>temp_minute || 59<temp_minute)
	{
		printf("[net]net_get_time error, file=%s, line=%d\n", __FILE__, __LINE__);
		return net_get_time_ret.RET_ERROR;
	}
	
	//检查秒数
	if(0>temp_second || 59<temp_second)
	{
		printf("[net]net_get_time error, file=%s, line=%d\n", __FILE__, __LINE__);
		return net_get_time_ret.RET_ERROR;
	}
	
	//赋值输出
	time_out->year = temp_year;
	time_out->month = temp_month;
	time_out->day = temp_day;
	time_out->hour = temp_hour;
	time_out->minute = temp_minute;
	time_out->second = temp_second;
	
	//函数返回
	return net_get_time_ret.RET_OK;
}

/*
	获取信号强度
	共四格信号，4最强，0没有
*/
int net_signal_quality(int *quality_out)
{
	struct net_signal_quality_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
		const int RET_TIMEOUT;
	}net_signal_quality_ret = {0,1,2};
	
	//局部变量
	int ret=0;
	int timeout_timer=0;
	char *p=NULL;
	int result_rssi=0;
	int result_ber=0;
	
	//检查参数
	if(NULL == quality_out)
	{
		printf("[net]net_signal_quality error, file=%s, line=%d\n", __FILE__, __LINE__);
		return net_signal_quality_ret.RET_ERROR;
	}
	
	//准备指令
	sprintf(net_send_buffer,"AT+CSQ\r");
	
	//清空接收缓冲区
	restart_usart3_recv();
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&net_send_buffer[0]),strlen(net_send_buffer));
	
	for(timeout_timer=0; ;timeout_timer+=10)
	{
		osDelay(10);
		if(NULL!=strstr(net_recv_buffer,"+CSQ:"))
		{
			common_wait_uart_receive(net_recv_buffer);
			printf("%s\n",net_recv_buffer);
			break;
		}
		if(NULL!=strstr(net_recv_buffer,"ERROR"))
		{
			printf("%s\n",net_recv_buffer);
			return net_signal_quality_ret.RET_ERROR;
		}
		if(1000 < timeout_timer)
		{
			printf("%s\n",net_recv_buffer);
			return net_signal_quality_ret.RET_TIMEOUT;
		}
	}
	
	//检查返回值
	p = strstr(net_recv_buffer,"+CSQ:");
	ret = sscanf(p,"+CSQ:%d,%d",&result_rssi,&result_ber);
	if(2!=ret)
	{
		printf("[net] sscanf error. file=%s, line=%d\n",__FILE__,__LINE__);
		return net_signal_quality_ret.RET_ERROR;
	}
	
	if(0>result_rssi || 199<result_rssi || (31<result_rssi && 99>result_rssi))
	{
		printf("[net] net_signal_quality error. file=%s, line=%d\n",__FILE__,__LINE__);
		return net_signal_quality_ret.RET_ERROR;
	}
	
	if(0>result_ber || 99<result_ber || (7<result_ber && 99>result_ber))
	{
		printf("[net] net_signal_quality error. file=%s, line=%d\n",__FILE__,__LINE__);
		return net_signal_quality_ret.RET_ERROR;
	}
	
	//信号等级
	if(99==result_rssi || 199==result_rssi )
	{
		*quality_out = 0;
	}
	else if((0<=result_rssi && 7>=result_rssi) || (100<=result_rssi && 130>=result_rssi))
	{
		*quality_out = 1;
	}
	else if((8<=result_rssi && 15>=result_rssi) || (131<=result_rssi && 170>=result_rssi))
	{
		*quality_out = 2;
	}
	else if((16<=result_rssi && 23>=result_rssi) || (171<=result_rssi && 180>=result_rssi))
	{
		*quality_out = 3;
	}
	else if((24<=result_rssi && 31>=result_rssi) || (181<=result_rssi && 191>=result_rssi))
	{
		*quality_out = 4;
	}
	else
	{
		printf("[net] net_signal_quality error. file=%s, line=%d\n",__FILE__,__LINE__);
		return net_signal_quality_ret.RET_ERROR;
	}
	
	//函数返回
	return net_signal_quality_ret.RET_OK;
}

/*
	AT+QFTPCFG 配置 FTP
*/
int net_ftp_configrate(struct ftp_config_struct *ftp_confing_in)
{
	struct net_ftp_configrate_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_ftp_configrate_ret = {0,1};
	
	//局部变量
	int ret=0;
	
	//先检查指针
	if(NULL == ftp_confing_in)
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_ftp_configrate_ret.RET_ERROR;
	}
	
	//配置用户名和密码
	if(0==strlen(ftp_confing_in->username) || 0==strlen(ftp_confing_in->password))
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_ftp_configrate_ret.RET_ERROR;
	}
	
	sprintf(net_send_buffer,"AT+QFTPCFG=\"account\",\"%s\",\"%s\"\r",ftp_confing_in->username,ftp_confing_in->password);
	
	ret = net_at_send(net_send_buffer,5000);
	if(0!=ret)
	{
		return net_ftp_configrate_ret.RET_ERROR;
	}
	//配置传输模式
	if(0>ftp_confing_in->transmode || 1<ftp_confing_in->transmode)
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_ftp_configrate_ret.RET_ERROR;
	}
	
	sprintf(net_send_buffer,"AT+QFTPCFG=\"transmode\",%d\r", ftp_confing_in->transmode);
	
	ret = net_at_send(net_send_buffer,5000);
	if(0!=ret)
	{
		return net_ftp_configrate_ret.RET_ERROR;
	}
	
	//函数返回
	return net_ftp_configrate_ret.RET_OK;
	
}

/*
	打开FTP网络连接函数
*/
int net_ftp_open(char *host_name, int port)
{
	struct net_ftp_open_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
		const int RET_TIMEOUT;
	}net_ftp_open_ret = {0,1,2};
	
	//局部变量
	int ret=0;
	int timeout_timer=0;
	char *p=NULL;
	int result_err=0;
	int result_protocol_error=0;
	
	//检查指针
	if(NULL == host_name)
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_ftp_open_ret.RET_ERROR;
	}

	//准备指令
	sprintf(net_send_buffer,"AT+QFTPOPEN=\"%s\",%d\r", host_name, port);
	
	//清空接收缓冲区
	restart_usart3_recv();
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&net_send_buffer[0]),strlen(net_send_buffer));
	
	for(timeout_timer=0; ;timeout_timer+=10)
	{
		osDelay(10);
		if(NULL!=strstr(net_recv_buffer,"+QFTPOPEN:"))
		{
			common_wait_uart_receive(net_recv_buffer);
			printf("%s\n",net_recv_buffer);
			break;
		}
		if(NULL!=strstr(net_recv_buffer,"ERROR"))
		{
			printf("%s\n",net_recv_buffer);
			return net_ftp_open_ret.RET_ERROR;
		}
		if(10000 < timeout_timer)
		{
			printf("%s\n",net_recv_buffer);
			return net_ftp_open_ret.RET_TIMEOUT;
		}
	}
	
	
	//检查返回值
	p = strstr(net_recv_buffer,"+QFTPOPEN:");
	ret = sscanf(p,"+QFTPOPEN:%d,%d",&result_err,&result_protocol_error);
	if(2!=ret)
	{
		printf("[net]sscanf error code=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
		return net_ftp_open_ret.RET_ERROR;
	}
	
	//判断返回值
	if(0!=result_err)
	{
		printf("[net]error. file=%s, line=%d\n", __FILE__, __LINE__);
		return net_ftp_open_ret.RET_ERROR;
	}
	
	//函数返回
	return net_ftp_open_ret.RET_OK;
}

/*
	配置当前FTP路径
*/
int net_ftp_cwd(char *path_name_in)
{
	struct net_ftp_cwd_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
		const int RET_TIMEOUT;
	}net_ftp_cwd_ret = {0,1,2};
	
	//局部变量
	int ret=0;
	int timeout_timer=0;
	char *p=NULL;
	int result_err=0;
	int result_protocol_error=0;
	
	//检查指针
	if(NULL == path_name_in)
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_ftp_cwd_ret.RET_ERROR;
	}

	//准备指令
	sprintf(net_send_buffer,"AT+QFTPCWD=\"%s\"\r", path_name_in);
	
	//清空接收缓冲区
	restart_usart3_recv();
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&net_send_buffer[0]),strlen(net_send_buffer));
	
	for(timeout_timer=0; ;timeout_timer+=10)
	{
		osDelay(10);
		if(NULL!=strstr(net_recv_buffer,"+QFTPCWD:"))
		{
			common_wait_uart_receive(net_recv_buffer);
			printf("%s\n",net_recv_buffer);
			break;
		}
		if(NULL!=strstr(net_recv_buffer,"ERROR"))
		{
			printf("%s\n",net_recv_buffer);
			return net_ftp_cwd_ret.RET_ERROR;
		}
		if(10000 < timeout_timer)
		{
			printf("%s\n",net_recv_buffer);
			return net_ftp_cwd_ret.RET_TIMEOUT;
		}
	}
	
	//检查返回值
	p = strstr(net_recv_buffer,"+QFTPCWD:");
	ret = sscanf(p,"+QFTPCWD:%d,%d",&result_err,&result_protocol_error);
	if(2!=ret)
	{
		printf("[net]sscanf error code=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
		return net_ftp_cwd_ret.RET_ERROR;
	}
	
	//判断返回值
	if(0!=result_err)
	{
		printf("[net]error. file=%s, line=%d\n", __FILE__, __LINE__);
		return net_ftp_cwd_ret.RET_ERROR;
	}
	
	//函数返回
	return net_ftp_cwd_ret.RET_OK;
}

/*
	获取当前FTP路径
*/
int net_ftp_pwd(char *path_name_out)
{
	struct net_ftp_pwd_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
		const int RET_TIMEOUT;
	}net_ftp_pwd_ret = {0,1,2};
	
	//局部变量
	int ret=0;
	int timeout_timer=0;
	char *p=NULL;
	int result_err=0;

	//检查指针
	if(NULL == path_name_out)
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_ftp_pwd_ret.RET_ERROR;
	}

	//准备指令
	sprintf(net_send_buffer,"AT+QFTPPWD\r");
	
	//清空接收缓冲区
	restart_usart3_recv();
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&net_send_buffer[0]),strlen(net_send_buffer));
	
	for(timeout_timer=0; ;timeout_timer+=10)
	{
		osDelay(10);
		if(NULL!=strstr(net_recv_buffer,"+QFTPPWD:"))
		{
			common_wait_uart_receive(net_recv_buffer);
			printf("%s\n",net_recv_buffer);
			break;
		}
		if(NULL!=strstr(net_recv_buffer,"ERROR"))
		{
			printf("%s\n",net_recv_buffer);
			return net_ftp_pwd_ret.RET_ERROR;
		}
		if(10000 < timeout_timer)
		{
			printf("%s\n",net_recv_buffer);
			return net_ftp_pwd_ret.RET_TIMEOUT;
		}
	}
	
	//检查返回值
	p = strstr(net_recv_buffer,"+QFTPPWD:");
	ret = sscanf(p,"+QFTPPWD:%d,",&result_err);
	if(1!=ret)
	{
		printf("[net]sscanf error code=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
		return net_ftp_pwd_ret.RET_ERROR;
	}
	
	//判断返回值
	if(0==result_err)
	{
		ret = sscanf(p,"+QFTPPWD:%d,%[^\r]",&result_err,path_name_out);
		if(2!=ret)
		{
			printf("[net]sscanf error code=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
			return net_ftp_pwd_ret.RET_ERROR;
		}
	}	
	
	if(0!=result_err)
	{
		printf("[net]error. file=%s, line=%d\n", __FILE__, __LINE__);
		return net_ftp_pwd_ret.RET_ERROR;
	}
	
	
	//函数返回
	return net_ftp_pwd_ret.RET_OK;
}

/*
	FTP上传文件(串口方式)
*/
int net_ftp_put_COM(char *file_name_in, int startpos_in, int uploadlen_in, int beof_in, char*payload_in)
{
	struct net_ftp_put_COM_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
		const int RET_TIMEOUT;
	}net_ftp_put_COM_ret = {0,1,2};
	
	//局部变量
	int ret=0;
	int timeout_timer=0;
	char *p=NULL;
	int result_err=0;
	int result_transferlen=0;
	
	//检查指针
	if(NULL == file_name_in)
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_ftp_put_COM_ret.RET_ERROR;
	}
	if(NULL == payload_in)
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_ftp_put_COM_ret.RET_ERROR;
	}
	
	//准备指令
	sprintf(net_send_buffer,"AT+QFTPPUT=\"%s\",\"COM:\",%d,%d,%d\r",file_name_in,startpos_in,uploadlen_in,beof_in);

	//清空接收缓冲区
	restart_usart3_recv();
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&net_send_buffer[0]),strlen(net_send_buffer));
	
	for(timeout_timer=0; ;timeout_timer+=10)
	{
		osDelay(10);
		if(NULL!=strstr(net_recv_buffer,"CONNECT"))
		{
			printf("%s\n",net_recv_buffer);
			break;
		}
		if(NULL!=strstr(net_recv_buffer,"ERROR"))
		{
			printf("%s\n",net_recv_buffer);
			return net_ftp_put_COM_ret.RET_ERROR;
		}
		if(10000 < timeout_timer)
		{
			printf("%s\n",net_recv_buffer);
			return net_ftp_put_COM_ret.RET_TIMEOUT;
		}
	}
	
	printf("[net]开始发送数据, file=%s, line=%d\n", __FILE__, __LINE__);
	//发送数据
	restart_usart3_recv();
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&payload_in[0]),uploadlen_in);
	
	for(timeout_timer=0; ;timeout_timer+=10)
	{
		osDelay(10);
		if(NULL!=strstr(net_recv_buffer,"+QFTPPUT:"))
		{
			common_wait_uart_receive(net_recv_buffer);
			printf("%s\n",net_recv_buffer);
			break;
		}
		if(NULL!=strstr(net_recv_buffer,"ERROR"))
		{
			printf("%s\n",net_recv_buffer);
			return net_ftp_put_COM_ret.RET_ERROR;
		}
		if(20000 < timeout_timer)
		{
			printf("%s\n",net_recv_buffer);
			printf("[net]等待超时, file=%s, line=%d\n",  __FILE__, __LINE__);
			return net_ftp_put_COM_ret.RET_TIMEOUT;
		}
	}
	
	//检查返回值
	p = strstr(net_recv_buffer,"+QFTPPUT:");
	ret = sscanf(p,"+QFTPPUT:%d,",&result_err);
	if(1!=ret)
	{
		printf("[net]sscanf error code=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
		return net_ftp_put_COM_ret.RET_ERROR;
	}
	
	//判断返回值
	if(0==result_err)
	{
		ret = sscanf(p,"+QFTPPUT:%d,%d",&result_err,&result_transferlen);
		if(2!=ret)
		{
			printf("[net]sscanf error code=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
			return net_ftp_put_COM_ret.RET_ERROR;
		}
	}	
	
	if(0!=result_err)
	{
		printf("[net]error. file=%s, line=%d\n", __FILE__, __LINE__);
		return net_ftp_put_COM_ret.RET_ERROR;
	}
	
	
	//函数返回
	return net_ftp_put_COM_ret.RET_OK;
}

/*
	FTP上传文件(文件方式)
*/
int net_ftp_put_FILE(char *file_name_in, char *local_name_in, int startpos_in)
{
	struct net_ftp_put_FILE_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
		const int RET_TIMEOUT;
	}net_ftp_put_FILE_ret = {0,1,2};
	
	//局部变量
	int ret=0;
	int timeout_timer=0;
	char *p=NULL;
	int result_err=0;
	int result_transferlen=0;
	
	//检查指针
	if(NULL == file_name_in)
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_ftp_put_FILE_ret.RET_ERROR;
	}
	if(NULL == local_name_in)
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_ftp_put_FILE_ret.RET_ERROR;
	}
	
	//准备指令
	sprintf(net_send_buffer,"AT+QFTPPUT=\"%s\",\"%s\",%d\r",file_name_in,local_name_in,startpos_in);

	//清空接收缓冲区
	restart_usart3_recv();
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&net_send_buffer[0]),strlen(net_send_buffer));
	
	for(timeout_timer=0; ;timeout_timer+=10)
	{
		osDelay(10);
		if(NULL!=strstr(net_recv_buffer,"+QFTPPUT:"))
		{
			common_wait_uart_receive(net_recv_buffer);
			printf("%s\n",net_recv_buffer);
			break;
		}
		if(NULL!=strstr(net_recv_buffer,"ERROR"))
		{
			printf("%s\n",net_recv_buffer);
			return net_ftp_put_FILE_ret.RET_ERROR;
		}
		if(10000 < timeout_timer)
		{
			printf("%s\n",net_recv_buffer);
			return net_ftp_put_FILE_ret.RET_TIMEOUT;
		}
	}
	
	//检查返回值
	p = strstr(net_recv_buffer,"+QFTPPUT:");
	ret = sscanf(p,"+QFTPPUT:%d,",&result_err);
	if(1!=ret)
	{
		printf("[net]sscanf error code=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
		return net_ftp_put_FILE_ret.RET_ERROR;
	}
	
	//判断返回值
	if(0==result_err)
	{
		ret = sscanf(p,"+QFTPPUT:%d,%d",&result_err,&result_transferlen);
		if(2!=ret)
		{
			printf("[net]sscanf error code=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
			return net_ftp_put_FILE_ret.RET_ERROR;
		}
	}	
	
	if(0!=result_err)
	{
		printf("[net]error. file=%s, line=%d\n", __FILE__, __LINE__);
		return net_ftp_put_FILE_ret.RET_ERROR;
	}
	
	
	//函数返回
	return net_ftp_put_FILE_ret.RET_OK;
}

/*
	下载文件
*/
int net_ftp_get(char *file_name_in, char *local_name_in, int startpos_in, int downloadlen_in, char *buffer_out)
{
	struct net_ftp_get_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
		const int RET_TIMEOUT;
	}net_ftp_get_ret = {0,1,2};
	
	//局部变量
	int ret=0;
	int timeout_timer=0;
	char *p=NULL;
	int result_err=0;
	int result_transferlen=0;
	
	//检查指针
	if(NULL == file_name_in)
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_ftp_get_ret.RET_ERROR;
	}
	if(NULL == local_name_in)
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_ftp_get_ret.RET_ERROR;
	}
	if(NULL == buffer_out)
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_ftp_get_ret.RET_ERROR;
	}
	
	//准备指令
	if(0==strcmp(local_name_in,"COM:"))
	{
		sprintf(net_send_buffer,"AT+QFTPGET=\"%s\",\"%s\",%d,%d\r",file_name_in,local_name_in,startpos_in,downloadlen_in);
	}
	else
	{
		sprintf(net_send_buffer,"AT+QFTPGET=\"%s\",\"%s\",%d\r",file_name_in,local_name_in,startpos_in);
	}
	
	//清空接收缓冲区
	restart_usart3_recv();
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&net_send_buffer[0]),strlen(net_send_buffer));
	
	for(timeout_timer=0; ;timeout_timer+=10)
	{
		osDelay(10);
		if(NULL!=strstr(net_recv_buffer,"+QFTPGET:"))
		{
			common_wait_uart_receive(net_recv_buffer);
			printf("%s\n",net_recv_buffer);
			break;
		}
		if(NULL!=strstr(net_recv_buffer,"ERROR"))
		{
			printf("%s\n",net_recv_buffer);
			return net_ftp_get_ret.RET_ERROR;
		}
		if(10000 < timeout_timer)
		{
			printf("%s\n",net_recv_buffer);
			return net_ftp_get_ret.RET_TIMEOUT;
		}
	}
	
	//检查返回值
	p = strstr(net_recv_buffer,"+QFTPGET:");
	ret = sscanf(p,"+QFTPGET:%d,",&result_err);
	if(1!=ret)
	{
		printf("[net]sscanf error code=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
		return net_ftp_get_ret.RET_ERROR;
	}
	
	//判断返回值
	if(0==result_err)
	{
		ret = sscanf(p,"+QFTPGET:%d,%d",&result_err,&result_transferlen);
		if(2!=ret)
		{
			printf("[net]sscanf error code=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
			return net_ftp_get_ret.RET_ERROR;
		}
	}	
	
	if(0!=result_err)
	{
		printf("[net]error. file=%s, line=%d\n", __FILE__, __LINE__);
		return net_ftp_get_ret.RET_ERROR;
	}
	
	//截取内容
	p = strstr(net_recv_buffer,"CONNECT\r\n");
	if(NULL!=p)
	{
		strncpy(buffer_out, p+9, downloadlen_in);
	}
	
	//函数返回                                                                                                                                  
	return net_ftp_get_ret.RET_OK;
}

/*
	获得文件长度
*/
int net_ftp_size(char *file_name_in, int *filesize_out)
{
	struct net_ftp_size_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
		const int RET_TIMEOUT;
	}net_ftp_size_ret = {0,1,2};
	
	//局部变量
	int ret=0;
	int timeout_timer=0;
	char *p=NULL;
	int result_err=0;
	int result_filesize=0;
	
	//检查指针
	if(NULL == file_name_in)
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_ftp_size_ret.RET_ERROR;
	}

	//准备指令
	sprintf(net_send_buffer,"AT+QFTPSIZE=\"%s\"\r",file_name_in);

	//清空接收缓冲区
	restart_usart3_recv();
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&net_send_buffer[0]),strlen(net_send_buffer));
	
	for(timeout_timer=0; ;timeout_timer+=10)
	{
		osDelay(10);
		if(NULL!=strstr(net_recv_buffer,"+QFTPSIZE:"))
		{
			common_wait_uart_receive(net_recv_buffer);
			printf("%s\n",net_recv_buffer);
			break;
		}
		if(NULL!=strstr(net_recv_buffer,"ERROR"))
		{
			printf("%s\n",net_recv_buffer);
			return net_ftp_size_ret.RET_ERROR;
		}
		if(10000 < timeout_timer)
		{
			printf("%s\n",net_recv_buffer);
			return net_ftp_size_ret.RET_TIMEOUT;
		}
	}
	
	//检查返回值
	p = strstr(net_recv_buffer,"+QFTPSIZE:");
	ret = sscanf(p,"+QFTPSIZE:%d,",&result_err);
	if(1!=ret)
	{
		printf("[net]sscanf error code=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
		return net_ftp_size_ret.RET_ERROR;
	}
	
	//判断返回值
	if(0==result_err)
	{
		ret = sscanf(p,"+QFTPSIZE:%d,%d",&result_err,&result_filesize);
		if(2!=ret)
		{
			printf("[net]sscanf error code=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
			return net_ftp_size_ret.RET_ERROR;
		}
		*filesize_out = result_filesize;
	}	
	
	if(0!=result_err)
	{
		printf("[net]error. file=%s, line=%d\n", __FILE__, __LINE__);
		return net_ftp_size_ret.RET_ERROR;
	}
	
	
	//函数返回
	return net_ftp_size_ret.RET_OK;
}

/*
	删除文件
*/
int net_ftp_del(char *file_name_in)
{
	struct net_ftp_del_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
		const int RET_TIMEOUT;
	}net_ftp_del_ret = {0,1,2};
	
	//局部变量
	int ret=0;
	int timeout_timer=0;
	char *p=NULL;
	int result_err=0;
	
	//检查指针
	if(NULL == file_name_in)
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_ftp_del_ret.RET_ERROR;
	}

	//准备指令
	sprintf(net_send_buffer,"AT+QFTPDEL=\"%s\"\r",file_name_in);

	//清空接收缓冲区
	restart_usart3_recv();
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&net_send_buffer[0]),strlen(net_send_buffer));
	
	for(timeout_timer=0; ;timeout_timer+=10)
	{
		osDelay(10);
		if(NULL!=strstr(net_recv_buffer,"+QFTPDEL:"))
		{
			common_wait_uart_receive(net_recv_buffer);
			printf("%s\n",net_recv_buffer);
			break;
		}
		if(NULL!=strstr(net_recv_buffer,"ERROR"))
		{
			printf("%s\n",net_recv_buffer);
			return net_ftp_del_ret.RET_ERROR;
		}
		if(10000 < timeout_timer)
		{
			printf("%s\n",net_recv_buffer);
			return net_ftp_del_ret.RET_TIMEOUT;
		}
	}
	
	//检查返回值
	p = strstr(net_recv_buffer,"+QFTPDEL:");
	ret = sscanf(p,"+QFTPDEL:%d,",&result_err);
	if(1!=ret)
	{
		printf("[net]sscanf error code=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
		return net_ftp_del_ret.RET_ERROR;
	}
	
	//判断返回值
	if(0!=result_err)
	{
		printf("[net]error. file=%s, line=%d\n", __FILE__, __LINE__);
		return net_ftp_del_ret.RET_ERROR;
	}
	
	
	//函数返回
	return net_ftp_del_ret.RET_OK;
}

/*
	建立文件夹
*/
int net_ftp_mkdir(char *folder_name_in)
{
	struct net_ftp_mkdir_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
		const int RET_TIMEOUT;
	}net_ftp_mkdir_ret = {0,1,2};
	
	//局部变量
	int ret=0;
	int timeout_timer=0;
	char *p=NULL;
	int result_err=0;
	
	//检查指针
	if(NULL == folder_name_in)
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_ftp_mkdir_ret.RET_ERROR;
	}

	//准备指令
	sprintf(net_send_buffer,"AT+QFTPMKDIR=\"%s\"\r",folder_name_in);

	//清空接收缓冲区
	restart_usart3_recv();
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&net_send_buffer[0]),strlen(net_send_buffer));
	
	for(timeout_timer=0; ;timeout_timer+=10)
	{
		osDelay(10);
		if(NULL!=strstr(net_recv_buffer,"+QFTPMKDIR:"))
		{
			common_wait_uart_receive(net_recv_buffer);
			printf("%s\n",net_recv_buffer);
			break;
		}
		if(NULL!=strstr(net_recv_buffer,"ERROR"))
		{
			printf("%s\n",net_recv_buffer);
			return net_ftp_mkdir_ret.RET_ERROR;
		}
		if(10000 < timeout_timer)
		{
			printf("%s\n",net_recv_buffer);
			return net_ftp_mkdir_ret.RET_TIMEOUT;
		}
	}
	
	//检查返回值
	p = strstr(net_recv_buffer,"+QFTPMKDIR:");
	ret = sscanf(p,"+QFTPMKDIR:%d,",&result_err);
	if(1!=ret)
	{
		printf("[net]sscanf error code=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
		return net_ftp_mkdir_ret.RET_ERROR;
	}
	
	//判断返回值
	if(0!=result_err)
	{
		printf("[net]error. file=%s, line=%d\n", __FILE__, __LINE__);
		return net_ftp_mkdir_ret.RET_ERROR;
	}
	
	//函数返回
	return net_ftp_mkdir_ret.RET_OK;
}


/*
	删除文件夹
*/
int net_ftp_rmdir(char *folder_name_in)
{
	struct net_ftp_rmdir_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
		const int RET_TIMEOUT;
	}net_ftp_rmdir_ret = {0,1,2};
	
	//局部变量
	int ret=0;
	int timeout_timer=0;
	char *p=NULL;
	int result_err=0;
	
	//检查指针
	if(NULL == folder_name_in)
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_ftp_rmdir_ret.RET_ERROR;
	}

	//准备指令
	sprintf(net_send_buffer,"AT+QFTPRMDIR=\"%s\"\r",folder_name_in);

	//清空接收缓冲区
	restart_usart3_recv();
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&net_send_buffer[0]),strlen(net_send_buffer));
	
	for(timeout_timer=0; ;timeout_timer+=10)
	{
		osDelay(10);
		if(NULL!=strstr(net_recv_buffer,"+QFTPRMDIR:"))
		{
			common_wait_uart_receive(net_recv_buffer);
			printf("%s\n",net_recv_buffer);
			break;
		}
		if(NULL!=strstr(net_recv_buffer,"ERROR"))
		{
			printf("%s\n",net_recv_buffer);
			return net_ftp_rmdir_ret.RET_ERROR;
		}
		if(10000 < timeout_timer)
		{
			printf("%s\n",net_recv_buffer);
			return net_ftp_rmdir_ret.RET_TIMEOUT;
		}
	}
	
	//检查返回值
	p = strstr(net_recv_buffer,"+QFTPRMDIR:");
	ret = sscanf(p,"+QFTPRMDIR:%d,",&result_err);
	if(1!=ret)
	{
		printf("[net]sscanf error code=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
		return net_ftp_rmdir_ret.RET_ERROR;
	}
	
	//判断返回值
	if(0!=result_err)
	{
		printf("[net]error. file=%s, line=%d\n", __FILE__, __LINE__);
		return net_ftp_rmdir_ret.RET_ERROR;
	}
	
	//函数返回
	return net_ftp_rmdir_ret.RET_OK;
}

/*
	获得ftp服务器状态
*/
int net_ftp_stat(int *ftpstat_out)
{
	struct net_ftp_stat_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
		const int RET_TIMEOUT;
	}net_ftp_stat_ret = {0,1,2};
	
	//局部变量
	int ret=0;
	int timeout_timer=0;
	char *p=NULL;
	int result_err=0;
	int result_ftpstat=0;
	
	//检查指针
	if(NULL == ftpstat_out)
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_ftp_stat_ret.RET_ERROR;
	}

	//准备指令
	sprintf(net_send_buffer,"AT+QFTPSTAT\r");

	//清空接收缓冲区
	restart_usart3_recv();
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&net_send_buffer[0]),strlen(net_send_buffer));
	
	for(timeout_timer=0; ;timeout_timer+=10)
	{
		osDelay(10);
		if(NULL!=strstr(net_recv_buffer,"+QFTPSTAT:"))
		{
			common_wait_uart_receive(net_recv_buffer);
			printf("%s\n",net_recv_buffer);
			break;
		}
		if(NULL!=strstr(net_recv_buffer,"ERROR"))
		{
			printf("%s\n",net_recv_buffer);
			return net_ftp_stat_ret.RET_ERROR;
		}
		if(10000 < timeout_timer)
		{
			printf("%s\n",net_recv_buffer);
			return net_ftp_stat_ret.RET_TIMEOUT;
		}
	}
	
	//检查返回值
	p = strstr(net_recv_buffer,"+QFTPSTAT:");
	ret = sscanf(p,"+QFTPSTAT:%d,%d",&result_err,&result_ftpstat);
	if(2!=ret)
	{
		printf("[net]sscanf error code=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
		return net_ftp_stat_ret.RET_ERROR;
	}
	
	//判断返回值
	if(0!=result_err)
	{
		printf("[net]error. file=%s, line=%d\n", __FILE__, __LINE__);
		return net_ftp_stat_ret.RET_ERROR;
	}
	
	*ftpstat_out = result_ftpstat;
	
	switch(result_ftpstat)
	{
		case 0:
			printf("[net] Opening an FTP(S) server\n");
			break;
		case 1:
			printf("[net] The FTP(S) server is open and idle\n");
			break;
		case 2:
			printf("[net] Transferring data with FTP(S) server\n");
			break;
		case 3:
			printf("[net] Closing the FTP(S) server\n");
			break;
		case 4:
			printf("[net] The FTP(S) server is closed\n");
			break;
		default:
			printf("[net]error. file=%s, line=%d\n", __FILE__, __LINE__);
			return net_ftp_stat_ret.RET_ERROR;
			//break;
	}
	
	//函数返回
	return net_ftp_stat_ret.RET_OK;
}

/*
	关闭ftp连接
*/
int net_ftp_close(void)
{
	struct net_ftp_close_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
		const int RET_TIMEOUT;
	}net_ftp_close_ret = {0,1,2};
	
	//局部变量
	int ret=0;
	int timeout_timer=0;
	char *p=NULL;
	int result_err=0;
	int result_protocol_error=0;
	

	//准备指令
	sprintf(net_send_buffer,"AT+QFTPCLOSE\r");

	//清空接收缓冲区
	restart_usart3_recv();
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&net_send_buffer[0]),strlen(net_send_buffer));
	
	for(timeout_timer=0; ;timeout_timer+=10)
	{
		osDelay(10);
		if(NULL!=strstr(net_recv_buffer,"+QFTPCLOSE:"))
		{
			common_wait_uart_receive(net_recv_buffer);
			printf("%s\n",net_recv_buffer);
			break;
		}
		if(NULL!=strstr(net_recv_buffer,"ERROR"))
		{
			printf("%s\n",net_recv_buffer);
			printf("[net]error. file=%s, line=%d\n", __FILE__, __LINE__);
			return net_ftp_close_ret.RET_ERROR;
		}
		if(10000 < timeout_timer)
		{
			printf("%s\n",net_recv_buffer);
			printf("[net]error. file=%s, line=%d\n", __FILE__, __LINE__);
			return net_ftp_close_ret.RET_TIMEOUT;
		}
	}
	
	//检查返回值
	p = strstr(net_recv_buffer,"+QFTPCLOSE:");
	ret = sscanf(p,"+QFTPCLOSE:%d,%d",&result_err,&result_protocol_error);
	if(2!=ret)
	{
		printf("[net]sscanf error code=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
		return net_ftp_close_ret.RET_ERROR;
	}
	
	//判断返回值
	if(0!=result_err)
	{
		printf("[net]error. file=%s, line=%d\n", __FILE__, __LINE__);
		return net_ftp_close_ret.RET_ERROR;
	}
	
	//函数返回
	return net_ftp_close_ret.RET_OK;
}

/*
	获取基站号
*/

int net_get_base_station(char *lac_out, char *ci_out)
{
	struct net_get_base_station_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
		const int RET_TIMEOUT;
	}net_get_base_station_ret = {0,1,2};
	
	//局部变量
	int ret=0;
	int timeout_timer=0;
	char *p=NULL;
	int result_stat=0;
	int result_act=0;
	int result_mode=0;
	
	//检查指针
	if(NULL == lac_out)
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_get_base_station_ret.RET_ERROR;
	}
	
	if(NULL == ci_out)
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_get_base_station_ret.RET_ERROR;
	}

	//设置模式
	ret = net_at_send("AT+CREG=2\r",5000);
	if(0!=ret)
	{
		printf("[net]error ret=%d. file=%s, line=%d\n", ret, __FILE__,__LINE__);
		return net_get_base_station_ret.RET_ERROR;
	}
	
	
	//准备指令
	sprintf(net_send_buffer,"AT+CREG?\r");

	//清空接收缓冲区
	restart_usart3_recv();
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&net_send_buffer[0]),strlen(net_send_buffer));
	
	for(timeout_timer=0; ;timeout_timer+=10)
	{
		osDelay(10);
		if(NULL!=strstr(net_recv_buffer,"+CREG:"))
		{
			common_wait_uart_receive(net_recv_buffer);
			printf("%s\n",net_recv_buffer);
			break;
		}
		if(NULL!=strstr(net_recv_buffer,"ERROR"))
		{
			printf("%s\n",net_recv_buffer);
			return net_get_base_station_ret.RET_ERROR;
		}
		if(10000 < timeout_timer)
		{
			
			printf("%s\n",net_recv_buffer);
			return net_get_base_station_ret.RET_TIMEOUT;
		}
		if((timeout_timer % 500) == 0)
		{
			if(Feed_dog == 0)
			{
				feed_dog_func(0);
//			  M41T65_write_data(0x2A,9);
			}
		}
	}
	if(Feed_dog == 0)
	{
		feed_dog_func(0);
//	    M41T65_write_data(0x2A,9);
	}
	//检查返回值
	p = strstr(net_recv_buffer,"+CREG:");
	ret = sscanf(p,"+CREG:%d,%d,\"%[^\"]\",\"%[^\"]\",%d", &result_mode, &result_stat, lac_out, ci_out, &result_act);
	if(5!=ret)
	{
		printf("[net]sscanf error code=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
		return net_get_base_station_ret.RET_ERROR;
	}
	
	//判断返回值
	if(0==result_stat)
	{
		printf("[net]error. file=%s, line=%d\n", __FILE__, __LINE__);
		return net_get_base_station_ret.RET_ERROR;
	}
	
	//函数返回
	return net_get_base_station_ret.RET_OK;
}

/*
	查看是否依附分组域
*/
int net_check_packet_domain(int *state_out)
{
	struct net_check_packet_domain_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
		const int RET_TIMEOUT;
	}net_check_packet_domain_ret = {0,1,2};
	
	//局部变量
	int ret=0;
	int timeout_timer=0;
	char *p=NULL;
	int result_stat=0;
	
	//检查指针
	if(NULL == state_out)
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_check_packet_domain_ret.RET_ERROR;
	}
	
	//准备指令
	sprintf(net_send_buffer,"AT+CGATT?\r");

	//清空接收缓冲区
	restart_usart3_recv();
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&net_send_buffer[0]),strlen(net_send_buffer));

	
	for(timeout_timer=0; ;timeout_timer+=10)
	{
		osDelay(10);
		if(NULL!=strstr(net_recv_buffer,"+CGATT:"))
		{
			common_wait_uart_receive(net_recv_buffer);
			printf("%s\n",net_recv_buffer);
			break;
		}
		if(NULL!=strstr(net_recv_buffer,"ERROR"))
		{
			printf("%s\n",net_recv_buffer);
			return net_check_packet_domain_ret.RET_ERROR;
		}
		if(10000 < timeout_timer)
		{
			printf("%s\n",net_recv_buffer);
			return net_check_packet_domain_ret.RET_TIMEOUT;
		}
	}
	
	//检查返回值
	p = strstr(net_recv_buffer,"+CGATT:");
	ret = sscanf(p,"+CGATT:%d", &result_stat);
	if(1!=ret)
	{
		printf("[net]sscanf error ret=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
		return net_check_packet_domain_ret.RET_ERROR;
	}
	
	//赋值
	*state_out = result_stat;
	
	//函数返回
	return net_check_packet_domain_ret.RET_OK;
}

/*
	查看MQTT连接状态
*/
int net_check_mqtt_connect(int *state_out)
{
	struct net_check_mqtt_connect_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
		const int RET_TIMEOUT;
	}net_check_mqtt_connect_ret = {0,1,2};
	
	//局部变量
	int ret=0;
	int timeout_timer=0;
	char *p=NULL;
	int result_stat=0;
	int client_idx=0;
	
	//检查指针
	if(NULL == state_out)
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_check_mqtt_connect_ret.RET_ERROR;
	}
	
	//准备指令
	sprintf(net_send_buffer,"AT+QMTCONN?\r");

	//清空接收缓冲区
	restart_usart3_recv();
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&net_send_buffer[0]),strlen(net_send_buffer));
	for(timeout_timer=0; ;timeout_timer+=10)
	{
		osDelay(10);
		if(NULL!=strstr(net_recv_buffer,"+QMTCONN:"))
		{
			common_wait_uart_receive(net_recv_buffer);
			printf("%s\n",net_recv_buffer);
			break;
		}
		if(NULL!=strstr(net_recv_buffer,"ERROR"))
		{
			printf("%s\n",net_recv_buffer);
			return net_check_mqtt_connect_ret.RET_ERROR;
		}
		if(10000 < timeout_timer)
		{
			printf("%s\n",net_recv_buffer);
			return net_check_mqtt_connect_ret.RET_TIMEOUT;
		}
	}
	
	//检查返回值
	p = strstr(net_recv_buffer,"+QMTCONN:");
	ret = sscanf(p,"+QMTCONN:%d,%d", &client_idx,&result_stat);
	if(2!=ret)
	{
		printf("[net]sscanf error ret=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
		return net_check_mqtt_connect_ret.RET_ERROR;
	}
	
	//赋值
	*state_out = result_stat;
	
	//函数返回
	return net_check_mqtt_connect_ret.RET_OK;
}

/*
	查看ICCID
*/
int net_get_iccid(char *iccid_out)
{
	struct net_get_iccid_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
		const int RET_TIMEOUT;
	}net_get_iccid_ret = {0,1,2};
	
	//局部变量
	int ret=0;
	int timeout_timer=0;
	char *p=NULL;
	char result_iccid[32]={0};
	
	//检查指针
	if(NULL == iccid_out)
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_get_iccid_ret.RET_ERROR;
	}
	
	//准备指令
	sprintf(net_send_buffer,"AT+QCCID\r");

	//清空接收缓冲区
	restart_usart3_recv();
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&net_send_buffer[0]),strlen(net_send_buffer));
	
	for(timeout_timer=0; ;timeout_timer+=10)
	{
		osDelay(10);
		if(NULL!=strstr(net_recv_buffer,"+QCCID:"))
		{
			common_wait_uart_receive(net_recv_buffer);
			printf("%s\n",net_recv_buffer);
			break;
		}
		if(NULL!=strstr(net_recv_buffer,"ERROR"))
		{
			printf("%s\n",net_recv_buffer);
			return net_get_iccid_ret.RET_ERROR;
		}
		if(10000 < timeout_timer)
		{
			printf("%s\n",net_recv_buffer);
			return net_get_iccid_ret.RET_TIMEOUT;
		}
	}
	
	//检查返回值
	p = strstr(net_recv_buffer,"+QCCID:");
	ret = sscanf(p,"+QCCID:%[^\r]", result_iccid);
	if(1!=ret)
	{
		printf("[net]sscanf error ret=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
		return net_get_iccid_ret.RET_ERROR;
	}
	
	//赋值
	strcpy(iccid_out, result_iccid);
	
	//函数返回
	return net_get_iccid_ret.RET_OK;
}

/*
	查看EC20F/EC200U ID
*/
int net_get_device_ID(char *iccid_out)
{
	struct net_get_iccid_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
		const int RET_TIMEOUT;
	}net_get_iccid_ret = {0,1,2};
	
	//局部变量
	int ret=0;
	int timeout_timer=0;
	char *p=NULL;
	char result_iccid[32]={0};
	
	//检查指针
	if(NULL == iccid_out)
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_get_iccid_ret.RET_ERROR;
	}
	
	//准备指令
	sprintf(net_send_buffer,"ATI\r");

	//清空接收缓冲区
	restart_usart3_recv();
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&net_send_buffer[0]),strlen(net_send_buffer));
	
	for(timeout_timer=0; ;timeout_timer+=10)
	{
		osDelay(10);
		if(NULL!=strstr(net_recv_buffer,"Quectel"))
		{
			common_wait_uart_receive(net_recv_buffer);
			printf("%s\n",net_recv_buffer);
			break;
		}
		if(NULL!=strstr(net_recv_buffer,"ERROR"))
		{
			printf("%s\n",net_recv_buffer);
			return net_get_iccid_ret.RET_ERROR;
		}
		if(10000 < timeout_timer)
		{
			printf("%s\n",net_recv_buffer);
			return net_get_iccid_ret.RET_TIMEOUT;
		}
	}
	
	//检查返回值
	p = strstr(net_recv_buffer,"EC20");
	ret = sscanf(p,"%[^\r]", result_iccid);
	if(1!=ret)
	{
		printf("[net]sscanf error ret=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
		return net_get_iccid_ret.RET_ERROR;
	}
	
	//赋值
	strcpy(iccid_out, result_iccid);
	
	//函数返回
	return net_get_iccid_ret.RET_OK;
}

/*
	查看电话号
*/
int net_short_message_send(char *num)
{
	struct net_get_short_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
		const int RET_TIMEOUT;
	}net_get_short_ret = {0,1,2};
	
	//局部变量
	int ret=0;
	int timeout_timer=0;
	char *p=NULL;
	char result_num[64]={0};
	
	//检查指针
	if(NULL == num)
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_get_short_ret.RET_ERROR;
	}
	
	//准备指令
	sprintf(net_send_buffer,"AT+CNUM\r");

	//清空接收缓冲区
	restart_usart3_recv();
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&net_send_buffer[0]),strlen(net_send_buffer));	
	
	for(timeout_timer=0; ;timeout_timer+=10)
	{
		DDL_DelayMS(10);
		if(NULL!=strstr(net_recv_buffer,"Quectel"))
		{
			common_wait_uart_receive(net_recv_buffer);
			printf("%s\n",net_recv_buffer);
			break;
		}
		if(NULL!=strstr(net_recv_buffer,"ERROR"))
		{
			printf("%s\n",net_recv_buffer);
			return net_get_short_ret.RET_ERROR;
		}
		if(1000 < timeout_timer)
		{
			printf("%s\n",net_recv_buffer);
			return net_get_short_ret.RET_TIMEOUT;
		}
	}
	
	//检查返回值
	p = strstr(net_recv_buffer,"EC20");
	ret = sscanf(p,"%[^\r]", result_num);
	if(1!=ret)
	{
		printf("[net]sscanf error ret=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
		return net_get_short_ret.RET_ERROR;
	}
	
	//赋值
	strcpy(num, result_num);
	
	//函数返回
	return net_get_short_ret.RET_OK;
	
}
/*
	终端发送短信，开关机状态
*/
int net_short_message_device(char *device_state,char *da)
{
	struct net_short_message_device_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
		const int RET_TIMEOUT;
	}net_short_message_device_ret = {0,1};
	int timeout_timer=0;
	//检查指针
	if(NULL == device_state)
	{
		printf("[net] net_mqtt_open error. file=%s, line=%d",__FILE__,__LINE__);
		return net_short_message_device_ret.RET_ERROR;
	}
	if(NULL == da)
	{
		printf("[net] net_mqtt_open error. file=%s, line=%d",__FILE__,__LINE__);
		return net_short_message_device_ret.RET_ERROR;
	}
	//准备指令
	memset(net_send_buffer, 0, sizeof(net_send_buffer));
	sprintf(net_send_buffer,"AT+CMGF=1\r");
	restart_usart3_recv();
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&net_send_buffer[0]),strlen(net_send_buffer));	
	
	for(timeout_timer=0; ;timeout_timer+=10)
	{
		DDL_DelayMS(10);
		if(NULL!=strstr(net_recv_buffer,"OK"))
		{
			common_wait_uart_receive(net_recv_buffer);
			printf("%s\n",net_recv_buffer);
			break;
		}
		if(NULL!=strstr(net_recv_buffer,"ERROR"))
		{
			printf("%s\n",net_recv_buffer);
			return net_short_message_device_ret.RET_ERROR;
		}
		if(1000 < timeout_timer)
		{
			printf("%s\n",net_recv_buffer);
			return net_short_message_device_ret.RET_TIMEOUT;
		}
	}
	//检查返回值

	//准备指令
	memset(net_send_buffer, 0, sizeof(net_send_buffer));
	sprintf(net_send_buffer,"AT+CSCS=\"GSM\"\r");
	restart_usart3_recv();
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&net_send_buffer[0]),strlen(net_send_buffer));	
	for(timeout_timer=0; ;timeout_timer+=10)
	{
		DDL_DelayMS(10);
		if(NULL!=strstr(net_recv_buffer,"OK"))
		{
			common_wait_uart_receive(net_recv_buffer);
			printf("%s\n",net_recv_buffer);
			break;
		}
		if(NULL!=strstr(net_recv_buffer,"ERROR"))
		{
			printf("%s\n",net_recv_buffer);
			return net_short_message_device_ret.RET_ERROR;
		}
		if(1000 < timeout_timer)
		{
			printf("%s\n",net_recv_buffer);
			return net_short_message_device_ret.RET_TIMEOUT;
		}
	}
	//准备数据
	memset(net_send_buffer, 0, sizeof(net_send_buffer));
	sprintf(net_send_buffer,"AT+CMGS=\"%s\"\r<%s %s><Ctrl+Z>\r",da,global_device_info.terminal_sn,device_state);
	restart_usart3_recv();
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&net_send_buffer[0]),strlen(net_send_buffer));	
	for(timeout_timer=0; ;timeout_timer+=10)
	{
		DDL_DelayMS(10);
		if(NULL!=strstr(net_recv_buffer,"+CMGS:"))
		{
			common_wait_uart_receive(net_recv_buffer);
			printf("%s\n",net_recv_buffer);
			break;
		}
		if(NULL!=strstr(net_recv_buffer,"ERROR"))
		{
			printf("%s\n",net_recv_buffer);
			return net_short_message_device_ret.RET_ERROR;
		}
		if(1000 < timeout_timer)
		{
			printf("%s\n",net_recv_buffer);
			return net_short_message_device_ret.RET_TIMEOUT;
		}
	}
	return net_short_message_device_ret.RET_OK;
}
