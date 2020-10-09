
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



// 6250-(62500*0.025*0.85) =4922
// 3125+(62500*0.025*0.85) =4453

int upFlag = 1;
volatile int flag = 0;
volatile uint32_t pwm_width_in_ticks = NEUTRAL_TICKS;

static void triangleSignal(void)
{
    uint32_t status=0;
    status = TimerIntStatus(TIMER0_BASE,true);
    TimerIntClear(TIMER0_BASE,status);
    if (flag==1)
    {
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, pwm_width_in_ticks-60);
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_3, pwm_width_in_ticks-60);
    //}
        UARTprintf("pwm: %d     ",pwm_width_in_ticks);
        if (pwm_width_in_ticks == 4922)
        {
            upFlag = 0;
        }
        if (pwm_width_in_ticks == 4453)
        {
            upFlag = 1;
        }

        if (pwm_width_in_ticks > 4766 && pwm_width_in_ticks <= 4922 || pwm_width_in_ticks == 4766)
        {
            if (upFlag == 1)
            {
                pwm_width_in_ticks++;
            }
            else if (pwm_width_in_ticks == 4766 && upFlag == 0)
            {
                pwm_width_in_ticks = 4610;
            }
            else
            {
//                pwmOut = pwmOut - 1;
                pwm_width_in_ticks--;
            }

        }
        else if (pwm_width_in_ticks >= 4453 && pwm_width_in_ticks < 4610 || pwm_width_in_ticks == 4610)
        {
            if (upFlag == 0)
            {
                pwm_width_in_ticks--;
            }
            else if (pwm_width_in_ticks == 4610 && upFlag == 1)
            {
                pwm_width_in_ticks = 4766;
            }
            else
            {
                pwm_width_in_ticks++;
            }
        }
    }

}

static void ConfigurePWM(void)
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
