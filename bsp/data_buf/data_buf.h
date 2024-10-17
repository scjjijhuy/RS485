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
#ifndef __DATA_BUF_H
#define __DATA_BUF_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "hc32_ll.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

struct imu_data_class
{
	float f_ax;
	float f_ay;
	float f_az;
	int sign;
};
struct imu_threshold_class
{
	float threshold_ax;
	float threshold_ay;
	float threshold_az;
	uint32_t Vibration_coefficient;
};
struct imu_class_data_block
{
	int32_t data_num;
	int32_t timer_5s;
	int32_t type;
	int32_t buf_full_sign;
};
struct data_buf_class_s
{
	struct imu_class_data_block data_b[10];	
	int32_t buf_num;
	int32_t max_num;
	void  (*run)(void);
};
extern struct data_buf_class_s data_buf_class;	
extern struct imu_threshold_class imu_threshold;
extern struct imu_data_class imu_data;
//extern uint32_t Vibration_coefficient;
/* USER CODE BEGIN Prototypes */
void data_buf_init(struct data_buf_class_s* _data_buf);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ __DATA_BUF_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
