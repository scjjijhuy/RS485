/*
	��Դ����ģ��ײ����
	���ߣ�lee
	ʱ�䣺2019/5/15
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
//	��ȡADCԭʼֵ
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
//	//�ֲ�����
//	int ret=0;
//	ADC_ChannelConfTypeDef sConfig = {0};
//	
//	//��ʼ��MSP
//	HAL_ADC_MspInit(&hadc1);
//	
//	//����ͨ��
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
//	//У׼
//	ret = HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
//	if (ret != HAL_OK)
//  {
//    printf("[power]error ret=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
//		goto RET_ERROR;
//  }
//	
//	//��ʼת��
//	ret = HAL_ADC_Start(&hadc1);
//	if (ret != HAL_OK)
//  {
//    printf("[power]error ret=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
//		goto RET_ERROR;
//  }
//	
//	//�ȴ�ת�����
//	ret = HAL_ADC_PollForConversion(&hadc1, 1000);
//	if (ret != HAL_OK)
//  {
//    printf("[power]error ret=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
//		goto RET_ERROR;
//  }
//	
//	//��ȡֵ
//	*data_out = HAL_ADC_GetValue(&hadc1);
//	
//	//ֹͣת��
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
//	�����ѹֵ
//	@ref ADC_HAL_EC_CHANNEL
//*/
//int power_adc_voltage_get(uint32_t channel_in, double *voltage_out)
//{
//	struct power_adc_voltage_get_ret_struct
//	{
//		const int RET_OK;
//		const int RET_ERROR;
//	}power_adc_voltage_get_ret = {0,1};

//	//�ֲ�����
//	int ret=0;
//	uint32_t vrefint_data = 0;
//	uint32_t adc_data_x = 0;
//	uint16_t vrefint_cal = 0;
//	int full_scale = 4095;
//	double voltage=0;
//	
//	//��ȡIN17�ڲ��ο���ѹԭʼ����
//	ret = power_adc_data_get(ADC_CHANNEL_VREFINT, &vrefint_data);
//	if (0 != ret)
//  {
//    printf("[power]error ret=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
//		goto RET_ERROR;
//  }
//	
//	//��ȡ��ص���ԭʼͨ����������
//	ret = power_adc_data_get(channel_in, &adc_data_x);
//	if (0 != ret)
//  {
//    printf("[power]error ret=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
//		goto RET_ERROR;
//  }
//	
//	//��ȡVREFINT_CAL�ڲ��ο���ѹУ׼ֵ
//	vrefint_cal = *(__IO uint16_t*)(VREFINT_CAL_ADDR);
//	
//	//ȷ��FULL_SCALE
//	full_scale = 4095;
//	
//	//�����ѹֵ
//	voltage = (3.0*(double)vrefint_cal*(double)adc_data_x) / ((double)vrefint_data*(double)full_scale);
//	
//	//��ֵ
//	*voltage_out = voltage;
//	
//	//����
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
	��ȡ��ص����ٷֱ�
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
	
	//��̬����
	static double voltage_array[VOL_ARRAY_SIZE]={0};
	static int init_flag=0xFF;
	static int percent_array[PER_ARRAY_SIZE]={0};
	static int last_percent=0;
	
	//�ֲ�����
	int ret=0;
	double voltage=0;
	double percent=0;
	int i=0;
	double max=-999;
	double min=999;
	double sum=0;
	int pass=0;
	
	//�������ͨ����ѹ
	AdcPolling();
	voltage = global_device_info.battery_voltage;
	if (0 != ret)
  {
    printf("[power]error ret=%d, file=%s, line=%d\n", ret, __FILE__, __LINE__);
		return power_battery_percent_ret.RET_ERROR;
  }
	
	//��ص�ѹ
//	voltage *= 1.5f;
	if(init_flag == 0xFF)
	{
		init_flag = 0;
		return power_battery_percent_ret.RET_ERROR;
	}
	//�Ƿ���Ҫ��ʼ���˲�����
	if(0==init_flag)
	{
		for(i=0;i<VOL_ARRAY_SIZE;i++)
		{
			voltage_array[i] = voltage;
		}
	}
	
	//��ֵ��ӵ���β
	for(i=0;i<VOL_ARRAY_SIZE-1;i++)
	{
		voltage_array[i] = voltage_array[i+1];
	}
	voltage_array[VOL_ARRAY_SIZE-1] = voltage;

	//����
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
	//��ѹ�ٷֱ�
	percent = ((voltage - BATTERY_LOW_V)/(BATTERY_HIGH_V - BATTERY_LOW_V))*100.0;
	
	if(1 > percent)
	{
		percent = 1;
	}
	if(100 < percent)
	{
		percent = 100;
	}
	
	//��ʼ��
	if(0==init_flag)
	{
		for(i=0;i<PER_ARRAY_SIZE;i++)
		{
			percent_array[i] = (int)percent;
		}
		last_percent = (int)percent;
	}
	
	//��ֵ��ӵ���β
	for(i=0;i<PER_ARRAY_SIZE-1;i++)
	{
		percent_array[i] = percent_array[i+1];
	}
	percent_array[PER_ARRAY_SIZE-1] = (int)percent;
	
	//��ֵ
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
	
	//��ʼ����־
	if(0==init_flag)
	{
		init_flag=1;
	}
	
	//����
	return power_battery_percent_ret.RET_OK;
}


///*
//	����豸�Ƿ���Ҫ��������
//*/
uint32_t power_sleep_times = 0;
int power_sleep_check(void)
{
	//����ֵ����
	struct power_sleep_check_ret_struct
	{
		const int RET_OK;
		const int RET_INVALID;
	}power_sleep_check_ret={0,1};
	
	//�ֲ�����
	uint32_t iLoop = 0;
	//�ж��豸�񶯿����Ƿ񵽴�����������û�еȴ��������һ�������ϴ�
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
//	//EC20�˳�����
////	net_power_sleep(0);
//	net_log_write("�豸����\r\n");
//	//����ʱ�䵽��׼����������
//	printf("\n[power]����ʱ�䵽��׼����������\n");
//	global_flag.sleep_flag = 1;
//	
//	osDelay(5000);
//	
//	//ɱ�߳�
//	osThreadTerminate (tid_Thread_led_beep);
////	osThreadTerminate (tid_Thread_mpu);
////	osThreadTerminate (tid_Thread_aht10);
//	osThreadTerminate (tid_Thread_rtc);
////	osThreadTerminate (tid_Thread_bluetooth);
//	osThreadTerminate (tid_Thread_gps);
//	osTimerDelete (id2);
//	

////*********************************************************************************
////������ 20220104  ι��
////*********************************************************************************		
//	M41T65_write_data(0x2A,9);
//  
////*********************************************************************************
////������ 20220104  ι��
////*********************************************************************************	
//  //����flash
//	global_flag.system_restart_flag=1;	
//	flash_save_global_variables();
//	
//	//��������ע����Ϣ
//	net_registration_device("sleep_down");
//	
//	//EC20��������
//	net_power_sleep(1);
//	
//	//��������
//	printf("\n[power]׼����Ͻ�������,����ʱ��%d��\n",global_device_config.sleep_time);
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
////������ 20220104  ι��
////*********************************************************************************		
//	M41T65_write_data(0x2A,9);
//	if(GPIO_PIN_SET == HAL_GPIO_ReadPin(AWAKEN_GPIO_Port, AWAKEN_Pin))
//	{
//		global_timer.sleep_timer_sec = 0;
//		printf("\n���𶯣���ֹ���߹��̣������豸\n");
//		system_restart(0);
//	}
//	
//	if(0 == global_device_config.sleep_network)
//	{
//		net_ec20_power(0);
//	}
//	
//	//��sd��������
////	while(1==global_ch376.busy)osDelay(20);
//	
//	//��CH376��Դ
////	osThreadTerminate (tid_Thread_ch376);
////	ch376_power(0);
////*********************************************************************************
////������ 20220104  �رտ��Ź�
////*********************************************************************************		
//	M41T65_write_data(0xFF,9);
//	//������RX�������ó��ⲿ�ж�
////	GPIO_InitStruct.Pin = UART4_RX_Pin;
////  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
////  GPIO_InitStruct.Pull = GPIO_PULLUP;
////  HAL_GPIO_Init(UART4_RX_GPIO_Port, &GPIO_InitStruct);
////	HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
////  HAL_NVIC_EnableIRQ(EXTI1_IRQn);
//	
//	
//	//����Դ�������ó��ⲿ�ж�
//	GPIO_InitStruct.Pin = POWER_KEY_Pin;
//  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
//  GPIO_InitStruct.Pull = GPIO_PULLUP;
//  HAL_GPIO_Init(POWER_KEY_GPIO_Port, &GPIO_InitStruct);
//	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
//  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
//	
//	//��Դ����
//	HAL_GPIO_WritePin(POWER_ON_GPIO_Port, POWER_ON_Pin, GPIO_PIN_SET);  
//	
//	//HAL_UARTEx_EnableStopMode(&huart4);
//	HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);

//	//���ٲ��ԣ�����ʱ��60s
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
//	//��Դ����
//	HAL_GPIO_WritePin(POWER_ON_GPIO_Port, POWER_ON_Pin, GPIO_PIN_SET);  
//	
//	global_flag.sleep_flag = 0;
//	printf("\n[power]�豸����\n");
//	global_timer.sleep_timer_sec = 0;
//	
//	HAL_NVIC_DisableIRQ(EXTI1_IRQn);
//	HAL_UART_MspInit(&huart4);
//	osDelay(100);
//	
//	system_restart(0);//for debug
//	
//	//��������
//	goto RET_OK;
//	
//RET_OK:
//	return power_sleep_check_ret.RET_OK;
//	
//RET_INVALID:
//	return power_sleep_check_ret.RET_INVALID;
//}

/*
	����豸�Ƿ���Ҫ�ػ�
*/
int power_down_check(void)
{
		//����ֵ����
	struct power_down_check_ret_struct
	{
		const int RET_OK;
		const int RET_INVALID;
	}power_down_check_ret={0,1};
	
	if(1==global_flag.power_button_flag || 2==global_flag.battery_low_flag || 1==global_flag.battery_low_flag)
	{
		//������������ʾ�����ػ�
//		led_run_control(LED_RED);
		
		//EC20�˳�����
//		net_power_sleep(0);
		
		if(1==global_flag.power_button_flag)
		{
//			net_log_write("�豸�ֶ��ػ�\r\n");
//			net_registration_device("power_down");
		}
		
		if(1==global_flag.battery_low_flag)
		{
//			net_log_write("�豸û���Զ��ػ�\r\n");
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
			//��ձ�����־
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

