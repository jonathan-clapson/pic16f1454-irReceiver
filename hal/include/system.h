#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#define SYS_FREQ 48000000

#if defined(_16F1454) || defined(_16F1455) || defined(_16F1459)
#pragma config FOSC = INTOSC
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config MCLRE = ON
#pragma config CP = OFF
#pragma config BOREN = ON
#pragma config CLKOUTEN = OFF
#pragma config IESO = OFF
#pragma config FCMEN = OFF
#pragma config WRT = OFF
#pragma config CPUDIV = NOCLKDIV
#pragma config USBLSCLK = 48MHz
#pragma config PLLMULT = 3x
#pragma config PLLEN = ENABLED
#pragma config STVREN = ON
#pragma config BORV = LO
#pragma config LPBOR = ON
#pragma config LVP = ON
#else
#error System specific details not set
#endif

int system_init();

#endif /* __SYSTEM_H__ */
