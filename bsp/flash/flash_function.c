/*
	flash功能函数源文件
	作者:lee
	时间：2019/5/9
*/
#include "main.h"

static unsigned char temp_buffer[0x2000]={0};//一page大小

/*
	将任意大小的数据保存到flash以页起始的地址中（只适用于STM32L476）
*/
int flash_write(int page_number, unsigned char *src, int length)
{
	//返回值定义
	struct flash_save_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}flash_save_ret={0,1};
	
	//局部变量定义
	int page_amount=0;
	int i=0;
	int last_length=length;
	int copy_size=0;
	uint32_t write_address=0;
	
	//检查输入参数是否合法
	if(NULL == src)
	{
		return flash_save_ret.RET_ERROR;
	}
	if(0>page_number || 63<page_number)
	{
		return flash_save_ret.RET_ERROR;
	}
	
	//计算需要几页才能存完
	page_amount = (length/0x2000) + 1;

	if(64<page_amount+page_number)
	{
		return flash_save_ret.RET_ERROR;
	}
	
	
	Flash_If_Init();
	
	//循环保存
	for(i=0; i<page_amount; i++)
	{
		//将其中一页装入缓存
		memset(temp_buffer, 0, 0x2000);
		copy_size=(last_length>0x2000)?0x2000:last_length;
		memcpy(temp_buffer, src+i*0x2000, copy_size);
		
		//计算写入地址
		write_address = FLASH_BASE + page_number*0x2000 + i*0x2000;
		
		//擦除该页
		Flash_If_Erase(write_address);
		
		//写入flash
		Flash_If_Write(temp_buffer, write_address, 0x2000);
		
		//更新剩余字节数
		last_length -= 0x2000;//最后一次循环，last_length减到负值也无所谓
		
		//延时
		//osDelay(10);

	}
	Flash_If_DeInit();
	
	//函数返回
	return flash_save_ret.RET_OK;
}

/*
	读取flash中的数据
*/
int flash_read(int page_number, unsigned char *buffer, int length)
{
	//返回值定义
	struct flash_read_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}flash_read_ret={0,1};
	
	//局部变量定义
	uint32_t dest_addr=0;
	
	//检查输入参数是否合法
	if(NULL == buffer)
	{
		return flash_read_ret.RET_ERROR;
	}
	if(0>page_number || 63<page_number)
	{
		return flash_read_ret.RET_ERROR;
	}
	if(64<(length/0x2000) + 1 +page_number)
	{
		return flash_read_ret.RET_ERROR;
	}
	
	//计算起始地址
	dest_addr = FLASH_BASE + page_number*0x2000;
	
	//读取到buffer
	Flash_If_Read(buffer, dest_addr, length);
	
	//函数返回
	return flash_read_ret.RET_OK;
}
int flash_erase(int page_number,uint8_t num)
{
	//局部变量定义
	int page_amount=0;
	uint32_t write_address=0;
	int i=0;
	if(0>page_number || 63<page_number)
	{
		return 1;
	}
	
	//计算需要几页才能存完
	page_amount = num;

	if(64<page_amount+page_number)
	{
		return 1;
	}
	  Flash_If_Init();
	  for(i=0; i<page_amount; i++)
	 {
		    write_address = FLASH_BASE + page_number*0x2000 + i*0x2000;
		    Flash_If_Erase(write_address);
	 }
	  
	  Flash_If_DeInit();
	 return 0;
}

void flash_info_import(void)
{
	memset(&flash_save_device_info,0,sizeof(struct flash_save_device_info_struct));
	
	memcpy(&flash_save_device_info.flash_upload_stack,&global_upload_stack,sizeof(struct stack_struct));	
	memcpy(&flash_save_device_info.flash_task_info,&global_task_info,sizeof(struct task_info_struct));	
	memcpy(&flash_save_device_info.flash_flag,&global_flag,sizeof(struct flag_struct));	
	memcpy(&flash_save_device_info.flash_threshold,&global_threshold,sizeof(struct threshold_struct));	
	memcpy(&flash_save_device_info.flash_device_config,&global_device_config,sizeof(struct device_config_struct));	
	memcpy(&flash_save_device_info.flash_device_info,&global_device_info,sizeof(struct device_info_struct));
}
void flash_info_export(void)
{
	memset(&global_upload_stack,0,sizeof(struct stack_struct));
	memcpy(&global_upload_stack,&flash_save_device_info.flash_upload_stack,sizeof(struct stack_struct));	
	
	memset(&global_task_info,0,sizeof(struct task_info_struct));
	memcpy(&global_task_info,&flash_save_device_info.flash_task_info,sizeof(struct task_info_struct));	
	
	
	
	memset(&global_threshold,0,sizeof(struct threshold_struct));
	memcpy(&global_threshold,&flash_save_device_info.flash_threshold,sizeof(struct threshold_struct));	
	
	memset(&global_device_config,0,sizeof(struct device_config_struct));
	memcpy(&global_device_config,&flash_save_device_info.flash_device_config,sizeof(struct device_config_struct));
	
//	memset(&global_device_info,0,sizeof(struct device_info_struct));
//	memcpy(&global_device_info,&flash_save_device_info.flash_device_info,sizeof(struct device_info_struct));
	
}
/*
	保存全局数据
*/
void flash_save_global_variables(void)
{
	int ret=0;
	
	//电量为0时不可以写flash
	if(0 == global_device_info.dump_energy)
	{
		return;
	}
	
	//防止函数重入
	if(0!=global_flag.flash_busy_flag)
	{
		printf("[flash]保存不可重入\n");
		return;
	}
	
	global_flag.flash_busy_flag=1;
	
	printf("\n[flash]保存flash\n");
	
	osDelay(10);
	
	
	//断点续传栈
	printf("[flash]保存上传栈，起始页%d，共%d页\n", UPLOAD_POOL_PAGE, sizeof(struct upload_data_struct)*STACK_SIZE/0x2000+1);
	ret = flash_write(UPLOAD_POOL_PAGE, (uint8_t*)&global_upload_pool[0], sizeof(struct upload_data_struct)*STACK_SIZE);
	if(0!=ret)
	{
		printf("\n[flash]flash_write UPLOAD_POOL_PAGE 错误码%d\n",ret);
	}
	//写数据信息
	flash_info_import();
	printf("[flash]保存上传栈表，起始页%d，共%d页\n", DEVICE_INFO_PAGE, sizeof(struct flash_save_device_info_struct)/0x2000+1);
	ret = flash_write(DEVICE_INFO_PAGE, (uint8_t*)&flash_save_device_info, sizeof(struct flash_save_device_info_struct));
	if(0!=ret)
	{
		printf("\n[flash]flash_write DEVICE_INFO_PAGE 错误码%d\n",ret);
	}
		
	osDelay(10);
	global_flag.flash_busy_flag=0;
}

/*
	读取全局数据
*/
void flash_load_global_variables(void)
{
	int ret=0;
	int i=0;
	int task_right=0;
//	static struct flag_struct temp_flag={0};
//	static struct device_info_struct temp_device_info={0};	
	global_flag.flash_busy_flag=1;
	printf("\n[flash]读取flash\n");
	
	
    //断点续传栈
	ret = flash_read(UPLOAD_POOL_PAGE, (uint8_t*)&global_upload_pool[0], sizeof(struct upload_data_struct)*STACK_SIZE);
	if(0!=ret)
	{
		printf("\n[flash]flash_read UPLOAD_POOL_PAGE 错误码%d\n",ret);
	}
	//读取FLASH存储的设备信息
	ret = flash_read(DEVICE_INFO_PAGE, (uint8_t*)&flash_save_device_info, sizeof(struct flash_save_device_info_struct));
	if(0!=ret)
	{
		printf("\n[flash]flash_read DEVICE_INFO_PAGE 错误码%d\n",ret);
	}
	flash_info_export();
	/*
		读取上传栈
	*/
	stack_check();

	/*
		读取任务信息
	*/
	//printf("\n[flash]读取任务信息 TASK_INFO_PAGE\n");
	for(i=1;i<64;i++)
	{
		if('\0'==global_task_info.task_num[i] && 0!=global_task_info.task_num[0])
		{
			task_right=1;
			break;
		}
	}
	if(0xff==(uint8_t)global_task_info.task_num[0])
	{
		task_right=0;
	}
	if(1!=task_right)
	{
		memset((uint8_t*)&global_task_info, 0, sizeof(struct task_info_struct));
	}
	
	/*
		读取FLAG变量
	*/
//    memset(&global_flag,0,sizeof(struct flag_struct));
//	memcpy(&global_flag,&flash_save_device_info.flash_flag,sizeof(struct flag_struct));	
	printf("\r\n[debug!!!!!!!!!!!!!] temp_flag.system_restart_flag = %d\r\n", global_flag.system_restart_flag);
	if(0>flash_save_device_info.flash_flag.system_restart_flag || 1<flash_save_device_info.flash_flag.system_restart_flag)
	{
//			global_flag.system_restart_flag = 0;
	}
	else
	{
		global_flag.system_restart_flag = flash_save_device_info.flash_flag.system_restart_flag;
	}
	
	if(0>flash_save_device_info.flash_flag.button_start_up_flag || 1<flash_save_device_info.flash_flag.button_start_up_flag)
	{
		global_flag.button_start_up_flag = 1;
	}
	else
	{
		global_flag.button_start_up_flag = flash_save_device_info.flash_flag.button_start_up_flag;
	}
	
	if(0>flash_save_device_info.flash_flag.already_print_flag || 1<flash_save_device_info.flash_flag.already_print_flag)
	{
		global_flag.already_print_flag = 0;
	}
	else
	{
		global_flag.already_print_flag = flash_save_device_info.flash_flag.already_print_flag;
	}
	
	
	/*
		读取阈值信息
	*/
	if(100<global_threshold.dump_energy || 0>global_threshold.dump_energy )
	{
		global_threshold.dump_energy = 10;
	}

	if(200<global_threshold.speed || 0>global_threshold.speed)
	{
		global_threshold.speed = 80;
	}
	
	/*
		读取设备配置信息
	*/
	if(1>global_device_config.awake_time)
	{
		global_device_config.awake_time = 300;
	}
	if(0>global_device_config.sleep_time)
	{
		global_device_config.sleep_time = 1800;
	}
	if(0>global_device_config.interval)
	{
		global_device_config.interval = 30;    //20220311
	}
	if(0>global_device_config.gnss_close_time)
	{
		global_device_config.gnss_close_time = 30;
	}
	if(0>global_device_config.gnss_open_time)
	{
		global_device_config.gnss_open_time = 3600;
	}
	if(0>global_device_config.gnss_mode || 2<global_device_config.gnss_mode)
	{
		global_device_config.gnss_mode=1;
	}
	
	global_device_config.gnss_once_flag=0;
	global_device_config.gnss_once_time=0;
	
	if(0>global_device_config.beep_mode || 1<global_device_config.beep_mode)
	{
		global_device_config.beep_mode=0;
	}
	
	if(1!=isnormal(global_device_config.vibration_coefficient))
	{
		global_device_config.vibration_coefficient=1;
	}
	
	if(1!=isnormal(global_device_config.vibration_1_x_a))
	{
		global_device_config.vibration_1_x_a=1;
	}
	if(1!=isnormal(global_device_config.vibration_1_x_b))
	{
		global_device_config.vibration_1_x_b=0;
	}
	if(1!=isnormal(global_device_config.vibration_1_y_a))
	{
		global_device_config.vibration_1_y_a=1;
	}
	if(1!=isnormal(global_device_config.vibration_1_y_b))
	{
		global_device_config.vibration_1_y_b=0;
	}
	if(1!=isnormal(global_device_config.vibration_1_z_a))
	{
		global_device_config.vibration_1_z_a=1;
	}
	if(1!=isnormal(global_device_config.vibration_1_z_b))
	{
		global_device_config.vibration_1_z_b=0;
	}
	
	if(1!=isnormal(global_device_config.vibration_2_x_a))
	{
		global_device_config.vibration_2_x_a=1;
	}
	if(1!=isnormal(global_device_config.vibration_2_x_b))
	{
		global_device_config.vibration_2_x_b=0;
	}
	if(1!=isnormal(global_device_config.vibration_2_y_a))
	{
		global_device_config.vibration_2_y_a=1;
	}
	if(1!=isnormal(global_device_config.vibration_2_y_b))
	{
		global_device_config.vibration_2_y_b=0;
	}
	if(1!=isnormal(global_device_config.vibration_2_z_a))
	{
		global_device_config.vibration_2_z_a=1;
	}
	if(1!=isnormal(global_device_config.vibration_2_z_b))
	{
		global_device_config.vibration_2_z_b=0;
	}
	
	if(0>global_device_config.rf433_mode || 1<global_device_config.rf433_mode)
	{
		global_device_config.rf433_mode=1;
	}
	
	if(0>global_device_config.sleep_network || 1<global_device_config.sleep_network)
	{
		global_device_config.sleep_network=0;
	}
	
	if(0>global_device_config.net_power_interval || 3600<global_device_config.net_power_interval)
	{
		global_device_config.net_power_interval=600;
	}
	
	/*
		读取设备信息
	*/
	if(0xff != (unsigned char)flash_save_device_info.flash_device_info.terminal_sn[0])
	{
		strcpy(global_device_info.terminal_sn, flash_save_device_info.flash_device_info.terminal_sn);
	}
	
	if(0xff != (unsigned char)flash_save_device_info.flash_device_info.start_up_time.year && \
			0!=flash_save_device_info.flash_device_info.start_up_time.year)
	{
		global_device_info.start_up_time = flash_save_device_info.flash_device_info.start_up_time;
	}
	
	if(0xff != (unsigned char)flash_save_device_info.flash_device_info.iccid[0])
	{
		strcpy(global_device_info.iccid, flash_save_device_info.flash_device_info.iccid);
	}
	global_flag.flash_busy_flag=0;
}

/*
	清空全局数据
*/
void flash_earse_global_variables(void)
{

	memset((uint8_t*)&global_upload_pool[0], 0, sizeof(struct upload_data_struct)*STACK_SIZE);
	
	flash_write(UPLOAD_POOL_PAGE, (uint8_t*)&global_upload_pool[0], sizeof(struct upload_data_struct)*STACK_SIZE);
		

	/*
		清空上传栈
	*/
	memset((uint8_t*)&global_upload_stack, 0, sizeof(struct stack_struct));
	global_upload_stack.size = STACK_SIZE;	
	/*
		清空任务信息
	*/
	memset((uint8_t*)&global_task_info, 0, sizeof(struct task_info_struct));
	strcpy(global_task_info.task_num, "no task_num");	
	/*
		清空FLAG变量
	*/
	memset((uint8_t*)&global_flag, 0, sizeof(struct flag_struct));
	global_flag.upload_enable_flag=1;
	global_flag.upload_data_flag=1;
	global_flag.filter_flag=1;	
	/*
		清空阈值信息
	*/
	memset((uint8_t*)&global_threshold, 0, sizeof(struct threshold_struct));
	global_threshold.dump_energy=10;

	global_threshold.speed=80;	
	/*
		清空设备配置信息
	*/
	memset((uint8_t*)&global_device_config, 0, sizeof(struct device_config_struct));
	global_device_config.sleep_time=1800;
	global_device_config.awake_time=300;
	global_device_config.interval=30;          //202203011
	global_device_config.gnss_open_time=3600;
	global_device_config.gnss_close_time=30;
	global_device_config.gnss_mode = 1;
	global_device_config.net_power_interval = 600;
	global_device_config.beep_mode=0;
	global_device_config.gnss_once_flag=0;
	global_device_config.gnss_once_time=0;
	global_device_config.rf433_mode=1;
	global_device_config.sleep_network=0;
	
	global_device_config.vibration_coefficient=1;
	
	global_device_config.vibration_1_x_a=1;
	global_device_config.vibration_1_x_b=0;
	global_device_config.vibration_1_y_a=1;
	global_device_config.vibration_1_y_b=0;
	global_device_config.vibration_1_z_a=1;
	global_device_config.vibration_1_z_b=0;
	
	global_device_config.vibration_2_x_a=1;
	global_device_config.vibration_2_x_b=0;
	global_device_config.vibration_2_y_a=1;
	global_device_config.vibration_2_y_b=0;
	global_device_config.vibration_2_z_a=1;
	global_device_config.vibration_2_z_b=0;
	
	/*
		清空设备信息
	*/
	memset((uint8_t*)&global_device_info, 0, sizeof(struct device_info_struct));
	global_device_info.dump_energy=100;

	flash_info_import();
	flash_write(DEVICE_INFO_PAGE, (uint8_t*)&flash_save_device_info, sizeof(struct flash_save_device_info_struct));

}

