/**
 * @file soft_uart_test.c
 * @brief Hardware Uart Test
 *
 * Program to test software uart functionality
 *
 * @author Jonathan Clapson jonathan.clapson@gmail.com
 * @date 2014
 * @copyright GNU GENERAL PUBLIC LICENSE version 2
 */

#include <stdint.h>
#include "pic14/pic16f1454.h"

#include "timer/timer.h"
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

void main()
{
	unsigned char ch = 0;
	unsigned char rc_val = 1;
	unsigned char i;

	clock_init();
	timer1_init_1us();
	uart_init();

	TRISC2 = 0;
	RC2 = rc_val;

	clock_init();
	timer1_init_1us();

	while(1){
//		uart_putc(0x65);
		for (i=0; i<100; i++)
			usleep(100);
	
		rc_val = !rc_val;
		RC2 = rc_val;
	}
}
