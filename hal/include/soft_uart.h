/**
 * @file soft_uart.h
 * @brief HAL for Software Uart
 *
 * Hardware abstraction layer for controlling software based uart
 *
 * @author Jonathan Clapson jonathan.clapson@gmail.com
 * @date 2014
 * @copyright GNU GENERAL PUBLIC LICENSE version 2
 */

 
#ifndef __PIC_SOFT_UART_H__
#define __PIC_SOFT_UART_H__

#include <xc.h>
#include <stdint.h>

#define Baudrate	1	//bps
//#define OneBitDelay	(1000000/Baudrate)
#define OneBitDelay	(3333)
#define DataBitCount	8	// no parity, no flow control
#define UART_RX		RC5	// UART RX pin
#define UART_TX		RC4	// UART TX pin

void soft_uart_init();

int soft_uart_getc_bl(uint8_t *data);
int soft_uart_putc_bl(uint8_t data);

#endif /* __PIC_SOFT_UART_H__ */
