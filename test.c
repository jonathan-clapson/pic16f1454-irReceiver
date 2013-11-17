#include "pic14/pic16f1454.h"

//unsigned int __at(_CONFIG1) configWord1 = 0x2ff4;
unsigned int __at(_CONFIG1) configWord1 = 0x2fe4;
unsigned int __at(_CONFIG2) configWord2 = 0x3fff;

//Setup variables
unsigned char ucharCount = 0;
unsigned int uintDelayCount = 0;
 
void main(void)
{
	//Set PORTC to all outputs
	TRISC = 0x00;
 
	ucharCount = 0;
	uintDelayCount = 0;
 
	//Loop forever
	while ( 1 )
	{
		//Delay Loop
		while ( uintDelayCount < 10000 )
		{
			//Increment the loop counter
			uintDelayCount++;
		}
 
		//Reset delay loop counter
		uintDelayCount = 0;
 
		//Increment the count
		ucharCount++;
 
		//Display the count on the PORTC pins
		PORTC = ucharCount;
 	}
 }
