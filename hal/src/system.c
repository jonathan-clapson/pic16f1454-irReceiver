#include <xc.h>
#include <system.h>

int system_init()
{
#if SYS_FREQ == 48000000
	OSCCONbits.IRCF = 0b1111; /* 0b1111 = 16MHz HFINTOSC postscalar */
#endif
	return 0;
}
