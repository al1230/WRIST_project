/*
 * {iso15693.h}
 *
 * {ISO15693 header file}
 *
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

#ifndef _ISO15693_H_
#define _ISO15693_H_

//================================================================

#include <msp430.h>                         // Processor specific header
#include "mcu.h"
#include "trf7970.h"
#include "types.h"
#include "uart.h"

//===============================================================
//Mandatory Command Request Flags
#define SIXTEEN_SLOTS   0x06				//tables 3 & 5 in ISO15693-3; single-subcarrier, high tag data rate, sixteen slots
#define ONE_SLOT        0x26				//tables 3 & 5 in ISO15693-3; single-subcarrier, high tag data rate, one slot

//Optional Command Request Flags
#define SSC_HTDR_NA		0x02 				//tables 3 & 4 in ISO15693-3; single-subcarrier, high tag data rate, non-addressed
#define SSC_HTDR_ADDR	0x22 				//tables 3 & 4 in ISO15693-3; single-subcarrier, high tag data rate, addressed

//===============================================================

// if disabled file ISO15693.c may be excluded from build
//#define    ENABLE15693                            // delete to disable standard

//===============================================================

void Iso15693FindTag(void);
void NFC_TYPEV_READ_SINGLE_BLOCK(u08_t StartBlock);
void Iso15693Anticollision(u08_t *mask, u08_t length);
void TRF7970A_Init();
void Find_Tag();
//void Iso15693ReadMultipleBlocks_0_18(void);

//===============================================================

#endif
