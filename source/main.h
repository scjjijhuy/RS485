/**
 *******************************************************************************
 * @file adc/adc_base/source/main.h
 * @brief This file contains the including files of main routine.
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
#ifndef __MAIN_H__
#define __MAIN_H__
#include <stdbool.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include "hc32_ll.h"
#include "ev_hc32f460_lqfp100_v2_bsp.h"
#include "cmsis_os.h"
#include "net_thread.h"
#include "net_port.h"
#include "gps_thread.h"
#include "rtc_thread.h"
#include "led_beep_thread.h"
#include "timer.h"
#include "usart.h"
#include "common_c.h"
#include "gps_port.h"
#include "rtc_port.h"
#include "flash_port.h"
#include "flash_function.h"
#include "stack.h"
#include "gpio.h"
#include "beep_port.h"
#include "led_port.h"
#include "led_function.h"
#include "adc.h"
#include "power_port.h"
#include "net_function.h"
#include "net_power.h"
#include "net_log.h"
#include "net_file_port.h"
#include "bsp_i2c.h"
#include "rx8025t.h"
#include "icm20689.h"
#include "ins.h"
#include "imu.h"


//#define APP_DESCRIPTOR  	 "app:B200G base V2.3";

extern uint8_t Feed_dog;
void GW_push_time(char *buffer_in);
#endif /* __MAIN_H__ */

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
