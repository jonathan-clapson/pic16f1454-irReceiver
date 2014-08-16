#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdint.h>

struct timer_t {
	uint16_t ms;
	uint16_t us;
};

#define TIMER_GREATER_THAN 1
#define TIMER_LESS_THAN -1
#define TIMER_EQUAL 0

int8_t timer_compare(struct timer_t *t1, struct timer_t *t2);

#endif /* __TIMER_H__ */
