// MSP430 Gesture Recognition using MPU6050 IMU & BOB-00544 MicroSD Card Reader
// 05/19/2014 Created by Alex Lam <al1230@gmail.com>
//
// Adaptations
//		I2C Library from Mushfiq Sarker <mushfiqsarker@inventige.com>
//		MicroSD Card Read from FatFS <http://elm-chan.org/fsw/ff/00index_e.html>
//		uWave Gesture Recognition from Jiayang Liu, Zhen Wang, and Lin Zhong <Rice University>
//
// Functionality
//		The system reads gesture templates from microSD Card, then waits for a double tap to initiate
//		the capture of a gesture for comparison. A rapid flashing red light on the MSP430 indicates
//		the upcoming capture. A green light indicates the capture. Flashing red and green lights
//		indicates a correct match.



#include "msp430f5529.h"
#include "IMU_MPU6050/MSP430_MPU6050.h"
#include "uWave.h"
#include "LEDs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define CORRECT_TEMPLATE 2
#define NFC 1

int timer=0;
int interruptFlag = 0;
int iter=0, returnVal=0;

int main(void)
 {

	  WDTCTL = WDTPW + WDTHOLD;             			// Stop watchdog timer


	  if( NFC)
	  {
		  	  P1DIR |= BIT3;							// P1.3 set as output
		  	  P1REN |= BIT3;							// P1.3 pull up resistor
		  	  P1OUT &= ~BIT3;							// Initialize P1.3 to low
	  }

	  ledInit();
	  msDelay(300);
	  initializeIMU();									// Initialize IMU
	  loadFilesFromMem();								// Read gesture template files from SD Card


	  __enable_interrupt();


	  for (timer=0; ;timer++)
	  {
		  	if (timer ==10000) timer = 0;

		  	if ( interruptFlag && detectDoubleTap(timer) )
		  	{
		  		flashLED1();							// Flash left LED to signify to an upcoming capture
		  		LED2_ON;								// After, flash right LED to signify a capture start

		  		captureGesture();
		  		LED2_OFF;								// Right LED off for end gesture
				returnVal = endGesture();				// Compute detection match

				if (returnVal==CORRECT_TEMPLATE)
				{
					printf("handshake detected\r\n\r\n");

					//Send an interrupt signal to NFC on gesture detect
					P1OUT ^= BIT3;
					msDelay(300);
					P1OUT ^= BIT3;

					flashBothLEDs(300);					// Flash both LEDs to indicate a correct match


				}
				else printf("no gesture detected\r\n\r\n");
				msDelay(500);
				returnVal=0;							// Clear return val
		  	}
		  	P1IE |= BIT4;                           	// Enable port interrupt
		  	interruptFlag = 0;							// Reset interrupt flag to zero
	  }


}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void) {

	interruptFlag = 1;
	P1IFG &= ~BIT4;										// Clear P1.4 IFG
	P1IE &= ~BIT4;                        				// Clear P1.4 IE

	_BIC_SR(LPM0_EXIT);
}

