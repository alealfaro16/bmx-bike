/*
 * I2C.c - I2C Library Source File
 *
 * Date: 		01/10/2016
 * Revision:		1.05
 * Author: 		PIFers
 *
 * This library supports the communication between TIVA and
 * devices via I2C interface.
 *
 * This library supports 4 I2C modules (from 0 to 3).
 *
 *		Hardware connections:
 * 		I2Cx |  SDA  |  SCL
 * 		---- + ----- + -----
 * 		I2C0 |  PB3	 |  PB2
 * 		I2C1	 |  PA7	 |  PA6
 * 		I2C2	 |  PE5	 |  PE4
 * 		I2C3	 |  PD1	 |  PD0
 *
 */

#include "i2c.h"

//****************************Function definitions*****************************

/******************************************************************************
 * Initialize and configure I2C module in TIVA
 *
 * Parameters:
 *  		ui32Base: I2Cx base address. The address is one of the following values:
 *  			I2C0_BASE, I2C1_BASE, I2C2_BASE, I2C3_BASE.
 *
 *		bFast:
 *  			true : I2C will run in fast mode (400kbps)
 *    		false: I2C will run in standard mode (100kbps)
 *
 * Return: none
 *****************************************************************************/
void ConfigureI2C(void)
{
    /*
         See section 15 and 17 of the TivaWare™ Peripheral Driver Library for more usage info.
       */
       //enable I2C module 0
       SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);

   //    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_I2C0)) {}

       //reset module
       SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);

       //enable GPIO peripheral that contains I2C 0
       SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

       // Configure the pin muxing for I2C0 functions on port B2 and B3.
       GPIOPinConfigure(GPIO_PB2_I2C0SCL);
       GPIOPinConfigure(GPIO_PB3_I2C0SDA);

       // Select the I2C function for these pins.
       GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
       GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

       // Enable and initialize the I2C0 master module.  Use the system clock for
       // the I2C0 module.  The last parameter sets the I2C data transfer rate.
       // If false the data rate is set to 100kbps and if true the data rate will
       // be set to 400kbps.
       I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), false);

       //clear I2C FIFOs
       HWREG(I2C0_BASE + I2C_O_FIFOCTL) = 80008000;
}

/******************************************************************************
 * Write multiple bytes to a slave device
 *
 * Parameters:
 * 	uiSlave_add: the slave address
 * 	*ucData    : the data that are going to be sent
 * 	uiCount	   : number of byte will be sent
 * 	ucStart_add: register address you want to write or a control byte
 *
 * Return: none
 *****************************************************************************/
void I2C_Write(uint32_t ui32Base, unsigned char uiSlave_add, unsigned char *ucData, uint16_t uiCount, unsigned char ucStart_add)
{
	unsigned int temp = 1;
	IntMasterDisable();
	// Set the slave address and setup for a transmit operation.
	I2CMasterSlaveAddrSet(ui32Base, uiSlave_add, false);
	// Place the address to be written in the data register.
	I2CMasterDataPut(ui32Base, ucStart_add);
	while (I2CMasterBusy(ui32Base));
	if (uiCount == 0)
		// Initiate send of character from Master to Slave
		I2CMasterControl(ui32Base, I2C_MASTER_CMD_SINGLE_SEND);
	else
	{
		// Start the burst cycle, writing the address as the first byte.
		I2CMasterControl(ui32Base, I2C_MASTER_CMD_BURST_SEND_START);
		// Write the next byte to the data register.
		while (I2CMasterBusy(ui32Base));
		I2CMasterDataPut(ui32Base, ucData[0]);
		for( ; temp < uiCount; temp++)        //Loop to send data if not the last byte
		{
			// Continue the burst write.
			I2CMasterControl(ui32Base, I2C_MASTER_CMD_BURST_SEND_CONT);
			// Write the next byte to the data register.
			while (I2CMasterBusy(ui32Base));
			I2CMasterDataPut(ui32Base, ucData[temp]);
		}
		// Finish the burst write.
		I2CMasterControl(ui32Base, I2C_MASTER_CMD_BURST_SEND_FINISH);
	}
	while (I2CMasterBusy(ui32Base));
	IntMasterEnable();
}

/******************************************************************************
 * Read multiple bytes from slave device
 *
 * Parameters:
 * 	uiSlave_add: the slave address
 * 	*ucRec_Data: a variable to save the data received
 * 	uiCount	   : number of byte will be received
 * 	ucStart_add: register address you want to read or a control byte
 *
 * Return: none
 *****************************************************************************/
void I2C_Read(uint32_t ui32Base, unsigned char uiSlave_add, unsigned char *ucRec_Data, uint16_t uiCount, unsigned char ucStart_add, bool bDummyRead)
{
	unsigned int uitemp = 0;
		// Set the slave address and setup for a transmit operation.
	IntMasterDisable();
		I2CMasterSlaveAddrSet(ui32Base, uiSlave_add, false);
		// Place the address to be written in the data register.
		I2CMasterDataPut(ui32Base, ucStart_add);
		// Start the burst cycle, writing the address as the first byte.
		I2CMasterControl(ui32Base, I2C_MASTER_CMD_BURST_SEND_START);

		while (I2CMasterBusy(ui32Base));
		//while (!(I2CMasterErr(ui32Base) == I2C_MASTER_ERR_NONE));

	I2CMasterSlaveAddrSet(ui32Base, uiSlave_add, true);
	if (uiCount == 1)
	{
		I2CMasterControl(ui32Base, I2C_MASTER_CMD_SINGLE_RECEIVE);
		while (I2CMasterBusy(ui32Base));
		ucRec_Data[0]  = I2CMasterDataGet(ui32Base);
		if (bDummyRead)
		{
			while (I2CMasterBusy(ui32Base));
			ucRec_Data[0]  = I2CMasterDataGet(ui32Base);
		}
	}
	else
	{
		I2CMasterControl(ui32Base, I2C_MASTER_CMD_BURST_RECEIVE_START);
		if (bDummyRead)
		{
			while (I2CMasterBusy(ui32Base));
			ucRec_Data[0]  = I2CMasterDataGet(ui32Base);
		}
		while (I2CMasterBusy(ui32Base));
		ucRec_Data[uitemp++]  = I2CMasterDataGet(ui32Base);
		for ( ; uitemp < (uiCount - 1); uitemp++)
		{
			I2CMasterControl(ui32Base, I2C_MASTER_CMD_BURST_RECEIVE_CONT);
			while (I2CMasterBusy(ui32Base));
			ucRec_Data[uitemp]  = I2CMasterDataGet(ui32Base);
		}
		I2CMasterControl(ui32Base, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
		while (I2CMasterBusy(ui32Base));
		ucRec_Data[uitemp]  = I2CMasterDataGet(ui32Base);
	}
//	while(I2CMasterIntStatus(ui32Base, false) == 0)
//	{
//	}
	while (I2CMasterBusy(ui32Base));
	IntMasterEnable();
}
