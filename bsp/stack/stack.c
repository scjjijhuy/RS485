/*
	ջ������ش���
	���ߣ�lee
	ʱ�䣺2019/5/9
*/

#include "main.h"

/*
	60�������ֵ����
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
	��������־����
*/
//int vibration_log_update(double vibration_x, double vibration_y, double vibration_z, struct time_struct time, double latitude, double longitude)
//{
//	int LOG_SIZE=60;
//	int update_flag=0;
//	double max_key=0;

//	global_flag.vibration_remote_log_busy_flag = 1;
//	
//	//ɸѡ������������ֵ�е����ֵ
//	max_key = fabs(vibration_x) > fabs(vibration_y) ? fabs(vibration_x) : fabs(vibration_y);
//	max_key = max_key > fabs(vibration_z) ? max_key : fabs(vibration_z);
//	
//	//�������ֵ��60����־�е���Сֵ�������
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
//	����ģʽ����־����
//*/
//uint8_t angle_max_sign = 0;
//int local_vibration_log_update(double vibration_x, double vibration_y, double vibration_z, struct time_struct time, double latitude, double longitude)
//{
//	int LOG_SIZE=60;
//	int update_flag=0;
//	double max_key=0;

//	global_flag.local_vibration_log_busy_flag = 1;
//	
//	//ɸѡ������������ֵ�е����ֵ
//	max_key = fabs(vibration_x) > fabs(vibration_y) ? fabs(vibration_x) : fabs(vibration_y);
//	max_key = max_key > fabs(vibration_z) ? max_key : fabs(vibration_z);
//	
//	//�������ֵ�ȱ���60����־�е���Сֵ�������
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
	�ϴ����ݵ�װ��(�ڲ���)
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
		//��λ��Ч
			global_upload_data.location_mode = 1;
		}
	}
	else
	{
		global_upload_data.location_mode = 0;
		
	}

	//������ֵ
	global_gps_out.max_speed=0;
//	angle_max_sign = 1;
}

/*
	�ж�ջ�Ƿ�Ϊ��
	��Ϊ�շ���1����Ϊ�շ���0
*/
int stack_is_empty(void)
{
	int result=0;
	
	result=(0 == global_upload_stack.rear);
	return result;
}

/*
	�ж�ջ�Ƿ�����
	����������1����������0
*/
int stack_is_full(void)
{
	int result=0;

	result=(STACK_SIZE == global_upload_stack.rear);
	return result;
}

/*
	��ջ����
*/
int stack_push()
{
	//�ֲ���������
	
	//�ж��豸ʱ���Ƿ�����
	if(0==global_device_info.device_time.year)
	{
//		rtc_config();
		printf("\n[stack_push]�豸ʱ���쳣,��ֹ��ջ����\n");
		return -1;
	}
	
	//�ж��ϴ������Ƿ��
	if(0==global_flag.upload_enable_flag)
	{
		printf("\n[stack_push]�ϴ�����û��,��ֹ��ջ����\n");
		return -1;
	}
	
	//�ж�ջ�Ƿ�����,װ������
	if (!stack_is_full())
	{
		//������������ֱ�ӷ���
		
		//��װ�ش���������ݽṹ��
		load_upload_data();
		
		//�������ݷ���ջ��
		global_upload_pool[global_upload_stack.rear] = global_upload_data;
		
		//ջβָ���һ
		global_upload_stack.rear = global_upload_stack.rear + 1;
		
		//ջ����һ
		global_upload_stack.length += 1;
	}
	else
	{
		//���������9���������ջ
		printf("[stack]ջ�Ѿ�����");
		

	}	
	return 0;
}

/*
	��ջ��ɾ��ջβ����
*/
int stack_pop(void)
{
	//�ж�ջ�Ƿ�Ϊ��
	if(!stack_is_empty())
	{
		//��ջ��Ϊ��
		
		//��ջβ���ݳ�ջ
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
	��ȡջβ��ֵ����ɾ��
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
	���ջ
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
	��ȡջ��ǰ����
*/
int stack_len(void)
{
	return global_upload_stack.length;
}

/*
	���ջ�Ƿ��쳣�����쳣�����
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



