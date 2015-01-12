#include <system.h>

#include <string.h>
#include <uart.h>

#include "timer.h"
#include "ir.h"

// timing definitions
#define LOW_PASS_FILTER 400

// to calculte timings, use (ms)*3
#define IR_TIME_AGC_BURST_MAX 33000
#define IR_TIME_AGC_BURST_MIN 24000
#define IR_TIME_SPACE_MAX 24000
#define IR_TIME_SPACE_MIN 9000
#define IR_TIME_DATA_BURST_MAX 6000
#define IR_TIME_DATA_BURST_MIN 600
#define IR_TIME_DATA_HIGH_MAX 9000
#define IR_TIME_DATA_HIGH_MIN 3000
#define IR_TIME_DATA_LOW_MAX 3000
#define IR_TIME_DATA_LOW_MIN 600

enum ir_state_t {
	IR_WAIT_AGC = -2,
	IR_WAIT_SPACE = -1,
	IR_DATA = 0,
};

// buffers pointers used for storing and processing data
volatile struct buf_t ir_raw_buffers[2];
volatile struct buf_t *ir_isr_buf;
volatile struct buf_t *ir_process_buf;
volatile uint8_t switch_req; // main routine must request switching so buffer isn't switched when ir is half done.

/*
 * Informs the ir routine that receiver is ready for a swap
 */
void ir_buffer_request_swap()
{
	switch_req = 1;
}

/*
 * Returns false if a swap is requested, true otherwise
 */
uint8_t ir_buffer_swapped()
{
	return !switch_req;
}

/*
 * Clears the isr buffer
 */
void ir_isr_buffer_clear()
{
	ir_isr_buf->len = 0;
	ir_isr_buf->repeat = 0;
}

/*
 * Swaps the IR buffers, this should only be called from within isr, right before a reset
 */
void ir_buffer_swap()
{
	volatile struct buf_t *tmp = ir_process_buf;
	ir_process_buf = ir_isr_buf;
	ir_isr_buf = tmp;

	// when we swap buffers we reset our new buffer
	ir_isr_buffer_clear();
}

/*
 * write to bitwise storage
 */
void bitStoreWrite(uint8_t *bitStore, uint8_t bitIndex, uint8_t val)
{
	if (val)
		bitStore[bitIndex>>3] |= (0x80 >> (bitIndex & 0x7));
	else
		bitStore[bitIndex>>3] &= ~(0x80 >> (bitIndex & 0x7));
}

volatile uint8_t bit_data[4];
volatile uint8_t flag = 0;
volatile uint8_t bit_data_len = 0;

/*
 * Initialise ir algorithm
 */
void ir_init()
{
	memset(&ir_raw_buffers, 0, sizeof(ir_raw_buffers));
	ir_isr_buf = &(ir_isr_buf[0]);
	ir_process_buf = &(ir_isr_buf[1]);

	// enable positive edge detection on ir pin
	IOCAP5 = 1;
	// enable negative edge detection on ir pin
	IOCAN5 = 1;
}

/*
 * ir_isr_detect_start_code detects the start conditions of the signal
 *
 * if an agc burst happens (in ANY state), start waiting for the gap
 * if the gap occurs, check its length, length signals either repeat or start of data
 * if data has started then clock it in while verifying gap lengths
 *
 * return 1 if need to reset timer, 0 otherwise
 */
uint8_t ir_isr_detect_start_code(enum ir_state_t *ir_state, uint16_t time_ellapsed, uint8_t pin_state)
{
	// Detect if an AGC burst happens
	if ( (time_ellapsed > (uint16_t) IR_TIME_AGC_BURST_MIN) && (!pin_state) ) {
		*ir_state = IR_WAIT_SPACE;
		DEBUG_MSG("w");
		return 1;
	}

	if ( *ir_state != IR_WAIT_SPACE ) return 0;

	if ( (time_ellapsed < IR_TIME_SPACE_MAX) && (time_ellapsed > IR_TIME_SPACE_MIN) && (pin_state) ) {
		// data state expects a clean buffer
		ir_isr_buffer_clear();
		*ir_state = IR_DATA;
		DEBUG_MSG("d");
		return 1;
	} else if (pin_state) {
		// Not checking repeat timing, if its not data, assume its repeat.
		// Flag the repeat to the isr.
		ir_isr_buf->repeat = 1;
		return 0;
	}
	return 0;
}

void ir_isr()
{
	static int last_low_pass_time = 0;

	static char rc2_val = 0;
	// check if this is an interrupt on change interrupt
	if (!IOCIF || !IOCIE) return;

	//sample ra5
	static uint8_t ra5_last = 0;
	uint8_t ra5;
	do {
		ra5 = RA5;
	} while (ra5 != RA5);
	// invert to show actual signal
	ra5	= (ra5)?0:1;

	//flip rc2
	rc2_val = rc2_val?0:1;
	LATC2=rc2_val;

	static enum ir_state_t ir_state = IR_WAIT_AGC;
	// pin state has changed
	if (IOCAF5) {
		LATA4=1;
		uint16_t ellapsed = timer_get_clocks_16bit();

		// check for overflow, if a signal hasn't been received in a while we should reset
		if ( timer_overflow() || (ellapsed > IR_TIME_AGC_BURST_MAX) ) {
			DEBUG_MSG("\r\nr");
			goto reset_ir_isr;
		}

		if (ir_isr_detect_start_code(&ir_state, ellapsed, ra5)) {
			goto reset_timer;
		}

		// check if the initial processing detected a repeat code, if so, see if ir data processing is ready to take it.
		if (ir_isr_buf->repeat) {
			DEBUG_MSG("rpt\r\n");
			if (switch_req) {
				ir_buffer_swap();
				switch_req = 0;
			}
			goto reset_ir_isr;
		}

		// if the start process hasn't shifted us into DATA mode then don't do anything more.
		if (ir_state != IR_DATA) goto reset_timer;

		// Begin Data Processing :D

		// If we've already got all bits then spin here until new IR Start detected or mains ready to swap
		/*if (ir_isr_buf->len >= IR_BITS_PER_TRANSMISSION) {
			flag = 1;
			if (switch_req) {
				ir_buffer_swap();
				ir_state = IR_WAIT_AGC;
			}
			DEBUG_MSG("done\r\n");
			goto reset_timer;
		}*/

		if (bit_data_len >= IR_BITS_PER_TRANSMISSION) {
			flag = 1;

			goto reset_timer;
		}

		// if the time since the last data is very short, then low pass filter it. Currently getting one weird bit that does its own thing :S (maybe its noise?)
		uint16_t diff = ellapsed - last_low_pass_time;
		if (diff < LOW_PASS_FILTER) {
			last_low_pass_time = ellapsed;
			goto clear_interrupt;
		}

		// if we get a falling edge check timing is correct then reset timer
		if ( !ra5 ) {
			if ( (ellapsed<IR_TIME_DATA_BURST_MIN) || (ellapsed > IR_TIME_DATA_BURST_MAX) ) {
				// we've lost sync :S
				DEBUG_MSG("IR DATA: burst sync error");
				//goto reset_ir_isr;
				goto reset_timer;
			}
			DEBUG_MSG("g");
			goto reset_timer;
		} else { // this is data! :D
			if ( (ellapsed<IR_TIME_DATA_HIGH_MAX) && (ellapsed>IR_TIME_DATA_HIGH_MIN) ) {
				//ir_isr_buf->len=0;
				//ir_isr_buf->buf[ir_isr_buf->len++] = 1;

				if ((bit_data_len < IR_BITS_PER_TRANSMISSION) && (!flag))
					bitStoreWrite(bit_data, bit_data_len++, 1);

				LATA4=1;
				uart_puts("1");
				//uart_putc(bit_data[0]);
				DEBUG_MSG("1");
				goto reset_timer;
			} else if ( (ellapsed<IR_TIME_DATA_LOW_MAX) && (ellapsed>IR_TIME_DATA_LOW_MIN) ) {
				//ir_isr_buf->len=0;
				//ir_isr_buf->buf[ir_isr_buf->len++] = 0;

				if ((bit_data_len < IR_BITS_PER_TRANSMISSION) && (!flag))
					bitStoreWrite(bit_data, bit_data_len++, 0);

				LATA4=0;
				uart_puts("0");
				//uart_putc(bit_data[0]);
				DEBUG_MSG("0");
				goto reset_timer;
			} else { // bit is wrong size!
				DEBUG_MSG("IR DATA: bit length error");
				goto reset_ir_isr;
			}
		}

		goto reset_timer;

reset_ir_isr:
		ir_isr_buffer_clear();
		ir_state = IR_WAIT_AGC;

reset_timer:
		timer_reset();
		last_low_pass_time = 0;

clear_interrupt:
		LATA4=0;
		// clear the interrupt
		IOCAF = 0;
	} else {
		uart_puts("BAD ISR!\r\n");
	}
}
