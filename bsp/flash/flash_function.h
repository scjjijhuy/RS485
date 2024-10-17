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
	�������С������д�뵽flash��ҳ��ʼ�ĵ�ַ�У�ֻ������STM32L476��
*/
int flash_write(int page_number, unsigned char *src, int length);

/*
	��ȡflash�е�����
*/
int flash_read(int page_number, unsigned char *buffer, int length);

/*
	����ȫ������
*/
void flash_save_global_variables(void);

/*
	��ȡȫ������
*/
void flash_load_global_variables(void);

/*
	���ȫ������
*/
void flash_earse_global_variables(void);
/*
    ����60���ֵ
*/
void write_global_log_vibration_function(void);

#endif
