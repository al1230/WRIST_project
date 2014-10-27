/*
 * LEDs.h
 *
 *  Created on: Apr 5, 2014
 *      Author: Alex
 */

#ifndef LEDS_H_
#define LEDS_H_

#define TOGGLE_LED1     P1OUT ^= BIT0;
#define TOGGLE_LED2 	P4OUT ^= BIT7; 		// Toggle LED state
#define LED1_ON			P10UT |= BIT0;
#define LED1_OFF		P1OUT &= ~BIT0;
#define LED2_ON			P4OUT |= BIT7;
#define LED2_OFF		P4OUT &= ~BIT7;

void ledInit(void);
void flashLED1(void);
void flashLED2(void);
void flashBothLEDs(int delay);



#endif /* LEDS_H_ */
