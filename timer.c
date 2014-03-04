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

typedef union timer_time {
	char high_byte, low_byte;
	unsigned int word;
};

union timer_time timer_data;

void clock_init(void)
{
        //clock is 16MHz
        SCS1 = 0;
        SCS0 = 0;
        IRCF3 = 1;
        IRCF2 = 1;
        IRCF1 = 1;
        IRCF0 = 1;
}

void timer1_init_1us(void)
{
	//set timer 1 to use Fosc/4
	TMR1CS1 = 0;
	TMR1CS0 = 0;

	//further divide by 4
	T1CKPS1 = 1;
	T1CKPS0 = 0;

	//clock is now 500KHz

	//enable timer
	TMR1ON = 1;
}

void usleep(unsigned int num)
{
	union timer_time current_time;

	/* reset timer */
	TMR1L = 0x00;
	TMR1H = 0x00;

	/* loop until timer expires */
	do {
		current_time.low_byte = TMR1L;
		current_time.high_byte = TMR1H;
	} while (current_time.word < num);
}

void main(void)
{
	int i;
	char rc_val = 0;
	clock_init();

	timer1_init_1us();

	TRISC = ~(1<<2);
	RC2 = rc_val;

	while (1){
		for (i=0; i<100; i++)
			usleep(100);

		rc_val = !rc_val;
		RC2 = rc_val;
	} 
}
