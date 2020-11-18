#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "bmx_bluetooth.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "driverlib/pwm.h"
#include "bmx_esc.h"



int8_t tx_buffer[100];
char rx_buffer[100];
volatile bool streaming_data = false, streaming_rpm = false;
volatile int rpm;

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
                streaming_data = true;
                TimerEnable(TIMER2_BASE, TIMER_A); //Start 5s countdown to start balance control

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
            setStopFlag();
            streaming_data = false;
        }

        index  = 0;
    }

}

void
//Using uart1 with interrupt enabled
ConfigureBluetoothUART(void)
{
    //
    // Enable the GPIO Peripheral used by the UART.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    //
    // Enable UART1
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);

    //
    // Configure GPIO Pins for UART mode.
    //
    GPIOPinConfigure(GPIO_PB0_U1RX);
    GPIOPinConfigure(GPIO_PB1_U1TX);
    GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);


    //configure divisor and format
    UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 38400, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    // UARTStdioConfig(1, 9600, 16000000);

    // UARTEchoSet(true);

    //IntMasterEnable(); //enable processor interrupts
    IntEnable(INT_UART1); //enable the UART interrupt
    UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT); ///only enable RX and TX interrupt
}

bool streamDataFlag(void)
{
    return streaming_data;
}

bool streamRPMFlag(void)
{
    return streaming_rpm;
}


void printBLEString(char *string){

  //char string[]="This is a test string.";
  int len = 0;
  memset(tx_buffer, 0 , sizeof(tx_buffer));
//  int8_t str_buffer[100];
//  IntMasterDisable();


  int i=0;
  while(string[i]!='\n') {
    tx_buffer[i]=string[i];
    i++;
    len++;
  }


  for(i=0;i<len-1;i++){
    UARTCharPut(UART1_BASE,tx_buffer[i]);
  }

  //Add newline (CR and LF)
  UARTCharPut(UART1_BASE,10); //LF (\n)
//  UARTCharPut(UART1_BASE,13); //CR

//  IntMasterEnable();

}

void printBLEInt(int16_t n){


  IntMasterDisable();
  int buffer[10];
  int len = intToASCII(n,buffer);

  int i;
  //Print each digit
  for(i = len-1; -1<i;i--){

    UARTCharPut(UART1_BASE,buffer[i]);
  }

  //Add newline (CR and LF)
  UARTCharPut(UART1_BASE,10); //LF
  UARTCharPut(UART1_BASE,13); //CR
  IntMasterEnable();



}

void printBLEFloat(int n, int d){

 IntMasterDisable();

  int buffer[10];
  int len = intToASCII(n,buffer);

  int i;
  //Print number
  for(i = len-1; -1<i;i--){

    UARTCharPut(UART1_BASE,buffer[i]);
  }

  UARTCharPut(UART1_BASE,46); //decimal point

    len = intToASCII(d,buffer);
    //Print decimal
    for(i = len-1; -1<i;i--){

      UARTCharPut(UART1_BASE,buffer[i]);
    }

    //Add newline (CR and LF)
      UARTCharPut(UART1_BASE,10); //LF
      UARTCharPut(UART1_BASE,13); //CR

      IntMasterEnable();

}

int intToASCII(int n, int * arr){


  // Iterates from the least significant digit to the most significant
  int len = 0;
  bool negative_n = false;
  //add negative sign
  if(n< 0)
  {
    negative_n = true;
    n = -n;//convert to positive
  }

  do{
   arr[len] = (n % 10) + 48;
   n /= 10;
   len++;
 } while (n != 0);

 //add negative sign
 if(negative_n)
 {
   arr[len] = 45;
   len++;
 }

 return len;
}
