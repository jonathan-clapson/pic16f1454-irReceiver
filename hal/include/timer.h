/**
 * @file timer.h
 * @brief HAL for hardware timers
 *
 * Provides functions to control hardware timers
 *
 * @author Jonathan Clapson jonathan.clapson@gmail.com
 * @date 2014
 * @copyright GNU GENERAL PUBLIC LICENSE version 2
 */

#ifndef __PIC_TIMER_H__
#define __PIC_TIMER_H__

typedef union timer_time {
	uint8_t byte[2];
	uint16_t word;
};

void clock_init();
void timer1_init_1us();
void timer0_init_1us();
uint8_t timer0_get_time();
uint16_t timer1_get_time();
void usleep(uint8_t num);
void t1_usleep(uint8_t num);

#endif /* __PIC_TIMER_H__ */
