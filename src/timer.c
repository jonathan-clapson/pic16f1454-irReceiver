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

/*
 * Gets the amount of clocks that have occured on timer1 since last reset
 */
uint16_t timer_get_clocks_16bit()
{
	uint8_t timer_low, timer_high;
	uint16_t timer;

	do {
		timer_high = TMR1H;
		timer_low = TMR1L;
	} while (timer_high != TMR1H);

	timer = (timer_high << 8) | timer_low;

	return timer;
}

/*
 * Initialises hardware timer1
 */
void timer_init()
{
	// set timer 1 to clock from FOSC/4
	T1CONbits.TMR1CS = 0x0;

	// set timer 1 to prescale by 2 (overall FOSC/4/4)
	T1CONbits.T1CKPS = 0x2;

	T1CONbits.nT1SYNC = 0x1; //T1SYNC is inverted, we don't want to synchronize with anything

	// disable timer1 gating
	TMR1GE = 0x0;

	// turn timer1 on
	TMR1ON = 0x1;
}

/*
 * Resets timer1 time
 *
 * This function is intendended to be called during interruptable code
 * As such, it disables the timer while resetting it
 */
void timer_reset()
{
	// turn off timer1
	TMR1ON = 0x0;

	// reset timer1
	TMR1H = 0;
	TMR1L = 1;

	TMR1IF = 0x0;

	TMR1ON = 0x1;

	// reset timer1 interrupt flag, used for detecting overflow
	TMR1IF = 0x0;
}

/*
 * Returns whether the systimer has overflowed.
 */
uint8_t timer_overflow()
{
	return TMR1IF;
}
