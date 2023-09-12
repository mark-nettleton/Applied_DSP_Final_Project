#include "timer.h"
#include "DSCInit.h"
#include "p33FJ256GP506.h"

volatile unsigned int timer1Count = 0;
volatile int flagLED = 0;

/* Interrupt handler for timer1. 
Used for 1 sec interval between pulses */
void __attribute__((__interrupt__,no_auto_psv)) _T1Interrupt(void)
{
	// Change the RED LED status each second.
	if (timer1Count++ % 10 == 0)
	{
		if (flagLED == 1)
		{
			RED_LED = SASK_LED_OFF; // Turn off the LED.
			flagLED = 0;
		}	
		else
		{
			RED_LED = SASK_LED_ON; // Turn on the LED.	
			flagLED = 1;
		}
	}
	IFS0bits.T1IF = 0; // Clear the interrupt flag.
}

// Configure T1 as a periodic timer and load the duration period.
void timer1Init(unsigned int count)
{	
	IPC0bits.T1IP = 1;      // Priority level is 1.
	IEC0bits.T1IE = 1;      // Timer1 interrupt enabled.
	PR1   = count;          // Interrupt period.
	T1CONbits.TCS = 0;      // Select the internal clock source.
	T1CONbits.TCKPS = 3;    // Pre-scale by 256.
 	T1CONbits.TON = 1;      // Start the timer.
}
