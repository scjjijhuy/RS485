/*
	栈操作相关代码
	作者：lee
	时间：2019/5/9
*/

#include "main.h"

/*
	60组振动最大值排序
*/
int compare_vibration(const void* value1, const void* value2)
{
	double result = (*(struct log_struct_vibration *)value2).max_key - (*(struct log_struct_vibration *)value1).max_key;
	if(0 < result)
	{
		return 1;
	}
	else if(0 > result)
	{
		return -1;
	}
	else 
	{
		return 0;
	}
}

/*
	网络振动日志更新
*/
//int vibration_log_update(double vibration_x, double vibration_y, double vibration_z, struct time_struct time, double latitude, double longitude)
//{
//	int LOG_SIZE=60;
//	int update_flag=0;
//	double max_key=0;

//	global_flag.vibration_remote_log_busy_flag = 1;
//	
//	//筛选出本次三轴振动值中的最大值
//	max_key = fabs(vibration_x) > fabs(vibration_y) ? fabs(vibration_x) : fabs(vibration_y);
//	max_key = max_key > fabs(vibration_z) ? max_key : fabs(vibration_z);
//	
//	//若此最大值比60组日志中的最小值大，则插入
//	if(max_key >= global_log_vibration.global_log_vibration[LOG_SIZE-1].max_key)
//	{
//		global_log_vibration.global_log_vibration[LOG_SIZE-1].max_key=max_key;
//		global_log_vibration.global_log_vibration[LOG_SIZE-1].data_x=vibration_x;
//		global_log_vibration.global_log_vibration[LOG_SIZE-1].data_y=vibration_y;
//		global_log_vibration.global_log_vibration[LOG_SIZE-1].data_z=vibration_z;
//		global_log_vibration.global_log_vibration[LOG_SIZE-1].time=time;
//		global_log_vibration.global_log_vibration[LOG_SIZE-1].latitude=latitude;
//		global_log_vibration.global_log_vibration[LOG_SIZE-1].longitude=longitude;
//		
//		update_flag=1;
//	}
//	qsort(global_log_vibration.global_log_vibration, LOG_SIZE, sizeof(global_log_vibration.global_log_vibration[0]), compare_vibration);
//	
//	global_flag.vibration_remote_log_busy_flag = 0;
//	
//	return update_flag;
//}

///*
//	本地模式振动日志更新
//*/
//uint8_t angle_max_sign = 0;
//int local_vibration_log_update(double vibration_x, double vibration_y, double vibration_z, struct time_struct time, double latitude, double longitude)
//{
//	int LOG_SIZE=60;
//	int update_flag=0;
//	double max_key=0;

//	global_flag.local_vibration_log_busy_flag = 1;
//	
//	//筛选出本次三轴振动值中的最大值
//	max_key = fabs(vibration_x) > fabs(vibration_y) ? fabs(vibration_x) : fabs(vibration_y);
//	max_key = max_key > fabs(vibration_z) ? max_key : fabs(vibration_z);
//	
//	//若此最大值比本地60组日志中的最小值大，则插入
//	if(max_key >= global_local_log_vibration.global_local_log_vibration[LOG_SIZE-1].max_key)
//	{
//		global_local_log_vibration.global_local_log_vibration[LOG_SIZE-1].max_key=max_key;
//		global_local_log_vibration.global_local_log_vibration[LOG_SIZE-1].data_x=vibration_x;
//		global_local_log_vibration.global_local_log_vibration[LOG_SIZE-1].data_y=vibration_y;
//		global_local_log_vibration.global_local_log_vibration[LOG_SIZE-1].data_z=vibration_z;
//		global_local_log_vibration.global_local_log_vibration[LOG_SIZE-1].time=time;
//		global_local_log_vibration.global_local_log_vibration[LOG_SIZE-1].latitude=latitude;
//		global_local_log_vibration.global_local_log_vibration[LOG_SIZE-1].longitude=longitude;
//		
//		update_flag=1;
//	}
//	qsort(global_local_log_vibration.global_local_log_vibration, LOG_SIZE, sizeof(global_local_log_vibration.global_local_log_vibration[0]), compare_vibration);
//	
//	global_flag.local_vibration_log_busy_flag = 0;
//	
//	return update_flag;
//}

/*
	上传数据的装载(内部用)
*/

void load_upload_data(void)
{
	
	global_upload_data.time = global_device_info.device_time;
	global_upload_data.ms = (global_timer.system_timer/10)%100;
	global_upload_data.latitude = global_gps_out.latitude;
	global_upload_data.NS[0] = global_gps_out.NS[0];
	global_upload_data.NS[1] = '\0';
	global_upload_data.longitude = global_gps_out.longitude;
	global_upload_data.EW[0] = global_gps_out.EW[0];
	global_upload_data.EW[1] = '\0';
	global_upload_data.dump_energy = global_device_info.dump_energy;
	global_upload_data.speed = global_gps_out.max_speed;
	global_upload_data.signal = global_device_info.signal_quality;            //2022
	
	if(0==global_gps_out.valid)
	{
		if((gps_raw.s_enable_state[0] != '?') && (gps_raw.s_latitude[0] != '?')  &&
		   (gps_raw.s_NS[0] != '?')           && (gps_raw.s_longitude[0] != '?') &&
		   (gps_raw.s_EW[0] != '?')           && (gps_raw.s_speed[0] != '?'))
		{
			global_upload_data.location_mode = 0;
		}
		else
		{
		//定位无效
			global_upload_data.location_mode = 1;
		}
	}
	else
	{
		global_upload_data.location_mode = 0;
		
	}

	//清空最大值
	global_gps_out.max_speed=0;
//	angle_max_sign = 1;
}

/*
	判断栈是否为空
	若为空返回1，不为空返回0
*/
int stack_is_empty(void)
{
	int result=0;
	
	result=(0 == global_upload_stack.rear);
	return result;
}

/*
	判断栈是否已满
	若已满返回1，不满返回0
*/
int stack_is_full(void)
{
	int result=0;

	result=(STACK_SIZE == global_upload_stack.rear);
	return result;
}

/*
	入栈操作
*/
int stack_push()
{
	//局部变量定义
	
	//判断设备时间是否正常
	if(0==global_device_info.device_time.year)
	{
//		rtc_config();
		printf("\n[stack_push]设备时间异常,终止入栈操作\n");
		return -1;
	}
	
	//判断上传开关是否打开
	if(0==global_flag.upload_enable_flag)
	{
		printf("\n[stack_push]上传开关没打开,终止入栈操作\n");
		return -1;
	}
	
	//判断栈是否已满,装载数据
	if (!stack_is_full())
	{
		//若不满则将数据直接放入
		
		//先装载待放入的数据结构体
		load_upload_data();
		
		//将新数据放入栈中
		global_upload_pool[global_upload_stack.rear] = global_upload_data;
		
		//栈尾指针加一
		global_upload_stack.rear = global_upload_stack.rear + 1;
		
		//栈长加一
		global_upload_stack.length += 1;
	}
	else
	{
		//添加类似于9连环程序堆栈
		printf("[stack]栈已经存满");
		

	}	
	return 0;
}

/*
	出栈并删除栈尾数据
*/
int stack_pop(void)
{
	//判断栈是否为空
	if(!stack_is_empty())
	{
		//若栈不为空
		
		//将栈尾数据出栈
		global_upload_data = global_upload_pool[global_upload_stack.rear - 1];
		memset(&global_upload_pool[global_upload_stack.rear - 1], 0, sizeof(struct upload_data_struct));
		global_upload_stack.rear = global_upload_stack.rear - 1;
		global_upload_stack.length -=1 ;
		
		return 0;
	}
	else
	{
		//stack empty
	}
	return -1;
}

/*
	获取栈尾的值，不删除
*/
int stack_back(void)
{

	if(!stack_is_empty())
	{

		global_upload_data = global_upload_pool[global_upload_stack.rear-1];
		return 0;
	}
	else
	{
		//stack empty
	}
	return -1;
}

/*
	清空栈
*/
void stack_clear(void)
{
	global_upload_stack.size = STACK_SIZE;
	global_upload_stack.front = 0;
	global_upload_stack.rear = 0;
	global_upload_stack.length = 0;
	global_upload_stack.sign = 0;
}

/*
	获取栈当前长度
*/
int stack_len(void)
{
	return global_upload_stack.length;
}

/*
	检查栈是否异常，若异常则清空
*/
int stack_check(void)
{

	if( global_upload_stack.size<global_upload_stack.front || 
			global_upload_stack.size<global_upload_stack.rear || 
			1>global_upload_stack.size || 
			global_upload_stack.size<global_upload_stack.length )
	{
		stack_clear();
		return 1;
	}
	return 0;
}



