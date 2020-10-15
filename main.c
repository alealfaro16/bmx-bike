
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

char imu_str[150];
int16_t roll_str, pitch_str, yaw_str; //IMU angles
int16_t rpm_str = 0;
char rx_buffer[100];

volatile bool streaming_data = false;

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

//UART interrupt handler. Put the function prototype with the "extern" attribute in startup_gcc
// and change the handler name in the interrupt map to make the handlers work
void UART1IntHandler(void)
{
    unsigned long ulStatus;
    static uint16_t index = 0;
    ulStatus = UARTIntStatus(UART1_BASE, true); //get interrupt status
    UARTIntClear(UART1_BASE, ulStatus); //clear the asserted interrupts

    while(UARTCharsAvail(UART1_BASE)) //loop while there are chars
    {
//    UARTCharPutNonBlocking(UART1_BASE, UARTCharGetNonBlocking(UART1_BASE));
//    //echo character
        rx_buffer[index] = UARTCharGetNonBlocking(UART1_BASE);
//        UARTCharPutNonBlocking(UART1_BASE, rx_buffer[index]);
        index++;
    }

    if(rx_buffer[index-1] == '\n' || (rx_buffer[index-1] == '\r'))
    {
        //Parse bool argument
        char * token;
        token = strtok(rx_buffer,";");
        token = strtok(NULL, ";");

        if(strstr(rx_buffer, "balance") != NULL)
        {

            if(strstr(token, "on") != NULL){

                printBLEString("balance on \n");
                turnPIDMW(true);
            }
            else if(strstr(token, "off") != NULL){

                printBLEString("balance off \n");
                turnPIDMW(false);
            }

        }
        else if(strstr(rx_buffer, "stream") != NULL)
        {
            if(strstr(token, "on") != NULL){

                printBLEString("stream on \n");
                streaming_data = true;
            }
            else if(strstr(token, "off") != NULL){

                printBLEString("stream off \n");
                streaming_data = false;
            }

        }
        else if(strstr(rx_buffer, "stop") != NULL)
        {
            //Turn off control and send neutral signal to ESC
            turnPIDMW(false);
            RPMtoESCSignal(0);
        }
        index  = 0;
    }

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
  ConfigurePWM();

  //Configure ISRs
  ConfigureIMUISR(); //called every 100ms
  ConfigureESCSignalISR(); //called every 1s

}


int main(void)
{
  delayMs(3000);

  InitializeTiva();

  uint8_t rev_data[10];
  IMU_init();
  while(!(LSM9DS1_begin())){};
//  initAccel();

  IMU_readWHOAMI_AG(&rev_data[0]);
  IMU_readWHOAMI_M(&rev_data[1]);
  UARTprintf("who_am_i: %x %x\n",rev_data[0],rev_data[1]);


  delayMs(3000);
  GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);

  //start ESC signal
//  RPMtoESCSignal(-150);

  while(1)
  {

//    // Turn on the LED.
//    //
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
//
//    //
//    // Delay for a bit.
    delayMs(100);

    if(streaming_data)
    {
        getIMUData(&roll_str, &pitch_str, &yaw_str);
        getRPM(&rpm_str);
        sprintf(imu_str,"y%3dyp%3dpr%3drs%3ds \n",yaw_str,pitch_str,roll_str, rpm_str);
        printBLEString(imu_str);
    }
//
// Turn off the LED.
//
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);
//
   delayMs(100);


  }

  return 0;
}
