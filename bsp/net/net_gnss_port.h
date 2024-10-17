#ifndef __NET_GNSS_PORT_H__
#define __NET_GNSS_PORT_H__

#include "common.h"

/*
	EC20 GNSS���ýṹ��
*/
struct gnss_config_struct 
{
	//NMEA ����� "none" �ǲ������"usbnmea" �����usb ��"uartdebug" �����debug���ڣ�Ĭ��usb
	char outport[16];
	
	//�Ƿ�����ʹ�� AT+QGPSGNMEA ָ�����nmea��� 0��ֹ��1����Ĭ��1
	int nmeasrc;
	
	//������� GPS NMEA ������ͣ���λȡ���ɽ��л����㣬1 GGA, 2 RMC, 4 GSV, 8 GSA, 16 VTG ,Ĭ����31(ȫѡ)
	int gpsnmeatype;
	
	//������� GLONASS NMEA ������ͣ��ɽ��л����㣬1 GSV, 2 GSA, 4 GNS, Ĭ����0��ȫ�أ�
	int glonassnmeatype;
	
	//�Ƿ����� GLONASS �� 0�رգ�1������Ĭ��1
	int glonassenable;
	
	//����ODPģʽ 0 �رգ�1 �͹���ģʽ��2 ׼��ģʽ��Ĭ��0
	int odpcontrol;
	
	//�رա����� DPO�� 0 �رգ�1 ������ Ĭ�� 1
	int dpoenable;
	
	//�����û���Ϳ����棬0 ����SSL���û��棬1 ��SSL���û��棬2 �����棬Ĭ��0
	int plane;
};

/*
	EC20 GNSS ����ṹ��
*/
struct gnss_out_struct
{
	//�Ƿ���Ч
	int valid;
	
	//utc ʱ��
	struct time_struct utc_time;
	
	//γ��
	double latitude;
	char NS[4];
	
	//����
	double longitude;
	char EW[4];
	
	//ˮƽ���� 0.5~99.9
	double hdop;
	
	//���θ߶�
	double altitude;
	
	//GNSS ��λģʽ��2 2D��λ��3 3D��λ
	int fix;
	
	//ƽ�溽��ddd���ȣ�.mm���֣��� ddd 0~360��mm 0~59
	double cog;
	
	//�ٶ� xxxx.x��ǧ��/Сʱ��
	double spkm;
	
	//�ٶ� xxxx.x�����
	double spkn;
	
	//�������� 0~12
	int nsat;
};

/*
	AT+QGPSCFG ����GNSS
*/
int net_gnss_configrate(struct gnss_config_struct *gnss_config_in);

/*
	ɾ����������

	0 ��XTRA�������е����ݣ�������
	1 ��ɾ���κ����ݣ�������
	2 ɾ����������ݣ�������
	3 ɾ��XTRA����
*/
int net_gnss_delete(int deletetype_in);

/*
	���� GNSS

fixmaxtime : ���λʱ�䣬��λ s �� 1~255��Ĭ��30
fixmaxdist �� ��λ��ȷ����ֵ����λ m ��0~1000��Ĭ��50
fixcount : ��λ���ξ͹ص�GNSS���棬Ϊ0��������λ����������ʵ�ʵĶ�λ����,0~1000
fixrate ���������εĶ�λ�������λ s ��1~65535��Ĭ��1
*/
int net_gnss_operate(int fixmaxtime_in, int fixmaxdist_in, int fixcount_in, int fixrate_in);

/*
	�ر� GNSS
*/
int net_gnss_end(void);

/*
	��ȡ��λ��Ϣ
mode_in:
	0: <latitude>,<longitude> format: ddmm.mmmmN/S,dddmm.mmmmE/W
	1: <latitude>,<longitude> format: ddmm.mmmmmm,N/S,dddmm.mmmmmm,E/W
	2: <latitude>,<longitude> format: (-)dd.ddddd,(-)ddd.ddddd
*/
int net_gnss_locate(struct gnss_out_struct *gnss_out);

/*
	ʹ��XTRA������GNSS������Ч

xtraenable_in : 0 �رգ�1 �ֶ�ע�����ݣ� 2 �Զ�ע�����ݣ�Ĭ��0
retries_in : �Զ����ص����Դ��� 0~10��Ĭ��3
retryInt_in : �Զ����ص� �� �� ʱ���� 1~120��Ĭ��10
dloadInt_in: �Զ����ص�ʱ����,24~168,Ĭ��48
*/
int net_gnss_xtra(int xtraenable_in, int retries_in, int retryInt_in, int dloadInt_in);


#endif

