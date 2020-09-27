#include <bmx_esc.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <math.h>

#include "inc/hw_gpio.h"
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"
#include "driverlib/qei.h"
#include "driverlib/pwm.h"

#include "utils/uartstdio.h"

//#include "bno055.h"
#include "lsm9ds1.h"
#include "i2c.h"
#include "bmx_imu.h"
#include "bmx_esc.h"
//#include "bmx_quaternion.h"
#include "bmx_utilities.h"
#include "bmx_encoder.h"
#include "bmx_bluetooth.h"
#include "delay.h"


void ConfigureUART()
{

    // Enable the GPIO Peripheral used by the UART.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    // Enable UART0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    // Configure GPIO Pins for UART mode.
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Use the internal 16MHz oscillator as the UART clock source.
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    // Initialize the UART for console I/O.
    UARTStdioConfig(0, 115200, 16000000);
}

void InitializeTiva()
{
  // Enable lazy stacking for interrupt handlers.  This allows floating-point
  // instructions to be used within interrupt handlers, but at the expense of
  // extra stack usage.
  FPULazyStackingEnable();

  // Set the clocking to run directly from the crystal.
  SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);


  // Enable the GPIO port that is used for the on-board LED.
   //
   SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

  // Enable the GPIO pins for the LED (PF2 & PF3).
   //
   GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);
   GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);

  ConfigureUART();
  ConfigureI2C();
  ConfigureBluetoothUART();
  ConfigureQEI();
  ConfigureQEIVel();
  ConfigureESCSignal();
}


int main(void)
{
  delayMs(3000);

  InitializeTiva();

//  s8 comres = 0;
//  struct bno055_t imu_board;

//  comres += init_imu(&imu_board);

  // Set to 9 DOF mode
//  comres += bno055_set_operation_mode(BNO055_OPERATION_MODE_NDOF);

//  u8 gyro_cal = 0, acc_cal = 0, mag_cal = 0, sys_cal = 0;
  uint8_t rev_data[10];
  IMU_init();
  while(!(LSM9DS1_begin())){};
//  initAccel();

  IMU_readWHOAMI_AG(&rev_data[0]);
  IMU_readWHOAMI_M(&rev_data[1]);
  UARTprintf("who_am_i: %x %x\n",rev_data[0],rev_data[1]);


  volatile int pos,pos_deg, vel, vel_deg;
//  float pos_f, vel_f;

//  struct bno055_quaternion_t q;
//  struct bno055_euler_float_t ea;
//  volatile int xang[2], yang[2], zang[2], wang[2];

  //Initialize strings for bluetooth printing
  char a[]="Encoder count (pulses):";
  int len_a = sizeof(a);
//
  char b[]="Encoder count (degrees):";
  int len_b = sizeof(b);
//
  char c[]="Encoder velocity (pulses):";
  int len_c = sizeof(c);
//
  char d[]="Encoder velocity (degrees/sec):";
  int len_d = sizeof(d);
//
//  char euler_s[100];

//  Quaternion qf

  delayMs(3000);
  GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);

  //start ESC signal
//  startESCSignal();
  while(1)
  {

//    // Turn on the LED.
//    //
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);

    //
    // Delay for a bit.
    delayMs(500);

//     Turn off the BLUE LED.

    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);

    delayMs(500);

    // Read position from encoder.
    //
//    pos = QEIPositionGet(QEI0_BASE);
//    vel = QEIVelocityGet(QEI0_BASE);


    //Print counts (pulses)
//    printString(a,len_a);
//      printInt(pos);
//     UARTprintf("pos = %d \n", pos);
////
//    pos_f = (float) pos;
//    pos_deg = (int) pos_f*0.043945;// division doesn't work for some reason  (360/8192) = 0.043945
//
//    //Print counts (degrees)
    printString(b,len_b);
//    UARTCharPut(UART1_BASE,10); //LF
//    UARTCharPut(UART1_BASE,13); //CR
//    printInt(pos_deg);
//    printQuaternion();

//
    delayMs(500);//500ms

//    //Print velocity (pulses)
//    printString(c,len_c);
//    printInt(vel);
//    UARTprintf("vel = %d \n", vel);
//
//
//    //Print velocity (degrees)
//    vel_f = (float) vel;
//    vel_deg = (int) vel_f*0.043945;// division doesn't work for some reason
//    printString(d,len_d);
//    printInt(vel_deg);
//
//    IntMasterDisable();
//    IntDisable(INT_TIMER0A);
//    IntDisable(INT_TIMER5A_TM4C123);
//    comres += bno055_get_gyro_calib_stat(&gyro_cal);
//    comres += bno055_get_mag_calib_stat(&mag_cal);
//    comres += bno055_get_accel_calib_stat(&acc_cal);
//    comres += bno055_get_sys_calib_stat(&sys_cal);
//
//    comres += bno055_read_quaternion_wxyz(&q);
//    IntEnable(INT_TIMER0A);
//    IntEnable(INT_TIMER5A_TM4C123);
//
//    qf = bnoquat_to_float(&q);
//
//    scale_divide(&qf, QUATERNION_SCALING);
//    normalize(&q);
//
//    ea = toEuler(&qf);
////
//    float_to_2ints(qf.x, xang, 3);
//    float_to_2ints(qf.w, wang, 3);
//    float_to_2ints(qf.y, yang, 3);
//    float_to_2ints(qf.z, zang, 3);
//
////    printInt(xang[1]);
////    printInt(yang[1]);
////    printInt(zang[1]);
////    printInt(wang[1]);
//    float_to_2ints(ea.r, xang, 3);
//    float_to_2ints(ea.p, yang, 3);
//    float_to_2ints(ea.h, zang, 3);

//    sprintf(euler_s,"%d, %d, %d",xang,yang,zang);
//    UARTPrintf()
//    printString(euler_s,10);

//    printInt(xang[1]);
//    printInt(yang[1]);
//    printInt(zang[1]);
//      printString("Euler x:",10);
//      printFloat(xang[0],xang[1]);
//      printFloat(yang[0],yang[1]);
//      printFloat(zang[0],zang[1]);



  }

  return 0;
}
