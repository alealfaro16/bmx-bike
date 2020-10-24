#ifndef __BLUETOOTH_H__
#define __BLUETOOTH_H__

#include "driverlib/uart.h"
#include "utils/uartstdio.h"

void UART1IntHandler(void);
void printBLEString(char *string);
void prinBLEtInt(int16_t n);
int intToASCII(int n, int * arr);
void printBLEFloat(int n, int d);
void ConfigureBluetoothUART(void);
bool streamDataFlag(void);

#endif // ____BLUETOOTH_H____
