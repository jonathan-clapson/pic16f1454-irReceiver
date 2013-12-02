#include <stdint.h>
#include "pic14/pic16f1454.h"

unsigned int __at(_CONFIG1) configWord1 =
        _FOSC_INTOSC &
         _WDTE_OFF &
        _PWRTE_OFF &
        _MCLRE_OFF &
        _CP_OFF &
        _BOREN_ON &
        _CLKOUTEN_OFF &
        _IESO_OFF &
        _FCMEN_ON;

unsigned int __at(_CONFIG2) configWord2 = 0x3fff & (~(1<<8));

#ifndef _XTAL_FREQ
// This definition is required to calibrate __delay_us() and __delay_ms()
#define _XTAL_FREQ 4000000
#endif

#define Baudrate              9600                      //bps
#define OneBitDelay           (1000000/Baudrate)
#define DataBitCount          8         // no parity, no flow control
#define UART_RX               RC5	// UART RX pin
#define UART_TX               RC4	// UART TX pin

//Function Declarations
void InitSoftUART(void);
unsigned char UART_Receive(void);
void UART_Transmit(const char);

void InitSoftUART(void)		// Initialize UART pins to proper values
{
	UART_TX = 1;			// TX pin is high in idle state
	TRISC = (1<<4);
}

void __delay_us(unsigned int dly)
{
	int i;
	for (i=0; i<dly/3; i++);
}

unsigned char UART_Receive(void)
{
	// Pin Configurations
	// GP1 is UART RX Pin

	unsigned char DataValue = 0;
	unsigned char i;

	//wait for start bit
	while(UART_RX==1);

	__delay_us(OneBitDelay);
	__delay_us(OneBitDelay/2);   // Take sample value in the mid of bit duration

	for ( i = 0; i < DataBitCount; i++ ) {
		if ( UART_RX == 1 ) { //if received bit is high
			DataValue += (1<<i);
		}
		__delay_us(OneBitDelay);
	}

	// Check for stop bit
	if ( UART_RX == 1 ) {      //Stop bit should be high
		__delay_us(OneBitDelay/2);
		return DataValue;
	} else {   //some error occurred !
		__delay_us(OneBitDelay/2);
		return 0x000;
	}
}

void UART_Transmit(const char DataValue)
{
	/* Basic Logic TX pin is usually high. A high to low bit is the starting bit 
and a low to high bit is the ending bit. No parity bit. No flow Control.

	   BitCount is the number of bits to transmit. Data is transmitted LSB first.
	*/
	unsigned char i;

	// Send Start Bit
	UART_TX = 0;
	__delay_us(OneBitDelay);

	for ( i = 0; i < DataBitCount; i++ ) {
		//Set Data pin according to the DataValue
		if( ((DataValue>>i)&0x1) == 0x1 ) {  //if Bit is high
			UART_TX = 1;
		} else {     //if Bit is low
			UART_TX = 0;
		}
	    	__delay_us(OneBitDelay);
	}

	//Send Stop Bit
	UART_TX = 1;
	__delay_us(OneBitDelay);
}

void main()
{

	unsigned char ch = 0;

	InitSoftUART();

	while(1){
		UART_Transmit(0xAA);
	}
}
