//*********************************************************************************
//LHK 20220420  定时器0A与0B
//*********************************************************************************	

/* Includes ------------------------------------------------------------------*/
#include "main.h"
uint32_t USART1_RX_num = 0,USART1_RX_p = 0;
uint32_t USART3_RX_num = 0,USART3_RX_p = 0;
//****************************************************************************************************************
//dma重載
//****************************************************************************************************************
void DMA_receive_IT(CM_USART_TypeDef *usart,CM_DMA_TypeDef *DMAx, uint8_t u8Ch,uint32_t u32DestAddr,uint32_t u32TransCnt)
{
	
	DMA_ChCmd(DMAx, u8Ch, DISABLE);	
	DMA_TransCompleteIntCmd(DMAx, (DMA_INT_TC_CH0<<u8Ch), DISABLE);
	DMA_SetDestAddr(DMAx, u8Ch, (uint32_t)u32DestAddr);
	DMA_SetBlockSize(DMAx,u8Ch,1);

	DMA_SetTransCount(DMAx, u8Ch, u32TransCnt);

	(void)DMA_ChCmd(DMAx, u8Ch, ENABLE);
    DMA_TransCompleteIntCmd(DMAx, (DMA_INT_TC_CH0<<u8Ch), ENABLE);
}	
void DMA_transfer_IT(CM_USART_TypeDef *usart,CM_DMA_TypeDef *DMAx, uint8_t u8Ch,uint32_t u32SrcAddr,uint32_t u32TransCnt)
{
	DMA_ChCmd(DMAx, u8Ch, DISABLE);	
	DMA_SetSrcAddr(DMAx, u8Ch, (uint32_t)u32SrcAddr);

	DMA_SetTransCount(DMAx, u8Ch, u32TransCnt);

	(void)DMA_ChCmd(DMAx, u8Ch, ENABLE);

	USART_FuncCmd(usart, USART_TX, ENABLE);
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
//****************************************************************************************************************
//usart1
//****************************************************************************************************************

/* DMA definition */
#define RX1_DMA_UNIT                     (CM_DMA1)
#define RX1_DMA_CH                       (DMA_CH0)
#define RX1_DMA_FCG_ENABLE()             (FCG_Fcg0PeriphClockCmd(FCG0_PERIPH_DMA1, ENABLE))
#define RX1_DMA_TRIG_SEL                 (AOS_DMA1_0)
#define RX1_DMA_TRIG_EVT_SRC             (EVT_SRC_USART1_RI)
#define RX1_DMA_RECONF_TRIG_SEL          (AOS_DMA_RC)
#define RX1_DMA_RECONF_TRIG_EVT_SRC      (EVT_SRC_AOS_STRG)
#define RX1_DMA_TC_INT                   (DMA_INT_TC_CH0)
#define RX1_DMA_TC_FLAG                  (DMA_FLAG_TC_CH0)
#define RX1_DMA_TC_IRQn                  (INT006_IRQn)
#define RX1_DMA_TC_INT_SRC               (INT_SRC_DMA1_TC0)

#define TX1_DMA_UNIT                     (CM_DMA2)
#define TX1_DMA_CH                       (DMA_CH0)
#define TX1_DMA_FCG_ENABLE()             (FCG_Fcg0PeriphClockCmd(FCG0_PERIPH_DMA2, ENABLE))
#define TX1_DMA_TRIG_SEL                 (AOS_DMA2_0)
#define TX1_DMA_TRIG_EVT_SRC             (EVT_SRC_USART1_TI)
#define TX1_DMA_TC_INT                   (DMA_INT_TC_CH0)
#define TX1_DMA_TC_FLAG                  (DMA_FLAG_TC_CH0)
#define TX1_DMA_TC_IRQn                  (INT007_IRQn)
#define TX1_DMA_TC_INT_SRC               (INT_SRC_DMA2_TC0)


/* USART RX/TX pin definition */
#define USART1_RX_PORT                   (GPIO_PORT_A)   /* PH13: USART1_RX */
#define USART1_RX_PIN                    (GPIO_PIN_03)
#define USART1_RX_GPIO_FUNC              (GPIO_FUNC_33)

#define USART1_TX_PORT                   (GPIO_PORT_A)   /* PH15: USART1_TX */
#define USART1_TX_PIN                    (GPIO_PIN_07)
#define USART1_TX_GPIO_FUNC              (GPIO_FUNC_32)

/* USART unit definition */
#define USART1_UNIT                      (CM_USART1)
#define USART1_FCG_ENABLE()              (FCG_Fcg1PeriphClockCmd(FCG1_PERIPH_USART1, ENABLE))

/* USART baudrate definition */
#define USART1_BAUDRATE                  (460800UL)

/* USART timeout bits definition */
#define USART1_TIMEOUT_BITS              (2000U)

/* USART interrupt definition */
#define USART1_TX_CPLT_IRQn              (INT008_IRQn)
#define USART1_TX_CPLT_INT_SRC           (INT_SRC_USART1_TCI)

#define USART1_RX_ERR_IRQn               (INT009_IRQn)
#define USART1_RX_ERR_INT_SRC            (INT_SRC_USART1_EI)

#define RS485_EN_PORT                   (GPIO_PORT_A)
#define RS485_EN_PIN                    (GPIO_PIN_05)
#define RS485_EN_TX()                     GPIO_SetPins(RS485_EN_PORT, RS485_EN_PIN)
#define RS485_EN_RX()                    GPIO_ResetPins(RS485_EN_PORT, RS485_EN_PIN)


/* Application frame length max definition */
#define APP_FRAME_LEN_MAX               (1024u)

//static __IO en_flag_status_t m_enRxFrameEnd;
static __IO uint16_t m_u16RxLen;
char cmd_recv_buffer[APP_FRAME_LEN_MAX];
uint8_t m_au8TxBuf[APP_FRAME_LEN_MAX] = {"usart1 tx test"};

/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/

/**
 * @brief  DMA transfer complete IRQ callback function.
 * @param  None
 * @retval None
 */

/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/

/**
 * @brief  DMA transfer complete IRQ callback function.
 * @param  None
 * @retval None
 */

static void RX1_DMA_TC_IrqCallback(void)
{
    m_u16RxLen = APP_FRAME_LEN_MAX;

    USART_FuncCmd(USART1_UNIT, USART_RX_TIMEOUT, DISABLE);

    DMA_ClearTransCompleteStatus(RX1_DMA_UNIT, RX1_DMA_TC_FLAG);
}

/**
 * @brief  DMA transfer complete IRQ callback function.
 * @param  None
 * @retval None
 */
static void TX1_DMA_TC_IrqCallback(void)
{
    USART_FuncCmd(USART1_UNIT, USART_INT_TX_CPLT, ENABLE);

    DMA_ClearTransCompleteStatus(TX1_DMA_UNIT, TX1_DMA_TC_FLAG);
}

/**
 * @brief  Initialize DMA.
 * @param  None
 * @retval int32_t:
 *           - LL_OK:                   Initialize successfully.
 *           - LL_ERR_INVD_PARAM:       Initialization paramters is invalid.
 */
int32_t USART1_DMA_Config(void)
{
    int32_t i32Ret;
    stc_dma_init_t stcDmaInit;
    stc_dma_llp_init_t stcDmaLlpInit;
    stc_irq_signin_config_t stcIrqSignConfig;
    static stc_dma_llp_descriptor_t stcLlpDesc;

    /* DMA&AOS FCG enable */
    RX1_DMA_FCG_ENABLE();
    TX1_DMA_FCG_ENABLE();
    FCG_Fcg0PeriphClockCmd(FCG0_PERIPH_AOS, ENABLE);

    /* USART_RX_DMA */
    (void)DMA_StructInit(&stcDmaInit);
    stcDmaInit.u32IntEn = DMA_INT_ENABLE;
    stcDmaInit.u32BlockSize = 1UL;
    stcDmaInit.u32TransCount = ARRAY_SZ(cmd_recv_buffer);
    stcDmaInit.u32DataWidth = DMA_DATAWIDTH_8BIT;
    stcDmaInit.u32DestAddr = (uint32_t)cmd_recv_buffer;
    stcDmaInit.u32SrcAddr = ((uint32_t)(&USART1_UNIT->DR) + 2UL);
    stcDmaInit.u32SrcAddrInc = DMA_SRC_ADDR_FIX;
    stcDmaInit.u32DestAddrInc = DMA_DEST_ADDR_INC;
    i32Ret = DMA_Init(RX1_DMA_UNIT, RX1_DMA_CH, &stcDmaInit);
    if (LL_OK == i32Ret) {
        (void)DMA_LlpStructInit(&stcDmaLlpInit);
        stcDmaLlpInit.u32State = DMA_LLP_ENABLE;
        stcDmaLlpInit.u32Mode  = DMA_LLP_WAIT;
        stcDmaLlpInit.u32Addr  = (uint32_t)&stcLlpDesc;
        (void)DMA_LlpInit(RX1_DMA_UNIT, RX1_DMA_CH, &stcDmaLlpInit);

        stcLlpDesc.SARx   = stcDmaInit.u32SrcAddr;
        stcLlpDesc.DARx   = stcDmaInit.u32DestAddr;
        stcLlpDesc.DTCTLx = (stcDmaInit.u32TransCount << DMA_DTCTL_CNT_POS) | (stcDmaInit.u32BlockSize << DMA_DTCTL_BLKSIZE_POS);;
        stcLlpDesc.LLPx   = (uint32_t)&stcLlpDesc;
        stcLlpDesc.CHCTLx = stcDmaInit.u32SrcAddrInc | stcDmaInit.u32DestAddrInc | stcDmaInit.u32DataWidth |  \
                            stcDmaInit.u32IntEn      | stcDmaLlpInit.u32State    | stcDmaLlpInit.u32Mode;

        DMA_ReconfigLlpCmd(RX1_DMA_UNIT, RX1_DMA_CH, ENABLE);
        DMA_ReconfigCmd(RX1_DMA_UNIT, ENABLE);
        AOS_SetTriggerEventSrc(RX1_DMA_RECONF_TRIG_SEL, RX1_DMA_RECONF_TRIG_EVT_SRC);

        stcIrqSignConfig.enIntSrc = RX1_DMA_TC_INT_SRC;
        stcIrqSignConfig.enIRQn  = RX1_DMA_TC_IRQn;
        stcIrqSignConfig.pfnCallback = &RX1_DMA_TC_IrqCallback;
        (void)INTC_IrqSignIn(&stcIrqSignConfig);
        NVIC_ClearPendingIRQ(stcIrqSignConfig.enIRQn);
        NVIC_SetPriority(stcIrqSignConfig.enIRQn, DDL_IRQ_PRIO_13);
        NVIC_EnableIRQ(stcIrqSignConfig.enIRQn);

        AOS_SetTriggerEventSrc(RX1_DMA_TRIG_SEL, RX1_DMA_TRIG_EVT_SRC);

        DMA_Cmd(RX1_DMA_UNIT, ENABLE);
        DMA_TransCompleteIntCmd(RX1_DMA_UNIT, RX1_DMA_TC_INT, ENABLE);
        (void)DMA_ChCmd(RX1_DMA_UNIT, RX1_DMA_CH, ENABLE);
    }

    /* USART_TX_DMA */
    (void)DMA_StructInit(&stcDmaInit);
    stcDmaInit.u32IntEn = DMA_INT_ENABLE;
    stcDmaInit.u32BlockSize = 1UL;
    stcDmaInit.u32TransCount = ARRAY_SZ(m_au8TxBuf);
    stcDmaInit.u32DataWidth = DMA_DATAWIDTH_8BIT;
    stcDmaInit.u32DestAddr = (uint32_t)(&USART1_UNIT->DR);
    stcDmaInit.u32SrcAddr = (uint32_t)m_au8TxBuf;
    stcDmaInit.u32SrcAddrInc = DMA_SRC_ADDR_INC;
    stcDmaInit.u32DestAddrInc = DMA_DEST_ADDR_FIX;
    i32Ret = DMA_Init(TX1_DMA_UNIT, TX1_DMA_CH, &stcDmaInit);
    if (LL_OK == i32Ret) {
        stcIrqSignConfig.enIntSrc = TX1_DMA_TC_INT_SRC;
        stcIrqSignConfig.enIRQn  = TX1_DMA_TC_IRQn;
        stcIrqSignConfig.pfnCallback = &TX1_DMA_TC_IrqCallback;
        (void)INTC_IrqSignIn(&stcIrqSignConfig);
        NVIC_ClearPendingIRQ(stcIrqSignConfig.enIRQn);
        NVIC_SetPriority(stcIrqSignConfig.enIRQn, DDL_IRQ_PRIO_13);
        NVIC_EnableIRQ(stcIrqSignConfig.enIRQn);

        AOS_SetTriggerEventSrc(TX1_DMA_TRIG_SEL, TX1_DMA_TRIG_EVT_SRC);

        DMA_Cmd(TX1_DMA_UNIT, ENABLE);
        DMA_TransCompleteIntCmd(TX1_DMA_UNIT, TX1_DMA_TC_INT, ENABLE);
    }

    return i32Ret;
}
/**
 * @brief  USART RX timeout IRQ callback.
 * @param  None
 * @retval None
 */

/**
 * @brief  USART TX complete IRQ callback function.
 * @param  None
 * @retval None
 */
static void USART1_TxComplete_IrqCallback(void)
{
    USART_FuncCmd(USART1_UNIT, (USART_TX | USART_INT_TX_CPLT), DISABLE);

//    USART_ClearStatus(USART1_UNIT, USART_FLAG_RX_TIMEOUT);

//    USART_FuncCmd(USART1_UNIT, USART_RX_TIMEOUT, ENABLE);

    USART_ClearStatus(USART1_UNIT, USART_FLAG_TX_CPLT);
}
static void USART1_RxError_IrqCallback(void)
{
    if (SET == USART_GetStatus(USART1_UNIT, (USART_FLAG_PARITY_ERR | USART_FLAG_FRAME_ERR))) {
        (void)USART_ReadData(USART1_UNIT);
    }

    USART_ClearStatus(USART1_UNIT, (USART_FLAG_PARITY_ERR | USART_FLAG_FRAME_ERR | USART_FLAG_OVERRUN));
}
void USART1_init(void)
{
	stc_usart_uart_init_t stcUartInit;
    stc_irq_signin_config_t stcIrqSigninConfig;
	stc_gpio_init_t stcGpioInit;
	USART1_DMA_Config();
	(void)GPIO_StructInit(&stcGpioInit);
	stcGpioInit.u16PinDir = PIN_DIR_IN;
	stcGpioInit.u16PullUp = PIN_PU_ON;
    (void)GPIO_Init(USART1_RX_PORT, USART1_RX_PIN, &stcGpioInit);
	GPIO_SetFunc(USART1_RX_PORT, USART1_RX_PIN, USART1_RX_GPIO_FUNC);
	stcGpioInit.u16PinDir = PIN_DIR_OUT;
	stcGpioInit.u16PullUp = PIN_PU_ON;
    (void)GPIO_Init(USART1_TX_PORT, USART1_TX_PIN, &stcGpioInit);
    GPIO_SetFunc(USART1_TX_PORT, USART1_TX_PIN, USART1_TX_GPIO_FUNC);

    /* Enable peripheral clock */
    USART1_FCG_ENABLE();
	 /* Initialize UART. */
    (void)USART_UART_StructInit(&stcUartInit);
    stcUartInit.u32ClockDiv = USART_CLK_DIV1;
    stcUartInit.u32CKOutput = USART_CK_OUTPUT_ENABLE;
    stcUartInit.u32Baudrate = USART1_BAUDRATE*2;
    stcUartInit.u32OverSampleBit = USART_OVER_SAMPLE_8BIT;
    if (LL_OK != USART_UART_Init(USART1_UNIT, &stcUartInit, NULL)) {
        for (;;) {
        }
    }

    /* Register RX error IRQ handler && configure NVIC. */
    stcIrqSigninConfig.enIRQn = USART1_RX_ERR_IRQn;
    stcIrqSigninConfig.enIntSrc = USART1_RX_ERR_INT_SRC;
    stcIrqSigninConfig.pfnCallback = &USART1_RxError_IrqCallback;
    (void)INTC_IrqSignIn(&stcIrqSigninConfig);
    NVIC_ClearPendingIRQ(stcIrqSigninConfig.enIRQn);
    NVIC_SetPriority(stcIrqSigninConfig.enIRQn, DDL_IRQ_PRIO_13);
    NVIC_EnableIRQ(stcIrqSigninConfig.enIRQn);

    /* Register TX complete IRQ handler. */
    stcIrqSigninConfig.enIRQn = USART1_TX_CPLT_IRQn;
    stcIrqSigninConfig.enIntSrc = USART1_TX_CPLT_INT_SRC;
    stcIrqSigninConfig.pfnCallback = &USART1_TxComplete_IrqCallback;
    (void)INTC_IrqSignIn(&stcIrqSigninConfig);
    NVIC_ClearPendingIRQ(stcIrqSigninConfig.enIRQn);
    NVIC_SetPriority(stcIrqSigninConfig.enIRQn, DDL_IRQ_PRIO_13);
    NVIC_EnableIRQ(stcIrqSigninConfig.enIRQn);

//    /* Register RX error IRQ handler. */
//    stcIrqSigninConfig.enIRQn = USART1_RX_ERR_IRQn;
//    stcIrqSigninConfig.enIntSrc = USART1_RX_ERR_INT_SRC;
//    stcIrqSigninConfig.pfnCallback = &USART1_RxError_IrqCallback;
//    (void)INTC_IrqSignIn(&stcIrqSigninConfig);
//    NVIC_ClearPendingIRQ(stcIrqSigninConfig.enIRQn);
//    NVIC_SetPriority(stcIrqSigninConfig.enIRQn, DDL_IRQ_PRIO_14);
//    NVIC_EnableIRQ(stcIrqSigninConfig.enIRQn);

	USART_FuncCmd(USART1_UNIT, (USART_RX | USART_INT_RX|USART_TX), ENABLE);
	RS485_EN_TX();
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
//****************************************************************************************************************
//usart2
//****************************************************************************************************************

/* DMA definition */
#define RX2_DMA_UNIT                     (CM_DMA1)
#define RX2_DMA_CH                       (DMA_CH2)
#define RX2_DMA_FCG_ENABLE()             (FCG_Fcg0PeriphClockCmd(FCG0_PERIPH_DMA1, ENABLE))
#define RX2_DMA_TRIG_SEL                 (AOS_DMA1_2)
#define RX2_DMA_TRIG_EVT_SRC             (EVT_SRC_USART2_RI)
#define RX2_DMA_RECONF_TRIG_SEL          (AOS_DMA_RC)
#define RX2_DMA_RECONF_TRIG_EVT_SRC      (EVT_SRC_AOS_STRG)
#define RX2_DMA_TC_INT                   (DMA_INT_TC_CH2)
#define RX2_DMA_TC_FLAG                  (DMA_FLAG_TC_CH2)
#define RX2_DMA_TC_IRQn                  (INT013_IRQn)
#define RX2_DMA_TC_INT_SRC               (INT_SRC_DMA1_TC2)

#define TX2_DMA_UNIT                     (CM_DMA2)
#define TX2_DMA_CH                       (DMA_CH2)
#define TX2_DMA_FCG_ENABLE()             (FCG_Fcg0PeriphClockCmd(FCG0_PERIPH_DMA2, ENABLE))
#define TX2_DMA_TRIG_SEL                 (AOS_DMA2_2)
#define TX2_DMA_TRIG_EVT_SRC             (EVT_SRC_USART2_TI)
#define TX2_DMA_TC_INT                   (DMA_INT_TC_CH2)
#define TX2_DMA_TC_FLAG                  (DMA_FLAG_TC_CH2)
#define TX2_DMA_TC_IRQn                  (INT014_IRQn)
#define TX2_DMA_TC_INT_SRC               (INT_SRC_DMA2_TC2)


/* USART RX/TX pin definition */
#define USART2_RX_PORT                   (GPIO_PORT_A)   /* PH13: USART2_RX *///新板
#define USART2_RX_PIN                    (GPIO_PIN_12)
#define USART2_RX_GPIO_FUNC              (GPIO_FUNC_37)

#define USART2_TX_PORT                   (GPIO_PORT_A)   /* PH15: USART2_TX */
#define USART2_TX_PIN                    (GPIO_PIN_11)
#define USART2_TX_GPIO_FUNC              (GPIO_FUNC_36)

/* USART unit definition */
#define USART2_UNIT                      (CM_USART2)
#define USART2_FCG_ENABLE()              (FCG_Fcg1PeriphClockCmd(FCG1_PERIPH_USART2, ENABLE))

/* USART baudrate definition */
#define USART2_BAUDRATE                  (9600UL)

/* USART timeout bits definition */
#define USART2_TIMEOUT_BITS              (2000U)

/* USART interrupt definition */
#define USART2_TX_CPLT_IRQn              (INT015_IRQn)
#define USART2_TX_CPLT_INT_SRC           (INT_SRC_USART2_TCI)

#define USART2_RX_ERR_IRQn               (INT016_IRQn)
#define USART2_RX_ERR_INT_SRC            (INT_SRC_USART2_EI)


/* Application frame length max definition */
#define USART2_FRAME_LEN_MAX               (1024U)

//static __IO en_flag_status_t m_enRxFrameEnd;
static __IO uint16_t m_u16RxLen;
//uint8_t m_au8Rx2Buf[USART2_FRAME_LEN_MAX];
extern char gps_recv_buf[1024];
uint8_t m_au8Tx2Buf[USART2_FRAME_LEN_MAX] = {"usart2 tx test"};


static void RX2_DMA_TC_IrqCallback(void)
{
//    m_u16RxLen = APP_FRAME_LEN_MAX;

    USART_FuncCmd(USART2_UNIT, USART_RX_TIMEOUT, DISABLE);

    DMA_ClearTransCompleteStatus(RX2_DMA_UNIT, RX2_DMA_TC_FLAG);
}

/**
 * @brief  DMA transfer complete IRQ callback function.
 * @param  None
 * @retval None
 */
static void TX2_DMA_TC_IrqCallback(void)
{
    USART_FuncCmd(USART2_UNIT, USART_INT_TX_CPLT, ENABLE);

    DMA_ClearTransCompleteStatus(TX2_DMA_UNIT, TX2_DMA_TC_FLAG);
}

/**
 * @brief  Initialize DMA.
 * @param  None
 * @retval int32_t:
 *           - LL_OK:                   Initialize successfully.
 *           - LL_ERR_INVD_PARAM:       Initialization paramters is invalid.
 */
int32_t USART2_DMA_Config(void)
{
    int32_t i32Ret;
    stc_dma_init_t stcDmaInit;
    stc_dma_llp_init_t stcDmaLlpInit;
    stc_irq_signin_config_t stcIrqSignConfig;
    static stc_dma_llp_descriptor_t stcLlpDesc;

    /* DMA&AOS FCG enable */
    RX2_DMA_FCG_ENABLE();
    TX2_DMA_FCG_ENABLE();
    FCG_Fcg0PeriphClockCmd(FCG0_PERIPH_AOS, ENABLE);

    /* USART_RX_DMA */
    (void)DMA_StructInit(&stcDmaInit);
    stcDmaInit.u32IntEn = DMA_INT_ENABLE;
    stcDmaInit.u32BlockSize = 1UL;
    stcDmaInit.u32TransCount = ARRAY_SZ(gps_recv_buf)-1;
    stcDmaInit.u32DataWidth = DMA_DATAWIDTH_8BIT;
    stcDmaInit.u32DestAddr = (uint32_t)gps_recv_buf;
    stcDmaInit.u32SrcAddr = ((uint32_t)(&USART2_UNIT->DR) + 2UL);
    stcDmaInit.u32SrcAddrInc = DMA_SRC_ADDR_FIX;
    stcDmaInit.u32DestAddrInc = DMA_DEST_ADDR_INC;
    i32Ret = DMA_Init(RX2_DMA_UNIT, RX2_DMA_CH, &stcDmaInit);
    if (LL_OK == i32Ret) {
        (void)DMA_LlpStructInit(&stcDmaLlpInit);
        stcDmaLlpInit.u32State = DMA_LLP_ENABLE;
        stcDmaLlpInit.u32Mode  = DMA_LLP_WAIT;
        stcDmaLlpInit.u32Addr  = (uint32_t)&stcLlpDesc;
        (void)DMA_LlpInit(RX2_DMA_UNIT, RX2_DMA_CH, &stcDmaLlpInit);

        stcLlpDesc.SARx   = stcDmaInit.u32SrcAddr;
        stcLlpDesc.DARx   = stcDmaInit.u32DestAddr;
        stcLlpDesc.DTCTLx = (stcDmaInit.u32TransCount << DMA_DTCTL_CNT_POS) | (stcDmaInit.u32BlockSize << DMA_DTCTL_BLKSIZE_POS);;
        stcLlpDesc.LLPx   = (uint32_t)&stcLlpDesc;
        stcLlpDesc.CHCTLx = stcDmaInit.u32SrcAddrInc | stcDmaInit.u32DestAddrInc | stcDmaInit.u32DataWidth |  \
                            stcDmaInit.u32IntEn      | stcDmaLlpInit.u32State    | stcDmaLlpInit.u32Mode;

        DMA_ReconfigLlpCmd(RX2_DMA_UNIT, RX2_DMA_CH, ENABLE);
        DMA_ReconfigCmd(RX2_DMA_UNIT, ENABLE);
        AOS_SetTriggerEventSrc(RX2_DMA_RECONF_TRIG_SEL, RX2_DMA_RECONF_TRIG_EVT_SRC);

        stcIrqSignConfig.enIntSrc = RX2_DMA_TC_INT_SRC;
        stcIrqSignConfig.enIRQn  = RX2_DMA_TC_IRQn;
        stcIrqSignConfig.pfnCallback = &RX2_DMA_TC_IrqCallback;
        (void)INTC_IrqSignIn(&stcIrqSignConfig);
        NVIC_ClearPendingIRQ(stcIrqSignConfig.enIRQn);
        NVIC_SetPriority(stcIrqSignConfig.enIRQn, DDL_IRQ_PRIO_11);
        NVIC_EnableIRQ(stcIrqSignConfig.enIRQn);

        AOS_SetTriggerEventSrc(RX2_DMA_TRIG_SEL, RX2_DMA_TRIG_EVT_SRC);

        DMA_Cmd(RX2_DMA_UNIT, ENABLE);
        DMA_TransCompleteIntCmd(RX2_DMA_UNIT, RX2_DMA_TC_INT, ENABLE);
        (void)DMA_ChCmd(RX2_DMA_UNIT, RX2_DMA_CH, ENABLE);
    }

    /* USART_TX_DMA */
    (void)DMA_StructInit(&stcDmaInit);
    stcDmaInit.u32IntEn = DMA_INT_ENABLE;
    stcDmaInit.u32BlockSize = 1UL;
    stcDmaInit.u32TransCount = ARRAY_SZ(m_au8Tx2Buf);
    stcDmaInit.u32DataWidth = DMA_DATAWIDTH_8BIT;
    stcDmaInit.u32DestAddr = (uint32_t)(&USART2_UNIT->DR);
    stcDmaInit.u32SrcAddr = (uint32_t)m_au8Tx2Buf;
    stcDmaInit.u32SrcAddrInc = DMA_SRC_ADDR_INC;
    stcDmaInit.u32DestAddrInc = DMA_DEST_ADDR_FIX;
    i32Ret = DMA_Init(TX2_DMA_UNIT, TX2_DMA_CH, &stcDmaInit);
    if (LL_OK == i32Ret) {
        stcIrqSignConfig.enIntSrc = TX2_DMA_TC_INT_SRC;
        stcIrqSignConfig.enIRQn  = TX2_DMA_TC_IRQn;
        stcIrqSignConfig.pfnCallback = &TX2_DMA_TC_IrqCallback;
        (void)INTC_IrqSignIn(&stcIrqSignConfig);
        NVIC_ClearPendingIRQ(stcIrqSignConfig.enIRQn);
        NVIC_SetPriority(stcIrqSignConfig.enIRQn, DDL_IRQ_PRIO_11);
        NVIC_EnableIRQ(stcIrqSignConfig.enIRQn);

        AOS_SetTriggerEventSrc(TX2_DMA_TRIG_SEL, TX2_DMA_TRIG_EVT_SRC);

        DMA_Cmd(TX2_DMA_UNIT, ENABLE);
        DMA_TransCompleteIntCmd(TX2_DMA_UNIT, TX2_DMA_TC_INT, ENABLE);
    }

    return i32Ret;
}
/**
 * @brief  USART RX timeout IRQ callback.
 * @param  None
 * @retval None
 */

/**
 * @brief  USART TX complete IRQ callback function.
 * @param  None
 * @retval None
 */
static void USART2_TxComplete_IrqCallback(void)
{
    USART_FuncCmd(USART2_UNIT, (USART_TX | USART_INT_TX_CPLT), DISABLE);

    USART_ClearStatus(USART2_UNIT, USART_FLAG_TX_CPLT);
}
static void USART2_RxError_IrqCallback(void)
{
    if (SET == USART_GetStatus(USART2_UNIT, (USART_FLAG_PARITY_ERR | USART_FLAG_FRAME_ERR))) {
        (void)USART_ReadData(USART2_UNIT);
    }

    USART_ClearStatus(USART2_UNIT, (USART_FLAG_PARITY_ERR | USART_FLAG_FRAME_ERR | USART_FLAG_OVERRUN));
}
void USART2_init(void)
{
	stc_usart_uart_init_t stcUartInit;
    stc_irq_signin_config_t stcIrqSigninConfig;
	stc_gpio_init_t stcGpioInit;
	USART2_DMA_Config();
	(void)GPIO_StructInit(&stcGpioInit);
	stcGpioInit.u16PinDir = PIN_DIR_IN;
	stcGpioInit.u16PullUp = PIN_PU_ON;
    (void)GPIO_Init(USART2_RX_PORT, USART2_RX_PIN, &stcGpioInit);
	GPIO_SetFunc(USART2_RX_PORT, USART2_RX_PIN, USART2_RX_GPIO_FUNC);
	(void)GPIO_StructInit(&stcGpioInit);
	stcGpioInit.u16PinDir = PIN_DIR_IN;
	stcGpioInit.u16PullUp = PIN_PU_ON;
    (void)GPIO_Init(USART2_TX_PORT, USART2_TX_PIN, &stcGpioInit);
    GPIO_SetFunc(USART2_TX_PORT, USART2_TX_PIN, USART2_TX_GPIO_FUNC);

    /* Enable peripheral clock */
    USART2_FCG_ENABLE();
	 /* Initialize UART. */
    (void)USART_UART_StructInit(&stcUartInit);
    stcUartInit.u32ClockDiv = USART_CLK_DIV16;
    stcUartInit.u32CKOutput = USART_CK_OUTPUT_ENABLE;
    stcUartInit.u32Baudrate = USART2_BAUDRATE;
    stcUartInit.u32OverSampleBit = USART_OVER_SAMPLE_8BIT;
    if (LL_OK != USART_UART_Init(USART2_UNIT, &stcUartInit, NULL)) {
        for (;;) {
        }
    }

    /* Register RX error IRQ handler && configure NVIC. */
    stcIrqSigninConfig.enIRQn = USART2_RX_ERR_IRQn;
    stcIrqSigninConfig.enIntSrc = USART2_RX_ERR_INT_SRC;
    stcIrqSigninConfig.pfnCallback = &USART2_RxError_IrqCallback;
    (void)INTC_IrqSignIn(&stcIrqSigninConfig);
    NVIC_ClearPendingIRQ(stcIrqSigninConfig.enIRQn);
    NVIC_SetPriority(stcIrqSigninConfig.enIRQn, DDL_IRQ_PRIO_11);
    NVIC_EnableIRQ(stcIrqSigninConfig.enIRQn);

    /* Register TX complete IRQ handler. */
    stcIrqSigninConfig.enIRQn = USART2_TX_CPLT_IRQn;
    stcIrqSigninConfig.enIntSrc = USART2_TX_CPLT_INT_SRC;
    stcIrqSigninConfig.pfnCallback = &USART2_TxComplete_IrqCallback;
    (void)INTC_IrqSignIn(&stcIrqSigninConfig);
    NVIC_ClearPendingIRQ(stcIrqSigninConfig.enIRQn);
    NVIC_SetPriority(stcIrqSigninConfig.enIRQn, DDL_IRQ_PRIO_11);
    NVIC_EnableIRQ(stcIrqSigninConfig.enIRQn);

	USART_FuncCmd(USART2_UNIT, (USART_RX | USART_INT_RX), ENABLE);
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
//****************************************************************************************************************
//usart3
//****************************************************************************************************************

/* DMA definition */
#define RX3_DMA_UNIT                     (CM_DMA1)
#define RX3_DMA_CH                       (DMA_CH3)
#define RX3_DMA_FCG_ENABLE()             (FCG_Fcg0PeriphClockCmd(FCG0_PERIPH_DMA1, ENABLE))
#define RX3_DMA_TRIG_SEL                 (AOS_DMA1_3)
#define RX3_DMA_TRIG_EVT_SRC             (EVT_SRC_USART3_RI)
#define RX3_DMA_RECONF_TRIG_SEL          (AOS_DMA_RC)
#define RX3_DMA_RECONF_TRIG_EVT_SRC      (EVT_SRC_AOS_STRG)
#define RX3_DMA_TC_INT                   (DMA_INT_TC_CH3)
#define RX3_DMA_TC_FLAG                  (DMA_FLAG_TC_CH3)
#define RX3_DMA_TC_IRQn                  (INT017_IRQn)
#define RX3_DMA_TC_INT_SRC               (INT_SRC_DMA1_TC3)

#define TX3_DMA_UNIT                     (CM_DMA2)
#define TX3_DMA_CH                       (DMA_CH3)
#define TX3_DMA_FCG_ENABLE()             (FCG_Fcg0PeriphClockCmd(FCG0_PERIPH_DMA2, ENABLE))
#define TX3_DMA_TRIG_SEL                 (AOS_DMA2_3)
#define TX3_DMA_TRIG_EVT_SRC             (EVT_SRC_USART3_TI)
#define TX3_DMA_TC_INT                   (DMA_INT_TC_CH3)
#define TX3_DMA_TC_FLAG                  (DMA_FLAG_TC_CH3)
#define TX3_DMA_TC_IRQn                  (INT018_IRQn)
#define TX3_DMA_TC_INT_SRC               (INT_SRC_DMA2_TC3)


/* USART RX/TX pin definition */
#define USART3_RX_PORT                   (GPIO_PORT_B)   /* PH13: USART3_RX *///新板
#define USART3_RX_PIN                    (GPIO_PIN_06)
#define USART3_RX_GPIO_FUNC              (GPIO_FUNC_33)

#define USART3_TX_PORT                   (GPIO_PORT_B)   /* PH15: USART3_TX */
#define USART3_TX_PIN                    (GPIO_PIN_05)
#define USART3_TX_GPIO_FUNC              (GPIO_FUNC_32)

/* USART unit definition */
#define USART3_UNIT                      (CM_USART3)
#define USART3_FCG_ENABLE()              (FCG_Fcg1PeriphClockCmd(FCG1_PERIPH_USART3, ENABLE))

/* USART baudrate definition */
#define USART3_BAUDRATE                  (115200UL)

/* USART timeout bits definition */
#define USART3_TIMEOUT_BITS              (2000U)

/* USART interrupt definition */
#define USART3_TX_CPLT_IRQn              (INT019_IRQn)
#define USART3_TX_CPLT_INT_SRC           (INT_SRC_USART3_TCI)

#define USART3_RX_ERR_IRQn               (INT020_IRQn)
#define USART3_RX_ERR_INT_SRC            (INT_SRC_USART3_EI)


/* Application frame length max definition */
#define USART3_FRAME_LEN_MAX               (1024U)

//static __IO en_flag_status_t m_enRxFrameEnd;
static __IO uint16_t m_u16RxLen;
//uint8_t m_au8Rx2Buf[USART3_FRAME_LEN_MAX];

char usart3_recv_buffer[1024];
char net_send_buffer[USART3_FRAME_LEN_MAX] = {"usart3 tx test"};


static void RX3_DMA_TC_IrqCallback(void)
{
//    m_u16RxLen = APP_FRAME_LEN_MAX;

    USART_FuncCmd(USART3_UNIT, USART_RX_TIMEOUT, DISABLE);

    DMA_ClearTransCompleteStatus(RX3_DMA_UNIT, RX3_DMA_TC_FLAG);
}

/**
 * @brief  DMA transfer complete IRQ callback function.
 * @param  None
 * @retval None
 */
static void TX3_DMA_TC_IrqCallback(void)
{
    USART_FuncCmd(USART3_UNIT, USART_INT_TX_CPLT, ENABLE);

    DMA_ClearTransCompleteStatus(TX3_DMA_UNIT, TX3_DMA_TC_FLAG);
}

/**
 * @brief  Initialize DMA.
 * @param  None
 * @retval int33_t:
 *           - LL_OK:                   Initialize successfully.
 *           - LL_ERR_INVD_PARAM:       Initialization paramters is invalid.
 */
int32_t USART3_DMA_Config(void)
{
    int32_t i32Ret;
    stc_dma_init_t stcDmaInit;
    stc_dma_llp_init_t stcDmaLlpInit;
    stc_irq_signin_config_t stcIrqSignConfig;
    static stc_dma_llp_descriptor_t stcLlpDesc;

    /* DMA&AOS FCG enable */
    RX3_DMA_FCG_ENABLE();
    TX3_DMA_FCG_ENABLE();
    FCG_Fcg0PeriphClockCmd(FCG0_PERIPH_AOS, ENABLE);

    /* USART_RX_DMA */
    (void)DMA_StructInit(&stcDmaInit);
    stcDmaInit.u32IntEn = DMA_INT_ENABLE;
    stcDmaInit.u32BlockSize = 1UL;
    stcDmaInit.u32TransCount = ARRAY_SZ(usart3_recv_buffer)-1;
    stcDmaInit.u32DataWidth = DMA_DATAWIDTH_8BIT;
    stcDmaInit.u32DestAddr = (uint32_t)usart3_recv_buffer;
    stcDmaInit.u32SrcAddr = ((uint32_t)(&USART3_UNIT->DR) + 2UL);
    stcDmaInit.u32SrcAddrInc = DMA_SRC_ADDR_FIX;
    stcDmaInit.u32DestAddrInc = DMA_DEST_ADDR_INC;
    i32Ret = DMA_Init(RX3_DMA_UNIT, RX3_DMA_CH, &stcDmaInit);
    if (LL_OK == i32Ret) {
        (void)DMA_LlpStructInit(&stcDmaLlpInit);
        stcDmaLlpInit.u32State = DMA_LLP_ENABLE;
        stcDmaLlpInit.u32Mode  = DMA_LLP_WAIT;
        stcDmaLlpInit.u32Addr  = (uint32_t)&stcLlpDesc;
        (void)DMA_LlpInit(RX3_DMA_UNIT, RX3_DMA_CH, &stcDmaLlpInit);

        stcLlpDesc.SARx   = stcDmaInit.u32SrcAddr;
        stcLlpDesc.DARx   = stcDmaInit.u32DestAddr;
        stcLlpDesc.DTCTLx = (stcDmaInit.u32TransCount << DMA_DTCTL_CNT_POS) | (stcDmaInit.u32BlockSize << DMA_DTCTL_BLKSIZE_POS);;
        stcLlpDesc.LLPx   = (uint32_t)&stcLlpDesc;
        stcLlpDesc.CHCTLx = stcDmaInit.u32SrcAddrInc | stcDmaInit.u32DestAddrInc | stcDmaInit.u32DataWidth |  \
                            stcDmaInit.u32IntEn      | stcDmaLlpInit.u32State    | stcDmaLlpInit.u32Mode;

        DMA_ReconfigLlpCmd(RX3_DMA_UNIT, RX3_DMA_CH, ENABLE);
        DMA_ReconfigCmd(RX3_DMA_UNIT, ENABLE);
        AOS_SetTriggerEventSrc(RX3_DMA_RECONF_TRIG_SEL, RX3_DMA_RECONF_TRIG_EVT_SRC);

        stcIrqSignConfig.enIntSrc = RX3_DMA_TC_INT_SRC;
        stcIrqSignConfig.enIRQn  = RX3_DMA_TC_IRQn;
        stcIrqSignConfig.pfnCallback = &RX3_DMA_TC_IrqCallback;
        (void)INTC_IrqSignIn(&stcIrqSignConfig);
        NVIC_ClearPendingIRQ(stcIrqSignConfig.enIRQn);
        NVIC_SetPriority(stcIrqSignConfig.enIRQn, DDL_IRQ_PRIO_11);
        NVIC_EnableIRQ(stcIrqSignConfig.enIRQn);

        AOS_SetTriggerEventSrc(RX3_DMA_TRIG_SEL, RX3_DMA_TRIG_EVT_SRC);

        DMA_Cmd(RX3_DMA_UNIT, ENABLE);
        DMA_TransCompleteIntCmd(RX3_DMA_UNIT, RX3_DMA_TC_INT, ENABLE);
        (void)DMA_ChCmd(RX3_DMA_UNIT, RX3_DMA_CH, ENABLE);
    }

    /* USART_TX_DMA */
    (void)DMA_StructInit(&stcDmaInit);
    stcDmaInit.u32IntEn = DMA_INT_ENABLE;
    stcDmaInit.u32BlockSize = 1UL;
    stcDmaInit.u32TransCount = ARRAY_SZ(net_send_buffer);
    stcDmaInit.u32DataWidth = DMA_DATAWIDTH_8BIT;
    stcDmaInit.u32DestAddr = (uint32_t)(&USART3_UNIT->DR);
    stcDmaInit.u32SrcAddr = (uint32_t)net_send_buffer;
    stcDmaInit.u32SrcAddrInc = DMA_SRC_ADDR_INC;
    stcDmaInit.u32DestAddrInc = DMA_DEST_ADDR_FIX;
    i32Ret = DMA_Init(TX3_DMA_UNIT, TX3_DMA_CH, &stcDmaInit);
    if (LL_OK == i32Ret) {
        stcIrqSignConfig.enIntSrc = TX3_DMA_TC_INT_SRC;
        stcIrqSignConfig.enIRQn  = TX3_DMA_TC_IRQn;
        stcIrqSignConfig.pfnCallback = &TX3_DMA_TC_IrqCallback;
        (void)INTC_IrqSignIn(&stcIrqSignConfig);
        NVIC_ClearPendingIRQ(stcIrqSignConfig.enIRQn);
        NVIC_SetPriority(stcIrqSignConfig.enIRQn, DDL_IRQ_PRIO_11);
        NVIC_EnableIRQ(stcIrqSignConfig.enIRQn);

        AOS_SetTriggerEventSrc(TX3_DMA_TRIG_SEL, TX3_DMA_TRIG_EVT_SRC);

        DMA_Cmd(TX3_DMA_UNIT, ENABLE);
        DMA_TransCompleteIntCmd(TX3_DMA_UNIT, TX3_DMA_TC_INT, ENABLE);
    }

    return i32Ret;
}
/**
 * @brief  USART RX timeout IRQ callback.
 * @param  None
 * @retval None
 */

/**
 * @brief  USART TX complete IRQ callback function.
 * @param  None
 * @retval None
 */
static void USART3_TxComplete_IrqCallback(void)
{
    USART_FuncCmd(USART3_UNIT, (USART_TX | USART_INT_TX_CPLT), DISABLE);

    USART_ClearStatus(USART3_UNIT, USART_FLAG_TX_CPLT);
}
static void USART3_RxError_IrqCallback(void)
{
    if (SET == USART_GetStatus(USART3_UNIT, (USART_FLAG_PARITY_ERR | USART_FLAG_FRAME_ERR))) {
        (void)USART_ReadData(USART3_UNIT);
    }

    USART_ClearStatus(USART3_UNIT, (USART_FLAG_PARITY_ERR | USART_FLAG_FRAME_ERR | USART_FLAG_OVERRUN));
}
void USART3_init(void)
{
	stc_usart_uart_init_t stcUartInit;
    stc_irq_signin_config_t stcIrqSigninConfig;
	stc_gpio_init_t stcGpioInit;
	USART3_DMA_Config();
	(void)GPIO_StructInit(&stcGpioInit);
	stcGpioInit.u16PinDir = PIN_DIR_IN;
	stcGpioInit.u16PullUp = PIN_PU_ON;
    (void)GPIO_Init(USART3_RX_PORT, USART3_RX_PIN, &stcGpioInit);
	GPIO_SetFunc(USART3_RX_PORT, USART3_RX_PIN, USART3_RX_GPIO_FUNC);
	(void)GPIO_StructInit(&stcGpioInit);
	stcGpioInit.u16PinDir = PIN_DIR_IN;
	stcGpioInit.u16PullUp = PIN_PU_ON;
    (void)GPIO_Init(USART3_TX_PORT, USART3_TX_PIN, &stcGpioInit);
    GPIO_SetFunc(USART3_TX_PORT, USART3_TX_PIN, USART3_TX_GPIO_FUNC);

    /* Enable peripheral clock */
    USART3_FCG_ENABLE();
	 /* Initialize UART. */
    (void)USART_UART_StructInit(&stcUartInit);
    stcUartInit.u32ClockDiv = USART_CLK_DIV1;
    stcUartInit.u32CKOutput = USART_CK_OUTPUT_ENABLE;
    stcUartInit.u32Baudrate = USART3_BAUDRATE;
    stcUartInit.u32OverSampleBit = USART_OVER_SAMPLE_8BIT;
    if (LL_OK != USART_UART_Init(USART3_UNIT, &stcUartInit, NULL)) {
        for (;;) {
        }
    }

    /* Register RX error IRQ handler && configure NVIC. */
    stcIrqSigninConfig.enIRQn = USART3_RX_ERR_IRQn;
    stcIrqSigninConfig.enIntSrc = USART3_RX_ERR_INT_SRC;
    stcIrqSigninConfig.pfnCallback = &USART3_RxError_IrqCallback;
    (void)INTC_IrqSignIn(&stcIrqSigninConfig);
    NVIC_ClearPendingIRQ(stcIrqSigninConfig.enIRQn);
    NVIC_SetPriority(stcIrqSigninConfig.enIRQn, DDL_IRQ_PRIO_10);
    NVIC_EnableIRQ(stcIrqSigninConfig.enIRQn);

    /* Register TX complete IRQ handler. */
    stcIrqSigninConfig.enIRQn = USART3_TX_CPLT_IRQn;
    stcIrqSigninConfig.enIntSrc = USART3_TX_CPLT_INT_SRC;
    stcIrqSigninConfig.pfnCallback = &USART3_TxComplete_IrqCallback;
    (void)INTC_IrqSignIn(&stcIrqSigninConfig);
    NVIC_ClearPendingIRQ(stcIrqSigninConfig.enIRQn);
    NVIC_SetPriority(stcIrqSigninConfig.enIRQn, DDL_IRQ_PRIO_10);
    NVIC_EnableIRQ(stcIrqSigninConfig.enIRQn);

	USART_FuncCmd(USART3_UNIT, (USART_RX | USART_INT_RX), ENABLE);
}
uint32_t USART3_RX_Receive_order(char *data,uint32_t iAddress) 
{
	uint32_t num = 0;
	num = DMA_GetTransCount(CM_DMA1,DMA_CH3);
	USART3_RX_num = 1024-num;
	if(USART3_RX_num != USART3_RX_p)
	{
		if(USART3_RX_num > USART3_RX_p)
		{
			if((iAddress + USART3_RX_num-USART3_RX_p) > 1024)
			{
				return 0;
			}
			memcpy(data + iAddress,usart3_recv_buffer+USART3_RX_p,USART3_RX_num-USART3_RX_p);
			iAddress += USART3_RX_num-USART3_RX_p;
		}
		else
		{
			if((iAddress + 1024-USART3_RX_p) > 1024)
			{
				return 0;
			}
			memcpy(data + iAddress,usart3_recv_buffer+USART3_RX_p,1024-USART3_RX_p);
			iAddress += 1024 - USART3_RX_p;
			if((iAddress + USART3_RX_num) > 1024)
			{
				return 0;
			}
			memcpy(data + iAddress,usart3_recv_buffer,USART3_RX_num);
			iAddress += USART3_RX_num;
		}
		USART3_RX_p = USART3_RX_num;
		
	}
	return iAddress;
}
