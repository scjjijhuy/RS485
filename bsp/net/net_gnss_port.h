#ifndef __NET_GNSS_PORT_H__
#define __NET_GNSS_PORT_H__

#include "common.h"

/*
	EC20 GNSS配置结构体
*/
struct gnss_config_struct 
{
	//NMEA 输出口 "none" 是不输出，"usbnmea" 输出到usb ，"uartdebug" 输出到debug串口，默认usb
	char outport[16];
	
	//是否允许使用 AT+QGPSGNMEA 指令输出nmea语句 0禁止，1允许，默认1
	int nmeasrc;
	
	//配置输出 GPS NMEA 语句类型，按位取，可进行或运算，1 GGA, 2 RMC, 4 GSV, 8 GSA, 16 VTG ,默认是31(全选)
	int gpsnmeatype;
	
	//配置输出 GLONASS NMEA 语句类型，可进行或运算，1 GSV, 2 GSA, 4 GNS, 默认是0（全关）
	int glonassnmeatype;
	
	//是否启用 GLONASS ， 0关闭，1开启，默认1
	int glonassenable;
	
	//设置ODP模式 0 关闭，1 低功耗模式，2 准备模式，默认0
	int odpcontrol;
	
	//关闭、开启 DPO， 0 关闭，1 开启， 默认 1
	int dpoenable;
	
	//设置用户面和控制面，0 不用SSL的用户面，1 用SSL的用户面，2 控制面，默认0
	int plane;
};

/*
	EC20 GNSS 输出结构体
*/
struct gnss_out_struct
{
	//是否有效
	int valid;
	
	//utc 时间
	struct time_struct utc_time;
	
	//纬度
	double latitude;
	char NS[4];
	
	//经度
	double longitude;
	char EW[4];
	
	//水平精度 0.5~99.9
	double hdop;
	
	//海拔高度
	double altitude;
	
	//GNSS 定位模式，2 2D定位，3 3D定位
	int fix;
	
	//平面航向ddd（度）.mm（分）， ddd 0~360，mm 0~59
	double cog;
	
	//速度 xxxx.x（千米/小时）
	double spkm;
	
	//速度 xxxx.x（海里）
	double spkn;
	
	//卫星数量 0~12
	int nsat;
};

/*
	AT+QGPSCFG 配置GNSS
*/
int net_gnss_configrate(struct gnss_config_struct *gnss_config_in);

/*
	删除辅助数据

	0 除XTRA以外所有的数据，冷启动
	1 不删除任何数据，热启动
	2 删除最近的数据，温启动
	3 删除XTRA数据
*/
int net_gnss_delete(int deletetype_in);

/*
	操作 GNSS

fixmaxtime : 最大定位时间，单位 s ， 1~255，默认30
fixmaxdist ： 定位精确度阈值，单位 m ，0~1000，默认50
fixcount : 定位几次就关掉GNSS引擎，为0则连续定位，非零则是实际的定位次数,0~1000
fixrate ：连续两次的定位间隔，单位 s ，1~65535，默认1
*/
int net_gnss_operate(int fixmaxtime_in, int fixmaxdist_in, int fixcount_in, int fixrate_in);

/*
	关闭 GNSS
*/
int net_gnss_end(void);

/*
	获取定位信息
mode_in:
	0: <latitude>,<longitude> format: ddmm.mmmmN/S,dddmm.mmmmE/W
	1: <latitude>,<longitude> format: ddmm.mmmmmm,N/S,dddmm.mmmmmm,E/W
	2: <latitude>,<longitude> format: (-)dd.ddddd,(-)ddd.ddddd
*/
int net_gnss_locate(struct gnss_out_struct *gnss_out);

/*
	使能XTRA，重启GNSS引擎生效

xtraenable_in : 0 关闭，1 手动注入数据， 2 自动注入数据，默认0
retries_in : 自动下载的重试次数 0~10，默认3
retryInt_in : 自动下载的 重 试 时间间隔 1~120，默认10
dloadInt_in: 自动下载的时间间隔,24~168,默认48
*/
int net_gnss_xtra(int xtraenable_in, int retries_in, int retryInt_in, int dloadInt_in);


#endif

