
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#include "inc/hw_gpio.h"
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "driverlib/qei.h"
#include "driverlib/pwm.h"

#include "utils/uartstdio.h"

//#include "bno055.h"
#include "lsm9ds1.h"
#include "i2c.h"
#include "bmx_imu.h"
#include "bmx_esc.h"
//#include "bmx_quaternion.h"
#include "bmx_utilities.h"
#include "bmx_encoder.h"
#include "bmx_bluetooth.h"

char data_stream_str[150];
int bike_angles[3] = {0, 0 ,0};
float roll=0;

uint32_t real_rpm;
int16_t cmd_mw_state[2] = {0, 0}; //commanded speed and acceleration
int16_t real_mw_state[2] = {0, 0}; //real speed and acceleration
//PID Control
float set_point = 178.7; // imu is not completely parallel to the floor
float sum_of_error = 0, prev_error = 0, error = 0, dt = 1;//dt = 1s
float Kp = -350, Ki = 1, Kd = 1;

void ConfigureTivaUART()
{

    // Enable the GPIO Peripheral used by the UART.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    // Enable UART0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    // Configure GPIO Pins for UART mode.
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Use the internal 16MHz oscillator as the UART clock source.
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    // Initialize the UART for console I/O.
    UARTStdioConfig(0, 115200, 16000000);
}


void InitializeTiva()
{
  // Enable lazy stacking for interrupt handlers.  This allows floating-point
  // instructions to be used within interrupt handlers, but at the expense of
  // extra stack usage.
  FPULazyStackingEnable();

  // Set the clocking to run directly from the crystal.
  //SysClock runs at 200MHz normally and is being divided by 4 to give 50MHz Sysclk
  SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);


  // Enable the GPIO port that is used for the on-board LED.
   //
   SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

  // Enable the GPIO pins for the LED (PF2 & PF3).
   //
   GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);
   GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);

  ConfigureTivaUART();
//  ConfigureI2C();
  ConfigureBluetoothUART();
  ConfigureIMUUART();
  ConfigureQEI();
  ConfigureQEIVel();
  ConfigurePWM();

  //Configure ISRs
//  ConfigureAcclTimer(); //TIMER5A called every 1ms
  ConfigurePIDControlISR(); //TIMER0A called every 10ms
  ConfigurePIDControlStartTimer(); //TIMER2A called to start control after 5s

}


int main(void)
{
  delayMs(3000);

  InitializeTiva();

  delayMs(3000);
  GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
  while(1)
  {

//    // Turn on the LED.
//    //
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
//
//    //
//    // Delay for a bit.
    delayMs(15);



    if(streamDataFlag())
    {
        getEulers(bike_angles);
        getRealMWState(real_mw_state);
//        sprintf(data_stream_str,"y%.2fyp%.2fpr%.2frs%3dsa%3dav%3dvl%3dl \n",(float) bike_angles[0]/1000.00,(float) bike_angles[1]/1000.00, (float) bike_angles[2]/1000.00, cmd_mw_state[0], cmd_mw_state[1], real_mw_state[0], real_mw_state[1]);
        sprintf(data_stream_str,"y%.2fyp%.2fpr%.2frs%3dsa%3dav%3dvl%3dl \n",(float) bike_angles[0]/1000.00,(float) bike_angles[1]/1000.00, (float) bike_angles[2]/1000.00, 0, 0, real_mw_state[0], 0);
        printBLEString(data_stream_str);

    }

    if(stopFlag())
    {

        sum_of_error = 0;
        cmd_mw_state[0]= 0;
        cmd_mw_state[1] = 0;
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_3, NEUTRAL_TICKS);
//        setMWRPM(0);
        clearStopFlag();
    }
    else
    {
        if(getControlFlag())
        {
            roll = (float) bike_angles[2]/1000;
            error = set_point - roll;
            sum_of_error = sum_of_error + error;
            cmd_mw_state[1] = Kp*error- Kd*cmd_mw_state[0];//+ Ki*sum_of_error +
            cmd_mw_state[0] += cmd_mw_state[1];

            if(cmd_mw_state[0] > MAX_RPM)
            {
                cmd_mw_state[0] = MAX_RPM;
            }
            else if(cmd_mw_state[0] <  MIN_RPM)
            {
                cmd_mw_state[0] = MIN_RPM;
            }
//            PWMPulseWidthSet(PWM0_BASE, PWM_OUT_3, 4850);
//            setMWRPM(cmd_mw_state[0]);
        }
    }
//
// Turn off the LED.
//
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);
//
   delayMs(15);


  }

  return 0;
}
