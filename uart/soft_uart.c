#include <stdint.h>
#include "pic14/pic16f1454.h"

#include "timer/timer.h"

#define Baudrate              1                      //bps
//#define OneBitDelay           (1000000/Baudrate)
#define OneBitDelay           (3333)
#define DataBitCount          8         // no parity, no flow control
#define UART_RX               RC5	// UART RX pin
#define UART_TX               RC4	// UART TX pin

/* Initialise the gpio's to use for uart */
void soft_uart_pins_init()
{
	/* UART TX Pin is high when idle */
	UART_TX = 1;			
//	int b;
//	b = PORTC;
//	b ^= (1<<4);
//	PORTC = b;
	/* configure uart tx pin as output */
	TRISC4 = 0;
	/*configure uart rx pin as input */
	TRISC5 = 1;
}

/* Initialize Software uart */
void soft_uart_init()
{
	soft_uart_pins_init();
}

int soft_uart_getc_bl(uint8_t *data)
{
	// Pin Configurations
	// GP1 is UART RX Pin

	unsigned char DataValue = 0;
	unsigned char i;

	//wait for start bit
	while(UART_RX==1);

	usleep(OneBitDelay);
	usleep(OneBitDelay/2);   // Take sample value in the mid of bit duration

	for ( i = 0; i < DataBitCount; i++ ) {
		if ( UART_RX == 1 ) { //if received bit is high
			DataValue += (1<<i);
		}
		usleep(OneBitDelay);
	}

	// Check for stop bit
	if ( UART_RX == 1 ) {      //Stop bit should be high
		usleep(OneBitDelay/2);
		*data = DataValue;
		return 0;
	} else {   //some error occurred !
		usleep(OneBitDelay/2);
		return -1;
	}
}

int soft_uart_putc_bl(uint8_t data)
{
	/* Basic Logic TX pin is usually high. 
	A high to low bit is the starting bit and a low to high bit is 
	the ending bit. No parity bit. No flow Control.
	BitCount is the number of bits to transmit. Data is 
	transmitted LSB first. 	*/

/*	unsigned char i;

	// Send Start Bit
	UART_TX = 0;
	usleep(OneBitDelay);

	for ( i = 0; i < DataBitCount; i++ ) {
		//Set Data pin according to the DataValue
		if((data>>i) &0x1) {  //if Bit is high
			UART_TX = 1;
		} else {     //if Bit is low
			UART_TX = 0;
		}
	    	usleep(OneBitDelay);
	}

	//Send Stop Bit
	UART_TX = 1;
	usleep(OneBitDelay);*/
	return 0;
}
