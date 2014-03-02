#include <stdint.h>
#include "pic14/pic16f1454.h"

unsigned int __at(_CONFIG1) configWord1 =
        _FOSC_INTOSC &
         _WDTE_OFF &
        _PWRTE_OFF &
        _MCLRE_OFF &
        _CP_OFF &
        _BOREN_ON &
        _CLKOUTEN_OFF &
        _IESO_OFF &
        _FCMEN_ON;

unsigned int __at(_CONFIG2) configWord2 = 0x3fff & (~(1<<8));

#define Baudrate              1                      //bps
//#define OneBitDelay           (1000000/Baudrate)
#define OneBitDelay           (3333)
#define DataBitCount          8         // no parity, no flow control
#define UART_RX               RC5	// UART RX pin
#define UART_TX               RC4	// UART TX pin

typedef union timer_time {
	char high_byte, low_byte;
	unsigned int word;
};

void clock_init(void)
{
        //clock is 16MHz
        SCS1 = 0;
        SCS0 = 0;
        IRCF3 = 1;
        IRCF2 = 1;
        IRCF1 = 1;
        IRCF0 = 1;
}

void timer1_init_1us(void)
{
	//set timer 1 to use Fosc/4
	TMR1CS1 = 0;
	TMR1CS0 = 0;

	//further divide by 4
	T1CKPS1 = 1;
	T1CKPS0 = 0;

	//clock is now 500KHz

	//enable timer
	TMR1ON = 1;
}

void usleep(unsigned int num)
{
	union timer_time current_time;

	/* reset timer */
	TMR1L = 0x00;
	TMR1H = 0x00;

	/* loop until timer expires */
	do {
		current_time.low_byte = TMR1L;
		current_time.high_byte = TMR1H;
	} while (current_time.word < num);
}

//Function Declarations
void InitSoftUART(void);
unsigned char UART_Receive(void);
void UART_Transmit(const char);

void InitSoftUART(void)		// Initialize UART pins to proper values
{
	/* UART TX Pin is high when idle */
	UART_TX = 1;			
//	int b;
//	b = PORTC;
//	b ^= (1<<4);
//	PORTC = b;
	/* configure uart tx pin as output */
	TRISC4 = 0;
	/*configure uart rx pin as input */
	TRISC5 = 1;
}

unsigned char UART_Receive(void)
{
	// Pin Configurations
	// GP1 is UART RX Pin

	unsigned char DataValue = 0;
	unsigned char i;

	//wait for start bit
	while(UART_RX==1);

	usleep(OneBitDelay);
	usleep(OneBitDelay/2);   // Take sample value in the mid of bit duration

	for ( i = 0; i < DataBitCount; i++ ) {
		if ( UART_RX == 1 ) { //if received bit is high
			DataValue += (1<<i);
		}
		usleep(OneBitDelay);
	}

	// Check for stop bit
	if ( UART_RX == 1 ) {      //Stop bit should be high
		usleep(OneBitDelay/2);
		return DataValue;
	} else {   //some error occurred !
		usleep(OneBitDelay/2);
		return 0x000;
	}
}

void UART_Transmit(const char DataValue)
{
	/* Basic Logic TX pin is usually high. 
	A high to low bit is the starting bit and a low to high bit is 
	the ending bit. No parity bit. No flow Control.
	BitCount is the number of bits to transmit. Data is 
	transmitted LSB first. 	*/

/*	unsigned char i;

	// Send Start Bit
	UART_TX = 0;
	usleep(OneBitDelay);

	for ( i = 0; i < DataBitCount; i++ ) {
		//Set Data pin according to the DataValue
		if((DataValue>>i) &0x1) {  //if Bit is high
			UART_TX = 1;
		} else {     //if Bit is low
			UART_TX = 0;
		}
	    	usleep(OneBitDelay);
	}

	//Send Stop Bit
	UART_TX = 1;
	usleep(OneBitDelay);*/
}

void main()
{
	unsigned char ch = 0;
	unsigned char rc_val = 1;
	unsigned char i;

	InitSoftUART();

	TRISC2 = 0;
	RC2 = rc_val;

	clock_init();
	timer1_init_1us();

	while(1){
//		UART_Transmit(0x65);
		for (i=0; i<100; i++)
			usleep(10000);
	
		rc_val = !rc_val;
		RC2 = rc_val;
	}
}
