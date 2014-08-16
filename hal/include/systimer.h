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

#ifndef __SYSTIMER_H__
#define __SYSTIMER_H__

#include <timer.h>

#define SYSTIMER_ERR_SUCCESS	0
#define SYSTIMER_ERR_DISABLED	-1

void systimer_init();
void systimer_interrupt();
int8_t systimer_get_time(struct timer_t *sys_timer);
//void usleep_nb(uint8_t num);

#endif /* __SYSTIMER_H__ */
