/**********************************************************************
* MARK NETTLETON: APPLIED DSP FINAL PROJECT 
* September 2023
* Instructor: James D. Broesch
* 
* This program represents a Doppler system to detect a target placed 
* between the speaker and the microphone.
* It demonstrates the use of the DMA to simultaneously transmit a 1000Hz 
* pulsed audio signal and capture the returned signal.
* The two signals are then comapred for correlation using an
* efficient Assembly language routine. 
* The pulse transmission stops when switch 2 is pressed.
* A timer interrupt provides background functions.
*
* FileName:        main.c
* Dependencies:    Header (.h) files if applicable, see below
* Processor:       Microchip dsPIC33FJ256GP506
* Compiler:        MPLAB® XC16 v2.10 or higher
*
************************************************************************/
#include <math.h>
#include <xc.h>

#include "p33FJ256GP506.h"
#include "WM8510CODEC.h"
#include "timer.h"
#include "dscInit.h"
#include "dma.h"

// Declare the vector funcions as external. Only MAC used here.
extern int vectorMAC(int *x, int *y, unsigned int loop_count, unsigned int scale);
extern int vectorShift(int *vector, int vectorLength);

// extern unsigned int dataReady; // Used in DCI Interrupt.

// FRAME_SIZE - Size of each audio frame 

#define N_FRAMES				1
#define FRAME_SIZE 				128
#define SIGNAL_SIZE 			FRAME_SIZE * N_FRAMES
#define PADDING                 8 // Number of samples in one 1000Hz cycle
#define THRESHOLD               2500 //Correlation threshold - used for LED

signed long int sigma[PADDING]; /* Vector used to calculate send and 
                                   return signal correlation */

// Main vectors used for transmit and return signals in DMA space.
extern signed int rxSignal[];
extern signed int txSignal[];

// Working pointers.
extern signed int *pSignalRx;
extern signed int *pSignalTx;

// The kth signal vector is placed in the x memory space.
// The k-1th vector is placed in the y memory space.
// This allows single cycle MAC operations.
signed int kth[SIGNAL_SIZE + PADDING +1]__attribute__((space(xmemory), aligned,));
signed int kminus1[SIGNAL_SIZE + 1]__attribute__ ((space(ymemory), aligned));

long signed int energy; // Variable for storing return signal's energy
unsigned int 	frameIndex;

// Frame recieved/transmitted DMA interrupt flags.
extern unsigned int rxFlag;
extern unsigned int txFlag;

// Make varibles in timer module visibile in this one.
extern unsigned int timer1Count;
extern int flagLED;

int main(void)
{	
	// Declarations for the output signal.
	float f = (float)16; // Number of cycles per frame (1000Hz))
	float T = (float)1/(float)FRAME_SIZE;
	float PI =  3.14159265;
    // Local variables used in main processing loop
	int m, n; // Working indeces.	

	// Intialize the board and the drivers
	DSCInit();

	// Initalize the CODEC.
	WM8510Init();
	// Start Audio input and output function
	WM8510Start();
		
	// Configure the CODEC for 8K operation	
	WM8510SampleRate8KConfig();

	// Use timer 1 as a real time clock.
	// 1 tick = 15,625 * 256 clock cycles = 100ms.
	timer1Init(15625);

	/*  Intialize and start the DMA channels.
        Channel 0 moves data from the transmit buffers to the DCI.
        Channel 1 moves data from the DCI to the recieve buffers.
    */ 
    dmaInit();
    
    pSignalTx = txSignal;   // Point to the transmit signal buffer.
    pSignalRx = rxSignal;   // Point to the received signal buffer.
    
    // Clear the correlation result vector.
	for (m = 0; m < PADDING; m++)
	{
		sigma[m] = 0;
	}
   
    /* Main processing loop. *************************************/
    while (SWITCH_S2)   // Quit if switch 2 is pressed.
    {
        if (timer1Count % 10 == 0) // Wait 1 second
        {                       
            frameIndex = 0;
            
            //Enable DMA channels
            DMA0CONbits.CHEN = 1; 
            DMA1CONbits.CHEN = 1;
            
            for(n = 0; n < SIGNAL_SIZE; n++)
            {
                txSignal[n] = 0; // clean the signal transfer buffer
            }
            
            while(frameIndex++ < 10) // transmit signal for ~150ms
            {
                    // Write the 1000Hz signal frame to the output                
                    for(n = 0; n < SIGNAL_SIZE; n++)
                    {
                        txSignal[n] = sin(2*PI*f*(float)n*T) * 0x7FFF;
                    }
                    while(txFlag != 1){} // Wait for frame transmitted
                    // Reset the DMA0 interrupt flag
                    txFlag = 0;
                
                /* Check for frame received then alternately write to 
                 * kth and k-1th buffers each second */
                if (rxFlag == 1 && frameIndex > 9) 
                {
                    if (flagLED == 0)
                    {
                        for(n = 0; n < SIGNAL_SIZE; n++)
                        {
                            kminus1[n] = rxSignal[n];
                        } 
                    }
                    
                    else 
                    {
                        for(n = 0; n < SIGNAL_SIZE + PADDING; n++)
                        {
                            kth[n] = rxSignal[n];
                        } 
                    }
                    rxFlag = 0;
                } 
            }
            DMA0CONbits.CHEN = 0; //Disable DMA channels
            DMA1CONbits.CHEN = 0;
            
            /* Calculate energy of received signal for use in normalising
             * sigma correlation vector */
            energy = 0;
            for(n = 0; n < SIGNAL_SIZE; n++) 
            {
                energy += ((long signed int)rxSignal[n] * (long signed int)rxSignal[n])>>15;
            }
            energy >>= 16;
            
            // Every other pulse, take dot product of kth and k-1th vectors
            // and store in sigma[m]
            if (flagLED == 1) 
            {
                for (m = 0; m < PADDING; m++)
                {    
                    sigma[m] = (long signed int)vectorMAC(kth+m, kminus1, SIGNAL_SIZE, 16)
                                * 0X7fff/energy;
                }
            }
            /* Finally light green LED if correlation between kth and
             * k-1th vectors falls below threshold value */
            int temp = 0;
			for (n = 0; n < PADDING; n++)
				if (sigma[n] > temp) temp = sigma[n];
			
			// Set your threshold value. 
			if (temp < THRESHOLD) GREEN_LED = SASK_LED_ON;
			else GREEN_LED = SASK_LED_OFF;            
        }
    }
	// Stop the CODEC interface.
	WM8510Stop();

return(0);
}
