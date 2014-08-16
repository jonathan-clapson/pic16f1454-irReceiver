#include <timer.h>

int8_t timer_compare(struct timer_t *t1, struct timer_t *t2)
{
	if (t1->ms > t2->ms) return TIMER_GREATER_THAN;
	if (t1->ms < t2->ms) return TIMER_LESS_THAN;
	if (t1->us > t2->us) return TIMER_GREATER_THAN;
	if (t1->us < t2->us) return TIMER_LESS_THAN;
	return TIMER_EQUAL;
}
