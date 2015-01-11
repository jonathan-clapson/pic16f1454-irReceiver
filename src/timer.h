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
#ifndef __TIMER_H__
#define __TIMER_H__

#include <xc.h>
#include <stdint.h>

uint16_t timer_get_clocks_16bit();
void timer_init();
void timer_reset();
uint8_t timer_overflow();

#endif /* __TIMER_H__ */
