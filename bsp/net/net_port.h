#ifndef __NET_PORT_H__
#define __NET_PORT_H__

#include "common_c.h"

/*
	时间结构体
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
	mqtt配置
*/
struct mqtt_config_struct
{
	//client_idx
	int client_idx;
	
	//MQTT版本号
	int mqtt_version;//3代表v3.1, 4代表v3.1.1,默认3
	
	//PDP上下文ID
	int pdpcid;//1~16,默认1
	
	//超时时间
	int pkt_timeout;//1~60,默认5秒
	int retry_times;//0~10,默认3次
	int timeout_notice;//1报告超时，0不报告，默认0
	
	//会话保持
	int clean_session;//为1丢弃，为0保持，默认为1
	
	//心跳时间
	int keep_alive_time;//为0永远保持连接，否则为断开的秒数0~3600，默认为120秒
	
	//SSL
	int ssl_enable;//为0使用普通TCP，为1使用SSL，默认为0
	int ssl_ctx_idx;//ssl上下文ID，0~5
	
	//接受模式
	int msg_recv_mode;//为0会在URC中显示，为1不会，默认为0
	int msg_len_enable;//为0不显示接收长度，为1显示，默认为1
	
};

/*
	频道qos结构体
*/
struct topic_qos_struct
{
	char topic[256];
	int qos;
};

/*
	频道结构体
*/
struct topic_struct
{
	char topic[256];
};

/*
	mqtt存储结构体
*/
struct mqtt_storage_struct
{
	int client_idx;
	int storage_flag[5];
};

/*
	mqtt消息结构体
*/
struct mqtt_message_struct
{
	int msg_id;
	char topic[256];
	int payload_len;
	char payload[1024];
};

/*
	mqtt urc 结构体
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
	ftp配置结构体
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
	
	//用户名和密码最长255字节
	char username[256];
	char password[256];
	
	//file_type 0是二进制，1是ASCII默认是0
	int filetype;
	
	//0是主动模式，由模块监听端口,1是被动模式，由FTP服务器监听端口默认是0
	int transmode;
	
	//PDP id 范围1-16 默认是1
	int contextID;
	
	//超时时间，20-180 默认90
	int rsptimeout;
	
	//0是FTP,1是FTPS,默认是0
	int ssltype;
	
	//选择SSL id 默认是0，可选0-5
	int sslctxid;
	
	//0 使用服务器分发的地址,1 使用服务器控制会话地址,默认0
	int data_address;
};

/*
	EC20重启
*/
int net_ec20_restart(int timeout_ms);

/*
	EC20开关
*/
int net_ec20_power(int on_off);

/*
	发送指令并检查是否返回OK
*/
int net_at_send(char *send_buffer ,int timeout_ms);

/*
	AT+QMTCFG 配置MQTT
*/
int net_mqtt_configrate(int client_idx, struct mqtt_config_struct *mqtt_config);

/*
	打开MQTT网络连接函数
*/
int net_mqtt_open(int client_idx,char *host_name, int port);

/*
	关闭MQTT连接函数
*/
int net_mqtt_close(int client_idx);

/*
	连接一个客户端到MQTT服务器
*/
int net_mqtt_connect(int client_idx, char *client_ID, char *username, char *password);

/*
	从MQTT服务器断开一个客户端
*/
int net_mqtt_disconnect(int client_idx);

/*
	订阅频道
*/
int net_mqtt_subscribe(int client_idx, int msgID, int topic_amount, struct topic_qos_struct *topic_qos_array);

/*
	取消订阅
*/
int net_mqtt_unsubscribe(int client_idx,int msgID, int topic_amount, struct topic_struct *topic_array);

/*
	发布消息
*/
int net_mqtt_publish(int client_idx, int msgID, int qos, int retain, char *topic, int msg_length , char *payload, int beep_in);

/*
	查看保存的信息数量
*/
int net_mqtt_receive_check(int *amount_out, struct mqtt_storage_struct mqtt_storage_array_out[6]);

/*
	读取一条信息
*/
int net_mqtt_receive_read(int client_idx, struct mqtt_message_struct *mqtt_message_out);

/*
	URC函数，主循环
*/
int net_mqtt_urc(char *buffer_in, struct mqtt_urc_struct *mqtt_urc_out);

/*
	获取网络时间
*/
int net_get_time(struct time_struct *time_out);

/*
	获取信号强度
	共四格信号，4最强，0没有
*/
int net_signal_quality(int *quality_out);

/*
	AT+QFTPCFG 配置 FTP
*/
int net_ftp_configrate(struct ftp_config_struct *ftp_confing_in);

/*
	打开FTP网络连接函数
*/
int net_ftp_open(char *host_name, int port);

/*
	关闭ftp连接
*/
int net_ftp_close(void);

/*
	配置当前FTP路径
*/
int net_ftp_cwd(char *path_name_in);

/*
	删除文件
*/
int net_ftp_del(char *file_name_in);

/*
	下载文件
*/
int net_ftp_get(char *file_name_in, char *local_name_in, int startpos_in, int downloadlen_in, char *buffer_out);

/*
	建立文件夹
*/
int net_ftp_mkdir(char *folder_name_in);

/*
	FTP上传文件(串口方式)
*/
int net_ftp_put_COM(char *file_name_in, int startpos_in, int uploadlen_in, int beof_in, char*payload_in);

/*
	FTP上传文件(文件方式)
*/
int net_ftp_put_FILE(char *file_name_in, char *local_name_in, int startpos_in);

/*
	获取当前FTP路径
*/
int net_ftp_pwd(char *path_name_out);

/*
	删除文件夹
*/
int net_ftp_rmdir(char *folder_name_in);

/*
	获得文件长度
*/
int net_ftp_size(char *file_name_in, int *filesize_out);

/*
	获得ftp服务器状态
*/
int net_ftp_stat(int *ftpstat_out);

/*
	获取基站号
*/
int net_get_base_station(char *lac_out, char *ci_out);

/*
	查看是否依附分组域
*/
int net_check_packet_domain(int *state_out);

/*
	查看MQTT连接状态
*/
int net_check_mqtt_connect(int *state_out);

/*
	查看ICCID
*/
int net_get_iccid(char *iccid_out);
/*
	查看EC20F/EC200U ID 
*/
int net_get_device_ID(char *iccid_out);

int net_phonebook_num(char *num);

int net_short_message_device(char *device_state,char *da);
#endif

