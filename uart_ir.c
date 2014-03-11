#include <stdint.h>

#include "cstdlib/stdio.h"

#include "pic14/pic16f1454.h"

#include "uart/uart.h"
#include "timer/timer.h"

volatile uint8_t toggle_counter = 0;

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

static void isr(void) __interrupt 0
{
	if ( IOCAF5 == 1 ) {
		/* handle */
		toggle_counter =1;
	
		IOCAF5 = 0;
	}
}

void main(void)
{
	int i,j,k;
	uint8_t ra5_val;
	char temp[5];
	char out[10];
	uint16_t u;
	int16_t d;
	int toggle = 0;

	INTCON = 0;

	/* Ensure RA5 isn't being used for clock input */
//	P2SEL = 0;

	/* set porta5 to input */
	TRISA5 = 1;

	/* set cpu clock */
	clock_init();

	/* init peripherals */
	timer0_init_1us();
	uart_init();

	/* enable interrupt on change on RA5 */
	IOCIE = 1;
	/* enable positive edge detection */
	IOCAP5 = 1;
	/* enable negative edge detection */
	IOCAN5 = 1;

	/* enable interrupts */
	GIE = 1;

	TRISC2 = 0;

	//Loop forever
	while ( 1 )
	{
		/*for (j=0; j<250; j++)
			for (i=0; i<16; i++)
				usleep(250);
		toggle = !toggle;
		RC2 = toggle;*/
		
		/* wait for an ir code */
		while (toggle_counter == 0);
		//u = timer0_get_time();
		//u = 24533;
		d = 23412;
		ra5_val=RA5;
		snprintf(out, sizeof(out), "%d,", (char *)&ra5_val);
		snprintf(temp, sizeof(out), "%d\r\n", (char *)&d);
		strncat(out, sizeof(out), temp);
		uart_puts(out);
		toggle_counter = 0;
 	}
}
