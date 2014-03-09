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
	char out_val[10] = "a";
	char out_time[10] = "";
	char out[15] ="";
	uint8_t ra5 = RA5;
	uint8_t time = timer0_get_time();
	
	if ( IOCAF5 == 1 ) {
		/* handle */
		toggle_counter ++;
	
		snprintf(out_val, sizeof(out_val), "%d,", &ra5);
		snprintf(out_time, sizeof(out_time), "%u\r\n", &time);
		strncat(out, sizeof(out), out_val);
		strncat(out, sizeof(out), out_time);
		uart_puts(out);

		/* clear interrupt */
		IOCAF5 = 0;
	}
}

void main(void)
{
	uint8_t i, j;
	uint8_t ra5_val;
	uint8_t k;

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

	//Loop forever
	while ( 1 )
	{
		/* wait for an ir code */
/*		while (toggle_counter == 0);
		uart_putc('1');
		*/
		

		

		/* sleep 1s */
		for (k=0; k<4; k++)
		for (j=0; j<250; j++)

			/* sleep 1 ms */
			for(i=0; i<4; i++)
				usleep(250);

		ra5_val = RA5;

/*		uart_putc('v');
		uart_putc(':');
		uart_putc(IOCAF5+48);
		uart_putc('\r');
		uart_putc('\n');*/
 	}
}
