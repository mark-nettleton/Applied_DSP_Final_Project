/**********************************************************************
* © 2007 Microchip Technology Inc.
*
* FileName:        sask.h
* Dependencies:    Header (.h) files if applicable, see below
* Processor:       dsPIC33FJ256GP506
* Compiler:        MPLAB® C30 v3.00 or higher
*
************************************************************************/
#ifndef __SASK_BOARD_H__
#define __SASK_BOARD_H__
#include "p33FJ256GP506.h"

/* The LEDS and their ports	*/
#define YELLOW_LED_TRIS		_TRISC13
#define GREEN_LED_TRIS		_TRISC14
#define RED_LED_TRIS		_TRISC15

#define YELLOW_LED		_LATC13
#define GREEN_LED			_LATC14
#define RED_LED		_LATC15

/* The Switches and their ports */

#define SWITCH_S1_TRIS	_TRISD8
#define SWITCH_S2_TRIS	_TRISD9

#define SWITCH_S1	_RD8
#define SWITCH_S2	_RD9

/* Volume control pins	*/

#define VOLUME_UPDN_TRIS	_TRISG0
#define VOLUME_CLK_TRIS		_TRISG1

#define VOLUME_UPDN	_LATG0
#define VOLUME_CLK	_LATG1

#define SWITCH_DEBOUNCE			12	/* Wait for this many times before switch press is valid		
									 * For this application, each increment is 
									 * occurs every at sampling interval *
									 * frames size */
#define SASK_LED_ON 	0
#define SASK_LED_OFF	1

#define REGULATOR_CONTROL_TRIS		TRISBbits.TRISB5
#define REGULATOR_CONTROL_ANPCFG	AD1PCFGLbits.PCFG5
#define REGULATOR_CONTROL_LAT		LATBbits.LATB5
#define REGULATOR_WAKE_UP_DELAY	0x800

void DSCInit(void);
int CheckSwitchS1(void);
int CheckSwitchS2(void);

#endif
