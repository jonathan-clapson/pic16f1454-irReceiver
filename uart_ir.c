/**
 * @file uart_ir.c
 * @brief Transmit received IR via UART
 *
 * Program which receives NEC IR commands and transmits
 * them to host device via uart
 *
 * @author Jonathan Clapson jonathan.clapson@gmail.com
 * @date 2014
 * @copyright GNU GENERAL PUBLIC LICENSE version 2
 */

#include <stdint.h>

#include "cstdlib/stdio.h"

#include "pic14/pic16f1454.h"

#include "uart/uart.h"
#include "timer/timer.h"

volatile uint8_t pin_toggled = 0;

/* timing definitions */
#define IR_AGC_BURST_MAX 11000
#define IR_AGC_BURST_MIN 7000
#define IR_SPACE_MAX 7000
#define IR_SPACE_MIN 3000
#define IR_GAP_MAX 3000
#define IR_GAP_MIN 200
#define IR_HIGH_MAX 3000
#define IR_HIGH_MIN 1000
#define IR_LOW_MAX 1000
#define IR_LOW_MIN 200
/* if something is expected but not recieved after this time reset */
#define IR_RESET_TIMEOUT 50000

/* repeat timings - the repeat functionality really doesn't seem to work to the specs on sbprojects.com */
/* repeat gap after agc burst is 2250uS */
#define IR_REPEAT_DELAY_MIN 1500 
#define IR_REPEAT_DELAY_MAX 3000 /* if a repeat happens it shows up in the space state upper cap needs to be same as space */
/* repeat then has a GAP */
//#define IR_REPEAT_BURST_MIN IR_GAP_MIN
//#define IR_REPEAT_BURST_MAX IR_GAP_MAX
/* apparently to this remote this delay is substantially longer than GAP...*/
#define IR_REPEAT_BURST_MIN IR_GAP_MIN
#define IR_REPEAT_BURST_MAX 60000
/* and we wait for 110ms */
/* apparently to this remote 110ms is 9ms... */
#define IR_REPEAT_MIN 7000
#define IR_REPEAT_MAX 65000 //this should be around 120ms

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

/* store ir messages */
union ir_rcv_buffer_t {
	struct parts {
		uint8_t addr;
		uint8_t inv_addr;
		uint8_t cmd;
		uint8_t inv_cmd;
		uint8_t new_value;
	} parts;
	uint8_t buffer[5];
};
union ir_rcv_buffer_t ir_rcv_buffer;

#define BIT_LOW 0
#define BIT_HIGH 1
#define BIT_ERR -1

#define IR_MSG_COMPLETE 1
#define IR_MSG_INCOMPLETE 0
#define IR_MSG_ERR -1

static void isr(void) __interrupt 0
{
	if ( IOCAF5 == 1 ) {
		/* handle */
		pin_toggled = 1;
	
		IOCAF5 = 0;
	}
}
enum ir_state_t {
	reset = 0,
	wait_agc,
	wait_space,
	wait_addr_bits,
	wait_inv_addr_bits,
	wait_cmd_bits,
	wait_inv_cmd_bits,
	wait_repeat_burst,
	wait_repeat
};
enum ir_state_t ir_state = reset;

enum ir_bit_state_t {
	wait_gap = 0,
	wait_data
};
enum ir_bit_state_t ir_bit_state = wait_gap;

void ir_reset()
{
	ir_state = reset;
	ir_bit_state = wait_gap;
}

int ir_get_bit(int time) {
	if (time < IR_HIGH_MAX && time > IR_HIGH_MIN) /* bit is 1 */
		return BIT_HIGH;
	else if (time < IR_LOW_MAX && time > IR_LOW_MIN) /* bit is 0 */
		return BIT_LOW;
	else /* bit is invalid */
		return BIT_ERR;	
}

void set_bit(uint8_t *byte, int pos)
{
	*byte |= (1 << pos);	
}
void clear_bit(uint8_t *byte, int pos)
{
	*byte &= ~(1 << pos);
}

/* I wanted this to be static in set_ir_bit, however sdcc doesn't seem to initialise static variables... */
int ir_bit_counter = 0;

int set_ir_bit(int bit)
{
	/* get the byte */
	int offset = ir_bit_counter/8;

	if (bit != 0 && bit != 1)
		return IR_MSG_ERR;

	/* set the bit */
	if (bit){
		set_bit(&(ir_rcv_buffer.buffer[offset]), ir_bit_counter%8);
//		uart_putc('1');
	} else {
		clear_bit(&(ir_rcv_buffer.buffer[offset]), ir_bit_counter%8);
//		uart_putc('0');
	}

	/* increment the ir counter */
	ir_bit_counter ++;
	/* check if message is complete */
	if (ir_bit_counter < 32)
		return IR_MSG_INCOMPLETE;
	/* message is complete, reset counter. Indicate its done */
	ir_bit_counter = 0;
	return IR_MSG_COMPLETE;
}

void main(void)
{
	char out[20];
	uint8_t ra5_val;
	uint16_t temp16;
	uint16_t time, old_time, diff_time;
	int toggle = 0;

	INTCON = 0;

	/* Ensure RA5 isn't being used for clock input */
//	P2SEL = 0;

	/* set porta5 to input */
	TRISA5 = 1;

	/* set cpu clock */
	clock_init();

	/* init peripherals */
	timer0_init_1us();
	timer1_init_1us();
	uart_init();

	/* enable interrupt on change on RA5 */
	IOCIE = 1;
	/* enable positive edge detection */
	IOCAP5 = 1;
	/* enable negative edge detection */
	IOCAN5 = 1;

	/* enable interrupts */
	GIE = 1;

	TRISC2 = 0;

	/* initialise new value flag */
	ir_rcv_buffer.parts.new_value = 0;
	/* initialise old time */
	old_time = timer1_get_time();

	//Loop forever
	while ( 1 )
	{
		/* wait for any part of an ir message
		 * if nothing comes before timeout expires
		 * then reset the ir communications 
		 */
		while (pin_toggled == 0) {
			time = timer1_get_time();
			diff_time = time - old_time;
			if (diff_time > IR_RESET_TIMEOUT) {
				ir_reset();
				ir_bit_counter = 0;
				old_time = time;
			}
			if (ir_rcv_buffer.parts.new_value) {
				temp16 = ir_rcv_buffer.parts.addr;
				snprintf(out, sizeof(out), "addr: %u\r\n", &temp16);
				uart_puts(out);
				temp16 = ir_rcv_buffer.parts.inv_addr;
				snprintf(out, sizeof(out), "i_addr: %u\r\n", &temp16);
				uart_puts(out);
				temp16 = ir_rcv_buffer.parts.cmd;
				snprintf(out, sizeof(out), "cmd: %u\r\n", &temp16);
				uart_puts(out);
				temp16 = ir_rcv_buffer.parts.inv_cmd;
				snprintf(out, sizeof(out), "i_cmd: %u\r\n", &temp16);
				uart_puts(out);
				ir_rcv_buffer.parts.new_value = 0;
			}
		}
		time = timer1_get_time();
		diff_time = time - old_time;
		
		if (ir_state == reset) {
			ra5_val = RA5;
			// check ra5_val is 0, if so its hopefully start of agc
			if (ra5_val == 0) {
				ir_state = wait_agc;
			} else {
				ir_state = reset;
			}
		} else if (ir_state == wait_agc) {
			/* check if its agc burst */
			if (diff_time < IR_AGC_BURST_MAX && diff_time > IR_AGC_BURST_MIN) {ir_state = wait_space; }
			else { ir_reset(); uart_puts("called reset from wait_agc\n"); 
snprintf(out, sizeof(out), "t:%u\r\n", &diff_time);
uart_puts(out);
			}
		} else if (ir_state == wait_space) {
			/* check if its a space */
			if (diff_time < IR_SPACE_MAX && diff_time > IR_SPACE_MIN) { ir_state = wait_addr_bits; }
			else if (diff_time < IR_REPEAT_DELAY_MAX && diff_time > IR_REPEAT_DELAY_MIN) { ir_state = wait_repeat_burst; }
			else { ir_reset(); uart_puts("called reset from wait_space\n"); }
		} else if (ir_state == wait_addr_bits) {
			if (ir_bit_state == wait_gap) {
				/* check if its a gap */
				if (diff_time < IR_GAP_MAX && diff_time > IR_GAP_MIN) { ir_bit_state = wait_data; }
				else { ir_reset(); uart_puts("gap expected\n"); }
			} else { /* this is the 1/0 indicator */
				int bit = ir_get_bit(diff_time);
				/* check if message is complete or theres an error */
				switch(set_ir_bit(bit)) {
					case IR_MSG_INCOMPLETE:
						ir_bit_state = wait_gap;
						break;
					case IR_MSG_COMPLETE:
						ir_reset();
						ir_rcv_buffer.parts.new_value = 1;
						break;
					default: /* error */
						ir_reset();
						uart_puts("bit expected\n");
						break;
				}
			}

		/* repeat handling */			
		} else if (ir_state == wait_repeat_burst ) {
			ir_rcv_buffer.parts.new_value = 1;
			if (diff_time < IR_REPEAT_BURST_MAX && diff_time > IR_REPEAT_BURST_MIN) { ir_state = wait_repeat; }
			else { ir_reset(); uart_puts("repeat burst error\n"); }
		} else if (ir_state == wait_repeat) {
			if (diff_time < IR_REPEAT_MAX && diff_time > IR_REPEAT_MIN) { ir_reset(); }
			else { ir_reset(); uart_puts("repeat error\n"); }
		} else {
			ir_reset();
			uart_puts("state error\n");
		}
		
		old_time = time;
		pin_toggled = 0;
 	}
}
