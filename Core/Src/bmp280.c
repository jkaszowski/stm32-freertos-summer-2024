#include "bmp280.h"

#define BMP280_S32_t int32_t
#define BMP280_U32_t uint32_t
#define BMP280_ADDR  (0x76 << 1)

// Returns temperature in DegC, double precision. Output value of “51.23” equals 51.23 DegC.
// t_fine carries fine temperature as global value
BMP280_S32_t   t_fine;
unsigned short dig_T1 = 27504;
short          dig_T2 = 26435;
short          dig_T3 = -1000;
double         bmp280_compensate_T_double(BMP280_S32_t adc_T)
{
  double var1, var2, T;
  var1 = (((double)adc_T) / 16384.0 - ((double)dig_T1) / 1024.0) * ((double)dig_T2);
  var2 = ((((double)adc_T) / 131072.0 - ((double)dig_T1) / 8192.0) *
          (((double)adc_T) / 131072.0 - ((double)dig_T1) / 8192.0)) *
         ((double)dig_T3);
  t_fine = (BMP280_S32_t)(var1 + var2);
  T      = (var1 + var2) / 5120.0;
  return T;
}

// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
// t_fine carries fine temperature as global value
inline BMP280_S32_t bmp280_compensate_T_int32(BMP280_S32_t adc_T)
{
  BMP280_S32_t var1, var2, T;
  var1 = ((((adc_T >> 3) - ((BMP280_S32_t)dig_T1 << 1))) * ((BMP280_S32_t)dig_T2)) >> 11;
  var2 =
      (((((adc_T >> 4) - ((BMP280_S32_t)dig_T1)) * ((adc_T >> 4) - ((BMP280_S32_t)dig_T1))) >> 12) *
       ((BMP280_S32_t)dig_T3)) >>
      14;
  t_fine = var1 + var2;
  T      = (t_fine * 5 + 128) >> 8;
  return T;
}

unsigned short dig_P1 = 36477;
short          dig_P2 = -10685;
short          dig_P3 = 3024;
short          dig_P4 = 2855;
short          dig_P5 = 140;
short          dig_P6 = -7;
short          dig_P7 = 15500;
short          dig_P8 = -14600;
short          dig_P9 = 6000;
// Returns pressure in Pa as double. Output value of “96386.2” equals 96386.2 Pa = 963.862 hPa
double bmp280_compensate_P_double(BMP280_S32_t adc_P)
{
  double var1, var2, p;
  var1 = ((double)t_fine / 2.0) - 64000.0;
  var2 = var1 * var1 * ((double)dig_P6) / 32768.0;
  var2 = var2 + var1 * ((double)dig_P5) * 2.0;
  var2 = (var2 / 4.0) + (((double)dig_P4) * 65536.0);
  var1 = (((double)dig_P3) * var1 * var1 / 524288.0 + ((double)dig_P2) * var1) / 524288.0;
  var1 = (1.0 + var1 / 32768.0) * ((double)dig_P1);
  if(var1 == 0.0)
  {
    return 0;        // avoid exception caused by division by zero
  }
  p    = 1048576.0 - (double)adc_P;
  p    = (p - (var2 / 4096.0)) * 6250.0 / var1;
  var1 = ((double)dig_P9) * p * p / 2147483648.0;
  var2 = p * ((double)dig_P8) / 32768.0;
  p    = p + (var1 + var2 + ((double)dig_P7)) / 16.0;
  return p;
}

// Returns pressure in Pa as unsigned 32 bit integer. Output value of “96386” equals 96386 Pa =
// 963.86 hPa
inline BMP280_U32_t bmp280_compensate_P_int32(BMP280_S32_t adc_P)
{
  BMP280_S32_t var1, var2;
  BMP280_U32_t p;
  var1 = (((BMP280_S32_t)t_fine) >> 1) - (BMP280_S32_t)64000;
  var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * ((BMP280_S32_t)dig_P6);
  var2 = var2 + ((var1 * ((BMP280_S32_t)dig_P5)) << 1);
  var2 = (var2 >> 2) + (((BMP280_S32_t)dig_P4) << 16);
  var1 = (((dig_P3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3) +
          ((((BMP280_S32_t)dig_P2) * var1) >> 1)) >>
         18;
  var1 = ((((32768 + var1)) * ((BMP280_S32_t)dig_P1)) >> 15);
  if(var1 == 0)
  {
    return 0;        // avoid exception caused by division by zero
  }
  p = (((BMP280_U32_t)(((BMP280_S32_t)1048576) - adc_P) - (var2 >> 12))) * 3125;
  if(p < 0x80000000)
  {
    p = (p << 1) / ((BMP280_U32_t)var1);
  }
  else
  {
    p = (p / (BMP280_U32_t)var1) * 2;
  }
  var1 = (((BMP280_S32_t)dig_P9) * ((BMP280_S32_t)(((p >> 3) * (p >> 3)) >> 13))) >> 12;
  var2 = (((BMP280_S32_t)(p >> 2)) * ((BMP280_S32_t)dig_P8)) >> 13;
  p    = (BMP280_U32_t)((BMP280_S32_t)p + ((var1 + var2 + dig_P7) >> 4));
  return p;
}
