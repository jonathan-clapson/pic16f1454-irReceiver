#ifndef __PIC_HARD_UART_H__
#define __PIC_HARD_UART_H__

#include <stdint.h>
#include "pic14/pic16f1454.h"

int hard_uart_putc_nb(uint8_t data);
int hard_uart_putc_bl(uint8_t data);

int hard_uart_getc_bl(uint8_t *data);
int hard_uart_getc_nb(uint8_t *data);

void hard_uart_init();

#endif /* __PIC_HARD_UART_H__ */
