#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "driverlib/sysctl.h"

#include "bmx_utilities.h"

void float_to_2ints(float input, int *output, unsigned int dec)
{
  int dec_places = 1;
  int i;
  for(i = 0; i < dec; i++)
  {
      dec_places *= 10;
  }

  output[0] = input; // number
  output[1] = abs(input*dec_places - output[0]*dec_places); // decimal
}

void delayMs(uint32_t ui32Ms) {

    // 1 clock cycle = 1 / SysCtlClockGet() second
    // 1 SysCtlDelay = 3 clock cycle = 3 / SysCtlClockGet() second
    // 1 second = SysCtlClockGet() / 3
    // 0.001 second = 1 ms = SysCtlClockGet() / 3 / 1000

    SysCtlDelay(ui32Ms * (SysCtlClockGet() / 3 / 1000));
}

void delayUs(uint32_t ui32Us) {
    SysCtlDelay(ui32Us * (SysCtlClockGet() / 3 / 1000000));
}
