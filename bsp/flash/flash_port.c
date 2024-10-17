/*
	flashµ×²ã´úÂë
*/

#include "main.h"


/**
  * @brief  Initializes Memory.
  * @param  None
  * @retval 0 if operation is successeful, MAL_FAIL else.
  */
flash_status_t Flash_If_Init(void)
{
  /* Unlock the internal flash */
  EFM_REG_Unlock();
  EFM_FWMC_Cmd(ENABLE);
  return FLASH_OK;
}

/**
  * @brief  De-Initializes Memory.
  * @param  None
  * @retval 0 if operation is successeful, MAL_FAIL else.
  */
flash_status_t Flash_If_DeInit(void)
{
  /* Lock the internal flash */
  EFM_FWMC_Cmd(DISABLE);
  EFM_REG_Lock();
  return FLASH_OK;
}

/**
  * @brief  Erases sector.
  * @param  Add: Address of sector to be erased.
  * @retval 0 if operation is successeful, MAL_FAIL else.
  */
flash_status_t Flash_If_Erase(uint32_t Add)
{
  /* Variable contains Flash operation status */
  EFM_SectorErase(Add);
  return FLASH_OK;
}

/**
  * @brief  Writes Data into Memory.
  * @param  src: Pointer to the source buffer. Address to be written to.
  * @param  dest: Pointer to the destination buffer.
  * @param  Len: Number of data to be written (in bytes).
  * @retval 0 if operation is successeful, MAL_FAIL else.
  */
flash_status_t Flash_If_Write(uint8_t *src, uint32_t dest_addr, uint32_t Len)
{
  /* Clear OPTVERR bit set on virgin samples */
  
	while (SET != EFM_GetStatus(EFM_FLAG_RDY)) 
	{		
	}
	EFM_Program(dest_addr, src,Len);
	
    return FLASH_OK;
}

/**
  * @brief  Reads Data into Memory.
  * @param  src: Pointer to the source buffer. Address to be written to.
  * @param  dest: Pointer to the destination buffer.
  * @param  Len: Number of data to be read (in bytes).
  * @retval return FLASH_OK.
  */
flash_status_t Flash_If_Read(uint8_t* buff, uint32_t dest_addr, uint32_t Len)
{

    uint32_t i;
    for(i = 0; i < Len; i++){
            buff[i] = *(__IO uint8_t*)(dest_addr + i);
    }
  /* Return a valid address to avoid HardFault */
  return FLASH_OK;
}



