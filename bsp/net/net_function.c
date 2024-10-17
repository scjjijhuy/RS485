/*
	������幦�����
	���ߣ�lee
	ʱ�䣺2019/12/31
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
	���������
*/
int net_function_service(char *buffer_in)
{
	struct net_function_service_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_function_service_ret = {0,1};
	
	//�ֲ�����
	int ret=0;
	//int amount=0;
	
	//��������
	ret = net_mqtt_urc(buffer_in, &mqtt_urc);
	if(0==ret)
	{
		if(1==mqtt_urc.state_flag)
		{
			printf("[net]���������ѶϿ�����������\n");
			global_flag.net_reconnect_flag=1;
			return net_function_service_ret.RET_OK;
		}
		
		if(1==mqtt_urc.receive_flag && 0==mqtt_urc.receive_recv_mode)
		{
			printf("[net]MQTT ���յ���Ϣ\n");
			//��ȡpayload
			ret = net_get_payload(&mqtt_urc, payload);
			if(0!=ret)
			{
				printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
				return net_function_service_ret.RET_ERROR;	
			}
			//����
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
			printf("[net]MQTT ���յ���Ϣ\n");
			//ѭ�������ջ������Ƿ�������
			check_storage();
		}
		
		if(1==mqtt_urc.ping_flag)
		{
			printf("[net]MQTT ping ʧ��\n");
			return net_function_service_ret.RET_OK;
		}
	}
	
	//��������
	return net_function_service_ret.RET_OK;
}


/*
	��������
*/
int net_function_connect(void)
{
	struct net_function_connect_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_function_connect_ret = {0,1};
	
	//��̬����
	static int mqtt_msg_id=1;
	
	//�ֲ�����
	struct topic_qos_struct topic_qos={0};
	int ret=0;
	
	//��������
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
	//��������
	return net_function_connect_ret.RET_OK;
}

/*
	��Ϣ��ȡ����
*/
int net_get_payload(struct mqtt_urc_struct *mqtt_urc_in, char *payload_out)
{
	struct net_get_payload_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_get_payload_ret={0,1};
	
	//�ֲ�����
	int ret=0;
	struct mqtt_message_struct mqtt_message={0};
	
	//������
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
	
	//�жϽ�������
	if(0 == mqtt_urc_in->receive_recv_mode)
	{
		//ֱ����ȡ
		strcpy(payload_out,mqtt_urc_in->receive_mqtt_message.payload);
		return net_get_payload_ret.RET_OK;
	}
	else if(1 == mqtt_urc_in->receive_recv_mode)
	{
		//�Ӵ洢�ռ��л�ȡ
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
	
	//��������
	return net_get_payload_ret.RET_OK;
}

/*
	������Ϣ
*/
int net_parse(char *buffer_in)
{
	struct net_parse_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_parse_ret={0,1};
	
	//�ֲ�����
	int ret=0;
	if(NULL != strstr(buffer_in, "\"file_url\":\""))
	{
		printf("\n[net]���� YSZD_SYS_UPGRADE �ص�\n");
		ret = net_recv_server_sys_upgrade_gw(buffer_in);
		if(0!=ret)
		{
			printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
			return net_parse_ret.RET_ERROR;
		}
		return net_parse_ret.RET_OK;
	}
	//���ܷ���
	if(NULL != strstr(buffer_in, "\"action\":\"server_startup_buzzer\""))
	{
		printf("\n[net]���� server_startup_buzzer �ص�\n");
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
		printf("\n[net]���� server_device_link �ص�\n");
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
		printf("\n[net]���� server_reboot �ص�\n");
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
		printf("\n[net]���� server_get_data �ص�\n");
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
//		printf("\n[net]���� server_get_video �ص�\n");
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
		printf("\n[net]���� server_task_start �ص�\n");
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
		printf("\n[net]���� server_task_over �ص�\n");
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
		printf("\n[net]���� server_open_upload �ص�\n");
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
		printf("\n[net]���� server_off_upload �ص�\n");
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
		printf("\n[net]���� server_push_config �ص�\n");
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
		printf("\n[net]���� server_push_threshold �ص�\n");
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
//		printf("\n[net]���� server_push_videoconfig �ص�\n");
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
//		printf("\n[net]���� server_get_max_datalog �ص�\n");
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
		printf("\n[net]���� server_push_taskinfo �ص�\n");
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
//		printf("\n[net]���� server_modify_logfile �ص�\n");
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
//		printf("\n[net]���� server_modify_log �ص�\n");
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
		printf("\n[net]���� server_gps_switch �ص�\n");
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
		printf("\n[net]���� server_push_time �ص�\n");
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
		printf("\n[net]���� server_get_local_max_datalog �ص�\n");
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
		printf("\n[net]���� server_get_device_info �ص�\n");
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
		printf("\n[net]���� server_gps_switch_second �ص�\n");
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
		printf("\n[net]���� server_buzzer_switch �ص�\n");
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
		printf("\n[net]���� server_vibration_coefficient �ص�\n");
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
//		printf("\n[net]���� server_rf433_switch �ص�\n");
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
		printf("\n[net]���� server_network_switch �ص�\n");
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
		printf("\n[net]���� server_run_log_file �ص�\n");
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
		printf("\n[net]���� server_shut_down �ص�\n");
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
//		printf("\n[net]���� server_clear_sd_card �ص�\n");
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
		printf("\n[net]��������֪ͨ���\n");
		global_flag.task_on_sign = 0;
//		ret = net_recv_server_reboot(buffer_in);
//		if(0!=ret)
//		{
//			printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
//			return net_parse_ret.RET_ERROR;
//		}
		return net_parse_ret.RET_OK;
	}
	//����˵��û��ƥ�������ERROR
	return net_parse_ret.RET_ERROR;
	
}

/*
	ȷ����Ϣ
*/
int net_rollback_msg(char *mqtt_action, char *session_id)
{
	//����ֵ����
	struct net_rollback_msg_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_rollback_msg_ret={0,1};
	
	//�ֲ���������
	char temp_string[64]={0};
	int ret=0;
	
	//ƴ�ӱ���
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
	
	//������Ϣ
	ret = net_mqtt_publish(global_mqtt_config.client_idx, 0, 0, 0, global_net_config.mqtt_pub_rollback_topic, strlen(temp_buffer) , temp_buffer, global_device_config.beep_mode);
	if(0!=ret)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_rollback_msg_ret.RET_ERROR;
	}
	
	//��������
	return net_rollback_msg_ret.RET_OK;
}

/*
	���������豸����
*/
int net_recv_server_startup_buzzer(char *buffer_in)
{
	struct net_recv_server_startup_buzzer_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_recv_server_startup_buzzer_ret = {0,1};
	
	//�ֲ�����
	char *p=NULL;
	char action[64]={0};
	char session_id[64]={0};
	int i=0;
	int j=0;
	
	//�������
	if(NULL == buffer_in)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_startup_buzzer_ret.RET_ERROR;
	}
	
	//������action
	p = strstr(buffer_in, "\"action\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"action\":\"%[^\"]", action);
	}

	//������session_id
	p = strstr(buffer_in, "\"session_id\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"session_id\":\"%[^\"]", session_id);
	}
	
	//����ȷ����Ϣ
	net_rollback_msg(action, session_id);
	
	//�÷�������
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
	
	//��������
	return net_recv_server_startup_buzzer_ret.RET_OK;
}

/*
	�����������豸�Ƿ�����
*/
int net_recv_server_device_link(char *buffer_in)
{
	struct net_recv_server_device_link_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_recv_server_device_link_ret = {0,1};
	
	//�ֲ�����
	char *p=NULL;
	char action[64]={0};
	char session_id[64]={0};
	
	//�������
	if(NULL == buffer_in)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_device_link_ret.RET_ERROR;
	}
	
	//������action
	p = strstr(buffer_in, "\"action\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"action\":\"%[^\"]", action);
	}

	//������session_id
	p = strstr(buffer_in, "\"session_id\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"session_id\":\"%[^\"]", session_id);
	}
	
	//����ȷ����Ϣ
	net_rollback_msg(action, session_id);
	
	//�������ߵ���ʱ
	global_timer.sleep_timer_sec = 0;
	
	//��������
	return net_recv_server_device_link_ret.RET_OK;
} 

/*
	���������豸����
*/
int net_recv_server_reboot(char *buffer_in)
{
	struct net_recv_server_reboot_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_recv_server_reboot_ret = {0,1};
	
	//�ֲ�����
	char *p=NULL;
	char action[64]={0};
	char session_id[64]={0};
	
	//�������
	if(NULL == buffer_in)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_reboot_ret.RET_ERROR;
	}
	
	//������action
	p = strstr(buffer_in, "\"action\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"action\":\"%[^\"]", action);
	}

	//������session_id
	p = strstr(buffer_in, "\"session_id\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"session_id\":\"%[^\"]", session_id);
	}
	
	//����ȷ����Ϣ
	net_rollback_msg(action, session_id);
	
	//����flash
	flash_save_global_variables();
	osDelay(100);
	
	//�����豸
	printf("[net]�豸����...\n");
	osDelay(50);
	system_restart(1);
	
	//��������
	return net_recv_server_reboot_ret.RET_OK;
}


/*
	���������豸�ϴ�����
*/
int net_recv_server_get_data(char *buffer_in)
{
	struct net_recv_server_get_data_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_recv_server_get_data_ret = {0,1};
	
	//�ֲ�����
	char *p=NULL;
	char action[64]={0};
	char session_id[64]={0};
	
	//�������
	if(NULL == buffer_in)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_get_data_ret.RET_ERROR;
	}
	
	//������action
	p = strstr(buffer_in, "\"action\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"action\":\"%[^\"]", action);
	}

	//������session_id
	p = strstr(buffer_in, "\"session_id\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"session_id\":\"%[^\"]", session_id);
	}
	
	//����ȷ����Ϣ
	net_rollback_msg(action, session_id);
	
	//�ϴ�����
	global_flag.upload_data_flag = 1;
	
	//��������
	return net_recv_server_get_data_ret.RET_OK;
}

/*
	���������豸��ʼ����
*/
int net_recv_server_task_start(char *buffer_in)
{
	struct net_recv_server_task_start_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_recv_server_task_start_ret = {0,1};
	
	//�ֲ�����
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
	
	//�������
	if(NULL == buffer_in)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_task_start_ret.RET_ERROR;
	}
	
	//������action
	p = strstr(buffer_in, "\"action\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"action\":\"%[^\"]", action);
	}

	//������session_id
	p = strstr(buffer_in, "\"session_id\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"session_id\":\"%[^\"]", session_id);
	}
	
	//�����������
	p = strstr(buffer_in, "\"task_num\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"task_num\":\"%[^\"]", global_task_info.task_num);
	}
	
	//����������ʼʱ��
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
//	//��������ͷFTP
//	strcpy(global_camera_info.camera_ftp_folder, global_task_info.task_num);
//	
//	//��������FTP���������ͷ
//	global_flag.config_video_ftp=1;
//	
	//�����־
	
	//��նϵ�����ջ
	stack_clear();
	memset(&global_upload_pool[0], 0, sizeof(struct upload_data_struct)*STACK_SIZE);
	
	//������ֵ
//	global_mpu_info.lean_angle_max_x=0;
//	global_mpu_info.lean_angle_max_y=0;
//	global_mpu_info.lean_angle_max_z=0;
//	global_mpu_info.vibration_max_x=0;
//	global_mpu_info.vibration_max_y=0;
//	global_mpu_info.vibration_max_z=0;
	
	//���񴴽�֮ǰӦ�ý����߼��������㣬����������ȡ�ɹ���ֱ�ӽ�������ģʽ rbw
	global_timer.sleep_timer_sec = 0;
	//����״̬����Ϊ����
	global_timer.sleep_timer_sec = 0;
	global_task_info.task_status=1;
	
	//����flash
	flash_save_global_variables();
	
	//����ȷ����Ϣ
	net_rollback_msg(action, session_id);
	
	//��ȡ��Ƶ
	printf("[net]��ȡ��Ƶ������ûд��...\n");
	
	//��������
	return net_recv_server_task_start_ret.RET_OK;
}

/*
	���������豸��������
*/
int net_recv_server_task_over(char *buffer_in)
{
	struct net_recv_server_task_over_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_recv_server_task_over_ret = {0,1};
	
	//�ֲ�����
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
	
	//�������
	if(NULL == buffer_in)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_task_over_ret.RET_ERROR;
	}

	//������action
	p = strstr(buffer_in, "\"action\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"action\":\"%[^\"]", action);
	}

	//������session_id
	p = strstr(buffer_in, "\"session_id\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"session_id\":\"%[^\"]", session_id);
	}
	
	//�����������
	p = strstr(buffer_in, "\"task_num\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"task_num\":\"%[^\"]", global_task_info.task_num);
	}
	
	//�������������ʱ��
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
	
	//����ȷ����Ϣ
	net_rollback_msg(action, session_id);
	
	//����״̬����Ϊ�ر�
	global_task_info.task_status=0;
	
	//����flash
	flash_save_global_variables();
	
	//�ϴ���־
	//printf("\n[m203c]�����������ʼ�ϴ���־");

	
	//��������
	return net_recv_server_task_over_ret.RET_OK;
}

/*
	���������豸�����ϴ�����
*/
int net_recv_server_open_upload(char *buffer_in)
{
	struct net_recv_server_open_upload_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_recv_server_open_upload_ret = {0,1};
	
	//�ֲ�����
	char *p=NULL;
	char action[64]={0};
	char session_id[64]={0};
	
	//�������
	if(NULL == buffer_in)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_open_upload_ret.RET_ERROR;
	}
	
	//������action
	p = strstr(buffer_in, "\"action\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"action\":\"%[^\"]", action);
	}

	//������session_id
	p = strstr(buffer_in, "\"session_id\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"session_id\":\"%[^\"]", session_id);
	}
	
	//����ȷ����Ϣ
	net_rollback_msg(action, session_id);
	
	//��λ�ϴ����ر�־
	global_flag.upload_enable_flag=1;
	
	//��������
	return net_recv_server_open_upload_ret.RET_OK;
}


/*
	���������豸��ֹ�ϴ�����
*/
int net_recv_server_off_upload(char *buffer_in)
{
	struct net_recv_server_off_upload_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_recv_server_off_upload_ret = {0,1};
	
	//�ֲ�����
	char *p=NULL;
	char action[64]={0};
	char session_id[64]={0};

	//�������
	if(NULL == buffer_in)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_off_upload_ret.RET_ERROR;
	}

	//������action
	p = strstr(buffer_in, "\"action\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"action\":\"%[^\"]", action);
	}

	//������session_id
	p = strstr(buffer_in, "\"session_id\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"session_id\":\"%[^\"]", session_id);
	}
	
	//����ȷ����Ϣ
	net_rollback_msg(action, session_id);
	
	//��λ�ϴ����ر�־
	global_flag.upload_enable_flag=0;
	
	//��������
	return net_recv_server_off_upload_ret.RET_OK;
}

/*
	�����������ն�������Ϣ
*/
int net_recv_server_push_config(char *buffer_in)
{
	struct net_recv_server_push_config_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_recv_server_push_config_ret = {0,1};
	
	//�ֲ�����
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
	//�������
	if(NULL == buffer_in)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_push_config_ret.RET_ERROR;
	}

	//������action
	p = strstr(buffer_in, "\"action\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"action\":\"%[^\"]", action);
	}

	//������session_id
	p = strstr(buffer_in, "\"session_id\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"session_id\":\"%[^\"]", session_id);
	}
	
	//������������ʱ��
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
	
	//����������
	p = strstr(buffer_in, "\"week_day\":\"");
	if(NULL!=p)
	{
		ret=sscanf(p, "\"week_day\":\"%d\"", &temp_weekday);
		if(1==ret)
		{
			global_rtc_set_time.time_for_set.weekday = temp_weekday;
		}
	}
	
	//����FTP IP
	p = strstr(buffer_in, "\"ftp_ip\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"ftp_ip\":\"%[^\"]", global_ftp_config.hostname);
	}
	
	//���� FTP user
	p = strstr(buffer_in, "\"ftp_user\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"ftp_user\":\"%[^\"]", global_ftp_config.username);
	}
	
	//���� ftp_pwd
	p = strstr(buffer_in, "\"ftp_pwd\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"ftp_pwd\":\"%[^\"]", global_ftp_config.password);
	}
	
	//���� port_num
	p = strstr(buffer_in, "\"port_num\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"port_num\":\"%d\"", &global_ftp_config.port);
	}
	
	//���� sleepTime
	p = strstr(buffer_in, "\"sleepTime\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"sleepTime\":\"%d\"", &global_device_config.sleep_time);
	}
	
	//���� awakeTime
	p = strstr(buffer_in, "\"awakeTime\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"awakeTime\":\"%d\"", &global_device_config.awake_time);
	}
//20220311	
	//���� interval
//	p = strstr(buffer_in, "\"interval\":\"");
//	if(NULL!=p)
//	{
//		sscanf(p, "\"interval\":\"%d\"", &global_device_config.interval);
//	}
	
	//���� max_data_path
	p = strstr(buffer_in, "\"max_data_path\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"max_data_path\":\"%[^\"]", global_ftp_config.max_data_path);
	}
	
	//���� run_log_path
	p = strstr(buffer_in, "\"run_log_path\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"run_log_path\":\"%[^\"]", global_ftp_config.run_log_path);
	}
	
	//����SN��
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
	
	//����ȷ����Ϣ
	net_rollback_msg(action, session_id);
	
	//��������
	return net_recv_server_push_config_ret.RET_OK;
}

/*
	�����������ն���ֵ��Ϣ
*/
int net_recv_server_push_threshold(char *buffer_in)
{
	struct net_recv_server_push_threshold_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_recv_server_push_threshold_ret = {0,1};
	
	//�ֲ�����
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
	
	//�������
	if(NULL == buffer_in)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_push_threshold_ret.RET_ERROR;
	}

	//������action
	p = strstr(buffer_in, "\"action\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"action\":\"%[^\"]", action);
	}

	//������session_id
	p = strstr(buffer_in, "\"session_id\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"session_id\":\"%[^\"]", session_id);
	}
	
	//������������ʱ��
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
	
	//����������
	p = strstr(buffer_in, "\"week_day\":\"");
	if(NULL!=p)
	{
		ret=sscanf(p, "\"week_day\":\"%d\"", &temp_weekday);
		if(1==ret)
		{
			global_rtc_set_time.time_for_set.weekday = temp_weekday;
		}
	}
	
	//���� dump_energy
	p = strstr(buffer_in, "\"dump_energy\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"dump_energy\":\"%d\"", &global_threshold.dump_energy);
	}
	
//	//���� temperature
//	p = strstr(buffer_in, "\"temperature\":\"");
//	if(NULL!=p)
//	{
//		sscanf(p, "\"temperature\":\"%lf,%lf\"", \
//			&global_threshold.temperature_low, \
//			&global_threshold.temperature_high);
//	}
	
	//���� lean_angle
//	p = strstr(buffer_in, "\"lean_angle\":\"");
//	if(NULL!=p)
//	{
//		sscanf(p, "\"lean_angle\":\"%lf,%lf\"", \
//			&global_threshold.lean_angle_x, \
//			&global_threshold.lean_angle_y);
//	}
	
	//���� pressure
//	p = strstr(buffer_in, "\"pressure\":\"");
//	if(NULL!=p)
//	{
//		sscanf(p, "\"pressure\":\"%lf,%lf\"", \
//			&global_threshold.pressure_low, \
//			&global_threshold.pressure_high);
//	}
	
	//���� vibration
//	p = strstr(buffer_in, "\"vibration\":\"");
//	if(NULL!=p)
//	{
//		sscanf(p, "\"vibration\":\"%lf,%lf,%lf\"", \
//			&global_threshold.vibration_x, \
//			&global_threshold.vibration_y, \
//			&global_threshold.vibration_z);
//	}
	
	//���� humidity
//	p = strstr(buffer_in, "\"humidity\":\"");
//	if(NULL!=p)
//	{
//		sscanf(p, "\"humidity\":\"%lf\"", \
//			&global_threshold.humidity);
//	}
	
	//���� speed
	p = strstr(buffer_in, "\"speed\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"speed\":\"%lf\"", \
			&global_threshold.speed);
	}
	
	//����ȷ����Ϣ
	net_rollback_msg(action, session_id);
	
	
	//��������
	return net_recv_server_push_threshold_ret.RET_OK;
}
/*
	���������豸����������Ϣ
*/
int net_recv_server_push_taskinfo(char *buffer_in)
{
	struct net_recv_server_push_taskinfo_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_recv_server_push_taskinfo_ret = {0,1};
	
	//�ֲ�����
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
	//�������
	if(NULL == buffer_in)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_push_taskinfo_ret.RET_ERROR;
	}

	//������action
	p = strstr(buffer_in, "\"action\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"action\":\"%[^\"]", action);
	}

	//������session_id
	p = strstr(buffer_in, "\"session_id\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"session_id\":\"%[^\"]", session_id);
	}
	
	//������ terminal_sn
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
	
	//������ task_num
	p = strstr(buffer_in, "\"task_num\":\"");
	if(NULL!=p)
	{
		ret = sscanf(p, "\"task_num\":\"%[^\"]", temp_task_num);
		if(1==ret)
		{
			have_task_num=1;
		}
	}
	
	//����������״̬
	p = strstr(buffer_in, "\"task_status\":\"");
	if(NULL!=p)
	{
		ret = sscanf(p, "\"task_status\":\"%d\"", &temp_task_status);
		if(1==ret)
		{
			have_task_status=1;
		}	
	}
	
	//����������ʼʱ��
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
	
	//�������������ʱ��
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
		//�ȽϽ��յ��ĺͱ����task_num�Ƿ�һ��
		ret = strcmp(temp_task_num, global_task_info.task_num);
		if(0!=ret)
		{
			//��һ�������ԭ�������־�Ͷϵ�����ջ��������flash
			stack_clear();
			memset(&global_upload_pool[0], 0, sizeof(struct upload_data_struct)*STACK_SIZE);
			
			//�����µ������
			strcpy(global_task_info.task_num, temp_task_num);
			
			//�����µ�����״̬
			global_task_info.task_status = temp_task_status;
			
			//��������ʼʱ��
			global_task_info.task_start_time.year = temp_start_time.year;
			global_task_info.task_start_time.month = temp_start_time.month;
			global_task_info.task_start_time.day = temp_start_time.day;
			global_task_info.task_start_time.hour = temp_start_time.hour;
			global_task_info.task_start_time.minute = temp_start_time.minute;
			global_task_info.task_start_time.second = temp_start_time.second;
			
			//�����������ʱ��
			global_task_info.task_end_time.year = temp_end_time.year;
			global_task_info.task_end_time.month = temp_end_time.month;
			global_task_info.task_end_time.day = temp_end_time.day;
			global_task_info.task_end_time.hour = temp_end_time.hour;
			global_task_info.task_end_time.minute = temp_end_time.minute;
			global_task_info.task_end_time.second = temp_end_time.second;
			
			//��������״̬��������ͷFTP
//			if(1==global_task_info.task_status)
//			{
//				strcpy(global_camera_info.camera_ftp_folder, global_task_info.task_num);
//				global_flag.config_video_ftp=1;
//			}
		}
		else
		{
			//������յ��ĺͱ���������һ��
			//�ж��Ƿ��ڹػ��������������
			if(1 == global_task_info.task_status && 0 == temp_task_status)
			{
				//�����ڹػ�ʱ�Ѿ���������նϵ�����ջ������λ�ϴ���־��־λ
				stack_clear();
				memset(&global_upload_pool[0], 0, sizeof(struct upload_data_struct)*STACK_SIZE);
				
				//�����µ������
				strcpy(global_task_info.task_num, temp_task_num);
				
				//�����µ�����״̬
				global_task_info.task_status = temp_task_status;
				
				//��������ʼʱ��
				global_task_info.task_start_time.year = temp_start_time.year;
				global_task_info.task_start_time.month = temp_start_time.month;
				global_task_info.task_start_time.day = temp_start_time.day;
				global_task_info.task_start_time.hour = temp_start_time.hour;
				global_task_info.task_start_time.minute = temp_start_time.minute;
				global_task_info.task_start_time.second = temp_start_time.second;
				
				//�����������ʱ��
				global_task_info.task_end_time.year = temp_end_time.year;
				global_task_info.task_end_time.month = temp_end_time.month;
				global_task_info.task_end_time.day = temp_end_time.day;
				global_task_info.task_end_time.hour = temp_end_time.hour;
				global_task_info.task_end_time.minute = temp_end_time.minute;
				global_task_info.task_end_time.second = temp_end_time.second;
			}
		}
	}
	
	//����ȷ����Ϣ
	net_rollback_msg(action, session_id);
	
	//��������
	return net_recv_server_push_taskinfo_ret.RET_OK;
}

/*
	�����ϴ�����
*/
int net_upload_data(void)
{
	struct net_upload_data_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_upload_data_ret = {0,1};
	
	//�ֲ�����
	char temp_string[64]={0};
	int ret=0;
	
	//׼������
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
		//GPS��λ��Ч
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
	
	//������Ϣ
	ret = net_mqtt_publish(global_mqtt_config.client_idx, 0, 0, 0, global_net_config.mqtt_pub_collection_topic, strlen(temp_buffer) , temp_buffer, global_device_config.beep_mode);
	if(0!=ret)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_upload_data_ret.RET_ERROR;
	}
	//��������
	return net_upload_data_ret.RET_OK;
}
/*
	�ն˷����������������60��������Ϣ��������
*/
struct time_struct new_task_time;
int net_registration_task_device_gw(char *device_state)
{
	struct net_registration_device_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_registration_device_ret = {0,1};
	
	//�ֲ�����
	char temp_string[64]={0};
	int ret=0;
	char temp_lac[32]={0};
	char temp_ci[32]={0};
	
	//�������
	if(NULL==device_state)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_registration_device_ret.RET_ERROR;
	}
	
	//׼������
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
		//��λ��Ч
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
	
	//�汾��
	sprintf(temp_string,"%s %s,",__DATE__,__TIME__);
	strcat(temp_buffer, temp_string);
	//�豸״̬
	sprintf(temp_string,"%s,",device_state);
	strcat(temp_buffer, temp_string);
	//���
	sprintf(temp_string,"%.2f,%.2f,",(double)0, (double)0);
	strcat(temp_buffer, temp_string);
	//����
	sprintf(temp_string,"%d,",global_device_info.dump_energy);
	strcat(temp_buffer, temp_string);
	//����ʱ��
	sprintf(temp_string,"%04d-%02d-%02d %02d:%02d:%02d,",\
					global_device_info.start_up_time.year+2000, global_device_info.start_up_time.month, global_device_info.start_up_time.day, \
					global_device_info.start_up_time.hour,global_device_info.start_up_time.minute,global_device_info.start_up_time.second);
	strcat(temp_buffer, temp_string);
	//��ȡ��վ
	net_get_base_station(temp_lac, temp_ci);
	//lac
	sprintf(temp_string,"%s,",temp_lac);
	strcat(temp_buffer, temp_string);
	//ci
	sprintf(temp_string,"%s",temp_ci);
	strcat(temp_buffer, temp_string);
	
	//������Ϣ
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
	//��������
	return net_registration_device_ret.RET_OK;
}
/*
	�ն˷���ע���豸��Ϣ
*/
int net_registration_device(char *device_state)
{
	struct net_registration_device_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_registration_device_ret = {0,1};
	
	//�ֲ�����
	char temp_string[64]={0};
	int ret=0;
	char temp_lac[32]={0};
	char temp_ci[32]={0};
	
	//�������
	if(NULL==device_state)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_registration_device_ret.RET_ERROR;
	}
	
	//׼������
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
		//��λ��Ч
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
	
	//�汾��
	sprintf(temp_string,"%s %s,",__DATE__,__TIME__);
	strcat(temp_buffer, temp_string);
	//�豸״̬
	sprintf(temp_string,"%s,",device_state);
	strcat(temp_buffer, temp_string);
	//���
	sprintf(temp_string,"%.2f,%.2f,",(double)0, (double)0);
	strcat(temp_buffer, temp_string);
	//����
	sprintf(temp_string,"%d,",global_device_info.dump_energy);
	strcat(temp_buffer, temp_string);
	//����ʱ��
	sprintf(temp_string,"%04d-%02d-%02d %02d:%02d:%02d,",\
					global_device_info.start_up_time.year+2000, global_device_info.start_up_time.month, global_device_info.start_up_time.day, \
					global_device_info.start_up_time.hour,global_device_info.start_up_time.minute,global_device_info.start_up_time.second);
	strcat(temp_buffer, temp_string);
	//��ȡ��վ
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
	//������Ϣ
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
	//��������
	return net_registration_device_ret.RET_OK;
}

/*
	�ն˷����ϴ���־��Ϣ
*/
int net_upload_log_msg(char *file_name, char *log_type)
{
	struct net_upload_log_msg_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_upload_log_msg_ret = {0,1};
	
	//�ֲ�����
	char temp_string[64]={0};
	int ret=0;
	
	//�������
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
	
	//����mqtt�ϴ��ɹ�֪ͨ����
	//ƴ�ӱ���
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
	
	//������Ϣ
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
	
	//��������
	return net_upload_log_msg_ret.RET_OK;
}

/*
	�ж��Ƿ񱨾�
*/
uint32_t Inclination_alarm_sign = 0;
//extern uint8_t angle_max_sign;
int check_if_alarm(void)
{
	
	return 0;
}

/*
	����������GPS����
*/
int net_recv_server_gps_switch(char *buffer_in)
{
	struct net_recv_server_gps_switch_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_recv_server_gps_switch_ret = {0,1};
	
	//�ֲ�����
	char *p=NULL;
	char action[64]={0};
	char session_id[64]={0};
	
	//�������
	if(NULL == buffer_in)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_gps_switch_ret.RET_ERROR;
	}

	//������action
	p = strstr(buffer_in, "\"action\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"action\":\"%[^\"]", action);
	}

	//������session_id
	p = strstr(buffer_in, "\"session_id\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"session_id\":\"%[^\"]", session_id);
	}
	
	//���� gps_switch
	p = strstr(buffer_in, "\"gps_switch\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"gps_switch\":\"%d\"", &global_device_config.gnss_mode);
	}
	
	//����ȷ����Ϣ
	net_rollback_msg(action, session_id);
	
	//����flash
	flash_save_global_variables();
	
	//��������
	return net_recv_server_gps_switch_ret.RET_OK;
}

/*
	������������
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
	
	//��̬��������
	static int mqtt_msg_id=1;
	static int init_flag=0;
	
	//�ֲ���������
	int state=0;
	int ret=0;
	struct topic_qos_struct topic_qos={0};
	int start_time=0;

	//��ʱ��ʼ
	start_time = global_timer.system_timer;
	
	//EC20�˳�����
	net_power_sleep(0);	
	//ѭ������
	while(1)
	{
		//��λE20
		if(3==init_flag)
		{
			init_flag = 0;
			net_ec20_restart(10000);
		}
		init_flag++;
		
		//ѭ�����SIM��ICCID
		ret = net_get_iccid(global_device_info.iccid);
		if(0!=ret)
		{
			ret = net_log_write("[net]û�м�鵽SIM����ICCID\n");
			printf("[net]û�м�鵽SIM����ICCID\n");
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
				net_log_write("[net]���ӳ�ʱ,û��鵽SIM��\n");
				printf("[net]���ӳ�ʱ,û��鵽SIM��...\n");
				return net_wait_reconnect_ret.RET_TIMEOUT;
			}

			continue;
		}
		else
		{
			printf("[net]SIM��ICCIDΪ:%s\n", global_device_info.iccid);
		}

		
		//�ȴ�������������
		printf("[net]����������������...\n");
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
					net_log_write("[net]��ǰ�ź�ǿ��Ϊ%d��\n", global_device_info.signal_quality);
					printf("[debug]��ǰ�ź�ǿ��Ϊ%d��\n", global_device_info.signal_quality);
				}
				net_log_write("[net]�������ӳ�ʱ\n");
				printf("[net]���ӳ�ʱ...\n");
				return net_wait_reconnect_ret.RET_TIMEOUT;
			}
		}
		
		//�����������ӣ�ѭ������ֱ���ɹ�
		printf("[net]���ڽ�����������...\n");

		ret = net_signal_quality(&global_device_info.signal_quality);
		if(0==ret)
		{
			net_log_write("[net]��ǰ�ź�ǿ��Ϊ%d��\n", global_device_info.signal_quality);
			printf("[debug]��ǰ�ź�ǿ��Ϊ%d��\n", global_device_info.signal_quality);
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
			printf("[net]�������ӳɹ�\n");
			break;
		}
		else
		{
			//led_gprs_mode(LED_RED,1);
			net_log_write("[net]��������ʧ��\n");
			printf("[net]��������ʧ��\n");
		}
	}
	return net_wait_reconnect_ret.RET_OK;
}

/*
	��ȡ������ʱ��
*/
int net_device_get_time(void)
{
	struct net_device_get_time_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_device_get_time_ret = {0,1};
	
	//�ֲ�����
	char temp_string[64]={0};
	int ret=0;
	
	//׼������
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
	
	//������Ϣ
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
	//��������
	return net_device_get_time_ret.RET_OK;
}

/*
	����������ʱ����Ϣ
*/
int net_recv_server_push_time(char *buffer_in)
{
	struct net_recv_server_push_time_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_recv_server_push_time_ret = {0,1};
	
	//�ֲ�����
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
	
	//�������
	if(NULL == buffer_in)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_push_time_ret.RET_ERROR;
	}

	//������action
	p = strstr(buffer_in, "\"action\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"action\":\"%[^\"]", action);
	}

	//������session_id
	p = strstr(buffer_in, "\"session_id\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"session_id\":\"%[^\"]", session_id);
	}
	
	//������������ʱ��
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
	
	//����������
	p = strstr(buffer_in, "\"week_day\":\"");
	if(NULL!=p)
	{
		ret=sscanf(p, "\"week_day\":\"%d\"", &temp_weekday);
		if(1==ret)
		{
			global_rtc_set_time.time_for_set.weekday = temp_weekday;
		}
	}
	
	//����γ��
	p = strstr(buffer_in, "\"latitude\":\"");
	if(NULL!=p)
	{
		//������
	}
	
	//��������
	p = strstr(buffer_in, "\"longitude\":\"");
	if(NULL!=p)
	{
		//������
	}
	
	//����ȷ����Ϣ
	net_rollback_msg(action, session_id);
	
	
	//��������
	return net_recv_server_push_time_ret.RET_OK;
}

/*
	���Ͱ��´�ӡ��ť��ӡ����60����־��Ϣ
*/
int net_client_local_printed(void)
{
	//����ֵ����
	struct net_client_local_printed_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_client_local_printed_ret={0,1};
	
	//�ֲ���������
	char temp_string[64]={0};
	int ret=0;
	
	//ƴ�ӱ���
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
		//GPS��λ��Ч
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
	
	//������Ϣ
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
	//��������
	return net_client_local_printed_ret.RET_OK;
}

/*
	���������豸�ϴ�����60�����ֵ��־
*/
int net_recv_server_get_local_max_datalog(char *buffer_in)
{
	struct net_recv_server_get_local_max_datalog_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_recv_server_get_local_max_datalog_ret = {0,1};
	
	//�ֲ�����
	char *p=NULL;
	char action[64]={0};
	char session_id[64]={0};
	int ret=0;
	char file_name[256]={0};
	
	//�������
	if(NULL == buffer_in)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_get_local_max_datalog_ret.RET_ERROR;
	}

	//������action
	p = strstr(buffer_in, "\"action\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"action\":\"%[^\"]", action);
	}

	//������session_id
	p = strstr(buffer_in, "\"session_id\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"session_id\":\"%[^\"]", session_id);
	}
	
	//����ȷ����Ϣ
	net_rollback_msg(action, session_id);
	
	//�ϴ�����60�����ֵ��־
	ret = net_upload_local_maxdata_log(file_name);
	if(0!=ret)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_get_local_max_datalog_ret.RET_ERROR;
	}
	
	//�����ϴ��������ֵ��־�ɹ�֪ͨ
	ret = net_upload_log_msg(file_name, "local_maxdata_log");
	if(0!=ret)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_get_local_max_datalog_ret.RET_ERROR;
	}
	
	//��������
	return net_recv_server_get_local_max_datalog_ret.RET_OK;
}

/*
	ftp�ϴ�����ģʽ60�����ֵ
*/
int net_upload_local_maxdata_log(char *file_name_out)
{
	struct net_upload_local_maxdata_log_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_upload_local_maxdata_log_ret = {0,1};
	
	//�ֲ�����
	int ret=0;
	char file_name[256]={0};
	int i=0;
	int start_pos=0;
	int j=0;
	int beof=0;
	
	//������
	if(NULL == file_name_out)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_upload_local_maxdata_log_ret.RET_ERROR;
	}
	
	//�ر�FTP����
	ret=net_ftp_close();
	
	
	//����ftp
	ret=net_ftp_configrate(&global_ftp_config);

	
	//��FTP����
	ret=net_ftp_open("119.3.241.184", 18356);

	
	//����·��(������������Ϣ������)
	//sprintf(global_ftp_config.max_data_path, "\\maxdata_log\\%s",global_device_info.uid);
	ret=net_ftp_cwd(global_ftp_config.max_data_path);
	
	//�ϴ�����
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
	
	//���
	strcpy(file_name_out,file_name);
	
	//��������
	return net_upload_local_maxdata_log_ret.RET_OK;
	
}

/*
	�ϴ��豸��Ϣ
*/
int net_upload_device_info(void)
{
	struct net_upload_device_info_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_upload_device_info_ret = {0,1};
	
	//�ֲ�����
	char temp_string[64]={0};
	int ret=0;
	
	//׼������
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
	//�����
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
	//����ֵ
	sprintf(temp_string,"%.2f,%.2f,%.2f,",\
			(double)0,\
			(double)0,\
			(double)0);
	strcat(temp_buffer, temp_string);
	//�����ֵ
	sprintf(temp_string,"%.2f,%.2f,",\
			(double)0,\
			(double)0);
	strcat(temp_buffer, temp_string);
	//ѹ����ֵ
	sprintf(temp_string,"%.2f,%.2f,",(double)0, (double)0);
	strcat(temp_buffer, temp_string);
	//�¶���ֵ
	sprintf(temp_string,"%.2f,%.2f,",(double)0,(double)0);
	strcat(temp_buffer, temp_string);
	//����ʱ��
	sprintf(temp_string,"%d,",global_device_config.awake_time);
	strcat(temp_buffer, temp_string);
	//����ʱ��
	sprintf(temp_string,"%d,",global_device_config.sleep_time);
	strcat(temp_buffer, temp_string);
	//�ϴ����
	sprintf(temp_string,"%d,",global_device_config.interval);
	strcat(temp_buffer, temp_string);
	//GPS״̬
	sprintf(temp_string,"%d,",global_device_config.gnss_mode);
	strcat(temp_buffer, temp_string);
	//����
	sprintf(temp_string,"%d,",global_device_info.dump_energy);
	strcat(temp_buffer, temp_string);
	//ICCID
	sprintf(temp_string,"%s,",global_device_info.iccid);
	strcat(temp_buffer, temp_string);
	//������ģʽ
	sprintf(temp_string,"%d,",global_device_config.beep_mode);
	strcat(temp_buffer, temp_string);
	//��ϵ��
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
	
	//rf433ģʽ
	sprintf(temp_string,"%d,",global_device_config.rf433_mode);
	strcat(temp_buffer, temp_string);
	//�������翪��ģʽ
	sprintf(temp_string,"%d,",global_device_config.sleep_network);
	strcat(temp_buffer, temp_string);
	//�豸SN��
	sprintf(temp_string,"%s,",global_device_info.terminal_sn);
	strcat(temp_buffer, temp_string);
	//�����ϴ�����
	sprintf(temp_string,"%d,",global_flag.upload_enable_flag);
	strcat(temp_buffer, temp_string);
	//���ݰ�װ��̬
	sprintf(temp_string,"%d,",0);
	strcat(temp_buffer, temp_string);
	//�豸��ѹ
	sprintf(temp_string,"%.2f,",global_device_info.battery_voltage);
	strcat(temp_buffer, temp_string);
	//�汾��
	sprintf(temp_string,"%s",global_device_info.Version_number);
	strcat(temp_buffer, temp_string);
	//������Ϣ
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
	//��������
	return net_upload_device_info_ret.RET_OK;
}

/*
	���������豸�����豸��Ϣ
*/
int net_recv_server_get_device_info(char *buffer_in)
{
	struct net_recv_server_get_device_info_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_recv_server_get_device_info_ret = {0,1};
	
	//�ֲ�����
	char *p=NULL;
	char action[64]={0};
	char session_id[64]={0};
	
	//�������
	if(NULL == buffer_in)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_get_device_info_ret.RET_ERROR;
	}
	
	//������action
	p = strstr(buffer_in, "\"action\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"action\":\"%[^\"]", action);
	}

	//������session_id
	p = strstr(buffer_in, "\"session_id\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"session_id\":\"%[^\"]", session_id);
	}
	
	//����ȷ����Ϣ
	net_rollback_msg(action, session_id);
	
	//�����豸��Ϣ
	net_upload_device_info();
	
	//��������
	return net_recv_server_get_device_info_ret.RET_OK;
}

/*
	���������豸����gps����һ��ʱ��
*/
int net_recv_server_gps_switch_second(char *buffer_in)
{
	struct net_recv_server_gps_switch_second_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_recv_server_gps_switch_second_ret = {0,1};
	
	//�ֲ�����
	char *p=NULL;
	char action[64]={0};
	char session_id[64]={0};
	int second = 0;
	
	//�������
	if(NULL == buffer_in)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_gps_switch_second_ret.RET_ERROR;
	}
	
	//������action
	p = strstr(buffer_in, "\"action\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"action\":\"%[^\"]", action);
	}

	//������session_id
	p = strstr(buffer_in, "\"session_id\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"session_id\":\"%[^\"]", session_id);
	}
	
	//������ second
	p = strstr(buffer_in, "\"second\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"second\":\"%d", &second);
	}
	
	//����ȷ����Ϣ
	net_rollback_msg(action, session_id);
	
	//�л�gpsģʽ
	global_device_config.gnss_once_time = second;
	global_timer.gps_timer_sec = 0;
	global_device_config.gnss_once_flag=1;
	

	//��������
	return net_recv_server_gps_switch_second_ret.RET_OK;
}

/*
	ftp�ϴ�������־
*/
int net_upload_run_log(char *file_name_out)
{
	struct net_upload_run_log_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_upload_run_log_ret = {0,1};
	
	//������
	if(NULL == file_name_out)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		net_ftp_close();
		return net_upload_run_log_ret.RET_ERROR;
	}
	
	//�ر�FTP����
	net_ftp_close();
	
	//����ftp
	net_ftp_configrate(&global_ftp_config);

	//��FTP����
	net_ftp_open("119.3.241.184", 18356);

	//����·��(������������Ϣ������)
	//sprintf(global_ftp_config.max_data_path, "\\maxdata_log\\%s",global_device_info.uid);
	net_ftp_cwd(global_ftp_config.run_log_path);
	
	//�ϴ��ļ�
	net_ftp_put_FILE("run_log.txt", "UFS:run_log.txt", 0);
	
	//���
	strcpy(file_name_out,"run_log.txt");
	
	//��������
	net_ftp_close();
	return net_upload_run_log_ret.RET_OK;
}

/*
	ftp�ϴ��񶯲���
	��������ΪNULL
*/
int net_upload_wave_file(char *file_name_out)
{
	struct net_upload_wave_file_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_upload_wave_file_ret = {0,1};
	
	//�ֲ�����
	char file_name[256]={0};

	//ƴ���ļ���
	sprintf(file_name,"wave_log_%04d%02d%02d%02d%02d%02d%02d.txt",\
					global_upload_data.time.year+2000, global_upload_data.time.month, global_upload_data.time.day, \
					global_upload_data.time.hour,global_upload_data.time.minute,global_upload_data.time.second,global_upload_data.ms);
	
	//�ر�FTP����
	net_ftp_close();
	
	//����ftp
	net_ftp_configrate(&global_ftp_config);

	//��FTP����
	net_ftp_open("119.3.241.184", 18356);

	//����·��(������������Ϣ������)
	//sprintf(global_ftp_config.max_data_path, "\\maxdata_log\\%s",global_device_info.uid);
	net_ftp_cwd(global_ftp_config.wave_path);
	
	//�ϴ��ļ�
	net_ftp_put_FILE(file_name, "UFS:wave_log.txt", 0);
	
	//���
	if(NULL != file_name_out)
	{
		strcpy(file_name_out,file_name);
	}
	
	//�����ϴ���ϱ���
	net_upload_wave_msg(file_name, "wave_form");
	
	//��������
	net_ftp_close();
	return net_upload_wave_file_ret.RET_OK;
	
	
	/*
	RET_ERROR:
	//�ر�FTP����
	net_ftp_close();
	return net_upload_wave_file_ret.RET_ERROR;
	*/
}

/*
	�ն˷����ϴ�����״̬��Ϣ
*/
int net_upload_wave_msg(char *file_name, char *log_type)
{
	struct net_upload_wave_msg_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_upload_wave_msg_ret = {0,1};
	
	//�ֲ�����
	char temp_string[64]={0};
	int ret=0;
	
	//�������
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
	
	//����mqtt�ϴ��ɹ�֪ͨ����
	//ƴ�ӱ���
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
	
	//������Ϣ
	ret = net_mqtt_publish(global_mqtt_config.client_idx, 0, 0, 0, global_net_config.mqtt_pub_wave_topic, strlen(temp_buffer) , temp_buffer, global_device_config.beep_mode);
	if(0!=ret)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_upload_wave_msg_ret.RET_ERROR;
	}
	
	//��������
	return net_upload_wave_msg_ret.RET_OK;
}

/*
	���������Ʒ���������
*/
int net_recv_server_beep_switch(char *buffer_in)
{
	struct net_recv_server_beep_switch_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_recv_server_beep_switch_ret = {0,1};
	
	//�ֲ�����
	char *p=NULL;
	char action[64]={0};
	char session_id[64]={0};
	
	//�������
	if(NULL == buffer_in)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_beep_switch_ret.RET_ERROR;
	}

	//������action
	p = strstr(buffer_in, "\"action\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"action\":\"%[^\"]", action);
	}

	//������session_id
	p = strstr(buffer_in, "\"session_id\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"session_id\":\"%[^\"]", session_id);
	}
	
	//���� switch_status
	p = strstr(buffer_in, "\"switch_status\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"switch_status\":\"%d\"", &global_device_config.beep_mode);
	}
	
	//����ȷ����Ϣ
	net_rollback_msg(action, session_id);
	
	//����flash
	flash_save_global_variables();
	
	//��������
	return net_recv_server_beep_switch_ret.RET_OK;
}

/*
	������������ϵ��
*/
int net_recv_server_set_coefficient(char *buffer_in)
{
	struct net_recv_server_set_coefficient_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_recv_server_set_coefficient_ret = {0,1};
	
	//�ֲ�����
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
	
	//�������
	if(NULL == buffer_in)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_set_coefficient_ret.RET_ERROR;
	}

	//������action
	p = strstr(buffer_in, "\"action\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"action\":\"%[^\"]", action);
	}

	//������session_id
	p = strstr(buffer_in, "\"session_id\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"session_id\":\"%[^\"]", session_id);
	}
	
	//���� value
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
	
	//����ȷ����Ϣ
	net_rollback_msg(action, session_id);
	
	//����flash
	flash_save_global_variables();
	
	//��������
	return net_recv_server_set_coefficient_ret.RET_OK;
}

/*
	��������������ʱ����ģ�鿪��
*/
int net_recv_server_sleep_network_switch(char *buffer_in)
{
	struct net_recv_server_sleep_network_switch_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_recv_server_sleep_network_switch_ret = {0,1};
	
	//�ֲ�����
	char *p=NULL;
	char action[64]={0};
	char session_id[64]={0};
	
	//�������
	if(NULL == buffer_in)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_sleep_network_switch_ret.RET_ERROR;
	}

	//������action
	p = strstr(buffer_in, "\"action\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"action\":\"%[^\"]", action);
	}

	//������session_id
	p = strstr(buffer_in, "\"session_id\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"session_id\":\"%[^\"]", session_id);
	}
	
	//���� switch_status
	p = strstr(buffer_in, "\"switch_status\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"switch_status\":\"%d\"", &global_device_config.sleep_network);
	}
	
	//����ȷ����Ϣ
	net_rollback_msg(action, session_id);
	
	//����flash
	flash_save_global_variables();
	
	//��������
	return net_recv_server_sleep_network_switch_ret.RET_OK;
}

/*
	��������ȡ�豸������־
*/
int net_recv_server_get_run_log_file(char *buffer_in)
{
	struct net_recv_server_get_run_log_file_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_recv_server_get_run_log_file_ret = {0,1};
	
	//�ֲ�����
	char *p=NULL;
	char action[64]={0};
	char session_id[64]={0};
	char run_log_name[128]={0};
	
	//�������
	if(NULL == buffer_in)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_get_run_log_file_ret.RET_ERROR;
	}

	//������action
	p = strstr(buffer_in, "\"action\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"action\":\"%[^\"]", action);
	}

	//������session_id
	p = strstr(buffer_in, "\"session_id\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"session_id\":\"%[^\"]", session_id);
	}
	
	//����ȷ����Ϣ
	net_rollback_msg(action, session_id);
	
	//�ϴ�������־
	net_upload_run_log(run_log_name);
	
	//��������
	return net_recv_server_get_run_log_file_ret.RET_OK;
}


/*
	���������豸�ػ�
*/
int net_recv_server_shut_down(char *buffer_in)
{
	struct net_recv_server_shut_down_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_recv_server_shut_down_ret = {0,1};
	
	//�ֲ�����
	char *p=NULL;
	char action[64]={0};
	char session_id[64]={0};
	
	//�������
	if(NULL == buffer_in)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_shut_down_ret.RET_ERROR;
	}

	//������action
	p = strstr(buffer_in, "\"action\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"action\":\"%[^\"]", action);
	}

	//������session_id
	p = strstr(buffer_in, "\"session_id\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"session_id\":\"%[^\"]", session_id);
	}
	
	//����ȷ����Ϣ
	net_rollback_msg(action, session_id);
	
	//ִ�йػ�׼��
	global_flag.net_shutdown_flag = 1;
	
	//��������
	return net_recv_server_shut_down_ret.RET_OK;
}

/*
	�������������豸Զ������
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
	
	//�ֲ�����
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
	//�������
	if(NULL == buffer_in)
	{
		printf("[net]error. file=%s, line=%d.\n", __FILE__, __LINE__);
		return net_recv_server_sys_upgrade_gw_ret.RET_ERROR;
	}
//	//������mid
//	p = strstr(buffer_in, "\"mid\":");
//	if(NULL!=p)
//	{
//		sscanf(p, "\"mid\":%[^\",]", global_remote_upgrade_info.upgrade_mid);
//	}
//	//������jobId 
//	p = strstr(buffer_in, "\"jobId\":");
//	if(NULL!=p)
//	{
//		sscanf(p, "\"jobId\":%[^\",]", global_remote_upgrade_info.upgrade_jobId);
//	}
//	//������md5
//	p = strstr(buffer_in, "\"md5\":\"");
//	if(NULL!=p)
//	{
//		sscanf(p, "\"md5\":\"%[^\"]", global_remote_upgrade_info.upgrade_md5);
//	}

	//������url
	p = strstr(buffer_in, "\"file_url\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"file_url\":\"%[^\"]", global_remote_upgrade_info.upgrade_url);
	}
	//������time
	p = strstr(buffer_in, "\"server_time\":\"");
	if(NULL!=p)
	{
		sscanf(p, "\"server_time\":\"%[^\"]", global_remote_upgrade_info.upgrade_timestamp);
	}
//	GW_push_time(buffer_in);
		//������version
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

	//������session_id
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
	printf("�汾����\n");
		  

//....................................................................................................

	//ִ������׼��
//	global_remote_upgrade_info.need_upgrade_flag=1;
	
	//��������
	return net_recv_server_sys_upgrade_gw_ret.RET_OK;
}

/*
	�����ϴ����ݱ���
*/
int net_upload_data_gw(uint8_t data_packet)
{
	struct net_upload_data_gw_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}net_upload_data_gw_ret = {0,1};
	
	//�ֲ�����
	char temp_string[64]={0};
	int ret=0;
	int iLoop = 0,iNum = 0;
//	int ciphetext_length=0;
//	uint32_t base64_length=0;
	
	//׼������
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
		
		//1 GPRS �ź�ǿ�� db
		sprintf(temp_string,"%d", global_upload_data.signal);
		strcat(temp_buffer, temp_string);
		
		//
		sprintf(temp_string,",");
		strcat(temp_buffer, temp_string);
		
		//2 ��ǰ��Դ����ֵ $
		sprintf(temp_string,"%d", global_upload_data.dump_energy);
		strcat(temp_buffer, temp_string);
		
		//
		sprintf(temp_string,",");
		strcat(temp_buffer, temp_string);
		
		//3 ��ǰ�����ٶ�ֵ ǧ��/Сʱ
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
		//4 ��ǰװ�þ���λ��ֵ ��
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
				
				//5 ��ǰװ��γ��λ��ֵ ��

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
		
		//15 GPS �ź���Ч��־ 1:��Ч 0:��Ч
		sprintf(temp_string,"%d",(1==global_upload_data.location_mode)?0:1);
		strcat(temp_buffer, temp_string);
		
		//
		sprintf(temp_string,",");
		strcat(temp_buffer, temp_string);
		
		//23 δ����
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
		
		//24 δ����
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
	
	
	
	//�Ƿ���Ҫ����
//	if(1==global_device_config.encryption_enable)
//	{
//		//�ȼ���
//		cipher_ecb_encryption(global_net_config.aes_key, (uint8_t *)temp_buffer, strlen(temp_buffer),  ciphetext_buffer, sizeof(ciphetext_buffer), &ciphetext_length);
//		//��ջ���
//		memset(temp_buffer, 0, sizeof(temp_buffer));
//		//base64
//		mbedtls_base64_encode( (unsigned char *)temp_buffer, sizeof(temp_buffer), &base64_length, ciphetext_buffer, ciphetext_length );
//		//������Ϣ
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
	//��������
	return net_upload_data_gw_ret.RET_OK;
}

/*
	�豸����
*/
void system_restart(uint8_t iSign)
{
	printf("�豸����...\n");
//*********************************************************************************
//������ 20220104  
//*********************************************************************************	
	if(iSign == 1)
	{
			global_flag.system_restart_flag=1;
			global_flag.button_start_up_flag=0;
			
			//����flash
			global_flag.flash_busy_flag=0;//æ��־ǿ����0��ȷ������ǰflash������
			flash_save_global_variables();
	}
//	M41T65_write_data(0x2A,9);
	feed_dog_func(0);
	osDelay(100);
	NVIC_SystemReset();
}

