#ifndef __NET_PORT_H__
#define __NET_PORT_H__

#include "common_c.h"

/*
	ʱ��ṹ��
*/
//struct time_struct
//{
//	unsigned char year;
//	unsigned char month;
//	unsigned char day;
//	unsigned char hour;
//	unsigned char minute;
//	unsigned char second;
//	unsigned char weekday;
//};

/*
	mqtt����
*/
struct mqtt_config_struct
{
	//client_idx
	int client_idx;
	
	//MQTT�汾��
	int mqtt_version;//3����v3.1, 4����v3.1.1,Ĭ��3
	
	//PDP������ID
	int pdpcid;//1~16,Ĭ��1
	
	//��ʱʱ��
	int pkt_timeout;//1~60,Ĭ��5��
	int retry_times;//0~10,Ĭ��3��
	int timeout_notice;//1���泬ʱ��0�����棬Ĭ��0
	
	//�Ự����
	int clean_session;//Ϊ1������Ϊ0���֣�Ĭ��Ϊ1
	
	//����ʱ��
	int keep_alive_time;//Ϊ0��Զ�������ӣ�����Ϊ�Ͽ�������0~3600��Ĭ��Ϊ120��
	
	//SSL
	int ssl_enable;//Ϊ0ʹ����ͨTCP��Ϊ1ʹ��SSL��Ĭ��Ϊ0
	int ssl_ctx_idx;//ssl������ID��0~5
	
	//����ģʽ
	int msg_recv_mode;//Ϊ0����URC����ʾ��Ϊ1���ᣬĬ��Ϊ0
	int msg_len_enable;//Ϊ0����ʾ���ճ��ȣ�Ϊ1��ʾ��Ĭ��Ϊ1
	
};

/*
	Ƶ��qos�ṹ��
*/
struct topic_qos_struct
{
	char topic[256];
	int qos;
};

/*
	Ƶ���ṹ��
*/
struct topic_struct
{
	char topic[256];
};

/*
	mqtt�洢�ṹ��
*/
struct mqtt_storage_struct
{
	int client_idx;
	int storage_flag[5];
};

/*
	mqtt��Ϣ�ṹ��
*/
struct mqtt_message_struct
{
	int msg_id;
	char topic[256];
	int payload_len;
	char payload[1024];
};

/*
	mqtt urc �ṹ��
*/
struct mqtt_urc_struct
{
	int state_flag;
	int state_client_idx;
	int state_code;
	
	int receive_flag;
	int receive_client_idx;
	int receive_recv_mode;
	struct mqtt_message_struct receive_mqtt_message;
	int receive_storage_id;
	
	int ping_flag;
	int ping_client_idx;
	int ping_code;
};

/*
	ftp���ýṹ��
*/
struct ftp_config_struct
{
	//ip port
	char hostname[64];
	int port;
	
	// path
	char max_data_path[64];
	char run_log_path[64];
	char download_path[64];
	char download_filename[64];
	char wave_path[64];
	
	//�û����������255�ֽ�
	char username[256];
	char password[256];
	
	//file_type 0�Ƕ����ƣ�1��ASCIIĬ����0
	int filetype;
	
	//0������ģʽ����ģ������˿�,1�Ǳ���ģʽ����FTP�����������˿�Ĭ����0
	int transmode;
	
	//PDP id ��Χ1-16 Ĭ����1
	int contextID;
	
	//��ʱʱ�䣬20-180 Ĭ��90
	int rsptimeout;
	
	//0��FTP,1��FTPS,Ĭ����0
	int ssltype;
	
	//ѡ��SSL id Ĭ����0����ѡ0-5
	int sslctxid;
	
	//0 ʹ�÷������ַ��ĵ�ַ,1 ʹ�÷��������ƻỰ��ַ,Ĭ��0
	int data_address;
};

/*
	EC20����
*/
int net_ec20_restart(int timeout_ms);

/*
	EC20����
*/
int net_ec20_power(int on_off);

/*
	����ָ�����Ƿ񷵻�OK
*/
int net_at_send(char *send_buffer ,int timeout_ms);

/*
	AT+QMTCFG ����MQTT
*/
int net_mqtt_configrate(int client_idx, struct mqtt_config_struct *mqtt_config);

/*
	��MQTT�������Ӻ���
*/
int net_mqtt_open(int client_idx,char *host_name, int port);

/*
	�ر�MQTT���Ӻ���
*/
int net_mqtt_close(int client_idx);

/*
	����һ���ͻ��˵�MQTT������
*/
int net_mqtt_connect(int client_idx, char *client_ID, char *username, char *password);

/*
	��MQTT�������Ͽ�һ���ͻ���
*/
int net_mqtt_disconnect(int client_idx);

/*
	����Ƶ��
*/
int net_mqtt_subscribe(int client_idx, int msgID, int topic_amount, struct topic_qos_struct *topic_qos_array);

/*
	ȡ������
*/
int net_mqtt_unsubscribe(int client_idx,int msgID, int topic_amount, struct topic_struct *topic_array);

/*
	������Ϣ
*/
int net_mqtt_publish(int client_idx, int msgID, int qos, int retain, char *topic, int msg_length , char *payload, int beep_in);

/*
	�鿴�������Ϣ����
*/
int net_mqtt_receive_check(int *amount_out, struct mqtt_storage_struct mqtt_storage_array_out[6]);

/*
	��ȡһ����Ϣ
*/
int net_mqtt_receive_read(int client_idx, struct mqtt_message_struct *mqtt_message_out);

/*
	URC��������ѭ��
*/
int net_mqtt_urc(char *buffer_in, struct mqtt_urc_struct *mqtt_urc_out);

/*
	��ȡ����ʱ��
*/
int net_get_time(struct time_struct *time_out);

/*
	��ȡ�ź�ǿ��
	���ĸ��źţ�4��ǿ��0û��
*/
int net_signal_quality(int *quality_out);

/*
	AT+QFTPCFG ���� FTP
*/
int net_ftp_configrate(struct ftp_config_struct *ftp_confing_in);

/*
	��FTP�������Ӻ���
*/
int net_ftp_open(char *host_name, int port);

/*
	�ر�ftp����
*/
int net_ftp_close(void);

/*
	���õ�ǰFTP·��
*/
int net_ftp_cwd(char *path_name_in);

/*
	ɾ���ļ�
*/
int net_ftp_del(char *file_name_in);

/*
	�����ļ�
*/
int net_ftp_get(char *file_name_in, char *local_name_in, int startpos_in, int downloadlen_in, char *buffer_out);

/*
	�����ļ���
*/
int net_ftp_mkdir(char *folder_name_in);

/*
	FTP�ϴ��ļ�(���ڷ�ʽ)
*/
int net_ftp_put_COM(char *file_name_in, int startpos_in, int uploadlen_in, int beof_in, char*payload_in);

/*
	FTP�ϴ��ļ�(�ļ���ʽ)
*/
int net_ftp_put_FILE(char *file_name_in, char *local_name_in, int startpos_in);

/*
	��ȡ��ǰFTP·��
*/
int net_ftp_pwd(char *path_name_out);

/*
	ɾ���ļ���
*/
int net_ftp_rmdir(char *folder_name_in);

/*
	����ļ�����
*/
int net_ftp_size(char *file_name_in, int *filesize_out);

/*
	���ftp������״̬
*/
int net_ftp_stat(int *ftpstat_out);

/*
	��ȡ��վ��
*/
int net_get_base_station(char *lac_out, char *ci_out);

/*
	�鿴�Ƿ�����������
*/
int net_check_packet_domain(int *state_out);

/*
	�鿴MQTT����״̬
*/
int net_check_mqtt_connect(int *state_out);

/*
	�鿴ICCID
*/
int net_get_iccid(char *iccid_out);
/*
	�鿴EC20F/EC200U ID 
*/
int net_get_device_ID(char *iccid_out);

int net_phonebook_num(char *num);

int net_short_message_device(char *device_state,char *da);
#endif

