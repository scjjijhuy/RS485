#ifndef _COMMON_C_H_
#define _COMMON_C_H_
/*
	公用头文件
	作者：lee
	时间：2019/12/19
*/

#include "hc32_ll.h"

/*------------------------------宏定义-----------------------------------*/
#define STACK_SIZE 50
#define RFFT_LEN (256)

#define FILTER_ENABLE (1)

#define DEBUG
#ifndef DEBUG
#define printf(...)
#endif

//#define BATTERY_8_4
#ifdef  BATTERY_8_4
#define BATTERY_HIGH_V (8400)
#define BATTERY_LOW_V	 (7500)
#else
#define BATTERY_HIGH_V (4120)
#define BATTERY_LOW_V	 (3250)
#endif

enum ch376_mode_enum
{
	USB_MODE=1,
	SD_MODE=2,
};

enum led_color_enum
{
	LED_OFF=0,
	LED_RED=1,
	LED_GREEN=2,
	LED_ORANGE=3,
};

/*------------------------------结构体定义-----------------------------------*/
typedef unsigned int time_t;     /* date/time in unix secs past 1-Jan-70 */
struct time_struct
{
	unsigned char year;
	unsigned char month;
	unsigned char day;
	unsigned char hour;
	unsigned char minute;
	unsigned char second;
	unsigned char weekday;
	unsigned char sign;
};
/*
	时间结构体
*/
struct gps_time_struct
{
	unsigned char hour;
	unsigned char minute;
	unsigned char second;
};

/*
	日期结构体
*/
struct gps_date_struct
{
	unsigned char year;
	unsigned char month;
	unsigned char day;
};

/*
	led控制结构体
*/
struct led_control_struct
{
	int color;
	int mode;
};

/*
	全局flag集合结构体
*/
struct flag_struct
{
	int sleep_flag;
	int power_button_flag;
	int awaken_flag;
	int gps_on_flag;
	int flash_busy_flag;
	int CENTER_reset_flag;
	int config_video_ftp;
	int upload_enable_flag;
	int upload_video_flag;
	int alarm_type;
	int CENTER_state_flag;
	int system_restart_flag;
	int upload_data_flag;
	int net_get_time_flag;
	int filter_flag;
	int beep_flag;
	int button_start_up_flag;//按下开机按钮的标志，发送完开机信息后清零，需要保存到flash
	int send_print_message_flag;//打印完毕发送信息标志
	int led_lock_flag;
	int local_vibration_log_busy_flag;
	int print_local_mode_flag;
	int battery_low_flag;
	int vibration_remote_log_busy_flag;
	int need_upload_wave;
	int print_run_log_flag;
	int net_power_on;
	int already_print_flag;//打印过日志的标志
	int wait_first_upload_flag;//开机等待发送第一条数据
	int net_shutdown_flag;//远程关机标志
	int task_on_sign;
	int start_up_sign;
};

/*
	全局定时器结构体
*/
struct timer_struct
{
	unsigned int system_timer;
	unsigned int gps_timer_sec;
	unsigned int mpu_timer_ms;
	unsigned int net_timer_ms;
	unsigned int sleep_timer_sec;
	unsigned int live_timer_sec;
	unsigned int gnss_timer_ms;
	unsigned int upload_interval_timer_ms;
	unsigned int get_video_timer_sec;
	unsigned int pressure_timer_sec;
	unsigned int beep_timer;
	unsigned int local_log_timer_sec;
	unsigned int get_time_sec;
	unsigned int mpu_test_timer_ms;
	unsigned int net_power_interval_sec;
	unsigned int system_timer_sec;
};

/*
	gps输出结果结构体
*/
struct gps_out_struct
{
	struct gps_time_struct time;
	char enable_state[2];
	double latitude;
	char NS[2];
	double longitude;
	char EW[2];
	double speed;
	double direction;
	struct gps_date_struct date;
	double magnetic_declination;
	char magnetic_EW[2];
	char ADEN[2];
	int valid;
	double max_speed;
	double last_speed;
};

/*
	rtc设置时间结构体
*/
struct rtc_set_time_struct
{
	int need_set;
	struct time_struct time_for_set;
};

/*
	设备信息结构体
*/
struct device_info_struct
{
	struct time_struct device_time;                          //8
	unsigned int time_stamp;								 //4						
	double tempurature;										 //8
	double humidity;										 //8
	int dump_energy;										 //4
	char terminal_sn[64];									 //64
	char uid[64];											 //64
	int signal_quality;										 //4
	struct time_struct start_up_time;//开机日期和时间		 //8
	char iccid[32];											 //32
	double battery_voltage;									 //8
	char Version_number[10];								 //10
	char device_ID[8];                  //EC20F/EC200U       //8
	char phone_number[11];
};

/*
	mpu输出结构体
*/
struct mpu_out_struct
{
	//时间
	char year;
	char month;
	char day;
	char hour;
	char minute;
	char second;
	int ms;
	
	//加速度
	double accel_x;
	double accel_y;
	double accel_z;
	
	//角速度
	double gyro_x;
	double gyro_y;
	double gyro_z;
	
	//磁场
	double magnetic_x;
	double magnetic_y;
	double magnetic_z;
	
	//角度
	double angle_x;
	double angle_y;
	double angle_z;
	
	//模块温度
	double temperature;
	
	//端口状态
	int D1_status;
	int D2_status;
	int D3_status;
	int D4_status;
	
	//气压
	double air_pressure;
	
	//高度
	double height;
	
	//经度
	double longitude;
	
	//纬度
	double latitude;
	
	//GPS高度
	double GPS_height;
	
	//GPS航向角
	double GPS_yaw;
	
	//GPS地速
	double GPS_velocity;
	
	//四元数
	double Q0;
	double Q1;
	double Q2;
	double Q3;
	
	//传感器号
	int mpu_num;
};

/*
	mpu信息结构体
*/
struct mpu_info_struct
{
	int fifo_count;
	
	double vibration_x;
	double vibration_y;
	double vibration_z;
	double vibration_max_x;
	double vibration_max_y;
	double vibration_max_z;
	struct time_struct vibration_max_time;
	uint32_t vibration_freq_x;
	uint32_t vibration_freq_y;
	uint32_t vibration_freq_z;
	uint32_t vibration_freq_norm;
	float32_t vibration_wave_x[RFFT_LEN];
	float32_t vibration_wave_y[RFFT_LEN];
	float32_t vibration_wave_z[RFFT_LEN];
	int wave_save_done;
	
	double lean_angle_x;
	double lean_angle_y;
	double lean_angle_z;
	double lean_angle_max_x;
	double lean_angle_max_y;
	double lean_angle_max_z;
	struct time_struct lean_angle_max_time;
	
};

/*
	上传数据结构体
*/
struct upload_data_struct
{
	struct time_struct time;      //8
	unsigned int ms;              //4
	double latitude;              //8
	char NS[4];					  //4
	double longitude;			  //8
	char EW[4];                   //4
//	double vibration_x;           //8
//	double vibration_y;           //8
//	double vibration_z;           //8
//	double lean_angle_x;          //8
//	double lean_angle_y;          //8
//	double lean_angle_z;
//	double temperature;           //8
//	double humidity;              //8
	int dump_energy;              //4
//	double pressure;              //8
	int location_mode;            //4
	char lac[32];								  //32
	char ci[32];								  //32
	double speed;								  //8
//	uint32_t vibration_frequency_x;//4
//	uint32_t vibration_frequency_y;//4
//	uint32_t vibration_frequency_z;//4
	int signal;         //2022     //4
};

/*
	任务信息结构体
*/
struct task_info_struct
{
	char task_num[64];
	int task_status;
	struct time_struct task_start_time;
	struct time_struct task_end_time;
};

/*
	摄像头信息结构体
*/
struct camera_info_struct
{
	char camera_ftp_address[32];
	int camera_ftp_port;
	char camera_ftp_user[32];
	char camera_ftp_password[32];
	char camera_ftp_folder[32];
	
	uint8_t rf433_channel;
	
	struct time_struct time;
	int ms;
	double vx_max;
	double vy_max;
	double vz_max;
};

/*
	振动日志结构体
*/
struct log_struct_vibration
{
	struct time_struct time;
	
	double data_x;
	double data_y;
	double data_z;
	double max_key;
	
	double latitude;
	double longitude;
};
struct global_log_vibration_struct
{
	  struct log_struct_vibration global_log_vibration[61];
	  int global_log_vibration_sign;
};
struct global_local_log_vibration_struct
{
	  struct log_struct_vibration global_local_log_vibration[61];
	  int global_local_log_vibration_sign;
};
/*
	压力信息结构体
*/
struct pressure_info_struct
{
	double pressure;
};

/*
	栈结构体
*/
struct stack_struct
{
	int front;
	int rear;
	int size;
	int length;
	int sign;
};

/*
	设备配置结构体
*/
struct device_config_struct
{
	int sleep_time;
	int awake_time;
	int interval;
	
	int gnss_mode;
	int gnss_open_time;
	int gnss_close_time;
	
	int gnss_once_flag;
	int gnss_once_time;
	
	int beep_mode;
	float vibration_coefficient;
	
	float vibration_1_x_a;
	float vibration_1_x_b;
	float vibration_1_y_a;
	float vibration_1_y_b;
	float vibration_1_z_a;
	float vibration_1_z_b;
	
	float vibration_2_x_a;
	float vibration_2_x_b;
	float vibration_2_y_a;
	float vibration_2_y_b;
	float vibration_2_z_a;
	float vibration_2_z_b;
	
	int rf433_mode;
	int sleep_network;
	
	int net_power_interval;
};

/*
	阈值结构体
*/
struct threshold_struct
{
	//电量阈值
	int dump_energy;
	
//	//温度阈值
//	double temperature_high;
//	double temperature_low;
//	
//	//倾角阈值
//	double lean_angle_x;
//	double lean_angle_y;
//	
//	//压力阈值
//	double pressure_high;
//	double pressure_low;
//	
//	//震动阈值
//	double vibration_x;
//	double vibration_y;
//	double vibration_z;
//	
	//湿度阈值
//	double humidity;
	
	//速度阈值
	double speed;
};
/*
	存储设备信息结构体
*/
struct flash_save_device_info_struct
{
	struct stack_struct         flash_upload_stack;
	struct task_info_struct     flash_task_info;
	struct flag_struct          flash_flag;
	struct threshold_struct     flash_threshold;
	struct device_config_struct flash_device_config;
	struct device_info_struct   flash_device_info;
};
/*
	蓝牙信息结构体
*/
struct bluetooth_info_struct
{
	double vibration_max_x;
	double vibration_max_y;
	double vibration_max_z;
};

/*
	休眠结构体
*/
struct sleep_struct
{
	int main_thread_sleep_done;
	int led_beep_thread_sleep_done;
	int mpu_thread_sleep_done;
	int aht10_thread_sleep_done;
	int gps_thread_sleep_done;
	int rtc_thread_sleep_done;
	int bluetooth_thread_sleep_done;
	int net_thread_sleep_done;
	int si4438_thread_sleep_done;
};

/*
	本地mpu信息结构体
*/
struct local_mpu_info_struct
{
	double vibration_max_x;
	double vibration_max_y;
	double vibration_max_z;
	struct time_struct vibraion_max_time;
	double vibration_max_latitude;
	double vibration_max_longitude;
};

/*
	ch376相关结构体
*/
struct ch376_struct
{
	int error;
	char sd_buffer[1024];
	int sd_need_write;
	int sd_need_clear;
	int usb_connect_state;
	int busy;
};
struct remote_upgrade_info_struct
{
	//需要升级标志，0不需要，1需要
	int need_upgrade_flag;
	
	uint32_t upgrade_len;
	
	char upgrade_mid[20];
	
	char upgrade_jobId[20];
	//远程升级URL
	char upgrade_url[256];
	
	//远程升级app的版本号
	char upgrade_version[64];
	
	//远程升级包的MD5
	char upgrade_md5[64];
	
	char upgrade_timestamp[20];
	
};

/*------------------------------全局变量声明-----------------------------------*/

/*
	led结构体
*/
extern struct led_control_struct global_led_run;
extern struct led_control_struct global_led_gps;
extern struct led_control_struct global_led_gprs;

/*
	全局flag集合结构体
*/
extern struct flag_struct global_flag;

/*
	全局定时器结构体
*/
extern struct timer_struct global_timer;

/*
	gps输出结果结构体
*/
extern struct gps_out_struct global_gps_out;
void global_gps_out_print(void);

/*
	rtc设置时间结构体
*/
extern struct rtc_set_time_struct global_rtc_set_time;
void global_rtc_set_time_print(void);

/*
	设备信息结构体
*/
extern struct device_info_struct global_device_info;
void global_device_info_print(void);

/*
	mpu输出结构体
*/
extern struct mpu_out_struct global_mpu_out;
void global_mpu_out_print(void);

/*
	mpu信息结构体
*/
extern struct mpu_info_struct global_mpu_info;
void global_mpu_info_print(void);

/*
	mqtt配置
*/
extern struct mqtt_config_struct global_mqtt_config;

/*
	网络配置结构体
*/
extern struct net_config_struct global_net_config;

/*
	上传数据结构体
*/
extern struct upload_data_struct global_upload_data;

/*
	任务信息结构体
*/
extern struct task_info_struct global_task_info;
void global_task_info_print(void);

/*
	摄像头信息结构体
*/
extern struct camera_info_struct global_camera_info;
void global_camera_info_print(void);

/*
	振动日志结构体
*/
extern struct global_log_vibration_struct global_log_vibration;
void global_log_vibration_print(void);

/*
	压力信息结构体
*/
extern struct pressure_info_struct global_pressure_info;
void global_pressure_info_print(void);

/*
	上传数据的栈
*/
extern struct stack_struct global_upload_stack;
void global_upload_stack_print(void);

/*
	上传数据栈池
*/
extern struct upload_data_struct global_upload_pool[STACK_SIZE];

/*
	ftp配置
*/
extern struct ftp_config_struct global_ftp_config;

/*
	设备配置结构体
*/
extern struct device_config_struct global_device_config;

/*
	阈值结构体
*/
extern struct threshold_struct global_threshold;

/*
	蓝牙信息结构体
*/
extern struct bluetooth_info_struct global_bluetooth_info;
void global_bluetooth_info_print(void);

/*
	打印需要的相关信息结构体
*/
extern struct printer_text_info_struct global_printer_text_info;

/*
	休眠结构体
*/
extern struct sleep_struct global_sleep;

/*
	EC20 GNSS配置结构体
*/
extern struct gnss_config_struct global_gnss_config;
void global_gnss_out_print(void);

/*
	EC20 GNSS 输出结构体
*/
extern struct gnss_out_struct global_gnss_out;

/*
	本地振动日志结构体
*/
extern struct global_local_log_vibration_struct global_local_log_vibration;
void global_local_log_vibration_print(void);

/*
	本地mpu信息结构体
*/
extern struct local_mpu_info_struct global_local_mpu_info;

/*
	ch376相关结构体
*/
extern struct ch376_struct global_ch376;

extern struct flash_save_device_info_struct  flash_save_device_info;

/*----------------------------公共函数声明--------------------------------*/

/*
		选出3个double型数据的绝对值最大值（内部用）
*/
double double_3_abs_max(double v1, double v2, double v3);

/*
	找出几个数的最大值
	total:待比较的数量
	sign：0 绝对值比较，1 有符号比较
	in_array: 输入数组
	out:输出的一个数
*/
int common_find_max(int total, int sign, double *in_array, double *out);

/*
	等待串口接收完毕
*/
void common_wait_uart_receive(char *buffer_in);

/*
	16位 modbus CRC 计算
*/
uint16_t system_crc16( uint8_t *arr_buff,int len);

/*
	设备重启
*/
void system_restart(uint8_t iSign);

/*
	时间先后比较函数
*/
int compare_time(struct time_struct time_1_in, struct time_struct time_2_in, int *greater_out);

/*
	将时间转换成时间戳
*/
time_t time_2_stamp(struct time_struct time_in);

extern struct remote_upgrade_info_struct global_remote_upgrade_info;
#endif



