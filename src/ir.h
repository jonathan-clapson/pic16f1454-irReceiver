#ifndef __IR_H__
#define __IR_H__

#include <string.h>
#include <uart.h>

#define IR_BITS_PER_TRANSMISSION 32

volatile struct buf_t {
	volatile uint8_t buf[IR_BITS_PER_TRANSMISSION];
	volatile uint8_t len;
	volatile uint8_t repeat;
};

void ir_init();
void ir_isr();

// Struct which holds the buffer data for non-interrupt routines to use
volatile struct buf_t *ir_process_buf;
// Request a swap from interrupt routine, interrupt will fill with next complete set of IR data
void ir_buffer_request_swap();
// Returns true if data buffers have been swapped
uint8_t ir_buffer_swapped();

extern volatile uint8_t data[4];
extern volatile uint8_t flag;
extern volatile uint8_t data_len;

#endif // __IR_H__

