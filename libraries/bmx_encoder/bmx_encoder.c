#include "bmx_encoder.h"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "driverlib/qei.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"


void ConfigureQEI(void){

  // Enable peripherals.
  //
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_QEI0);

  //
  // Wait for the QEI0 module to be ready.
  //
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_QEI0)) {}

  //
  // Unlock GPIOD7 - Like PF0 its used for NMI - Without this step it doesn't work.
  //
  HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY; // In Tiva include this is the same as "_DD" in older versions (0x4C4F434B)
  HWREG(GPIO_PORTD_BASE + GPIO_O_CR) |= 0x80;
  HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = 0;

  //
  // Set what pins are PhA0, PhB0, and IDX0.
  //
  GPIOPinConfigure(GPIO_PD6_PHA0);
  GPIOPinConfigure(GPIO_PD7_PHB0);
  GPIOPinConfigure(GPIO_PD3_IDX0);

  //
  // Set GPIO pins for QEI. This sets them pull up and makes them inputs.
  //
  GPIOPinTypeQEI(GPIO_PORTD_BASE, GPIO_PIN_6 |  GPIO_PIN_7 | GPIO_PIN_3);

  //
  // Disable peripheral and int before configuration.
  //
  QEIDisable(QEI0_BASE);
  QEIIntDisable(QEI0_BASE, QEI_INTERROR | QEI_INTDIR | QEI_INTTIMER | QEI_INTINDEX);

  //
  // Configure quadrature encoder.
  //
  QEIConfigure(QEI0_BASE, (QEI_CONFIG_CAPTURE_A_B  | QEI_CONFIG_RESET_IDX | QEI_CONFIG_QUADRATURE | QEI_CONFIG_NO_SWAP), 8192);

  //
  // Enable quadrature encoder.
  //
  QEIEnable(QEI0_BASE);

  //
  // Set position to a middle value so we can see if things are working.
  //
  QEIPositionSet(QEI0_BASE, 512);

}

void ConfigureQEIVel(void){

  // Configure quadrature encoder.
  //
  QEIVelocityConfigure(QEI0_BASE, QEI_VELDIV_1, SysCtlClockGet() / 3); //Accumulates encoder value every sec

  //
  // Enable quadrature velocity module.
  //
  QEIVelocityEnable(QEI0_BASE);


}

int pulsesToDegrees(float pulses, float ppr){

    float deg = pulses/ppr;

    return (int) deg;
}
