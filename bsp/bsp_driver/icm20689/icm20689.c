#include "icm20689.h" 
#include "fmath.h"
#include "main.h"
#define EXAMPLE_SPI_MASTER_SLAVE        (SPI_MASTER)
//#define EXAMPLE_SPI_BUF_LEN             (128UL)

#define SPI_SCK_PORT                    (GPIO_PORT_A)
#define SPI_SCK_PIN                     (GPIO_PIN_09)
#define SPI_SCK_FUNC                    (GPIO_FUNC_43)

/* SPI_MOSI Port/Pin definition */
#define SPI_MOSI_PORT                   (GPIO_PORT_A)
#define SPI_MOSI_PIN                    (GPIO_PIN_08)
#define SPI_MOSI_FUNC                   (GPIO_FUNC_40)

/* SPI_MISO Port/Pin definition */
#define SPI_MISO_PORT                   (GPIO_PORT_A)
#define SPI_MISO_PIN                    (GPIO_PIN_11)
#define SPI_MISO_FUNC                   (GPIO_FUNC_41)

/* SPI_MISO Port/Pin definition */ 
#define SPI_NSS_PORT                   (GPIO_PORT_A)
#define SPI_NSS_PIN                    (GPIO_PIN_10)
//#define SPI_NSS_FUNC                   (Func_Spi1_Miso)
#define SPI_NSS_HIGH()                     GPIO_SetPins(SPI_NSS_PORT, SPI_NSS_PIN)
#define SPI_NSS_LOW()                    GPIO_ResetPins(SPI_NSS_PORT, SPI_NSS_PIN)
/* SPI unit and clock definition */
#define SPI_UNIT                        (CM_SPI1)
#define SPI_CLK                        (FCG1_PERIPH_SPI1)




//#define SPI_TX_INT_SOURCE               (INT_SPI1_SPTI)
//#define SPI_RX_INT_SOURCE               (INT_SPI1_SPRI)


/* Choose SPI master or slave mode */
#define SPI_MASTER_MODE
//#define SPI_SLAVE_MODE
//static uint8_t tx_dat[15] = {0x80 | 0x3b};
static uint8_t rx_dat[15];
static const float acc_sens = 32.0f / 65536;// * 9.8035f;
static const float gyr_sens = 0.0174532925f / 65.5f;

void SPI_CS_init(void)
{
	stc_gpio_init_t stcGpioInit;
	(void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinState = PIN_STAT_RST;
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
	 (void)GPIO_Init(SPI_NSS_PORT, SPI_NSS_PIN, &stcGpioInit);
	  SPI_NSS_HIGH();
}
/**
 *******************************************************************************
 ** \brief Configure SPI peripheral function
 **
 ** \param [in] None
 **
 ** \retval None
 **
 ******************************************************************************/
static void Spi_Config(void)
{
	  stc_spi_init_t stcSpiInit;


    /* Configure Port */
//    GPIO_SetFunc(SPI_SS_PORT,   SPI_SS_PIN,   SPI_SS_FUNC);
    GPIO_SetFunc(SPI_SCK_PORT,  SPI_SCK_PIN,  SPI_SCK_FUNC);
    GPIO_SetFunc(SPI_MOSI_PORT, SPI_MOSI_PIN, SPI_MOSI_FUNC);
    GPIO_SetFunc(SPI_MISO_PORT, SPI_MISO_PIN, SPI_MISO_FUNC);

    /* Configuration SPI */
    FCG_Fcg1PeriphClockCmd(SPI_CLK, ENABLE);
    SPI_StructInit(&stcSpiInit);
    stcSpiInit.u32WireMode          = SPI_3_WIRE;
    stcSpiInit.u32TransMode         = SPI_FULL_DUPLEX;
    stcSpiInit.u32MasterSlave       = EXAMPLE_SPI_MASTER_SLAVE;
    stcSpiInit.u32Parity            = SPI_PARITY_INVD;
    stcSpiInit.u32SpiMode           = SPI_MD_3;
    stcSpiInit.u32BaudRatePrescaler = SPI_BR_CLK_DIV2;
    stcSpiInit.u32DataBits          = SPI_DATA_SIZE_8BIT;
    stcSpiInit.u32FirstBit          = SPI_FIRST_MSB;
    stcSpiInit.u32FrameLevel        = SPI_1_FRAME;
    (void)SPI_Init(SPI_UNIT, &stcSpiInit);
    SPI_Cmd(SPI_UNIT, ENABLE);
//    stc_spi_init_t stcSpiInit;

//    /* configuration structure initialization */
//    MEM_ZERO_STRUCT(stcSpiInit);

//    /* Configuration peripheral clock */
//    PWC_Fcg1PeriphClockCmd(SPI_UNIT_CLOCK, Enable);

//    /* Configuration SPI pin */
//    PORT_SetFunc(SPI_SCK_PORT, SPI_SCK_PIN, SPI_SCK_FUNC, Disable);
//    PORT_SetFunc(SPI_MOSI_PORT, SPI_MOSI_PIN, SPI_MOSI_FUNC, Disable);
//    PORT_SetFunc(SPI_MISO_PORT, SPI_MISO_PIN, SPI_MISO_FUNC, Disable);

//    /* Configuration SPI structure */
//    stcSpiInit.enClkDiv = SpiClkDiv8;
//    stcSpiInit.enFrameNumber = SpiFrameNumber1;
//    stcSpiInit.enDataLength = SpiDataLengthBit8;
//    stcSpiInit.enFirstBitPosition = SpiFirstBitPositionMSB;
//    stcSpiInit.enSckPolarity = SpiSckIdleLevelHigh;
//    stcSpiInit.enSckPhase = SpiSckOddChangeEvenSample;
//    stcSpiInit.enReadBufferObject = SpiReadReceiverBuffer;
//    stcSpiInit.enWorkMode = SpiWorkMode3Line;
//    stcSpiInit.enTransMode = SpiTransFullDuplex;
//    stcSpiInit.enCommAutoSuspendEn = Disable;
//    stcSpiInit.enModeFaultErrorDetectEn = Disable;
//    stcSpiInit.enParitySelfDetectEn = Disable;
//    stcSpiInit.enParityEn = Disable;
//    stcSpiInit.enParity = SpiParityEven;

//#ifdef SPI_MASTER_MODE
//    stcSpiInit.enMasterSlaveMode = SpiModeMaster;
//    stcSpiInit.stcDelayConfig.enSsSetupDelayOption = SpiSsSetupDelayCustomValue;
//    stcSpiInit.stcDelayConfig.enSsSetupDelayTime = SpiSsSetupDelaySck1;
//    stcSpiInit.stcDelayConfig.enSsHoldDelayOption = SpiSsHoldDelayCustomValue;
//    stcSpiInit.stcDelayConfig.enSsHoldDelayTime = SpiSsHoldDelaySck1;
//    stcSpiInit.stcDelayConfig.enSsIntervalTimeOption = SpiSsIntervalCustomValue;
//    stcSpiInit.stcDelayConfig.enSsIntervalTime = SpiSsIntervalSck6PlusPck2;
//#endif

//#ifdef SPI_SLAVE_MODE
//    stcSpiInit.enMasterSlaveMode = SpiModeSlave;
//#endif

//    SPI_Init(SPI_UNIT, &stcSpiInit);
//    SPI_Cmd(SPI_UNIT, Enable);
}
static uint8_t spi_write_read(uint8_t reg)
{
	while (RESET == SPI_GetStatus(SPI_UNIT, SPI_FLAG_TX_BUF_EMPTY));
	SPI_WriteData(SPI_UNIT, reg);
	while (RESET == SPI_GetStatus(SPI_UNIT, SPI_FLAG_RX_BUF_FULL));
	return SPI_ReadData(SPI_UNIT);
}

static void spi_write_single_dat(unsigned char reg, unsigned char dat)
{
	SPI_NSS_LOW();
	spi_write_read(reg);
	spi_write_read(dat);
	SPI_NSS_HIGH();
}

static void icm_20689_init(void)
{
	spi_write_single_dat(0x6b,0x80);
	osDelay(100);	
	spi_write_single_dat(0x6b, 0x00);
	osDelay(100);	
	spi_write_single_dat(0x1b, 0x08);	//+-2000  0x00 +-250	0x08 +-500	0x10 +-1000 0x18 +-2000
	osDelay(100);	
	spi_write_single_dat(0x1c, 0x18);	//+-16g		0x00 +-2g		0x08 +-4g		0x10 +-8g 	0x18 +-16g
	osDelay(100);
	
	spi_write_single_dat(0x6b, 0x00);	//osc_gyr_z
	osDelay(100);	
	spi_write_single_dat(0x19, 0x00);	//update 1000hz
	osDelay(100);	
	spi_write_single_dat(0x1a, 0x00);	//gyr_filter   0x00 250hz 0.97ms   0x03 41hz 5.9ms   0x04 20hz 9.9ms
	osDelay(100);
	spi_write_single_dat(0x1d, 0x00);	//acc_filter   0x00 460hz 1.94ms   0x04 20hz 19.8ms  0x05 10hz 35.7ms   0x06 5hz 66.96ms
//	spi_write_single_dat(0x1d, 0x00);	//acc_filter   0x00 460hz 1.94ms   0x04 20hz 19.8ms  0x05 10hz 35.7ms   0x06 5hz 66.96ms

	osDelay(10);
}

void icm20689_init(void)
{
	SPI_CS_init();
	Spi_Config();
	icm_20689_init();
}
void icm_20689_data(void)
{
	uint8_t iLoop = 0;
	char u8TxBuffer = 0x80|0x3b;
	SPI_NSS_LOW();
	spi_write_read(u8TxBuffer);
	/* Wait rx buffer full */
	for(iLoop = 0;iLoop < 15;iLoop++)
	{
		rx_dat[iLoop] = spi_write_read(0x00);
	}
	SPI_NSS_HIGH();
}
void icm20689_read(float* ax, float* ay, float* az, float* gx, float* gy, float* gz)
{
    icm_20689_data();
	*ax = msb_uchar_to_short(rx_dat, 0x00) * acc_sens;
	*ay = msb_uchar_to_short(rx_dat, 0x02) * acc_sens;
	*az = msb_uchar_to_short(rx_dat, 0x04) * acc_sens;

	*gx = msb_uchar_to_short(rx_dat, 0x08) * gyr_sens;
	*gy = msb_uchar_to_short(rx_dat, 0x0a) * gyr_sens; 
	*gz = msb_uchar_to_short(rx_dat, 0x0c) * gyr_sens;

}

void icm20689_get_adc(uint8_t* adc)
{
  *adc = rx_dat[0];
  *(adc+1) = rx_dat[1];
  *(adc+2) = rx_dat[2];
  *(adc+3) = rx_dat[3];
  *(adc+4) = rx_dat[4];
  *(adc+5) = rx_dat[5];
  *(adc+6) = rx_dat[6];
  *(adc+7) = rx_dat[7];
  *(adc+8) = rx_dat[8];
  *(adc+9) = rx_dat[9];
  *(adc+10) = rx_dat[10];
  *(adc+11) = rx_dat[11];
  *(adc+12) = rx_dat[12];
  *(adc+13) = rx_dat[13];
  *(adc+14) = rx_dat[14];
}




