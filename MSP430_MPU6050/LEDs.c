/*
 * LEDs.c
 *
 * Useful functions for onboard MSP430 LED initializations and usage.
 *
 *
 *  Created on: Apr 5, 2014
 *      Author: Alex
 */
#include <msp430f5529.h>
#include "LEDs.h"
#include "IMU_MPU6050/I2C_MSP430.h"



void ledInit(){

	P1DIR |= BIT0;
	P4DIR |= BIT7;						// P4.7 (LED) output

	//Initialize outputs to zero
	P1OUT &= ~BIT3;
	P4OUT &= ~BIT7;
}

void flashLED1()
{
	int i;
	for (i=0; i<68; i++)
	{
		P1OUT ^= BIT0; 					// Toggle LED state
		msDelay(60);
	}
	LED1_OFF;
}

void flashLED2()
{
	int i;
	for (i=0; i<20; i++)
	{
		P4OUT ^= BIT7; 					// Toggle LED state
		msDelay(60);
	}
	LED2_OFF;
}

void flashBothLEDs(int delay)
{

	// Flash both LEDs (P1.0 & P4.7) if gesture is detected
	P1OUT ^= BIT0; P4OUT ^= BIT7;
	msDelay(delay);
	P1OUT ^= BIT0; P4OUT ^= BIT7;
	msDelay(delay);
	P1OUT ^= BIT0; P4OUT ^= BIT7;
	msDelay(delay);
	P1OUT ^= BIT0; P4OUT ^= BIT7;

}


