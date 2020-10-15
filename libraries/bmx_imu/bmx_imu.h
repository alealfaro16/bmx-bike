/// \file bmx_imu.h
/// \brief Functions to interact with the IMU. These functions initialize and allow
/// for easy interaction with the BNO055 IMU breakout board.

#ifndef BMX_IMU_HG
#define BMX_IMU_HG

//#include "bno055.h"

/* LSM9DSL */

void printAccel(void);
void printGyro(void);
void printMag(void);
void printQuaternion(void);
void ConfigureIMUISR(void);
void getIMUData(int16_t * roll, int16_t * pitch, int16_t * yaw);

#endif
