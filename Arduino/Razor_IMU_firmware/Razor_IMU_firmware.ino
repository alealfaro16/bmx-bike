/******************************************************************************
SparkFun 9DoF Razor M0 Example Firmware
Jim Lindblom @ SparkFun Electronics
Original creation date: November 22, 2016
https://github.com/sparkfun/9DOF_Razor_IMU/Firmware
This example firmware for the SparkFun 9DoF Razor IMU M0 
demonstrates how to grab accelerometer, gyroscope, magnetometer,
and quaternion values from the MPU-9250's digital motion processor
(DMP). It prints those values to a serial port and, if a card is
present, an SD card.
Values printed can be configured using the serial port. Settings
can be modified using the included "config.h" file.
Resources:
SparkFun MPU9250-DMP Arduino Library:
  https://github.com/sparkfun/SparkFun_MPU-9250-DMP_Arduino_Library
FlashStorage Arduino Library
  https://github.com/cmaglie/FlashStorage
Development environment specifics:
  Firmware developed using Arduino IDE 1.6.12
Hardware:
  SparkFun 9DoF Razor IMU M0 (SEN-14001)
  https://www.sparkfun.com/products/14001
******************************************************************************/
// MPU-9250 Digital Motion Processing (DMP) Library
#include <SparkFunMPU9250-DMP.h>
// SD Library manages file and hardware control
#include <SD.h>
// config.h manages default logging parameters and can be used
// to adjust specific parameters of the IMU
#include "config.h"
// Flash storage (for nv storage on ATSAMD21)
#ifdef ENABLE_NVRAM_STORAGE
#include <FlashStorage.h>
#endif

MPU9250_DMP imu; // Create an instance of the MPU9250_DMP class

/////////////////////////////
// Logging Control Globals //
/////////////////////////////
// Defaults all set in config.h
bool enableSDLogging = ENABLE_SD_LOGGING;
bool enableSerialLogging = ENABLE_UART_LOGGING;
bool enableYaw = true; //transmit yaw first and the the others
bool enablePitch = false; 
bool enableRoll = false; 
unsigned short accelFSR = IMU_ACCEL_FSR;
unsigned short gyroFSR = IMU_GYRO_FSR;
unsigned short fifoRate = DMP_SAMPLE_RATE;

/////////////////////
// SD Card Globals //
/////////////////////
bool sdCardPresent = false; // Keeps track of if SD card is plugged in
String logFileName; // Active logging file
String logFileBuffer; // Buffer for logged data. Max is set in config

///////////////////////
// LED Blink Control //
///////////////////////
//bool ledState = false;
uint32_t lastBlink = 0;
void blinkLED()
{
  static bool ledState = false;
  digitalWrite(HW_LED_PIN, ledState);
  ledState = !ledState;
}

#ifdef ENABLE_NVRAM_STORAGE
  ///////////////////////////
  // Flash Storage Globals //
  ///////////////////////////
  // Logging parameters are all stored in non-volatile memory.
  // They should maintain the previously set config value.
  FlashStorage(flashEnableSDLogging, bool);
  FlashStorage(flashFirstRun, bool);
  FlashStorage(flashEnableSD, bool);
  FlashStorage(flashEnableSerialLogging, bool);
  FlashStorage(flashenableTime, bool);
  FlashStorage(flashEnableCalculatedValues, bool);
  FlashStorage(flashEnableAccel, bool);
  FlashStorage(flashEnableGyro, bool);
  FlashStorage(flashEnableCompass, bool);
  FlashStorage(flashEnableQuat, bool);
  FlashStorage(flashEnableEuler, bool);
  FlashStorage(flashEnableHeading, bool);
  FlashStorage(flashAccelFSR, unsigned short);
  FlashStorage(flashGyroFSR, unsigned short);
  FlashStorage(flashLogRate, unsigned short);
#endif

void setup()
{
  // Initialize LED, interrupt input, and serial port.
  // LED defaults to off:
  initHardware(); 
  // Initialize the MPU-9250. Should return true on success:
  if ( !initIMU() ) 
  {
    LOG_PORT.println("Error connecting to MPU-9250");
    while (1) ; // Loop forever if we fail to connect
    // LED will remain off in this state.
  }

}

void loop()
{
  // The loop constantly checks for new serial input:
//  if ( LOG_PORT.available() )
//  {
//    // If new input is available on serial port
//    parseSerialInput(LOG_PORT.read()); // parse it
//  }

  // Then check IMU for new data, and log it
  if ( !imu.fifoAvailable() ) // If no new data is available
    return;                   // return to the top of the loop

  // Read from the digital motion processor's FIFO
  if ( imu.dmpUpdateFifo() != INV_SUCCESS )
    return; // If that fails (uh, oh), return to top

  // If enabled, read from the compass.
//  if ( (enableCompass || enableHeading) && (imu.updateCompass() != INV_SUCCESS) )
//    return; // If compass read fails (uh, oh) return to top

  // If logging (to either UART and SD card) is enabled
  if ( enableSerialLogging || enableSDLogging)
    logIMUData(); // Log new data

}

void logIMUData(void)
{
  String imuLog = ""; // Create a fresh line to log

  if (enableYaw) // If Euler-angle logging is enabled
  {
    imu.computeEulerAngles();
    imuLog +=  "yaw=" + String(int(imu.yaw*1000));

    enableYaw = false;
    enablePitch = true;

    imuLog += "\r\n"; // Add a new line
  }

  else if (enablePitch) // If Euler-angle logging is enabled
  {
    imu.computeEulerAngles();
    imuLog +=  "pitch=" + String(int(imu.pitch*1000));

    enablePitch = false;
    enableRoll = true;

    imuLog += "\r\n"; // Add a new line
  }

  else if (enableRoll) // 
  {
    imu.computeEulerAngles();
    imuLog +=  "roll=" + String(int(imu.roll*1000));

    enableRoll = false;
    enableYaw = true;

    imuLog += "\r\n"; // Add a new line
  }

  

  if (enableSerialLogging)  // If serial port logging is enabled
    LOG_PORT.print(imuLog); // Print log line to serial port

  // If SD card logging is enabled & a card is plugged in
  if ( sdCardPresent && enableSDLogging)
  {
    // If adding this log line will put us over the buffer length:
    if (imuLog.length() + logFileBuffer.length() >=
        SD_LOG_WRITE_BUFFER_SIZE)
    {
      sdLogString(logFileBuffer); // Log SD buffer
      logFileBuffer = ""; // Clear SD log buffer 
      blinkLED(); // Blink LED every time a new buffer is logged to SD
    }
    // Add new line to SD log buffer
    logFileBuffer += imuLog;
  }
  else
  {
    // Blink LED once every second (if only logging to serial port)
    if ( millis() > lastBlink + UART_BLINK_RATE )
    {
      blinkLED(); 
      lastBlink = millis();
    }
  }
}

void initHardware(void)
{
  // Set up LED pin (active-high, default to off)
  pinMode(HW_LED_PIN, OUTPUT);
  digitalWrite(HW_LED_PIN, LOW);

  // Set up MPU-9250 interrupt input (active-low)
  pinMode(MPU9250_INT_PIN, INPUT_PULLUP);

  // Set up serial log port
  LOG_PORT.begin(SERIAL_BAUD_RATE);
}

bool initIMU(void)
{
  // imu.begin() should return 0 on success. Will initialize
  // I2C bus, and reset MPU-9250 to defaults.
  if (imu.begin() != INV_SUCCESS)
    return false;

  // Set up MPU-9250 interrupt:
  imu.enableInterrupt(); // Enable interrupt output
  imu.setIntLevel(1);    // Set interrupt to active-low
  imu.setIntLatched(1);  // Latch interrupt output

  // Configure sensors:
  // Set gyro full-scale range: options are 250, 500, 1000, or 2000:
  imu.setGyroFSR(gyroFSR);
  // Set accel full-scale range: options are 2, 4, 8, or 16 g 
  imu.setAccelFSR(accelFSR);
  // Set gyro/accel LPF: options are5, 10, 20, 42, 98, 188 Hz
  imu.setLPF(IMU_AG_LPF); 
  // Set gyro/accel sample rate: must be between 4-1000Hz
  // (note: this value will be overridden by the DMP sample rate)
  imu.setSampleRate(IMU_AG_SAMPLE_RATE); 
  // Set compass sample rate: between 4-100Hz
  imu.setCompassSampleRate(IMU_COMPASS_SAMPLE_RATE); 

  // Configure digital motion processor. Use the FIFO to get
  // data from the DMP.
  unsigned short dmpFeatureMask = 0;
  if (ENABLE_GYRO_CALIBRATION)
  {
    // Gyro calibration re-calibrates the gyro after a set amount
    // of no motion detected
    dmpFeatureMask |= DMP_FEATURE_SEND_CAL_GYRO;
  }
  else
  {
    // Otherwise add raw gyro readings to the DMP
    dmpFeatureMask |= DMP_FEATURE_SEND_RAW_GYRO;
  }
  // Add accel and quaternion's to the DMP
  dmpFeatureMask |= DMP_FEATURE_SEND_RAW_ACCEL;
  dmpFeatureMask |= DMP_FEATURE_6X_LP_QUAT;

  // Initialize the DMP, and set the FIFO's update rate:
  imu.dmpBegin(dmpFeatureMask, fifoRate);

  return true; // Return success
}

bool initSD(void)
{
  // SD.begin should return true if a valid SD card is present
  if ( !SD.begin(SD_CHIP_SELECT_PIN) )
  {
    return false;
  }

  return true;
}

// Log a string to the SD card
bool sdLogString(String toLog)
{
  // Open the current file name:
  File logFile = SD.open(logFileName, FILE_WRITE);
  
  // If the file will get too big with this new string, create
  // a new one, and open it.
  if (logFile.size() > (SD_MAX_FILE_SIZE - toLog.length()))
  {
    logFileName = nextLogFile();
    logFile = SD.open(logFileName, FILE_WRITE);
  }

  // If the log file opened properly, add the string to it.
  if (logFile)
  {
    logFile.print(toLog);
    logFile.close();

    return true; // Return success
  }

  return false; // Return fail
}

// Find the next available log file. Or return a null string
// if we've reached the maximum file limit.
String nextLogFile(void)
{
  String filename;
  int logIndex = 0;

  for (int i = 0; i < LOG_FILE_INDEX_MAX; i++)
  {
    // Construct a file with PREFIX[Index].SUFFIX
    filename = String(LOG_FILE_PREFIX);
    filename += String(logIndex);
    filename += ".";
    filename += String(LOG_FILE_SUFFIX);
    // If the file name doesn't exist, return it
    if (!SD.exists(filename))
    {
      return filename;
    }
    // Otherwise increment the index, and try again
    logIndex++;
  }

  return "";
}
