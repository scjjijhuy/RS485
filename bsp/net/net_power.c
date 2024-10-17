/*
	网络模块电源管理
	作者：lee
	时间：2020/2/14
*/

#include "main.h"

//extern UART_HandleTypeDef huart4;
extern char net_send_buffer[1024];
extern char net_recv_buffer[1024];

/*
	EC20休眠配置
	1:允许休眠 0:不允许休眠
*/
int net_power_config(int sleep_enable_in)
{
	struct net_power_config_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_power_config_ret={0,1};
	
	/*局部变量*/
	int ret=0;
	
	/*检查参数*/
	if(0>sleep_enable_in || 1<sleep_enable_in)
	{
		return net_power_config_ret.RET_ERROR;
	}
	
	/*发送命令*/
	sprintf(net_send_buffer,"AT+QSCLK=%d\r",sleep_enable_in);
	ret = net_at_send(net_send_buffer ,2000);
	if(0!=ret)
	{
		printf("[net]error! , file=%s, line=%d\n", __FILE__, __LINE__);
		return net_power_config_ret.RET_ERROR;
	}
	
	/*函数返回*/
	return net_power_config_ret.RET_OK;
}

/*
	EC20休眠使能
	1:进入休眠 2:退出休眠
*/
int net_power_sleep(int enable_in)
{
	struct net_power_sleep_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_power_sleep_ret={0,1};
	
	//进入休眠
	if(1==enable_in)
	{
		NET_DTR_ON();
	}
	
	//退出休眠
	if(0==enable_in)
	{
		NET_DTR_OFF();
		osDelay(10);//如果不加这延时，EC20反应不过来
	}
	
	return net_power_sleep_ret.RET_OK;
}

