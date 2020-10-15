
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/qei.h"

#include "bmx_esc.h"
#include "bmx_imu.h"

#include "utils/uartstdio.h"

volatile int pos, vel, vel_rpm, prev_vel_rpm = 0, dir;
volatile bool PIDOn = false;

float A = 0.5;
float B = 0.5;

int16_t roll, pitch, yaw, prev_roll; //IMU angles
int16_t rpm;

int16_t sum_of_error = 0, prev_error = 0, derror = 0, dt = 1;//dt = 1s
float Kp = 3, Ki =4, Kd = 3;


static void ESCSignal(void)
{
    uint32_t status=0;
    status = TimerIntStatus(TIMER0_BASE,true);
    TimerIntClear(TIMER0_BASE,status);
    // Read position from encoder.
    //
//    pos = QEIPositionGet(QEI0_BASE);
//    vel = QEIVelocityGet(QEI0_BASE);
//    dir = QEIDirectionGet(QEI0_BASE);
//
//    vel_rpm = (int) vel*0.007324*A + B*prev_vel_rpm; //(60/8192)
//    prev_vel_rpm = vel_rpm;
//    UARTprintf("vel = %d rpm \n", vel_rpm*dir);
    if(PIDOn)
    {
        //Get IMU data
        getIMUData(&roll, &pitch, &yaw);

        //Roll used with PID controller to control MW speed
        sum_of_error = roll*dt;
        derror = (roll - prev_error)/dt;

        rpm = Kp*roll + Ki*sum_of_error;
        RPMtoESCSignal(rpm);
    }

}

void turnPIDMW(bool state)
{
    PIDOn = state;
}

void getRPM(int16_t * rpm_str)
{
   *rpm_str = rpm;
}

void ConfigurePWM(void)
{
    // Divide the system frequency by 8.
    SysCtlPWMClockSet(PWM_SYSCLK_DIVIDER);

    // Tiva has 2 PWM modules
    // Each module has 4 PWM generators
    // Each PWM generator has two outputs that shares the same timer and frequency.
    // I think you can set individual duty cycle to these two outputs

    // This enables PWM module 0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_PWM0))
    {
    }

    // Use PWM2 and PWM3 of module 0, which would be the first output of generator 1 of module 0
    // See data sheet for PWM pin-out
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB))
    {
    }
    GPIOPinConfigure(GPIO_PB4_M0PWM2);
    GPIOPinConfigure(GPIO_PB5_M0PWM3);
    GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_4);
    GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_5);

    // Configure the PWM generator for count down mode with immediate updates to the parameters.
    // SYNC parameter has something to do with the change of the period parameter N
    // NO_SYNC means the parameter can be changed without a synchronize event trigger
    PWMGenConfigure(PWM0_BASE, PWM_GEN_1, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);

    // N = (1 / f) * SysClk.  Where N is the PWMGenPeriodSet
    // function parameter, f is the desired frequency, and SysClk is the
    // PWM clock frequency after this function.
    // DIV =16, N = 62500
    // DIV = 8, N = 125000
    // DIV = 4, N = 250000 and so on
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_1, PWM_N_TICKS);

    // Set the pulse width of PWM2 and PWM3 for a 7.5% duty cycle.
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, NEUTRAL_TICKS);
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_3, NEUTRAL_TICKS);

    // Start the timers in generator 1.
    PWMGenEnable(PWM0_BASE, PWM_GEN_1);

    // Enable the outputs.
    PWMOutputState(PWM0_BASE, PWM_OUT_2_BIT|PWM_OUT_3_BIT, true);

}

void ConfigureESCSignalISR(void)
{
    uint32_t period;
    period = 50000000; //1s

    // 5 regular 16/32 bit timer block, 5 wide 32/64 bit timer block
    // Each timer block has timer output A and B
    // See Section 11 of the data sheet for more Information

    // Enable 16/32 bit Timer 0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0))
    {
    }

    // TIMER_CFG_PERIODIC means full-width periodic
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

    // Timer load set
    // !! WARNGING !!
    // DO NOT USE SysCtlClockGet() TO SET THE PERIOD
    // IT WILL NOT WORK AS INTENDED, USE THE period VARIABLE INSTEAD

    TimerLoadSet(TIMER0_BASE, TIMER_A, period-1);

    //Enable timer 0A
    TimerEnable(TIMER0_BASE, TIMER_A);

    //Configuration for timer based interrupt if needed

    //Link the timer0,A and the ISR together
    // ISR is the your routine function
    TimerIntRegister(TIMER0_BASE, TIMER_A, ESCSignal);

    //Enable interrupt on timer 0A
    IntEnable(INT_TIMER0A);

    //Enable timer interrupt
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

}


void RPMtoESCSignal(int16_t speed){

  uint16_t ticks;
  rpm = speed;

  if(speed >0)
  {
     ticks = FORWARD_A*rpm + FORWARD_B;

  }
  else
  {
      ticks = REVERSE_A*rpm+ REVERSE_B;

  }

  //Set PWM to the desired speed
  UARTprintf("ticks = %d \n", ticks);
  PWMPulseWidthSet(PWM0_BASE, PWM_OUT_3, ticks);

}

