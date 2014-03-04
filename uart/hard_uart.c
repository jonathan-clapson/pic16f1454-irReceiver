#include <stdint.h>
#include "pic14/pic16f1454.h"

void hard_uart_tx_init()
{
	//don't invert tx
	SCKP = 0;

//	TRISC4 = 0;
//	TRISC5 = 1;

	//Make sure interrupt is disabled
	TXIE = 0;
	//Disable 9 bit transmission
	TX9 = 0;
	//disable sending break
	SENDB = 0;

	//enable eusart
	SPEN=1;

	//enable asynchronous transmission
	TXEN = 1;
}

void hard_uart_rx_init()
{
	//disable RX
	CREN = 0;
}

void hard_uart_baud_init()
{
	BRG16 = 0;

	//set baud rate to 9600Hz
	SPBRGL = 25;
	SPBRGH = 0;	
	
	//set low baud rate modes
	BRGH = 0;

	//set asynchronous mode
	SYNC = 0;
}

void hard_uart_init()
{
	hard_uart_baud_init();
	hard_uart_tx_init();	
	hard_uart_rx_init();
}

int hard_uart_putc_nb(uint8_t data)
{
	/* if TRMT bit is clear then transmit */
	if (TRMT) {
		TXREG = data;
		return 0;
	}
	return -1;	
}

int hard_uart_putc_bl(uint8_t data)
{
	while (!TRMT);
	TXREG = data;
	return 0;
}

int hard_uart_getc_bl(uint8_t *data)
{
	*data = 0;
	return -1;
}

int hard_uart_getc_nb(uint8_t *data)
{
	*data = 0;
	return -1;
}
