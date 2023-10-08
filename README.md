# Applied_DSP_Final_Project

**September 2023**  
University of California, San Diego  
ECE-40164 Applied DSP  
Instructor: James F. Broesch

MPLAB X IDE v6.10
Board: Microchip dsPIC33F-GP-MC_DFP

## Overview
This project represents a doppler system utlising a Microchip DSP-kit board.  
Continuous sound pulses are emitted from a speaker, connected to the output of a board, to detect movement between the board's microphone and a solid surface (ie. wall). It works by comparing the phases of the sent and received signals. A moving object will alter the phase of the echoed signal depending on the speed of the object. If the phase changes beyond a preset threshold, the green LED on the board illuminates. All this occurs in real time, so an Assembly language MAC algorithm is used for efficiency.

## Methodology  
Comments are included throughout the code for clarity of methodology.
In summary:
-	A sine wave with frequency 1000Hz is generated at 16 cycles per frame.  
-	The timer module is used to start a clock which is used to indicate the passing of 1 second. The red LED alternates on/off every second; this is  used to create a flag for alternating between writing to the kth vector and the k-1th vector.  
-	The transmit and receive vectors are assigned to the DMA space to enable simultaneous send and capture of the pulsed audio signal.  
-	The 1000Hz sine wave is written to the txSignal vector and transmited for approximately 150ms using the frameIndex variable for timing.  
-	We then check for a received signal and either write it to the kth or k-1th vector depending on the status of the red LED.  
-	Once both vectors are filled, the energy of the received signal is calculated for normalising the sigma vector results and use the vectorMAC Assembly language routine to calculate the sigma values, which get stored in a sigma vector the size of one cycle of the sine wave.  
-	Finally the values of sigma within the vector are checked and the green LED is lit if the maximum value falls below a predetermined threshold, defaulted to 2500.  
