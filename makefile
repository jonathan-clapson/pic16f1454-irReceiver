PIC=16f1454
CFLAGS=--std-c99 --use-non-free -mpic14 -p$(PIC)
INCDIR="./"

UART=PIC_USE_HARD_UART

all: soft_uart_test hard_uart_test timer_test

# support objects
soft_uart_obj: timer_obj
	sdcc $(CFLAGS) -I$(INCDIR) -DPIC_USE_SOFT_UART -c uart/soft_uart.c
	
hard_uart_obj:
	sdcc $(CFLAGS) -I$(INCDIR) -DPIC_USE_HARD_UART -c uart/hard_uart.c

timer_obj:
	sdcc $(CFLAGS) -I$(INCDIR) -c timer/timer.c

# test binaries
soft_uart_test: soft_uart_obj
	sdcc $(CFLAGS) -I$(INCDIR) -DPIC_USE_SOFT_UART -c soft_uart_test.c
	sdcc $(CFLAGS) -DPIC_USE_SOFT_UART soft_uart_test.o soft_uart.o timer.o

hard_uart_test: hard_uart_obj
	sdcc $(CFLAGS) -I$(INCDIR) -DPIC_USE_HARD_UART -c hard_uart_test.c
	sdcc $(CFLAGS) -DPIC_USE_HARD_UART hard_uart_test.o hard_uart.o

timer_test: timer_obj
	sdcc $(CFLAGS) -I$(INCDIR) -c timer_test.c
	sdcc $(CFLAGS) timer_test.o timer.o


#to build multiple files, need to use sdcc -c which will build some sort of temporary intermediate
# sdcc -c foo1.c
# sdcc -c foo2.c
# sdcc -c foomain.c
# sdcc foomain.rel foo1.rel foo2.rel

clean:
	rm *.o
	rm *.hex
	rm *.asm
	rm *.cod
	rm *.lst
#	rm soft_uart_test hard_uart_test timer_test
