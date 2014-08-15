/**
 * @file timer.c
 * @brief HAL for hardware timers
 *
 * Provides functions to control hardware timers
 *
 * @author Jonathan Clapson jonathan.clapson@gmail.com
 * @date 2014
 * @copyright GNU GENERAL PUBLIC LICENSE version 2
 */

#include <stdint.h>
#include <xc.h>

#include "timer.h"

void clock_init()
{
        /* clock source set by config words */
        SCS1 = 0;
        SCS0 = 0;
	/* clock divider set to 16MHz */
        IRCF3 = 1;
        IRCF2 = 1;
        IRCF1 = 1;
        IRCF0 = 1;
}

void timer0_init_1us()
{
	/* set timer 0 to timer mode */
	TMR0CS = 0;

	/* 
	 *timer 0 clk starts at clk(16MHz)/4 = 4MHz
	 * divide by 4 to get 1MHz
	 */
	PS2=0;
	PS1=0;
	PS0=1;
	/* enable pre-scaler */
	PSA = 1;
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

uint8_t timer0_get_time()
{
	return TMR0;	
}

uint16_t timer1_get_time()
{
	union timer_time time;
	time.byte[0] = TMR1L;
	time.byte[1] = TMR1H;
	return time.word;
}

void usleep(uint8_t num)
{
	uint8_t timer = TMR0, expire_timer = TMR0+num;
	
	while(timer < num){
		timer = TMR0;
	}
}

void t1_usleep(uint8_t num)
{
	union timer_time current_time;

	/* reset timer */
	TMR1L = 0x00;
	TMR1H = 0x00;

	/* loop until timer expires */
	do {
		current_time.byte[0] = TMR1L;
		current_time.byte[1] = TMR1H;
	} while (current_time.word < num);
}

