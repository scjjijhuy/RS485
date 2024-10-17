#ifndef _LPF_H
#define _LPF_H
#include "hc32_ll.h"
enum lpf_mask
{
	lpf_ax 				=		0x00,
	lpf_ay,
	lpf_az,
	
	lpf_gx,
	lpf_gy,
	lpf_gz,
	
	lpf_dx,
	lpf_dy,
	lpf_dz,

	lpf_dgy_tar_x,
	lpf_dgy_tar_y,
	lpf_dgy_tar_z,	

	lpf_mx,
	lpf_my,
	lpf_mz,
	
	lpf_ang_tar_x,
	lpf_ang_tar_y,
	lpf_ang_err_z,
	
	lpf_bus_thr,
	lpf_bus_rol,
	lpf_bus_pit,
	lpf_bus_yaw,
	
	lpf_thr_cnt,
};

float lpf_calc(uint8_t mask, float v);

#endif
