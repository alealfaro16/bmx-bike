#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "bmx_bluetooth.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"



int8_t tx_buffer[100];

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
