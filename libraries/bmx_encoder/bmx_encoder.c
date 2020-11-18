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
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"

#include "bmx_bluetooth.h"

uint32_t real_rpm, prev_rpm, accl;
int16_t current_ms, prev_ms, ms_count;

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

void countMS(void)
{
    uint32_t status=0;
    status = TimerIntStatus(TIMER5_BASE,true);
    TimerIntClear(TIMER5_BASE,status);

    if(streamDataFlag())
    {
        ms_count++;
    }

}

void ConfigureAcclTimer(void)
{
    uint32_t period;
    period = 50000; //1ms


    // Enable 16/32 bit Timer 0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER5);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER5))
    {
    }

    // TIMER_CFG_PERIODIC means full-width periodic
    TimerConfigure(TIMER5_BASE, TIMER_CFG_PERIODIC);

    // Timer load set
    // !! WARNGING !!
    // DO NOT USE SysCtlClockGet() TO SET THE PERIOD
    // IT WILL NOT WORK AS INTENDED, USE THE period VARIABLE INSTEAD

    TimerLoadSet(TIMER5_BASE, TIMER_A, period-1);

    //Enable timer 0A
    TimerDisable(TIMER5_BASE, TIMER_A);

    //Configuration for timer based interrupt if needed

    //Link the timer0,A and the ISR together
    // ISR is the your routine function
    TimerIntRegister(TIMER5_BASE, TIMER_A, countMS);
//
//    //Enable interrupt on timer 0A
    IntEnable(INT_TIMER5A);
//
//    //Enable timer interrupt
    TimerIntEnable(TIMER5_BASE, TIMER_TIMA_TIMEOUT);

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

void getRealMWState(int16_t * real_mw_state)
{
    prev_rpm = real_rpm;
    real_rpm = QEIVelocityGet(QEI0_BASE);
    real_rpm = (int) real_rpm*0.007324;//(constant is (360/8192)/6 to get RPM from pulses per second)

    real_mw_state[0] = real_rpm*QEIDirectionGet(QEI0_BASE);

    current_ms = ms_count;
    ms_count = 0;

    real_mw_state[1] = (real_rpm - prev_rpm)/current_ms;
}

int pulsesToDegrees(float pulses, float ppr){

    float deg = pulses/ppr;

    return (int) deg;
}
