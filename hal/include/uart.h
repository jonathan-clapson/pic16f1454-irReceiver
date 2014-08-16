/**
 * @file uart.h
 * @brief provides generic functions uart
 *
 * Provides functions for uart. These will call either the hardware
 * or software backend based on whether #PIC_USE_HARD_UART is defined
 *
 * @author Jonathan Clapson jonathan.clapson@gmail.com
 * @date 2014
 * @copyright GNU GENERAL PUBLIC LICENSE version 2
 */

#ifndef __PIC_UART_H__
#define __PIC_UART_H__

//if using software uart don't define this
#define PIC_USE_HARD_UART

#ifdef PIC_USE_HARD_UART
#include <hard_uart.h>
#define uart_init() 	hard_uart_init()

#define uart_getc_nb(X) 	hard_uart_getc_nb()
#define uart_getc_bl(X) 	hard_uart_getc_bl()
#define uart_getc(X)		hard_uart_getc_bl()

#define uart_putc_nb(X)		hard_uart_putc_nb(X)
#define uart_putc_bl(X)		hard_uart_putc_bl(X)
#define uart_putc(X)		hard_uart_putc_bl(X)

#define uart_puts_bl(X)		hard_uart_puts_bl(X)
#define uart_puts(X)		hard_uart_puts_bl(X)
#else
#error Software uart currently not functional
#include <soft_uart.h>
#define uart_init() 		soft_uart_init()

#define uart_getc_nb(X)		/* printf() */ soft_uart_getc_bl() /* print a message saying it can't be done */
#define uart_getc_bl(X)		soft_uart_getc_bl(X)
#define uart_getc(X)		soft_uart_getc_bl(X)

#define uart_putc_nb(X)		soft_uart_putc_bl(X) /* print a message saying it can't be done */
#define uart_putc_bl(X)		soft_uart_putc_bl(X)
#define uart_putc(X)		soft_uart_putc_bl(X)
#endif

#endif /* __PIC_UART_H__ */
