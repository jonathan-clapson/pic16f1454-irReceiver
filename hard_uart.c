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

void uart_init()
{
	//set baud rate to 19.2K
	SPBRGL = 25;
	SPBRGH = 0;

	BRGH = 0;
	SYNC = 0;

	//set up baud rate control register
	BRG16 = 0;

	TRISC4 = 0;
	TRISC5 = 1;

	//enable eusart
	SPEN=1;

	//enable asynchronous transmission
//	TXSTA = _TXEN | ~_SYNC | ~BRGH; 
//	TXSTA = _TXEN; 
	TXEN = 1;
}

void uart_putc_nb(uint8_t data)
{
//	if (TXSTA & _TRMT) //if TRMT bit is clear its okay to transmit
		TXREG = data;
}
 
void main(void)
{
	unsigned int delay_count = 0;	
	int local_portc = 0;

	//Set PORTC to all outputs, setting bits makes it an input
	TRISC = 0x00;

//	PORTC = 0x00;

//	uart_init();

	while (1)
	{
		PORTC=0xFF;
		for (delay_count=0; delay_count<10; delay_count++);
		PORTC=0x00;
		for (delay_count=0; delay_count<10; delay_count++);
	}

	//Loop forever
	while ( 1 )
	{
//		uart_putc_nb('a');
//		uart_putc_nb('\n');

		//reset loop counter
		delay_count = 0;

		//Delay Loop
//		for (delay_count=0; delay_count<10000; delay_count++)
/*		while ( delay_count < 10000 )
		{
			//Increment the loop counter
			delay_count++;
		}
 
		//Display the count on the PORTC pins
		local_portc ^= 1<<2;
		//PORTC = local_portc;
*/
 	}
 }
