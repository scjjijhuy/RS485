#ifndef _IMU_H
#define _IMU_H
#include "hc32_ll.h"
#include "main.h"
enum SENSOR_DATA_ERR
{
	SENSOR_DATA_ERR_OK = 0,											
	SENSOR_DATA_ERR_NULL,		
  SENSOR_DATA_ERR_CNT,	  
	SENSOR_DATA_ERR_SAME,
  SENSOR_DATA_ERR_CHK,
	SENSOR_DATA_ERR_RANGE,
  SENSOR_DATA_ERR_CREDIT,
  SENSOR_DATA_ERR_VAR
};
struct imu_class_s
{
  uint16_t  read_freq;
  
	float		  ax_ori;
	float		  ax_lpf;
	float		  ax_bis;
	float		  ax;
	
	float		  ay_ori;
	float		  ay_lpf;
	float		  ay_bis;
	float		  ay;
	
	float		  az_ori;
	float		  az_lpf;
	float		  az_bis;
	float		  az;
	
	float		  gx_ori;
	float		  gx_lpf;
	float		  gx_bis;
	float		  gx;
	float		  gx_lat;
	
	float		  gy_ori;
	float		  gy_lpf;
	float		  gy_bis;
	float		  gy;
	float		  gy_lat;
	
	float		  gz_ori;
	float		  gz_lpf;
	float		  gz_bis;
	float		  gz;
	float		  gz_lat;
	
	float		  dgx_ori;
	float		  dgx;
	
	float		  dgy_ori;
	float		  dgy;
	
	float		  dgz_ori;
	float		  dgz;
  
  uint8_t   adc[15];
  uint8_t   last_adc[15];
  
  uint32_t  repeat_cnt;
  uint32_t  range_cnt;
  uint32_t  rec_err;
  
	uint32_t  rec_dt;
	uint32_t	rec_tic;
  bool      inited;
	
	//***** function ******
	void 			(*run) (void);
};
extern struct imu_class_s imu_class;
void imu_init(struct imu_class_s* _imu);

#endif
