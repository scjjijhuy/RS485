/*
	日志记录相关代码
	作者：lee
	时间：2021.3.8
*/

#include "main.h"

char log_buffer[1024];// __attribute__((section("CCRRAM")))={0};
char temp_buffer_1[1024];// __attribute__((section("CCRRAM")))={0};
char temp_buffer_2[2048];// __attribute__((section("CCRRAM")))={0};

/*
	向日志文件添加一条记录
*/
int net_log_write(const char *buffer_in, ...)
{
	struct net_log_write_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
		const int RET_TIMEROUT;
	}net_log_write_ret = {0,1};
	
	//局部变量
	char file_list[128]={0};
	int file_amount=0;
	int free_size=0;
	int total_size=0;
	int ret=0;
	int flie_handle=0;
	char *p = NULL;
	int log_size=0;
	va_list ap;
	
	//检查输入参数
	if(NULL == buffer_in)
	{
		printf("[net] error. file=%s, line=%d\n",__FILE__,__LINE__);
		net_file_close(flie_handle);
		return net_log_write_ret.RET_ERROR;
	}
	
	//查看是否有文件
	net_file_list("*", file_list, &file_amount);
	printf("[log]当前有%d个文件,列表如下:%s\r\n", file_amount, file_list);
	
	//查看剩余空间大小
	net_file_space("UFS", &free_size, &total_size);
	printf("[log]文件系统剩余大小%d，总大小%d\r\n", free_size, total_size);
	
	//如果日志文件超过剩余文件大小的二分之一则删除
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
	
	//打开日志文件
	ret = net_file_open("run_log.txt" , 0, &flie_handle);
	if(0!=ret)
	{
		printf("[net] error. file=%s, line=%d\n",__FILE__,__LINE__);
		net_file_close(flie_handle);
		return ret;
	}
	
	//设置文件指针
	ret = net_file_seek(flie_handle , 0, 2);
	if(0!=ret)
	{
		printf("[net] error. file=%s, line=%d\n",__FILE__,__LINE__);
		net_file_close(flie_handle);
		return net_log_write_ret.RET_ERROR;
	}
	
	//写入内容
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
	
	//函数返回
	net_file_close(flie_handle);
	return net_log_write_ret.RET_OK;
}

/*
	串口输出运行日志
*/
int net_log_print(void)
{
	struct net_log_print_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_log_print_ret = {0,1};
	
	//局部变量
	char file_list[128]={0};
	int file_amount=0;
	int free_size=0;
	int total_size=0;
	int ret=0;
	int flie_handle=0;
	char *p = NULL;
	int log_size=0;
	
	//查看是否有文件
	net_file_list("*", file_list, &file_amount);
	printf("[log]当前有%d个文件,列表如下:%s\r\n", file_amount, file_list);
	
	//查看剩余空间大小
	net_file_space("UFS", &free_size, &total_size);
	printf("[log]文件系统剩余大小%d，总大小%d\r\n", free_size, total_size);
	
	//如果日志文件超过剩余文件大小的二分之一则删除
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
	
	//打开日志文件
	ret = net_file_open("run_log.txt" , 0, &flie_handle);
	if(0!=ret)
	{
		printf("[net] error. file=%s, line=%d\n",__FILE__,__LINE__);
		net_file_close(flie_handle);
		return net_log_print_ret.RET_ERROR;
	}
	
	//循环读出内容
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
			printf("[net] 读取完毕\r\n");
			break;
		}
	}
	
	//函数返回
	net_file_close(flie_handle);
	return net_log_print_ret.RET_OK;
}

/*
	保存波形数据
*/
int net_wave_write(void)
{
	struct net_wave_write_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_wave_write_ret = {0,1};
	
	//局部变量
//	int ret=0;
//	int flie_handle=0;
//	int i=0;
//	int j=0;
//	int index=0;

	
	//新建日志文件
//	ret = net_file_open("wave_log.txt" , 1, &flie_handle);
//	if(0!=ret)
//	{
//		//失败就再试一次
//		ret = net_file_open("wave_log.txt" , 1, &flie_handle);
//		if(0!=ret)
//		{
//			printf("[net] error. file=%s, line=%d\n",__FILE__,__LINE__);
//			net_file_close(flie_handle);
//			return net_wave_write_ret.RET_ERROR;
//		}
//	}
//	
//	//设置文件指针
//	ret = net_file_seek(flie_handle , 0, 0);
//	if(0!=ret)
//	{
//		printf("[net] error. file=%s, line=%d\n",__FILE__,__LINE__);
//		net_file_close(flie_handle);
//		return net_wave_write_ret.RET_ERROR;
//	}
//	
//	//先写入振动发生的时间
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
//	//sprintf(temp_buffer_1, "振动峰值：%.2f,%.2f,%.2f\r\n", global_mpu_info.vibration_max_x, global_mpu_info.vibration_max_y, global_mpu_info.vibration_max_z);
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
//	//循环分8批写入数据，一次32组数据,600字节左右
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

	
	//函数返回
//	net_file_close(flie_handle);
	return net_wave_write_ret.RET_OK;	
}

