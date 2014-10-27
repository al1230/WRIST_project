
#include <MSP430F5529.h>
#include "ff.h"
#include "diskio.h"

FIL file;                                               /* Opened file object */
FATFS fatfs;                                            /* File system object */
DIRS dir;                                               /* Directory object   */
FRESULT errCode;                                        /* Error code object  */
FRESULT res;                                            /* Result object      */
UINT bytesRead;                                         /* Bytes read object  */
//UINT read;                                              /* Read bytes object  */


FRESULT WriteFile(char*, char*, WORD);
void fat_init(FIL* testFile, char* fileName, int flag);
