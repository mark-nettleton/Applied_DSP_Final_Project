#include "WM8510CODEC.h"
#include "p33FJ256GP506.h"

/* DCI Interrupt. Not used in this version

extern signed int signalIn;
extern signed int signalOut;

volatile unsigned int dataReady = 0;
volatile unsigned int count = 0;

void __attribute__((__interrupt__,no_auto_psv)) _DCIInterrupt(void)
{
	// Get the recieved sample from the CODEC.
	signalIn = RXBUF0;

	signalOut = signalIn;

	// Send the output sample to the CODEC.
	TXBUF0 = signalOut;

	// Set the data ready flag.
	dataReady = 1;

	_DCIIF = 0;
}
*/
void WM8510Init()
{
    //Disable these modules before configuring them
	DCICON1bits.DCIEN = 0;
	I2C1CONbits.I2CEN = 0;

	I2C1CONbits.I2CSIDL = 0;    /* I2C is running during idle	*/
	I2C1CONbits.IPMIEN 	= 0;    /* IPMI is disabled             */
	I2C1CONbits.A10M	= 0;    /* 7 bit slave address          */
	I2C1CONbits.DISSLW 	= 1;    /* No slew control              */
	I2C1CONbits.SMEN 	= 0;	/* No SMBus Enabled             */
    
    I2C1BRG = WM8510_I2CBAUD;   /* Set the I2C baud rate to 100KHz. */
	
	_TRISG2 = 1;                /* I2C TRISSCL  */
	_TRISG3 = 1;                /* I2C TRISSDA  */
	
	DCICON1bits.DCISIDL = 1;    // Halt when the processor is idle.
	DCICON1bits.DLOOP = 0;      // No digital loop back.
	DCICON1bits.CSCKD = 1;      // Sample Clock Direction Control bit
	DCICON1bits.CSCKE = 1;      // Sample Clock Direction Control bit
	DCICON1bits.COFSD = 1;      // Frame Synchronization Direction Control bit
	DCICON1bits.UNFM = 0;       // Underflow Mode bit
	DCICON1bits.CSDOM = 0;      // Serial Data Output Mode bit
	DCICON1bits.DJST = 0;       // DCI Data Justification Control bit
	DCICON1bits.COFSM = 0b00;   // Frame Sync Mode bits
	DCICON2bits.BLEN = 0;
	DCICON2bits.COFSG = 0;
	DCICON2bits.WS = 0b1111;

	TSCON = 1;
	RSCON = 1;	
    
}
	
void WM8510Start()
{  
    TXBUF0	= 0;                // Dummy write to start the DMA.
	I2C1CONbits.I2CEN = 1;      // Enable the I2C module	
	DCICON1bits.DCIEN = 1;      // Enable the DCI module
	_DCIIF = 0;                 // Clear the interrupt flag.
//	_DCIIE = 1;                 // Enable the interrupt.
//	_DCIIP = 7;                 // Set the DCI interrupt to the highest priority.
}
	
void WM8510Stop()
{
	DCICON1bits.DCIEN = 0;      // Stop the DCI module.
	_DCIIE = 0;                 // Stop Interrupts
}

int WM8510IOCtl(int command, void * value)
{
	/* Use the I2C module to send the control data to the codec
	 * Send the device address first, the upper control word next
	 * which consists of register address and bit B8 of control data
	 * and finally send the control data bits B7-B0	*/
	
	CommandValueWord commandValue;
	
	commandValue.wordValue = *((int *)value);
	commandValue.bytes[1] &= 0x1;
	command = command << 1;
	command = command | commandValue.bytes[1];
	
	I2C1CONbits.SEN = 1;			// Send the start condition
	while(I2C1CONbits.SEN == 1);	// Wait for the condition to complete
	
	I2C1TRN = WM8510_I2C_ADDR;      // Address of the WM8510
	while(I2C1STATbits.TRSTAT);		// Wait till this has been tranmitted
	if (I2C1STATbits.ACKSTAT)		// If this value is 1 then a NACK was
	{								// was recieved	
		I2C1CONbits.PEN = 1;
		while(I2C1CONbits.PEN);		// Send the stop condition if a NACK
		return(-1);					// was received	
	}

	I2C1TRN = command;				// The address of the register.
	while(I2C1STATbits.TRSTAT);		// Wait till this has been tranmitted	
	if (I2C1STATbits.ACKSTAT)		// If this value is 1 then a NACK was
	{								// was recieved.
		I2C1CONbits.PEN = 1;
		while(I2C1CONbits.PEN);		// Send the stop condition if a NACK
		return(-1);					// was received				
	}
	
	I2C1TRN = commandValue.bytes[0];// The value of the register
	while(I2C1STATbits.TRSTAT);		// Wait till this has been tranmitted
	if (I2C1STATbits.ACKSTAT)		// If this value is 1 then a NACK was
	{								// was recieved.
		I2C1CONbits.PEN = 1;
		while(I2C1CONbits.PEN);		// Send the stop condition if a NACK
		return(-1);					// was received.
	}
	
	I2C1CONbits.PEN = 1;
	while(I2C1CONbits.PEN);			// Send the stop condition.
	return(1);
}

void WM8510SampleRate8KConfig(void)
{
	int commandValue,result;
	commandValue = 1;		/* Any value can be written to reset the codec	*/
	result = WM8510IOCtl(WM8510_SOFTWARE_RESET, 	(void *) &commandValue);
	if (result == -1) while(1);
	commandValue = 0b001101111;		
	WM8510IOCtl(WM8510_POWER_MGMT1, 		(void *) &commandValue);
	if (result == -1) while(1);
	commandValue = 0b000010101;
	WM8510IOCtl(WM8510_POWER_MGMT2, 		(void *) &commandValue);
	if (result == -1) while(1);
	commandValue = 0b010001001;
	WM8510IOCtl(WM8510_POWER_MGMT3, 		(void *) &commandValue);
	if (result == -1) while(1);
	commandValue = 0b000011000;
	WM8510IOCtl(WM8510_AUDIO_INTERFACE, 	(void *) &commandValue);
	if (result == -1) while(1);
	commandValue = 0b110101001;
	WM8510IOCtl(WM8510_CLOCKGEN_CTRL, 		(void *) &commandValue);
	if (result == -1) while(1);
	commandValue = 0b000001010;
	WM8510IOCtl(WM8510_ADDITIONAL_CTRL, 	(void *) &commandValue);
	if (result == -1) while(1);
	commandValue = 0b000000100;
	WM8510IOCtl(WM8510_GPIO_STUFF, 			(void *) &commandValue);
	if (result == -1) while(1);
	commandValue = 0b100001000;
	WM8510IOCtl(WM8510_ADC_CONTROL	, 		(void *) &commandValue);
	if (result == -1) while(1);
    
	commandValue = 0b000000101;
	WM8510IOCtl(WM8510_NOTCH_FILTER2, 		(void *) &commandValue);
	if (result == -1) while(1);
	
	commandValue = 0b000111111;
	WM8510IOCtl(WM8510_NOTCH_FILTER3, 		(void *) &commandValue);
	if (result == -1) while(1);
	
	commandValue = 0b001111101;
	WM8510IOCtl(WM8510_NOTCH_FILTER4, 		(void *) &commandValue);
	if (result == -1) while(1);
	
	commandValue = 0b100000000;
	WM8510IOCtl(WM8510_NOTCH_FILTER4, 		(void *) &commandValue);
	if (result == -1) while(1);
	
	commandValue = 0b000011000;
	WM8510IOCtl(WM8510_PLL_N, 				(void *) &commandValue);
	if (result == -1) while(1);
	commandValue = 0b000001100;
	WM8510IOCtl(WM8510_PLL_K1, 				(void *) &commandValue);
	if (result == -1) while(1);
	commandValue = 0b010010011;
	WM8510IOCtl(WM8510_PLL_K2, 				(void *) &commandValue);
	if (result == -1) while(1);
	commandValue = 0b011101001;
	WM8510IOCtl(WM8510_PLL_K3, 				(void *) &commandValue);
	if (result == -1) while(1);
	commandValue = 0b000000100;
	WM8510IOCtl(WM8510_INPUT_CTRL, 			(void *) &commandValue);
	if (result == -1) while(1);
	commandValue = 0b000000000;
	WM8510IOCtl(WM8510_ADC_BOOST_CTRL, 	(void *) &commandValue);
	if (result == -1) while(1);
	commandValue = 0b000000001;
	WM8510IOCtl(WM8510_MONO_MIXER_CTRL,	 (void *) &commandValue);
	if (result == -1) while(1);

}


