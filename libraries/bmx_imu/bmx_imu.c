#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
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
float froll, fpitch, fyaw;
//int16_t temperature; // Chip temperature

const float PI = 3.14159265358979323846;
char imu_rx_buffer[100];
int euler_ang[3]; //yaw, pitch, roll

/* Razor IMU Functions */

void
//Using uart1 with interrupt enabled
ConfigureIMUUART(void)
{
    //
    // Enable the GPIO Peripheral used by the UART.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    //
    // Enable UART1
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);

    //
    // Configure GPIO Pins for UART mode.
    //
    GPIOPinConfigure(GPIO_PC6_U3RX);
    GPIOPinConfigure(GPIO_PC7_U3TX);
    GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7);


    //configure divisor and format
    UARTConfigSetExpClk(UART3_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    // UARTStdioConfig(1, 9600, 16000000);

    // UARTEchoSet(true);

    //IntMasterEnable(); //enable processor interrupts
    IntEnable(INT_UART3); //enable the UART interrupt
    UARTIntEnable(UART3_BASE, UART_INT_RX | UART_INT_RT); ///only enable RX and TX interrupt
}

void UART3IntHandler(void)
{
   unsigned long ulStatus;
   static uint16_t index = 0;
   ulStatus = UARTIntStatus(UART3_BASE, true); //get interrupt status
   UARTIntClear(UART3_BASE, ulStatus); //clear the asserted interrupts

   while(UARTCharsAvail(UART3_BASE)) //loop while there are chars
   {
//    UARTCharPutNonBlocking(UART1_BASE, UARTCharGetNonBlocking(UART1_BASE));
//    //echo character
       imu_rx_buffer[index] = UARTCharGetNonBlocking(UART3_BASE);
//       UARTCharPutNonBlocking(UART1_BASE, imu_rx_buffer[index]);
       index++;
   }

   if(imu_rx_buffer[index-1] == '\n' || (imu_rx_buffer[index-1] == '\r'))
   {
       char * number;

       if(strstr(imu_rx_buffer, "yaw") != NULL)
       {
           number = strchr(imu_rx_buffer , '=');
           number++;
           euler_ang[0] = atoi(number);
       }
       else if(strstr(imu_rx_buffer, "pitch") != NULL)
       {
           number = strchr(imu_rx_buffer , '=');
           number++;
           euler_ang[1] = atoi(number);
       }
       else if(strstr(imu_rx_buffer, "roll") != NULL)
       {

           number = strchr(imu_rx_buffer , '=');
           number++;
           euler_ang[2] = atoi(number);
       }
       //Parse roll only
//       euler_ang[0] = atoi(imu_rx_buffer);
       //Parse numbers
//       char * token;
//       token = strtok(imu_rx_buffer,";");
//       int i = 0;
//
//       euler_ang[0] = atoi(token); //yaw
//
//       token = strtok(NULL, ";");
//
//       euler_ang[1] = atoi(token); //pitch
//
//       token = strtok(NULL, ";");
//
//       euler_ang[2] = atoi(token); //roll

       index = 0;
   }

}

void getEulers(int * euler_arr)
{

//    euler_arr[0] = euler_ang[0];
    int i;
    for(i=0;i<3;i++)
    {
        if(i == 2)
        {
            euler_arr[i] = euler_ang[i]; //+ 180000;
        }
        else
        {
            euler_arr[i] = euler_ang[i];
        }

    }
}

/* LSM9DS1 Functions */
// Calculate pitch, roll, and yaw.
// Pitch/roll calculations take from this app note:
// http://cache.freescale.com/files/sensors/doc/app_note/AN3461.pdf?fpsp=1
// Heading calculations taken from this app note:
// http://www51.honeywell.com/aero/common/documents/myaerospacecatalog-documents/Defense_Brochures-documents/Magnetic__Literature_Application_notes-documents/AN203_Compass_Heading_Using_Magnetometers.pdf
static void readQuaternion()
{


  readAccel(&ax, &ay, &az);
  readMag(&mx, &my, &mz);

  froll = atan2(ay, az);
  fpitch = atan2(-ax, sqrt(ay * ay + az * az));

//  float yaw;
  if (my == 0)
    fyaw = (mx < 0) ? 180.0 : 0;
  else
    fyaw = atan2(mx, my);

//  heading -= DECLINATION * PI / 180;

  if (fyaw > PI) fyaw -= (2 * PI);
  else if (fyaw < -PI) fyaw += (2 * PI);
  else if (fyaw < 0) fyaw += 2 * PI;

  // Convert everything from radians to degrees:
  fyaw *= 180.0 / PI;
  fpitch *= 180.0 / PI;
  froll  *= 180.0 / PI;

  //Convert into millidegrees and ints
  i16roll = froll*1000;
  i16pitch = fpitch*1000;
  i16yaw = fyaw*1000;
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

void getIMUDataFloat(float * roll, float * pitch, float * yaw)
{
    *roll = froll;
    *pitch = fpitch;
    *yaw = fyaw;
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
