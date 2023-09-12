/**********************************************************************
* © 2007 Microchip Technology Inc.
*
* FileName:        sask.c
* Dependencies:    Header (.h) files if applicable, see below
* Processor:       dsPIC33FJ256GP506
* Compiler:        MPLAB® C30 v3.00 or higher
*
************************************************************************/

#include "dscInit.h"

// FBS
#pragma config BWRP = WRPROTECT_OFF     // Boot Segment Write Protect (Boot Segment may be written)
#pragma config BSS = NO_FLASH           // Boot Segment Program Flash Code Protection (No Boot program Flash segment)
#pragma config RBS = NO_RAM             // Boot Segment RAM Protection (No Boot RAM)

// FSS
#pragma config SWRP = WRPROTECT_OFF     // Secure Segment Program Write Protect (Secure Segment may be written)
#pragma config SSS = NO_FLASH           // Secure Segment Program Flash Code Protection (No Secure Segment)
#pragma config RSS = NO_RAM             // Secure Segment Data RAM Protection (No Secure RAM)

// FGS
#pragma config GWRP = OFF               // General Code Segment Write Protect (User program memory is not write-protected)
#pragma config GSS = OFF                // General Segment Code Protection (User program memory is not code-protected)

// FOSCSEL
#pragma config FNOSC = FRC              // Oscillator Mode (Internal Fast RC (FRC))
#pragma config IESO = ON                // Two-speed Oscillator Start-Up Enable (Start up with FRC, then switch)

// FOSC
#pragma config POSCMD = NONE            // Primary Oscillator Source (Primary Oscillator Disabled)
#pragma config OSCIOFNC = ON            // OSC2 Pin Function (OSC2 pin has digital I/O function)
#pragma config FCKSM = CSECMD           // Clock Switching and Monitor (Clock switching is enabled, Fail-Safe Clock Monitor is disabled)

// FWDT
#pragma config WDTPOST = PS32768        // Watchdog Timer Postscaler (1:32,768)
#pragma config WDTPRE = PR128           // WDT Prescaler (1:128)
#pragma config WINDIS = OFF             // Watchdog Timer Window (Watchdog Timer in Non-Window mode)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (Watchdog timer enabled/disabled by user software)

int debounceS1;						/* Debounce counter for switch S1	*/
int debounceS2;						/* Debounce counter for switch S2	*/

void DSCInit(void)
{
	// Configure Oscillator to operate the device at 40MHz.
	//Fosc= Fin*M/(N1*N2), Fcy=Fosc/2
	//Fosc= 7.37M*40/(2*2)=80Mhz for 7.37M input clock */
 
	PLLFBD=41;					/* M=39	*/
	CLKDIVbits.PLLPOST=0;		/* N1=2	*/
	CLKDIVbits.PLLPRE=0;		/* N2=2	*/
	OSCTUN=0;			

	__builtin_write_OSCCONH(0x01);	//Initiate Clock Switch to FRC with PLL.
	__builtin_write_OSCCONL(0x01);
	while (OSCCONbits.COSC != 0b01); // Wait for the Clock switch to occur.
	while(!OSCCONbits.LOCK);

	/* Intialize the board LED and swicth ports	
	 * and turn all LEDS off. Also switches on the 
	 * VDD regulator on the serial flash memory 
	 * chip 	*/
	 
	long vddRegWakeUpDelay;
	
	YELLOW_LED_TRIS	= 0;
	RED_LED_TRIS = 0;		
 	GREEN_LED_TRIS = 0;	

 	YELLOW_LED = SASK_LED_OFF;	
	RED_LED	= SASK_LED_OFF;		
 	GREEN_LED = SASK_LED_OFF;		

 	SWITCH_S1_TRIS = 1;	
 	SWITCH_S2_TRIS	= 1;
 	
	VOLUME_UPDN_TRIS = 0;	
	VOLUME_CLK_TRIS	= 0;
	
	VOLUME_UPDN = 0;
	VOLUME_CLK = 0;	
    
    debounceS1 		= 0;
	debounceS2 		= 0;
	
	REGULATOR_CONTROL_ANPCFG = 1;
	REGULATOR_CONTROL_TRIS	= 0;
	REGULATOR_CONTROL_LAT = 1;

	for(vddRegWakeUpDelay = 0;
		vddRegWakeUpDelay < REGULATOR_WAKE_UP_DELAY;
			vddRegWakeUpDelay++)
				Nop();

}

int CheckSwitchS1(void)
{
	/* This function returns a 1 if a valid key press was detected on SW1 */
	int isActive;
	if(SWITCH_S1 == 0){
		
		debounceS1++;
		if (debounceS1 == SWITCH_DEBOUNCE){
			
			/* This means that the Switch S1 was pressed long enough
			 * for a valid key press	*/
			 isActive = 1;
		}
	}
	else	{
		debounceS1 = 0;
		isActive = 0;
	}
	
	return(isActive);
}	


int CheckSwitchS2(void)
{
	/* This function returns a 1 if a valid key press was detected on SW2 */
	int isActive;
	if(SWITCH_S2 == 0){
		/* If SW2 press was valid then toggle the record function	*/
		debounceS2++;
		if (debounceS2 == SWITCH_DEBOUNCE){
			
			/* This means that the Switch S1 was pressed long enough
			 * for a valid key press	*/
			isActive = 1;
		}
	}
	else	{
		debounceS2 = 0;
		isActive = 0;
	}
	return(isActive);
}