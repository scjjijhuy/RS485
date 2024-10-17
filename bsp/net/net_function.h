#ifndef __NET_FUNCTION_H__
#define __NET_FUNCTION_H__

#include "common_c.h"
#define APPLICATION_OK   			           0XA2
#define APPLICATION_UPDATE_REQUEST             0XA3
#define APPLICATION_OUTPUT_PORT_SELECT_UART    0XA5
#define APP_DESC_BUF_MAX                       32
//#define APP_DESCRIPTOR  	 "app:C200G base V1.0";
/*
	网络配置结构体
*/
struct net_config_struct
{
	char mqtt_hostname[128];
	int mqtt_port;
	char mqtt_client_id[128];
	char mqtt_user_name[128];
	char mqtt_password[128];
	char mqtt_sub_ins_topic[128];
	char mqtt_pub_upload_topic[128];
	char mqtt_pub_upload_topic_gw[128];
	char mqtt_pub_rollback_topic[128];
	char mqtt_pub_collection_topic[128];
	char mqtt_pub_collection_topic_gw[128];
	char mqtt_pub_wave_topic[128];
	char mqtt_pub_task_topic[128];
	
	char mqtt_pub_data_topic_gw[128];
};

/*
	网络服务函数
*/
int net_function_service(char *buffer_in);

/*
	网络连接
*/
int net_function_connect(void);

/*
	消息提取函数
*/
int net_get_payload(struct mqtt_urc_struct *mqtt_urc_in, char *payload_out);

/*
	解析消息
*/
int net_parse(char *buffer_in);

/*
	确认消息
*/
int net_rollback_msg(char *mqtt_action, char *session_id);

/*
	服务器让设备响铃
*/
int net_recv_server_startup_buzzer(char *buffer_in);

/*
	服务器测试设备是否在线
*/
int net_recv_server_device_link(char *buffer_in);

/*
	服务器让设备重启
*/
int net_recv_server_reboot(char *buffer_in);

/*
	服务器让设备上传数据
*/
int net_recv_server_get_data(char *buffer_in);

/*
	服务器让设备获取视频
*/
int net_recv_server_get_video(char *buffer_in);

/*
	服务器让设备开始任务
*/
int net_recv_server_task_start(char *buffer_in);

/*
	服务器让设备结束任务
*/
int net_recv_server_task_over(char *buffer_in);

/*
	服务器让设备允许上传数据
*/
int net_recv_server_open_upload(char *buffer_in);

/*
	服务器让设备禁止上传数据
*/
int net_recv_server_off_upload(char *buffer_in);

/*
	ftp上传60组最大值
*/
int net_upload_maxdata_log(char *file_name_out);

/*
	服务器推送终端配置信息
*/
int net_recv_server_push_config(char *buffer_in);

/*
	服务器推送终端阈值信息
*/
int net_recv_server_push_threshold(char *buffer_in);

/*
	服务器推送摄像头配置信息
*/
int net_recv_server_push_videoconfig(char *buffer_in);

/*
	服务器让设备上传60组最大值日志
*/
int net_recv_server_get_max_datalog(char *buffer_in);

/*
	ftp下载60组最大值
*/
int net_download_maxdata_log(void);

/*
	服务器向设备推送任务信息
*/
int net_recv_server_push_taskinfo(char *buffer_in);

/*
	终端发送注册设备信息
*/
int net_registration_device(char *device_state);
/*
	终端发送新任务开启，清除60组数据信息（国网）
*/
int net_registration_task_device_gw(char *device_state);
/*
	终端发送上传日志信息
*/
int net_upload_log_msg(char *file_name, char *log_type);

/*
	正常上传数据
*/
int net_upload_data(void);
int net_upload_data_gw(uint8_t data_packet);
/*
	判断是否报警
*/
int check_if_alarm(void);

/*
	gps定位服务函数
*/
int net_gnss_service(void);

/*
	服务器让设备下载60组最大值
*/
int net_recv_server_modify_logfile(char *buffer_in);

/*
	mqtt修改单条数据
*/
int net_recv_server_modify_log(char *recv_buffer);

/*
	发送按下打印按钮打印远程60组日志消息
*/
int net_client_printed(void);

/*
	服务器控制GPS开关
*/
int net_recv_server_gps_switch(char *buffer_in);

/*
	网络重新连接
*/
int net_wait_reconnect(int timeout_ms_in);

/*
	获取服务器时间
*/
int net_device_get_time(void);

/*
	服务器推送时间信息
*/
int net_recv_server_push_time(char *buffer_in);

/*
	发送按下打印按钮打印本地60组日志消息
*/
int net_client_local_printed(void);

/*
	服务器让设备上传本地60组最大值日志
*/
int net_recv_server_get_local_max_datalog(char *buffer_in);

/*
	ftp上传本地模式60组最大值
*/
int net_upload_local_maxdata_log(char *file_name_out);

/*
	上传设备信息
*/
int net_upload_device_info(void);

/*
	服务器让设备传输设备信息
*/
int net_recv_server_get_device_info(char *buffer_in);

/*
	服务器让设备开启gps持续一段时间
*/
int net_recv_server_gps_switch_second(char *buffer_in);

/*
	ftp上传运行日志
*/
int net_upload_run_log(char *file_name_out);

/*
	ftp上传振动波形
*/
int net_upload_wave_file(char *file_name_out);

/*
	终端发送上传波形状态信息
*/
int net_upload_wave_msg(char *file_name, char *log_type);

/*
	服务器控制蜂鸣器开关
*/
int net_recv_server_beep_switch(char *buffer_in);

/*
	服务器设置振动系数
*/
int net_recv_server_set_coefficient(char *buffer_in);

/*
	服务器控制433开关
*/
int net_recv_server_rf433_switch(char *buffer_in);

/*
	服务器控制休眠时网络模块开关
*/
int net_recv_server_sleep_network_switch(char *buffer_in);

/*
	服务器拉取设备运行日志
*/
int net_recv_server_get_run_log_file(char *buffer_in);

/*
	服务器让设备关机
*/
int net_recv_server_shut_down(char *buffer_in);

/*
	服务器让设备清空SD卡
*/
int net_recv_server_clear_sd_card(char *buffer_in);

int net_recv_server_sys_upgrade_gw(char *buffer_in);
#endif

