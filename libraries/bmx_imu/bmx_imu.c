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
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"

#include "utils/uartstdio.h"

#include "bmx_imu.h"
#include "bmx_bluetooth.h"
//#include "bno055.h"
#include "lsm9ds1.h"

int16_t gx, gy, gz; // x, y, and z axis readings of the gyroscope
int16_t ax, ay, az; // x, y, and z axis readings of the accelerometer
int16_t mx, my, mz; // x, y, and z axis readings of the magnetometer
int16_t i16roll, i16pitch, i16yaw; //IMU angles
//int16_t temperature; // Chip temperature

const float PI = 3.14159265358979323846;

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
static void readQuaternion()
{


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

  i16roll = roll;
  i16pitch = pitch;
  i16yaw = yaw;

//  sprintf(imu_str,"y%3dyp%3dpr%3dr \n",i16yaw,i16pitch,i16roll);
//  sprintf(imu_str,"y%fyp%fpr%fr \n",i16yaw,i16pitch,i16roll);
//  printString(imu_str);

//  UARTprintf("Pitch: %3d\n",i16pitch);
//  UARTprintf("Roll: %3d\n",i16roll);
//  UARTprintf("Heading: %3d\n",i16yaw);

//  printInt(i16roll);
}

static void ImuReadData(void)
{
    uint32_t status=0;
    status = TimerIntStatus(TIMER5_BASE,true);
    TimerIntClear(TIMER5_BASE,status);

    // Read IMU quaternions.
    readQuaternion();

}

void getIMUData(int16_t * roll, int16_t * pitch, int16_t * yaw)
{
    *roll = i16roll;
    *pitch = i16pitch;
    *yaw = i16yaw;
}

void ConfigureIMUISR(void)
{
    uint32_t period;
    period = 5000000; //100ms


    // Enable 16/32 bit Timer 0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER5);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER5))
    {
    }

    // TIMER_CFG_PERIODIC means full-width periodic
    TimerConfigure(TIMER5_BASE, TIMER_CFG_PERIODIC);

    // Timer load set
    // !! WARNGING !!
    // DO NOT USE SysCtlClockGet() TO SET THE PERIOD
    // IT WILL NOT WORK AS INTENDED, USE THE period VARIABLE INSTEAD

    TimerLoadSet(TIMER5_BASE, TIMER_A, period-1);

    //Enable timer 0A
    TimerEnable(TIMER5_BASE, TIMER_A);

    //Configuration for timer based interrupt if needed

    //Link the timer0,A and the ISR together
    // ISR is the your routine function
    TimerIntRegister(TIMER5_BASE, TIMER_A, ImuReadData);

    //Enable interrupt on timer 0A
    IntEnable(INT_TIMER5A);

    //Enable timer interrupt
    TimerIntEnable(TIMER5_BASE, TIMER_TIMA_TIMEOUT);

}
