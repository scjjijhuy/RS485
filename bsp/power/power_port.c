/*
	电源管理模块底层代码
	作者：lee
	时间：2019/5/15
*/
#include "main.h"

//extern void SystemClock_Config(void);
//extern void SystemClock_Config_internal(void);
//extern uint32_t sysclock;
//extern osThreadId tid_Thread_led_beep; 
////extern osThreadId tid_Thread_mpu;
////extern osThreadId tid_Thread_aht10;
//extern osThreadId tid_Thread_rtc;
//extern osThreadId tid_Thread_bluetooth;
//extern osThreadId tid_Thread_gps; 
//extern osThreadId tid_Thread_net;
////extern osThreadId tid_Thread_ch376;  
//extern osTimerId id2;

//extern RTC_HandleTypeDef hrtc;
//extern ADC_HandleTypeDef hadc1;
//extern UART_HandleTypeDef huart4;

//static volatile uint8_t  AD_done;       /* AD conversion done flag  */
//int32_t ADC_StartConversion (void);
//int32_t ADC_ConversionDone (void);
//int32_t ADC_GetValue (void);

//#define BATTERY_ARRAY_LENGTH 10
//int battery_array[BATTERY_ARRAY_LENGTH]={0};



///*
//	读取ADC原始值
//	@ref ADC_HAL_EC_CHANNEL
//*/
//int power_adc_data_get(uint32_t channel_in, uint32_t *data_out)
//{
//	struct power_adc_data_get_ret_struct
//	{
//		const int RET_OK;
//		const int RET_ERROR;
//	}power_adc_data_get_ret = {0,1};
//	
//	//局部变量
//	int ret=0;
//	ADC_ChannelConfTypeDef sConfig = {0};
//	
//	//初始化MSP
//	HAL_ADC_MspInit(&hadc1);
//	
//	//配置通道
//	sConfig.Channel = channel_in;
//  sConfig.Rank = ADC_REGULAR_RANK_1;
//  sConfig.SamplingTime = ADC_SAMPLETIME_640CYCLES_5;
//  sConfig.SingleDiff = ADC_SINGLE_ENDED;
//  sConfig.OffsetNumber = ADC_OFFSET_NONE;
//  sConfig.Offset = 0;
//	ret = HAL_ADC_ConfigChannel(&hadc1, &sConfig);
//  if (ret != HAL_OK)
//  {
//    printf("[power]error ret=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
//		goto RET_ERROR;
//  }
//	
//	//校准
//	ret = HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
//	if (ret != HAL_OK)
//  {
//    printf("[power]error ret=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
//		goto RET_ERROR;
//  }
//	
//	//开始转换
//	ret = HAL_ADC_Start(&hadc1);
//	if (ret != HAL_OK)
//  {
//    printf("[power]error ret=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
//		goto RET_ERROR;
//  }
//	
//	//等待转换完毕
//	ret = HAL_ADC_PollForConversion(&hadc1, 1000);
//	if (ret != HAL_OK)
//  {
//    printf("[power]error ret=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
//		goto RET_ERROR;
//  }
//	
//	//获取值
//	*data_out = HAL_ADC_GetValue(&hadc1);
//	
//	//停止转换
//	ret = HAL_ADC_Stop(&hadc1);
//	if (ret != HAL_OK)
//  {
//    printf("[power]error ret=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
//		goto RET_ERROR;
//  }
//	
//	goto RET_OK;
//	
//	RET_OK:
//	HAL_ADC_MspDeInit(&hadc1);
//	return power_adc_data_get_ret.RET_OK;
//	
//	RET_ERROR:
//	HAL_ADC_MspDeInit(&hadc1);
//	return power_adc_data_get_ret.RET_ERROR;
//}

///*
//	计算电压值
//	@ref ADC_HAL_EC_CHANNEL
//*/
//int power_adc_voltage_get(uint32_t channel_in, double *voltage_out)
//{
//	struct power_adc_voltage_get_ret_struct
//	{
//		const int RET_OK;
//		const int RET_ERROR;
//	}power_adc_voltage_get_ret = {0,1};

//	//局部变量
//	int ret=0;
//	uint32_t vrefint_data = 0;
//	uint32_t adc_data_x = 0;
//	uint16_t vrefint_cal = 0;
//	int full_scale = 4095;
//	double voltage=0;
//	
//	//读取IN17内部参考电压原始数据
//	ret = power_adc_data_get(ADC_CHANNEL_VREFINT, &vrefint_data);
//	if (0 != ret)
//  {
//    printf("[power]error ret=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
//		goto RET_ERROR;
//  }
//	
//	//读取电池电量原始通道连接数据
//	ret = power_adc_data_get(channel_in, &adc_data_x);
//	if (0 != ret)
//  {
//    printf("[power]error ret=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
//		goto RET_ERROR;
//  }
//	
//	//读取VREFINT_CAL内部参考电压校准值
//	vrefint_cal = *(__IO uint16_t*)(VREFINT_CAL_ADDR);
//	
//	//确定FULL_SCALE
//	full_scale = 4095;
//	
//	//计算电压值
//	voltage = (3.0*(double)vrefint_cal*(double)adc_data_x) / ((double)vrefint_data*(double)full_scale);
//	
//	//赋值
//	*voltage_out = voltage;
//	
//	//返回
//	goto RET_OK;
//	
//	RET_OK:
//	return power_adc_voltage_get_ret.RET_OK;
//	
//	RET_ERROR:
//	return power_adc_voltage_get_ret.RET_ERROR;
//}
double ivoltage=0;
int power_battery_percent_once(int *percent_out, double *voltage_out)
{
	int ret=0;
	AdcPolling();
	return ret;
}

/*
	读取电池电量百分比
*/
int power_battery_percent(int *percent_out, double *voltage_out)
{
	struct power_battery_percent_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}power_battery_percent_ret={0,1};
	
	#define VOL_ARRAY_SIZE (20)
	#define PER_ARRAY_SIZE (10)
	
	//静态变量
	static double voltage_array[VOL_ARRAY_SIZE]={0};
	static int init_flag=0xFF;
	static int percent_array[PER_ARRAY_SIZE]={0};
	static int last_percent=0;
	
	//局部变量
	int ret=0;
	double voltage=0;
	double percent=0;
	int i=0;
	double max=-999;
	double min=999;
	double sum=0;
	int pass=0;
	
	//测量电池通道电压
	AdcPolling();
	voltage = global_device_info.battery_voltage;
	if (0 != ret)
  {
    printf("[power]error ret=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
		return power_battery_percent_ret.RET_ERROR;
  }
	
	//电池电压
//	voltage *= 1.5f;
	if(init_flag == 0xFF)
	{
		init_flag = 0;
		return power_battery_percent_ret.RET_ERROR;
	}
	//是否需要初始化滤波队列
	if(0==init_flag)
	{
		for(i=0;i<VOL_ARRAY_SIZE;i++)
		{
			voltage_array[i] = voltage;
		}
	}
	
	//新值添加到队尾
	for(i=0;i<VOL_ARRAY_SIZE-1;i++)
	{
		voltage_array[i] = voltage_array[i+1];
	}
	voltage_array[VOL_ARRAY_SIZE-1] = voltage;

	//计算
	for(i=0;i<VOL_ARRAY_SIZE;i++)
	{
		sum += voltage_array[i];
		
		if(min>voltage_array[i])
		{
			min = voltage_array[i];
		}
		if(max<voltage_array[i])
		{
			max = voltage_array[i];
		}
	}
	
	sum = sum - min - max;
	
	voltage = sum/(VOL_ARRAY_SIZE-2);
	
	*voltage_out = voltage;
	voltage *= 1000;
	//电压百分比
	percent = ((voltage - BATTERY_LOW_V)/(BATTERY_HIGH_V - BATTERY_LOW_V))*100.0;
	
	if(1 > percent)
	{
		percent = 1;
	}
	if(100 < percent)
	{
		percent = 100;
	}
	
	//初始化
	if(0==init_flag)
	{
		for(i=0;i<PER_ARRAY_SIZE;i++)
		{
			percent_array[i] = (int)percent;
		}
		last_percent = (int)percent;
	}
	
	//新值添加到队尾
	for(i=0;i<PER_ARRAY_SIZE-1;i++)
	{
		percent_array[i] = percent_array[i+1];
	}
	percent_array[PER_ARRAY_SIZE-1] = (int)percent;
	
	//赋值
	for(i=0;i<PER_ARRAY_SIZE-1;i++)
	{
		if(percent_array[i]!=percent_array[i+1])
		{
			pass=1;
			break;
		}
	}
	
	if(1==pass)
	{
		*percent_out = last_percent;
	}
	else
	{
		*percent_out = percent_array[0];
	}
	
	last_percent = *percent_out;
	
	//初始化标志
	if(0==init_flag)
	{
		init_flag=1;
	}
	
	//返回
	return power_battery_percent_ret.RET_OK;
}


///*
//	检查设备是否需要进入休眠
//*/
uint32_t power_sleep_times = 0;
int power_sleep_check(void)
{
	//返回值定义
	struct power_sleep_check_ret_struct
	{
		const int RET_OK;
		const int RET_INVALID;
	}power_sleep_check_ret={0,1};
	
	//局部变量
	uint32_t iLoop = 0;
	//判断设备振动开关是否到达间隔，并且有没有等待开机后第一条数据上传
	if(global_device_config.awake_time > global_timer.sleep_timer_sec || 1 == global_flag.wait_first_upload_flag)
	{
		return  power_sleep_check_ret.RET_INVALID;
	}
	if((global_flag.start_up_sign != 0) && (global_timer.system_timer_sec <3600))
	{
		 return power_sleep_check_ret.RET_INVALID;
	}
	if(global_flag.sleep_flag == 0)
	{
		global_flag.sleep_flag = 1;
		power_sleep_times = global_timer.system_timer;
//		net_registration_device("sleep_down");
		for(iLoop = 0;iLoop < 300;iLoop++)
		{
			if(Feed_dog == 0)
			{
				feed_dog_func(0);
			}
			else
			{
				osDelay(10);
			}
		}
//		osDelay(3000);
//		restart_usart3_recv(); 
	}
	
	if((global_timer.system_timer - power_sleep_times) > global_device_config.sleep_time*1000)
	{
		global_timer.sleep_timer_sec = 0;
		global_flag.sleep_flag = 0;
//		net_registration_device("wake_up");
		beep(1);
		osDelay(50);
		beep(0);
		for(iLoop = 0;iLoop < 300;iLoop++)
		{
			if(Feed_dog == 0)
			{
				feed_dog_func(0);
			}
			else
			{
				osDelay(10);
			}
		}
//		osDelay(3000);
	}
	return power_sleep_check_ret.RET_OK;
}
//	//EC20退出休眠
////	net_power_sleep(0);
//	net_log_write("设备休眠\r\n");
//	//休眠时间到，准备进入休眠
//	printf("\n[power]休眠时间到，准备进入休眠\n");
//	global_flag.sleep_flag = 1;
//	
//	osDelay(5000);
//	
//	//杀线程
//	osThreadTerminate (tid_Thread_led_beep);
////	osThreadTerminate (tid_Thread_mpu);
////	osThreadTerminate (tid_Thread_aht10);
//	osThreadTerminate (tid_Thread_rtc);
////	osThreadTerminate (tid_Thread_bluetooth);
//	osThreadTerminate (tid_Thread_gps);
//	osTimerDelete (id2);
//	

////*********************************************************************************
////刘华科 20220104  喂狗
////*********************************************************************************		
//	M41T65_write_data(0x2A,9);
//  
////*********************************************************************************
////刘华科 20220104  喂狗
////*********************************************************************************	
//  //保存flash
//	global_flag.system_restart_flag=1;	
//	flash_save_global_variables();
//	
//	//发送休眠注册信息
//	net_registration_device("sleep_down");
//	
//	//EC20进入休眠
//	net_power_sleep(1);
//	
//	//进入休眠
//	printf("\n[power]准备完毕进入休眠,休眠时间%d秒\n",global_device_config.sleep_time);
//	
//	led_run_control(LED_OFF);
//	led_gps_control(LED_OFF);
//	led_gprs_control(LED_OFF);
//	
////	pressure_power(0);
////	mpu_power(0,1);
////	mpu_power(0,2);
////	printer_power(0);
//	gps_power(0);
//	
//	osDelay(3000);
////*********************************************************************************
////刘华科 20220104  喂狗
////*********************************************************************************		
//	M41T65_write_data(0x2A,9);
//	if(GPIO_PIN_SET == HAL_GPIO_ReadPin(AWAKEN_GPIO_Port, AWAKEN_Pin))
//	{
//		global_timer.sleep_timer_sec = 0;
//		printf("\n有震动，终止休眠过程，重启设备\n");
//		system_restart(0);
//	}
//	
//	if(0 == global_device_config.sleep_network)
//	{
//		net_ec20_power(0);
//	}
//	
//	//等sd卡保存完
////	while(1==global_ch376.busy)osDelay(20);
//	
//	//关CH376电源
////	osThreadTerminate (tid_Thread_ch376);
////	ch376_power(0);
////*********************************************************************************
////刘华科 20220104  关闭看门狗
////*********************************************************************************		
//	M41T65_write_data(0xFF,9);
//	//将蓝牙RX引脚配置成外部中断
////	GPIO_InitStruct.Pin = UART4_RX_Pin;
////  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
////  GPIO_InitStruct.Pull = GPIO_PULLUP;
////  HAL_GPIO_Init(UART4_RX_GPIO_Port, &GPIO_InitStruct);
////	HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
////  HAL_NVIC_EnableIRQ(EXTI1_IRQn);
//	
//	
//	//将电源按键配置成外部中断
//	GPIO_InitStruct.Pin = POWER_KEY_Pin;
//  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
//  GPIO_InitStruct.Pull = GPIO_PULLUP;
//  HAL_GPIO_Init(POWER_KEY_GPIO_Port, &GPIO_InitStruct);
//	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
//  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
//	
//	//电源保持
//	HAL_GPIO_WritePin(POWER_ON_GPIO_Port, POWER_ON_Pin, GPIO_PIN_SET);  
//	
//	//HAL_UARTEx_EnableStopMode(&huart4);
//	HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);

//	//快速测试，休眠时长60s
//	
//	HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, global_device_config.sleep_time, RTC_WAKEUPCLOCK_CK_SPRE_16BITS);
//	// HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 60, RTC_WAKEUPCLOCK_CK_SPRE_16BITS);//for debug
//	
//	HAL_SuspendTick();
//	HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
//	
//	//wake up and reset
//	HAL_ResumeTick();
//	SystemClock_Config();
//	sysclock = HAL_RCC_GetSysClockFreq();
//	if(sysclock == 4000000)
//	{
//		  SystemClock_Config_internal();
//	}
//	//电源保持
//	HAL_GPIO_WritePin(POWER_ON_GPIO_Port, POWER_ON_Pin, GPIO_PIN_SET);  
//	
//	global_flag.sleep_flag = 0;
//	printf("\n[power]设备唤醒\n");
//	global_timer.sleep_timer_sec = 0;
//	
//	HAL_NVIC_DisableIRQ(EXTI1_IRQn);
//	HAL_UART_MspInit(&huart4);
//	osDelay(100);
//	
//	system_restart(0);//for debug
//	
//	//函数返回
//	goto RET_OK;
//	
//RET_OK:
//	return power_sleep_check_ret.RET_OK;
//	
//RET_INVALID:
//	return power_sleep_check_ret.RET_INVALID;
//}

/*
	检查设备是否需要关机
*/
int power_down_check(void)
{
		//返回值定义
	struct power_down_check_ret_struct
	{
		const int RET_OK;
		const int RET_INVALID;
	}power_down_check_ret={0,1};
	
	if(1==global_flag.power_button_flag || 2==global_flag.battery_low_flag || 1==global_flag.battery_low_flag)
	{
		//三个灯亮红提示即将关机
//		led_run_control(LED_RED);
		
		//EC20退出休眠
//		net_power_sleep(0);
		
		if(1==global_flag.power_button_flag)
		{
//			net_log_write("设备手动关机\r\n");
//			net_registration_device("power_down");
		}
		
		if(1==global_flag.battery_low_flag)
		{
//			net_log_write("设备没电自动关机\r\n");
//			net_registration_device("low_off");
		}
		else if(2==global_flag.battery_low_flag)
		{
			global_flag.battery_low_flag = 3;
			beep(1);
			osDelay(500);
			beep(0);
			osDelay(50);
			beep(1);
			osDelay(50);
			beep(0);
			osDelay(50);
			beep(1);
			osDelay(50);
			beep(0);
			return power_down_check_ret.RET_OK;
		}
		
		if(1==global_flag.already_print_flag)
		{
			//清空本地日志
			memset((uint8_t*)&global_local_log_vibration.global_local_log_vibration[0], 0, sizeof(struct log_struct_vibration)*60);
			global_flag.already_print_flag = 0;
		}
		
//		net_mqtt_disconnect(global_mqtt_config.client_idx);
//		net_mqtt_close(global_mqtt_config.client_idx);
		global_flag.button_start_up_flag=1;
		global_flag.system_restart_flag=0;
		flash_save_global_variables();
		
		global_flag.power_button_flag = 0;
		global_flag.battery_low_flag = 0;
		gps_power_switch(DISABLE);
//		NET_POWER_OFF();
		osDelay(100);
		while(1==global_flag.flash_busy_flag)osDelay(20);
		feed_dog_func(1);
		KEY_OUT_OFF();
		osDelay(8000);
	}
	else
	{
		return power_down_check_ret.RET_INVALID;
	}
	
	return power_down_check_ret.RET_OK;
}

