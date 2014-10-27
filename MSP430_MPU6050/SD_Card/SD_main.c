/*The following code is a stripped down version of a previous project, there
 * may be parts of this code that are unnecessary. This example will create
 * text file and write a few letters. If you have any questions or would like
 * to learn more how to implement this project using more extensive commands
 * please contact me at OCB456@aol.com. Thanks!
 * */

#include "SD_main.h"

unsigned char MST_Data,SLV_Data;
int result=1;

void fat_init(FIL* testFile, char *fileName, int flag){
    errCode = FR_NOT_READY;

    while (errCode != FR_OK){                               //go until f_open returns FR_OK (function successful)
        errCode = f_mount(0, &fatfs);                       //mount drive number 0
        errCode = f_opendir(&dir, "/");				    	//root directory

        if (!flag) errCode = f_open(testFile, fileName, FA_OPEN_EXISTING | FA_READ);
        else errCode = f_open(testFile, fileName, FA_CREATE_ALWAYS | FA_WRITE);

        if(errCode != FR_OK)
            result=0;                                       //used as a debugging flag
    }
}
