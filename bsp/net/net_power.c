/*
	����ģ���Դ����
	���ߣ�lee
	ʱ�䣺2020/2/14
*/

#include "main.h"

//extern UART_HandleTypeDef huart4;
extern char net_send_buffer[1024];
extern char net_recv_buffer[1024];

/*
	EC20��������
	1:�������� 0:����������
*/
int net_power_config(int sleep_enable_in)
{
	struct net_power_config_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_power_config_ret={0,1};
	
	/*�ֲ�����*/
	int ret=0;
	
	/*������*/
	if(0>sleep_enable_in || 1<sleep_enable_in)
	{
		return net_power_config_ret.RET_ERROR;
	}
	
	/*��������*/
	sprintf(net_send_buffer,"AT+QSCLK=%d\r",sleep_enable_in);
	ret = net_at_send(net_send_buffer ,2000);
	if(0!=ret)
	{
		printf("[net]error! , file=%s, line=%d\n", __FILE__, __LINE__);
		return net_power_config_ret.RET_ERROR;
	}
	
	/*��������*/
	return net_power_config_ret.RET_OK;
}

/*
	EC20����ʹ��
	1:�������� 2:�˳�����
*/
int net_power_sleep(int enable_in)
{
	struct net_power_sleep_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_power_sleep_ret={0,1};
	
	//��������
	if(1==enable_in)
	{
		NET_DTR_ON();
	}
	
	//�˳�����
	if(0==enable_in)
	{
		NET_DTR_OFF();
		osDelay(10);//�����������ʱ��EC20��Ӧ������
	}
	
	return net_power_sleep_ret.RET_OK;
}

