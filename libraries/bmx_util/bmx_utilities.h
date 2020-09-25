/// \file bmx_utilities.h
/// \brief General purpose functions. These are useful functions that are not
/// specifically dedicated to any one sensor or routine.

#ifndef BMX_UTILITIES_HG
#define BMX_UTILITIES_HG

/// \brief Represents a float using 2 integers. Used for printing floats to UART
/// \param input: the floating point number
/// \param *output: pointer to an array of 2 integers to store the converted representation
/// \param dec: number of decimal places to save. Must be 1>= && <=6
void float_to_2ints(float input, int *output, unsigned int dec);

#endif
