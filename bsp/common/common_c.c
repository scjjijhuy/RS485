
#include "main.h"
/*
	gps输出结果结构体
*/
struct gps_out_struct global_gps_out=
{
	.time={0},
	.enable_state={0},
	.latitude=0,
	.NS={0},
	.longitude=0,
	.EW={0},
	.speed=0,
	.direction=0,
	.date={0},
	.magnetic_declination=0,
	.magnetic_EW={0},
	.ADEN={0},
	.valid=0,
	.max_speed=0,
	.last_speed=0,
};
/*
	全局定时器结构体
*/
struct timer_struct global_timer=
{
	.system_timer=0,
	.gps_timer_sec=0,
	.mpu_timer_ms=0,
	.net_timer_ms=999999999,
	.sleep_timer_sec=0,
	.live_timer_sec=0,
	.gnss_timer_ms=0,
	.upload_interval_timer_ms=0,
	.get_video_timer_sec=300,
	.pressure_timer_sec=999,
	.beep_timer=0,
	.local_log_timer_sec=99999,
	.get_time_sec=0,
	.mpu_test_timer_ms=0,
	.net_power_interval_sec=0,
	.system_timer_sec=0,
};
/*
	全局flag集合结构体
*/
struct flag_struct global_flag=
{
	.sleep_flag=0,
	.power_button_flag=0,
	.awaken_flag=0,
	.gps_on_flag=0,
	.flash_busy_flag=0,
	.CENTER_reset_flag=0,
	.config_video_ftp=0,
	.upload_enable_flag=1,
	.upload_video_flag=0,
	.alarm_type=0,
	.CENTER_state_flag=0,
	.system_restart_flag=0,
	.upload_data_flag=1,
	.net_get_time_flag=0,
	.filter_flag = FILTER_ENABLE,//振动值滤波功能1开启0关闭
	.beep_flag=0,
	.button_start_up_flag=0,
	.send_print_message_flag=0,
	.led_lock_flag=0,
	.local_vibration_log_busy_flag=0,
	.print_local_mode_flag=0,
	.battery_low_flag=0,
	.vibration_remote_log_busy_flag=0,
	.need_upload_wave=0,
	.print_run_log_flag=0,
	.net_power_on=0,
	.already_print_flag=0,
	.wait_first_upload_flag=0,
	.net_shutdown_flag=0,
	.task_on_sign = 0,
	.start_up_sign = 0,
};
/*
	休眠结构体
*/
struct sleep_struct global_sleep=
{
	.main_thread_sleep_done=0,
	.led_beep_thread_sleep_done=0,
	.mpu_thread_sleep_done=0,
	.aht10_thread_sleep_done=0,
	.gps_thread_sleep_done=0,
	.rtc_thread_sleep_done=0,
	.bluetooth_thread_sleep_done=0,
	.net_thread_sleep_done=0,
	.si4438_thread_sleep_done=0,
};
/*
	设备信息结构体
*/
struct device_info_struct global_device_info=
{
	.device_time={0},
	.time_stamp=0,
	.tempurature=0,
	.humidity=0,
	.terminal_sn={0},
	.dump_energy=100,
	.uid={0},
	.signal_quality=0,
	.iccid={0},
	.battery_voltage=BATTERY_HIGH_V,
	.Version_number={0},
	.device_ID={0},                    //EC20F/EC200U
	.phone_number = {"15940211652"}, 
};
/*
	rtc设置时间结构体
*/
struct rtc_set_time_struct global_rtc_set_time=
{
	.need_set=0,
	.time_for_set={0},
};


/*
	设备配置结构体
*/
struct device_config_struct global_device_config=
{
	.sleep_time=1800,
	.awake_time=300,
	.interval=30,
	
	.gnss_mode=1,
	.gnss_open_time=3600,
	.gnss_close_time=30,
	
	.gnss_once_flag=0,
	.gnss_once_time=0,
	
	.beep_mode=0,
	
	.vibration_coefficient=1,
	
	.vibration_1_x_a=1,
	.vibration_1_x_b=0,
	.vibration_1_y_a=1,
	.vibration_1_y_b=0,
	.vibration_1_z_a=1,
	.vibration_1_z_b=0,
	
	.vibration_2_x_a=1,
	.vibration_2_x_b=0,
	.vibration_2_y_a=1,
	.vibration_2_y_b=0,
	.vibration_2_z_a=1,
	.vibration_2_z_b=0,
	
	.rf433_mode=1,
	.sleep_network=0,
	
	.net_power_interval=600,
};
/*
	任务信息结构体
*/
struct task_info_struct global_task_info=
{
	.task_num={"no task_num"},
	.task_status=0,
	.task_start_time={0},
	.task_end_time={0},
};

/*
	mqtt配置
*/
struct mqtt_config_struct global_mqtt_config=
{
	.client_idx=0,
	
	//MQTT版本号
	.mqtt_version=3,//3代表v3.1, 4代表v3.1.1,默认3
	
	//PDP上下文ID
	.pdpcid=1,//1~16,默认1
	
	//超时时间
	.pkt_timeout=60,//1~60,默认5秒
	.retry_times=3,//0~10,默认3次
	.timeout_notice=1,//1报告超时，0不报告，默认0
	
	//会话保持
	.clean_session=1,//为1丢弃，为0保持，默认为1
	
	//心跳时间
	.keep_alive_time=3599,//为0永远保持连接，否则为断开的秒数0~3599，默认为120秒
	
	//SSL
	.ssl_enable=0,//为0使用普通TCP，为1使用SSL，默认为0
	.ssl_ctx_idx=0,//ssl上下文ID，0~5
	
	//接受模式
	.msg_recv_mode=1,//为0会在URC中显示，为1不会，默认为0
	.msg_len_enable=1,//为0不显示接收长度，为1显示，默认为1
	
};
/*
	网络配置结构体
*/
struct net_config_struct global_net_config=
{
	//.mqtt_hostname={"119.3.204.58"},
	//.mqtt_hostname={"device.seicsc.com"},
	
	.mqtt_hostname={"119.3.202.243"},
	//.mqtt_port=17615,
	
	.mqtt_port=18085,
	
	.mqtt_client_id={"UID"},
	.mqtt_user_name={"terminal_mqtt"},
	.mqtt_password={"syseic@_terminal"},
	.mqtt_sub_ins_topic={"/8adc78618d46/UID/ins"},
	.mqtt_pub_upload_topic={"/8adc78618d46/topic_upload"},
	.mqtt_pub_upload_topic_gw={"/8adc78618d48/topic_upload"},
	.mqtt_pub_rollback_topic={"/8adc78618d46/topic_rollback_msg"},
	.mqtt_pub_collection_topic={"/8adc78618d48/topic_collection"},
//	.mqtt_pub_collection_topic_gw={"/8adc78618d48/topic_collection_gw"},
	.mqtt_pub_collection_topic_gw={"/8adc78618d48/topic_collection_gw"},	
	.mqtt_pub_wave_topic={"/8adc78618d46/topic_wave_form"},
	.mqtt_pub_task_topic={"/8adc78618d48/task_log_clear"},
	.mqtt_pub_data_topic_gw={"/v1.1/{edgeId}/b_device/data"},
};
struct remote_upgrade_info_struct global_remote_upgrade_info =
{
	//需要升级标志，0不需要，1需要
	.need_upgrade_flag=0,
	 
	.upgrade_len=0,
	//远程升级URL
	.upgrade_url={0},
	.upgrade_mid={0},
	
	.upgrade_jobId={0},
	//远程升级app的版本号
	.upgrade_version={0},
	
	//远程升级包的MD5
	.upgrade_md5={0},
	.upgrade_timestamp={0},
};
/*
	阈值结构体
*/
struct threshold_struct global_threshold=
{
	//电量阈值
	.dump_energy=10,
//	
//	//温度阈值
//	.temperature_high=50,
//	.temperature_low=-30,
//	
//	//倾角阈值
//	.lean_angle_x=30,
//	.lean_angle_y=30,
//	
//	//压力阈值
//	.pressure_high=1000,
//	.pressure_low=-1000,
//	
//	//震动阈值
//	.vibration_x=0.5,
//	.vibration_y=0.5,
//	.vibration_z=0.5,
	
	//湿度阈值
//	.humidity=50,
	
	//速度阈值
	.speed=80,
};
/*
	上传数据的栈
*/
struct stack_struct global_upload_stack =
{
	.front=0,
	.rear=0,
	.size=STACK_SIZE,//这个大小要和下面的数组一样
	.length=0,
	.sign=0,
};
struct upload_data_struct global_upload_data = 
{
	.time={0},
	.ms=0,
	.latitude=0,
	.NS={0},
	.longitude=0,
	.EW={0},
//	.vibration_x=0,
//	.vibration_y=0,
//	.vibration_z=0,
//	.lean_angle_x=0,
//	.lean_angle_y=0,
//	.lean_angle_z=0,
//	.temperature=0,
//	.humidity=0,
	.dump_energy=0,
//	.pressure=999.9,
	.location_mode=0,
	.lac=0,
	.ci=0,
	.speed=0,
//	.vibration_frequency_x=0,
//	.vibration_frequency_y=0,
//	.vibration_frequency_z=0,
	.signal=0,
};
/*
	led_run控制结构体
*/
struct led_control_struct global_led_run=
{
	.color=LED_GREEN,
	.mode=0,
};
/*
	上传数据栈池
*/
struct upload_data_struct global_upload_pool[STACK_SIZE]={0};
/*
	本地振动日志结构体
*/
struct global_local_log_vibration_struct global_local_log_vibration;
/*
	振动日志结构体
*/
//struct global_log_vibration_struct global_log_vibration;
/*
	存储设备信息结构体
*/
struct flash_save_device_info_struct  flash_save_device_info;
/*
	等待串口接收完毕
*/

/*
	ftp配置
*/
struct ftp_config_struct global_ftp_config=
{
	//ip port
	.hostname="119.3.241.184",
	.port=18356,
	
	//path
	.max_data_path={"/"},
	.run_log_path={"/"},
	.download_path={"/"},
	.wave_path={"/"},
	
	.download_filename={""},
	
	//用户名和密码最长255字节
	.username="terminal_upload",
	.password="sysEic@terminal",
	
	//file_type 0是二进制，1是ASCII默认是0
	.filetype=1,
	
	//0是主动模式，由模块监听端口,1是被动模式，由FTP服务器监听端口默认是0
	.transmode=1,
	
	//PDP id 范围1-16 默认是1
	.contextID=1,
	
	//超时时间，20-180 默认90
	.rsptimeout=180,
	
	//0是FTP,1是FTPS,默认是0
	.ssltype=0,
	
	//选择SSL id 默认是0，可选0-5
	.sslctxid=0,
	
	//0 使用服务器分发的地址,1 使用服务器控制会话地址,默认0
	.data_address=0,
};

void common_wait_uart_receive(char *buffer_in)
{
	int string_length = -1;

	while(string_length != strlen(buffer_in))
	{
		string_length = strlen(buffer_in);
		osDelay(10);
	}
};

/*
	时间先后比较函数
*/
int compare_time(struct time_struct time_1_in, struct time_struct time_2_in, int *greater_out)
{
	struct compare_time_ret_struct
	{
		const int RET_OK;
		const int RET_ERROR;
	}compare_time_ret = {0,1};
	
	//变量定义
	struct tm tm_1 = {0};
	struct tm tm_2 = {0};
	time_t time_s_1=0;
	time_t time_s_2=0;
	
	//时间1转换
	tm_1.tm_year = time_1_in.year +2000 - 1900;
	tm_1.tm_mon = time_1_in.month-1;
	tm_1.tm_mday = time_1_in.day;
	tm_1.tm_hour = time_1_in.hour;
	tm_1.tm_min = time_1_in.minute;
	tm_1.tm_sec = time_1_in.second;
	
	time_s_1 = mktime(&tm_1);
	
	//时间2转换
	tm_2.tm_year = time_2_in.year +2000 - 1900;
	tm_2.tm_mon = time_2_in.month-1;
	tm_2.tm_mday = time_2_in.day;
	tm_2.tm_hour = time_2_in.hour;
	tm_2.tm_min = time_2_in.minute;
	tm_2.tm_sec = time_2_in.second;
	
	time_s_2 = mktime(&tm_2);
	
	//比较大小
	if(time_s_1 > time_s_2)
	{
		*greater_out = 1;
	}
	else
	{
		*greater_out = 0;
	}
	
	//函数返回
	return compare_time_ret.RET_OK;
	
}


