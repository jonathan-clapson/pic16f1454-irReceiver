/**
 * @file irq.c
 * @brief IRQ handling on pic16f145(4/5/9)
 *
 * IRQ's are added to a list, when an interrupt occurs
 * the IRQ handlers from the list are called to handle it
 *
 * @author Jonathan Clapson jonathan.clapson@gmail.com
 * @date 2014
 * @copyright GNU GENERAL PUBLIC LICENSE version 2
 */

#include <xc.h>
#include <string.h>
#include <irq.h>
#include <uart.h>
void (*irq_handlers[17]) ( void );

/*
 * Initialise irq functionality
 */
void irq_init()
{
	memset(irq_handlers, 0, sizeof(irq_handlers));
}

/*
 * Dispatch irq's to irq handler
 */
void interrupt interrupt_handler (void)
{
	int i;
	//check toplevel interrupts
	for (i=0; i<IRQ_PIR1_START; i++)
		if (irq_handlers[i]) { irq_handlers[i]();  }


	//check PIR1 if needed
	if (PIR1) {
		for (i=IRQ_PIR1_START; i<IRQ_PIR2_START; i++)
			if (irq_handlers[i]) irq_handlers[i]();
	}

	//check PIR2 if needed
	if (PIR2) {
		for (i=IRQ_PIR2_START; i<=IRQ_PIR2_END; i++)
			if (irq_handlers[i]) irq_handlers[i]();
	}
}

/*
 * Add IRQ handler to list
 */
int irq_register_handler (int irq, void (*irq_handler_func)(void))
{
	if (irq>IRQ_PIR2_END)
		return IRQ_ERR_OUT_OF_RANGE;

	if (irq_handlers[irq] != NULL)
		return IRQ_ERR_ALREADY_REGISTERED;

	uart_puts("reg\n");

	irq_handlers[irq] = irq_handler_func;

	return IRQ_ERR_SUCCESS;
}

/*
 * Remove IRQ handler from list
 */
int irq_free_handler (int irq)
{
	if (irq_handlers[irq] == NULL)
		return IRQ_ERR_NOT_REGISTERED;

	irq_handlers[irq] = NULL;

	return IRQ_ERR_SUCCESS;
}
