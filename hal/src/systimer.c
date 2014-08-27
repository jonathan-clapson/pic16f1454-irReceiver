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

#include <xc.h>
#include <stdint.h>
#include <string.h>

#include <system.h>
#include <timer.h>
#include <systimer.h>

//remove this
#include <uart.h>

volatile struct timer_t interrupt_system_timer;

/*
 * Initialises hardware timer for recording time in us
 *
 * WARNING: This function requires interrupts
 * don't call this function if you haven't setup something to call timer0_interrupt
 */
void systimer_init()
{
	//reset the system timer
	memset(&interrupt_system_timer, 0, sizeof(interrupt_system_timer));

	// set timer 0 to timer mode
	TMR0CS = 0;

	// TIMER 0 is either fed by FOSC/4 or FOSC/4/prescaler
	// Setup prescaler for divide 32
	PS2=1;
	PS1=0;
	PS0=0;
	PSA=0;

	// enable interrupt
	TMR0IE = 1;
}

/*
 * Handles timer overflow
 */
void systimer_interrupt()
{
	// every time systimer overflows 255*32*4/(FOSC)*1000000 uS have ellapsed
	const static uint16_t micros_per_overflow = 256*32*4/SYS_FREQ_MHz;

	if (!(TMR0IF & TMR0IE)) {
		return;
	}

	// increment timer
	interrupt_system_timer.us += micros_per_overflow;

	// check for overflow into ms
	if (interrupt_system_timer.us > 1000) {
		interrupt_system_timer.us -= 1000;
		interrupt_system_timer.ms ++;
	}

	//clear interrupt
	TMR0IF = 0;
}

int8_t systimer_get_time(struct timer_t *sys_timer)
{
	if (!TMR0IE)
		return SYSTIMER_ERR_DISABLED;

	//grab a copy of the system timer buffer
	//FIXME: if this gets interrupted during copy data will go bad :S
	memcpy(sys_timer, &interrupt_system_timer, sizeof(struct timer_t));

	// tried to resolve the amount of micros that have pass since overflow
	// but xc8 compiler can't produce the code necessary to do it with fixed point math
	// and 24bit float seems to not have enough resolution because I get wrong answers?

	return SYSTIMER_ERR_SUCCESS;
}

/*
 * serves as an example of how to use system timer as well as a non blocking usleep
 */
/*void usleep_nb(uint8_t num)
{
	union timer_time current_time;

	// reset timer
	TMR1L = 0x00;
	TMR1H = 0x00;

	// loop until timer expires
	do {
		current_time.byte[0] = TMR1L;
		current_time.byte[1] = TMR1H;
	} while (current_time.word < num);
}*/

