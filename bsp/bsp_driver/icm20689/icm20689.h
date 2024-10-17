#ifndef _ICM20689_H
#define _ICM20689_H

#include "hc32_ll.h"

void icm20689_init(void);
void icm20689_read(float* ax, float* ay, float* az, float* gx, float* gy, float* gz);
void icm20689_get_adc(uint8_t* adc);
#endif
