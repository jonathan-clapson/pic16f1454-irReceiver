#include <stdint.h>
#include "pic14/pic16f1454.h"

#include "uart/uart.h"

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

void set_cpu_periph_clock_freq()
{
	IRCF0 = 1;
	IRCF1 = 1;
	IRCF2 = 1;
	IRCF3 = 1;
}

void main(void)
{
	unsigned int delay_count = 0;	
	int local_portc = 0;

	set_cpu_periph_clock_freq();

	uart_init();

	//Loop forever
	while ( 1 )
	{
		uart_putc('a');
		uart_putc('\n');

 	}
}
