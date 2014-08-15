/**
 * @file hard_uart.h
 * @brief HAL for Hardware Uart
 *
 * Hardware abstraction layer for controlling hardware based uart
 *
 * @author Jonathan Clapson jonathan.clapson@gmail.com
 * @date 2014
 * @copyright GNU GENERAL PUBLIC LICENSE version 2
 */

#ifndef __PIC_HARD_UART_H__
#define __PIC_HARD_UART_H__

#include <stdint.h>

int hard_uart_putc_nb(uint8_t data);
int hard_uart_putc_bl(uint8_t data);

int hard_uart_getc_bl(uint8_t *data);
int hard_uart_getc_nb(uint8_t *data);

int hard_uart_puts_bl(uint8_t *data);

void hard_uart_init();

#endif /* __PIC_HARD_UART_H__ */
