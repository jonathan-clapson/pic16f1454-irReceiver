#ifndef __IRQ_H__
#define __IRQ_H__

/* toplevel interrupt allocation */
enum irq_toplevel {
	IRQ_TIMER0 = 0, //TMR0
	IRQ_EXTERNAL, //INT
	IRQ_INT_ON_CHANGE, //IOCI
	IRQ_PERIPH1,
	IRQ_PERIPH2
};

/* pir1 interrupt allocation */
enum irq_PIR1 {
	IRQ_TMR1_GATE = 5, //TMR1G
	IRQ_ADC, //ADIF
	IRQ_UART_RX, //RCIF
	IRQ_UART_TX, //TXIF
	IRQ_SSP1, //SSP1IF
	IRQ_TMR2, //TMR2
	IRQ_TMR1, //TMR1
};
#define IRQ_PIR1_START IRQ_TMR1_GATE

/* pir2 interrupt allocation */
enum irq_PIR2 {
	// begin PIR1 interrupts
	IRQ_OSC_FAIL = 12, //OSF
	IRQ_OSC_NUM_CONT2, //C2
	IRQ_OSC_NUM_CONT1, //C1
	IRQ_SSP1_BUS_COLLISION, //BCL1
	IRQ_USB, //USB
	IRQ_ACTIVE_CLOCK_TUNE //ACT
};
#define IRQ_PIR2_START	IRQ_OSC_FAIL
#define IRQ_PIR2_END	IRQ_ACTIVE_CLOCK_TUNE

/* ERROR definitions */
#define IRQ_ERR_SUCCESS 0
#define IRQ_ERR_OUT_OF_RANGE -1
#define IRQ_ERR_ALREADY_REGISTERED -2
#define IRQ_ERR_NOT_REGISTERED -3

void irq_init();
void interrupt interrupt_handler (void);
int irq_register_handler (int irq, void (*irq_handler_func)(void));
int irq_free_handler (int irq);

#endif __IRQ_H__
