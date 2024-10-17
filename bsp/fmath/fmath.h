#ifndef _FMATH_H
#define _FMATH_H
#include "hc32_ll.h"

typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned long  ulong;
#define	g								9.8035f
#define	g_inv						0.1020f
#define	eps							1.0e-5f

#define twopi						6.2831853072f
#define pi 							3.1415926536f
#define halfpi					1.5707963268f

#define deg_to_rad			0.0174532925f
#define rad_to_deg			57.295779513f

float		wrap(float value, float min, float max);
float		cons(float value, float min, float max);

float		map_float(float x, float in_min, float in_max, float out_min, float out_max);

short		msb_uchar_to_short(uchar * buff, uint i);

ushort	lsb_uchar_to_ushort(uchar * buff, uint i);
uint		lsb_uchar_to_uint(uchar * buff, uint i);
short		lsb_uchar_to_short(uchar * buff, uint i);
int			lsb_uchar_to_int(uchar * buff, uint i);
float		lsb_uchar_to_float(uchar * buff, uint i);

void		lsb_ushort_to_uchar(ushort value, uchar * buff, uint i);
void		lsb_uint_to_uchar(uint value, uchar * buff, uint i);
void		lsb_int_to_uchar(int value, uchar * buff, uint i);
void		lsb_float_to_uchar(float value, uchar * buff, uint i);

float		fisqrt(float x);
float		fpow(float v, uchar n);

float		interf(const float * src, const float * dst, const uint cnt, float x);

float   max(float x, float y);
float   min(float x, float y);

void    nor_two(float *x, float *y);
void    nor_thr(float *x, float *y, float *z);
void    nor_for(float *w, float *x, float *y, float *z);

int   	abs_int(int val);

void    from_euler_to_dcm_bf_to_ef(float rol, float pit, float yaw, float* R);

/************************ WGS84 *********************************/
int32_t get_lon_from_dp(int32_t lon, int32_t lat, float dp);

int32_t get_lat_from_dp(int32_t lat, float dp);

float 	get_dp_from_lon(int32_t lon1, int32_t lat1, int32_t lon2, int32_t lat2);

float 	get_dp_from_lat(int32_t lon1, int32_t lat1, int32_t lon2, int32_t lat2);

/************************ Vector2 *********************************/
typedef struct 
{
	float x;
	float y;
}Vector2f;

void vector2f_new(Vector2f* v, float x, float y);

void vector2f_rotate(Vector2f* v, float agl);

float vector2f_norm(Vector2f mag);  

Vector2f vector2f_sat(Vector2f mag,float max); 

float vector2f_dot(Vector2f ab,Vector2f bc); 

Vector2f vector2f_scale(Vector2f vector,float mod);  

Vector2f vector2f_sub(Vector2f a,Vector2f b); 

float math_dem_is_zero(float data);

/************************Vector3**********************************/
typedef struct
{
	float x;
	float y;
	float z;
} Vector3f;

void vector3f_new(Vector3f* v, float x, float y, float z);
void vector3f_zero(Vector3f* v);
void vector3f_multiply(Vector3f* v, float coe, Vector3f* out);
void vector3f_add(Vector3f* v1, Vector3f* v2, Vector3f* out);
void vector3f_subtract(Vector3f* v1, Vector3f* v2, Vector3f* out);
float vector3f_dot(Vector3f v1, Vector3f v2);
void vector3f_cross(Vector3f v1, Vector3f v2, Vector3f* out);
float vector3f_mag(Vector3f v);
void vector3f_norm(Vector3f* v);

/***********************Quaternion****************************/
typedef struct
{
	float q0;
	float q1;
	float q2;
	float q3;
} Quaternion;

void quat_init(Quaternion* q);
void quat_update(Quaternion* q, float gx, float gy, float gz, float dt);
void get_eular_angle_from_quat(Quaternion* q, float* roll, float* pitch, float* yaw);
void quat_to_dcm(Quaternion* q, float* dcm);
void from_body_to_ned(float* dcm, Vector3f* body, Vector3f* ned);
void from_ned_to_body(float* dcm, Vector3f* ned, Vector3f* body);
void reset_quat_by_euler_angle(Quaternion* q, float roll, float pitch, float yaw);

#endif
