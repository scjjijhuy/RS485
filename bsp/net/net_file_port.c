/*
	����ģ���ļ������ײ�ͷ�ļ�
	���ߣ�lee
	ʱ�䣺2020/1/7
*/

#include "main.h"



extern char net_send_buffer[1024];
extern char net_recv_buffer[1024];

/*
	��ȡ�洢�ռ��С
*/
int net_file_space(char *name_pattern_in, int *free_size_out, int *total_size_out)
{
	struct net_file_space_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
		const int RET_TIMEOUT;
	}net_file_space_ret = {0,1,2};
	
	//�ֲ�����
	int ret=0;
	int timeout_timer=0;
	char *p=NULL;
	int result_free_size=0;
	int result_total_size=0;
	
	//���ָ��
	if(NULL == name_pattern_in)
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_file_space_ret.RET_ERROR;
	}
	
	//׼��ָ��
	sprintf(net_send_buffer,"AT+QFLDS=\"%s\"\r", name_pattern_in);

	//��ս��ջ�����
//	memset(net_recv_buffer,0,sizeof(net_recv_buffer));
//	DMA_receive_IT(CM_USART2,CM_DMA1,DMA_CH2,(uint32_t)(&net_recv_buffer[0]),sizeof(net_recv_buffer)-1);
	restart_usart3_recv();
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&net_send_buffer[0]),strlen(net_send_buffer));
	
	
	for(timeout_timer=0; ;timeout_timer+=10)
	{
		DDL_DelayMS(10);
		if(NULL!=strstr(net_recv_buffer,"+QFLDS:"))
		{
			common_wait_uart_receive(net_recv_buffer);
			printf("%s\n",net_recv_buffer);
			break;
		}
		if(NULL!=strstr(net_recv_buffer,"ERROR"))
		{
			printf("%s\n",net_recv_buffer);
			return net_file_space_ret.RET_ERROR;
		}
		if(10000 < timeout_timer)
		{
			printf("%s\n",net_recv_buffer);
			return net_file_space_ret.RET_TIMEOUT;
		}
	}
	
	
	
	//��鷵��ֵ
	p = strstr(net_recv_buffer,"+QFLDS:");
	ret = sscanf(p,"+QFLDS:%d,%d", &result_free_size, &result_total_size);
	if(2!=ret)
	{
		printf("[net]sscanf error code=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
		return net_file_space_ret.RET_ERROR;
	}
	
	//���
	*free_size_out = result_free_size;
	*total_size_out = result_total_size;
	
	//��������
	return net_file_space_ret.RET_OK;
}

/*
	ɾ���ļ�
*/
int net_file_delete(char *filename_in)
{
	struct net_file_delete_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_file_delete_ret = {0,1};
	
	//�ֲ�����
	int ret=0;
	
	//���ָ��
	if(NULL == filename_in)
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_file_delete_ret.RET_ERROR;
	}
	
	//׼��ָ��
	sprintf(net_send_buffer,"AT+QFDEL=\"%s\"\r", filename_in);

	//����ָ��
	ret=net_at_send(net_send_buffer,5000);
	if(0!=ret)
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_file_delete_ret.RET_ERROR;
	}
	
	//��������
	return net_file_delete_ret.RET_OK;
}

/*
	�ϴ��ļ�
*/
int net_file_upload(char *filename_in, int file_size_in, int timeout_in, int ackmode_in ,char *payload_in)
{
	struct net_file_upload_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
		const int RET_TIMEOUT;
	}net_file_upload_ret = {0,1,2};
	
	//�ֲ�����
	int ret=0;
	int timeout_timer=0;
	int total_times=0;
	int i=0;
	int result_upload_size=0;
	int result_checksum=0;
	char *p=NULL;
	
	//������
	if(NULL == filename_in)
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_file_upload_ret.RET_ERROR;
	}
	
	if(NULL == payload_in)
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_file_upload_ret.RET_ERROR;
	}
	
	//׼��ָ��
	sprintf(net_send_buffer,"AT+QFUPL=\"%s\",%d,%d,%d\r", filename_in, file_size_in, timeout_in, ackmode_in);
	
	//��ս��ջ�����
//	memset(net_recv_buffer,0,sizeof(net_recv_buffer));
//	DMA_receive_IT(CM_USART2,CM_DMA1,DMA_CH2,(uint32_t)(&net_recv_buffer[0]),sizeof(net_recv_buffer)-1);
	restart_usart3_recv();
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&net_send_buffer[0]),strlen(net_send_buffer));
	
	
	for(timeout_timer=0; ;timeout_timer+=10)
	{
		DDL_DelayMS(10);
		if(NULL!=strstr(net_recv_buffer,"CONNECT"))
		{
			printf("%s\n",net_recv_buffer);
			break;
		}
		if(NULL!=strstr(net_recv_buffer,"ERROR"))
		{
			printf("%s\n",net_recv_buffer);
			return net_file_upload_ret.RET_ERROR;
		}
		if(10000 < timeout_timer)
		{
			printf("%s\n",net_recv_buffer);
			return net_file_upload_ret.RET_TIMEOUT;
		}
	}
	
	//�����ļ�����
	if(1==ackmode_in)
	{
		//������Ҫ���ռ��Ρ�A��
		total_times = file_size_in/1024;
		
		if(0==total_times)
		{
//			memset(net_recv_buffer,0,sizeof(net_recv_buffer));
//			DMA_receive_IT(CM_USART2,CM_DMA1,DMA_CH2,(uint32_t)(&net_recv_buffer[0]),sizeof(net_recv_buffer)-1);
			restart_usart3_recv();
			DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&payload_in[0]),file_size_in);
	
			for(timeout_timer=0; ;timeout_timer+=10)
			{
				DDL_DelayMS(10);
				if(NULL!=strstr(net_recv_buffer,"+QFUPL:"))
				{
					common_wait_uart_receive(net_recv_buffer);
					printf("%s\n",net_recv_buffer);
					break;
				}
				if(NULL!=strstr(net_recv_buffer,"ERROR"))
				{
					printf("%s\n",net_recv_buffer);
					return net_file_upload_ret.RET_ERROR;
				}
				if(10000 < timeout_timer)
				{
					printf("%s\n",net_recv_buffer);
					return net_file_upload_ret.RET_TIMEOUT;
				}
			}
		}
		else
		{
			for(i=0;i<total_times;i++)
			{
//				memset(net_recv_buffer,0,sizeof(net_recv_buffer));
//				DMA_receive_IT(CM_USART2,CM_DMA1,DMA_CH2,(uint32_t)(&net_recv_buffer[0]),sizeof(net_recv_buffer)-1);
				restart_usart3_recv();
				DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&payload_in[+i*1024]),file_size_in);

				for(timeout_timer=0; ;timeout_timer+=10)
				{
					DDL_DelayMS(10);
					if(NULL!=strstr(net_recv_buffer,"A"))
					{
						printf("%s\n",net_recv_buffer);
						break;
					}
					if(NULL!=strstr(net_recv_buffer,"ERROR"))
					{
						printf("%s\n",net_recv_buffer);
						return net_file_upload_ret.RET_ERROR;
					}
					if(10000 < timeout_timer)
					{
						printf("%s\n",net_recv_buffer);
						return net_file_upload_ret.RET_TIMEOUT;
					}
				}
			}
//			memset(net_recv_buffer,0,sizeof(net_recv_buffer));
//			DMA_receive_IT(CM_USART2,CM_DMA1,DMA_CH2,(uint32_t)(&net_recv_buffer[0]),sizeof(net_recv_buffer)-1);
			restart_usart3_recv();
			DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&payload_in[+i*1024]),file_size_in);

			for(timeout_timer=0; ;timeout_timer+=10)
			{
				DDL_DelayMS(10);
				if(NULL!=strstr(net_recv_buffer,"+QFUPL:"))
				{
					common_wait_uart_receive(net_recv_buffer);
					printf("%s\n",net_recv_buffer);
					break;
				}
				if(NULL!=strstr(net_recv_buffer,"ERROR"))
				{
					printf("%s\n",net_recv_buffer);
					return net_file_upload_ret.RET_ERROR;
				}
				if(10000 < timeout_timer)
				{
					printf("%s\n",net_recv_buffer);
					return net_file_upload_ret.RET_TIMEOUT;
				}
			}
		
		}
	}
	
	if(0==ackmode_in)
	{
//		memset(net_recv_buffer,0,sizeof(net_recv_buffer));
//		DMA_receive_IT(CM_USART2,CM_DMA1,DMA_CH2,(uint32_t)(&net_recv_buffer[0]),sizeof(net_recv_buffer)-1);
		restart_usart3_recv();
		DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&payload_in[0]),file_size_in);
		for(timeout_timer=0; ;timeout_timer+=10)
		{
			DDL_DelayMS(10);
			if(NULL!=strstr(net_recv_buffer,"+QFUPL:"))
			{
				common_wait_uart_receive(net_recv_buffer);
				printf("%s\n",net_recv_buffer);
				break;
			}
			if(NULL!=strstr(net_recv_buffer,"ERROR"))
			{
				printf("%s\n",net_recv_buffer);
				return net_file_upload_ret.RET_ERROR;
			}
			if(10000 < timeout_timer)
			{
				printf("%s\n",net_recv_buffer);
				return net_file_upload_ret.RET_TIMEOUT;
			}
		}
	}
	
	//��鷵��ֵ
	p = strstr(net_recv_buffer,"+QFUPL:");
	ret = sscanf(p,"+QFUPL:%d,%x", &result_upload_size, &result_checksum);
	if(2!=ret)
	{
		printf("[net]sscanf error code=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
		return net_file_upload_ret.RET_ERROR;
	}
	
	//���
	if(file_size_in != result_upload_size)
	{
		printf("[net]sscanf error code=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
		return net_file_upload_ret.RET_ERROR;
	}
	
	//��������
	return net_file_upload_ret.RET_OK;	
}

/*
	�ļ�����
*/
int net_file_download(char *filename_in, int payload_size_in, char *payload_out)
{
	struct net_file_download_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
		const int RET_TIMEOUT;
	}net_file_download_ret={0,1,2};
	
	//�ֲ�����
	int ret=0;
	int timeout_timer=0;
	int result_checksum=0;
	char *p=NULL;
	int result_download_size=0;
	int i=0;
	
	//������
	if(NULL == filename_in)
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_file_download_ret.RET_ERROR;
	}
	
	if(NULL == payload_out)
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_file_download_ret.RET_ERROR;
	}
	
	//׼��ָ��
	sprintf(net_send_buffer,"AT+QFDWL=\"%s\"\r", filename_in);
	
	//��ս��ջ�����

	memset(payload_out,0,payload_size_in);
	DMA_receive_IT(CM_USART2,CM_DMA1,DMA_CH2,(uint32_t)(&payload_out[0]),payload_size_in-1);
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&net_send_buffer[0]),strlen(net_send_buffer));

	for(timeout_timer=0; ;timeout_timer+=10)
	{
		DDL_DelayMS(10);
		if(NULL!=strstr(payload_out,"+QFDWL:"))
		{
			common_wait_uart_receive(net_recv_buffer);
			printf("%s\n",payload_out);
			break;
		}
		if(NULL!=strstr(payload_out,"ERROR"))
		{
			printf("%s\n",payload_out);
			return net_file_download_ret.RET_ERROR;
		}
		if(10000 < timeout_timer)
		{
			printf("%s\n",payload_out);
			return net_file_download_ret.RET_TIMEOUT;
		}
	}
	
	//��鷵��ֵ
	p = strstr(payload_out,"+QFDWL:");
	ret = sscanf(p,"+QFDWL:%d,%x", &result_download_size, &result_checksum);
	if(2!=ret)
	{
		printf("[net]sscanf error code=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
		return net_file_download_ret.RET_ERROR;
	}
	
	//��ȡ����
	p = strstr(payload_out,"CONNECT");
	if(NULL!=p)
	{
		for(i=0;i<(p+7-payload_out);i++)
		{
			payload_out[i]=p[i+7];
		}
		payload_out[i]='\0';
	}
	
	//��������
	return net_file_download_ret.RET_OK;
}

/*
	���ļ�
	mode:
	0 �ɶ�д��������
	1 �ɶ�д�����Դ�ļ�
	2 ֻ��
*/
int net_file_open(char *filename_in , int mode_in, int *filehandle_out)
{
	struct net_file_open_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
		const int RET_TIMEOUT;
	}net_file_open_ret={0,1,2};
	
	//�ֲ�����
	int ret=0;
	int timeout_timer=0;
	char *p=NULL;
	int result_filehandle=0;
	
	//������
	if(NULL == filename_in)
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_file_open_ret.RET_ERROR;
	}
	
	if(0>mode_in || 2<mode_in)
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_file_open_ret.RET_ERROR;
	}
	
	//׼��ָ��
	sprintf(net_send_buffer,"AT+QFOPEN=\"%s\",%d\r", filename_in, mode_in);
	
	//��ս��ջ�����
//	memset(net_recv_buffer,0,sizeof(net_recv_buffer));
//	DMA_receive_IT(CM_USART2,CM_DMA1,DMA_CH2,(uint32_t)(&net_recv_buffer[0]),sizeof(net_recv_buffer)-1);
	restart_usart3_recv();
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&net_send_buffer[0]),strlen(net_send_buffer));

	
	for(timeout_timer=0; ;timeout_timer+=10)
	{
		DDL_DelayMS(10);
		if(NULL!=strstr(net_recv_buffer,"+QFOPEN:"))
		{
			common_wait_uart_receive(net_recv_buffer);
			printf("%s\n",net_recv_buffer);
			break;
		}
		if(NULL!=strstr(net_recv_buffer,"ERROR"))
		{
			printf("%s\n",net_recv_buffer);
			return net_file_open_ret.RET_ERROR;
		}
		if(10000 < timeout_timer)
		{
			printf("%s\n",net_recv_buffer);
			return net_file_open_ret.RET_TIMEOUT;
		}
	}
	
	//��鷵��ֵ
	p = strstr(net_recv_buffer,"+QFOPEN:");
	ret = sscanf(p,"+QFOPEN:%d", &result_filehandle);
	if(1!=ret)
	{
		printf("[net]sscanf error code=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
		return net_file_open_ret.RET_ERROR;
	}
	
	//��ֵ���
	*filehandle_out = result_filehandle;
	
	//��������
	return net_file_open_ret.RET_OK;
}

/*
	���ļ�
*/
int net_file_read(int filehandle_in , int length_in, char *payload_out)
{
	struct net_file_read_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
		const int RET_TIMEOUT;
	}net_file_read_ret={0,1,2};
	
	//�ֲ�����
//	int ret=0;
	int timeout_timer=0;
	char *p=NULL;
	char *begin=NULL;
	char *end=NULL;
	
	//������	
	if(NULL == payload_out)
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_file_read_ret.RET_ERROR;
	}
	
	if(0>length_in || 1024<length_in)
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_file_read_ret.RET_ERROR;
	}
	
	//׼��ָ��
	sprintf(net_send_buffer,"AT+QFREAD=%d,%d\r", filehandle_in, length_in);
	
	//��ս��ջ�����
//	memset(net_recv_buffer,0,sizeof(net_recv_buffer));
//	DMA_receive_IT(CM_USART2,CM_DMA1,DMA_CH2,(uint32_t)(&net_recv_buffer[0]),sizeof(net_recv_buffer)-1);
	restart_usart3_recv();
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&net_send_buffer[0]),strlen(net_send_buffer));

	
	for(timeout_timer=0; ;timeout_timer+=10)
	{
		DDL_DelayMS(10);
		if(NULL!=strstr(net_recv_buffer,"\nOK"))
		{
			common_wait_uart_receive(net_recv_buffer);
			printf("%s\n",net_recv_buffer);
			break;
		}
		if(NULL!=strstr(net_recv_buffer,"ERROR"))
		{
			printf("%s\n",net_recv_buffer);
			return net_file_read_ret.RET_ERROR;
		}
		if(10000 < timeout_timer)
		{
			printf("%s\n",net_recv_buffer);
			return net_file_read_ret.RET_TIMEOUT;
		}
	}
	
	//��ȡ����
	p=strstr(net_recv_buffer,"\n");
	if(NULL!=p)
	{
		p=strstr(p+=1,"\n");
		if(NULL!=p)
		{
			begin = p+1;
			end = strstr(begin,"\nOK");
			memcpy(payload_out, begin, end-begin);
		}
		else
		{
			printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
			return net_file_read_ret.RET_ERROR;
		}
	}
	else
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_file_read_ret.RET_ERROR;
	}
	
	//��������
	return net_file_read_ret.RET_OK;
}

/*
	д�ļ�
*/
int net_file_write(int filehandle_in , int length_in, int timeout_in, char *payload_in)
{
	struct net_file_write_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
		const int RET_TIMEOUT;
	}net_file_write_ret={0,1,2};
	
	//�ֲ�����
	int ret=0;
	int timeout_timer=0;
	char *p=NULL;
	int result_written_length=0;
	int result_total_length=0;
	
	//������
	if(NULL == payload_in)
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_file_write_ret.RET_ERROR;
	}
	
	//׼��ָ��
	sprintf(net_send_buffer,"AT+QFWRITE=%d,%d,%d\r", filehandle_in, length_in,timeout_in);
	
	//��ս��ջ�����
//	memset(net_recv_buffer,0,sizeof(net_recv_buffer));
//	DMA_receive_IT(CM_USART2,CM_DMA1,DMA_CH2,(uint32_t)(&net_recv_buffer[0]),sizeof(net_recv_buffer)-1);
	restart_usart3_recv();
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&net_send_buffer[0]),strlen(net_send_buffer));

	
	for(timeout_timer=0; ;timeout_timer+=10)
	{
		DDL_DelayMS(10);
		if(NULL!=strstr(net_recv_buffer,"CONNECT"))
		{
			printf("%s\n",net_recv_buffer);
			break;
		}
		if(NULL!=strstr(net_recv_buffer,"ERROR"))
		{
			printf("%s\n",net_recv_buffer);
			return net_file_write_ret.RET_ERROR;
		}
		if(10000 < timeout_timer)
		{
			printf("%s\n",net_recv_buffer);
			return net_file_write_ret.RET_TIMEOUT;
		}
	}
	
	//��������
//	memset(net_recv_buffer,0,sizeof(net_recv_buffer));
//	DMA_receive_IT(CM_USART2,CM_DMA1,DMA_CH2,(uint32_t)(&net_recv_buffer[0]),sizeof(net_recv_buffer)-1);
	restart_usart3_recv();
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&payload_in[0]),length_in);

	for(timeout_timer=0; ;timeout_timer+=10)
	{
		DDL_DelayMS(10);
		if(NULL!=strstr(net_recv_buffer,"+QFWRITE:"))
		{
			common_wait_uart_receive(net_recv_buffer);
			printf("%s\n",net_recv_buffer);
			break;
		}
		if(NULL!=strstr(net_recv_buffer,"ERROR"))
		{
			printf("%s\n",net_recv_buffer);
			return net_file_write_ret.RET_ERROR;
		}
		if(10000 < timeout_timer)
		{
			printf("%s\n",net_recv_buffer);
			return net_file_write_ret.RET_TIMEOUT;
		}
	}
	
	//��鷵��ֵ
	p = strstr(net_recv_buffer,"+QFWRITE:");
	ret = sscanf(p,"+QFWRITE:%d,%d", &result_written_length, &result_total_length);
	if(2!=ret)
	{
		printf("[net]sscanf error code=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
		return net_file_write_ret.RET_ERROR;
	}
	
	//��������
	return net_file_write_ret.RET_OK;
}

/*
	�����ļ�ָ��
	position_in��0 ��ͷ��1 ��ǰ��2��β
*/
int net_file_seek(int filehandle_in , int offset_in, int position_in)
{
	struct net_file_seek_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_file_seek_ret={0,1};
	
	//�ֲ�����
	int ret=0;
	
	//������
	if(0>position_in || 2<position_in)
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_file_seek_ret.RET_ERROR;
	}
	
	//׼��ָ��
	sprintf(net_send_buffer,"AT+QFSEEK=%d,%d,%d\r", filehandle_in, offset_in, position_in);
	
	//����ָ��
	ret = net_at_send(net_send_buffer, 5000);
	if(0!=ret)
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_file_seek_ret.RET_ERROR;
	}
	
	//��������
	return net_file_seek_ret.RET_OK;
}

/*
	�鿴�ļ�ָ�뵱ǰƫ����
*/
int net_file_position(int filehandle_in , int *offset_out)
{
	struct net_file_position_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
		const int RET_TIMEOUT;
	}net_file_position_ret={0,1,2};
	
	//�ֲ�����
	int ret=0;
	int timeout_timer=0;
	char *p=NULL;
	int result_offset=0;
	
	//������
	if(NULL != offset_out)
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_file_position_ret.RET_ERROR;
	}
	
	//׼��ָ��
	sprintf(net_send_buffer,"AT+QFPOSITION=%d\r", filehandle_in);
	
	//��ս��ջ�����
//	memset(net_recv_buffer,0,sizeof(net_recv_buffer));
//	DMA_receive_IT(CM_USART2,CM_DMA1,DMA_CH2,(uint32_t)(&net_recv_buffer[0]),sizeof(net_recv_buffer)-1);
	restart_usart3_recv();
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&net_send_buffer[0]),strlen(net_send_buffer));
	
	for(timeout_timer=0; ;timeout_timer+=10)
	{
		DDL_DelayMS(10);
		if(NULL!=strstr(net_recv_buffer,"+QFPOSITION:"))
		{
			common_wait_uart_receive(net_recv_buffer);
			printf("%s\n",net_recv_buffer);
			break;
		}
		if(NULL!=strstr(net_recv_buffer,"ERROR"))
		{
			printf("%s\n",net_recv_buffer);
			return net_file_position_ret.RET_ERROR;
		}
		if(10000 < timeout_timer)
		{
			printf("%s\n",net_recv_buffer);
			return net_file_position_ret.RET_TIMEOUT;
		}
	}
	
	//��鷵��ֵ
	p = strstr(net_recv_buffer,"+QFPOSITION:");
	ret = sscanf(p,"+QFPOSITION:%d", &result_offset);
	if(1!=ret)
	{
		printf("[net]sscanf error code=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
		return net_file_position_ret.RET_ERROR;
	}
	
	//���
	*offset_out = result_offset;
	
	//��������
	return net_file_position_ret.RET_OK;
}

/*
	�ض��ļ�
*/
int net_file_truncate(int filehandle_in)
{
	struct net_file_truncate_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_file_truncate_ret={0,1};
	
	//�ֲ�����
	int ret=0;
	
	//׼��ָ��
	sprintf(net_send_buffer,"AT+QFTUCAT=%d\r", filehandle_in);
	
	//����ָ��
	ret = net_at_send(net_send_buffer, 5000);
	if(0!=ret)
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_file_truncate_ret.RET_ERROR;
	}
	
	//��������
	return net_file_truncate_ret.RET_OK;
}

/*
	�ر��ļ�
*/
int net_file_close(int filehandle_in)
{
	struct net_file_close_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_file_close_ret={0,1};
	
	//�ֲ�����
	int ret=0;
	
	//׼��ָ��
	sprintf(net_send_buffer,"AT+QFCLOSE=%d\r", filehandle_in);
	
	//����ָ��
	ret = net_at_send(net_send_buffer, 5000);
	if(0!=ret)
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_file_close_ret.RET_ERROR;
	}
	
	//��������
	return net_file_close_ret.RET_OK;
}

/*
	�ƶ��ļ�
*/
int net_file_move(char *srcfilename_in, char *destfilename_in, int copy_in, int overwrite_in)
{
	struct net_file_move_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_file_move_ret={0,1};
	
	//�ֲ�����
	int ret=0;
	
	//׼��ָ��
	sprintf(net_send_buffer,"AT+QFMOV=\"%s\",\"%s\",%d,%d\r", srcfilename_in, destfilename_in, copy_in, overwrite_in);
	
	//����ָ��
	ret = net_at_send(net_send_buffer, 5000);
	if(0!=ret)
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_file_move_ret.RET_ERROR;
	}
	
	//��������
	return net_file_move_ret.RET_OK;
}

/*
	��ȡ�ļ��б�
*/
int net_file_list(char *name_pattern_in, char *list_out, int *file_amount)
{
	struct net_file_list_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
		const int RET_TIMEOUT;
	}net_file_list_ret = {0,1,2};
	
	//�ֲ�����
	int ret=0;
	int timeout_timer=0;
	char *p=NULL;
	char temp_buffer[256]={0};
	int i=0;
	
	//���ָ��
	if(NULL == list_out || NULL==name_pattern_in)
	{
		printf("[net]error. file=%s, line=%d",__FILE__,__LINE__);
		return net_file_list_ret.RET_ERROR;
	}
	
	//׼��ָ��
	sprintf(net_send_buffer,"AT+QFLST=\"%s\"\r", name_pattern_in);

	//��ս��ջ�����
//	memset(net_recv_buffer,0,sizeof(net_recv_buffer));
//	DMA_receive_IT(CM_USART2,CM_DMA1,DMA_CH2,(uint32_t)(&net_recv_buffer[0]),sizeof(net_recv_buffer)-1);
	restart_usart3_recv();
	DMA_transfer_IT(CM_USART3,CM_DMA2,DMA_CH3,(uint32_t)(&net_send_buffer[0]),strlen(net_send_buffer));

	
	for(timeout_timer=0; ;timeout_timer+=10)
	{
		DDL_DelayMS(10);
		if(NULL!=strstr(net_recv_buffer,"+QFLST:"))
		{
			common_wait_uart_receive(net_recv_buffer);
			printf("%s\n",net_recv_buffer);
			break;
		}
		if(NULL!=strstr(net_recv_buffer,"ERROR"))
		{
			printf("%s\n",net_recv_buffer);
			return net_file_list_ret.RET_ERROR;
		}
		if(10000 < timeout_timer)
		{
			printf("%s\n",net_recv_buffer);
			return net_file_list_ret.RET_TIMEOUT;
		}
	}
	

	//��鷵��ֵ
	p = net_recv_buffer;
	for(i=0;;i++)
	{
		p = strstr(p,"+QFLST:");
		if(NULL == p)
		{
			break;
		}
		ret = sscanf(p,"+QFLST:%[^\r]\r", temp_buffer);
		if(1!=ret)
		{
			printf("[net]sscanf error code=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
			return net_file_list_ret.RET_ERROR;
		}
		strcat(list_out, temp_buffer);
		strcat(list_out, ",");
		p+=1;
	}
	
	//���
	*file_amount = i;
	
	//��������
	return net_file_list_ret.RET_OK;
}



