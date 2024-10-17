/*
	网络具体功能相关
	作者：lee
	时间：2019/12/31
*/

#include "main.h"

extern struct mqtt_message_struct mqtt_message;
extern struct mqtt_storage_struct mqtt_storage_array[6];
void check_storage(void);

static char temp_buffer[1024] = {0};
static struct mqtt_urc_struct mqtt_urc = {0};
static char payload[1024] = {0};
extern int compare_vibration(const void* value1, const void* value2);

/*
	网络服务函数
*/
int net_function_service(char *buffer_in)
{
	struct net_function_service_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_function_service_ret = {0,1};
	
	//局部变量
	int ret=0;
	//int amount=0;
	
	//监听网络
	ret = net_mqtt_urc(buffer_in, &mqtt_urc);
	if(0==ret)
	{
		if(1==mqtt_urc.state_flag)
		{
			printf("[net]网络连接已断开，重新连接\n");
			global_flag.net_reconnect_flag=1;
			return net_function_service_ret.RET_OK;
		}
		
		if(1==mqtt_urc.receive_flag && 0==mqtt_urc.receive_recv_mode)
		{
			printf("[net]MQTT 接收到消息\n");
			//拿取payload
			ret = net_get_payload(&mqtt_urc, payload);
			if(0!=ret)
			{
				printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
				return net_function_service_ret.RET_ERROR;	
			}
			//解析
			ret = net_parse(payload);
			if(0!=ret)
			{
				printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
				return net_function_service_ret.RET_ERROR;	
			}
			return net_function_service_ret.RET_OK;
		}
		
		if(1==mqtt_urc.receive_flag && 1==mqtt_urc.receive_recv_mode)
		{
			printf("[net]MQTT 接收到消息\n");
			//循环检查接收缓存中是否有数据
			check_storage();
		}
		
		if(1==mqtt_urc.ping_flag)
		{
			printf("[net]MQTT ping 失败\n");
			return net_function_service_ret.RET_OK;
		}
	}
	
	//函数返回
	return net_function_service_ret.RET_OK;
}


/*
	网络连接
*/
int net_function_connect(void)
{
	struct net_function_connect_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_function_connect_ret = {0,1};
	
	//静态变量
	static int mqtt_msg_id=1;
	
	//局部变量
	struct topic_qos_struct topic_qos={0};
	int ret=0;
	
	//连接网络
	net_mqtt_disconnect(global_mqtt_config.client_idx);
	net_mqtt_close(global_mqtt_config.client_idx);
	net_mqtt_configrate(global_mqtt_config.client_idx, &global_mqtt_config);
	net_mqtt_open(global_mqtt_config.client_idx,global_net_config.mqtt_hostname, global_net_config.mqtt_port);
	net_mqtt_connect(global_mqtt_config.client_idx, global_net_config.mqtt_client_id, global_net_config.mqtt_user_name, global_net_config.mqtt_password);
	
	mqtt_msg_id = mqtt_msg_id%65535+1;
	strcpy(topic_qos.topic, global_net_config.mqtt_sub_ins_topic);
	topic_qos.qos=0;
	ret = net_mqtt_subscribe(global_mqtt_config.client_idx, mqtt_msg_id, 1, &topic_qos);
	if(0 != ret)
	{
//		//led_gprs_mode(LED_RED,1);
		printf("[net]net_function_connect error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_function_connect_ret.RET_ERROR;
	}
	else
	{
		//led_gprs_mode(LED_GREEN,1);
	}
	//函数返回
	return net_function_connect_ret.RET_OK;
}

/*
	消息提取函数
*/
int net_get_payload(struct mqtt_urc_struct *mqtt_urc_in, char *payload_out)
{
	struct net_get_payload_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_get_payload_ret={0,1};
	
	//局部变量
	int ret=0;
	struct mqtt_message_struct mqtt_message={0};
	
	//检查参数
	if(NULL == mqtt_urc_in)
	{
		printf("[net]net_get_payload error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_get_payload_ret.RET_ERROR;
	}
	if(NULL == payload_out)
	{
		printf("[net]net_get_payload error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_get_payload_ret.RET_ERROR;
	}
	
	//判断接收类型
	if(0 == mqtt_urc_in->receive_recv_mode)
	{
		//直接提取
		strcpy(payload_out,mqtt_urc_in->receive_mqtt_message.payload);
		return net_get_payload_ret.RET_OK;
	}
	else if(1 == mqtt_urc_in->receive_recv_mode)
	{
		//从存储空间中获取
		ret = net_mqtt_receive_read(mqtt_urc_in->receive_client_idx, &mqtt_message);
		if(0!=ret)
		{
			printf("[net]net_get_payload error. file=%s, line=%d.\n", __FILE__, __LINE__);
			return net_get_payload_ret.RET_ERROR;
		}
		strcpy(payload_out,mqtt_message.payload);
	}
	else
	{
		printf("[net]net_get_payload error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_get_payload_ret.RET_ERROR;
	}
	
	//函数返回
	return net_get_payload_ret.RET_OK;
}

/*
	解析消息
*/
int net_parse(char *buffer_in)
{
	struct net_parse_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_parse_ret={0,1};
	
	//局部变量
	int ret=0;
	if(NULL != strstr(buffer_in, "\"file_url\":\""))
	{
		printf("\n[net]进入 YSZD_SYS_UPGRADE 回调\n");
		ret = net_recv_server_sys_upgrade_gw(buffer_in);
		if(0!=ret)
		{
			printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
			return net_parse_ret.RET_ERROR;
		}
		return net_parse_ret.RET_OK;
	}
	//功能分类
	if(NULL != strstr(buffer_in, "\"action\":\"server_startup_buzzer\""))
	{
		printf("\n[net]进入 server_startup_buzzer 回调\n");
		ret = net_recv_server_startup_buzzer(buffer_in);
		if(0!=ret)
		{
			printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
			return net_parse_ret.RET_ERROR;
		}
		return net_parse_ret.RET_OK;
	}
	
	if(NULL != strstr(buffer_in, "\"action\":\"server_device_link\""))
	{
		printf("\n[net]进入 server_device_link 回调\n");
		ret = net_recv_server_device_link(buffer_in);
		if(0!=ret)
		{
			printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
			return net_parse_ret.RET_ERROR;
		}
		return net_parse_ret.RET_OK;
	}
	
	if(NULL != strstr(buffer_in, "\"action\":\"server_reboot\""))
	{
		printf("\n[net]进入 server_reboot 回调\n");
		ret = net_recv_server_reboot(buffer_in);
		if(0!=ret)
		{
			printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
			return net_parse_ret.RET_ERROR;
		}
		return net_parse_ret.RET_OK;
	}
	
	if(NULL != strstr(buffer_in, "\"action\":\"server_get_data\""))
	{
		printf("\n[net]进入 server_get_data 回调\n");
		ret = net_recv_server_get_data(buffer_in);
		if(0!=ret)
		{
			printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
			return net_parse_ret.RET_ERROR;
		}
		return net_parse_ret.RET_OK;
	}
	
//	if(NULL != strstr(buffer_in, "\"action\":\"server_get_video\""))
//	{
//		printf("\n[net]进入 server_get_video 回调\n");
//		ret = net_recv_server_get_video(buffer_in);
//		if(0!=ret)
//		{
//			printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
//			return net_parse_ret.RET_ERROR;
//		}
//		return net_parse_ret.RET_OK;
//	}
	
	if(NULL != strstr(buffer_in, "\"action\":\"server_task_start\""))
	{
		printf("\n[net]进入 server_task_start 回调\n");
		ret = net_recv_server_task_start(buffer_in);
		if(0!=ret)
		{
			printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
			return net_parse_ret.RET_ERROR;
		}
		return net_parse_ret.RET_OK;
	}
	
	if(NULL != strstr(buffer_in, "\"action\":\"server_task_over\""))
	{
		printf("\n[net]进入 server_task_over 回调\n");
		ret = net_recv_server_task_over(buffer_in);
		if(0!=ret)
		{
			printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
			return net_parse_ret.RET_ERROR;
		}
		return net_parse_ret.RET_OK;
	}
	
	if(NULL != strstr(buffer_in, "\"action\":\"server_open_upload\""))
	{
		printf("\n[net]进入 server_open_upload 回调\n");
		ret = net_recv_server_open_upload(buffer_in);
		if(0!=ret)
		{
			printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
			return net_parse_ret.RET_ERROR;
		}
		return net_parse_ret.RET_OK;
	}
	
	if(NULL != strstr(buffer_in, "\"action\":\"server_off_upload\""))
	{
		printf("\n[net]进入 server_off_upload 回调\n");
		ret = net_recv_server_off_upload(buffer_in);
		if(0!=ret)
		{
			printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
			return net_parse_ret.RET_ERROR;
		}
		return net_parse_ret.RET_OK;
	}
	
	if(NULL != strstr(buffer_in, "\"action\":\"server_push_config\""))
	{
		printf("\n[net]进入 server_push_config 回调\n");
		ret = net_recv_server_push_config(buffer_in);
		if(0!=ret)
		{
			printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
			return net_parse_ret.RET_ERROR;
		}
		return net_parse_ret.RET_OK;
	}
	
	if(NULL != strstr(buffer_in, "\"action\":\"server_push_threshold\""))
	{
		printf("\n[net]进入 server_push_threshold 回调\n");
		ret = net_recv_server_push_threshold(buffer_in);
		if(0!=ret)
		{
			printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
			return net_parse_ret.RET_ERROR;
		}
		return net_parse_ret.RET_OK;
	}
	
//	if(NULL != strstr(buffer_in, "\"action\":\"server_push_videoconfig\""))
//	{
//		printf("\n[net]进入 server_push_videoconfig 回调\n");
//		ret = net_recv_server_push_videoconfig(buffer_in);
//		if(0!=ret)
//		{
//			printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
//			return net_parse_ret.RET_ERROR;
//		}
//		return net_parse_ret.RET_OK;
//	}
	
//	if(NULL != strstr(buffer_in, "\"action\":\"server_get_max_datalog\""))
//	{
//		printf("\n[net]进入 server_get_max_datalog 回调\n");
//		ret = net_recv_server_get_max_datalog(buffer_in);
//		if(0!=ret)
//		{
//			printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
//			return net_parse_ret.RET_ERROR;
//		}
//		return net_parse_ret.RET_OK;
//	}
	
	if(NULL != strstr(buffer_in, "\"action\":\"server_push_taskinfo\""))
	{
		printf("\n[net]进入 server_push_taskinfo 回调\n");
		ret = net_recv_server_push_taskinfo(buffer_in);
		if(0!=ret)
		{
			printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
			return net_parse_ret.RET_ERROR;
		}
		return net_parse_ret.RET_OK;
	}
//	
//	if(NULL != strstr(buffer_in, "\"action\":\"server_modify_logfile\""))
//	{
//		printf("\n[net]进入 server_modify_logfile 回调\n");
//		ret = net_recv_server_modify_logfile(buffer_in);
//		if(0!=ret)
//		{
//			printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
//			return net_parse_ret.RET_ERROR;
//		}
//		return net_parse_ret.RET_OK;
//	}
	
//	if(NULL != strstr(buffer_in, "\"action\":\"server_modify_log\""))
//	{
//		printf("\n[net]进入 server_modify_log 回调\n");
//		ret = net_recv_server_modify_log(buffer_in);
//		if(0!=ret)
//		{
//			printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
//			return net_parse_ret.RET_ERROR;
//		}
//		return net_parse_ret.RET_OK;
//	}
	
	if(NULL != strstr(buffer_in, "\"action\":\"server_gps_switch\""))
	{
		printf("\n[net]进入 server_gps_switch 回调\n");
		ret = net_recv_server_gps_switch(buffer_in);
		if(0!=ret)
		{
			printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
			return net_parse_ret.RET_ERROR;
		}
		return net_parse_ret.RET_OK;
	}
	
	if(NULL != strstr(buffer_in, "\"action\":\"server_push_time\""))
	{
		printf("\n[net]进入 server_push_time 回调\n");
		ret = net_recv_server_push_time(buffer_in);
		if(0!=ret)
		{
			printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
			return net_parse_ret.RET_ERROR;
		}
		return net_parse_ret.RET_OK;
	}
	
	if(NULL != strstr(buffer_in, "\"action\":\"server_get_local_max_datalog\""))
	{
		printf("\n[net]进入 server_get_local_max_datalog 回调\n");
		ret = net_recv_server_get_local_max_datalog(buffer_in);
		if(0!=ret)
		{
			printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
			return net_parse_ret.RET_ERROR;
		}
		return net_parse_ret.RET_OK;
	}
	
	if(NULL != strstr(buffer_in, "\"action\":\"server_get_device_info\""))
	{
		printf("\n[net]进入 server_get_device_info 回调\n");
		ret = net_recv_server_get_device_info(buffer_in);
		if(0!=ret)
		{
			printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
			return net_parse_ret.RET_ERROR;
		}
		return net_parse_ret.RET_OK;
	}
	
	if(NULL != strstr(buffer_in, "\"action\":\"server_gps_switch_second\""))
	{
		printf("\n[net]进入 server_gps_switch_second 回调\n");
		ret = net_recv_server_gps_switch_second(buffer_in);
		if(0!=ret)
		{
			printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
			return net_parse_ret.RET_ERROR;
		}
		return net_parse_ret.RET_OK;
	}
	
	if(NULL != strstr(buffer_in, "\"action\":\"server_buzzer_switch\""))
	{
		printf("\n[net]进入 server_buzzer_switch 回调\n");
		ret = net_recv_server_beep_switch(buffer_in);
		if(0!=ret)
		{
			printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
			return net_parse_ret.RET_ERROR;
		}
		return net_parse_ret.RET_OK;
	}
	
	if(NULL != strstr(buffer_in, "\"action\":\"server_vibration_coefficient\""))
	{
		printf("\n[net]进入 server_vibration_coefficient 回调\n");
		ret = net_recv_server_set_coefficient(buffer_in);
		if(0!=ret)
		{
			printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
			return net_parse_ret.RET_ERROR;
		}
		return net_parse_ret.RET_OK;
	}
	
//	if(NULL != strstr(buffer_in, "\"action\":\"server_rf433_switch\""))
//	{
//		printf("\n[net]进入 server_rf433_switch 回调\n");
//		ret = net_recv_server_rf433_switch(buffer_in);
//		if(0!=ret)
//		{
//			printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
//			return net_parse_ret.RET_ERROR;
//		}
//		return net_parse_ret.RET_OK;
//	}

	if(NULL != strstr(buffer_in, "\"action\":\"server_network_switch\""))
	{
		printf("\n[net]进入 server_network_switch 回调\n");
		ret = net_recv_server_sleep_network_switch(buffer_in);
		if(0!=ret)
		{
			printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
			return net_parse_ret.RET_ERROR;
		}
		return net_parse_ret.RET_OK;
	}
	
	if(NULL != strstr(buffer_in, "\"action\":\"server_run_log_file\""))
	{
		printf("\n[net]进入 server_run_log_file 回调\n");
		ret = net_recv_server_get_run_log_file(buffer_in);
		if(0!=ret)
		{
			printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
			return net_parse_ret.RET_ERROR;
		}
		return net_parse_ret.RET_OK;
	}
	
	if(NULL != strstr(buffer_in, "\"action\":\"server_shut_down\""))
	{
		printf("\n[net]进入 server_shut_down 回调\n");
		ret = net_recv_server_shut_down(buffer_in);
		if(0!=ret)
		{
			printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
			return net_parse_ret.RET_ERROR;
		}
		return net_parse_ret.RET_OK;
	}
	
//	if(NULL != strstr(buffer_in, "\"action\":\"server_clear_sd_card\""))
//	{
//		printf("\n[net]进入 server_clear_sd_card 回调\n");
//		ret = net_recv_server_clear_sd_card(buffer_in);
//		if(0!=ret)
//		{
//			printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
//			return net_parse_ret.RET_ERROR;
//		}
//		return net_parse_ret.RET_OK;
//	}
	if(NULL != strstr(buffer_in, "\"action\":\"task_log_clear\""))
	{
		printf("\n[net]新任务建立通知完成\n");
		global_flag.task_on_sign = 0;
//		ret = net_recv_server_reboot(buffer_in);
//		if(0!=ret)
//		{
//			printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
//			return net_parse_ret.RET_ERROR;
//		}
		return net_parse_ret.RET_OK;
	}
	//到这说明没有匹配项，返回ERROR
	return net_parse_ret.RET_ERROR;
	
}

/*
	确认消息
*/
int net_rollback_msg(char *mqtt_action, char *session_id)
{
	//返回值定义
	struct net_rollback_msg_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_rollback_msg_ret={0,1};
	
	//局部变量定义
	char temp_string[64]={0};
	int ret=0;
	
	//拼接报文
	memset(temp_buffer, 0, sizeof(temp_buffer));
	//action
	sprintf(temp_string,"rollback_msg,");
	strcat(temp_buffer, temp_string);
	//uid
	sprintf(temp_string,"%s,",global_device_info.uid);
	strcat(temp_buffer, temp_string);
	//session_id
	sprintf(temp_string,"%s,",session_id);
	strcat(temp_buffer, temp_string);
	//date_time
	sprintf(temp_string,"%04d-%02d-%02d %02d:%02d:%02d,",\
					global_device_info.device_time.year+2000, global_device_info.device_time.month, global_device_info.device_time.day,\
					global_device_info.device_time.hour,global_device_info.device_time.minute,global_device_info.device_time.second);
	strcat(temp_buffer, temp_string);
	//mqtt_action
	sprintf(temp_string,"%s,",mqtt_action);
	strcat(temp_buffer, temp_string);
	//state
	sprintf(temp_string,"success");
	strcat(temp_buffer, temp_string);
	
	//发布消息
	ret = net_mqtt_publish(global_mqtt_config.client_idx, 0, 0, 0, global_net_config.mqtt_pub_rollback_topic, strlen(temp_buffer) , temp_buffer, global_device_config.beep_mode);
	if(0!=ret)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_rollback_msg_ret.RET_ERROR;
	}
	
	//函数返回
	return net_rollback_msg_ret.RET_OK;
}

/*
	服务器让设备响铃
*/
int net_recv_server_startup_buzzer(char *buffer_in)
{
	struct net_recv_server_startup_buzzer_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_recv_server_startup_buzzer_ret = {0,1};
	
	//局部变量
	char *p=NULL;
	char action[64]={0};
	char session_id[64]={0};
	int i=0;
	int j=0;
	
	//参数检查
	if(NULL == buffer_in)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_startup_buzzer_ret.RET_ERROR;
	}
	
	//解析出action
	p = strstr(buffer_in, "\"action\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"action\":\"%[^\"]", action);
	}

	//解析出session_id
	p = strstr(buffer_in, "\"session_id\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"session_id\":\"%[^\"]", session_id);
	}
	
	//发送确认消息
	net_rollback_msg(action, session_id);
	
	//让蜂鸣器响
	for(j=0;j<3;j++)
	{
		for(i=0;i<2;i++)
		{
			beep(1);
			osDelay(200);
			beep(0);
			osDelay(200);
		}
		osDelay(400);
	}
	
	//函数返回
	return net_recv_server_startup_buzzer_ret.RET_OK;
}

/*
	服务器测试设备是否在线
*/
int net_recv_server_device_link(char *buffer_in)
{
	struct net_recv_server_device_link_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_recv_server_device_link_ret = {0,1};
	
	//局部变量
	char *p=NULL;
	char action[64]={0};
	char session_id[64]={0};
	
	//参数检查
	if(NULL == buffer_in)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_device_link_ret.RET_ERROR;
	}
	
	//解析出action
	p = strstr(buffer_in, "\"action\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"action\":\"%[^\"]", action);
	}

	//解析出session_id
	p = strstr(buffer_in, "\"session_id\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"session_id\":\"%[^\"]", session_id);
	}
	
	//发送确认消息
	net_rollback_msg(action, session_id);
	
	//重置休眠倒计时
	global_timer.sleep_timer_sec = 0;
	
	//函数返回
	return net_recv_server_device_link_ret.RET_OK;
} 

/*
	服务器让设备重启
*/
int net_recv_server_reboot(char *buffer_in)
{
	struct net_recv_server_reboot_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_recv_server_reboot_ret = {0,1};
	
	//局部变量
	char *p=NULL;
	char action[64]={0};
	char session_id[64]={0};
	
	//参数检查
	if(NULL == buffer_in)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_reboot_ret.RET_ERROR;
	}
	
	//解析出action
	p = strstr(buffer_in, "\"action\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"action\":\"%[^\"]", action);
	}

	//解析出session_id
	p = strstr(buffer_in, "\"session_id\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"session_id\":\"%[^\"]", session_id);
	}
	
	//发送确认消息
	net_rollback_msg(action, session_id);
	
	//保存flash
	flash_save_global_variables();
	osDelay(100);
	
	//重启设备
	printf("[net]设备重启...\n");
	osDelay(50);
	system_restart(1);
	
	//函数返回
	return net_recv_server_reboot_ret.RET_OK;
}


/*
	服务器让设备上传数据
*/
int net_recv_server_get_data(char *buffer_in)
{
	struct net_recv_server_get_data_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_recv_server_get_data_ret = {0,1};
	
	//局部变量
	char *p=NULL;
	char action[64]={0};
	char session_id[64]={0};
	
	//参数检查
	if(NULL == buffer_in)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_get_data_ret.RET_ERROR;
	}
	
	//解析出action
	p = strstr(buffer_in, "\"action\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"action\":\"%[^\"]", action);
	}

	//解析出session_id
	p = strstr(buffer_in, "\"session_id\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"session_id\":\"%[^\"]", session_id);
	}
	
	//发送确认消息
	net_rollback_msg(action, session_id);
	
	//上传数据
	global_flag.upload_data_flag = 1;
	
	//函数返回
	return net_recv_server_get_data_ret.RET_OK;
}

/*
	服务器让设备开始任务
*/
int net_recv_server_task_start(char *buffer_in)
{
	struct net_recv_server_task_start_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_recv_server_task_start_ret = {0,1};
	
	//局部变量
	char *p=NULL;
	char action[64]={0};
	char session_id[64]={0};
	int ret=0;
	int temp_year=0;
	int temp_month=0;
	int temp_day=0;
	int temp_hour=0;
	int temp_minute=0;
	int temp_second=0;
	
	//参数检查
	if(NULL == buffer_in)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_task_start_ret.RET_ERROR;
	}
	
	//解析出action
	p = strstr(buffer_in, "\"action\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"action\":\"%[^\"]", action);
	}

	//解析出session_id
	p = strstr(buffer_in, "\"session_id\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"session_id\":\"%[^\"]", session_id);
	}
	
	//解析出任务号
	p = strstr(buffer_in, "\"task_num\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"task_num\":\"%[^\"]", global_task_info.task_num);
	}
	
	//解析出任务开始时间
	p = strstr(buffer_in, "\"server_time\":\"");
	if(NULL!=p)
	{
		ret = sscanf(p, "\"server_time\":\"20%02d-%02d-%02d %02d:%02d:%02d",\
						&temp_year,&temp_month,&temp_day,&temp_hour,&temp_minute,&temp_second);
		if(6==ret)
		{
			global_task_info.task_start_time.year = temp_year;
			global_task_info.task_start_time.month = temp_month;
			global_task_info.task_start_time.day = temp_day;
			global_task_info.task_start_time.hour = temp_hour;
			global_task_info.task_start_time.minute = temp_minute;
			global_task_info.task_start_time.second = temp_second;
		}
	}
//	
//	//配置摄像头FTP
//	strcpy(global_camera_info.camera_ftp_folder, global_task_info.task_num);
//	
//	//发出配置FTP命令给摄像头
//	global_flag.config_video_ftp=1;
//	
	//清空日志
	
	//清空断点续传栈
	stack_clear();
	memset(&global_upload_pool[0], 0, sizeof(struct upload_data_struct)*STACK_SIZE);
	
	//清空最大值
//	global_mpu_info.lean_angle_max_x=0;
//	global_mpu_info.lean_angle_max_y=0;
//	global_mpu_info.lean_angle_max_z=0;
//	global_mpu_info.vibration_max_x=0;
//	global_mpu_info.vibration_max_y=0;
//	global_mpu_info.vibration_max_z=0;
	
	//任务创建之前应该将休眠计数器清零，以免任务拉取成功后直接进入休眠模式 rbw
	global_timer.sleep_timer_sec = 0;
	//任务状态设置为开启
	global_timer.sleep_timer_sec = 0;
	global_task_info.task_status=1;
	
	//保存flash
	flash_save_global_variables();
	
	//发送确认消息
	net_rollback_msg(action, session_id);
	
	//获取视频
	printf("[net]获取视频函数还没写完...\n");
	
	//函数返回
	return net_recv_server_task_start_ret.RET_OK;
}

/*
	服务器让设备结束任务
*/
int net_recv_server_task_over(char *buffer_in)
{
	struct net_recv_server_task_over_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_recv_server_task_over_ret = {0,1};
	
	//局部变量
	char *p=NULL;
	char action[64]={0};
	char session_id[64]={0};
	int ret=0;
	int temp_year=0;
	int temp_month=0;
	int temp_day=0;
	int temp_hour=0;
	int temp_minute=0;
	int temp_second=0;
	
	//参数检查
	if(NULL == buffer_in)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_task_over_ret.RET_ERROR;
	}

	//解析出action
	p = strstr(buffer_in, "\"action\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"action\":\"%[^\"]", action);
	}

	//解析出session_id
	p = strstr(buffer_in, "\"session_id\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"session_id\":\"%[^\"]", session_id);
	}
	
	//解析出任务号
	p = strstr(buffer_in, "\"task_num\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"task_num\":\"%[^\"]", global_task_info.task_num);
	}
	
	//解析出任务结束时间
	p = strstr(buffer_in, "\"server_time\":\"");
	if(NULL!=p)
	{
		ret = sscanf(p, "\"server_time\":\"20%02d-%02d-%02d %02d:%02d:%02d",\
						&temp_year,&temp_month,&temp_day,&temp_hour,&temp_minute,&temp_second);
		if(6==ret)
		{
			global_task_info.task_end_time.year = temp_year;
			global_task_info.task_end_time.month = temp_month;
			global_task_info.task_end_time.day = temp_day;
			global_task_info.task_end_time.hour = temp_hour;
			global_task_info.task_end_time.minute = temp_minute;
			global_task_info.task_end_time.second = temp_second;
		}
	}
	
	//发送确认消息
	net_rollback_msg(action, session_id);
	
	//任务状态设置为关闭
	global_task_info.task_status=0;
	
	//保存flash
	flash_save_global_variables();
	
	//上传日志
	//printf("\n[m203c]任务结束，开始上传日志");

	
	//函数返回
	return net_recv_server_task_over_ret.RET_OK;
}

/*
	服务器让设备允许上传数据
*/
int net_recv_server_open_upload(char *buffer_in)
{
	struct net_recv_server_open_upload_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_recv_server_open_upload_ret = {0,1};
	
	//局部变量
	char *p=NULL;
	char action[64]={0};
	char session_id[64]={0};
	
	//参数检查
	if(NULL == buffer_in)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_open_upload_ret.RET_ERROR;
	}
	
	//解析出action
	p = strstr(buffer_in, "\"action\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"action\":\"%[^\"]", action);
	}

	//解析出session_id
	p = strstr(buffer_in, "\"session_id\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"session_id\":\"%[^\"]", session_id);
	}
	
	//发送确认消息
	net_rollback_msg(action, session_id);
	
	//置位上传开关标志
	global_flag.upload_enable_flag=1;
	
	//函数返回
	return net_recv_server_open_upload_ret.RET_OK;
}


/*
	服务器让设备禁止上传数据
*/
int net_recv_server_off_upload(char *buffer_in)
{
	struct net_recv_server_off_upload_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_recv_server_off_upload_ret = {0,1};
	
	//局部变量
	char *p=NULL;
	char action[64]={0};
	char session_id[64]={0};

	//参数检查
	if(NULL == buffer_in)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_off_upload_ret.RET_ERROR;
	}

	//解析出action
	p = strstr(buffer_in, "\"action\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"action\":\"%[^\"]", action);
	}

	//解析出session_id
	p = strstr(buffer_in, "\"session_id\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"session_id\":\"%[^\"]", session_id);
	}
	
	//发送确认消息
	net_rollback_msg(action, session_id);
	
	//置位上传开关标志
	global_flag.upload_enable_flag=0;
	
	//函数返回
	return net_recv_server_off_upload_ret.RET_OK;
}

/*
	服务器推送终端配置信息
*/
int net_recv_server_push_config(char *buffer_in)
{
	struct net_recv_server_push_config_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_recv_server_push_config_ret = {0,1};
	
	//局部变量
	char *p=NULL;
	char action[64]={0};
	char session_id[64]={0};
	int ret=0;
	int temp_year=0;
	int temp_month=0;
	int temp_day=0;
	int temp_hour=0;
	int temp_minute=0;
	int temp_second=0;
	int temp_weekday=0;
	char temp_sn[20] = {0};
	int8_t sn_len = 0;
	//参数检查
	if(NULL == buffer_in)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_push_config_ret.RET_ERROR;
	}

	//解析出action
	p = strstr(buffer_in, "\"action\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"action\":\"%[^\"]", action);
	}

	//解析出session_id
	p = strstr(buffer_in, "\"session_id\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"session_id\":\"%[^\"]", session_id);
	}
	
	//解析出服务器时间
	p = strstr(buffer_in, "\"server_time\":\"");
	if(NULL!=p)
	{
		ret=sscanf(p, "\"server_time\":\"20%02d-%02d-%02d %02d:%02d:%02d\"", \
			&temp_year,
			&temp_month,
			&temp_day,
			&temp_hour,
			&temp_minute,
			&temp_second);
		if(6==ret)
		{
			global_rtc_set_time.time_for_set.year=temp_year;
			global_rtc_set_time.time_for_set.month=temp_month;
			global_rtc_set_time.time_for_set.day=temp_day;
			global_rtc_set_time.time_for_set.hour=temp_hour;
			global_rtc_set_time.time_for_set.minute=temp_minute;
			global_rtc_set_time.time_for_set.second=temp_second;
			global_rtc_set_time.need_set=1;
		}
	}
	
	//解析出星期
	p = strstr(buffer_in, "\"week_day\":\"");
	if(NULL!=p)
	{
		ret=sscanf(p, "\"week_day\":\"%d\"", &temp_weekday);
		if(1==ret)
		{
			global_rtc_set_time.time_for_set.weekday = temp_weekday;
		}
	}
	
	//解析FTP IP
	p = strstr(buffer_in, "\"ftp_ip\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"ftp_ip\":\"%[^\"]", global_ftp_config.hostname);
	}
	
	//解析 FTP user
	p = strstr(buffer_in, "\"ftp_user\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"ftp_user\":\"%[^\"]", global_ftp_config.username);
	}
	
	//解析 ftp_pwd
	p = strstr(buffer_in, "\"ftp_pwd\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"ftp_pwd\":\"%[^\"]", global_ftp_config.password);
	}
	
	//解析 port_num
	p = strstr(buffer_in, "\"port_num\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"port_num\":\"%d\"", &global_ftp_config.port);
	}
	
	//解析 sleepTime
	p = strstr(buffer_in, "\"sleepTime\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"sleepTime\":\"%d\"", &global_device_config.sleep_time);
	}
	
	//解析 awakeTime
	p = strstr(buffer_in, "\"awakeTime\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"awakeTime\":\"%d\"", &global_device_config.awake_time);
	}
//20220311	
	//解析 interval
//	p = strstr(buffer_in, "\"interval\":\"");
//	if(NULL!=p)
//	{
//		sscanf(p, "\"interval\":\"%d\"", &global_device_config.interval);
//	}
	
	//解析 max_data_path
	p = strstr(buffer_in, "\"max_data_path\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"max_data_path\":\"%[^\"]", global_ftp_config.max_data_path);
	}
	
	//解析 run_log_path
	p = strstr(buffer_in, "\"run_log_path\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"run_log_path\":\"%[^\"]", global_ftp_config.run_log_path);
	}
	
	//解析SN码
	p = strstr(buffer_in, "\"terminal_sn\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"terminal_sn\":\"%[^\"]", global_device_info.terminal_sn);
		sprintf(temp_sn,"%s",global_device_info.terminal_sn);
		sn_len = strlen(global_device_info.terminal_sn) - 8;
		if(sn_len > 0)
		{
			 sprintf(global_device_info.terminal_sn,"021002%s",temp_sn+sn_len);
		}
		else
		{
			sprintf(global_device_info.terminal_sn,"02100221120010");
		}
	}
	
	//发送确认消息
	net_rollback_msg(action, session_id);
	
	//函数返回
	return net_recv_server_push_config_ret.RET_OK;
}

/*
	服务器推送终端阈值信息
*/
int net_recv_server_push_threshold(char *buffer_in)
{
	struct net_recv_server_push_threshold_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_recv_server_push_threshold_ret = {0,1};
	
	//局部变量
	char *p=NULL;
	char action[64]={0};
	char session_id[64]={0};
	int temp_year=0;
	int temp_month=0;
	int temp_day=0;
	int temp_hour=0;
	int temp_minute=0;
	int temp_second=0;
	int ret=0;
	int temp_weekday=0;
	
	//参数检查
	if(NULL == buffer_in)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_push_threshold_ret.RET_ERROR;
	}

	//解析出action
	p = strstr(buffer_in, "\"action\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"action\":\"%[^\"]", action);
	}

	//解析出session_id
	p = strstr(buffer_in, "\"session_id\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"session_id\":\"%[^\"]", session_id);
	}
	
	//解析出服务器时间
	p = strstr(buffer_in, "\"server_time\":\"");
	if(NULL!=p)
	{
		ret=sscanf(p, "\"server_time\":\"20%02d-%02d-%02d %02d:%02d:%02d\"", \
			&temp_year,
			&temp_month,
			&temp_day,
			&temp_hour,
			&temp_minute,
			&temp_second);
		if(6==ret)
		{
			global_rtc_set_time.time_for_set.year=temp_year;
			global_rtc_set_time.time_for_set.month=temp_month;
			global_rtc_set_time.time_for_set.day=temp_day;
			global_rtc_set_time.time_for_set.hour=temp_hour;
			global_rtc_set_time.time_for_set.minute=temp_minute;
			global_rtc_set_time.time_for_set.second=temp_second;
			global_rtc_set_time.need_set=1;
		}
	}
	
	//解析出星期
	p = strstr(buffer_in, "\"week_day\":\"");
	if(NULL!=p)
	{
		ret=sscanf(p, "\"week_day\":\"%d\"", &temp_weekday);
		if(1==ret)
		{
			global_rtc_set_time.time_for_set.weekday = temp_weekday;
		}
	}
	
	//解析 dump_energy
	p = strstr(buffer_in, "\"dump_energy\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"dump_energy\":\"%d\"", &global_threshold.dump_energy);
	}
	
//	//解析 temperature
//	p = strstr(buffer_in, "\"temperature\":\"");
//	if(NULL!=p)
//	{
//		sscanf(p, "\"temperature\":\"%lf,%lf\"", \
//			&global_threshold.temperature_low, \
//			&global_threshold.temperature_high);
//	}
	
	//解析 lean_angle
//	p = strstr(buffer_in, "\"lean_angle\":\"");
//	if(NULL!=p)
//	{
//		sscanf(p, "\"lean_angle\":\"%lf,%lf\"", \
//			&global_threshold.lean_angle_x, \
//			&global_threshold.lean_angle_y);
//	}
	
	//解析 pressure
//	p = strstr(buffer_in, "\"pressure\":\"");
//	if(NULL!=p)
//	{
//		sscanf(p, "\"pressure\":\"%lf,%lf\"", \
//			&global_threshold.pressure_low, \
//			&global_threshold.pressure_high);
//	}
	
	//解析 vibration
//	p = strstr(buffer_in, "\"vibration\":\"");
//	if(NULL!=p)
//	{
//		sscanf(p, "\"vibration\":\"%lf,%lf,%lf\"", \
//			&global_threshold.vibration_x, \
//			&global_threshold.vibration_y, \
//			&global_threshold.vibration_z);
//	}
	
	//解析 humidity
//	p = strstr(buffer_in, "\"humidity\":\"");
//	if(NULL!=p)
//	{
//		sscanf(p, "\"humidity\":\"%lf\"", \
//			&global_threshold.humidity);
//	}
	
	//解析 speed
	p = strstr(buffer_in, "\"speed\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"speed\":\"%lf\"", \
			&global_threshold.speed);
	}
	
	//发送确认消息
	net_rollback_msg(action, session_id);
	
	
	//函数返回
	return net_recv_server_push_threshold_ret.RET_OK;
}
/*
	服务器向设备推送任务信息
*/
int net_recv_server_push_taskinfo(char *buffer_in)
{
	struct net_recv_server_push_taskinfo_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_recv_server_push_taskinfo_ret = {0,1};
	
	//局部变量
	char *p=NULL;
	char action[64]={0};
	char session_id[64]={0};
	int ret=0;
	struct time_struct temp_start_time={0};
	struct time_struct temp_end_time={0};
	int temp_task_status=0;
	int have_task_num=0;
	int have_task_status=0;
	int have_start_time=0;
	short temp_year=0;
	short temp_month=0;
	short temp_day=0;
	short temp_hour=0;
	short temp_minute=0;
	short temp_second=0;
	char temp_task_num[64]={0};
	char temp_sn[20] = {0};
	int8_t sn_len = 0;
	//参数检查
	if(NULL == buffer_in)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_push_taskinfo_ret.RET_ERROR;
	}

	//解析出action
	p = strstr(buffer_in, "\"action\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"action\":\"%[^\"]", action);
	}

	//解析出session_id
	p = strstr(buffer_in, "\"session_id\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"session_id\":\"%[^\"]", session_id);
	}
	
	//解析出 terminal_sn
	p = strstr(buffer_in, "\"terminal_sn\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"terminal_sn\":\"%[^\"]", global_device_info.terminal_sn);
		sprintf(temp_sn,"%s",global_device_info.terminal_sn);
		sn_len = strlen(global_device_info.terminal_sn) - 8;
		if(sn_len > 0)
		{
			 sprintf(global_device_info.terminal_sn,"021002%s",temp_sn+sn_len);
		}
		else
		{
			sprintf(global_device_info.terminal_sn,"02100221120010");
		}	
	}
	
	//解析出 task_num
	p = strstr(buffer_in, "\"task_num\":\"");
	if(NULL!=p)
	{
		ret = sscanf(p, "\"task_num\":\"%[^\"]", temp_task_num);
		if(1==ret)
		{
			have_task_num=1;
		}
	}
	
	//解析出任务状态
	p = strstr(buffer_in, "\"task_status\":\"");
	if(NULL!=p)
	{
		ret = sscanf(p, "\"task_status\":\"%d\"", &temp_task_status);
		if(1==ret)
		{
			have_task_status=1;
		}	
	}
	
	//解析出任务开始时间
	p = strstr(buffer_in, "\"start_time\":\"");
	if(NULL!=p)
	{
		ret = sscanf(p, "\"start_time\":\"20%02hd-%02hd-%02hd %02hd:%02hd:%02hd\"", \
			&temp_year, \
			&temp_month, \
			&temp_day, \
			&temp_hour, \
			&temp_minute, \
			&temp_second);
		
		if(6==ret)
		{
			have_start_time=1;
			
			temp_start_time.year = temp_year;
			temp_start_time.month = temp_month;
			temp_start_time.day = temp_day;
			temp_start_time.hour = temp_hour;
			temp_start_time.minute = temp_minute;
			temp_start_time.second = temp_second;
		}
	}
	
	//解析出任务结束时间
	p = strstr(buffer_in, "\"end_time\":\"");
	if(NULL!=p)
	{
		ret = sscanf(p, "\"end_time\":\"20%02hd-%02hd-%02hd %02hd:%02hd:%02hd\"", \
			&temp_year, \
			&temp_month, \
			&temp_day, \
			&temp_hour, \
			&temp_minute, \
			&temp_second);
		
		if(6==ret)
		{
			temp_end_time.year = temp_year;
			temp_end_time.month = temp_month;
			temp_end_time.day = temp_day;
			temp_end_time.hour = temp_hour;
			temp_end_time.minute = temp_hour;
			temp_end_time.second = temp_second;
		}
	}
	
	if(1==have_task_num && 1==have_task_status && 1==have_start_time)
	{
		//比较接收到的和保存的task_num是否一致
		ret = strcmp(temp_task_num, global_task_info.task_num);
		if(0!=ret)
		{
			//不一致则清空原任务的日志和断点续传栈，并保存flash
			stack_clear();
			memset(&global_upload_pool[0], 0, sizeof(struct upload_data_struct)*STACK_SIZE);
			
			//保存新的任务号
			strcpy(global_task_info.task_num, temp_task_num);
			
			//保存新的任务状态
			global_task_info.task_status = temp_task_status;
			
			//保存任务开始时间
			global_task_info.task_start_time.year = temp_start_time.year;
			global_task_info.task_start_time.month = temp_start_time.month;
			global_task_info.task_start_time.day = temp_start_time.day;
			global_task_info.task_start_time.hour = temp_start_time.hour;
			global_task_info.task_start_time.minute = temp_start_time.minute;
			global_task_info.task_start_time.second = temp_start_time.second;
			
			//保存任务结束时间
			global_task_info.task_end_time.year = temp_end_time.year;
			global_task_info.task_end_time.month = temp_end_time.month;
			global_task_info.task_end_time.day = temp_end_time.day;
			global_task_info.task_end_time.hour = temp_end_time.hour;
			global_task_info.task_end_time.minute = temp_end_time.minute;
			global_task_info.task_end_time.second = temp_end_time.second;
			
			//根据任务状态配置摄像头FTP
//			if(1==global_task_info.task_status)
//			{
//				strcpy(global_camera_info.camera_ftp_folder, global_task_info.task_num);
//				global_flag.config_video_ftp=1;
//			}
		}
		else
		{
			//如果接收到的和保存的任务号一致
			//判断是否在关机过程中任务结束
			if(1 == global_task_info.task_status && 0 == temp_task_status)
			{
				//任务在关机时已经结束，清空断点续传栈，并置位上传日志标志位
				stack_clear();
				memset(&global_upload_pool[0], 0, sizeof(struct upload_data_struct)*STACK_SIZE);
				
				//保存新的任务号
				strcpy(global_task_info.task_num, temp_task_num);
				
				//保存新的任务状态
				global_task_info.task_status = temp_task_status;
				
				//保存任务开始时间
				global_task_info.task_start_time.year = temp_start_time.year;
				global_task_info.task_start_time.month = temp_start_time.month;
				global_task_info.task_start_time.day = temp_start_time.day;
				global_task_info.task_start_time.hour = temp_start_time.hour;
				global_task_info.task_start_time.minute = temp_start_time.minute;
				global_task_info.task_start_time.second = temp_start_time.second;
				
				//保存任务结束时间
				global_task_info.task_end_time.year = temp_end_time.year;
				global_task_info.task_end_time.month = temp_end_time.month;
				global_task_info.task_end_time.day = temp_end_time.day;
				global_task_info.task_end_time.hour = temp_end_time.hour;
				global_task_info.task_end_time.minute = temp_end_time.minute;
				global_task_info.task_end_time.second = temp_end_time.second;
			}
		}
	}
	
	//发送确认消息
	net_rollback_msg(action, session_id);
	
	//函数返回
	return net_recv_server_push_taskinfo_ret.RET_OK;
}

/*
	正常上传数据
*/
int net_upload_data(void)
{
	struct net_upload_data_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_upload_data_ret = {0,1};
	
	//局部变量
	char temp_string[64]={0};
	int ret=0;
	
	//准备数据
	memset(temp_buffer, 0, sizeof(temp_buffer));
	//action
	sprintf(temp_string,"device_upload_data,");
	strcat(temp_buffer, temp_string);
	//uid
	sprintf(temp_string,"%s,",global_device_info.uid);
	strcat(temp_buffer, temp_string);
	//session_id
	sprintf(temp_string,"%02d%02d%02d%02d%02d%02d%02d,", \
					global_device_info.device_time.year, global_device_info.device_time.month, global_device_info.device_time.day, \
					global_device_info.device_time.hour,global_device_info.device_time.minute,global_device_info.device_time.second,\
					(global_timer.system_timer/10)%100);
	strcat(temp_buffer, temp_string);
	//date_time
	sprintf(temp_string,"%04d-%02d-%02d %02d:%02d:%02d.%02d,",\
					global_upload_data.time.year+2000, global_upload_data.time.month, global_upload_data.time.day, \
					global_upload_data.time.hour,global_upload_data.time.minute,global_upload_data.time.second,global_upload_data.ms);
	strcat(temp_buffer, temp_string);
	//location
	if(1==global_upload_data.location_mode)
	{
		//GPS定位无效
		sprintf(temp_string,"0.000000,0.000000,");
		strcat(temp_buffer, temp_string);
	}
	else
	{
		sprintf(temp_string,"%f%s,%f%s,", \
			global_upload_data.longitude, \
			global_upload_data.EW, \
			global_upload_data.latitude, \
			global_upload_data.NS);
		strcat(temp_buffer, temp_string);
	}
	//task_num
	sprintf(temp_string,"%s,",global_task_info.task_num);
	strcat(temp_buffer, temp_string);
	//speed
	sprintf(temp_string,"%.1f,",global_upload_data.speed);
	strcat(temp_buffer, temp_string);
	//height
	sprintf(temp_string,"0,");
	strcat(temp_buffer, temp_string);
	//direction
	sprintf(temp_string,"0,");
	strcat(temp_buffer, temp_string);
	//vibration

	//lean_angle

	//temperature

	//pressure

	//dump_energy
	sprintf(temp_string,"%d,",global_upload_data.dump_energy);
	strcat(temp_buffer, temp_string);
	//humidity

	//location_mode
	sprintf(temp_string,"%d,",global_upload_data.location_mode);
	strcat(temp_buffer, temp_string);
	//vibration_frequency

	
	sprintf(temp_string,"%d,",global_upload_data.signal);
	strcat(temp_buffer, temp_string);
	//lac
	if(0==strlen(global_upload_data.lac))
	{
		sprintf(temp_string,"0,");
		strcat(temp_buffer, temp_string);
	}
	else
	{
		sprintf(temp_string,"%s,",global_upload_data.lac);
		strcat(temp_buffer, temp_string);
	}
	
	//ci
	if(0==strlen(global_upload_data.ci))
	{
		sprintf(temp_string,"0");
		strcat(temp_buffer, temp_string);
	}
	else
	{
		sprintf(temp_string,"%s",global_upload_data.ci);
		strcat(temp_buffer, temp_string);
	}
	
	//发布消息
	ret = net_mqtt_publish(global_mqtt_config.client_idx, 0, 0, 0, global_net_config.mqtt_pub_collection_topic, strlen(temp_buffer) , temp_buffer, global_device_config.beep_mode);
	if(0!=ret)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_upload_data_ret.RET_ERROR;
	}
	//函数返回
	return net_upload_data_ret.RET_OK;
}
/*
	终端发送新任务开启，清除60组数据信息（国网）
*/
struct time_struct new_task_time;
int net_registration_task_device_gw(char *device_state)
{
	struct net_registration_device_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_registration_device_ret = {0,1};
	
	//局部变量
	char temp_string[64]={0};
	int ret=0;
	char temp_lac[32]={0};
	char temp_ci[32]={0};
	
	//参数检查
	if(NULL==device_state)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_registration_device_ret.RET_ERROR;
	}
	
	//准备数据
	memset(temp_buffer, 0, sizeof(temp_buffer));
	//action
	sprintf(temp_string,"registration_device,");
	strcat(temp_buffer, temp_string);
	//uid
	sprintf(temp_string,"%s,",global_device_info.uid);
	strcat(temp_buffer, temp_string);
	//session_id
	sprintf(temp_string,"%02d%02d%02d%02d%02d%02d%02d,", \
					new_task_time.year, new_task_time.month, new_task_time.day, \
					new_task_time.hour,new_task_time.minute,new_task_time.second,\
					(global_timer.system_timer/10)%100);
	strcat(temp_buffer, temp_string);
	//date_time
	sprintf(temp_string,"%04d-%02d-%02d %02d:%02d:%02d,",\
					new_task_time.year+2000, new_task_time.month, new_task_time.day, \
					new_task_time.hour,new_task_time.minute,new_task_time.second);
	strcat(temp_buffer, temp_string);
	//location
	if(0==global_gps_out.valid)
	{
		//定位无效
		sprintf(temp_string,"0.000000,0.000000,");
		strcat(temp_buffer, temp_string);
	}
	else
	{
		sprintf(temp_string,"%f%s,%f%s,", \
			global_gps_out.longitude, \
			global_gps_out.EW, \
			global_gps_out.latitude, \
			global_gps_out.NS);
		strcat(temp_buffer, temp_string);
	}
	
	//版本号
	sprintf(temp_string,"%s %s,",__DATE__,__TIME__);
	strcat(temp_buffer, temp_string);
	//设备状态
	sprintf(temp_string,"%s,",device_state);
	strcat(temp_buffer, temp_string);
	//倾角
	sprintf(temp_string,"%.2f,%.2f,",(double)0, (double)0);
	strcat(temp_buffer, temp_string);
	//电量
	sprintf(temp_string,"%d,",global_device_info.dump_energy);
	strcat(temp_buffer, temp_string);
	//开机时间
	sprintf(temp_string,"%04d-%02d-%02d %02d:%02d:%02d,",\
					global_device_info.start_up_time.year+2000, global_device_info.start_up_time.month, global_device_info.start_up_time.day, \
					global_device_info.start_up_time.hour,global_device_info.start_up_time.minute,global_device_info.start_up_time.second);
	strcat(temp_buffer, temp_string);
	//获取基站
	net_get_base_station(temp_lac, temp_ci);
	//lac
	sprintf(temp_string,"%s,",temp_lac);
	strcat(temp_buffer, temp_string);
	//ci
	sprintf(temp_string,"%s",temp_ci);
	strcat(temp_buffer, temp_string);
	
	//发布消息
	ret = net_mqtt_publish(global_mqtt_config.client_idx, 0, 0, 0, global_net_config.mqtt_pub_task_topic, strlen(temp_buffer) , temp_buffer, global_device_config.beep_mode);
	if(0!=ret)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_registration_device_ret.RET_ERROR;
	}
//	ret = net_mqtt_publish(global_mqtt_config.client_idx, 0, 0, 0, global_net_config.mqtt_pub_upload_topic_gw, strlen(temp_buffer) , temp_buffer, global_device_config.beep_mode);
//	if(0!=ret)
//	{
//		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
//		return net_registration_device_ret,RET_ERROR;
//	}
	//函数返回
	return net_registration_device_ret.RET_OK;
}
/*
	终端发送注册设备信息
*/
int net_registration_device(char *device_state)
{
	struct net_registration_device_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_registration_device_ret = {0,1};
	
	//局部变量
	char temp_string[64]={0};
	int ret=0;
	char temp_lac[32]={0};
	char temp_ci[32]={0};
	
	//参数检查
	if(NULL==device_state)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_registration_device_ret.RET_ERROR;
	}
	
	//准备数据
	memset(temp_buffer, 0, sizeof(temp_buffer));
	//action
	sprintf(temp_string,"registration_device,");
	strcat(temp_buffer, temp_string);
	//uid
	sprintf(temp_string,"%s,",global_device_info.uid);
	strcat(temp_buffer, temp_string);
	//session_id
	sprintf(temp_string,"%02d%02d%02d%02d%02d%02d%02d,", \
					global_device_info.device_time.year, global_device_info.device_time.month, global_device_info.device_time.day, \
					global_device_info.device_time.hour,global_device_info.device_time.minute,global_device_info.device_time.second,\
					(global_timer.system_timer/10)%100);
	strcat(temp_buffer, temp_string);
	//date_time
	sprintf(temp_string,"%04d-%02d-%02d %02d:%02d:%02d,",\
					global_device_info.device_time.year+2000, global_device_info.device_time.month, global_device_info.device_time.day, \
					global_device_info.device_time.hour,global_device_info.device_time.minute,global_device_info.device_time.second);
	strcat(temp_buffer, temp_string);
	//location
	if(0==global_gps_out.valid)
	{
		//定位无效
		sprintf(temp_string,"0.000000,0.000000,");
		strcat(temp_buffer, temp_string);
	}
	else
	{
		sprintf(temp_string,"%f%s,%f%s,", \
			global_gps_out.longitude, \
			global_gps_out.EW, \
			global_gps_out.latitude, \
			global_gps_out.NS);
		strcat(temp_buffer, temp_string);
	}
	
	//版本号
	sprintf(temp_string,"%s %s,",__DATE__,__TIME__);
	strcat(temp_buffer, temp_string);
	//设备状态
	sprintf(temp_string,"%s,",device_state);
	strcat(temp_buffer, temp_string);
	//倾角
	sprintf(temp_string,"%.2f,%.2f,",(double)0, (double)0);
	strcat(temp_buffer, temp_string);
	//电量
	sprintf(temp_string,"%d,",global_device_info.dump_energy);
	strcat(temp_buffer, temp_string);
	//开机时间
	sprintf(temp_string,"%04d-%02d-%02d %02d:%02d:%02d,",\
					global_device_info.start_up_time.year+2000, global_device_info.start_up_time.month, global_device_info.start_up_time.day, \
					global_device_info.start_up_time.hour,global_device_info.start_up_time.minute,global_device_info.start_up_time.second);
	strcat(temp_buffer, temp_string);
	//获取基站
	net_get_base_station(temp_lac, temp_ci);
	//lac
	sprintf(temp_string,"%s,",temp_lac);
	strcat(temp_buffer, temp_string);
	//ci
	sprintf(temp_string,"%s,",temp_ci);
	strcat(temp_buffer, temp_string);
	if(CHARG_IN() == PIN_RESET)
	{
		sprintf(temp_string,"1");
		strcat(temp_buffer, temp_string);
	}
	else
	{
		sprintf(temp_string,"0");
		strcat(temp_buffer, temp_string);
	}
	//发布消息
	ret = net_mqtt_publish(global_mqtt_config.client_idx, 0, 0, 0, global_net_config.mqtt_pub_upload_topic, strlen(temp_buffer) , temp_buffer, global_device_config.beep_mode);
	if(0!=ret)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_registration_device_ret.RET_ERROR;
	}
	ret = net_mqtt_publish(global_mqtt_config.client_idx, 0, 0, 0, global_net_config.mqtt_pub_upload_topic_gw, strlen(temp_buffer) , temp_buffer, global_device_config.beep_mode);
	if(0!=ret)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_registration_device_ret.RET_ERROR;
	}
	//函数返回
	return net_registration_device_ret.RET_OK;
}

/*
	终端发送上传日志信息
*/
int net_upload_log_msg(char *file_name, char *log_type)
{
	struct net_upload_log_msg_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_upload_log_msg_ret = {0,1};
	
	//局部变量
	char temp_string[64]={0};
	int ret=0;
	
	//参数检查
	if(NULL==file_name)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_upload_log_msg_ret.RET_ERROR;
	}
	
	if(NULL==log_type)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_upload_log_msg_ret.RET_ERROR;
	}
	
	//发送mqtt上传成功通知报文
	//拼接报文
	memset(temp_buffer, 0, sizeof(temp_buffer));
	//action
	sprintf(temp_string,"upload_log_msg,");
	strcat(temp_buffer, temp_string);
	//uid
	sprintf(temp_string,"%s,",global_device_info.uid);
	strcat(temp_buffer, temp_string);
	//session_id
	sprintf(temp_string,"%02d%02d%02d%02d%02d%02d%02d,",\
					global_device_info.device_time.year, global_device_info.device_time.month, global_device_info.device_time.day,\
					global_device_info.device_time.hour,global_device_info.device_time.minute,global_device_info.device_time.second,\
					(global_timer.system_timer/10)%100);
	strcat(temp_buffer, temp_string);
	//date_time
	sprintf(temp_string,"%04d-%02d-%02d %02d:%02d:%02d,",\
					global_device_info.device_time.year+2000, global_device_info.device_time.month, global_device_info.device_time.day,\
					global_device_info.device_time.hour,global_device_info.device_time.minute,global_device_info.device_time.second);
	strcat(temp_buffer, temp_string);
	//file_name
	sprintf(temp_string,"%s,",file_name);
	strcat(temp_buffer, temp_string);
	//log type
	sprintf(temp_string,"maxdata_log");
	strcat(temp_buffer, temp_string);
	
	//发布消息
	ret = net_mqtt_publish(global_mqtt_config.client_idx, 0, 0, 0, global_net_config.mqtt_pub_upload_topic, strlen(temp_buffer) , temp_buffer, global_device_config.beep_mode);
	if(0!=ret)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_upload_log_msg_ret.RET_ERROR;
	}
	
	ret = net_mqtt_publish(global_mqtt_config.client_idx, 0, 0, 0, global_net_config.mqtt_pub_upload_topic_gw, strlen(temp_buffer) , temp_buffer, global_device_config.beep_mode);
	if(0!=ret)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_upload_log_msg_ret.RET_ERROR;
	}
	
	//函数返回
	return net_upload_log_msg_ret.RET_OK;
}

/*
	判断是否报警
*/
uint32_t Inclination_alarm_sign = 0;
//extern uint8_t angle_max_sign;
int check_if_alarm(void)
{
	
	return 0;
}

/*
	服务器控制GPS开关
*/
int net_recv_server_gps_switch(char *buffer_in)
{
	struct net_recv_server_gps_switch_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_recv_server_gps_switch_ret = {0,1};
	
	//局部变量
	char *p=NULL;
	char action[64]={0};
	char session_id[64]={0};
	
	//参数检查
	if(NULL == buffer_in)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_gps_switch_ret.RET_ERROR;
	}

	//解析出action
	p = strstr(buffer_in, "\"action\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"action\":\"%[^\"]", action);
	}

	//解析出session_id
	p = strstr(buffer_in, "\"session_id\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"session_id\":\"%[^\"]", session_id);
	}
	
	//解析 gps_switch
	p = strstr(buffer_in, "\"gps_switch\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"gps_switch\":\"%d\"", &global_device_config.gnss_mode);
	}
	
	//发送确认消息
	net_rollback_msg(action, session_id);
	
	//保存flash
	flash_save_global_variables();
	
	//函数返回
	return net_recv_server_gps_switch_ret.RET_OK;
}

/*
	网络重新连接
*/
extern uint8_t Feed_dog;
uint32_t ICCID_steck = 0;
int net_wait_reconnect(int timeout_ms_in)
{
	struct net_wait_reconnect_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
		const int RET_TIMEOUT;
	}net_wait_reconnect_ret = {0,1,2};
	
	//静态变量定义
	static int mqtt_msg_id=1;
	static int init_flag=0;
	
	//局部变量定义
	int state=0;
	int ret=0;
	struct topic_qos_struct topic_qos={0};
	int start_time=0;

	//计时开始
	start_time = global_timer.system_timer;
	
	//EC20退出休眠
	net_power_sleep(0);	
	//循环连接
	while(1)
	{
		//复位E20
		if(3==init_flag)
		{
			init_flag = 0;
			net_ec20_restart(10000);
		}
		init_flag++;
		
		//循环检查SIM卡ICCID
		ret = net_get_iccid(global_device_info.iccid);
		if(0!=ret)
		{
			ret = net_log_write("[net]没有检查到SIM卡的ICCID\n");
			printf("[net]没有检查到SIM卡的ICCID\n");
			if(ret == net_wait_reconnect_ret.RET_TIMEOUT)
			{
				Feed_dog = 1;
				system_restart(1); 
			}
			//led_gprs_mode(LED_RED,2);
			ICCID_steck++;
			if(ICCID_steck > 5)
			{
//				 Feed_dog = 1;
//				system_restart(1); 
				
			}
			if(timeout_ms_in && (timeout_ms_in < (global_timer.system_timer - start_time)))
			{
				//led_gprs_mode(LED_RED,2);
				net_log_write("[net]连接超时,没检查到SIM卡\n");
				printf("[net]连接超时,没检查到SIM卡...\n");
				return net_wait_reconnect_ret.RET_TIMEOUT;
			}

			continue;
		}
		else
		{
			printf("[net]SIM卡ICCID为:%s\n", global_device_info.iccid);
		}

		
		//等待依附数据网络
		printf("[net]正在依附数据网络...\n");
		while(1)
		{
			power_sleep_check();
			power_down_check();
			
			net_check_packet_domain(&state);
			if(1==state)
			{
				break;
			}
			osDelay(1000);
			if(timeout_ms_in && (timeout_ms_in < (global_timer.system_timer - start_time)))
			{
				//led_gprs_mode(LED_RED,1);
				ret = net_signal_quality(&global_device_info.signal_quality);
				if(0==ret)
				{
					net_log_write("[net]当前信号强度为%d格\n", global_device_info.signal_quality);
					printf("[debug]当前信号强度为%d格\n", global_device_info.signal_quality);
				}
				net_log_write("[net]网络连接超时\n");
				printf("[net]连接超时...\n");
				return net_wait_reconnect_ret.RET_TIMEOUT;
			}
		}
		
		//进行网络连接，循环连接直到成功
		printf("[net]正在进行网络连接...\n");

		ret = net_signal_quality(&global_device_info.signal_quality);
		if(0==ret)
		{
			net_log_write("[net]当前信号强度为%d格\n", global_device_info.signal_quality);
			printf("[debug]当前信号强度为%d格\n", global_device_info.signal_quality);
		}
		osDelay(1000);
		net_mqtt_disconnect(global_mqtt_config.client_idx);
		osDelay(1000);
		net_mqtt_close(global_mqtt_config.client_idx);
		osDelay(1000);
		net_mqtt_configrate(global_mqtt_config.client_idx, &global_mqtt_config);
		osDelay(1000);
		net_mqtt_open(global_mqtt_config.client_idx,global_net_config.mqtt_hostname, global_net_config.mqtt_port);
		osDelay(1000);
		net_mqtt_connect(global_mqtt_config.client_idx, global_net_config.mqtt_client_id, global_net_config.mqtt_user_name, global_net_config.mqtt_password);
		osDelay(1000);
		mqtt_msg_id = mqtt_msg_id%65535+1;
		strcpy(topic_qos.topic, global_net_config.mqtt_sub_ins_topic);
		topic_qos.qos=0;
		ret = net_mqtt_subscribe(global_mqtt_config.client_idx, mqtt_msg_id, 1, &topic_qos);
		if(0 == ret)
		{
			//led_gprs_mode(LED_GREEN,1);
			printf("[net]网络连接成功\n");
			break;
		}
		else
		{
			//led_gprs_mode(LED_RED,1);
			net_log_write("[net]网络连接失败\n");
			printf("[net]网络连接失败\n");
		}
	}
	return net_wait_reconnect_ret.RET_OK;
}

/*
	获取服务器时间
*/
int net_device_get_time(void)
{
	struct net_device_get_time_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_device_get_time_ret = {0,1};
	
	//局部变量
	char temp_string[64]={0};
	int ret=0;
	
	//准备数据
	memset(temp_buffer, 0, sizeof(temp_buffer));
	//action
	sprintf(temp_string,"device_get_time,");
	strcat(temp_buffer, temp_string);
	//uid
	sprintf(temp_string,"%s,",global_device_info.uid);
	strcat(temp_buffer, temp_string);
	//session_id
	sprintf(temp_string,"%02d%02d%02d%02d%02d%02d%02d,", \
					global_device_info.device_time.year, global_device_info.device_time.month, global_device_info.device_time.day, \
					global_device_info.device_time.hour,global_device_info.device_time.minute,global_device_info.device_time.second,\
					(global_timer.system_timer/10)%100);
	strcat(temp_buffer, temp_string);
	//date_time
	sprintf(temp_string,"%04d-%02d-%02d %02d:%02d:%02d.%02d,",\
					global_upload_data.time.year+2000, global_upload_data.time.month, global_upload_data.time.day, \
					global_upload_data.time.hour,global_upload_data.time.minute,global_upload_data.time.second,global_upload_data.ms);
	strcat(temp_buffer, temp_string);
	//lac
	if(0==strlen(global_upload_data.lac))
	{
		sprintf(temp_string,"0,");
		strcat(temp_buffer, temp_string);
	}
	else
	{
		sprintf(temp_string,"%s,",global_upload_data.lac);
		strcat(temp_buffer, temp_string);
	}
	
	//ci
	if(0==strlen(global_upload_data.ci))
	{
		sprintf(temp_string,"0");
		strcat(temp_buffer, temp_string);
	}
	else
	{
		sprintf(temp_string,"%s",global_upload_data.ci);
		strcat(temp_buffer, temp_string);
	}
	
	//发布消息
	ret = net_mqtt_publish(global_mqtt_config.client_idx, 0, 0, 0, global_net_config.mqtt_pub_upload_topic, strlen(temp_buffer) , temp_buffer, global_device_config.beep_mode);
	if(0!=ret)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_device_get_time_ret.RET_ERROR;
	}
	ret = net_mqtt_publish(global_mqtt_config.client_idx, 0, 0, 0, global_net_config.mqtt_pub_upload_topic_gw, strlen(temp_buffer) , temp_buffer, global_device_config.beep_mode);
	if(0!=ret)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_device_get_time_ret.RET_ERROR;
	}
	//函数返回
	return net_device_get_time_ret.RET_OK;
}

/*
	服务器推送时间信息
*/
int net_recv_server_push_time(char *buffer_in)
{
	struct net_recv_server_push_time_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_recv_server_push_time_ret = {0,1};
	
	//局部变量
	char *p=NULL;
	char action[64]={0};
	char session_id[64]={0};
	int temp_year=0;
	int temp_month=0;
	int temp_day=0;
	int temp_hour=0;
	int temp_minute=0;
	int temp_second=0;
	int ret=0;
	int temp_weekday=0;
	
	//参数检查
	if(NULL == buffer_in)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_push_time_ret.RET_ERROR;
	}

	//解析出action
	p = strstr(buffer_in, "\"action\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"action\":\"%[^\"]", action);
	}

	//解析出session_id
	p = strstr(buffer_in, "\"session_id\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"session_id\":\"%[^\"]", session_id);
	}
	
	//解析出服务器时间
	p = strstr(buffer_in, "\"server_time\":\"");
	if(NULL!=p)
	{
		ret=sscanf(p, "\"server_time\":\"20%02d-%02d-%02d %02d:%02d:%02d\"", \
			&temp_year,
			&temp_month,
			&temp_day,
			&temp_hour,
			&temp_minute,
			&temp_second);
		if(6==ret)
		{
			global_rtc_set_time.time_for_set.year=temp_year;
			global_rtc_set_time.time_for_set.month=temp_month;
			global_rtc_set_time.time_for_set.day=temp_day;
			global_rtc_set_time.time_for_set.hour=temp_hour;
			global_rtc_set_time.time_for_set.minute=temp_minute;
			global_rtc_set_time.time_for_set.second=temp_second;
			global_rtc_set_time.need_set=1;
		}
	}
	
	//解析出星期
	p = strstr(buffer_in, "\"week_day\":\"");
	if(NULL!=p)
	{
		ret=sscanf(p, "\"week_day\":\"%d\"", &temp_weekday);
		if(1==ret)
		{
			global_rtc_set_time.time_for_set.weekday = temp_weekday;
		}
	}
	
	//解析纬度
	p = strstr(buffer_in, "\"latitude\":\"");
	if(NULL!=p)
	{
		//待开发
	}
	
	//解析经度
	p = strstr(buffer_in, "\"longitude\":\"");
	if(NULL!=p)
	{
		//待开发
	}
	
	//发送确认消息
	net_rollback_msg(action, session_id);
	
	
	//函数返回
	return net_recv_server_push_time_ret.RET_OK;
}

/*
	发送按下打印按钮打印本地60组日志消息
*/
int net_client_local_printed(void)
{
	//返回值定义
	struct net_client_local_printed_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_client_local_printed_ret={0,1};
	
	//局部变量定义
	char temp_string[64]={0};
	int ret=0;
	
	//拼接报文
	memset(temp_buffer, 0, sizeof(temp_buffer));
	//action
	sprintf(temp_string,"client_local_printed,");
	strcat(temp_buffer, temp_string);
	//uid
	sprintf(temp_string,"%s,",global_device_info.uid);
	strcat(temp_buffer, temp_string);
	//session_id
	sprintf(temp_string,"%02d%02d%02d%02d%02d%02d%02d,", \
					global_device_info.device_time.year, global_device_info.device_time.month, global_device_info.device_time.day, \
					global_device_info.device_time.hour,global_device_info.device_time.minute,global_device_info.device_time.second,\
					(global_timer.system_timer/10)%100);
	strcat(temp_buffer, temp_string);
	//date_time
	sprintf(temp_string,"%04d-%02d-%02d %02d:%02d:%02d,",\
					global_upload_data.time.year+2000, global_upload_data.time.month, global_upload_data.time.day, \
					global_upload_data.time.hour,global_upload_data.time.minute,global_upload_data.time.second);
	strcat(temp_buffer, temp_string);
	//location
	if(1==global_upload_data.location_mode)
	{
		//GPS定位无效
		sprintf(temp_string,"0.000000,0.000000,");
		strcat(temp_buffer, temp_string);
	}
	else
	{
		sprintf(temp_string,"%f%s,%f%s,", \
			global_upload_data.longitude, \
			global_upload_data.EW, \
			global_upload_data.latitude, \
			global_upload_data.NS);
		strcat(temp_buffer, temp_string);
	}
	//lac
	sprintf(temp_string,"%s,",global_upload_data.lac);
	strcat(temp_buffer, temp_string);
	//ci
	sprintf(temp_string,"%s",global_upload_data.ci);
	strcat(temp_buffer, temp_string);
	
	//发布消息
	ret = net_mqtt_publish(global_mqtt_config.client_idx, 0, 0, 0, global_net_config.mqtt_pub_upload_topic, strlen(temp_buffer) , temp_buffer, global_device_config.beep_mode);
	if(0!=ret)
	{
		printf("[net]error code=%d. file=%s, line=%d.\n", ret, __FILE__, __LINE__);
		return net_client_local_printed_ret.RET_ERROR;
	}
	ret = net_mqtt_publish(global_mqtt_config.client_idx, 0, 0, 0, global_net_config.mqtt_pub_upload_topic_gw, strlen(temp_buffer) , temp_buffer, global_device_config.beep_mode);
	if(0!=ret)
	{
		printf("[net]error code=%d. file=%s, line=%d.\n", ret, __FILE__, __LINE__);
		return net_client_local_printed_ret.RET_ERROR;
	}
	//函数返回
	return net_client_local_printed_ret.RET_OK;
}

/*
	服务器让设备上传本地60组最大值日志
*/
int net_recv_server_get_local_max_datalog(char *buffer_in)
{
	struct net_recv_server_get_local_max_datalog_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_recv_server_get_local_max_datalog_ret = {0,1};
	
	//局部变量
	char *p=NULL;
	char action[64]={0};
	char session_id[64]={0};
	int ret=0;
	char file_name[256]={0};
	
	//参数检查
	if(NULL == buffer_in)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_get_local_max_datalog_ret.RET_ERROR;
	}

	//解析出action
	p = strstr(buffer_in, "\"action\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"action\":\"%[^\"]", action);
	}

	//解析出session_id
	p = strstr(buffer_in, "\"session_id\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"session_id\":\"%[^\"]", session_id);
	}
	
	//发送确认消息
	net_rollback_msg(action, session_id);
	
	//上传本地60组最大值日志
	ret = net_upload_local_maxdata_log(file_name);
	if(0!=ret)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_get_local_max_datalog_ret.RET_ERROR;
	}
	
	//发送上传本地最大值日志成功通知
	ret = net_upload_log_msg(file_name, "local_maxdata_log");
	if(0!=ret)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_get_local_max_datalog_ret.RET_ERROR;
	}
	
	//函数返回
	return net_recv_server_get_local_max_datalog_ret.RET_OK;
}

/*
	ftp上传本地模式60组最大值
*/
int net_upload_local_maxdata_log(char *file_name_out)
{
	struct net_upload_local_maxdata_log_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_upload_local_maxdata_log_ret = {0,1};
	
	//局部变量
	int ret=0;
	char file_name[256]={0};
	int i=0;
	int start_pos=0;
	int j=0;
	int beof=0;
	
	//检查参数
	if(NULL == file_name_out)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_upload_local_maxdata_log_ret.RET_ERROR;
	}
	
	//关闭FTP连接
	ret=net_ftp_close();
	
	
	//配置ftp
	ret=net_ftp_configrate(&global_ftp_config);

	
	//打开FTP连接
	ret=net_ftp_open("119.3.241.184", 18356);

	
	//配置路径(服务器配置信息里面有)
	//sprintf(global_ftp_config.max_data_path, "\\maxdata_log\\%s",global_device_info.uid);
	ret=net_ftp_cwd(global_ftp_config.max_data_path);
	
	//上传数据
	memset(payload,0,sizeof(payload));
	memset(temp_buffer,0,sizeof(temp_buffer));
	
	sprintf(file_name,"maxdata_log_20%02d%02d%02d%02d%02d%02d.txt",\
		global_device_info.device_time.year,\
		global_device_info.device_time.month,\
		global_device_info.device_time.day,\
		global_device_info.device_time.hour,\
		global_device_info.device_time.minute,\
		global_device_info.device_time.second);
	
	sprintf(temp_buffer,"%s,local_vibration,%s;\n",global_task_info.task_num,global_device_info.uid);
	
	start_pos=0;
	
	ret=net_ftp_put_COM(file_name, start_pos, strlen(temp_buffer), beof, temp_buffer);
	if(0!=ret)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_upload_local_maxdata_log_ret.RET_ERROR;
	}
	
	start_pos += strlen(temp_buffer);
	
	for(j=0;j<6;j++)
	{
		memset(payload,0,sizeof(payload));
		
		for(i=0;i<10;i++)
		{
			sprintf(temp_buffer,"%.2f,%.2f,%.2f,%f,%f,20%02d-%02d-%02d %02d:%02d:%02d;\n",\
			global_local_log_vibration.global_local_log_vibration[i+j*10].data_x,\
			global_local_log_vibration.global_local_log_vibration[i+j*10].data_y,\
			global_local_log_vibration.global_local_log_vibration[i+j*10].data_z,\
			global_local_log_vibration.global_local_log_vibration[i+j*10].latitude,\
			global_local_log_vibration.global_local_log_vibration[i+j*10].longitude,\
			global_local_log_vibration.global_local_log_vibration[i+j*10].time.year,\
			global_local_log_vibration.global_local_log_vibration[i+j*10].time.month,\
			global_local_log_vibration.global_local_log_vibration[i+j*10].time.day,\
			global_local_log_vibration.global_local_log_vibration[i+j*10].time.hour,\
			global_local_log_vibration.global_local_log_vibration[i+j*10].time.minute,\
			global_local_log_vibration.global_local_log_vibration[i+j*10].time.second);
			
			strcat(payload,temp_buffer);
		}
		
		if(5==j)
		{
			beof=1;
		}
		ret = net_ftp_put_COM(file_name, start_pos, strlen(payload), beof, payload);
		if(0!=ret)
		{
			printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
			return net_upload_local_maxdata_log_ret.RET_ERROR;
		}
		start_pos += strlen(payload);	
	}
	
	//输出
	strcpy(file_name_out,file_name);
	
	//函数返回
	return net_upload_local_maxdata_log_ret.RET_OK;
	
}

/*
	上传设备信息
*/
int net_upload_device_info(void)
{
	struct net_upload_device_info_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_upload_device_info_ret = {0,1};
	
	//局部变量
	char temp_string[64]={0};
	int ret=0;
	
	//准备数据
	memset(temp_buffer, 0, sizeof(temp_buffer));
	//action
	sprintf(temp_string,"upload_device_info,");
	strcat(temp_buffer, temp_string);
	//uid
	sprintf(temp_string,"%s,",global_device_info.uid);
	strcat(temp_buffer, temp_string);
	//session_id
	sprintf(temp_string,"%02d%02d%02d%02d%02d%02d%02d,", \
					global_device_info.device_time.year, global_device_info.device_time.month, global_device_info.device_time.day, \
					global_device_info.device_time.hour,global_device_info.device_time.minute,global_device_info.device_time.second,\
					(global_timer.system_timer/10)%100);
	strcat(temp_buffer, temp_string);
	//date_time
	sprintf(temp_string,"%04d-%02d-%02d %02d:%02d:%02d.%02d,",\
					global_upload_data.time.year+2000, global_upload_data.time.month, global_upload_data.time.day, \
					global_upload_data.time.hour,global_upload_data.time.minute,global_upload_data.time.second,global_upload_data.ms);
	strcat(temp_buffer, temp_string);
	//任务号
	if(0==global_task_info.task_num[0])
	{
		sprintf(temp_string,"0,");
		strcat(temp_buffer, temp_string);
	}
	else
	{
		sprintf(temp_string,"%s,",global_task_info.task_num);
		strcat(temp_buffer, temp_string);
	}
	//振动阈值
	sprintf(temp_string,"%.2f,%.2f,%.2f,",\
			(double)0,\
			(double)0,\
			(double)0);
	strcat(temp_buffer, temp_string);
	//倾角阈值
	sprintf(temp_string,"%.2f,%.2f,",\
			(double)0,\
			(double)0);
	strcat(temp_buffer, temp_string);
	//压力阈值
	sprintf(temp_string,"%.2f,%.2f,",(double)0, (double)0);
	strcat(temp_buffer, temp_string);
	//温度阈值
	sprintf(temp_string,"%.2f,%.2f,",(double)0,(double)0);
	strcat(temp_buffer, temp_string);
	//唤醒时间
	sprintf(temp_string,"%d,",global_device_config.awake_time);
	strcat(temp_buffer, temp_string);
	//休眠时间
	sprintf(temp_string,"%d,",global_device_config.sleep_time);
	strcat(temp_buffer, temp_string);
	//上传间隔
	sprintf(temp_string,"%d,",global_device_config.interval);
	strcat(temp_buffer, temp_string);
	//GPS状态
	sprintf(temp_string,"%d,",global_device_config.gnss_mode);
	strcat(temp_buffer, temp_string);
	//电量
	sprintf(temp_string,"%d,",global_device_info.dump_energy);
	strcat(temp_buffer, temp_string);
	//ICCID
	sprintf(temp_string,"%s,",global_device_info.iccid);
	strcat(temp_buffer, temp_string);
	//蜂鸣器模式
	sprintf(temp_string,"%d,",global_device_config.beep_mode);
	strcat(temp_buffer, temp_string);
	//振动系数
	sprintf(temp_string,"%g:%g:%g:%g:%g:%g:%g:%g:%g:%g:%g:%g,", \
		global_device_config.vibration_1_x_a,\
		global_device_config.vibration_1_x_b,\
		global_device_config.vibration_1_y_a,\
		global_device_config.vibration_1_y_b,\
		global_device_config.vibration_1_z_a,\
		global_device_config.vibration_1_z_b,\
		global_device_config.vibration_2_x_a,\
		global_device_config.vibration_2_x_b,\
		global_device_config.vibration_2_y_a,\
		global_device_config.vibration_2_y_b,\
		global_device_config.vibration_2_z_a,\
		global_device_config.vibration_2_z_b);
		
	strcat(temp_buffer, temp_string);
	
	//rf433模式
	sprintf(temp_string,"%d,",global_device_config.rf433_mode);
	strcat(temp_buffer, temp_string);
	//休眠网络开关模式
	sprintf(temp_string,"%d,",global_device_config.sleep_network);
	strcat(temp_buffer, temp_string);
	//设备SN码
	sprintf(temp_string,"%s,",global_device_info.terminal_sn);
	strcat(temp_buffer, temp_string);
	//数据上传开关
	sprintf(temp_string,"%d,",global_flag.upload_enable_flag);
	strcat(temp_buffer, temp_string);
	//数据安装姿态
	sprintf(temp_string,"%d,",0);
	strcat(temp_buffer, temp_string);
	//设备电压
	sprintf(temp_string,"%.2f,",global_device_info.battery_voltage);
	strcat(temp_buffer, temp_string);
	//版本号
	sprintf(temp_string,"%s",global_device_info.Version_number);
	strcat(temp_buffer, temp_string);
	//发布消息
	ret = net_mqtt_publish(global_mqtt_config.client_idx, 0, 0, 0, global_net_config.mqtt_pub_upload_topic, strlen(temp_buffer) , temp_buffer, global_device_config.beep_mode);
	if(0!=ret)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_upload_device_info_ret.RET_ERROR;
	}
	ret = net_mqtt_publish(global_mqtt_config.client_idx, 0, 0, 0, global_net_config.mqtt_pub_upload_topic_gw, strlen(temp_buffer) , temp_buffer, global_device_config.beep_mode);
	if(0!=ret)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_upload_device_info_ret.RET_ERROR;
	}
	//函数返回
	return net_upload_device_info_ret.RET_OK;
}

/*
	服务器让设备传输设备信息
*/
int net_recv_server_get_device_info(char *buffer_in)
{
	struct net_recv_server_get_device_info_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_recv_server_get_device_info_ret = {0,1};
	
	//局部变量
	char *p=NULL;
	char action[64]={0};
	char session_id[64]={0};
	
	//参数检查
	if(NULL == buffer_in)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_get_device_info_ret.RET_ERROR;
	}
	
	//解析出action
	p = strstr(buffer_in, "\"action\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"action\":\"%[^\"]", action);
	}

	//解析出session_id
	p = strstr(buffer_in, "\"session_id\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"session_id\":\"%[^\"]", session_id);
	}
	
	//发送确认消息
	net_rollback_msg(action, session_id);
	
	//发送设备信息
	net_upload_device_info();
	
	//函数返回
	return net_recv_server_get_device_info_ret.RET_OK;
}

/*
	服务器让设备开启gps持续一段时间
*/
int net_recv_server_gps_switch_second(char *buffer_in)
{
	struct net_recv_server_gps_switch_second_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_recv_server_gps_switch_second_ret = {0,1};
	
	//局部变量
	char *p=NULL;
	char action[64]={0};
	char session_id[64]={0};
	int second = 0;
	
	//参数检查
	if(NULL == buffer_in)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_gps_switch_second_ret.RET_ERROR;
	}
	
	//解析出action
	p = strstr(buffer_in, "\"action\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"action\":\"%[^\"]", action);
	}

	//解析出session_id
	p = strstr(buffer_in, "\"session_id\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"session_id\":\"%[^\"]", session_id);
	}
	
	//解析出 second
	p = strstr(buffer_in, "\"second\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"second\":\"%d", &second);
	}
	
	//发送确认消息
	net_rollback_msg(action, session_id);
	
	//切换gps模式
	global_device_config.gnss_once_time = second;
	global_timer.gps_timer_sec = 0;
	global_device_config.gnss_once_flag=1;
	

	//函数返回
	return net_recv_server_gps_switch_second_ret.RET_OK;
}

/*
	ftp上传运行日志
*/
int net_upload_run_log(char *file_name_out)
{
	struct net_upload_run_log_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_upload_run_log_ret = {0,1};
	
	//检查参数
	if(NULL == file_name_out)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		net_ftp_close();
		return net_upload_run_log_ret.RET_ERROR;
	}
	
	//关闭FTP连接
	net_ftp_close();
	
	//配置ftp
	net_ftp_configrate(&global_ftp_config);

	//打开FTP连接
	net_ftp_open("119.3.241.184", 18356);

	//配置路径(服务器配置信息里面有)
	//sprintf(global_ftp_config.max_data_path, "\\maxdata_log\\%s",global_device_info.uid);
	net_ftp_cwd(global_ftp_config.run_log_path);
	
	//上传文件
	net_ftp_put_FILE("run_log.txt", "UFS:run_log.txt", 0);
	
	//输出
	strcpy(file_name_out,"run_log.txt");
	
	//函数返回
	net_ftp_close();
	return net_upload_run_log_ret.RET_OK;
}

/*
	ftp上传振动波形
	参数可以为NULL
*/
int net_upload_wave_file(char *file_name_out)
{
	struct net_upload_wave_file_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_upload_wave_file_ret = {0,1};
	
	//局部变量
	char file_name[256]={0};

	//拼接文件名
	sprintf(file_name,"wave_log_%04d%02d%02d%02d%02d%02d%02d.txt",\
					global_upload_data.time.year+2000, global_upload_data.time.month, global_upload_data.time.day, \
					global_upload_data.time.hour,global_upload_data.time.minute,global_upload_data.time.second,global_upload_data.ms);
	
	//关闭FTP连接
	net_ftp_close();
	
	//配置ftp
	net_ftp_configrate(&global_ftp_config);

	//打开FTP连接
	net_ftp_open("119.3.241.184", 18356);

	//配置路径(服务器配置信息里面有)
	//sprintf(global_ftp_config.max_data_path, "\\maxdata_log\\%s",global_device_info.uid);
	net_ftp_cwd(global_ftp_config.wave_path);
	
	//上传文件
	net_ftp_put_FILE(file_name, "UFS:wave_log.txt", 0);
	
	//输出
	if(NULL != file_name_out)
	{
		strcpy(file_name_out,file_name);
	}
	
	//发送上传完毕报文
	net_upload_wave_msg(file_name, "wave_form");
	
	//函数返回
	net_ftp_close();
	return net_upload_wave_file_ret.RET_OK;
	
	
	/*
	RET_ERROR:
	//关闭FTP连接
	net_ftp_close();
	return net_upload_wave_file_ret.RET_ERROR;
	*/
}

/*
	终端发送上传波形状态信息
*/
int net_upload_wave_msg(char *file_name, char *log_type)
{
	struct net_upload_wave_msg_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_upload_wave_msg_ret = {0,1};
	
	//局部变量
	char temp_string[64]={0};
	int ret=0;
	
	//参数检查
	if(NULL==file_name)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_upload_wave_msg_ret.RET_ERROR;
	}
	
	if(NULL==log_type)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_upload_wave_msg_ret.RET_ERROR;
	}
	
	//发送mqtt上传成功通知报文
	//拼接报文
	memset(temp_buffer, 0, sizeof(temp_buffer));
	//action
	sprintf(temp_string,"net_upload_wave,");
	strcat(temp_buffer, temp_string);
	//uid
	sprintf(temp_string,"%s,",global_device_info.uid);
	strcat(temp_buffer, temp_string);
	//session_id
	sprintf(temp_string,"%02d%02d%02d%02d%02d%02d%02d,",\
					global_device_info.device_time.year, global_device_info.device_time.month, global_device_info.device_time.day,\
					global_device_info.device_time.hour,global_device_info.device_time.minute,global_device_info.device_time.second,\
					(global_timer.system_timer/10)%100);
	strcat(temp_buffer, temp_string);
	//date_time
	sprintf(temp_string,"%04d-%02d-%02d %02d:%02d:%02d,",\
					global_device_info.device_time.year+2000, global_device_info.device_time.month, global_device_info.device_time.day,\
					global_device_info.device_time.hour,global_device_info.device_time.minute,global_device_info.device_time.second);
	strcat(temp_buffer, temp_string);
	//file_name
	sprintf(temp_string,"%s,",file_name);
	strcat(temp_buffer, temp_string);
	//log type
	sprintf(temp_string,"%s",log_type);
	strcat(temp_buffer, temp_string);
	
	//发布消息
	ret = net_mqtt_publish(global_mqtt_config.client_idx, 0, 0, 0, global_net_config.mqtt_pub_wave_topic, strlen(temp_buffer) , temp_buffer, global_device_config.beep_mode);
	if(0!=ret)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_upload_wave_msg_ret.RET_ERROR;
	}
	
	//函数返回
	return net_upload_wave_msg_ret.RET_OK;
}

/*
	服务器控制蜂鸣器开关
*/
int net_recv_server_beep_switch(char *buffer_in)
{
	struct net_recv_server_beep_switch_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_recv_server_beep_switch_ret = {0,1};
	
	//局部变量
	char *p=NULL;
	char action[64]={0};
	char session_id[64]={0};
	
	//参数检查
	if(NULL == buffer_in)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_beep_switch_ret.RET_ERROR;
	}

	//解析出action
	p = strstr(buffer_in, "\"action\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"action\":\"%[^\"]", action);
	}

	//解析出session_id
	p = strstr(buffer_in, "\"session_id\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"session_id\":\"%[^\"]", session_id);
	}
	
	//解析 switch_status
	p = strstr(buffer_in, "\"switch_status\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"switch_status\":\"%d\"", &global_device_config.beep_mode);
	}
	
	//发送确认消息
	net_rollback_msg(action, session_id);
	
	//保存flash
	flash_save_global_variables();
	
	//函数返回
	return net_recv_server_beep_switch_ret.RET_OK;
}

/*
	服务器设置振动系数
*/
int net_recv_server_set_coefficient(char *buffer_in)
{
	struct net_recv_server_set_coefficient_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_recv_server_set_coefficient_ret = {0,1};
	
	//局部变量
	char *p=NULL;
	char action[64]={0};
	char session_id[64]={0};
	
	float v_1_x_a=0;
	float v_1_x_b=0;
	float v_1_y_a=0;
	float v_1_y_b=0;
	float v_1_z_a=0;
	float v_1_z_b=0;
	
	float v_2_x_a=0;
	float v_2_x_b=0;
	float v_2_y_a=0;
	float v_2_y_b=0;
	float v_2_z_a=0;
	float v_2_z_b=0;
	
	float v_temp=0;
	int ret=0;
	
	//参数检查
	if(NULL == buffer_in)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_set_coefficient_ret.RET_ERROR;
	}

	//解析出action
	p = strstr(buffer_in, "\"action\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"action\":\"%[^\"]", action);
	}

	//解析出session_id
	p = strstr(buffer_in, "\"session_id\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"session_id\":\"%[^\"]", session_id);
	}
	
	//解析 value
	p = strstr(buffer_in, "\"value\":\"");
	if(NULL!=p)
	{
		ret = sscanf(p, "\"value\":\"%f:%f:%f:%f:%f:%f:%f:%f:%f:%f:%f:%f\"", &v_1_x_a, &v_1_x_b, &v_1_y_a, &v_1_y_b, &v_1_z_a, &v_1_z_b,\
			&v_2_x_a, &v_2_x_b, &v_2_y_a, &v_2_y_b, &v_2_z_a, &v_2_z_b);
		if(12==ret)
		{
			global_device_config.vibration_1_x_a = v_1_x_a;
			global_device_config.vibration_1_x_b = v_1_x_b;
			global_device_config.vibration_1_y_a = v_1_y_a;
			global_device_config.vibration_1_y_b = v_1_y_b;
			global_device_config.vibration_1_z_a = v_1_z_a;
			global_device_config.vibration_1_z_b = v_1_z_b;
			
			global_device_config.vibration_2_x_a = v_2_x_a;
			global_device_config.vibration_2_x_b = v_2_x_b;
			global_device_config.vibration_2_y_a = v_2_y_a;
			global_device_config.vibration_2_y_b = v_2_y_b;
			global_device_config.vibration_2_z_a = v_2_z_a;
			global_device_config.vibration_2_z_b = v_2_z_b;
		}
		else
		{
			ret = sscanf(p, "\"value\":\"%f\"", &v_temp);
			if(1==ret)
			{
				global_device_config.vibration_1_x_a = v_temp;
				global_device_config.vibration_1_x_b = 0;
				global_device_config.vibration_1_y_a = v_temp;
				global_device_config.vibration_1_y_b = 0;
				global_device_config.vibration_1_z_a = v_temp;
				global_device_config.vibration_1_z_b = 0;
				
				global_device_config.vibration_2_x_a = v_temp;
				global_device_config.vibration_2_x_b = 0;
				global_device_config.vibration_2_y_a = v_temp;
				global_device_config.vibration_2_y_b = 0;
				global_device_config.vibration_2_z_a = v_temp;
				global_device_config.vibration_2_z_b = 0;
			}
		}
	}
	
	//发送确认消息
	net_rollback_msg(action, session_id);
	
	//保存flash
	flash_save_global_variables();
	
	//函数返回
	return net_recv_server_set_coefficient_ret.RET_OK;
}

/*
	服务器控制休眠时网络模块开关
*/
int net_recv_server_sleep_network_switch(char *buffer_in)
{
	struct net_recv_server_sleep_network_switch_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_recv_server_sleep_network_switch_ret = {0,1};
	
	//局部变量
	char *p=NULL;
	char action[64]={0};
	char session_id[64]={0};
	
	//参数检查
	if(NULL == buffer_in)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_sleep_network_switch_ret.RET_ERROR;
	}

	//解析出action
	p = strstr(buffer_in, "\"action\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"action\":\"%[^\"]", action);
	}

	//解析出session_id
	p = strstr(buffer_in, "\"session_id\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"session_id\":\"%[^\"]", session_id);
	}
	
	//解析 switch_status
	p = strstr(buffer_in, "\"switch_status\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"switch_status\":\"%d\"", &global_device_config.sleep_network);
	}
	
	//发送确认消息
	net_rollback_msg(action, session_id);
	
	//保存flash
	flash_save_global_variables();
	
	//函数返回
	return net_recv_server_sleep_network_switch_ret.RET_OK;
}

/*
	服务器拉取设备运行日志
*/
int net_recv_server_get_run_log_file(char *buffer_in)
{
	struct net_recv_server_get_run_log_file_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_recv_server_get_run_log_file_ret = {0,1};
	
	//局部变量
	char *p=NULL;
	char action[64]={0};
	char session_id[64]={0};
	char run_log_name[128]={0};
	
	//参数检查
	if(NULL == buffer_in)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_get_run_log_file_ret.RET_ERROR;
	}

	//解析出action
	p = strstr(buffer_in, "\"action\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"action\":\"%[^\"]", action);
	}

	//解析出session_id
	p = strstr(buffer_in, "\"session_id\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"session_id\":\"%[^\"]", session_id);
	}
	
	//发送确认消息
	net_rollback_msg(action, session_id);
	
	//上传运行日志
	net_upload_run_log(run_log_name);
	
	//函数返回
	return net_recv_server_get_run_log_file_ret.RET_OK;
}


/*
	服务器让设备关机
*/
int net_recv_server_shut_down(char *buffer_in)
{
	struct net_recv_server_shut_down_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_recv_server_shut_down_ret = {0,1};
	
	//局部变量
	char *p=NULL;
	char action[64]={0};
	char session_id[64]={0};
	
	//参数检查
	if(NULL == buffer_in)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_shut_down_ret.RET_ERROR;
	}

	//解析出action
	p = strstr(buffer_in, "\"action\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"action\":\"%[^\"]", action);
	}

	//解析出session_id
	p = strstr(buffer_in, "\"session_id\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"session_id\":\"%[^\"]", session_id);
	}
	
	//发送确认消息
	net_rollback_msg(action, session_id);
	
	//执行关机准备
	global_flag.net_shutdown_flag = 1;
	
	//函数返回
	return net_recv_server_shut_down_ret.RET_OK;
}

/*
	国网服务器让设备远程升级
*/
extern uint32_t tim2_tim[3],DMA_UART2_sum[3],sum_C;
extern char recv_buffer[1024];
char app_descriptor_str[APP_DESC_BUF_MAX] = APP_DESCRIPTOR;  
int net_recv_server_sys_upgrade_gw(char *buffer_in)
{
	struct net_recv_server_sys_upgrade_gw_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_recv_server_sys_upgrade_gw_ret = {0,1};
	
	//局部变量
	char *p=NULL;
	uint8_t temp_buf[512] = {0};
	char action[64]={0};
	char session_id[64]={0};
//	char Add_check_buf[8] = {0};
//	uint32_t firmware_end = 0x4B4F0A0D;
	uint32_t iLoop = 0;
	char temp_len[3],version_d = 0,version_y = 0,iLen = 0;
	p = strstr(buffer_in, "\"size\":");
	if(NULL!=p)
	{
		sscanf(p, "\"size\":%[^\",]", temp_len);
	}
	for(iLen = 0;iLen < 3;iLen++)
	{
		  if(temp_len[iLen] == 0)
			{
				break;
			}
	}
	global_remote_upgrade_info.upgrade_len = 0;
	for(iLoop = 0;iLoop < iLen;iLoop++)
	{
		  global_remote_upgrade_info.upgrade_len = global_remote_upgrade_info.upgrade_len * 10 + (temp_len[iLoop]-0x30);
	}
	//参数检查
	if(NULL == buffer_in)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_sys_upgrade_gw_ret.RET_ERROR;
	}
//	//解析出mid
//	p = strstr(buffer_in, "\"mid\":");
//	if(NULL!=p)
//	{
//		sscanf(p, "\"mid\":%[^\",]", global_remote_upgrade_info.upgrade_mid);
//	}
//	//解析出jobId 
//	p = strstr(buffer_in, "\"jobId\":");
//	if(NULL!=p)
//	{
//		sscanf(p, "\"jobId\":%[^\",]", global_remote_upgrade_info.upgrade_jobId);
//	}
//	//解析出md5
//	p = strstr(buffer_in, "\"md5\":\"");
//	if(NULL!=p)
//	{
//		sscanf(p, "\"md5\":\"%[^\"]", global_remote_upgrade_info.upgrade_md5);
//	}

	//解析出url
	p = strstr(buffer_in, "\"file_url\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"file_url\":\"%[^\"]", global_remote_upgrade_info.upgrade_url);
	}
	//解析出time
	p = strstr(buffer_in, "\"server_time\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"server_time\":\"%[^\"]", global_remote_upgrade_info.upgrade_timestamp);
	}
//	GW_push_time(buffer_in);
		//解析出version
	p = strstr(buffer_in, "\"version_num\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"version_num\":\"%[^\"]", global_remote_upgrade_info.upgrade_version);
	}	
	p = strstr(buffer_in, "\"action\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"action\":\"%[^\"]", action);
	}

	//解析出session_id
	p = strstr(buffer_in, "\"session_id\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"session_id\":\"%[^\"]", session_id);
	}
//  net_rollback_msg(action, session_id);
	net_registration_device("upgrade");

	for(iLoop = 0;iLoop < strlen(global_remote_upgrade_info.upgrade_version);iLoop++)
	{
			if(global_remote_upgrade_info.upgrade_version[iLoop] == '.')
			{
				continue;	
			}
			version_d = version_d * 10 + (global_remote_upgrade_info.upgrade_version[iLoop]-0x30);
  }
	for(iLoop = 0;iLoop < strlen(app_descriptor_str+15);iLoop++)
	{
			if(app_descriptor_str[iLoop+15] == '.')
			{
				continue;	
			}
			if(app_descriptor_str[iLoop+15] == 'V')
			{
				continue;	
			}
			version_y = version_y * 10 + (app_descriptor_str[iLoop+15]-0x30);
  }
	if(version_d >= version_y)
	{			
			uint32_t iLoop = 0,iLen = 0;
			iLen = strlen(buffer_in);
			temp_buf[0] = APPLICATION_UPDATE_REQUEST;
			for(iLoop = 0;iLoop < iLen;iLoop++)
			{
					temp_buf[iLoop + 1] = buffer_in[iLoop];
			}
	}
	global_timer.sleep_timer_sec = 0;
	

	if(version_d >= version_y)
	{ 
		  
			flash_write(14,temp_buf,512);  
//		  M41T65_write_data(0xFF,9);
		    feed_dog_func(1);
			system_restart(1); 
	}
	printf("版本错误\n");
		  

//....................................................................................................

	//执行升级准备
//	global_remote_upgrade_info.need_upgrade_flag=1;
	
	//函数返回
	return net_recv_server_sys_upgrade_gw_ret.RET_OK;
}

/*
	国网上传数据报文
*/
int net_upload_data_gw(uint8_t data_packet)
{
	struct net_upload_data_gw_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_upload_data_gw_ret = {0,1};
	
	//局部变量
	char temp_string[64]={0};
	int ret=0;
	int iLoop = 0,iNum = 0;
//	int ciphetext_length=0;
//	uint32_t base64_length=0;
	
	//准备数据
	memset(temp_buffer, 0, sizeof(temp_buffer));
	//
//	sprintf(temp_string,"{\"deviceId\":\"");
//	strcat(temp_buffer, temp_string);
	
	//deviceId
	sprintf(temp_string,"%s",global_device_info.terminal_sn);
	strcat(temp_buffer, temp_string);
	
	//
	sprintf(temp_string,",");
	strcat(temp_buffer, temp_string);
	
	//timestamp
	sprintf(temp_string,"%04d-%02d-%02d %02d:%02d:%02d",\
					global_upload_data.time.year+2000, global_upload_data.time.month, global_upload_data.time.day, \
					global_upload_data.time.hour,global_upload_data.time.minute,global_upload_data.time.second);
	strcat(temp_buffer, temp_string);
	
	//
	sprintf(temp_string,",");
	strcat(temp_buffer, temp_string);
	
	//type
	sprintf(temp_string,"YSZD_METRICS");
	strcat(temp_buffer, temp_string);
	
	//
	sprintf(temp_string,";");//{\"2\":{\"1\":\"");
	strcat(temp_buffer, temp_string);
	
	
	for(iLoop = 0;iLoop < data_packet;iLoop++)
	{
		if(iLoop > 0)
		{
			if(global_upload_stack.rear-iLoop > 0)
			{
			    global_upload_data = global_upload_pool[global_upload_stack.rear -(1+iLoop)];
			}
			else
			{
				break;
			}
		}
		iNum++;
//		sprintf(temp_string,"{\"2\":{\"1\":\"");
//		strcat(temp_buffer, temp_string);
		
		//1 GPRS 信号强度 db
		sprintf(temp_string,"%d", global_upload_data.signal);
		strcat(temp_buffer, temp_string);
		
		//
		sprintf(temp_string,",");
		strcat(temp_buffer, temp_string);
		
		//2 当前电源容量值 $
		sprintf(temp_string,"%d", global_upload_data.dump_energy);
		strcat(temp_buffer, temp_string);
		
		//
		sprintf(temp_string,",");
		strcat(temp_buffer, temp_string);
		
		//3 当前运输速度值 千米/小时
		sprintf(temp_string,"%d",(int)global_upload_data.speed);
		strcat(temp_buffer, temp_string);
		
		//
		sprintf(temp_string,",");
		strcat(temp_buffer, temp_string);
//		sprintf(temp_string,"%f%s,%f%s,", \
//			global_upload_data.longitude, \
//			global_upload_data.EW, \
//			global_upload_data.latitude, \
//			global_upload_data.NS);		
		//4 当前装置经度位置值 度
		if(0==global_upload_data.location_mode)
		{
			if((global_upload_data.longitude == 0.000000) || (global_upload_data.latitude == 0.000000))
			{
				sprintf(temp_string,"0.000000,0.000000,");
				strcat(temp_buffer, temp_string);
			}
			else
			{
				sprintf(temp_string,"%f%s",global_upload_data.longitude,global_upload_data.EW);
				strcat(temp_buffer, temp_string);

			
				//
				sprintf(temp_string,",");
				strcat(temp_buffer, temp_string);
				
				//5 当前装置纬度位置值 度

				sprintf(temp_string,"%f%s",global_upload_data.latitude,global_upload_data.NS);
				strcat(temp_buffer, temp_string);
				
				sprintf(temp_string,",");
				strcat(temp_buffer, temp_string);
			}
		}
		else
		{
			sprintf(temp_string,"0.000000,0.000000,");
			strcat(temp_buffer, temp_string);
		}
		
		//15 GPS 信号有效标志 1:有效 0:无效
		sprintf(temp_string,"%d",(1==global_upload_data.location_mode)?0:1);
		strcat(temp_buffer, temp_string);
		
		//
		sprintf(temp_string,",");
		strcat(temp_buffer, temp_string);
		
		//23 未定义
		if(0==strlen(global_upload_data.lac))
		{
			sprintf(temp_string,"0,");	
			strcat(temp_buffer, temp_string);
		}
		else
		{
			sprintf(temp_string,"%s",global_upload_data.lac);
			strcat(temp_buffer, temp_string);
			
		
		//
			sprintf(temp_string,",");
			strcat(temp_buffer, temp_string);
		}
		
		//24 未定义
		if(0==strlen(global_upload_data.ci))
		{
			sprintf(temp_string,"0,");	
			strcat(temp_buffer, temp_string);
		}
		else
		{
			sprintf(temp_string,"%s",global_upload_data.ci);
			strcat(temp_buffer, temp_string);
			
			
			//
			sprintf(temp_string,",");
			strcat(temp_buffer, temp_string);
		}
		
		//timestamp
		sprintf(temp_string,"%04d-%02d-%02d %02d:%02d:%02d.%02d",\
						global_upload_data.time.year+2000, global_upload_data.time.month, global_upload_data.time.day, \
						global_upload_data.time.hour,global_upload_data.time.minute,global_upload_data.time.second,global_upload_data.ms);//(global_timer.system_timer/10)%100);
		strcat(temp_buffer, temp_string);
		
		//
		sprintf(temp_string,";");
		strcat(temp_buffer, temp_string);
	}
	
	
	
	//是否需要加密
//	if(1==global_device_config.encryption_enable)
//	{
//		//先加密
//		cipher_ecb_encryption(global_net_config.aes_key, (uint8_t *)temp_buffer, strlen(temp_buffer),  ciphetext_buffer, sizeof(ciphetext_buffer), &ciphetext_length);
//		//清空缓存
//		memset(temp_buffer, 0, sizeof(temp_buffer));
//		//base64
//		mbedtls_base64_encode( (unsigned char *)temp_buffer, sizeof(temp_buffer), &base64_length, ciphetext_buffer, ciphetext_length );
//		//发布消息
//		
//	}
//	ret = net_mqtt_publish(global_mqtt_config.client_idx, 0, 0, 0, global_net_config.mqtt_pub_collection_topic_gw, strlen(temp_buffer) , temp_buffer, global_device_config.beep_mode);
//	if(0!=ret)
//	{
//		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
//		return net_upload_data_gw_ret.RET_ERROR;
//	}
	ret = net_mqtt_publish(global_mqtt_config.client_idx, 0, 0, 0, global_net_config.mqtt_pub_data_topic_gw, strlen(temp_buffer) , temp_buffer, global_device_config.beep_mode);
	if(0!=ret)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_upload_data_gw_ret.RET_ERROR;
	}
	for(iLoop = 0;iLoop < iNum;iLoop++)
	{
		stack_pop();
	}
	//函数返回
	return net_upload_data_gw_ret.RET_OK;
}

/*
	设备重启
*/
void system_restart(uint8_t iSign)
{
	printf("设备重启...\n");
//*********************************************************************************
//刘华科 20220104  
//*********************************************************************************	
	if(iSign == 1)
	{
			global_flag.system_restart_flag=1;
			global_flag.button_start_up_flag=0;
			
			//保存flash
			global_flag.flash_busy_flag=0;//忙标志强制清0，确保重启前flash保存上
			flash_save_global_variables();
	}
//	M41T65_write_data(0x2A,9);
	feed_dog_func(0);
	osDelay(100);
	NVIC_SystemReset();
}

