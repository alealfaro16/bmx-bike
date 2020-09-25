#ifndef __BLUETOOTH_H__
#define __BLUETOOTH_H__

#include "driverlib/uart.h"
#include "utils/uartstdio.h"

void UART1IntHandler(void);
void printString(char *string, int len);
void printInt(int n);
int intToASCII(int n, int * arr);
void printFloat(int n, int d);
void ConfigureBluetoothUART(void);

#endif // ____BLUETOOTH_H____
