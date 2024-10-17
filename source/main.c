/**
 *******************************************************************************
 * @file  adc/adc_base/source/main.c
 * @brief Main program ADC base for the Device Driver Library.
 @verbatim
   Change Logs:
   Date             Author          Notes
   2022-03-31       CDT             First version
 @endverbatim
 *******************************************************************************
 * Copyright (C) 2022, Xiaohua Semiconductor Co., Ltd. All rights reserved.
 *
 * This software component is licensed by XHSC under BSD 3-Clause license
 * (the "License"); You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                    opensource.org/licenses/BSD-3-Clause
 *
 *******************************************************************************
 */

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "main.h"
#include "data_buf.h"
//const static char app_descriptor[32] __attribute__((at(0x00020800))) = APP_DESCRIPTOR; //  

struct imu_class_s imu_class;
struct ins_class_s ins_class;
struct data_buf_class_s data_buf_class;
int32_t fputc(int32_t ch, FILE *f)
{
	uint32_t iLoop = 0;
	while(((CM_USART1->SR&0X40)==0) && (iLoop++ < 10000));//循环发送,直到发送完毕   
	CM_USART1->DR = (uint8_t) ch;      
	return ch;
}
void GW_push_time(char *buffer_in)
{
	char *p=NULL;
	int ret=0;
	int temp_year=0;
	int temp_month=0;
	int temp_day=0;
	int temp_hour=0;
	int temp_minute=0;
	int temp_second=0;
	p = strstr(buffer_in, "live:");
	if(NULL!=p)
	{
		ret=sscanf(p, "live:%02d-%02d-%02d %02d:%02d:%02d", \
			&temp_year,
			&temp_month,
			&temp_day,
			&temp_hour,
			&temp_minute,
			&temp_second);
		if(6==ret)
		{
			global_rtc_set_time.time_for_set.year=temp_year;
			global_rtc_set_time.time_for_set.month=temp_month;
			global_rtc_set_time.time_for_set.day=temp_day;
			global_rtc_set_time.time_for_set.hour=temp_hour;
			global_rtc_set_time.time_for_set.minute=temp_minute;
			global_rtc_set_time.time_for_set.second=temp_second;
			
			global_rtc_set_time.time_for_set.weekday = (global_rtc_set_time.time_for_set.year/400 - 2*global_rtc_set_time.time_for_set.year/100 + temp_year/4 + 13*(temp_month+1)/5 + temp_day)%7;
			global_rtc_set_time.need_set=1;
		}
	}
}
void restart_usart1_recv(void)
{
	memset(cmd_recv_buffer,0,sizeof(cmd_recv_buffer));
	DMA_receive_IT(CM_USART1,CM_DMA1,DMA_CH0,(uint32_t)(&cmd_recv_buffer[0]),sizeof(cmd_recv_buffer)-1);
	(void)USART_ReadData(CM_USART1);
}
/**
 * @brief  Main function of adc_base project
 * @param  None
 * @retval int32_t return value, if needed
 */
 uint32_t led_Power_time = 0;
 uint32_t main_tim = 0;
uint8_t Feed_dog = 0;
stc_efm_unique_id_t id;
float tt = 0.0;
uint8_t tt2 = 0;
int32_t main(void)
{
    /* System clock is MRC@8MHz */
    uint32_t u32Pclk1,iLoop = 0;

	stc_clock_freq_t stcClkTmp;

    /* MCU Peripheral registers write unprotected. */
    LL_PERIPH_WE(LL_PERIPH_GPIO | LL_PERIPH_FCG | LL_PERIPH_PWC_CLK_RMU | LL_PERIPH_EFM | LL_PERIPH_SRAM);
    BSP_CLK_Init();
	/* Get pclk1 */
    CLK_GetClockFreq(&stcClkTmp);
    u32Pclk1 = stcClkTmp.u32Pclk1Freq;\
	if(u32Pclk1 > 4000000)
	{
	}	
	for(iLoop = 0;iLoop < 200000;iLoop++);
	TMR0_Config();
	USART1_init();
	RTC_Config();
	gpio_init();
	
	icm20689_init();

	osKernelInitialize (); 
	Init_Timers();
	Init_Thread_led_beep();
	Init_Thread_rtc();
	Init_Thread_net();
	Init_Thread_gps();
    osKernelStart (); 
    /***************** Configuration end, application start **************/
    /* USER CODE BEGIN WHILE */

  imu_init(&imu_class);
	ins_init(&ins_class);
	data_buf_init(&data_buf_class);
	//进入大循环，准备接收控制指令
//	restart_usart1_recv();   //串口1 待处理
	led_Power_time = global_timer.system_timer_sec;
    while (1) 
	{ 
		
			if(imu_data.sign == 1)
			{
				  main_tim++;
					imu_data.sign = 0;
					printf("ax=%.2lf ay=%.2lf az=%.2lf R=%d P=%d,Y=%d \n",imu_data.f_ax,imu_data.f_ay,imu_data.f_az,(int)(ins_class.ang_x*57.29f),(int)(ins_class.ang_y*57.29f),(int)(ins_class.ang_z*57.29f));
			}
		
    }
}

/**
 * @}
 */

/**
 * @}
 */

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
