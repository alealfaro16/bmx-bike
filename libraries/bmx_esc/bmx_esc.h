#ifndef BMX_ESC_H
#define BMX_ESC_H

/* For reference on how the pwm values are calculated look at
     http://shukra.cedt.iisc.ernet.in/edwiki/EmSys:TM4C123G_LaunchPad_-_PWM  */

//SysClock is configured to run at 50MHz
#define SYS_CLK_FREQUENCY 50
// PWM Signal is to the ESC is a servo signal (50Hz and use duty cycle to determine speed)
// Full forward - 10%
// Neutral - 7.5%
// Full reverse - 5%

#define PWM_SYSCLK_DIVIDER SYSCTL_PWMDIV_16 //Clock frequency = 50MHz/16 = 3.125MHz or clock period =  3.2e-4 ms
#define PWM_N_TICKS 62500 //calculated using the formula N = (1/f) * SysClk (look at link for details)

//Duty cycle in ticks
// 5%=3125
// 7.5% = 4688
// 10% = 6250
#define FULL_REVERSE_TICKS  3125
#define NEUTRAL_TICKS  4688
#define FULL_FORWARD_TICKS  6250



// 0.025ms dead-band = 78 counts out of 62500
#define DEADBAND_MICRO_S 25
// Forward actual starting point = 4688+78=4766
// Reverse actual starting point = 4688-78=4610
#define DEADBAND_TICKS  78
#define FORWARD_START_TICKS  (NEUTRAL_TICKS + DEADBAND_TICKS)
#define REVERSE_START_TICKS  (NEUTRAL_TICKS - DEADBAND_TICKS)

//Configures a PWM
void ConfigureESCSignal(void);
void startESCSignal(void);

#endif /* BMX_ESC_H */
