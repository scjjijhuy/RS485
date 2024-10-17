/*
	����ģ�鶨λ�����ײ�ͷ�ļ�
	���ߣ�lee
	ʱ�䣺2020/1/13
*/

#include "main.h"

extern UART_HandleTypeDef huart4;
extern DMA_HandleTypeDef hdma_uart4_rx;
extern DMA_HandleTypeDef hdma_uart4_tx;

extern char send_buffer[1024];
extern char recv_buffer[1024];


/*
	AT+QGPSCFG ����GNSS
*/
int net_gnss_configrate(struct gnss_config_struct *gnss_config_in)
{
	struct net_gnss_configrate_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_gnss_configrate_ret = {0,1};
	
	//�ֲ�����
	int ret=0;
	
	//�ȼ��ָ��
	if(NULL == gnss_config_in)
	{
		printf("[net] error. file=%s, line=%d",__FILE__,__LINE__);
		goto RET_ERROR;
	}
	
	//����NEMA�����
	sprintf(send_buffer,"AT+QGPSCFG=\"outport\",\"%s\"\r", gnss_config_in->outport);
	
	ret = net_at_send(send_buffer,5000);
	if(0!=ret)
	{
		goto RET_ERROR;
	}
	
	//�������NEMA���
	if(0>gnss_config_in->nmeasrc || 1<gnss_config_in->nmeasrc)
	{
		printf("[net] error. file=%s, line=%d",__FILE__,__LINE__);
		goto RET_ERROR;
	}
	
	sprintf(send_buffer,"AT+QGPSCFG=\"nmeasrc\",%d\r", gnss_config_in->nmeasrc);
	
	ret = net_at_send(send_buffer,5000);
	if(0!=ret)
	{
		goto RET_ERROR;
	}
	
	//������� GPS NMEA �������
	sprintf(send_buffer,"AT+QGPSCFG=\"gpsnmeatype\",%d\r", gnss_config_in->gpsnmeatype);
	
	ret = net_at_send(send_buffer,5000);
	if(0!=ret)
	{
		goto RET_ERROR;
	}
	
	/*
	//������� GLONASS NMEA �������
	sprintf(send_buffer,"AT+QGPSCFG=\"glonassnmeatype\",%d\r", gnss_config_in->glonassnmeatype);
	
	ret = net_at_send(send_buffer,5000);
	if(0!=ret)
	{
		goto RET_ERROR;
	}
	
	//�����Ƿ����� GLONASS
	sprintf(send_buffer,"AT+QGPSCFG=\"glonassenable\",%d\r", gnss_config_in->glonassenable);
	
	ret = net_at_send(send_buffer,5000);
	if(0!=ret)
	{
		goto RET_ERROR;
	}
	*/
	
	//����ODPģʽ
	sprintf(send_buffer,"AT+QGPSCFG=\"odpcontrol\",%d\r", gnss_config_in->odpcontrol);
	
	ret = net_at_send(send_buffer,5000);
	if(0!=ret)
	{
		goto RET_ERROR;
	}
	
	//�رա����� DPO
	sprintf(send_buffer,"AT+QGPSCFG=\"dpoenable\",%d\r", gnss_config_in->dpoenable);
	
	ret = net_at_send(send_buffer,5000);
	if(0!=ret)
	{
		goto RET_ERROR;
	}
	
	//�����û���Ϳ�����
	sprintf(send_buffer,"AT+QGPSCFG=\"plane\",%d\r", gnss_config_in->plane);
	
	ret = net_at_send(send_buffer,5000);
	if(0!=ret)
	{
		goto RET_ERROR;
	}
	
	//��������
	goto RET_OK;
	
	RET_OK:
		return net_gnss_configrate_ret.RET_OK;
	
	RET_ERROR:
		return net_gnss_configrate_ret.RET_ERROR;
}

/*
	ɾ����������

	0 ��XTRA�������е����ݣ�������
	1 ��ɾ���κ����ݣ�������
	2 ɾ����������ݣ�������
	3 ɾ��XTRA����
*/
int net_gnss_delete(int deletetype_in)
{
	struct net_gnss_delete_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_gnss_delete_ret = {0,1};
	
	//�ֲ�����
	int ret=0;
	
	//������
	if(0>deletetype_in || 3<deletetype_in)
	{
		printf("[net] error. file=%s, line=%d",__FILE__,__LINE__);
		goto RET_ERROR;
	}
	
	//׼��ָ��
	sprintf(send_buffer,"AT+QGPSDEL=%d\r", deletetype_in);
	
	ret = net_at_send(send_buffer,5000);
	if(0!=ret)
	{
		goto RET_ERROR;
	}
	
	//��������
	goto RET_OK;
	
	RET_OK:
		return net_gnss_delete_ret.RET_OK;
	
	RET_ERROR:
		return net_gnss_delete_ret.RET_ERROR;
}

/*
	���� GNSS

fixmaxtime : ���λʱ�䣬��λ s �� 1~255��Ĭ��30
fixmaxdist �� ��λ��ȷ����ֵ����λ m ��0~1000��Ĭ��50
fixcount : ��λ���ξ͹ص�GNSS���棬Ϊ0��������λ����������ʵ�ʵĶ�λ����,0~1000
fixrate ���������εĶ�λ�������λ s ��1~65535��Ĭ��1
*/
int net_gnss_operate(int fixmaxtime_in, int fixmaxdist_in, int fixcount_in, int fixrate_in)
{
	struct net_gnss_operate_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_gnss_operate_ret = {0,1};
	
	//�ֲ�����
	int ret=0;
	
	//������
	if(1>fixmaxtime_in || 255<fixmaxtime_in)
	{
		printf("[net] error. file=%s, line=%d",__FILE__,__LINE__);
		goto RET_ERROR;
	}
	
	if(0>fixmaxdist_in || 1000<fixmaxdist_in)
	{
		printf("[net] error. file=%s, line=%d",__FILE__,__LINE__);
		goto RET_ERROR;
	}
	
	if(0>fixcount_in || 1000<fixcount_in)
	{
		printf("[net] error. file=%s, line=%d",__FILE__,__LINE__);
		goto RET_ERROR;
	}
	
	if(1>fixrate_in || 65535<fixrate_in)
	{
		printf("[net] error. file=%s, line=%d",__FILE__,__LINE__);
		goto RET_ERROR;
	}
	
	//׼��ָ��
	sprintf(send_buffer,"AT+QGPS=1,%d,%d,%d,%d\r", fixmaxtime_in, fixmaxdist_in, fixcount_in, fixrate_in);
	
	ret = net_at_send(send_buffer,5000);
	if(0!=ret)
	{
		goto RET_ERROR;
	}
	
	//��������
	goto RET_OK;
	
	RET_OK:
		return net_gnss_operate_ret.RET_OK;
	
	RET_ERROR:
		return net_gnss_operate_ret.RET_ERROR;
}


/*
	�ر� GNSS
*/
int net_gnss_end(void)
{
	struct net_gnss_end_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_gnss_end_ret = {0,1};
	
	//�ֲ�����
	int ret=0;
	
	//������
	
	//׼��ָ��
	sprintf(send_buffer,"AT+QGPSEND\r");
	
	ret = net_at_send(send_buffer,5000);
	if(0!=ret)
	{
		goto RET_ERROR;
	}
	
	//��������
	goto RET_OK;
	
	RET_OK:
		return net_gnss_end_ret.RET_OK;
	
	RET_ERROR:
		return net_gnss_end_ret.RET_ERROR;
}

/*
	��ȡ��λ��Ϣ
mode_in:(Ŀǰֻ֧�� 1 )
	0: <latitude>,<longitude> format: ddmm.mmmmN/S,dddmm.mmmmE/W
	1: <latitude>,<longitude> format: ddmm.mmmmmm,N/S,dddmm.mmmmmm,E/W
	2: <latitude>,<longitude> format: (-)dd.ddddd,(-)ddd.ddddd
*/
int net_gnss_locate(struct gnss_out_struct *gnss_out)
{
	struct net_gnss_locate_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
		const int RET_TIMEOUT;
	}net_gnss_locate_ret = {0,1,2};
	
	//�ֲ�����
	int ret=0;
	int timeout_timer=0;
	char *p=NULL;
	char UTC[16]={0};
	char date[16]={0};
	char temp_buffer[16]={0};
	int mode_in=1;
	
	//������
	if(NULL == gnss_out)
	{
		printf("[net] error. file=%s, line=%d",__FILE__,__LINE__);
		goto RET_ERROR;
	}
	
	if(0>mode_in || 2<mode_in)
	{
		printf("[net] error. file=%s, line=%d",__FILE__,__LINE__);
		goto RET_ERROR;
	}
	
	//׼��ָ��
	sprintf(send_buffer,"AT+QGPSLOC=%d\r", mode_in);
	
	//��ս��ջ�����
	memset(recv_buffer,0,sizeof(recv_buffer));
	
	ret = HAL_UART_DMAStop(&huart4);
	if(0!=ret)
	{
		printf("[net]HAL_UART_DMAStop error code=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
	}
	
	HAL_DMA_Abort_IT(&hdma_uart4_rx);
	
	ret = HAL_UART_Receive_DMA(&huart4, (uint8_t*)recv_buffer, sizeof(recv_buffer)-1);
	if(0!=ret)
	{
		printf("[net]HAL_UART_Receive_DMA error code=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
	}
	
	ret = HAL_UART_Transmit_DMA(&huart4, (uint8_t*)send_buffer, strlen(send_buffer));
	if(0!=ret)
	{
		printf("[net]HAL_UART_Transmit_DMA error code=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
	}
	
	for(timeout_timer=0; ;timeout_timer+=10)
	{
		osDelay(10);
		if(NULL!=strstr(recv_buffer,"+QGPSLOC:"))
		{
			common_wait_uart_receive(recv_buffer);
			printf("%s",recv_buffer);
			break;
		}
		if(NULL!=strstr(recv_buffer,"ERROR"))
		{
			printf("%s",recv_buffer);
			goto RET_ERROR;
		}
		if(10000 < timeout_timer)
		{
			printf("%s",recv_buffer);
			goto RET_TIMEOUT;
		}
	}
	
	//��鷵��ֵ
	p = strstr(recv_buffer,"+QGPSLOC:");
	ret = sscanf(p,"+QGPSLOC: %[^,],%lf,%[^,],%lf,%[^,],%lf,%lf,%d,%lf,%lf,%lf,%[^,],%d", \
		UTC, \
		&gnss_out->latitude, \
		gnss_out->NS, \
		&gnss_out->longitude, \
		gnss_out->EW, \
		&gnss_out->hdop, \
		&gnss_out->altitude, \
		&gnss_out->fix, \
		&gnss_out->cog, \
		&gnss_out->spkm, \
		&gnss_out->spkn, \
		date, \
		&gnss_out->nsat);
	
	if(13!=ret)
	{
		printf("[net]sscanf error code=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
		goto RET_ERROR;
	}
	if(6!=strlen(date))
	{
		printf("[net]strlen error code=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
		goto RET_ERROR;
	}
	
	//����ʱ��
	temp_buffer[0]=date[4];
	temp_buffer[1]=date[5];
	temp_buffer[2]='\0';
	gnss_out->utc_time.year = atoi(temp_buffer);
	
	temp_buffer[0]=date[2];
	temp_buffer[1]=date[3];
	temp_buffer[2]='\0';
	gnss_out->utc_time.month = atoi(temp_buffer);
	
	temp_buffer[0]=date[0];
	temp_buffer[1]=date[1];
	temp_buffer[2]='\0';
	gnss_out->utc_time.day = atoi(temp_buffer);
	
	temp_buffer[0]=UTC[0];
	temp_buffer[1]=UTC[1];
	temp_buffer[2]='\0';
	gnss_out->utc_time.hour = atoi(temp_buffer);
	
	temp_buffer[0]=UTC[2];
	temp_buffer[1]=UTC[3];
	temp_buffer[2]='\0';
	gnss_out->utc_time.minute = atoi(temp_buffer);
	
	temp_buffer[0]=UTC[4];
	temp_buffer[1]=UTC[5];
	temp_buffer[2]='\0';
	gnss_out->utc_time.second = atoi(temp_buffer);
	
	//��������
	goto RET_OK;
	
	RET_OK:
	gnss_out->valid=1;
	ret = HAL_UART_DMAStop(&huart4);
	if(0!=ret)
	{
		printf("[net]HAL_UART_DMAStop error code=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
	}
	return net_gnss_locate_ret.RET_OK;
	
	RET_ERROR:
	memset(gnss_out,0,sizeof(struct gnss_out_struct));
	gnss_out->valid=0;
	ret = HAL_UART_DMAStop(&huart4);
	if(0!=ret)
	{
		printf("[net]HAL_UART_DMAStop error code=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
	}
	return net_gnss_locate_ret.RET_ERROR;
		
	RET_TIMEOUT:
	memset(gnss_out,0,sizeof(struct gnss_out_struct));
	gnss_out->valid=0;
	ret = HAL_UART_DMAStop(&huart4);
	if(0!=ret)
	{
		printf("[net]HAL_UART_DMAStop error code=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
	}
	return net_gnss_locate_ret.RET_TIMEOUT;
}

/*
	ʹ��XTRA������GNSS������Ч

xtraenable_in : 0 �رգ�1 �ֶ�ע�����ݣ� 2 �Զ�ע�����ݣ�Ĭ��0
retries_in : �Զ����ص����Դ��� 0~10��Ĭ��3
retryInt_in : �Զ����ص� �� �� ʱ���� 1~120��Ĭ��10
dloadInt_in: �Զ����ص�ʱ����,24~168,Ĭ��48
*/
int net_gnss_xtra(int xtraenable_in, int retries_in, int retryInt_in, int dloadInt_in)
{
	struct net_gnss_xtra_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_gnss_xtra_ret = {0,1};
	
	//�ֲ�����
	int ret=0;
	
	//������
	if(0>xtraenable_in || 2<xtraenable_in)
	{
		printf("[net] error. file=%s, line=%d",__FILE__,__LINE__);
		goto RET_ERROR;
	}
	
	if(0>retries_in || 10<retries_in)
	{
		printf("[net] error. file=%s, line=%d",__FILE__,__LINE__);
		goto RET_ERROR;
	}
	
	if(1>retryInt_in || 120<retryInt_in)
	{
		printf("[net] error. file=%s, line=%d",__FILE__,__LINE__);
		goto RET_ERROR;
	}
	
	if(24>dloadInt_in || 168<dloadInt_in)
	{
		printf("[net] error. file=%s, line=%d",__FILE__,__LINE__);
		goto RET_ERROR;
	}
	
	//׼��ָ��
	sprintf(send_buffer,"AT+QGPSXTRA=%d,%d,%d,%d\r", xtraenable_in, retries_in, retryInt_in, dloadInt_in);
	
	ret = net_at_send(send_buffer,5000);
	if(0!=ret)
	{
		goto RET_ERROR;
	}
	
	//��������
	goto RET_OK;
	
	RET_OK:
		return net_gnss_xtra_ret.RET_OK;
	
	RET_ERROR:
		return net_gnss_xtra_ret.RET_ERROR;
}
