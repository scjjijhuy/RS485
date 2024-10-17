#include "ins.h"
#include "imu.h"
#include "fmath.h"
#include "arm_math.h"


#define MAG_CORRECT_ANG
#define ACC_CORRECT_ANG

static struct ins_class_s* ins;
float bod_ang[9];
static void bod_to_enu(float ix, float iy, float iz, float *ox, float *oy, float *oz)
{
	bod_ang[0] = (0.5f - ins->qy*ins->qy - ins->qz*ins->qz) * 2 ;
	bod_ang[1] = (ins->qw*ins->qz - ins->qx*ins->qy) * 2;
	bod_ang[2] = (ins->qw*ins->qy + ins->qx*ins->qz) * 2;
	
	bod_ang[3] = (0.5f - ins->qx*ins->qx - ins->qz*ins->qz) * 2;
	bod_ang[4] =  (ins->qw*ins->qz + ins->qx*ins->qy) * 2;
	bod_ang[5] = (ins->qw*ins->qx - ins->qy*ins->qz) * 2;
	
	bod_ang[6] = (0.5f - ins->qx*ins->qx - ins->qy*ins->qy) * 2;
	bod_ang[7] = (ins->qw*ins->qy - ins->qx*ins->qz) * 2;
	bod_ang[8] = (ins->qw*ins->qx + ins->qy*ins->qz) * 2;
	
	*ox = 2 * (ix * (0.5f - ins->qy*ins->qy - ins->qz*ins->qz) - iy * (ins->qw*ins->qz - ins->qx*ins->qy) + iz * (ins->qw*ins->qy + ins->qx*ins->qz));
	*oy = 2 * (iy * (0.5f - ins->qx*ins->qx - ins->qz*ins->qz) + ix * (ins->qw*ins->qz + ins->qx*ins->qy) - iz * (ins->qw*ins->qx - ins->qy*ins->qz));
	*oz = 2 * (iz * (0.5f - ins->qx*ins->qx - ins->qy*ins->qy) - ix * (ins->qw*ins->qy - ins->qx*ins->qz) + iy * (ins->qw*ins->qx + ins->qy*ins->qz));
}

static void enu_to_bod(float ix, float iy, float iz, float *ox, float *oy, float *oz)
{
	*ox = 2 * (ix * (0.5f - ins->qy*ins->qy - ins->qz*ins->qz) + iy * (ins->qw*ins->qz + ins->qx*ins->qy) - iz * (ins->qw*ins->qy - ins->qx*ins->qz));
	*oy = 2 * (iy * (0.5f - ins->qx*ins->qx - ins->qz*ins->qz) - ix * (ins->qw*ins->qz - ins->qx*ins->qy) + iz * (ins->qw*ins->qx + ins->qy*ins->qz));
	*oz = 2 * (iz * (0.5f - ins->qx*ins->qx - ins->qy*ins->qy) + ix * (ins->qw*ins->qy + ins->qx*ins->qz) - iy * (ins->qw*ins->qx - ins->qy*ins->qz));
}

//********************************** ATTITUDE **********************************

float ang_z_f = 0.0;
float dqw;
float dqx;
float dqy;
float dqz;
float mag_ang_z = 0.0;
float ang_z;
static void calc_quat(void)
{
	float ax = -imu_class.ax, ay = imu_class.ay, az = imu_class.az;
	float gx = imu_class.gx, gy = -imu_class.gy, gz = -imu_class.gz;
	float mx = 662.777344f, my = -41.2726479f, mz = 994.077515f; 

#ifdef ACC_CORRECT_ANG
	
	if(fabsf(ax) > eps || fabsf(ay) > eps || fabsf(az) > eps)
	{
		nor_thr(&ax, &ay, &az);

		float ex = 0.0f, ey = 0.0f, ez = g;
//		if(ins->gp_sts && gps_class.fix)
//		{
//			ex = ins->acc_udp_e;
//			ey = ins->acc_udp_n;
//		}
		nor_thr(&ex, &ey, &ez);
		
		float vx = 0.0f, vy = 0.0f, vz = 0.0f;
		enu_to_bod(ex, ey, ez, &vx, &vy, &vz);

		float acc_gx = (ay * vz - az * vy);
		float acc_gy = (az * vx - ax * vz);
		float acc_gz = (ax * vy - ay * vx);

		ins->acc_gx_int += acc_gx * ins->acc_ki * ins->ins_dt;
		ins->acc_gy_int += acc_gy * ins->acc_ki * ins->ins_dt;
		ins->acc_gz_int += acc_gz * ins->acc_ki * ins->ins_dt;
		
		gx += ins->acc_kp * acc_gx + ins->acc_gx_int;
		gy += ins->acc_kp * acc_gy + ins->acc_gy_int;
		gz += ins->acc_kp * acc_gz + ins->acc_gz_int;
	}
#endif

#ifdef MAG_CORRECT_ANG

	if(fabsf(mx) > eps || fabsf(my) > eps || fabsf(mz) > eps)
	{

	}
#endif

	dqw =(-ins->qx * gx - ins->qy * gy - ins->qz * gz) * ins->ins_ht;
	dqx = (ins->qw * gx + ins->qy * gz - ins->qz * gy) * ins->ins_ht;
	dqy = (ins->qw * gy - ins->qx * gz + ins->qz * gx) * ins->ins_ht;
	dqz = (ins->qw * gz + ins->qx * gy - ins->qy * gx) * ins->ins_ht;
	
	ins->qw += dqw; ins->qx += dqx; ins->qy += dqy; 
	if(fabs(dqz) < 0.0001)
	{
		dqz = 0;
	}
	ins->qz += dqz;
	nor_for(&(ins->qw), &(ins->qx), &(ins->qy), &(ins->qz));
}
float tmp_ang_z = 0.0;
float gyr_z_int = 0;
int totalgyr[3] = {0};
int ins_num = 0,kf_start = 0;

float  tmp_roll,tmp_pitch,tmp_yaw;
static void calc_angle(void)
{

	  

    ins->ang_x = atan2f( 2 * (ins->qy*ins->qz + ins->qw*ins->qx), 1 - 2 * (ins->qx*ins->qx + ins->qy*ins->qy));
		ins->ang_y = asinf( -2 * (ins->qx*ins->qz - ins->qw*ins->qy));
		ins->ang_z = atan2f( 2 * (ins->qx*ins->qy + ins->qw*ins->qz), 1 - 2 * (ins->qy*ins->qy + ins->qz*ins->qz));	  

	ins->cos_ang_x = arm_cos_f32(ins->ang_x);
	ins->sin_ang_x = arm_sin_f32(ins->ang_x);
	ins->cos_ang_y = arm_cos_f32(ins->ang_y);
	ins->sin_ang_y = arm_sin_f32(ins->ang_y);
	ins->cos_ang_z = arm_cos_f32(ins->ang_z);
	ins->sin_ang_z = arm_sin_f32(ins->ang_z);
}
float dcm[3][3];
static void calc_dcm(void)
{
	ins->dcm[0][0] =  ins->qx*ins->qx - ins->qy*ins->qy - ins->qz*ins->qz + ins->qw*ins->qw;
	ins->dcm[1][0] = 2.0f * (ins->qx*ins->qy + ins->qw*ins->qz);
	ins->dcm[2][0] = 2.0f * (ins->qx*ins->qz - ins->qw*ins->qy);
	ins->dcm[0][1] = 2.0f * (ins->qx*ins->qy - ins->qw*ins->qz);
	ins->dcm[1][1] = -ins->qx*ins->qx + ins->qy*ins->qy - ins->qz*ins->qz + ins->qw*ins->qw;
	ins->dcm[2][1] = 2.0f * (ins->qy*ins->qz + ins->qw*ins->qx);
	ins->dcm[0][2] = 2.0f * (ins->qx*ins->qz + ins->qw*ins->qy);
	ins->dcm[1][2] = 2.0f * (ins->qy*ins->qz - ins->qw*ins->qx);
	ins->dcm[2][2] = -ins->qx*ins->qx - ins->qy*ins->qy + ins->qz*ins->qz + ins->qw*ins->qw;	
	
	float cy = cos(ins->ang_z);
	float sy = sin(ins->ang_z);
	float cp = cos(ins->ang_y);
	float sp = sin(ins->ang_y);
	float cr = cos(ins->ang_x);
	float sr = sin(ins->ang_x);
	dcm[0][0] = cy*cp;
	dcm[0][1] = cy*sp*sr - sy*cr;
	dcm[0][2] = cy*sp*cr + sy*sr;
	dcm[1][0] = sy*cp;
	dcm[1][1] = sy*sp*sr + cy*cr;
	dcm[1][2] = sy*sp*cr - cy*sr;
	dcm[2][0] =  -sp;
	dcm[2][1] = cp*sr;
	dcm[2][2] = cp*cr;
	
}

static void init_quat(void)
{
//	if(hrt_time_ms() < 1000 && !ins->inited)
//	{
//		ins->qw = arm_cos_f32(mag_class.ang_z * 0.5f);
//		ins->qx = 0.0f;
//		ins->qy = 0.0f;
//		ins->qz = arm_sin_f32(mag_class.ang_z * 0.5f);
//	}
//	else
//	{
		ins->inited = true;
//	}
}



//********************************** POSITIONM **********************************
void nav_acc_update(void)
{
	bod_to_enu(imu_class.ax, -imu_class.ay, -imu_class.az, &(ins->acc_now_n), &(ins->acc_now_e), &(ins->acc_now_u));
}

void nav_cor_update(void)
{
	ins->nav_cor_c = 2 * (ins->qw * ins->qw + ins->qz * ins->qz) - 1.0f;
	ins->nav_cor_c = cons(ins->nav_cor_c, 0.80f, 1.00f);
}
float pos_e = 1.0f,pos_n = 1.0f,pos_u = 1.0f;
void gps_err_update(void)
{
//	if(!gps_class.home_fix){ return; }
//	
//	float dx = 0.0f, dy = 0.0f, dz = 0.0f;
//	bod_to_enu(0.035f, 0.035f, 0.152f, &dx, &dy, &dz);
	
	ins->gh.ind++;
	ins->gh.ind = ins->gh.ind == ins->gps_old_cnt ? 0 : ins->gh.ind;
	ins->gh.xs[ins->gh.ind] = ins->pos_now_e;
	ins->gh.ys[ins->gh.ind] = ins->pos_now_n;
	ins->gh.zs[ins->gh.ind] = ins->pos_now_gp;
	

	int ind = ins->gh.ind ;
	ind = ind < 0 ? ind + ins->gps_old_cnt : ind;
	ind = cons(ind, 0, ins->gps_old_cnt - 1);
	
	ins->pos_bis_e  = pos_e - ins->gh.xs[ind];
	ins->pos_bis_n  = pos_n - ins->gh.ys[ind];
	ins->pos_bis_gp = pos_u - ins->gh.zs[ind];
}

static void gps_cal_update(void)
{
//	if(!gps_class.home_fix || gps_class.err_cnt > GPS_ERR_PERMIT)
//	{
//		ins->gp_sts = false; return;
//	}
	ins->gp_sts = true;
	
	float pos_bod_err_x =  ins->cos_ang_z * ins->pos_bis_e + ins->sin_ang_z * ins->pos_bis_n;
	float pos_bod_err_y = -ins->sin_ang_z * ins->pos_bis_e + ins->cos_ang_z * ins->pos_bis_n;
//	float pos_bod_err_x =  ins->cos_ang_z * ins->pos_bis_n + ins->sin_ang_z * ins->pos_bis_e;
//	float pos_bod_err_y = -ins->sin_ang_z * ins->pos_bis_n + ins->cos_ang_z * ins->pos_bis_e;
	
	ins->acc_int_e += pos_bod_err_x * ins->coe_acc_xy * ins->ins_dt;
	ins->acc_int_n += pos_bod_err_y * ins->coe_acc_xy * ins->ins_dt;
	
	float acc_now_cor_x = ins->cos_ang_z * ins->acc_int_e - ins->sin_ang_z * ins->acc_int_n;
	float acc_now_cor_y = ins->sin_ang_z * ins->acc_int_e + ins->cos_ang_z * ins->acc_int_n;
	
	ins->acc_udp_e = ins->acc_now_e + acc_now_cor_x; 
	ins->acc_udp_n = ins->acc_now_n + acc_now_cor_y;

	ins->vel_now_e += ins->pos_bis_e * ins->coe_vel_xy * ins->ins_dt;
	ins->vel_now_n += ins->pos_bis_n * ins->coe_vel_xy * ins->ins_dt;
	
	ins->pos_now_e += ins->pos_bis_e * ins->coe_pos_xy * ins->ins_dt;
	ins->pos_now_n += ins->pos_bis_n * ins->coe_pos_xy * ins->ins_dt;

	ins->pos_now_e += ins->vel_now_e * ins->ins_dt + 0.5f * ins->acc_udp_e * ins->ins_dt * ins->ins_dt;
	ins->pos_now_n += ins->vel_now_n * ins->ins_dt + 0.5f * ins->acc_udp_n * ins->ins_dt * ins->ins_dt;
	
	ins->vel_now_e += ins->acc_udp_e * ins->ins_dt;
	ins->vel_now_n += ins->acc_udp_n * ins->ins_dt;

	ins->acc_int_gp += ins->pos_bis_gp * ins->coe_acc_z * ins->ins_dt;
	ins->acc_udp_gp = (g + ins->acc_now_u) + ins->acc_int_gp;
	ins->vel_now_gp += ins->pos_bis_gp * ins->coe_vel_z * ins->ins_dt;
	ins->pos_now_gp += ins->pos_bis_gp * ins->coe_pos_z * ins->ins_dt;
	ins->pos_now_gp += ins->vel_now_gp * ins->ins_dt + 0.5f * ins->acc_udp_gp * ins->ins_dt * ins->ins_dt;
	ins->vel_now_gp += ins->acc_udp_gp * ins->ins_dt;	
}

void vel_xy_update(void)
{
	arm_sqrt_f32(ins->vel_now_e * ins->vel_now_e + ins->vel_now_n * ins->vel_now_n, &(ins->vel_now_l));
}

static void alt_use_gps_update(void)
{
//	if(hal_tic_ms() - lrt > 25)
//	{
//		ins->alt_gps_err = ins->pos_now_gp - ins->pos_now_u;
//	}
//	lrt = hal_tic_ms();
	
	ins->vel_now_u = ins->vel_now_gp;
	ins->pos_now_u = ins->pos_now_gp - ins->alt_gps_err;
}

static void ins_run(void)
{
  //******* ATTITUDE *******//
  calc_quat();
#ifndef SIMU
//	calc_quat_new();
//	calc_quat_free();
  calc_angle();
#endif
	calc_dcm();
	init_quat();
	if(imu_class.inited)
	{
//			state_updata();
	}
  //******* POSITIONM *******//
  nav_acc_update();
	nav_cor_update();
  
#ifndef SIMU
	gps_err_update();
	gps_cal_update();
//	vel_xy_update();
#endif	
  
//	mw_err_update();
//	mw_coe_update();
//	mw_alt_update();

#ifndef SIMU 
//  mw_cal_update();
//  
//	if(ins->mw_sts && ins->pos_now_mw < 20.0f)
//	{
//		alt_use_mw_update();
//	}
//	else
//	{
		alt_use_gps_update();
//	}
#endif
}

void ins_init(struct ins_class_s* _ins)
{
  ins = _ins;
  
  ins->qw = 1.0f;
  ins->qx = 0.0f;
  ins->qy = 0.0f;
  ins->qz = 0.0f;
  ins->acc_kp = 0.2f;
  ins->acc_ki = 1e-2f;
  ins->nav_cor_c = 1.0f;
  ins->gps_old_cnt = GPS_OLD_CNT;
  ins->ins_dt = 1.0f / 1000;
  ins->ins_ht = 0.5f / 1000;
  ins->mag_kp = 0.2f;
  ins->mag_ki = 1e-2f;
  ins->inited = false;
  
  ins->time_cons_xy = 1.0f;
	ins->coe_pos_xy = 3.0f / (ins->time_cons_xy);
	ins->coe_vel_xy = 3.0f / (ins->time_cons_xy * ins->time_cons_xy);
	ins->coe_acc_xy = 1.0f / (ins->time_cons_xy * ins->time_cons_xy * ins->time_cons_xy);
  ins->time_cons_z = 1.0f;
	ins->coe_pos_z = 3.0f / (ins->time_cons_z);
	ins->coe_vel_z = 3.0f / (ins->time_cons_z * ins->time_cons_z);
	ins->coe_acc_z = 1.0f / (ins->time_cons_z * ins->time_cons_z * ins->time_cons_z);
  ins->run = &ins_run;
	ins->initing = true;
}

