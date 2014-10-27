/*
 *  uWave.h
 *  DarwiinRemote
 *
 *  Created by Guest on 6/23/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef UWAVE_H
#define UWAVE_H

//macros for quantization
//(8,4) applies to 100Hz; if 50Hz, change to (4,2)...
#define QUAN_WIN_SIZE 8
#define QUAN_MOV_STEP 4


//max number of gesture templates
#define NUM_TEMPLATES 3
#define DIMENSION 3 
#define MAX_ACC_LEN 100

#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <msp430f5529.h>
#include "SD_Card/SD_main.h"
#include "IMU_MPU6050/MSP430_MPU6050.H"
#include "LEDs.h"

/////////////////////////////////////////////////////////////////////////////


extern int recordFlag;

extern int tempIndex;

struct GestureStruct {
	int data[MAX_ACC_LEN][DIMENSION];
	int length;
};
typedef struct GestureStruct Gesture;
Gesture templates[NUM_TEMPLATES];

// 2D array to hold ACC data, each row contains (x, y, z) of an ACC sample   
int accBuffer[MAX_ACC_LEN][DIMENSION];
int accIndex;
int** allocAccBuf(int len);
void freeArray(int** p, int len);
void captureGesture();
int endGesture(); //return the gesture index if in recognizing mode (recordFlag = 0)

int quantizeAcc(int acc_data[MAX_ACC_LEN][DIMENSION], int length);
int DTWdistance(int sample1[MAX_ACC_LEN][DIMENSION], int length1, int sample2[MAX_ACC_LEN][DIMENSION], int length2, int i, int j, int* table);
int DetectGesture(int input[MAX_ACC_LEN][DIMENSION], int length, Gesture* templates);
Gesture readFile(int index);
int writeFile(int data[MAX_ACC_LEN][DIMENSION], int len, int index);
void loadFilesFromMem();


#endif

