#pragma once

#include <stdint.h>

#define BMP280_S32_t int32_t

double bmp280_compensate_T_double(BMP280_S32_t adc_T);
double bmp280_compensate_P_double(BMP280_S32_t adc_P);
