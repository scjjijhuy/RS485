#include "fmath.h"
#include "arm_math.h"
#define lat_cof		0.0111177521f

// pow	113.3us		1888
// sqrt	11.10us		185
// /		0.137us		2
// *		0.060us
// fabs	0.452us

// fisqrt	0.208us
// arm_sqrt_f32		0.190us
// arm_cos_f32		0.756us
// arm_sin_f32		0.756us
// cos						20.8 us
// sin						17.6 us
// atan2					42.8 us
// asin						33.1 us

float wrap(float value, float min, float max)	// min < value < max
{
  float tmp = value;
  uint8_t cnt = 0;
  if(min > max)
    return tmp;
  while(tmp >= max)
	{
		tmp -= (max - min);
    if(cnt++ > 100)
      return tmp;
	}
	while(tmp < min)
	{
		tmp += (max - min);
    if(cnt++ > 100)
      return tmp;
	}
	return tmp;
}

float cons(float value, float min, float max)	// min < value < max
{
	if(value > max)
	{
		return max;
	}
	else if(value < min)
	{
		return min;
	}
	else
	{
		return value;
	}
}

float map_float(float x, float in_min, float in_max, float out_min, float out_max)
{
	if(x < in_min){ return out_min; }
	if(x > in_max){ return out_max; }
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

short msb_uchar_to_short(uint8_t* buff, uint i)
{
	ushort value = (ushort)((buff[i]<<8) | buff[i+1]);
	return *((short*)(&value));
}

ushort lsb_uchar_to_ushort(uint8_t* buff, uint i)
{
	return (ushort)((buff[i]) | buff[i+1]<<8);
}

uint32_t lsb_uchar_to_uint(uint8_t* buff, uint i)
{
	return (uint32_t)(buff[i] | (buff[i+1]<<8) | (buff[i+2]<<16) | (buff[i+3]<<24));
}

short lsb_uchar_to_short(uint8_t* buff, uint i)
{
	ushort value = (ushort)(buff[i] | (buff[i+1]<<8));
	return *((short*)(&value));
}

int lsb_uchar_to_int(uint8_t* buff, uint i)
{
	uint32_t value = (uint32_t)(buff[i] | (buff[i+1]<<8) | (buff[i+2]<<16) | (buff[i+3]<<24));
	return *((int*)(&value));
}

float lsb_uchar_to_float(uint8_t* buff, uint i)
{
	uint value = (uint)(buff[i] | (buff[i+1]<<8) | (buff[i+2]<<16) | (buff[i+3]<<24));
	return *((float*)(&value));
}

void lsb_ushort_to_uchar(ushort value, uchar * buff, uint i)
{
	buff[i++] = (uchar)(value);
	buff[i++] = (uchar)(value >> 8);
}

void lsb_uint_to_uchar(uint value, uchar * buff, uint i)
{
	buff[i++] = (uchar)(value);
	buff[i++] = (uchar)(value >> 8);
	buff[i++] = (uchar)(value >> 16);
	buff[i++] = (uchar)(value >> 24);
}

void lsb_int_to_uchar(int value, uchar * buff, uint i)
{
	uint tmp = *((uint*)(&value));
	buff[i++] = (uchar)(tmp);
	buff[i++] = (uchar)(tmp >> 8);
	buff[i++] = (uchar)(tmp >> 16);
	buff[i++] = (uchar)(tmp >> 24);
}

void lsb_float_to_uchar(float value, uchar * buff, uint i)
{
	uint tmp = *((uint*)(&value));
	buff[i++] = (uchar)(tmp);
	buff[i++] = (uchar)(tmp >> 8);
	buff[i++] = (uchar)(tmp >> 16);
	buff[i++] = (uchar)(tmp >> 24);
}

float fisqrt(float x)
{
	 float y = x;
	 long i = *(long*)&y;
	 i = 0x5f3759df - (i >> 1);
	 y = *(float*)&i;
	 y = y * (1.5f - 0.5f * x * y * y);
	 return y;
}

float fpow(float v, uchar n)
{
	float r = v;
	for(uchar i = 1; i < n; i++)
	{
		r *= v;
	}
	return r;
}

float interf(const float * src, const float * dst, const uint cnt, float x)
{
	if(x <= src[0])
	{
		return dst[0];
	}

	if(x >= src[cnt - 1])
	{
		return dst[cnt - 1];
	}

	for(uint i = 1; i < cnt; i++)
	{
		if(x < src[i])
		{
			return dst[i] - (dst[i] - dst[i-1]) / (src[i] - src[i-1]) * (src[i] - x);
		}
	}
	
	return 0;
}

float max(float x, float y)
{
	return x > y ? x : y;
}

float min(float x, float y)
{
	return x < y ? x : y;
}

void nor_two(float *x, float *y)
{
  float nor = 1.0f;
	arm_sqrt_f32(*x * *x + *y * *y, &nor);
	if(fabsf(nor) < eps)
	{
		*x = 0.0f; *y = 0.0f;
	}
	else
	{
		*x /= nor; *y /= nor;
	}
}

void nor_thr(float *x, float *y, float *z)
{
	float nor = 1.0f;
	arm_sqrt_f32(*x * *x + *y * *y + *z * *z, &nor);
	if(fabsf(nor) < eps)
	{
		*x = 0.0f; *y = 0.0f; *z = 0.0f;
	}
	else
	{
		*x /= nor; *y /= nor; *z /= nor;
	}
}

void nor_for(float *w, float *x, float *y, float *z)
{
	float nor = 1.0f;
	arm_sqrt_f32(*w * *w + *x * *x + *y * *y + *z * *z, &nor);
	if(fabsf(nor) < eps)
	{
		*w = 1.0f; *x = 0.0f; *y = 0.0f; *z = 0.0f;
	}
	else
	{
		*w /= nor; *x /= nor; *y /= nor; *z /= nor;
	}
}

int abs_int(int val)
{
	if(val < 0)
		return -val;
	return val;
}
/***********************************************************************************/

void vector2f_new(Vector2f* v, float x, float y)
{
	v->x = x;
	v->y = y;
}

// 东北天方向（东向为0，逆时针方向，0~2pi）
void vector2f_rotate(Vector2f* v, float agl)
{
	float x = v->x;
	float y = v->y;
	float sina = sinf(agl);
	float cosa = cosf(agl);
	v->x = x*cosa - y*sina;
	v->y = x*sina + y*cosa;
}

/*
 *函数名：math_vector2f_norm
 *功能：用于求二维矢量的模
 *输入：Vector2f类型数据
 *输出: float类型矢量模
 *时间：2018年4月9日
 */
float vector2f_norm(Vector2f mag)
{
	float mod;
	arm_sqrt_f32(mag.x*mag.x + mag.y*mag.y,&mod);
  return mod;
}

/*
 *函数名：math_vector2f_sat
 *功能：二维矢量饱和函数
 *输入：Vector2f类型数据，max允许的最大的模
 *输出: Vector2f类型缩比后矢量
 *时间：2018年6月16日
 */
Vector2f vector2f_sat(Vector2f mag,float max)
{
	float mod = 1;
  arm_sqrt_f32(mag.x*mag.x  + mag.y*mag.y,&mod);  //求输入矢量模
	
	if(mod > max)
	{   
		 //限制分母大小
		 if(mod < 1e-5f)
		 {
			 mod = 1e-5f;
		 }
		 
		 mod = max/mod;
		 mag.x = mag.x * mod;    //缩比
		 mag.y = mag.y * mod;
		return mag;
	}
	else
	{
		return mag;
	}
}

/*
 *函数名：math_vector2f_dot
 *功能：用于计算两个二维矢量的点乘运算
 *输入：Vector2f类型数据的两个矢量
 *输出: float类型输出
 *时间：2018年4月9日
 */
float vector2f_dot(Vector2f ab,Vector2f bc)
{
   return ab.x*bc.x + ab.y*bc.y;
}

/*
 *函数名：math_vector2f_scale
 *功能：用于将向量变为方向一致模为设置值德向量
 *输入：Vector2f类型数据的向量，float类型模大小
 *输出: Vector2f类型输出
 *时间：2018年8月21日
 */
Vector2f vector2f_scale(Vector2f vector,float mod)
{
	Vector2f output;
	
	//求向量模
	float norm;
	arm_sqrt_f32(vector.x*vector.x + vector.y*vector.y,&norm);
	
	if(norm < (float)1e-5)
	{
		norm = 1e-5;
	}
	
	output.x = vector.x / norm *mod;
	output.y = vector.y / norm *mod;
	
	return output;
}

/*
 *函数名：math_vector2f_sub
 *功能：计算二维向量相减,结果A-B
 *输入：Vector2f类型数据的向量，float类型模大小
 *输出: Vector2f类型输出
 *时间：2018年8月21日
 */
Vector2f vector2f_sub(Vector2f a,Vector2f b)
{
	Vector2f output;
	
	output.x = a.x - b.x;
	output.y = a.y - b.y;
	
	return output;
}

/*
 *函数名：math_dem_is_zero
 *功能：判断分母数据是否为零，如果小于eps
 *输入：float类型分母数据
 *输出: float类型分母数据
 *时间：2018年10月30日
 */
float math_dem_is_zero(float data)
{
	if(data >= 0 && data <1e-5f)
	{
		return 1e-5f;
	}
	else if(data < 0 && data > -1e-5f)
	{
		return -1e-5f;
	}
	else
	{
	  return data;
	}
}

void from_euler_to_dcm_bf_to_ef(float rol, float pit, float yaw, float* R)
{
	float cr = cosf(rol);
  float cp = cosf(pit);
  float cy = cosf(yaw);
  float sr = sinf(rol);
  float sp = sinf(pit);
  float sy = sinf(yaw);
	R[0] = cp*cy;
	R[1] = sr*sp*cy-cr*sy;
	R[2] = cr*sp*cy + sr*sy;
	R[3] = cp*sy;
	R[4] = sr*sp*sy + cr*cy;
	R[5] = cr*sp*sy - sr*cy;
	R[6] = -sp;
	R[7] = sr*cp;
	R[8] = cr*cp;
}

/************************ WGS84 *********************************/
int32_t get_lon_from_dp(int32_t lon, int32_t lat, float dp)
{
	float lon_cof = lat_cof * arm_cos_f32(lat * 1e-7f * deg_to_rad);
	int32_t ret = wrap(dp / lon_cof + lon, -1.8e9f, 1.8e9f);
	return ret;
}

int32_t get_lat_from_dp(int32_t lat, float dp)
{
	int32_t ret = dp / lat_cof + lat;
	return ret;
}

float get_dp_from_lon(int32_t lon1, int32_t lat1, int32_t lon2, int32_t lat2)
{
	float lon_cof = lat_cof * arm_cos_f32(lat1 * 1e-7f * deg_to_rad);
	return wrap(lon1 - lon2, -1.8e9f, 1.8e9f) * lon_cof;
}

float get_dp_from_lat(int32_t lon1, int32_t lat1, int32_t lon2, int32_t lat2)
{
	return (lat1 - lat2) * lat_cof;
}

//********************** vector3f *********************
void vector3f_new(Vector3f* v, float x, float y, float z)
{
	v->x = x;
	v->y = y;
	v->z = z;
}

void vector3f_zero(Vector3f* v)
{
	v->x = 0;
	v->y = 0;
	v->z = 0;
}

void vector3f_multiply(Vector3f* v, float coe, Vector3f* out)
{
	out->x = v->x * coe;
	out->y = v->y * coe;
	out->z = v->z * coe;
}

void vector3f_add(Vector3f* v1, Vector3f* v2, Vector3f* out)
{
	out->x = v1->x + v2->x;
	out->y = v1->y + v2->y;
	out->z = v1->z + v2->z;
}

void vector3f_subtract(Vector3f* v1, Vector3f* v2, Vector3f* out)
{
	out->x = v1->x - v2->x;
	out->y = v1->y - v2->y;
	out->z = v1->z - v2->z;
}

float vector3f_dot(Vector3f v1, Vector3f v2)
{
	float ret;
	ret = v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
	return ret;
}

void vector3f_cross(Vector3f v1, Vector3f v2, Vector3f* out)
{
	out->x = v1.y*v2.z - v1.z*v2.y;
	out->y = v1.z*v2.x - v1.x*v2.z;
	out->z = v1.x*v2.y - v1.y*v2.x;
}

float vector3f_mag(Vector3f v)
{
	float mod;
	arm_sqrt_f32(v.x*v.x + v.y*v.y + v.z*v.z, &mod);
  return mod;
}

void vector3f_norm(Vector3f* v)
{
	float val;
	arm_sqrt_f32(v->x*v->x + v->y*v->y + v->z*v->z, &val);
	if(val < (float)1e-5)
		val = 1e-5;
	v->x /= val;
	v->y /= val;
	v->z /= val;
}

//********************** quat *********************
void quat_init(Quaternion* q)
{
	q->q0 = 1;
	q->q1 = 0;
	q->q2 = 0;
	q->q3 = 0;
}

void quat_update(Quaternion* q, float gx, float gy, float gz, float dt)
{
	q->q0 = q->q0 + (-q->q1 * gx - q->q2 * gy - q->q3 * gz) * dt * 0.5f;
	q->q1 = q->q1 + (q->q0 * gx + q->q2 * gz - q->q3 * gy) * dt * 0.5f;
	q->q2 = q->q2 + (q->q0 * gy - q->q1 * gz + q->q3 * gx) * dt * 0.5f;
	q->q3 = q->q3 + (q->q0 * gz + q->q1 * gy - q->q2 * gx) * dt * 0.5f;
	
	float norm = sqrt(q->q0 * q->q0 + q->q1 * q->q1 + q->q2 * q->q2 + q->q3 * q->q3);
	q->q0 = q->q0 / norm;
	q->q1 = q->q1 / norm;
	q->q2 = q->q2 / norm;
	q->q3 = q->q3 / norm;
}

void get_eular_angle_from_quat(Quaternion* q, float* roll, float* pitch, float* yaw)
{
	*roll = atan2f(2.0f * (q->q0 * q->q1 + q->q2 * q->q3), 1 - 2.0f * (q->q1 * q->q1 + q->q2 * q->q2));

	*pitch = asin(2.0f * (q->q0 * q->q2 - q->q1 * q->q3));

	*yaw = atan2(2 * (q->q1 * q->q2 + q->q0 * q->q3), q->q0 * q->q0 + q->q1 * q->q1 - q->q2 * q->q2 - q->q3 * q->q3);
	if (*yaw < 0)
		*yaw += twopi;
}

void quat_to_dcm(Quaternion* q, float* dcm)
{
	float q0q0 = q->q0 * q->q0;
	float q0q1 = q->q0 * q->q1;
	float q0q2 = q->q0 * q->q2;
	float q0q3 = q->q0 * q->q3;
	float q1q1 = q->q1 * q->q1;
	float q1q2 = q->q1 * q->q2;
	float q1q3 = q->q1 * q->q3;
	float q2q2 = q->q2 * q->q2;
	float q2q3 = q->q2 * q->q3;
	float q3q3 = q->q3 * q->q3;

	// Convert q->R, This R converts body frame to inertial frame.
	dcm[0] = q0q0 + q1q1 - q2q2 - q3q3;
	dcm[3] = 2.0f * (q1q2 + q0q3);
	dcm[6] = 2.0f * (q1q3 - q0q2);
	dcm[1] = 2.0f * (q1q2 - q0q3);
	dcm[4] = q0q0 - q1q1 + q2q2 - q3q3;
	dcm[7] = 2.0f * (q2q3 + q0q1);
	dcm[2] = 2.0f * (q1q3 + q0q2);
	dcm[5] = 2.0f * (q2q3 - q0q1);
	dcm[8] = q0q0 - q1q1 - q2q2 + q3q3;
}

void from_body_to_ned(float* dcm, Vector3f* body, Vector3f* ned)
{
	float abody[3] = { body->x, body->y, body->z };
	ned->x = ned->y = ned->z = 0;
	for (int i = 0; i < 3; i++)
	{
		ned->x += dcm[i] * abody[i];
		ned->y += dcm[i+3] * abody[i];
		ned->z += dcm[i+6] * abody[i];
	}
}

void from_ned_to_body(float* dcm, Vector3f* ned, Vector3f* body)
{
	float aned[3] = { ned->x, ned->y, ned->z };
	body->x = body->y = body->z = 0;
	for (int i = 0; i < 3; i++)
	{
		body->x += dcm[i*3] * aned[i];
		body->y += dcm[i*3+1] * aned[i];
		body->z += dcm[i*3+2] * aned[i];
	}
}

void reset_quat_by_euler_angle(Quaternion* q, float roll, float pitch, float yaw)
{
	float cr2 = cosf(roll * 0.5f);
	float cp2 = cosf(pitch * 0.5f);
	float cy2 = cosf(yaw * 0.5f);
	float sr2 = sinf(roll * 0.5f);
	float sp2 = sinf(pitch * 0.5f);
	float sy2 = sinf(yaw * 0.5f);

	q->q0 = cr2 * cp2 * cy2 + sr2 * sp2 * sy2;
	q->q1 = sr2 * cp2 * cy2 - cr2 * sp2 * sy2;
	q->q2 = cr2 * sp2 * cy2 + sr2 * cp2 * sy2;
	q->q3 = cr2 * cp2 * sy2 - sr2 * sp2 * cy2;
}
