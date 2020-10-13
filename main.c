
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <math.h>

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


void ConfigureUART()
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

  ConfigureUART();
  ConfigureI2C();
  ConfigureBluetoothUART();
  ConfigureQEI();
  ConfigureQEIVel();
  ConfigureESCSignal();
}


int main(void)
{
  delayMs(3000);

  InitializeTiva();

  uint8_t rev_data[10];
//  IMU_init();
//  while(!(LSM9DS1_begin())){};
////  initAccel();
//
//  IMU_readWHOAMI_AG(&rev_data[0]);
//  IMU_readWHOAMI_M(&rev_data[1]);
//  UARTprintf("who_am_i: %x %x\n",rev_data[0],rev_data[1]);


  delayMs(3000);
  GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);

  //start ESC signal
  RPMtoESCSignal(-150);

  while(1)
  {

//    // Turn on the LED.
//    //
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
//
//    //
//    // Delay for a bit.
    delayMs(500);
//
// Turn off the LED.
//
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);
//
   delayMs(500);


  }

  return 0;
}
