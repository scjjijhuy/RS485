//*********************************************************************************
//LHK 20220420  数据处理
//*********************************************************************************	

/* Includes ------------------------------------------------------------------*/
#include "data_buf.h"
#include "imu.h"
#include "ins.h"
#include "math.h"
#include "fmath.h"
#include "timer.h"
struct data_buf_class_s* data_buf;
struct imu_data_class imu_data;
struct imu_threshold_class imu_threshold = 
{
	.threshold_ax = 1.0,
	.threshold_ay = 1.0,
	.threshold_az = 1.0,
	.Vibration_coefficient = 75,
};
void data_buf_run(void)
{

	imu_data.f_ax = (imu_class.ax_ori - ins_class.sin_ang_y);//*100/imu_threshold.Vibration_coefficient; 
	imu_data.f_ay = (imu_class.ay_ori - ins_class.sin_ang_x);//*100/imu_threshold.Vibration_coefficient; 
	if(fabs(ins_class.ang_x) < halfpi)
	{
		imu_data.f_az = (imu_class.az_ori - sqrt(1-(ins_class.sin_ang_y*ins_class.sin_ang_y + ins_class.sin_ang_x*ins_class.sin_ang_x)));//*100/imu_threshold.Vibration_coefficient;		
	}
	else
	{
		imu_data.f_az = (imu_class.az_ori + sqrt(1-(ins_class.sin_ang_y*ins_class.sin_ang_y + ins_class.sin_ang_x*ins_class.sin_ang_x)));//*100/imu_threshold.Vibration_coefficient;	
	}
    
	imu_data.sign = 1;
}
void data_buf_init(struct data_buf_class_s* _data_buf)
{
	data_buf = _data_buf;

	
	data_buf->buf_num = 0;
	data_buf->max_num = 0;
	data_buf->run = &data_buf_run;
}
/* USER CODE END 1 */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
