
#include "main.h" 
#include "data_buf.h"
/*----------------------------------------------------------------------------
 *      Timer: Sample timer functions
 *---------------------------------------------------------------------------*/ 
 static void Timer2_Callback (void const *arg);  
 osTimerId id2;  
static uint32_t  exec2;                                         // argument for the timer call back function
static osTimerDef (Timer2, Timer2_Callback);
uint32_t temp_timer = 0;
static void Timer2_Callback (void const *arg)
{
	temp_timer++;
		
	/*
		系统开机总时长
	*/
	global_timer.system_timer += 1;
	
	/*
		九轴传感器定时器
	*/
	global_timer.mpu_timer_ms+=1;
	global_timer.mpu_test_timer_ms+=1;
	
	/*
		网络线程定时器
	*/
	global_timer.net_timer_ms+=1;
	
	/*
		GNSS毫秒定时器
	*/ 
	global_timer.gnss_timer_ms+=1;
	
	/*
		上传间隔计时器
	*/
	global_timer.upload_interval_timer_ms+=1;
	
	/*
		蜂鸣器定时器
	*/
	global_timer.beep_timer += 1;
	
	/*
		秒定时器集合
	*/
	if(0 == global_timer.system_timer%1000)
	{
		global_timer.gps_timer_sec += 1;
		global_timer.sleep_timer_sec += 1;
//		global_timer.live_timer_sec += 1;
		global_timer.get_video_timer_sec += 1;
		global_timer.pressure_timer_sec += 1;
		global_timer.local_log_timer_sec += 1;
		global_timer.get_time_sec+=1;
		global_timer.net_power_interval_sec+=1;
		global_timer.system_timer_sec++;
	}
}

// Example: Create and Start timers
void Init_Timers (void)
{
  osStatus status;                                              // function return status
 
  // Create periodic timer
  exec2 = 2;
  id2 = osTimerCreate(osTimer(Timer2), osTimerPeriodic, &exec2);
  if (id2 != NULL)
	{    // Periodic timer created
    // start timer with periodic 1ms interval
    status = osTimerStart (id2, 1);            
    if (status != osOK)
		{
      // Timer could not be started
    }
  }
}
//*******************************************************************************
/* TMR0 unit and channel definition */
#define TMR0_UNIT                       (CM_TMR0_1)
#define TMR0_CLK                        (FCG2_PERIPH_TMR0_1)
#define TMR0_CH                         (TMR0_CH_A)
#define TMR0_TRIG_CH                    (AOS_TMR0)
#define TMR0_CH_INT                     (TMR0_INT_CMP_A)
#define TMR0_CH_FLAG                    (TMR0_FLAG_CMP_A)
#define TMR0_INT_SRC                    (INT_SRC_TMR0_1_CMP_A)
#define TMR0_IRQn                       (INT015_IRQn)
/* Period = 1 / (Clock freq / div) * (Compare value + 1) = 500ms */
#define TMR0_CMP_VALUE                  (XTAL_VALUE / 8U / 80U - 1U)

/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/

/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 * @brief  TMR0 compare interrupt callback function.
 * @param  None
 * @retval None
 */
 uint32_t timer_0A;
static void TMR0_CompareIrqCallback(void)
{
    timer_0A++;
	  if(ins_class.initing == true)
		{
			imu_class.run();
			ins_class.run();
			data_buf_class.run();
		}
    TMR0_ClearStatus(TMR0_UNIT, TMR0_CH_FLAG);
}

/**
 * @brief  Configure TMR0.
 * @note   In asynchronous clock, If you want to write a TMR0 register, you need to wait for
 *         at least 3 asynchronous clock cycles after the last write operation!
 * @param  None
 * @retval None
 */
void TMR0_Config(void)
{
    stc_tmr0_init_t stcTmr0Init;
    stc_irq_signin_config_t stcIrqSignConfig;

    /* Enable timer0 and AOS clock */
    FCG_Fcg2PeriphClockCmd(TMR0_CLK, ENABLE);
    FCG_Fcg0PeriphClockCmd(FCG0_PERIPH_AOS, ENABLE);

    /* TIMER0 configuration */
    (void)TMR0_StructInit(&stcTmr0Init);
    stcTmr0Init.u32ClockSrc     = TMR0_CLK_SRC_INTERN_CLK;
    stcTmr0Init.u32ClockDiv     = TMR0_CLK_DIV8;
    stcTmr0Init.u32Func         = TMR0_FUNC_CMP;
    stcTmr0Init.u16CompareValue = (uint16_t)TMR0_CMP_VALUE;
    (void)TMR0_Init(TMR0_UNIT, TMR0_CH, &stcTmr0Init);
    DDL_DelayMS(1U);
    TMR0_HWStopCondCmd(TMR0_UNIT, TMR0_CH, ENABLE);
    DDL_DelayMS(1U);
    TMR0_IntCmd(TMR0_UNIT, TMR0_CH_INT, ENABLE);
    DDL_DelayMS(1U);
    AOS_SetTriggerEventSrc(TMR0_TRIG_CH, BSP_KEY_KEY10_EVT);

    /* Interrupt configuration */
    stcIrqSignConfig.enIntSrc    = TMR0_INT_SRC;
    stcIrqSignConfig.enIRQn      = TMR0_IRQn;
    stcIrqSignConfig.pfnCallback = &TMR0_CompareIrqCallback;
    (void)INTC_IrqSignIn(&stcIrqSignConfig);
    NVIC_ClearPendingIRQ(stcIrqSignConfig.enIRQn);
    NVIC_SetPriority(stcIrqSignConfig.enIRQn, DDL_IRQ_PRIO_DEFAULT);
    NVIC_EnableIRQ(stcIrqSignConfig.enIRQn);
		TMR0_Start(TMR0_UNIT, TMR0_CH);
}
