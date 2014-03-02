LIBDIRS=/usr/share/sdcc/non-free/lib/pic14
INCDIRS=/usr/share/sdcc/non-free/include

all: soft_uart hard_uart timer

soft_uart:
	sdcc --std-c99 -I$(INCDIRS) -L$(LIBDIRS) -mpic14 -p16f1454 -o soft_uart soft_uart.c

hard_uart:
	sdcc --std-c99 -I$(INCDIRS) -L$(LIBDIRS) -mpic14 -p16f1454 -o hard_uart hard_uart.c

timer:
	sdcc --std-c99 -I$(INCDIRS) -L$(LIBDIRS) -mpic14 -p16f1454 -o timer timer.c

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
	rm *.rel
	rm soft_uart hard_uart timer
