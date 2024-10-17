/**
  ******************************************************************************
  * File Name          : TIM.h
  * Description        : This file provides code for the configuration
  *                      of the TIM instances.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2021 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __gpio_H
#define __gpio_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "hc32_ll.h"
#include <stdbool.h>
#include <string.h>
//LED
//GREEN
#define LED_GREEN_PORT              			 (GPIO_PORT_A)
#define LED_GREEN_PIN               		    (GPIO_PIN_01)
#define LED_GREEN_ON()                     GPIO_SetPins(LED_GREEN_PORT, LED_GREEN_PIN)
#define LED_GREEN_OFF()                    GPIO_ResetPins(LED_GREEN_PORT, LED_GREEN_PIN)

//RED
#define LED_RED_PORT              			 (GPIO_PORT_C)
#define LED_RED_PIN               		(GPIO_PIN_13)
#define LED_RED_ON()                     GPIO_SetPins(LED_RED_PORT, LED_RED_PIN)
#define LED_RED_OFF()                    GPIO_ResetPins(LED_RED_PORT, LED_RED_PIN)

//YELLOW
#define LED_YELLOW_PORT              			 (GPIO_PORT_H)
#define LED_YELLOW_PIN               		    (GPIO_PIN_02)
#define LED_YELLOW_ON()                     GPIO_SetPins(LED_YELLOW_PORT, LED_YELLOW_PIN)
#define LED_YELLOW_OFF()                    GPIO_ResetPins(LED_YELLOW_PORT, LED_YELLOW_PIN)

//#define LED_PORT              			(GPIO_PORT_B)
//#define LED_BLUE_PIN               		(GPIO_PIN_15)
//#define LED_RED_PIN               		(GPIO_PIN_14)
//#define LED_BULE_ON()                     GPIO_SetPins(LED_PORT, LED_BLUE_PIN)
//#define LED_BLUE_OFF()                    GPIO_ResetPins(LED_PORT, LED_BLUE_PIN)
//#define LED_RED_ON()                     GPIO_SetPins(LED_PORT, LED_RED_PIN)
//#define LED_RED_OFF()                    GPIO_ResetPins(LED_PORT, LED_RED_PIN)
//quantity of electricity led
//#define LED_BAT_1_Pin 					(GPIO_PIN_10)
//#define LED_BAT_1_GPIO_Port 			(GPIO_PORT_A)
//#define LED_BAT_1_ON()                     GPIO_SetPins(LED_BAT_1_GPIO_Port, LED_BAT_1_Pin)
//#define LED_BAT_1_OFF()                    GPIO_ResetPins(LED_BAT_1_GPIO_Port, LED_BAT_1_Pin)

#define LED_BAT_2_Pin 					(GPIO_PIN_09)
#define LED_BAT_2_GPIO_Port 			(GPIO_PORT_A)
#define LED_BAT_2_ON()                     GPIO_SetPins(LED_BAT_2_GPIO_Port, LED_BAT_2_Pin)
#define LED_BAT_2_OFF()                    GPIO_ResetPins(LED_BAT_2_GPIO_Port, LED_BAT_2_Pin)

//#define LED_BAT_3_Pin 					(GPIO_PIN_05)
//#define LED_BAT_3_GPIO_Port 			(GPIO_PORT_B)
//#define LED_BAT_3_ON()                     GPIO_SetPins(LED_BAT_3_GPIO_Port, LED_BAT_3_Pin)
//#define LED_BAT_3_OFF()                    GPIO_ResetPins(LED_BAT_3_GPIO_Port, LED_BAT_3_Pin)

//#define LED_BAT_4_Pin 					(GPIO_PIN_06)
//#define LED_BAT_4_GPIO_Port				(GPIO_PORT_B)
//#define LED_BAT_4_ON()                     GPIO_SetPins(LED_BAT_4_GPIO_Port, LED_BAT_4_Pin)
//#define LED_BAT_4_OFF()                    GPIO_ResetPins(LED_BAT_4_GPIO_Port, LED_BAT_4_Pin)

#define LED_BAT_5_Pin 					(GPIO_PIN_07)
#define LED_BAT_5_GPIO_Port 			(GPIO_PORT_B)
#define LED_BAT_5_ON()                     GPIO_SetPins(LED_BAT_5_GPIO_Port, LED_BAT_5_Pin)
#define LED_BAT_5_OFF()                    GPIO_ResetPins(LED_BAT_5_GPIO_Port, LED_BAT_5_Pin)

//GPS开关
#define GPS_POWER_PORT				(GPIO_PORT_A)
#define GPS_POWER_PIN				(GPIO_PIN_05)
#define GPS_POWER_ON()               GPIO_SetPins(GPS_POWER_PORT, GPS_POWER_PIN)
#define GPS_POWER_OFF()              GPIO_ResetPins(GPS_POWER_PORT, GPS_POWER_PIN)
//蓝牙开关
//#define BLUE_TOOTH_POWER_PORT		(GPIO_PORT_F)
//#define BLUE_TOOTH_POWER_PIN		(GPIO_PIN_01)
//#define BLUE_TOOTH_POWER_ON()        GPIO_SetPins(BLUE_TOOTH_POWER_PORT, BLUE_TOOTH_POWER_PIN)
//#define BLUE_TOOTH_POWER_OFF()       GPIO_ResetPins(BLUE_TOOTH_POWER_PORT, BLUE_TOOTH_POWER_PIN)
//按键输入
#define KEY_IN_PORT					(GPIO_PORT_B)
#define KEY_IN_PIN					(GPIO_PIN_02)
#define KEY_IN()       				 GPIO_ReadInputPins(KEY_IN_PORT, KEY_IN_PIN) 

//开关机
#define KEY_OUT_PORT				(GPIO_PORT_A)
#define KEY_OUT_PIN					(GPIO_PIN_02)
#define KEY_OUT_ON()        		GPIO_SetPins(KEY_OUT_PORT, KEY_OUT_PIN)
#define KEY_OUT_OFF()       		GPIO_ResetPins(KEY_OUT_PORT, KEY_OUT_PIN)

//CENTER POWER ON
#define CENTER_BAT_Pin 					(GPIO_PIN_10)
#define CENTER_BAT_GPIO_Port 			(GPIO_PORT_A)
#define CENTER_BAT_ON()                     GPIO_SetPins(CENTER_BAT_GPIO_Port, CENTER_BAT_Pin)
#define CENTER_BAT_OFF()                    GPIO_ResetPins(CENTER_BAT_GPIO_Port, CENTER_BAT_Pin)
////4G开关
//#define NET_POWER_PORT		(GPIO_PORT_B)
//#define NET_POWER_PIN		(GPIO_PIN_13)
//#define NET_POWER_ON()        GPIO_SetPins(NET_POWER_PORT, NET_POWER_PIN)
//#define NET_POWER_OFF()       GPIO_ResetPins(NET_POWER_PORT, NET_POWER_PIN)
////4G休眠
//#define NET_DTR_PORT		(GPIO_PORT_A)
//#define NET_DTR_PIN		    (GPIO_PIN_01)
//#define NET_DTR_ON()        GPIO_SetPins(NET_DTR_PORT, NET_DTR_PIN)
//#define NET_DTR_OFF()       GPIO_ResetPins(NET_DTR_PORT, NET_DTR_PIN)



//看门狗
#define JLINK
#ifdef JLINK
#define FEED_DOG_Pin				(GPIO_PIN_03)   
#define FEED_DOG_Port				(GPIO_PORT_B)    
#define FEED_DOG_OUT_ON()        	GPIO_SetPins(FEED_DOG_Port, FEED_DOG_Pin)
#define FEED_DOG_OUT_OFF()       	GPIO_ResetPins(FEED_DOG_Port, FEED_DOG_Pin)
#define SWITCH_DOG_Pin				(GPIO_PIN_04)   
#define SWITCH_DOG_Port				(GPIO_PORT_B)    
#define SWITCH_DOG_OUT_ON()        	GPIO_SetPins(SWITCH_DOG_Port, SWITCH_DOG_Pin)
#define SWITCH_DOG_OUT_OFF()       	GPIO_ResetPins(SWITCH_DOG_Port, SWITCH_DOG_Pin)
#else
#define FEED_DOG_Pin				(GPIO_PIN_08)   
#define FEED_DOG_Port				(GPIO_PORT_B)    
#define FEED_DOG_OUT_ON()        	GPIO_SetPins(FEED_DOG_Port, FEED_DOG_Pin)
#define FEED_DOG_OUT_OFF()       	GPIO_ResetPins(FEED_DOG_Port, FEED_DOG_Pin)
#define SWITCH_DOG_Pin				(GPIO_PIN_09)   
#define SWITCH_DOG_Port				(GPIO_PORT_B)    
#define SWITCH_DOG_OUT_ON()        	GPIO_SetPins(SWITCH_DOG_Port, SWITCH_DOG_Pin)
#define SWITCH_DOG_OUT_OFF()       	GPIO_ResetPins(SWITCH_DOG_Port, SWITCH_DOG_Pin)
#endif
//充电状态
#define CHARG_Pin					(GPIO_PIN_01)  
#define CHARG_Port					(GPIO_PORT_B) 
#define CHARG_IN()					 GPIO_ReadInputPins(CHARG_Port, CHARG_Pin) 

//I2C
#define I2C_DIR_WR	0		/* 写控制bit */
#define I2C_DIR_RD	1		/* 读控制bit */

#define I2C_SDA_Pin   (GPIO_PIN_9)
#define I2C_SDA_Port  (GPIO_PORT_B)  
#define SDA_1 GPIO_SetPins(I2C_SDA_Port, I2C_SDA_Pin)
#define SDA_0 GPIO_ResetPins(I2C_SDA_Port, I2C_SDA_Pin)

#define I2C_SCL_Pin   (GPIO_PIN_8)
#define I2C_SCL_Port  (GPIO_PORT_B)  
#define SCL_1 GPIO_SetPins(I2C_SCL_Port, I2C_SCL_Pin)
#define SCL_0 GPIO_ResetPins(I2C_SCL_Port, I2C_SCL_Pin)


#define SDA()  GPIO_ReadInputPins(I2C_SDA_Port, I2C_SDA_Pin) 
/* USER CODE END Includes */
//#define LED_RED_PORT                (GPIO_PORT_F)
//#define LED_RED_PIN                 (GPIO_PIN_15)

//#define SD_ON_PORT                (GPIO_PORT_C)
//#define SD_ON_PIN                 (GPIO_PIN_07)


struct beep_class_s
{
	uint32_t beep_time_open;
	uint32_t sign;
};
extern struct beep_class_s beep_class;
void gpio_init(void);
void led_control(bool i_bool,uint8_t sign);
void gps_power_switch(en_functional_state_t enNewState);
void blue_tooth_power_switch(en_functional_state_t enNewState);
void beep_on(void);
void BSP_LED_On(uint8_t u8Led);
void BSP_LED_Off(uint8_t u8Led);
void feed_dog_func(uint8_t fsign);
/* USER CODE BEGIN Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ __gpio_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
