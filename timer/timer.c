#include <stdint.h>
#include "pic14/pic16f1454.h"

#include "timer/timer.h"

union timer_time timer_data;

void clock_init()
{
        //clock is 16MHz
        SCS1 = 0;
        SCS0 = 0;
        IRCF3 = 1;
        IRCF2 = 1;
        IRCF1 = 1;
        IRCF0 = 1;
}

void timer1_init_1us()
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

