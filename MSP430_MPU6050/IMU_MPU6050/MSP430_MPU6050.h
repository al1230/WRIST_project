/*
 * MSP430_MPU6050.h
 *
 *  Created on: Jul 5, 2012
 *      Author: Mushfiq
 */

#ifndef MSP430_MPU6050_H_
#define MSP430_MPU6050_H_

#include "I2C_MSP430.h"
#include <msp430f5529.h>
#include "MPU6050.h"
#include <stdint.h>
#include <stdio.h>


/********* MPU6050 Address Declaration ***********/
#define MPU6050Addr 0x68																				//<! Stores the default address of MPU6050. An alternate address if 0x69.
/*************************************************/

/********* Internal Variables ***********/

static int tapCounter=0;
static int pastTime=0;

/**************************************/

/********* MPU6050 Functions ***********/

void getMotion6(int16_t* ax, int16_t* ay, int16_t* az, int16_t* gx, int16_t* gy, int16_t* gz);			//<! Function that retrieves the 6-axis motion from MPU6050/6000
uint8_t getDeviceID();																					//<! Function that retrieves the slave device address.
void setClockSource(uint8_t source);																	//<! Function that sets the clock source for the MPU6050/6000.
void setFullScaleGyroRange(uint8_t range);																//<! Function that sets the gyroscope range.
uint8_t setFullScaleAccelRange(uint8_t range);															//<! Function that sets the accelerometer range.
void setSleepEnabled(uint8_t enabled);																	//<! Function that wakes up the slave device.

void initializeIMU();																					//<! Function that setups IMU. This function can be altered by user.
void msDelay(unsigned int msTime);																		//<! Function that delays clock cycles by the specified number of miliseconds
void setDLPFConfig(uint8_t source);
void setInterruptPin();

/********* Work in Progress ************/
int detectDoubleTap(int currentTime);																	//<! Function that detections two taps on the IMU in rapid succession of each other

/**************************************/


#endif /* MSP430_MPU6050_H_ */
