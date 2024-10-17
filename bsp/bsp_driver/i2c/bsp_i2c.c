//*********************************************************************************
//刘华科 20220104  
//*********************************************************************************	
// 文件名称：bsp_i2c.c
#include "main.h"


/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/

/**
 * @brief  Master transmit data
 *
 * @param  [in] u16DevAddr          The slave address
 * @param  [in] au8Data             The data array
 * @param  [in] u32Size             Data size
 * @param  [in] u32Timeout          Time out count
 * @retval int32_t:
 *            - LL_OK:              Success
 *            - LL_ERR_TIMEOUT:     Time out
 */
int32_t I2C_Master_Transmit(uint16_t u16DevAddr, uint8_t const au8Data[], uint32_t u32Size, uint32_t u32Timeout)
{
    int32_t i32Ret;
 
    I2C_SWResetCmd(I2C_UNIT, ENABLE);
	I2C_Cmd(I2C_UNIT, ENABLE);
    I2C_SWResetCmd(I2C_UNIT, DISABLE);
    i32Ret = I2C_Start(I2C_UNIT, u32Timeout);
    if (LL_OK == i32Ret) {
#ifdef I2C_10BITS_ADDR
        i32Ret = I2C_Trans10BitAddr(I2C_UNIT, u16DevAddr, I2C_DIR_TX, u32Timeout);
#else
        i32Ret = I2C_TransAddr(I2C_UNIT, u16DevAddr, I2C_DIR_TX, u32Timeout);
#endif

        if (LL_OK == i32Ret) {
            i32Ret = I2C_TransData(I2C_UNIT, au8Data, u32Size, u32Timeout);
        }
    }

    (void)I2C_Stop(I2C_UNIT, u32Timeout);
    I2C_Cmd(I2C_UNIT, DISABLE);

    return i32Ret;
}

/**
 * @brief  Master receive data
 *
 * @param  [in] u16DevAddr          The slave address
 * @param  [in] au8Data             The data array
 * @param  [in] u32Size             Data size
 * @param  [in] u32Timeout          Time out count
 * @retval int32_t:
 *            - LL_OK:              Success
 *            - LL_ERR_TIMEOUT:     Time out
 */
int32_t I2C_Master_Receive(uint16_t u16DevAddr, uint8_t au8Data[], uint32_t u32Size, uint32_t u32Timeout)
{
    int32_t i32Ret;

    I2C_Cmd(I2C_UNIT, ENABLE);
    I2C_SWResetCmd(I2C_UNIT, ENABLE);
    I2C_SWResetCmd(I2C_UNIT, DISABLE);
    i32Ret = I2C_Start(I2C_UNIT, u32Timeout);
    if (LL_OK == i32Ret) {
        if (1UL == u32Size) {
            I2C_AckConfig(I2C_UNIT, I2C_NACK);
        }

#ifdef I2C_10BITS_ADDR
        i32Ret = I2C_Trans10BitAddr(I2C_UNIT, u16DevAddr, I2C_DIR_RX, u32Timeout);
#else
        i32Ret = I2C_TransAddr(I2C_UNIT, u16DevAddr, I2C_DIR_RX, u32Timeout);
#endif
        if (LL_OK == i32Ret) {
            i32Ret = I2C_MasterReceiveDataAndStop(I2C_UNIT, au8Data, u32Size, u32Timeout);
        }

        I2C_AckConfig(I2C_UNIT, I2C_ACK);
    }

    if (LL_OK != i32Ret) {
        (void)I2C_Stop(I2C_UNIT, u32Timeout);
    }
    I2C_Cmd(I2C_UNIT, DISABLE);
    return i32Ret;
}
int32_t I2C_Master_Receive_Simplified(uint16_t u16DevAddr, uint8_t addr_and_mode,uint8_t au8Data[], uint32_t u32Size, uint32_t u32Timeout)
{
    int32_t i32Ret;

    I2C_SWResetCmd(I2C_UNIT, ENABLE);
	I2C_Cmd(I2C_UNIT, ENABLE);
    I2C_SWResetCmd(I2C_UNIT, DISABLE);
    i32Ret = I2C_Start(I2C_UNIT, u32Timeout);
    if (LL_OK == i32Ret) 
	{

#ifdef I2C_10BITS_ADDR
        i32Ret = I2C_Trans10BitAddr(I2C_UNIT, u16DevAddr, I2C_DIR_TX, u32Timeout);
#else
        i32Ret = I2C_TransAddr(I2C_UNIT, u16DevAddr, I2C_DIR_TX, u32Timeout);
#endif
		if (LL_OK != i32Ret) 
		{
			return i32Ret;
		}
		i32Ret = I2C_TransData(I2C_UNIT, &addr_and_mode, 1, u32Timeout);
		if (LL_OK != i32Ret) 
		{
			return i32Ret;
		}
		i32Ret = I2C_Restart(I2C_UNIT, u32Timeout);
		if (LL_OK == i32Ret)
		{
#ifdef I2C_10BITS_ADDR
			i32Ret = I2C_Trans10BitAddr(I2C_UNIT, u16DevAddr, I2C_DIR_RX, u32Timeout);
#else
			i32Ret = I2C_TransAddr(I2C_UNIT, u16DevAddr, I2C_DIR_RX, u32Timeout);
#endif			
			i32Ret = I2C_ReceiveData(I2C_UNIT,au8Data,u32Size,u32Timeout);
			i32Ret = I2C_Stop(I2C_UNIT, u32Timeout);
			if(LL_OK != i32Ret)
			{
				return LL_ERR;
			}
		}	
		else
		{
			return LL_ERR;
		}
    }

    if (LL_OK != i32Ret) {
		I2C_AckConfig(I2C_UNIT, I2C_ACK); 
        (void)I2C_Stop(I2C_UNIT, u32Timeout);
    }
    I2C_Cmd(I2C_UNIT, DISABLE);
    return i32Ret;
}
/**
 * @brief  Initialize the I2C peripheral for master
 * @param  None
 * @retval int32_t:
 *            - LL_OK:                  Success
 *            - LL_ERR_INVD_PARAM:      Invalid parameter
 */
int32_t Master_Initialize(void)
{
    int32_t i32Ret;
    stc_i2c_init_t stcI2cInit;
    float32_t fErr;
	
	GPIO_SetFunc(I2C_SCL_PORT, I2C_SCL_PIN, I2C_GPIO_SCL_FUNC);
    GPIO_SetFunc(I2C_SDA_PORT, I2C_SDA_PIN, I2C_GPIO_SDA_FUNC);
	FCG_Fcg1PeriphClockCmd(I2C_FCG_USE, ENABLE);
    I2C_DeInit(I2C_UNIT);

    (void)I2C_StructInit(&stcI2cInit);
    stcI2cInit.u32ClockDiv = I2C_CLK_DIV2;
    stcI2cInit.u32Baudrate = I2C_BAUDRATE;
    stcI2cInit.u32SclTime = 3UL;
    i32Ret = I2C_Init(I2C_UNIT, &stcI2cInit, &fErr);

    I2C_BusWaitCmd(I2C_UNIT, ENABLE);

    return i32Ret;
}






	
/*********************************************END OF FILE**********************/
