//*********************************************************************************
//LHK 20220420  定时器0A与0B
//*********************************************************************************	

/* Includes ------------------------------------------------------------------*/
#include "main.h"

void led_gpio_init(void)
{
	stc_gpio_init_t stcGpioInit;
    /* LED initialize */
    (void)GPIO_StructInit(&stcGpioInit);
	stcGpioInit.u16PinState = PIN_STAT_RST;
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
  (void)GPIO_Init(LED_GREEN_PORT, LED_GREEN_PIN, &stcGpioInit);
	(void)GPIO_Init(LED_RED_PORT, LED_RED_PIN, &stcGpioInit);
	(void)GPIO_Init(LED_YELLOW_PORT, LED_YELLOW_PIN, &stcGpioInit);
	
	
	(void)GPIO_StructInit(&stcGpioInit);
	stcGpioInit.u16PinState = PIN_STAT_RST;
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
//    (void)GPIO_Init(LED_BAT_1_GPIO_Port, LED_BAT_1_Pin, &stcGpioInit);
	(void)GPIO_Init(LED_BAT_2_GPIO_Port, LED_BAT_2_Pin, &stcGpioInit);

	
}
void key_gpio_init(void)
{
	stc_gpio_init_t stcGpioInit;
    /* LED initialize */
	
	(void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16ExtInt = PIN_EXTINT_ON;
    stcGpioInit.u16PullUp = PIN_PU_ON;
    (void)GPIO_Init(KEY_IN_PORT, KEY_IN_PIN, &stcGpioInit);
//	KEY_OUT_ON() ;
    (void)GPIO_StructInit(&stcGpioInit);
	stcGpioInit.u16PinState = PIN_STAT_RST;
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    (void)GPIO_Init(KEY_OUT_PORT, KEY_OUT_PIN, &stcGpioInit);
	  KEY_OUT_ON() ;
}
void CENTER_BAT_init(void)
{
		stc_gpio_init_t stcGpioInit;
		(void)GPIO_StructInit(&stcGpioInit);
		stcGpioInit.u16PinState = PIN_STAT_RST;
		stcGpioInit.u16PinDir = PIN_DIR_OUT;
		(void)GPIO_Init(CENTER_BAT_GPIO_Port, CENTER_BAT_Pin, &stcGpioInit);
}
void beep_gpio_init(void)
{
	stc_gpio_init_t stcGpioInit;
    /* LED initialize */
    (void)GPIO_StructInit(&stcGpioInit);
	stcGpioInit.u16PinState = PIN_STAT_RST;
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
////    (void)GPIO_Init(BEEP_PORT, BEEP_PIN, &stcGpioInit);
//	BEEP_OFF();
//	GPIO_OE(LED_GREEN_PORT, LED_GREEN_PIN, Enable);
}
void gps_power_switch_gpio_init(void)
{
	stc_gpio_init_t stcGpioInit;
    /* LED initialize */
    (void)GPIO_StructInit(&stcGpioInit);
	stcGpioInit.u16PinState = PIN_STAT_RST;
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    (void)GPIO_Init(GPS_POWER_PORT, GPS_POWER_PIN, &stcGpioInit);
}
//void net_gpio_init(void)
//{
//	stc_gpio_init_t stcGpioInit;
//    (void)GPIO_StructInit(&stcGpioInit);
//	stcGpioInit.u16PinState = PIN_STAT_RST;
//    stcGpioInit.u16PinDir = PIN_DIR_OUT;
//    (void)GPIO_Init(NET_POWER_PORT, NET_POWER_PIN, &stcGpioInit);
//	
//    (void)GPIO_StructInit(&stcGpioInit);
//	stcGpioInit.u16PinState = PIN_STAT_RST;
//    stcGpioInit.u16PinDir = PIN_DIR_OUT;
//    (void)GPIO_Init(NET_DTR_PORT, NET_DTR_PIN, &stcGpioInit);
//		NET_POWER_OFF();
//		NET_DTR_OFF();//关闭休眠状态
//}
void AWAKEN_gpio_init(void)
{

}
void switch_dog_init(void)
{
	stc_gpio_init_t stcGpioInit;
	GPIO_SetDebugPort(GPIO_PSPCR_SPFE_2|GPIO_PSPCR_SPFE_4,DISABLE);
	(void)GPIO_StructInit(&stcGpioInit);
	stcGpioInit.u16PinDir = PIN_DIR_OUT;
	stcGpioInit.u16PullUp = PIN_PU_ON;
	(void)GPIO_Init(FEED_DOG_Port, FEED_DOG_Pin, &stcGpioInit);
	(void)GPIO_Init(SWITCH_DOG_Port, SWITCH_DOG_Pin, &stcGpioInit);
}
void CHARG_init(void)
{
	stc_gpio_init_t stcGpioInit;
	(void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16ExtInt = PIN_EXTINT_ON;
    stcGpioInit.u16PullUp = PIN_PU_ON;
	 (void)GPIO_Init(CHARG_Port, CHARG_Pin, &stcGpioInit);
	
}
void gpio_init(void)
{
	led_gpio_init();
	gps_power_switch_gpio_init();
	beep_gpio_init();
	key_gpio_init();
	CENTER_BAT_init();
//	net_gpio_init();
	switch_dog_init();
	CHARG_init();
}

void gps_power_switch(en_functional_state_t enNewState)
{
	if (DISABLE == enNewState) 
	{
		GPS_POWER_OFF();
	}
	else
	{
		GPS_POWER_ON();
	}
}

void feed_dog_func(uint8_t fsign)
{
	if(fsign == 0)
	{
		SWITCH_DOG_OUT_OFF();
		FEED_DOG_OUT_ON();
		osDelay(5);
		FEED_DOG_OUT_OFF();
		osDelay(5);
	}
	else
	{
		SWITCH_DOG_OUT_ON();
	}
	
}
//void blue_tooth_power_switch(en_functional_state_t enNewState)
//{
//	if (DISABLE == enNewState) 
//	{
//		BLUE_TOOTH_POWER_OFF();
//	}
//	else
//	{
//		BLUE_TOOTH_POWER_ON();
//	}
//}
//extern uint32_t tim0_A;
//struct beep_class_s beep_class;
//void beep_on(void)
//{
//	if(beep_class.sign == 2)
//	{
//		beep_class.sign = 1;
//		beep_class.beep_time_open = tim0_A;
//	}
//	if((tim0_A < 40) && (key_class.key_start < 2))
//	{
//		return;
//	}
//	if((tim0_A - beep_class.beep_time_open) < 30) 
//	{
//		BEEP_ON();
//	}
//	else
//	{
//		beep_class.sign = 0;
//		beep_class.beep_time_open = 0;
//		BEEP_OFF();
//	}
//}
//void BSP_LED_On(uint8_t u8Led)
//{
////    BSP_IO_WritePortPin(LED_PORT, u8Led, LED_ON);
////	GPIO_SetPins(LED_PORT,LED_RED_PIN);
//	GPIO_SetPins(SD_ON_PORT,SD_ON_PIN);	
//}

/**
 * @brief  Turn off LEDs.
 * @param  [in] u8Led LED
 *   @arg  LED_RED
 *   @arg  LED_YELLOW
 *   @arg  LED_BLUE
 * @retval none
 */
void BSP_LED_Off(uint8_t u8Led)
{
//    BSP_IO_WritePortPin(LED_PORT, u8Led, LED_OFF);
//	GPIO_ResetPins(LED_RED_PORT,LED_RED_PIN);
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
