#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "ili9341.h"
#include "utils.h"

// uint32_t delay = 300;

void LCD_init() {
    // int time = 0;

    GPIOPinWrite(GPIO_PORTA_BASE, CS, 0);// CS

    LCD_command(ILI9341_SWRESET);
    delayMs(300); // 300ms

    LCD_command(0xEF);
  	LCD_data(0x03);
	  LCD_data(0x80);
	  LCD_data(0x02);
    delayMs(150); // 150ms

    LCD_command(0xCF);
  	LCD_data(0x00);
	  LCD_data(0xC1);
	  LCD_data(0x30);
    delayMs(150); // 150ms

    LCD_command(0xED);
  	LCD_data(0x64);
	  LCD_data(0x03);
	  LCD_data(0x12);
    LCD_data(0x81);
    delayMs(150);// 150ms

    LCD_command(0xE8);
  	LCD_data(0x85);
	  LCD_data(0x00);
	  LCD_data(0x78);
    delayMs(150);// 150ms

    LCD_command(0xCB);
  	LCD_data(0x39);
	  LCD_data(0x2C);
	  LCD_data(0x00);
    LCD_data(0x34);
    LCD_data(0x02);
    delayMs(150);// 150ms

    LCD_command(0xF7);
  	LCD_data(0x20);
    delayMs(150);// 150ms

    LCD_command(0xEA);
  	LCD_data(0x00);
	  LCD_data(0x00);
    delayMs(150);// 150ms

    LCD_command(ILI9341_PWCTR1);
  	LCD_data(0x23);
    delayMs(150);// 150ms

    LCD_command(ILI9341_PWCTR2);
  	LCD_data(0x10);
    delayMs(150);// 150ms

    LCD_command(ILI9341_VMCTR1 );
  	LCD_data(0x3e);
    LCD_data(0x28);
    delayMs(150);// 150ms

    LCD_command(ILI9341_VMCTR2);
  	LCD_data(0x86);
    delayMs(150);// 150ms

    LCD_command(ILI9341_MADCTL);
  	LCD_data(0x48);
    delayMs(150);// 150ms
/*
    LCD_command(ILI9341_VSCRSADD);
  	LCD_data(0x00);
    delayMs(150);// 150ms
 */
    LCD_command(ILI9341_PIXFMT);
  	LCD_data(0x55);
    delayMs(150);// 150ms

    LCD_command(ILI9341_FRMCTR1);
  	LCD_data(0x00);
    LCD_data(0x18);
    delayMs(150);// 150ms

    LCD_command( ILI9341_DFUNCTR);
  	LCD_data(0x08);
    LCD_data(0x82);
    LCD_data(0x27);
    delayMs(150);// 150ms

    LCD_command(0xF2);
  	LCD_data(0); // 1
    LCD_data(0x00);
    delayMs(150);// 150ms

    LCD_command(ILI9341_GAMMASET);
  	LCD_data(0x01);
    delayMs(150);// 150ms

    LCD_command(ILI9341_GMCTRP1);
  	LCD_data(0x0F);
    LCD_data(0x31);
    LCD_data(0x2B);
    LCD_data(0x0C);
    LCD_data(0x0E);
    LCD_data(0x08);
    LCD_data(0x4E);
    LCD_data(0xF1);
    LCD_data(0x37);
    LCD_data(0x07);
    LCD_data(0x10);
    LCD_data(0x03);
    LCD_data(0x0E);
    LCD_data(0x09);
    LCD_data(0x00);
    delayMs(150);// 150ms

    LCD_command(ILI9341_GMCTRN1);
  	LCD_data(0x00);
    LCD_data(0x0E);
    LCD_data(0x14);
    LCD_data(0x03);
    LCD_data(0x11);
    LCD_data(0x07);
    LCD_data(0x31);
    LCD_data(0xC1);
    LCD_data(0x48);
    LCD_data(0x08);
    LCD_data(0x0F);
    LCD_data(0x0C);
    LCD_data(0x31);
    LCD_data(0x36);
    LCD_data(0x0F);
    delayMs(150);// 150ms

    LCD_command(0xB1);
  	LCD_data(0x00);
    LCD_data(0x10);
    delayMs(150);// 150ms

    LCD_command(ILI9341_SLPOUT);
    delayMs(150);// 150ms

    LCD_command(ILI9341_DISPON);

    GPIOPinWrite(GPIO_PORTA_BASE, CS, CS);// CS
    delayMs(150);// 150ms


    GPIOPinWrite(GPIO_PORTA_BASE, CS, 0); // CS

    LCD_command(ILI9341_MADCTL);
    LCD_data(MADCTL_MX | MADCTL_BGR); // rotation
    delayMs(150);// 150ms

    GPIOPinWrite(GPIO_PORTA_BASE, CS, 0); // CS
}

//Put bit in the SPI buffer and wait for the transfer to be completed
void spi_io(unsigned char bit) {

  SSIDataPut(SSI0_BASE, bit);

  while(SSIBusy(SSI0_BASE))
  {
  }
  return;
}

void LCD_command(unsigned char comm) {
    GPIOPinWrite(GPIO_PORTA_BASE, DC, 0); // DC pin set to low to let the LCD know it is a command and not data
    spi_io(comm);
    GPIOPinWrite(GPIO_PORTA_BASE, DC, DC);
}

//Use to send 8 bits of data
void LCD_data(unsigned char data) {
    spi_io(data);
}
//Use to send 16 bits of data
void LCD_data16(unsigned short data) {
    spi_io(data>>8);
    spi_io(data);
}

void LCD_setAddr(unsigned short x, unsigned short y, unsigned short w, unsigned short h) {

    LCD_command(ILI9341_CASET); // Column
    LCD_data16(x);
	  LCD_data16(x+w-1);

	  LCD_command(ILI9341_PASET); // Page
	  LCD_data16(y);
	  LCD_data16(y+h-1);

	  LCD_command(ILI9341_RAMWR); // Into RAM
}

void LCD_drawPixel(unsigned short x, unsigned short y, unsigned short color) {
  // check boundary

    GPIOPinWrite(GPIO_PORTA_BASE, CS, 0); // CS

    LCD_setAddr(x,y,1,1);
    LCD_data16(color);

    GPIOPinWrite(GPIO_PORTA_BASE, CS, CS); // CS
}
//
void LCD_clearScreen(unsigned short color) {
    int i;

    GPIOPinWrite(GPIO_PORTA_BASE, CS, 0); // CS

    LCD_setAddr(0,0,ILI9341_TFTWIDTH,ILI9341_TFTHEIGHT);
	for (i = 0;i < ILI9341_TFTWIDTH*ILI9341_TFTHEIGHT; i++){
		LCD_data16(color);
	}

    GPIOPinWrite(GPIO_PORTA_BASE, CS, CS); // CS
}
//
void LCD_printLetter(char letter,int x,int y,unsigned short FontColor, unsigned short BackColor){
    int i;
    int j;

    for(i=0;i<5;i++){ //iterate through columns of matrix
        char col = ASCII[letter-0x20][i]; //convert ASCII letter into bit matrix
        for(j=0;j<8;j++){ //iterate through the bits in a column
            char pix = (col>>j)&0x1; //get each bit one by one

            if(pix==1) {
                //if statement to prevent pixel outside of margins can be put here
                LCD_drawPixel(x+i,y+j,FontColor);
            }
            else{
                LCD_drawPixel(x+i,y+j,BackColor);
            }
        }
    }
}

void LCD_print(char *m,int x,int y,unsigned short FontColor, unsigned short BackColor){
    int t =0;
    while(m[t]){
        LCD_printLetter(m[t],x+(t*5),y,FontColor,BackColor);
        t++;
    }
}
//
void LCD_printRectangle(int x, int y, int width,int height,unsigned short color){
    int i;

    GPIOPinWrite(GPIO_PORTA_BASE, CS, 0); // CS

    LCD_setAddr(x,y,width,height);
	for (i = 0;i < width*height; i++){
		LCD_data16(color);
	}

    GPIOPinWrite(GPIO_PORTA_BASE, CS, CS); // CS
}
