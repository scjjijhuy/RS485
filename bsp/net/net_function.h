#ifndef __NET_FUNCTION_H__
#define __NET_FUNCTION_H__

#include "common_c.h"
#define APPLICATION_OK   			           0XA2
#define APPLICATION_UPDATE_REQUEST             0XA3
#define APPLICATION_OUTPUT_PORT_SELECT_UART    0XA5
#define APP_DESC_BUF_MAX                       32
//#define APP_DESCRIPTOR  	 "app:C200G base V1.0";
/*
	�������ýṹ��
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
	���������
*/
int net_function_service(char *buffer_in);

/*
	��������
*/
int net_function_connect(void);

/*
	��Ϣ��ȡ����
*/
int net_get_payload(struct mqtt_urc_struct *mqtt_urc_in, char *payload_out);

/*
	������Ϣ
*/
int net_parse(char *buffer_in);

/*
	ȷ����Ϣ
*/
int net_rollback_msg(char *mqtt_action, char *session_id);

/*
	���������豸����
*/
int net_recv_server_startup_buzzer(char *buffer_in);

/*
	�����������豸�Ƿ�����
*/
int net_recv_server_device_link(char *buffer_in);

/*
	���������豸����
*/
int net_recv_server_reboot(char *buffer_in);

/*
	���������豸�ϴ�����
*/
int net_recv_server_get_data(char *buffer_in);

/*
	���������豸��ȡ��Ƶ
*/
int net_recv_server_get_video(char *buffer_in);

/*
	���������豸��ʼ����
*/
int net_recv_server_task_start(char *buffer_in);

/*
	���������豸��������
*/
int net_recv_server_task_over(char *buffer_in);

/*
	���������豸�����ϴ�����
*/
int net_recv_server_open_upload(char *buffer_in);

/*
	���������豸��ֹ�ϴ�����
*/
int net_recv_server_off_upload(char *buffer_in);

/*
	ftp�ϴ�60�����ֵ
*/
int net_upload_maxdata_log(char *file_name_out);

/*
	�����������ն�������Ϣ
*/
int net_recv_server_push_config(char *buffer_in);

/*
	�����������ն���ֵ��Ϣ
*/
int net_recv_server_push_threshold(char *buffer_in);

/*
	��������������ͷ������Ϣ
*/
int net_recv_server_push_videoconfig(char *buffer_in);

/*
	���������豸�ϴ�60�����ֵ��־
*/
int net_recv_server_get_max_datalog(char *buffer_in);

/*
	ftp����60�����ֵ
*/
int net_download_maxdata_log(void);

/*
	���������豸����������Ϣ
*/
int net_recv_server_push_taskinfo(char *buffer_in);

/*
	�ն˷���ע���豸��Ϣ
*/
int net_registration_device(char *device_state);
/*
	�ն˷����������������60��������Ϣ��������
*/
int net_registration_task_device_gw(char *device_state);
/*
	�ն˷����ϴ���־��Ϣ
*/
int net_upload_log_msg(char *file_name, char *log_type);

/*
	�����ϴ�����
*/
int net_upload_data(void);
int net_upload_data_gw(uint8_t data_packet);
/*
	�ж��Ƿ񱨾�
*/
int check_if_alarm(void);

/*
	gps��λ������
*/
int net_gnss_service(void);

/*
	���������豸����60�����ֵ
*/
int net_recv_server_modify_logfile(char *buffer_in);

/*
	mqtt�޸ĵ�������
*/
int net_recv_server_modify_log(char *recv_buffer);

/*
	���Ͱ��´�ӡ��ť��ӡԶ��60����־��Ϣ
*/
int net_client_printed(void);

/*
	����������GPS����
*/
int net_recv_server_gps_switch(char *buffer_in);

/*
	������������
*/
int net_wait_reconnect(int timeout_ms_in);

/*
	��ȡ������ʱ��
*/
int net_device_get_time(void);

/*
	����������ʱ����Ϣ
*/
int net_recv_server_push_time(char *buffer_in);

/*
	���Ͱ��´�ӡ��ť��ӡ����60����־��Ϣ
*/
int net_client_local_printed(void);

/*
	���������豸�ϴ�����60�����ֵ��־
*/
int net_recv_server_get_local_max_datalog(char *buffer_in);

/*
	ftp�ϴ�����ģʽ60�����ֵ
*/
int net_upload_local_maxdata_log(char *file_name_out);

/*
	�ϴ��豸��Ϣ
*/
int net_upload_device_info(void);

/*
	���������豸�����豸��Ϣ
*/
int net_recv_server_get_device_info(char *buffer_in);

/*
	���������豸����gps����һ��ʱ��
*/
int net_recv_server_gps_switch_second(char *buffer_in);

/*
	ftp�ϴ�������־
*/
int net_upload_run_log(char *file_name_out);

/*
	ftp�ϴ��񶯲���
*/
int net_upload_wave_file(char *file_name_out);

/*
	�ն˷����ϴ�����״̬��Ϣ
*/
int net_upload_wave_msg(char *file_name, char *log_type);

/*
	���������Ʒ���������
*/
int net_recv_server_beep_switch(char *buffer_in);

/*
	������������ϵ��
*/
int net_recv_server_set_coefficient(char *buffer_in);

/*
	����������433����
*/
int net_recv_server_rf433_switch(char *buffer_in);

/*
	��������������ʱ����ģ�鿪��
*/
int net_recv_server_sleep_network_switch(char *buffer_in);

/*
	��������ȡ�豸������־
*/
int net_recv_server_get_run_log_file(char *buffer_in);

/*
	���������豸�ػ�
*/
int net_recv_server_shut_down(char *buffer_in);

/*
	���������豸���SD��
*/
int net_recv_server_clear_sd_card(char *buffer_in);

int net_recv_server_sys_upgrade_gw(char *buffer_in);
#endif

