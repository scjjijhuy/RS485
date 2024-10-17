#ifndef _INS_H
#define _INS_H
#include "hc32_ll.h"
#include "main.h"
#define GPS_OLD_CNT   10

struct gps_history_s
{
	uint32_t ind;
	float xs[GPS_OLD_CNT], ys[GPS_OLD_CNT], zs[GPS_OLD_CNT];
};

struct ins_class_s
{
  // ATTITUDE
  float ang_x;
  float ang_y;
  float ang_z;
  float qw;
  float qx; 
  float qy;
  float qz;
				 
  float cos_ang_x;
  float sin_ang_x;
  float cos_ang_y;
  float sin_ang_y;
  float cos_ang_z;
  float sin_ang_z;

  float dcm[3][3];
  float acc_gx_int;
  float acc_gy_int;
  float acc_gz_int;
  float mag_gz_int;

  float acc_kp;
  float acc_ki;
  
  float mag_kp;
  float mag_ki;
  
  bool  inited;
  
  // POSITION
  bool  gp_sts;
  bool  ms_sts;
  bool  mw_sts;

  float nav_cor_c;

  float acc_int_e;
  float acc_int_n;
  float acc_now_e;
  float acc_now_n;
  float acc_now_u;
  float acc_udp_e;
  float acc_udp_n;
  float vel_now_e;
  float vel_now_n;
  float vel_now_u;
  float pos_now_e;
  float pos_now_n; 
  float pos_now_u;
  float pos_bis_e;
  float pos_bis_n;
  float vel_now_l;

  float acc_int_ms;
  float acn_now_ms;
  float pos_bis_ms;
  float vel_now_ms;
  float pos_now_ms;

  float acc_int_mw;
  float acc_udp_mw;
  float pos_bis_mw;
  float vel_now_mw;
  float pos_now_mw;

  float acc_int_gp;
  float acc_udp_gp;
  float pos_bis_gp;
  float vel_now_gp;
  float pos_now_gp;

  float alt_coe_mw;
  float alt_coe_gp;
  float alt_coe_ms;

  uint16_t gps_old_cnt;
  
  float ins_dt;
  float ins_ht;
  
  struct gps_history_s gh;
  
  uint32_t last_gps_rec_tic;
  
  float time_cons_xy;
	float coe_pos_xy;
	float coe_vel_xy;
	float coe_acc_xy;
  float time_cons_z;
	float coe_pos_z;
	float coe_vel_z;
	float coe_acc_z;
  
  float pos_lat_gp;
  
  float alt_gps_err;
  float alt_mw_err;
  bool  initing;
  //***** function ******
  void  (*run)(void);
};
extern struct ins_class_s ins_class;
void ins_init(struct ins_class_s* _ins);

#endif
