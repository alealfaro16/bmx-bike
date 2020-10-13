#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>

#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

#include "driverlib/i2c.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

#include "utils/uartstdio.h"

#include "bmx_imu.h"
#include "bmx_bluetooth.h"
//#include "bno055.h"
#include "lsm9ds1.h"

//int16_t gx, gy, gz; // x, y, and z axis readings of the gyroscope
//int16_t ax, ay, az; // x, y, and z axis readings of the accelerometer
//int16_t mx, my, mz; // x, y, and z axis readings of the magnetometer
//int16_t temperature; // Chip temperature

const float PI = 3.14159265358979323846;
char imu_str[100];

/* LSM9DS1 Functions */

void printAccel()
{
  // To read from the accelerometer, you must first call the
  // readAccel() function. When this exits, it'll update the
  // ax, ay, and az variables with the most current data.
  int16_t  ax, ay, az; // x, y, and z axis readings of the accelerometer

  readAccel(&ax, &ay, &az);

  // Now we can use the ax, ay, and az variables as we please.
  // Either print them as raw ADC values, or calculated in g's.
  UARTprintf("A: ");
  UARTprintf("x:%5d ",ax);
  UARTprintf("y:%5d ",ay);
  UARTprintf("z:%5d ",az);
//  UARTprintf("\n");
}

void printGyro()
{
  // To read from the accelerometer, you must first call the
  // readAccel() function. When this exits, it'll update the
  // ax, ay, and az variables with the most current data.
  int16_t gx, gy, gz; // x, y, and z axis readings of the magnetometer

  readGyro(&gx, &gy, &gz);

  // Now we can use the ax, ay, and az variables as we please.
  // Either print them as raw ADC values, or calculated in g's.
  UARTprintf("G: ");
  UARTprintf("x:%5d ",gx);
  UARTprintf("y:%5d ",gy);
  UARTprintf("z:%5d ",gz);
//  UARTprintf("\n");
}

void printMag()
{
  // To read from the accelerometer, you must first call the
  // readAccel() function. When this exits, it'll update the
  // ax, ay, and az variables with the most current data.
  int16_t mx, my, mz; // x, y, and z axis readings of the magnetometer

  readMag(&mx, &my, &mz);

  // Now we can use the ax, ay, and az variables as we please.
  // Either print them as raw ADC values, or calculated in g's.
  UARTprintf("M: ");
  UARTprintf("x:%5d ",mx);
  UARTprintf("y:%5d ",my);
  UARTprintf("z:%5d ",mz);
  UARTprintf("\n");
}

// Calculate pitch, roll, and yaw.
// Pitch/roll calculations take from this app note:
// http://cache.freescale.com/files/sensors/doc/app_note/AN3461.pdf?fpsp=1
// Heading calculations taken from this app note:
// http://www51.honeywell.com/aero/common/documents/myaerospacecatalog-documents/Defense_Brochures-documents/Magnetic__Literature_Application_notes-documents/AN203_Compass_Heading_Using_Magnetometers.pdf
void printQuaternion()//float ax, float ay, float az, float mx, float my, float mz)
{
  int16_t  ax, ay, az; // x, y, and z axis readings of the accelerometer
  int16_t mx, my, mz; // x, y, and z axis readings of the magnetometer

  readAccel(&ax, &ay, &az);
  readMag(&mx, &my, &mz);

  float roll = atan2(ay, az);
  float pitch = atan2(-ax, sqrt(ay * ay + az * az));

  float yaw;
  if (my == 0)
    yaw = (mx < 0) ? 180.0 : 0;
  else
    yaw = atan2(mx, my);

//  heading -= DECLINATION * PI / 180;

  if (yaw > PI) yaw -= (2 * PI);
  else if (yaw < -PI) yaw += (2 * PI);
  else if (yaw < 0) yaw += 2 * PI;

  // Convert everything from radians to degrees:
  yaw *= 180.0 / PI;
  pitch *= 180.0 / PI;
  roll  *= 180.0 / PI;

  int16_t i16roll, i16pitch, i16yaw;
  i16roll = roll;
  i16pitch = pitch;
  i16yaw = yaw;

  sprintf(imu_str,"y%3dyp%3dpr%3dr \n",i16yaw,i16pitch,i16roll);
//  sprintf(imu_str,"y%fyp%fpr%fr \n",i16yaw,i16pitch,i16roll);
  printString(imu_str);

//  UARTprintf("Pitch: %3d,",i16pitch);
//  UARTprintf("Roll: %3d,",i16roll);
//  UARTprintf("Heading: %3d",i16heading);
//  UARTprintf(" @degree\n");
//  printInt(i16roll);
}

