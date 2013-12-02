LIBDIRS=/usr/share/sdcc/non-free/lib/pic14
INCDIRS=/usr/share/sdcc/non-free/include

all:
	sdcc --std-c99 -I$(INCDIRS) -L$(LIBDIRS) -mpic14 -p16f1454 main.c 

clean:
	rm main.hex
	rm main.asm
	rm main.o
	rm main.cod
	rm main.lst
