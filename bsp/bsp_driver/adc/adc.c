//*********************************************************************************
//LHK 20220420  ¶¨Ê±Æ÷0AÓë0B
//*********************************************************************************	

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/


/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/

static void AdcClockConfig(void);
static void AdcInitConfig(void);
static void AdcSetPinAnalogMode(void);


/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/

/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/

/**
 * @brief  Main function of ADC base project
 * @param  None
 * @retval int32_t return value, if needed
 */

/**
 * @brief  ADC configuration.
 * @param  None
 * @retval None
 */
void AdcConfig(void)
{
    AdcClockConfig();
    AdcInitConfig();
}

/**
 * @brief  Configures ADC clock.
 * @param  None
 * @retval None
 */
static void AdcClockConfig(void)
{
#if (ADC_CLK_SEL == ADC_CLK_SYS_CLK)
    /*
     * 1. Configures the clock divider of PCLK2 and PCLK4 here or in the function of configuring the system clock.
     *    In this example, the system clock is MRC@8MHz.
     *    PCLK4 is the digital interface clock, and PCLK2 is the analog circuit clock.
     *    Make sure that PCLK2 and PCLK4 meet the following conditions:
     *      PCLK4 : PCLK2 = 1:1, 2:1, 4:1, 8:1, 1:2, 1:4.
     *      PCLK2 is in range [1MHz, 60MHz].
     */
    CLK_SetClockDiv((CLK_BUS_PCLK2 | CLK_BUS_PCLK4), (CLK_PCLK2_DIV8 | CLK_PCLK4_DIV2));
#endif
    /* 2. Specifies the clock source of ADC. */
    CLK_SetPeriClockSrc(ADC_CLK);
}

/**
 * @brief  Initializes ADC.
 * @param  None
 * @retval None
 */
static void AdcInitConfig(void)
{
    stc_adc_init_t stcAdcInit;

    /* 1. Enable ADC peripheral clock. */
    FCG_Fcg3PeriphClockCmd(ADC_PERIPH_CLK, ENABLE);

    /* 2. Modify the default value depends on the application. Not needed here. */
    (void)ADC_StructInit(&stcAdcInit);

    /* 3. Initializes ADC. */
    (void)ADC_Init(ADC_UNIT, &stcAdcInit);

    /* 4. ADC channel configuration. */
    /* 4.1 Set the ADC pin to analog input mode. */
    AdcSetPinAnalogMode();
    /* 4.2 Enable ADC channels. Call ADC_ChCmd() again to enable more channels if needed. */
    ADC_ChCmd(ADC_UNIT, ADC_SEQ, ADC_CH, ENABLE);

    /* 5. Conversion data average calculation function, if needed.
          Call ADC_ConvDataAverageChCmd() again to enable more average channels if needed. */
    ADC_ConvDataAverageConfig(ADC_UNIT, ADC_AVG_CNT8);
    ADC_ConvDataAverageChCmd(ADC_UNIT, ADC_CH, ENABLE);
}

/**
 * @brief  Set specified ADC pin to analog mode.
 * @param  None
 * @retval None
 */
static void AdcSetPinAnalogMode(void)
{
    stc_gpio_init_t stcGpioInit;

    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinAttr = PIN_ATTR_ANALOG;
    (void)GPIO_Init(ADC_CH_PORT, ADC_CH_PIN, &stcGpioInit);
}

/**
 * @brief  Use ADC in polling mode.
 * @param  None
 * @retval None
 */
uint16_t u16AdcValue0;
void AdcPolling(void)
{
    uint16_t u16AdcValue;
	uint16_t bat_V = 0.0f;
    int32_t iRet = LL_ERR;
    __IO uint32_t u32TimeCount = 0UL;

    /* Can ONLY start sequence A conversion.
       Sequence B needs hardware trigger to start conversion. */
    ADC_Start(ADC_UNIT);
    do {
        if (ADC_GetStatus(ADC_UNIT, ADC_EOC_FLAG) == SET) {
            ADC_ClearStatus(ADC_UNIT, ADC_EOC_FLAG);
            iRet = LL_OK;
            break;
        }
    } while (u32TimeCount++ < ADC_TIMEOUT_VAL);

    if (iRet == LL_OK) {
        /* Get any ADC value of sequence A channel that needed. */
        u16AdcValue = ADC_GetValue(ADC_UNIT, ADC_CH);
		bat_V = ADC_CAL_VOL(u16AdcValue)*1.5;
		if(bat_V < BATTERY_LOW_V)
		{
			u16AdcValue0 = 0;
		}
		else
		{
			u16AdcValue0 = ((bat_V - BATTERY_LOW_V)*100.0/(BATTERY_HIGH_V - BATTERY_LOW_V));
			if(u16AdcValue0 > 99)
			{
				u16AdcValue0 = 100;
			}
		}
		global_device_info.battery_voltage = (double)bat_V/1000;
		if(bat_V < 3101)
		{
			feed_dog_func(1);
			KEY_OUT_OFF();
		}

    } else {
        ADC_Stop(ADC_UNIT);
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

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
