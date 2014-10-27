/*
 *  uWave.c - uWave: Accelerometer-based Personalized Gesture Recognition
 *	Developed by Jiayang Liu, Zhen Wang, and Lin Zhong - Rice University
 *
 */
//


#include "uWave.h"

int recordFlag = 0; // record gesture to file or recognize it
int tempIndex = 0; //index of template when recording gesture


////////////////////////////////////////////////////////////////////////////////////////////////////
void freeArray(int** p, int len) {
	int i;
	for( i = 0; i < len; i++)
		free(p[i]);
	free(p);
}

//routine at beginning
void captureGesture(){
	int iter;
	int16_t ax, ay, az;					//<! Holds the raw accelerometer data.
	int16_t gx, gy, gz;					//<! Holds the raw gyroscope data.
	float g_ax, g_ay, g_az;				// scaled Accel values
	float sensitivityFactor = 8192.0;

	for (iter=0; iter<MAX_ACC_LEN; iter++)
	{
		if (iter==0) printf("/************begin gesture**************/\r\n");
		msDelay(25);
		getMotion6( &ax, &ay, &az, &gx, &gy, &gz);	//Obtain live IMU values

		//Scale down the live IMU values using the sensitivity factor
		g_ax= ((float)ax/sensitivityFactor);
		g_ay= ((float)ay/sensitivityFactor);
		g_az= ((float)az/sensitivityFactor);

		//Store the values into the acceleration buffer at the current index
		accBuffer[accIndex][0] = g_ax*10;
		accBuffer[accIndex][1] = g_ay*10;
		accBuffer[accIndex][2] = (g_az-1)*10;
		accIndex++;

		if (iter==MAX_ACC_LEN-1) printf("/************gesture finished***********/\r\n");
	}
	

}
		
//routine at end: record gesture or detect gesture
int endGesture() {
	int ret = 0;
	if( recordFlag == 1) {									//write to file

		writeFile(accBuffer, accIndex, tempIndex);
		tempIndex++;
	} else {												//recognize gesture
		accIndex = quantizeAcc(accBuffer, accIndex);     	//quantize acc
		ret = DetectGesture(accBuffer, accIndex, templates);
	}
	return ret;
}

//quantize
int quantizeAcc(int acc_data[MAX_ACC_LEN][DIMENSION], int length) {
	int i=0, j, k = 0, l, window = QUAN_WIN_SIZE, sum;
	int **temp = (int**)malloc(sizeof(int*)*(length/QUAN_MOV_STEP + 1));
	for(i = 0; i < length/QUAN_MOV_STEP + 1; i++)
		temp[i] = (int*)malloc(sizeof(int)*DIMENSION);

	//take moving window average
	i = 0;
	while(i < length) {
		if( i + window > length)
			window = length - i;
		for( l = 0; l < DIMENSION; l++) {
			sum = 0;
			for( j = i; j < window+i; j++)
				sum += acc_data[j][l];
			temp[k][l] = sum*1.0/window;
		}
		k++;
		i += QUAN_MOV_STEP;
	}//while
	//nonlinear quantization and copy quantized value to original buffer 	
	for( i = 0; i < k; i++)
		for( l = 0; l < DIMENSION; l++) {		
			if( temp[i][l] > 10 ) {
				if( temp[i][l] > 20)
					temp[i][l] = 16;
				else
					temp[i][l] = 10 + (temp[i][l]-10)/10*5;
			} else if( temp[i][l] < -10) {
				if( temp[i][l] < -20)
					temp[i][l] = -16;
				else 
					temp[i][l] = -10 + (temp[i][l] + 10)/10*5;
			}
			acc_data[i][l] = temp[i][l];
		}

	freeArray(temp, length/QUAN_MOV_STEP +1);
	return k;
}//quantize

//DTW algorithm, return distance
int DTWdistance(int sample1[MAX_ACC_LEN][DIMENSION], int length1, int sample2[MAX_ACC_LEN][DIMENSION], int length2, int i, int j, int* table) {
	
	if( i < 0 || j < 0)
		return 32000;
	int tableWidth = length2;
	int localDistance = 0;
	int k;
	for( k = 0; k < DIMENSION; k++)
		localDistance += ((sample1[i][k]-sample2[j][k])*(sample1[i][k]-sample2[j][k]));
	
	int sdistance, s1, s2, s3;
	
	if( i == 0 && j == 0) {
		if( table[i*tableWidth+j] < 0)
			table[i*tableWidth+j] = localDistance;
		return localDistance;
	} else if( i==0) {
		if( table[i*tableWidth+(j-1)] < 0)
			sdistance = DTWdistance(sample1, length1, sample2, length2, i, j-1, table);
		else
			sdistance = table[i*tableWidth+j-1];
	} else if( j==0) {
		if( table[(i-1)*tableWidth+ j] < 0)
			sdistance = DTWdistance(sample1, length1, sample2, length2, i-1, j, table);
		else
			sdistance = table[(i-1)*tableWidth+j];  
	} else {
		if( table[i*tableWidth+(j-1)] < 0)
			s1 = DTWdistance(sample1, length1, sample2, length2, i, j-1, table);
		else
			s1 = table[i*tableWidth+(j-1)];
		if( table[(i-1)*tableWidth+ j] < 0)
			s2 = DTWdistance(sample1, length1, sample2, length2, i-1, j, table);
		else
			s2 = table[(i-1)*tableWidth+ j];
		if( table[(i-1)*tableWidth+ j-1] < 0)
			s3 = DTWdistance(sample1, length1, sample2, length2, i-1, j-1, table);
		else
			s3 = table[(i-1)*tableWidth+ j-1];
		sdistance = s1 < s2 ? s1:s2;
		sdistance = sdistance < s3 ? sdistance:s3;
	}
	table[i*tableWidth+j] = localDistance + sdistance;
	return table[i*tableWidth+j];
}

//write/read gesture file
Gesture readFile(int index) {
	FIL fp;
	char fname[10];
	char buffer[8];
	char *ptr;
	int data[DIMENSION*MAX_ACC_LEN], i=0,j, n;
	Gesture ret;
	
	bzero(data, DIMENSION*MAX_ACC_LEN);
	sprintf(fname, "%d", index);
	fat_init(&fp, strcat(fname, ".uwv"), 0);


	i=0;
	do{
		bzero(buffer, 8);
		f_gets(buffer, 8, &fp);


		ptr = strtok(buffer, "\r \n");
		data[i]=atoi(ptr);

		ptr = strtok(NULL, "\r \n");
		data[i+1] = atoi(ptr);

		ptr = strtok(NULL, "\r \n");
		data[i+2] = atoi(ptr);

		i += 3;
	} while (!f_eof(&fp) && i<DIMENSION*MAX_ACC_LEN);
	n = (i-3)/DIMENSION;
	ret.length = n;
	for(i = 0; i < n; i++)
		for(j = 0; j < DIMENSION; j++)
			ret.data[i][j] = data[(i*DIMENSION)+j];
	
	f_close(&fp);
	f_mount(0,0);
	return ret;
}


int writeFile(int data[MAX_ACC_LEN][DIMENSION], int len, int index){
	FIL fp;
	int i,j;
	char fname[10];
	sprintf(fname, "%d", index);
	fat_init(&fp, strcat(fname, ".uwv"), 1);
	for(i = 0; i < len; i++) {
		for( j = 0; j < DIMENSION; j++) {
			f_printf(&fp, "%d ", data[i][j]);
		}
		f_printf(&fp, "\n");
	}
	
	f_close(&fp);
	f_mount(0,0);
	return 0;
}


//detect gesture, return detected template index
int DetectGesture(int input[MAX_ACC_LEN][DIMENSION], int length, Gesture* templates)
{
	if( length <= 0)
		return -1;
	int i, ret = 0,j;

	int distances[NUM_TEMPLATES];
	bzero(distances, NUM_TEMPLATES);
	int* table;
	for( i = 0; i < NUM_TEMPLATES; i++) {
		table = (int*) malloc(length * templates[i].length*sizeof(int));
		for( j = 0; j < length*templates[i].length; j++)
			table[j] = -1;
		
		distances[i] = DTWdistance(input, length, templates[i].data, templates[i].length, length-1, templates[i].length-1, table);
		distances[i] /= (length + templates[i].length);
		free(table);
	}  
	
	for( i = 1; i < NUM_TEMPLATES; i++) {
		if( distances[i] < distances[ret]) {
			ret = i;
		}
	}

	return ret;
}

void loadFilesFromMem(){
	int i;
	if (!recordFlag)
	{
		printf("reading from SD Card...\r\n");

		for( i = 0; i < NUM_TEMPLATES; i++) {
			templates[i] = readFile(i);
			templates[i].length = quantizeAcc(templates[i].data, templates[i].length);
		}
		printf("reading completed. device ready\r\n\r\n");
		flashLED2();
	}

}

