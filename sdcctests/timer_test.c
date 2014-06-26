/**
 * @file timer_test.c
 * @brief Hardware Timer Test
 *
 * Program to test hardware timers
 *
 * @author Jonathan Clapson jonathan.clapson@gmail.com
 * @date 2014
 * @copyright GNU GENERAL PUBLIC LICENSE version 2
 */

#include <stdint.h>
#include "pic14/pic16f1454.h"

#include "timer/timer.h"

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
			usleep(1200);

		rc_val = !rc_val;
		RC2 = rc_val;
	} 
}
