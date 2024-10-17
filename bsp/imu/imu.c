#include "imu.h" 
#include "stdbool.h"
#include "fmath.h"
#include "math.h"
#include "lpf.h"
#include "icm20689.h" 
static struct imu_class_s* imu;
extern uint32_t timer_0A;
static void imu_rec_update(void)
{
	imu->rec_dt = timer_0A - imu->rec_tic;
	imu->rec_tic = timer_0A;
}
void imu_dat_chk(void)
{
	if(imu->adc[ 0] == imu->last_adc[ 0]
	&& imu->adc[ 1] == imu->last_adc[ 1]
	&& imu->adc[ 2] == imu->last_adc[ 2]
	&& imu->adc[ 3] == imu->last_adc[ 3]
	&& imu->adc[ 4] == imu->last_adc[ 4]
	&& imu->adc[ 5] == imu->last_adc[ 5]
	&& imu->adc[ 6] == imu->last_adc[ 6]
	&& imu->adc[ 7] == imu->last_adc[ 7]
	&& imu->adc[ 8] == imu->last_adc[ 8]
	&& imu->adc[ 9] == imu->last_adc[ 9]
	&& imu->adc[10] == imu->last_adc[10]
	&& imu->adc[11] == imu->last_adc[11]
	&& imu->adc[12] == imu->last_adc[12]
	&& imu->adc[13] == imu->last_adc[13]
	&& imu->adc[14] == imu->last_adc[14])
	{
		imu->repeat_cnt++;
	}
	else
	{
		imu->last_adc[ 0] = imu->adc[ 0];
		imu->last_adc[ 1] = imu->adc[ 1];
		imu->last_adc[ 2] = imu->adc[ 2];
		imu->last_adc[ 3] = imu->adc[ 3];
		imu->last_adc[ 4] = imu->adc[ 4];
		imu->last_adc[ 5] = imu->adc[ 5];
		imu->last_adc[ 6] = imu->adc[ 6];
		imu->last_adc[ 7] = imu->adc[ 7];
		imu->last_adc[ 8] = imu->adc[ 8];
		imu->last_adc[ 9] = imu->adc[ 9];
		imu->last_adc[10] = imu->adc[10];
		imu->last_adc[11] = imu->adc[11];
		imu->last_adc[12] = imu->adc[12];
		imu->last_adc[13] = imu->adc[13];
		imu->last_adc[14] = imu->adc[14];
		imu->repeat_cnt = 0;
	}
	
	if(fabsf(imu->ax_ori) > 50.0f 
	|| fabsf(imu->ay_ori) > 50.0f
	|| fabsf(imu->az_ori) > 50.0f
	|| fabsf(imu->gx_ori) >  6.0f
	|| fabsf(imu->gy_ori) >  6.0f
	|| fabsf(imu->gz_ori) >  6.0f)
	{
		imu->range_cnt++;
	}
	else
	{
		imu->range_cnt = 0;
	}
	
	if(imu->repeat_cnt > 100)
	{
		imu->rec_err = SENSOR_DATA_ERR_NULL;
	}
	else if(imu->range_cnt > 100)
	{
		imu->rec_err = SENSOR_DATA_ERR_RANGE;
	}
	else
	{
		imu->rec_err = SENSOR_DATA_ERR_OK;
	}
}

void imu_acc_lpf(void)
{
	imu->ax_lpf = lpf_calc(lpf_ax, imu->ax_ori);
	imu->ay_lpf = lpf_calc(lpf_ay, imu->ay_ori);
	imu->az_lpf = lpf_calc(lpf_az, imu->az_ori);
}

void imu_gyr_lpf(void)
{
	imu->gx_lpf = lpf_calc(lpf_gx, imu->gx_ori);
	imu->gy_lpf = lpf_calc(lpf_gy, imu->gy_ori);
	imu->gz_lpf = lpf_calc(lpf_gz, imu->gz_ori);
}
//extern uint32_t timer_0B;
void imu_bis_cal(void)
{
	if(timer_0A<1000 && !imu->inited) 
	{
		imu->ax_bis = imu->ax_lpf;
		imu->ay_bis = imu->ay_lpf;
		if(imu->az_lpf>0)
		{
			imu->az_bis = imu->az_lpf - 1;
		}
		else
		{
			imu->az_bis = imu->az_lpf + 1;
		}
//		imu->ax_bis = 0;
//		imu->ay_bis = 0;
//		imu->az_bis = 0;
		
		imu->gx_bis = imu->gx_lpf;
		imu->gy_bis = imu->gy_lpf;
		imu->gz_bis = imu->gz_lpf;
		
		imu->ax = 0;
		imu->ay = 0;
		imu->az = 1;
		
		imu->gx = 0;
		imu->gy = 0;
		imu->gz = 0;
	}
	else
	{
		imu->ax = imu->ax_lpf - imu->ax_bis;
		imu->ay = imu->ay_lpf - imu->ay_bis;
		imu->az = imu->az_lpf - imu->az_bis;
		
		imu->ax_ori = imu->ax_ori - imu->ax_bis;
		imu->ay_ori = imu->ay_ori - imu->ay_bis;
		imu->az_ori = imu->az_ori - imu->az_bis;
		
		imu->gx = imu->gx_lpf - imu->gx_bis;
		imu->gy = imu->gy_lpf - imu->gy_bis;
		imu->gz = imu->gz_lpf - imu->gz_bis;
		imu->inited = true;
	}
}

void imu_dgy_cal(void)
{
	imu->dgx_ori = imu->read_freq * (imu->gx_ori - imu->gx_lat);
	imu->dgy_ori = imu->read_freq * (imu->gy_ori - imu->gy_lat);
	imu->dgz_ori = imu->read_freq * (imu->gz_ori - imu->gz_lat);
	imu->gx_lat = imu->gx_ori;
	imu->gy_lat = imu->gy_ori;
	imu->gz_lat = imu->gz_ori;
}

void imu_dgy_lpf(void)
{
	imu->dgx = lpf_calc(lpf_dx, imu->dgx_ori);
	imu->dgy = lpf_calc(lpf_dy, imu->dgy_ori);
	imu->dgz = lpf_calc(lpf_dz, imu->dgz_ori);
}

void imu_run(void)
{
#ifndef SIMU
	icm20689_read(&(imu->ax_ori), &(imu->ay_ori), &(imu->az_ori), &(imu->gx_ori), &(imu->gy_ori), &(imu->gz_ori));
#else
	float ax, ay, az, gx, gy, gz;
	hal_imu_read(&ax, &ay, &az, &gx, &gy, &gz);
#endif

  icm20689_get_adc(imu->adc);
  
  imu_dat_chk();
  
  imu_acc_lpf();
  
  imu_gyr_lpf();
  
  imu_bis_cal();
  
  imu_dgy_cal();
  
  imu_dgy_lpf();
	imu_rec_update();
}

void imu_init(struct imu_class_s* _imu)
{
  imu             = _imu;
  imu->inited     = false;
  imu->read_freq  = 1000;
	
	imu->run        = &imu_run;
}
