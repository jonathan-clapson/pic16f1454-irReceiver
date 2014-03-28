//#include "cstdlib/stdint.h"
#include <stdint.h>

#include "cstdlib/stdio.h"

#include <pic14/pic14regs.h>

#include "uart/uart.h"
#include "timer/timer.h"

#define USB_EP0_BUFFER_SIZE 8

volatile uint8_t pin_toggled = 0;

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

/* usb buffer setup */
typedef union {
	/* CPU MODE */
	struct {
		unsigned BC8 : 1;
		unsigned BC9 : 1;
		unsigned BSTALL : 1;
		unsigned DTSEN : 1;
		unsigned : 2;
		unsigned DTS : 1;
		unsigned UOWN : 1;
	};
	/* SIE MODE */
	struct {
		/* first two bits are already defined above */
		unsigned : 2;
		unsigned PID0 : 1;
		unsigned PID1 : 1;
		unsigned PID2 : 1;
		unsigned PID3 : 1;
		/* next bit is reserved, last bit is already defined above */
		unsigned : 2;
	};
	/* SIE MODE shortcut */
	struct {
		unsigned : 2;
		unsigned PID : 4;
		unsigned : 2;
	};
} __BDnSTATbits_t;

typedef struct {
	__BDnSTATbits_t STATbits;
	uint8_t CNT;
	uint16_t ADDR;
} __USB_BD_T;
__at(0x2000) volatile __USB_BD_T BD0;
__at(0x2004) volatile __USB_BD_T BD1;

__at(0x2070) volatile uint8_t usb_ep0_out_buf[USB_EP0_BUFFER_SIZE];
__at(0x20F0) volatile uint8_t usb_ep0_in_buf[USB_EP0_BUFFER_SIZE];

void uart_put_uint8(uint8_t val, int eol)
{
	uint8_t data;
	data = (val/100)%10;
	if (data)
		uart_putc(data+'0');
	data = (val/10)%10;
	if (data)
		uart_putc(data+'0');
	data = val%10;
	uart_putc(data+'0');
	if (eol) 
		uart_puts("\r\n");
}

void core_interrupt() {
	uart_puts("core interrupt should not be called!\n");
	return;
}

void service_transaction() {
	char tmp[10];
	uint8_t transaction_dir;
	uint8_t endpoint;
	transaction_dir = USTATbits.DIR;

	//endpoint = USTAT & (_ENDP0 | _ENDP1 | _ENDP2 | _ENDP3); //Ignoring ENDP3 as can't use snprintf and it makes bigger than 10
	//endpoint = endpoint >> 1;
	endpoint = USTATbits.ENDP;
	//PING PONG bit
	//PPBI
	uart_puts("st:");	
	uart_put_uint8(transaction_dir, 0);
	uart_putc(':');
	uart_put_uint8(endpoint, 1);
}

void service_usb_int() {
	uart_puts("int!\r\n");
	if (UIRbits.SOFIF) {
		uart_puts("usb SOFIF\r\n");
		// clear interrupt
		UIRbits.SOFIF = 0;
	} else if (UIRbits.STALLIF) {
		uart_puts("usb STALLIF\r\n");
		// clear interrupt
		UIRbits.STALLIF = 0;
	} else if (UIRbits.IDLEIF) {
		uart_puts("usb IDLEIF\r\n");
		// clear interrupt
		UIRbits.IDLEIF = 0;
	} else if (UIRbits.TRNIF) {
		service_transaction();
		uart_puts("usb TRNIF\r\n");
		UIRbits.TRNIF = 0;
	} else if (UIRbits.ACTVIF) {
		uart_puts("usb ACTVIF\r\n");
		UIRbits.ACTVIF = 0;
	} else if (UIRbits.UERRIF) {
		uart_puts("usb UERRIF\r\n");
		UIRbits.UERRIF = 0;
	} else if (UIRbits.URSTIF) {
		uart_puts("usb URSTIF\r\n");
		UIRbits.URSTIF = 0;
	} else {
		uart_puts("unhandled!\r\n");
	}
	return;
}

void setup_bd0()
{
	/* documentation states that BD0 is EP0 out in No ping pong mode */

	/* stat memory */
	BD0.STATbits.UOWN = 1; //cpu initially owns
	/* enable sync for now */
	BD0.STATbits.DTSEN = 1;
	/* Don't stall */
	BD0.STATbits.BSTALL = 0;

	/* set to store up to 20 bytes of packets */
	BD0.STATbits.BC9 = 0;
	BD0.STATbits.BC8 = 0;
	BD0.CNT = USB_EP0_BUFFER_SIZE;

	/* set starting memory address of buffer */
	BD0.ADDR = (volatile uint16_t) usb_ep0_out_buf;

}
void setup_bd1()
{
	/* documentation states that BD1 is EP0 in in No ping pong mode */
	BD1.STATbits.UOWN = 1; //mcu initially owns
	/* Enable sync */
	BD1.STATbits.DTS = 1;
	BD1.STATbits.DTSEN = 1;
	/* Don't stall */
	BD1.STATbits.BSTALL = 0;

	/* set to 1 bytes per packet */
	BD1.STATbits.BC9 = 0;
	BD1.STATbits.BC8 = 0;
	BD1.CNT = 0;

	/* set starting memory address of buffer */
	BD1.ADDR = (volatile uint16_t ) usb_ep0_in_buf;
}

static void isr(void) __interrupt 0
{
uart_puts("isr\n");
	/* check if a usb related interrupt occured */
	if (PIR2bits.USBIF)
		service_usb_int();
	else
		core_interrupt();
}

void usb_ep0_init()
{
	//disable stall
	UEP0bits.EPSTALL = 0;

	//enable handshaking
	UEP0bits.EPHSHK = 1;
	
	//enable input output and control transfers on endpoint 0
	UEP0bits.EPOUTEN = 1;
	UEP0bits.EPINEN = 1;
	UEP0bits.EPCONDIS = 0;

	//init buffer
	setup_bd0();
	setup_bd1();	
}

void usb_init()
{
	UCON = 0; //clear UCON
	UIE = 0; //disable all usb interrupts

	UCFGbits.UTEYE = 0; //disable eye testing
	UCFGbits.PPB = 0;
	UCFGbits.FSEN = 1; //enable full speed usb
	UCFGbits.UPUEN = 1; //enable pull up to indicate full speed mode to host

	UCONbits.PPBRST = 1; //reset ping pong buffers to point to even buffer descriptors
	//UCONbits.PKTDIS = 0; //not sure what to set this to? says its auto set to one when SETUP packet happens
	UCONbits.RESUME = 1; //dont pause signalling?
	UCONbits.SUSPND = 0; //usb suspend disabled

	UEIR = 0; //clear all usb error bits

	// enable all usb interrupts in UIE
	UIEbits.URSTIE = 1; // usb reset interrupt
	UIEbits.UERRIE = 1; //usb error interrupt
	UIEbits.ACTVIE = 1; //bus activity detected interrupt
	UIEbits.TRNIE = 1; //transaction complete interrupt
	UIEbits.IDLEIE = 1; //idle interrupt 
	UIEbits.STALLIE = 1; //stall interrupt
	UIEbits.SOFIE = 1; //start of frame token interrupt

	// set address to 0 when not configured
	//UADDR = 0;

	//enable usb interrupts through PIE2
	PIE2bits.USBIE = 1;

	//usb_ep0_init();

	UCONbits.USBEN = 1; //enable usb module
}

void usb_sleep()
{
}

void main(void)
{
	unsigned char i, cnt, state = 0;
	unsigned char buffer[4];
	unsigned char buffer2[8];

	uint16_t usb_reset_timer;
	uint16_t usb_connect_timer;

	clock_init(); //set clock to 16MHz

	uart_init();

	timer1_init_1us();

	//enable peripheral interrupts (usb is on this)
	INTCONbits.PEIE = 1;

	// enable interrupts globally
	INTCONbits.GIE = 1;

	t1_usleep(255);

	usb_init();
	uart_puts("usb init\r\n");

	usb_connect_timer = timer1_get_time();
	while (1) {
		if (UCONbits.SE0) {
			/* if SE0 occurs for more than 2.5us then the host is requesting to connect */
			if (timer1_get_time() > usb_connect_timer + 3)
			uart_puts("Connect requested\r\n");
		} else {
			usb_connect_timer = timer1_get_time();
		}
	if (UIRbits.SOFIF) {
		uart_puts("usb SOFIF\r\n");
		// clear interrupt
		UIRbits.SOFIF = 0;
	} else if (UIRbits.STALLIF) {
		uart_puts("usb STALLIF\r\n");
		// clear interrupt
		UIRbits.STALLIF = 0;
	} else if (UIRbits.IDLEIF) {
		uart_puts("usb IDLEIF\r\n");
		// clear interrupt
		UIRbits.IDLEIF = 0;
	} else if (UIRbits.TRNIF) {
		service_transaction();
		uart_puts("usb TRNIF\r\n");
		UIRbits.TRNIF = 0;
	} else if (UIRbits.ACTVIF) {
		uart_puts("usb ACTVIF\r\n");
		UIRbits.ACTVIF = 0;
	} else if (UIRbits.UERRIF) {
		uart_puts("usb UERRIF\r\n");
		UIRbits.UERRIF = 0;
	} else if (UIRbits.URSTIF) {
		uart_puts("usb URSTIF\r\n");
		UIRbits.URSTIF = 0;
	}

	}
}
