#ifndef __BSP_I2C_H
#define	__BSP_I2C_H

#include "main.h"
/* Define I2C unit used for the example */
#define I2C_UNIT                        (CM_I2C3)
#define I2C_FCG_USE                     (FCG1_PERIPH_I2C3)
/* Define slave device address for example */
#define DEVICE_ADDR                     (0x32U)
/* If I2C 10 bit address, open the define for I2C_10BITS_ADDR */
//#define I2C_10BITS_ADDR               (1U)

/* Define port and pin for SDA and SCL */
#define I2C_SCL_PORT                    (GPIO_PORT_B)
#define I2C_SCL_PIN                     (GPIO_PIN_08)
#define I2C_SDA_PORT                    (GPIO_PORT_B)
#define I2C_SDA_PIN                     (GPIO_PIN_09)
#define I2C_GPIO_SCL_FUNC               (GPIO_FUNC_49)
#define I2C_GPIO_SDA_FUNC               (GPIO_FUNC_48)

#define TIMEOUT                         (0x40000UL)

/* Define Write and read data length for the example */
#define TEST_DATA_LEN                   (256U)
/* Define i2c baudrate */
#define I2C_BAUDRATE                    (400000UL)

int32_t I2C_Master_Transmit(uint16_t u16DevAddr, uint8_t const au8Data[], uint32_t u32Size, uint32_t u32Timeout);
int32_t I2C_Master_Receive(uint16_t u16DevAddr, uint8_t au8Data[], uint32_t u32Size, uint32_t u32Timeout);
int32_t Master_Initialize(void);

int32_t I2C_Master_Receive_Simplified(uint16_t u16DevAddr, uint8_t addr_and_mode,uint8_t au8Data[], uint32_t u32Size, uint32_t u32Timeout);
#endif /* __BSP_I2C_H */
