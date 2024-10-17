#ifndef _FLASH_FUNCTION_H_
#define _FLASH_FUNCTION_H_
#define FLASH_BASE     0x00000000
//#define STACK_TABLE_PAGE 511

//#define TASK_INFO_PAGE 506
//#define FLAG_PAGE 505
//#define THRESHOLD_PAGE 504
//#define DEVICE_CONFIG_PAGE 503
//#define DEVICE_INFO_PAGE 502
#define DEVICE_INFO_PAGE 62
#define UPLOAD_POOL_PAGE 61
/*
	将任意大小的数据写入到flash以页起始的地址中（只适用于STM32L476）
*/
int flash_write(int page_number, unsigned char *src, int length);

/*
	读取flash中的数据
*/
int flash_read(int page_number, unsigned char *buffer, int length);

/*
	保存全局数据
*/
void flash_save_global_variables(void);

/*
	读取全局数据
*/
void flash_load_global_variables(void);

/*
	清空全局数据
*/
void flash_earse_global_variables(void);
/*
    保存60组大值
*/
void write_global_log_vibration_function(void);

#endif
