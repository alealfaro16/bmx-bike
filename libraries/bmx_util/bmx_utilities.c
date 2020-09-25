#include <stdlib.h>

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
