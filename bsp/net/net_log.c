/*
	��־��¼��ش���
	���ߣ�lee
	ʱ�䣺2021.3.8
*/

#include "main.h"

char log_buffer[1024];// __attribute__((section("CCRRAM")))={0};
char temp_buffer_1[1024];// __attribute__((section("CCRRAM")))={0};
char temp_buffer_2[2048];// __attribute__((section("CCRRAM")))={0};

/*
	����־�ļ����һ����¼
*/
int net_log_write(const char *buffer_in, ...)
{
	struct net_log_write_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
		const int RET_TIMEROUT;
	}net_log_write_ret = {0,1};
	
	//�ֲ�����
	char file_list[128]={0};
	int file_amount=0;
	int free_size=0;
	int total_size=0;
	int ret=0;
	int flie_handle=0;
	char *p = NULL;
	int log_size=0;
	va_list ap;
	
	//����������
	if(NULL == buffer_in)
	{
		printf("[net] error. file=%s, line=%d\n",__FILE__,__LINE__);
		net_file_close(flie_handle);
		return net_log_write_ret.RET_ERROR;
	}
	
	//�鿴�Ƿ����ļ�
	net_file_list("*", file_list, &file_amount);
	printf("[log]��ǰ��%d���ļ�,�б�����:%s\r\n", file_amount, file_list);
	
	//�鿴ʣ��ռ��С
	net_file_space("UFS", &free_size, &total_size);
	printf("[log]�ļ�ϵͳʣ���С%d���ܴ�С%d\r\n", free_size, total_size);
	
	//�����־�ļ�����ʣ���ļ���С�Ķ���֮һ��ɾ��
	p = strstr(file_list,"\"run_log.txt\"");
	if(NULL!=p)
	{
		ret=sscanf(p,"\"run_log.txt\",%d,",&log_size);
		if(1!=ret)
		{
			printf("[net] error. file=%s, line=%d\n",__FILE__,__LINE__);
			net_file_close(flie_handle);
			return net_log_write_ret.RET_ERROR;
		}
		if((total_size/2)<log_size)
		{
			net_file_delete("run_log.txt");
		}
	}
	
	//����־�ļ�
	ret = net_file_open("run_log.txt" , 0, &flie_handle);
	if(0!=ret)
	{
		printf("[net] error. file=%s, line=%d\n",__FILE__,__LINE__);
		net_file_close(flie_handle);
		return ret;
	}
	
	//�����ļ�ָ��
	ret = net_file_seek(flie_handle , 0, 2);
	if(0!=ret)
	{
		printf("[net] error. file=%s, line=%d\n",__FILE__,__LINE__);
		net_file_close(flie_handle);
		return net_log_write_ret.RET_ERROR;
	}
	
	//д������
	memset(temp_buffer_2,0,sizeof(temp_buffer_2));
	memset(temp_buffer_1,0,sizeof(temp_buffer_1));
	sprintf(temp_buffer_2, "%04d-%02d-%02d %02d:%02d:%02d ",\
					global_device_info.device_time.year+2000, \
					global_device_info.device_time.month, \
					global_device_info.device_time.day, \
					global_device_info.device_time.hour, \
					global_device_info.device_time.minute, \
					global_device_info.device_time.second);
	
	va_start(ap,buffer_in);  
  vsprintf(temp_buffer_1,buffer_in,ap);  
  va_end(ap); 
	
	strcat(temp_buffer_2, temp_buffer_1);
	ret = net_file_write(flie_handle , strlen(temp_buffer_2), 10, temp_buffer_2);
	if(0!=ret)
	{
		printf("[net] error. file=%s, line=%d\n",__FILE__,__LINE__);
		net_file_close(flie_handle);
		return net_log_write_ret.RET_ERROR;
	}
	
	//��������
	net_file_close(flie_handle);
	return net_log_write_ret.RET_OK;
}

/*
	�������������־
*/
int net_log_print(void)
{
	struct net_log_print_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_log_print_ret = {0,1};
	
	//�ֲ�����
	char file_list[128]={0};
	int file_amount=0;
	int free_size=0;
	int total_size=0;
	int ret=0;
	int flie_handle=0;
	char *p = NULL;
	int log_size=0;
	
	//�鿴�Ƿ����ļ�
	net_file_list("*", file_list, &file_amount);
	printf("[log]��ǰ��%d���ļ�,�б�����:%s\r\n", file_amount, file_list);
	
	//�鿴ʣ��ռ��С
	net_file_space("UFS", &free_size, &total_size);
	printf("[log]�ļ�ϵͳʣ���С%d���ܴ�С%d\r\n", free_size, total_size);
	
	//�����־�ļ�����ʣ���ļ���С�Ķ���֮һ��ɾ��
	p = strstr(file_list,"\"run_log.txt\"");
	if(NULL!=p)
	{
		ret=sscanf(p,"\"run_log.txt\",%d,",&log_size);
		if(1!=ret)
		{
			printf("[net] error. file=%s, line=%d\n",__FILE__,__LINE__);
			net_file_close(flie_handle);
			return net_log_print_ret.RET_ERROR;
		}
		if((total_size/2)<log_size)
		{
			net_file_delete("run_log.txt");
		}
	}
	
	//����־�ļ�
	ret = net_file_open("run_log.txt" , 0, &flie_handle);
	if(0!=ret)
	{
		printf("[net] error. file=%s, line=%d\n",__FILE__,__LINE__);
		net_file_close(flie_handle);
		return net_log_print_ret.RET_ERROR;
	}
	
	//ѭ����������
	ret = net_file_seek(flie_handle , 0, 0);
	if(0!=ret)
	{
		printf("[net] error. file=%s, line=%d\n",__FILE__,__LINE__);
		net_file_close(flie_handle);
		return net_log_print_ret.RET_ERROR;
	}
	
	for(;;)
	{
		memset(log_buffer,0,sizeof(log_buffer));

		ret = net_file_read(flie_handle, 500, log_buffer);
		if(0!=ret)
		{
			printf("[net] ��ȡ���\r\n");
			break;
		}
	}
	
	//��������
	net_file_close(flie_handle);
	return net_log_print_ret.RET_OK;
}

/*
	���沨������
*/
int net_wave_write(void)
{
	struct net_wave_write_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_wave_write_ret = {0,1};
	
	//�ֲ�����
//	int ret=0;
//	int flie_handle=0;
//	int i=0;
//	int j=0;
//	int index=0;

	
	//�½���־�ļ�
//	ret = net_file_open("wave_log.txt" , 1, &flie_handle);
//	if(0!=ret)
//	{
//		//ʧ�ܾ�����һ��
//		ret = net_file_open("wave_log.txt" , 1, &flie_handle);
//		if(0!=ret)
//		{
//			printf("[net] error. file=%s, line=%d\n",__FILE__,__LINE__);
//			net_file_close(flie_handle);
//			return net_wave_write_ret.RET_ERROR;
//		}
//	}
//	
//	//�����ļ�ָ��
//	ret = net_file_seek(flie_handle , 0, 0);
//	if(0!=ret)
//	{
//		printf("[net] error. file=%s, line=%d\n",__FILE__,__LINE__);
//		net_file_close(flie_handle);
//		return net_wave_write_ret.RET_ERROR;
//	}
//	
//	//��д���񶯷�����ʱ��
//	memset(temp_buffer_2,0,sizeof(temp_buffer_2));
//	memset(temp_buffer_1,0,sizeof(temp_buffer_1));
//	
//	sprintf(temp_buffer_1, "%04d-%02d-%02d %02d:%02d:%02d\r\n",\
//					global_mpu_info.vibration_max_time.year+2000, \
//					global_mpu_info.vibration_max_time.month, \
//					global_mpu_info.vibration_max_time.day, \
//					global_mpu_info.vibration_max_time.hour, \
//					global_mpu_info.vibration_max_time.minute, \
//					global_mpu_info.vibration_max_time.second);
//	strcat(temp_buffer_2, temp_buffer_1);
//	
//	//sprintf(temp_buffer_1, "�񶯷�ֵ��%.2f,%.2f,%.2f\r\n", global_mpu_info.vibration_max_x, global_mpu_info.vibration_max_y, global_mpu_info.vibration_max_z);
//	//strcat(temp_buffer_2, temp_buffer_1);
//	
//	ret = net_file_write(flie_handle , strlen(temp_buffer_2), 10, temp_buffer_2);
//	if(0!=ret)
//	{
//		printf("[net] error. file=%s, line=%d\n",__FILE__,__LINE__);
//		net_file_close(flie_handle);
//		return net_wave_write_ret.RET_ERROR;
//	}
//	
//	
//	//ѭ����8��д�����ݣ�һ��32������,600�ֽ�����
//	for(i=0; i<8; i++)
//	{
//		memset(temp_buffer_2,0,sizeof(temp_buffer_2));
//		memset(temp_buffer_1,0,sizeof(temp_buffer_1));
//		
//		for(j=0; j<32; j++)
//		{
//			index = i*32 + j;
//			
//			sprintf(temp_buffer_1, "%.2f,%.2f,%.2f\r\n", \
//				global_mpu_info.vibration_wave_x[index],\
//				global_mpu_info.vibration_wave_y[index],\
//				global_mpu_info.vibration_wave_z[index]);
//			
//			strcat(temp_buffer_2, temp_buffer_1);
//		}
//		
//		ret = net_file_write(flie_handle , strlen(temp_buffer_2), 10, temp_buffer_2);
//		if(0!=ret)
//		{
//			printf("[net] error. file=%s, line=%d\n",__FILE__,__LINE__);
//			net_file_close(flie_handle);
//			return net_wave_write_ret.RET_ERROR;
//		}
//	}

	
	//��������
//	net_file_close(flie_handle);
	return net_wave_write_ret.RET_OK;	
}

