#ifndef _FLASH_PORT_H_
#define _FLASH_PORT_H_

/*
	flash底层函数头文件
	作者：lee
	时间：2019/5/8
*/
#include "common_c.h"

typedef enum 
{
	FLASH_OK=0,
	FLASH_ERR=1,
	FLASH_CHECK_ERR=2,
}flash_status_t;


/**
  * @brief  Initializes Memory.
  * @param  None
  * @retval 0 if operation is successeful, MAL_FAIL else.
  */
flash_status_t Flash_If_Init(void);

/**
  * @brief  De-Initializes Memory.
  * @param  None
  * @retval 0 if operation is successeful, MAL_FAIL else.
  */
flash_status_t Flash_If_DeInit(void);

/**
  * @brief  Erases sector.
  * @param  Add: Address of sector to be erased.
  * @retval 0 if operation is successeful, MAL_FAIL else.
  */
flash_status_t Flash_If_Erase(uint32_t Add);

/**
  * @brief  Writes Data into Memory.
  * @param  src: Pointer to the source buffer. Address to be written to.
  * @param  dest: Pointer to the destination buffer.
  * @param  Len: Number of data to be written (in bytes).
  * @retval 0 if operation is successeful, MAL_FAIL else.
  */
flash_status_t Flash_If_Write(uint8_t *src, uint32_t dest_addr, uint32_t Len);

/**
  * @brief  Reads Data into Memory.
  * @param  src: Pointer to the source buffer. Address to be written to.
  * @param  dest: Pointer to the destination buffer.
  * @param  Len: Number of data to be read (in bytes).
  * @retval return FLASH_OK.
  */
flash_status_t Flash_If_Read(uint8_t* buff, uint32_t dest_addr, uint32_t Len);

#endif
