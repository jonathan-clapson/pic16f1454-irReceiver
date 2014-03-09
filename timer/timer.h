#ifndef __PIC_TIMER_H__
#define __PIC_TIMER_H__

typedef union timer_time {
	char high_byte, low_byte;
	unsigned int word;
};

void clock_init();
void timer1_init_1us();
void timer0_init_1us();
uint8_t timer0_get_time();
void usleep(uint8_t num);

#endif /* __PIC_TIMER_H__ */
