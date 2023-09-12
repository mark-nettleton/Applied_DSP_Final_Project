#include <xc.h>
#include "dma.h"

// Declare the flags.
unsigned int rxFlag = 0;
unsigned int txFlag = 0;

// Transmit and receive buffers.
// The attribute forces the buffers to be defined in the DMA space.
signed int rxSignal[FRAME_SIZE + PADDING] __attribute__((space(dma)));
signed int txSignal[FRAME_SIZE] __attribute__((space(dma)));

// Pointers to the buffers.
signed int *pSignalRx;
signed int *pSignalTx;

unsigned int n;

// Interrupt on frame transmitted.
void __attribute__((interrupt, no_auto_psv)) _DMA0Interrupt(void)
{
	txFlag = 1;
    IFS0bits.DMA0IF = 0;		//Clear the DMA0 Interrupt Flag
}

// Interrupt on frame received. 
void __attribute__((interrupt, no_auto_psv)) _DMA1Interrupt(void)
{
	rxFlag = 1;
    IFS0bits.DMA1IF = 0;		//Clear the DMA1 Interrupt Flag
}

void dmaInit()
{
	unsigned int n;

	// Clear the buffer.
	for(n=0; n < FRAME_SIZE; n++)
		{
		txSignal[n] = 0;
		rxSignal[n] = 0;
		}

    // INITIALISES THE DMA CHANNELS TO POINT TO THE SEND AND RECIEVE BUFFERS
	DMA0STA = __builtin_dmaoffset(txSignal); //DMA ch.0 sends input signal
	DMA1STA = __builtin_dmaoffset(rxSignal); //DMA ch.1 sends output signal

	// DCI TXBUF0 address
	DMA0PAD = (volatile unsigned int) &TXBUF0;
	// DCI RXBUF0 address
	DMA1PAD = (volatile unsigned int) &RXBUF0;
	// Number of words to transfer.
	DMA0CNT = FRAME_SIZE - 1;
	DMA1CNT = FRAME_SIZE - 1;
	// DMA Interrupt Request on DCI Transfer Done.
	DMA0REQ = 0x003C;	
	DMA1REQ = 0x003C;

	//DMA channel 0 direction is write.
	DMA0CONbits.DIR = 1;
	// Continous transfer, no ping pong.
	DMA0CONbits.MODE = 0;
	// Addresing mode is set to post-increment.
	DMA0CONbits.AMODE = 0;

	//DMA channel 1 direction is read.
	DMA1CONbits.DIR = 0;
	// Continous transfer, no ping pong.
	DMA1CONbits.MODE = 0;
	// Addresing mode is set to post-increment.
	DMA1CONbits.AMODE = 0;

	IFS0bits.DMA0IF  = 0;			// Clear DMA 0 interrupt
	IEC0bits.DMA0IE  = 1;			// Enable DMA 0 interrupt
	DMA0CONbits.CHEN = 1;			// Enable DMA 0 Channel	

	IFS0bits.DMA1IF  = 0;			// Clear DMA 1 interrupt
	IEC0bits.DMA1IE  = 1;			// Enable DMA 1 interrupt
	DMA1CONbits.CHEN = 1;			// Enable DMA 1 Channel	
}
