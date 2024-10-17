/*
	gps底层源文件
	作者：lee
	时间：2019/8/15
*/

#include "main.h"
extern uint8_t gps_Factory_settings_state;

//gps输出结果结构体 
struct gps_struct
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
};

//gps生字段缓存结构体


//gps熟字段缓存结构体
struct gps_ripe_struct
{
	//解析后的变量 
	struct gps_time_struct 	d_utc_time;
	char 				d_enable_state[2];
	double 				d_latitude;
	char 				d_NS[2];
	double 				d_longitude;
	char 				d_EW[2];
	double				d_speed;
	double 				d_direction;
	struct gps_date_struct 	d_utc_date;
	double 				d_magnetic_declination;
	char 				d_magnetic_EW[2];
	char	 			d_ADEN[2];
	unsigned char 		d_check_value;
};

/////////////////////////////////////////变量定义 

//临时缓存 
static char gps_temp_buffer[128]={0};
struct gps_raw_struct gps_raw=
{
	.s_utc_time = {0},
	.s_enable_state = {'?'},
	.s_latitude = {'?'},
	.s_NS = {'?'},
	.s_longitude = {'?'},
	.s_EW = {'?'},
	.s_speed = {'?'},
	.s_direction = {0},
	.s_utc_date = {0},
	.s_magnetic_declination = {0},
	.s_magnetic_EW = {0},
	.s_ADEN = {0},
	.s_check_value = {0},
}; 
struct gps_ripe_struct gps_ripe={0}; 

//时间日期临时变量
time_t temp_time_t=0; 
struct tm temp_tm={0};

//传出的结构体 
struct gps_struct gps_info={0};

//函数声明 
int gps_parse(char *gps_str, struct gps_struct *gps_out);
void gps_print(struct gps_struct *gps_in);

/*
	gps开关控制
*/
int gps_power(int on_off)
{
    return 0;
}
uint8_t gps_data_switch = 0;
int gps_analysis(char *recv_buf)
{
	int ret=0;
	
	ret = gps_parse(recv_buf, &gps_info);
	if(gps_info.speed > 5)
	{
		  global_timer.sleep_timer_sec = 0;
	}
	if(0 == ret)
	{
		//printf("\n[gps]gps解析成功\n");
		//gps_print(&gps_info);
		global_gps_out.date.year = gps_info.date.year;
		global_gps_out.date.month = gps_info.date.month;
		global_gps_out.date.day = gps_info.date.day;
		global_gps_out.time.hour = gps_info.time.hour;
		global_gps_out.time.minute = gps_info.time.minute;
		global_gps_out.time.second = gps_info.time.second;
		global_gps_out.latitude = gps_info.latitude;
		global_gps_out.NS[0] = gps_info.NS[0];
		global_gps_out.longitude = gps_info.longitude;
		global_gps_out.EW[0] = gps_info.EW[0];
		global_gps_out.speed = gps_info.speed;
		global_gps_out.direction = gps_info.direction;
		global_gps_out.enable_state[0]=gps_info.enable_state[0];
		global_gps_out.valid = gps_info.enable_state[0]=='A'?1:0;
	}
	else
	{
		//printf("\n[gps]gps解析失败\n");
		if(ret == -2)
		{
			if((temp_gps-gps_time_out) > 30)
			{
				gps_time_out = temp_gps;
				global_gps_out.valid = 0;
			}
		}
		else
		{
			global_gps_out.valid = 0;
		}
	}
	return 0;
}
void gps_checksum(char *tmp)
{
	uint8_t ck_a = 0, ck_b = 0;	
	uint16_t j;
	uint8_t cnt = tmp[4] + 6;
	for(j = 2; j < cnt; j++)
	{
		ck_a = ck_a + tmp[j];
		ck_b = ck_b + ck_a;
	}
	tmp[j++] = ck_a;
	tmp[j] = ck_b;
}
void gps_config(void)
{
	 char gngga_cfg_disable[11] = {0xB5,0x62,0x06,0x01,0x03,0x00,0xF0,0x00,0x00,0xFA,0x0F};
//	static char gngga_cfg_enable[11]  = {0xB5,0x62,0x06,0x01,0x03,0x00,0xFF,0x00,0x01,0xFB,0x10};
	 char gngll_cfg_disable[11] = {0xB5,0x62,0x06,0x01,0x03,0x00,0xF0,0x01,0x00,0xFB,0x11};
	 char gngsa_cfg_disable[11] = {0xB5,0x62,0x06,0x01,0x03,0x00,0xF0,0x02,0x00,0xFC,0x13};
	 char gngsv_cfg_disable[11] = {0xB5,0x62,0x06,0x01,0x03,0x00,0xF0,0x03,0x00,0xFD,0x15};
	 char gnvtg_cfg_disable[11] = {0xB5,0x62,0x06,0x01,0x03,0x00,0xF0,0x05,0x00,0xFF,0x19};
	 char gnzda_cfg_disable[11] = {0xB5,0x62,0x06,0x01,0x03,0x00,0xF0,0x08,0x00,0x02,0x1F};
	 char gps_save_config[21]   = {0xB5,0x62,0x06,0x09,0x0D,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x17,0x31,0xBF};
		
    gps_checksum(gngga_cfg_disable);
	gps_checksum(gngll_cfg_disable);
	gps_checksum(gngsa_cfg_disable);
	gps_checksum(gngsv_cfg_disable);
	gps_checksum(gnvtg_cfg_disable);
	gps_checksum(gnzda_cfg_disable);
	gps_checksum(gps_save_config);
	
    DMA_transfer_IT(CM_USART2,CM_DMA2,DMA_CH2,(uint32_t)(&gngga_cfg_disable[0]),11);
	osDelay(100);
	DMA_transfer_IT(CM_USART2,CM_DMA2,DMA_CH2,(uint32_t)(&gngll_cfg_disable[0]),11);
	osDelay(100);
	DMA_transfer_IT(CM_USART2,CM_DMA2,DMA_CH2,(uint32_t)(&gngsa_cfg_disable[0]),11);
	osDelay(100);
	DMA_transfer_IT(CM_USART2,CM_DMA2,DMA_CH2,(uint32_t)(&gngsv_cfg_disable[0]),11);
	osDelay(100);
	DMA_transfer_IT(CM_USART2,CM_DMA2,DMA_CH2,(uint32_t)(&gnvtg_cfg_disable[0]),11);
	osDelay(100);
	DMA_transfer_IT(CM_USART2,CM_DMA2,DMA_CH2,(uint32_t)(&gnzda_cfg_disable[0]),11);
	osDelay(100);
	DMA_transfer_IT(CM_USART2,CM_DMA2,DMA_CH2,(uint32_t)(&gps_save_config[0]),21);
	osDelay(100);	
}
//////////////////////////////////////////////////////////////////////////

//gps信息输出函数 
void gps_print(struct gps_struct *gps_in)
{
	printf("\n--------------------------------------------------------------------------------\n");
	printf("time = %02d:%02d:%02d\n", gps_in->time.hour, gps_in->time.minute, gps_in->time.second);
	printf("enable_state = %s\n",gps_in->enable_state);
	printf("latitude = %f\n",gps_in->latitude);
	printf("NS = %s\n",gps_in->NS);
	printf("longitude = %f\n",gps_in->longitude);
	printf("EW = %s\n",gps_in->EW);
	printf("speed = %f km/h\n",gps_in->speed);
	printf("direction = %f\n",gps_in->direction);
	printf("date = %04d-%02d-%02d\n",gps_in->date.year+2000, gps_in->date.month, gps_in->date.day);
	printf("magnetic_declination = %f\n",gps_in->magnetic_declination);
	printf("magnetic_EW = %s\n",gps_in->magnetic_EW);
	printf("ADEN = %s\n",gps_in->ADEN);
	printf("valid = %d\n",global_gps_out.valid);
	printf("\n--------------------------------------------------------------------------------\n");
}
void GPS_data_init(void)
{
	memset(&gps_raw, 0, sizeof(gps_raw));
	gps_raw.s_enable_state[0] = '?';
	gps_raw.s_NS[0] = '?';
}
//gps解析函数 
int gps_parse(char *gps_str, struct gps_struct *gps_out)
{
	//局部变量
	char* p=NULL;
	int i=0;
	unsigned char my_check_value=0;
	int temp_check_value=0;
	
	//printf("\n[gps]原始字符串：\n%s\n",gps_str);
	
	//清空缓存
	memset(gps_temp_buffer, 0, sizeof(gps_temp_buffer));
//	memset(&gps_raw, 0, sizeof(gps_raw));
	GPS_data_init();
	memset(&gps_ripe, 0, sizeof(gps_ripe)); 
 
	//先检查是否有GNRMC字段
 	p=strstr(gps_str,"$GNRMC");
 	if(p!=NULL)
 	{
		//printf("\n[gps]发现$GNRMC字段\n"); 	
 	}
 	else
 	{
	  //printf("\n[gps]未发现$GNRMC字段，函数返回-1\n");
	 	return -2; 
 	}
	
	//先计算校验值
	p=strstr(gps_str,"$GNRMC");
	if(p!=NULL)
	{
		memset(gps_temp_buffer,0,sizeof(gps_temp_buffer));
		for(i=0,p+=1 ; *p!='*' && *p!='\0' ; )
		{
			gps_temp_buffer[i++]=*p;
			p+=1;
		}
		//printf("\n[gps]待校验的字符串为:\n%s\n",gps_temp_buffer);
		
		for(i=0; i<strlen(gps_temp_buffer); i++)
		{
			my_check_value^=gps_temp_buffer[i];
		}
		//printf("\n[gps]计算后的校验值为%x\n", my_check_value);
	}
//	
//	//再添加占位符 
	p=strstr(gps_str,"$GNGGA");
	if(p!=NULL)
	{
		//配置文件
		gps_Factory_settings_state = 1;
	}
	p=strstr(gps_str,"$GNRMC");
	if(p!=NULL)
	{
		memset(gps_temp_buffer,0,sizeof(gps_temp_buffer));
		for(i=0 ; *p!='\n' && *p!='\0' ; )
		{
			if(*p!=',')
			{
				gps_temp_buffer[i++]=*p;
			}
			else
			{
				gps_temp_buffer[i++]='?';
				gps_temp_buffer[i++]=*p;	
			}
			p++;
		}
	}
//	//printf("\n[gps]添加完占位符之后的效果：\n%s\n",gps_temp_buffer);
////*********************************************************************************
////刘华科 20220104  
////*********************************************************************************		
	if(gps_data_switch == 1)
	{
			printf("\n[gps]添加完占位符之后的效果：\n%s\n",gps_temp_buffer);
	}
	//再拆分字段 
	sscanf(gps_temp_buffer,"$GNRMC?,%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^*]*%[^\n]", \
	gps_raw.s_utc_time, gps_raw.s_enable_state, gps_raw.s_latitude, gps_raw.s_NS, gps_raw.s_longitude, gps_raw.s_EW, gps_raw.s_speed,\
	 gps_raw.s_direction, gps_raw.s_utc_date, gps_raw.s_magnetic_declination, gps_raw.s_magnetic_EW,\
	 gps_raw.s_ADEN, gps_raw.s_check_value);
	
	/*
	printf("\n[gps]拆分后的效果：\n%s/%s/%s/%s/%s/%s/%s/%s/%s/%s/%s/%s/%s\n",\
	 gps_raw.s_utc_time, gps_raw.s_enable_state, gps_raw.s_latitude, gps_raw.s_NS, gps_raw.s_longitude, gps_raw.s_EW, gps_raw.s_speed,\
	 gps_raw.s_direction, gps_raw.s_utc_date, gps_raw.s_magnetic_declination, gps_raw.s_magnetic_EW,\
	 gps_raw.s_ADEN, gps_raw.s_check_value);
	*/
	//再解析字段 
//	if((gps_raw.s_enable_state[0] != '?') && (gps_raw.s_latitude[0] != '?') &&
//	   (gps_raw.s_NS[0] != '?') && (gps_raw.s_longitude[0] != '?') &&
//	   (gps_raw.s_EW[0] != '?') && (gps_raw.s_speed[0] != '?'))
//	{
//		
//	}
 	//s_utc_time
	char temp_hour[8]={0};
	char temp_minute[8]={0};
	char temp_second[8]={0};
	
	temp_hour[0]=gps_raw.s_utc_time[0];
	temp_hour[1]=gps_raw.s_utc_time[1];
	temp_hour[2]='\0';
	temp_minute[0]=gps_raw.s_utc_time[2];
	temp_minute[1]=gps_raw.s_utc_time[3];
	temp_minute[2]='\0';
	temp_second[0]=gps_raw.s_utc_time[4];
	temp_second[1]=gps_raw.s_utc_time[5];
	temp_second[3]='\0';
	
	gps_ripe.d_utc_time.hour = atoi(temp_hour);
	gps_ripe.d_utc_time.minute = atoi(temp_minute);
	gps_ripe.d_utc_time.second = atoi(temp_second);
	
	//printf("\n[gps][d_utc_time]h=%d,m=%d,s=%d\n", gps_ripe.d_utc_time.hour, gps_ripe.d_utc_time.minute, gps_ripe.d_utc_time.second );
	
	//s_enable_state
	gps_ripe.d_enable_state[0] = gps_raw.s_enable_state[0];
	gps_ripe.d_enable_state[1]='\0';
	//printf("\n[gps][d_enable_state]=%s\n",gps_ripe.d_enable_state);
	
	//s_latitude
	gps_ripe.d_latitude = atof(gps_raw.s_latitude);
	//printf("\n[gps][d_latitude]=%f\n",gps_ripe.d_latitude);
	
	//s_NS
	gps_ripe.d_NS[0]=gps_raw.s_NS[0];
	gps_ripe.d_NS[1]='\0';
	//printf("\n[gps][s_NS]%s\n",gps_ripe.d_NS);
	
	//s_longitude
	gps_ripe.d_longitude = atof(gps_raw.s_longitude);
	//printf("\n[gps][d_longitude]=%f\n",gps_ripe.d_longitude);
	
	//s_EW
	gps_ripe.d_EW[0] = gps_raw.s_EW[0];
	gps_ripe.d_EW[1] = '\0';
	//printf("\n[gps][d_EW]%s\n",gps_ripe.d_EW);
	
	//s_speed
	gps_ripe.d_speed = atof(gps_raw.s_speed);
 // printf("\n[gps][d_speed]=%f\n",gps_ripe.d_speed);
	
	//s_direction
	gps_ripe.d_direction = atof(gps_raw.s_direction);
	//printf("\n[gps][d_direction]=%f\n",gps_ripe.d_direction);
	
	//s_utc_date
	char temp_year[8]={0};
	char temp_month[8]={0};
	char temp_day[8]={0};
	
	temp_day[0]=gps_raw.s_utc_date[0];
	temp_day[1]=gps_raw.s_utc_date[1];
	temp_day[2]='\0';
	temp_month[0]=gps_raw.s_utc_date[2];
	temp_month[1]=gps_raw.s_utc_date[3];
	temp_month[2]='\0';
	temp_year[0]=gps_raw.s_utc_date[4];
	temp_year[1]=gps_raw.s_utc_date[5];
	temp_year[3]='\0';
	
	gps_ripe.d_utc_date.year = atoi(temp_year);
	gps_ripe.d_utc_date.month = atoi(temp_month);
	gps_ripe.d_utc_date.day = atoi(temp_day);
	
 // printf("\n[gps][d_utc_date]y=%d,m=%d,d=%d\n", gps_ripe.d_utc_date.year,gps_ripe.d_utc_date.month,gps_ripe.d_utc_date.day );
	
	//s_magnetic_declination
	gps_ripe.d_magnetic_declination = atof(gps_raw.s_magnetic_declination);
	//printf("\n[gps][d_magnetic_declination]=%f\n", gps_ripe.d_magnetic_declination);
	
	//s_magnetic_EW
	gps_ripe.d_magnetic_EW[0] = gps_raw.s_magnetic_EW[0];
	gps_ripe.d_magnetic_EW[1] = '\0';
	//printf("\n[gps][d_magnetic_EW]=%s\n", gps_ripe.d_magnetic_EW);
	
	//s_ADEN
	gps_ripe.d_ADEN[0] = gps_raw.s_ADEN[0];
	gps_ripe.d_ADEN[1] = '\0';
	//printf("\n[gps][d_ADEN]=%s\n", gps_ripe.d_ADEN);
	
	//s_check_value
	sscanf(gps_raw.s_check_value,"%x",&temp_check_value);
	gps_ripe.d_check_value=temp_check_value;
	
	//printf("\n[gps][d_check_value]=%x\n", gps_ripe.d_check_value);
	
	//判断校验值是否正确
	if(gps_ripe.d_check_value == my_check_value)
	{
		//printf("\n[gps]校验值相同，数据有效\n");
		restart_usart2_recv();
	}
	else
	{
		//printf("\n[gps]校验值不同，数据无效\n");
		return -2;
	}
	
	//将时间转换为+8时区
	temp_tm.tm_sec = gps_ripe.d_utc_time.second;
	temp_tm.tm_min = gps_ripe.d_utc_time.minute;
	temp_tm.tm_hour = gps_ripe.d_utc_time.hour;
	temp_tm.tm_mday = gps_ripe.d_utc_date.day;
	temp_tm.tm_mon = gps_ripe.d_utc_date.month - 1;
	temp_tm.tm_year = 2000 + gps_ripe.d_utc_date.year - 1900;
	temp_tm.tm_wday = 0;
	temp_tm.tm_yday = 0;
	temp_tm.tm_isdst = -1;
	
	temp_time_t = mktime(&temp_tm);
	if(-1 != (int)temp_time_t) 
	{
		//printf("\n[gps]mktime = %d\n",temp_time_t);
		temp_time_t += 3600*8;
		temp_tm = *localtime(&temp_time_t);
		//printf("[gps]转换后的时间Local time is: %s", asctime(&temp_tm));
		gps_ripe.d_utc_time.second = temp_tm.tm_sec;
		gps_ripe.d_utc_time.minute = temp_tm.tm_min;
		gps_ripe.d_utc_time.hour = temp_tm.tm_hour;
		gps_ripe.d_utc_date.day = temp_tm.tm_mday;
		gps_ripe.d_utc_date.month = temp_tm.tm_mon + 1;
		gps_ripe.d_utc_date.year = temp_tm.tm_year - 100;
	}
	else
	{
		//printf("\n[gps]日期转换出错，mktime()返回-1\n");
		return -1;
	}
    
  //将速度转换为km/h
 	gps_ripe.d_speed *= 1.852;
 	//printf("\n[gps]转换后的速度speed = %fkm/h\n",gps_ripe.d_speed);
    
  //装载到传出参数里
 	gps_out->time 					= gps_ripe.d_utc_time;
 	gps_out->enable_state[0] 		= gps_ripe.d_enable_state[0];
 	gps_out->enable_state[1] 		= gps_ripe.d_enable_state[1];
	gps_out->latitude 				= gps_ripe.d_latitude;
	gps_out->NS[0] 					= gps_ripe.d_NS[0];
	gps_out->NS[1]					= gps_ripe.d_NS[1];
	gps_out->longitude 				= gps_ripe.d_longitude;
	gps_out->EW[0] 					= gps_ripe.d_EW[0];
	gps_out->EW[1] 					= gps_ripe.d_EW[1];
	gps_out->speed					= gps_ripe.d_speed;
	gps_out->direction				= gps_ripe.d_direction;
	gps_out->date					= gps_ripe.d_utc_date;
	gps_out->magnetic_declination 	= gps_ripe.d_magnetic_declination;
	gps_out->magnetic_EW[0]			= gps_ripe.d_magnetic_EW[0];
	gps_out->magnetic_EW[1]			= gps_ripe.d_magnetic_EW[1];
	gps_out->ADEN[0]				= gps_ripe.d_ADEN[0];
	gps_out->ADEN[1]				= gps_ripe.d_ADEN[1];
    gps_time_out = temp_gps;
	return 0;
}

