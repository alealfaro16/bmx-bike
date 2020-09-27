#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>

#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

#include "driverlib/i2c.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

#include "bmx_imu.h"
#include "bmx_bluetooth.h"
//#include "bno055.h"
#include "lsm9ds1.h"

//int16_t gx, gy, gz; // x, y, and z axis readings of the gyroscope
//int16_t ax, ay, az; // x, y, and z axis readings of the accelerometer
//int16_t mx, my, mz; // x, y, and z axis readings of the magnetometer
//int16_t temperature; // Chip temperature

const float PI = 3.14159265358979323846;
char imu_str[100];


extern void UARTprintf(const char *pcString, ...);

/* LSM9DS1 Functions */

//void printAccel()
//{
//  // To read from the accelerometer, you must first call the
//  // readAccel() function. When this exits, it'll update the
//  // ax, ay, and az variables with the most current data.
//  readAccel();
//
//  // Now we can use the ax, ay, and az variables as we please.
//  // Either print them as raw ADC values, or calculated in g's.
//  UARTprintf("A: ");
//  UARTprintf("x:%5d ",ax);
//  UARTprintf("y:%5d ",ay);
//  UARTprintf("z:%5d ",az);
////  UARTprintf("\n");
//}
//
//void printGyro()
//{
//  // To read from the accelerometer, you must first call the
//  // readAccel() function. When this exits, it'll update the
//  // ax, ay, and az variables with the most current data.
//  readGyro();
//
//  // Now we can use the ax, ay, and az variables as we please.
//  // Either print them as raw ADC values, or calculated in g's.
//  UARTprintf("G: ");
//  UARTprintf("x:%5d ",gx);
//  UARTprintf("y:%5d ",gy);
//  UARTprintf("z:%5d ",gz);
////  UARTprintf("\n");
//}
//
//void printMag()
//{
//  // To read from the accelerometer, you must first call the
//  // readAccel() function. When this exits, it'll update the
//  // ax, ay, and az variables with the most current data.
//  readMag();
//
//  // Now we can use the ax, ay, and az variables as we please.
//  // Either print them as raw ADC values, or calculated in g's.
//  UARTprintf("M: ");
//  UARTprintf("x:%5d ",mx);
//  UARTprintf("y:%5d ",my);
//  UARTprintf("z:%5d ",mz);
//  UARTprintf("\n");
//}

// Calculate pitch, roll, and yaw.
// Pitch/roll calculations take from this app note:
// http://cache.freescale.com/files/sensors/doc/app_note/AN3461.pdf?fpsp=1
// Heading calculations taken from this app note:
// http://www51.honeywell.com/aero/common/documents/myaerospacecatalog-documents/Defense_Brochures-documents/Magnetic__Literature_Application_notes-documents/AN203_Compass_Heading_Using_Magnetometers.pdf
void printQuaternion()//float ax, float ay, float az, float mx, float my, float mz)
{
  int16_t  ax, ay, az; // x, y, and z axis readings of the accelerometer
  int16_t mx, my, mz; // x, y, and z axis readings of the magnetometer

  readAccel(&ax, &ay, &az);
  readMag(&mx, &my, &mz);

  float roll = atan2(ay, az);
  float pitch = atan2(-ax, sqrt(ay * ay + az * az));

  float yaw;
  if (my == 0)
    yaw = (mx < 0) ? 180.0 : 0;
  else
    yaw = atan2(mx, my);

//  heading -= DECLINATION * PI / 180;

  if (yaw > PI) yaw -= (2 * PI);
  else if (yaw < -PI) yaw += (2 * PI);
  else if (yaw < 0) yaw += 2 * PI;

  // Convert everything from radians to degrees:
  yaw *= 180.0 / PI;
  pitch *= 180.0 / PI;
  roll  *= 180.0 / PI;

  int16_t i16roll, i16pitch, i16yaw;
  i16roll = roll;
  i16pitch = pitch;
  i16yaw = yaw;

  sprintf(imu_str,"y%3dyp%3dpr%3dr \n",i16yaw,i16pitch,i16roll);
  printString(imu_str);

//  UARTprintf("Pitch: %3d,",i16pitch);
//  UARTprintf("Roll: %3d,",i16roll);
//  UARTprintf("Heading: %3d",i16heading);
//  UARTprintf(" @degree\n");
//  printInt(i16roll);
}



/* BNO055 Functions  */

//s8 _imu_i2c_read(u8 dev_address, u8 reg_address, u8 *arr_data, u8 count)
//{
//  // UARTprintf("Read Counts %d, \n", count);
//  s8 comres = 0;
//  // This function is used to select the device to read from
//  // false == write to slave
//  I2CMasterSlaveAddrSet(I2C0_BASE, dev_address, false);
//
//  // Set the I2C Bus to tell the device which first register is meant to be read
//  I2CMasterDataPut(I2C0_BASE, reg_address);
//
//  // send slave address, control bit, and register address byte to slave device
//  I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);
//
//  //wait for MCU to finish transaction
//  while(I2CMasterBusy(I2C0_BASE));
//
//  // Read in 1 byte
//  if(count == 1)
//  {
//    // specify that we are going to read from slave device
//    // true == read from slave
//    I2CMasterSlaveAddrSet(I2C0_BASE, dev_address, true);
//
//    //send slave address, control bit, and recieve the byte of data
//    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
//    while(I2CMasterBusy(I2C0_BASE));
//
//    // write byte to data variable
//    arr_data[0] = I2CMasterDataGet(I2C0_BASE);
//
//    // Check for errors
//    if (I2CMasterErr(I2C0_BASE)==I2C_MASTER_ERR_NONE)
//    {
//      comres = 0; // success
//    }
//    else
//    {
//      comres = -1; // error occured
//    }
//  }
//
//  // Read in 2 bytes
//  else if(count == 2)
//  {
//    // Get first byte
//    I2CMasterSlaveAddrSet(I2C0_BASE, dev_address, true);
//    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
//    while(I2CMasterBusy(I2C0_BASE));
//
//    // put read data in data array
//    arr_data[0] = I2CMasterDataGet(I2C0_BASE);
//
//    // Check for errors
//    if (I2CMasterErr(I2C0_BASE)==I2C_MASTER_ERR_NONE)
//    {
//      comres = 0; // success
//    }
//    else
//    {
//      comres = -1; // error occured
//    }
//
//
//    // Read final byte from slave
//    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
//    while(I2CMasterBusy(I2C0_BASE));
//    // put read data in data array
//    arr_data[1] = I2CMasterDataGet(I2C0_BASE);
//
//    // Check for errors
//    if (I2CMasterErr(I2C0_BASE)==I2C_MASTER_ERR_NONE)
//    {
//      comres = 0; // success
//    }
//    else
//    {
//      comres = -1; // error occured
//    }
//
//  }
//
//  // Read in more than 2 bytes
//  else
//  {
//    u8 i;
//    for(i = 0; i < count; i++)
//    {
//      if(i == 0)
//      {
//        // Start Communication
//        I2CMasterSlaveAddrSet(I2C0_BASE, dev_address, true);
//        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
//        // UARTprintf("Start ");
//      }
//      else if(i == count -1)
//      {
//        // Read the last byte
//        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
//        // UARTprintf(" End\n");
//      }
//      else
//      {
//        // read middle byte
//        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT);
//        // UARTprintf(" Cont ");
//      }
//
//      while(I2CMasterBusy(I2C0_BASE));
//
//      // put read data in data array
//      arr_data[i] = I2CMasterDataGet(I2C0_BASE);
//
//      if (I2CMasterErr(I2C0_BASE)==I2C_MASTER_ERR_NONE)
//      {
//        comres = 0; // success
//      }
//      else
//      {
//        comres = -1; // error occured, end comms and exit
//        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
//        while(I2CMasterBusy(I2C0_BASE));
//        i = count;
//      }
//    }
//  }
//
//  return comres;
//}
//
//s8 _imu_i2c_write(u8 dev_address, u8 reg_address, u8 *var_data, u8 count)
//{
//    // UARTprintf("Write Counts %d, \n", count);
//    s8 comres = 0;
//    // Tell the master module what address it will place on the bus when
//    // communicating with the slave.
//    I2CMasterSlaveAddrSet(I2C0_BASE, dev_address, false);
//
//    //send the slave address, control bit, and registar address for where to write to
//    I2CMasterDataPut(I2C0_BASE, reg_address);
//
//    //Initiate send of data from the MCU
//    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);
//
//    // Wait until MCU is done transferring.
//    while(I2CMasterBusy(I2C0_BASE));
//
//    // the BNO055 only ever writes 1 byte of info so if count != 1, throw an error
//    if(count == 1)
//    {
//
//      // send the information to write
//      I2CMasterDataPut(I2C0_BASE, *var_data);
//
//      // Initiate send of data from the MCU
//      I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
//
//      // Wait until MCU is done transferring
//      while(I2CMasterBusy(I2C0_BASE));
//
//      if(I2CMasterErr(I2C0_BASE)==I2C_MASTER_ERR_NONE)
//      {
//        comres = 0; // Success
//      }
//      else
//      {
//        comres = -1; // Error
//      }
//
//      UARTprintf("Write Err %x, \n", I2CMasterErr(I2C0_BASE));
//      UARTprintf("ERR ADDR ACK %x, \n", I2C_MASTER_ERR_ADDR_ACK);
//      UARTprintf("ERR DATA ACK %x, \n", I2C_MASTER_ERR_DATA_ACK);
//      UARTprintf("ERR ARB LOST %x, \n", I2C_MASTER_ERR_ARB_LOST);
//
//    }
//    else
//    {
//      comres = -1; // Error
//    }
//
//    return comres;
//}
//
//void _ms_delay(u32 ms)
//{
//  SysCtlDelay(ms * 5334); // 16000000MHz/3000 ~= 5334 assembly commands per ms
//}
//
//s8 init_imu(struct bno055_t *sensor)
//{
//  // initialize setup struct and populate the required information
//  s8 err = 0;
//  sensor->bus_write = _imu_i2c_write;
//  sensor->bus_read = _imu_i2c_read;
//  sensor->delay_msec = _ms_delay;
//  sensor->dev_addr = BNO055_I2C_ADDR1;
//
//  // bno055 builtin initialization function
//  err = bno055_init(sensor);
//  return err;
//}

// s8 calibration_status(u8 *calibration_arr)
// {
//   s8 comres = 0;
//
//   comres += bno055_get_mag_calib_stat(calibration_arr);
//
//
// }
