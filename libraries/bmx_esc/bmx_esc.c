
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
#include "bmx_esc.h"

volatile uint32_t pwmOut = 4766;

// 0.025ms dead-band = 156 counts out of 125000
// Forward actual starting point = 9375 + 156 = 9531
// Reverse actual starting point = 9375 - 156 = 9219
//4688+78=4766
//4688-78=4610
// 5%=3125
// 7.5% = 4688
// 6250-(62500*0.025*0.85) =4922
// 3125+(62500*0.025*0.85) =4453

int upFlag = 1;
volatile int flag = 0;

static void triangleSignal(void)
{
    uint32_t status=0;
    status = TimerIntStatus(TIMER0_BASE,true);
    TimerIntClear(TIMER0_BASE,status);
    if (flag==1)
    {
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, pwmOut-60);
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_3, pwmOut-60);
    //}
        UARTprintf("pwm: %d     ",pwmOut);
        if (pwmOut == 4922)
        {
            upFlag = 0;
        }
        if (pwmOut == 4453)
        {
            upFlag = 1;
        }

        if (pwmOut > 4766 && pwmOut <= 4922 || pwmOut == 4766)
        {
            if (upFlag == 1)
            {
                pwmOut++;
            }
            else if (pwmOut == 4766 && upFlag == 0)
            {
                pwmOut = 4610;
            }
            else
            {
                pwmOut = pwmOut - 1;
            }

        }
        else if (pwmOut >= 4453 && pwmOut < 4610 || pwmOut == 4610)
        {
            if (upFlag == 0)
            {
                pwmOut = pwmOut - 1;
            }
            else if (pwmOut == 4610 && upFlag == 1)
            {
                pwmOut = 4766;
            }
            else
            {
                pwmOut++;
            }
        }
    }

}

static void ConfigurePWM(void)
{
    // Divide the system frequency by 8.
    SysCtlPWMClockSet(SYSCTL_PWMDIV_16);

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
    // DIV = 8, N = 125000
    // DIV = 4, N = 250000 and so on
    // 5% duty cycle of 125000 is 6250 (FULL REVERSE)
    // 7.5% duty cycle of 125000 is 9375 (NEUTRAL)
    // 10% duty cycle  of 125000 is 12500 (FULL FORWARD)
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_1, 62500);

    // Set the pulse width of PWM2 and PWM3 for a 7.5% duty cycle.
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, 4688);
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_3, 4688);

    // Start the timers in generator 1.
    PWMGenEnable(PWM0_BASE, PWM_GEN_1);

    // Enable the outputs.
    PWMOutputState(PWM0_BASE, PWM_OUT_2_BIT|PWM_OUT_3_BIT, true);

}

static void ConfigureESCSignalISR(void (*f)(void))
{
    uint32_t period;
    uint32_t period2;
    period = 500000; //1s
    period2 = 50000;

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
    TimerIntRegister(TIMER0_BASE, TIMER_A, *f);

    //Enable interrupt on timer 0A
    IntEnable(INT_TIMER0A);

    //Enable timer interrupt
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);


//    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER5);
//    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER5))
//    {
//    }
//    TimerConfigure(TIMER5_BASE, TIMER_CFG_PERIODIC);
//    TimerLoadSet(TIMER5_BASE, TIMER_A, period2 -1);
//
//    TimerIntRegister(TIMER5_BASE, TIMER_A, *ff);
//
//    TimerIntEnable(TIMER5_BASE, TIMER_TIMA_TIMEOUT);
//
//    TimerEnable(TIMER5_BASE, TIMER_A);

}

void ConfigureESCSignal(void){
    ConfigurePWM();
    ConfigureESCSignalISR(triangleSignal);
}
void startESCSignal(void){
    flag=1;
}
