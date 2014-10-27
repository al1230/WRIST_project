/*
 * {iso15693.c}
 *
 * {ISO15693 Specific Functions & Anti-collision}
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

#include "iso15693.h"

//===============================================================

u08_t afi = 0;
extern u08_t flags = 0;							// request flags used in command, based on ISO15693-3, tables 3, 4 and 5
extern u08_t blocknum = 0;						// for making block number dynamic (somewhat at this time :))
extern u08_t buf[300];
extern u08_t i_reg;
extern u08_t irq_flag;
extern s08_t rxtx_state;
extern u08_t rx_error_flag;
extern u08_t stand_alone_flag;
extern u08_t remote_flag;
extern u08_t Tag_Count;

//KIm added
extern u08_t UID_holder[10];
extern u08_t UID_holder2[10];
extern int UID_chooser;
extern u08_t enable;
extern int flag;
extern int counter;

//===============================================================
// NAME: void Iso15693FindTag(void)
//
// BRIEF: Is used to detect ISO15693 conform tags in stand alone
// mode.
//
// INPUTS:
//
// OUTPUTS:
//
// PROCESS:	[1] turn on RF driver
//			[2] do a complete anti-collision sequence
//			[3] turn off RF driver
//
// NOTE: If ISO15693 conform Tag is detected, ISO15693 LED will
//       be turned on.
//
// CHANGE:
// DATE  		WHO	DETAIL
// 23Nov2010	RP	Original Code
//===============================================================

void
Iso15693FindTag(void)
{
	Trf7970TurnRfOn();

	Trf7970WriteIsoControl(0x02);

	// The VCD should wait at least 1 ms after it activated the
	// powering field before sending the first request, to
	// ensure that the VICCs are ready to receive it. (ISO15693-3)
	McuDelayMillisecond(6);

	flags = SIXTEEN_SLOTS;
	//flags = ONE_SLOT;

	buf[20] = 0x00;
	Iso15693Anticollision(&buf[20], 0x00);					// send Inventory request

	Trf7970TurnRfOff();

	// clear any IRQs
	Trf7970ResetIrqStatus();
}

//===============================================================
// NAME: void NFC_TYPEV_READ_SINGLE_BLOCK(void)
//
// BRIEF: 	Is used to issue optional ISO15693 command: Read Single Block
//			Function: Reads a single block of ISO15693 tag memory and loops, incrementing the block number each time.
//
// INPUTS:	request flags, block # (in this case default request flags are 0x02, block # starts a 0x00)
//
// OUTPUTS:	Block Data, from 0x00 to 0x3F (256 bytes of data) or to the point where error flags are returned (for tags smaller than 256 bytes, i.e. HF-I Pro or Standard)
//
// PROCESS:	[1] turn on RF driver
//			[2] do a complete read single block sequence (Command Code 0x20)
//			[3] turn off RF driver
//			[4] loop until block 0x3F is read or error flags are returned in response (see note in INPUTS section)
////
// CHANGE:
// DATE  		WHO	DETAIL
// 01/17/2014	JDW / JC Original Code
//===============================================================

void NFC_TYPEV_READ_SINGLE_BLOCK(u08_t ui8StartBlock)
{
	u08_t	i = 1;
	u16_t	k;

	Trf7970TurnRfOn();

	Trf7970WriteIsoControl(0x02);

	// The VCD should wait at least 2mSec after activating the
	// magnetic field before sending the first request, to
	// ensure that the VICC is ready to receive it. (ISO15693-3)

	rxtx_state = 1; 										//resetting buffer pointer
	McuDelayMillisecond(6); 								//plenty of time :)

	flags = SSC_HTDR_NA;									//non-addressed request flags

//	flags = SSC_HTDR_ADDR;									//addressed (requires getting and storing UID first)

		buf[0] = 0x8F;										//reset FIFO
		buf[1] = 0x91;										// sending with CRC, means TRF79xx will append CRC to string going out over the air
		buf[2] = 0x3D;										// write continuous from 1D
		buf[3] = 0x00;										//upper and middle nibbles of transmit byte length
		buf[4] = 0x30;										//lower and broken nibbles of transmit byte length
		buf[5] = flags;										// ISO15693 flags
		buf[6] = 0x20;										// Read Single Block command code
		buf[7] = ui8StartBlock;								// Block # (variable, for HF-I Plus device can go to 0x3F, Pro and Standard handled with "error" response flags)

		Trf7970ResetIrqStatus();							//clearing IRQ (just in case)

		McuCounterSet();									// TimerA set
		COUNT_VALUE = COUNT_1ms * 30;						// 30ms, not 20ms
		IRQ_CLR;											// PORT2 interrupt flag clear (inside MSP430)
		IRQ_ON;

		Trf7970RawWrite(&buf[0], 8);						//issuing the Read Single Block command

		i_reg = 0x01;
		irq_flag = 0x00;
		START_COUNTER;										//	Starting Timeout

		while(irq_flag == 0x00)
		{
		}													// wait for end of TX interrupt
		RESET_COUNTER;

		McuCounterSet();									// TimerA set
		COUNT_VALUE = COUNT_1ms * 20;
		START_COUNTER;										// start timer up mode

		irq_flag = 0x00;

		while(irq_flag == 0x00)
		{
		}													// wait for interrupt
		RESET_COUNTER;

		while(i_reg == 0x01)								// wait for RX complete
		{
			k++;

			if(k == 0xFFF0)
			{
				i_reg = 0x00;
				rx_error_flag = 0x00;
			}
		}

		if( i_reg == 0xFF)
			{												 // if received block data in buffer
			if((stand_alone_flag == 1) && (buf[1] == 0x00))  // Confirming "no error" in response flags byte
			{
				#ifdef ENABLE_HOST

					UartSendCString("NFC-V Block ");
					UartPutByte(ui8StartBlock);
					UartSendCString(" Data:      ");
					UartPutChar('[');

					for(i = 2; i < 6; i++)
					{
						UartPutByte(buf[i]);				// send block data to host
					}

					UartPutChar(']');
					UartPutCrlf();
				#endif

					rxtx_state = 1; 						//resetting buffer pointer

			}
		}

	Trf7970TurnRfOff();

	// clear any IRQs
	Trf7970ResetIrqStatus();
}

//===============================================================
// NAME: void Iso15693ReadMultipleBlocks(void)
//
// BRIEF: 	Is used to issue optional ISO15693 command: Read Multiple Blocks
//			Function: Reads multiple blocks of ISO15693 tag memory.
//
// INPUTS:	request flags, block # (in this case default request flags are 0x02, starting block # is 0x00, reading 8 blocks of data)
//
// OUTPUTS:	Block Data
//
// PROCESS:	[1] turn on RF driver
//			[2] do a complete read multiple blocks sequence (Command Code 0x23)
//			[3] turn off RF driver
//
//
// CHANGE:
// DATE  		WHO	DETAIL
// 01/17/2014	JDW	Original Code
//===============================================================
void Iso15693ReadMultipleBlocks_0_18(void) //only use for 2k bit tag
{
	u08_t	i = 1;
	u16_t	k;

	Trf7970TurnRfOn();

	Trf7970WriteIsoControl(0x02);

	// The VCD should wait at least 2mSec after activating the
	// magnetic field before sending the first request, to
	// ensure that the VICC is ready to receive it. (ISO15693-3)
	McuDelayMillisecond(6); 								//plenty of time :)

	flags = SSC_HTDR_NA;									//non-addressed
//	flags = SSC_HTDR_ADDR;									//addressed (requires getting and storing UID first)

		buf[0] = 0x8F;										//reset FIFO
		buf[1] = 0x91;										// sending with CRC, means TRF79xx will append CRC to string going out over the air
		buf[2] = 0x3D;										// write continuous from 1D
		buf[3] = 0x00;										// upper and middle nibbles of transmit byte length
		buf[4] = 0x40;										// lower and broken nibbles of transmit byte length
		buf[5] = flags;										// ISO15693 flags
		buf[6] = 0x23;										// Read Multiple Blocks command code
		buf[7] = 0x00;										// starting block # (hardcoded for now)
		buf[8] = 0x18;										// # of blocks (0x18 = 25 blocks, hardcoded for now, see comment on 26th block )


		Trf7970ResetIrqStatus();							//clearing IRQ (just in case)

		McuCounterSet();									// TimerA set
		COUNT_VALUE = COUNT_1ms * 30;						// 30ms, not 20ms
		IRQ_CLR;											// PORT2 interrupt flag clear (inside MSP430)
		IRQ_ON;

		Trf7970RawWrite(&buf[0], 9);						//issuing the Read Multiple Blocks command

		i_reg = 0x01;
		irq_flag = 0x00;
		START_COUNTER;										//Starting Timeout

		while(irq_flag == 0x00)
		{
		}													// wait for end of TX interrupt
		RESET_COUNTER;

		McuCounterSet();									// TimerA set
		COUNT_VALUE = COUNT_1ms * 250;
		START_COUNTER;										// start timer up mode

		irq_flag = 0x00;

		while(irq_flag == 0x00)
		{
		}													// wait for interrupt
		RESET_COUNTER;

		while(i_reg == 0x01)								// wait for RX complete
		{
			k++;

			if(k == 0xFFF0)
			{
				i_reg = 0x00;
				rx_error_flag = 0x00;
			}
		}

		if( i_reg == 0xFF){		// if received block data in buffer
			if(stand_alone_flag == 1)
			{
				//found = 1;
				#ifdef ENABLE_HOST
					//Tag_Count++;
					UartPutChar('[');
					UartSendCString("Now reading 9 data blocks, data is rotated");
					UartPutChar(']');
					UartPutCrlf();
					UartSendCString("BLOCK 00:   ");
					//for(i = 5; i > 1; i--)		//is 4 bytes of block data as it comes in
					for(i = 2; i < 6; i++)			//rotates the 4 bytes of block data
					{
						UartPutByte(buf[i]);		// send block 0 data to host
					}
					UartPutCrlf();
					UartSendCString("BLOCK 01:   ");
					for(i = 6; i < 10; i++)
					{
						UartPutByte(buf[i]);		// send block 1 data to host
					}
					UartPutCrlf();
					UartSendCString("BLOCK 02:   ");
					for(i = 10; i < 14; i++)
					{
						UartPutByte(buf[i]);		// send block 2 data to host
					}
					UartPutCrlf();
					UartSendCString("BLOCK 03:   ");
					for(i = 14; i < 18; i++)
					{
						UartPutByte(buf[i]);		// send block 3 data to host
					}
					UartPutCrlf();
					UartSendCString("BLOCK 04:   ");
					for(i = 18; i < 22; i++)
					{
						UartPutByte(buf[i]);		// send block 4 data to host
					}
					UartPutCrlf();
					UartSendCString("BLOCK 05:   ");
					for(i = 22; i < 26; i++)
					{
						UartPutByte(buf[i]);		// send block 5 data to host
					}
					UartPutCrlf();
					UartSendCString("BLOCK 06:   ");
					for(i = 26; i < 30; i++)
					{
						UartPutByte(buf[i]);		// send block 6 data to host
					}
					UartPutCrlf();
					UartSendCString("BLOCK 07:   ");
					for(i = 30; i < 34; i++)
					{
						UartPutByte(buf[i]);		// send block 7 data to host
					}
					UartPutCrlf();
					UartSendCString("BLOCK 08:   ");
					for(i = 34; i < 38; i++)
					{
						UartPutByte(buf[i]);		// send block 8 data to host
					}
					UartPutCrlf();
					UartSendCString("BLOCK 09:   ");
					for(i = 38; i < 42; i++)
					{
						UartPutByte(buf[i]);		// send block 9 data to host
					}
					UartPutCrlf();
					UartSendCString("BLOCK 0A:   ");
					for(i = 42; i < 46; i++)
					{
						UartPutByte(buf[i]);		// send block 10 data to host
					}
					UartPutCrlf();
					UartSendCString("BLOCK 0B:   ");
					for(i = 46; i < 50; i++)
					{
						UartPutByte(buf[i]);		// send block 11 data to host
					}
					UartPutCrlf();
					UartSendCString("BLOCK 0C:   ");
					for(i = 50; i < 54; i++)
					{
						UartPutByte(buf[i]);		// send block 12 data to host
					}
					UartPutCrlf();
					UartSendCString("BLOCK 0D:   ");
					for(i = 54; i < 58; i++)
					{
						UartPutByte(buf[i]);		// send block 13 data to host
					}
					UartPutCrlf();
					UartSendCString("BLOCK 0E:   ");
					for(i = 58; i < 62; i++)
					{
						UartPutByte(buf[i]);		// send block 14 data to host
					}
					UartPutCrlf();
					UartSendCString("BLOCK 0F:   ");
					for(i = 62; i < 66; i++)
					{
						UartPutByte(buf[i]);		// send block 15 data to host
					}
					UartPutCrlf();
					UartSendCString("BLOCK 10:   ");
					for(i = 66; i < 70; i++)
					{
						UartPutByte(buf[i]);		// send block 16 data to host
					}
					UartPutCrlf();
					UartSendCString("BLOCK 11:   ");
					for(i = 70; i < 74; i++)
					{
						UartPutByte(buf[i]);		// send block 17 data to host
					}
					UartPutCrlf();
					UartSendCString("BLOCK 12:   ");
					for(i = 74; i < 78; i++)
					{
						UartPutByte(buf[i]);		// send block 18 data to host
					}
					UartPutCrlf();
					UartSendCString("BLOCK 13:   ");
					for(i = 78; i < 82; i++)
					{
						UartPutByte(buf[i]);		// send block 19 data to host
					}
					UartPutCrlf();
					UartSendCString("BLOCK 14:   ");
					for(i = 82; i < 86; i++)
					{
						UartPutByte(buf[i]);		// send block 20 data to host
					}
					UartPutCrlf();
					UartSendCString("BLOCK 15:   ");
					for(i = 86; i < 90; i++)
					{
						UartPutByte(buf[i]);		// send block 21 data to host
					}
					UartPutCrlf();
					UartSendCString("BLOCK 16:   ");
					for(i = 90; i < 94; i++)
					{
						UartPutByte(buf[i]);		// send block 22 data to host
					}
					UartPutCrlf();
					UartSendCString("BLOCK 17:   ");
					for(i = 94; i < 98; i++)
					{
						UartPutByte(buf[i]);		// send block 23 data to host
					}
					UartPutCrlf();
					UartSendCString("BLOCK 18:   ");
					for(i = 98; i < 102; i++)
					{
						UartPutByte(buf[i]);		// send block 24 data to host
					}

					UartPutCrlf();
					UartPutCrlf();

					McuDelayMillisecond(250);

				#endif
			}
		}

	Trf7970TurnRfOff();

	Trf7970ResetIrqStatus(); // clear any IRQs
}

//===============================================================
// NAME: void Iso15693Anticollision(u08_t *mask, u08_t length)
//
// BRIEF: Is used to perform a inventory cycle of 1 or 16 time slots.
//
// INPUTS:
//	Parameters:
//		u08_t		*mask		mask value
//		u08_t		length		number of significant bits of
//								mask value
//
// OUTPUTS:
//
// PROCESS:	[1] send command
//			[2] receive respond
//			[3] send respond to host
//
// CHANGE:
// DATE  		WHO	DETAIL
// 23Nov2010	RP	Original Code
//===============================================================

void
Iso15693Anticollision(u08_t *mask, u08_t length)		// host command 0x14
{
	u08_t	i = 1, j = 1, command[2], no_slots, found = 0;

	u08_t	*p_slot_no;
	u08_t 	slot_no[17];
	u08_t	new_mask[8], new_length, mask_size;
	u32_t	size;
	u08_t	fifo_length = 0;

	u16_t	k;

	slot_no[0] = 0x00;

	buf[0] = ISO_CONTROL;
	buf[1] = 0x02;							// Receive with no CRC, means the TRF79xx will check CRC and strip out of FIFO
	Trf7970WriteIsoControl(buf[1]);
	Trf7970ReadSingle(buf,1);
	McuDelayMillisecond(2);

	if((flags & BIT5) == 0x00)							// flag bit5 is the number of slots indicator
	{
		no_slots = 16;									// 16 slots if bit is cleared
	}
	else
	{
		no_slots = 1;									// 1 slot if bit is set
	}

	p_slot_no = &slot_no[0];							// slot number pointer

	mask_size = (((length >> 2) + 1) >> 1);				// mask_size is 1 for length = 4 or 8

	buf[0] = 0x8F;
	buf[1] = 0x91;										// send with CRC
	buf[2] = 0x3D;										// write continuous from 1D
	buf[5] = flags;										// ISO15693 flags
	buf[6] = 0x01;										// anti-collision command code

	//optional AFI should be here
	if(flags & 0x10)
	{
		// mask_size is 2 for length = 12 or 16 ;
		// and so on

		size = mask_size + 4;							// mask value + mask length + AFI + command code + flags

		buf[7] = afi;
		buf[8] = length;								// mask length
		if(length > 0)
		{
			for(i = 0; i < mask_size; i++)
            {
            	buf[9 + i] = *(mask + i);
            }
		}
		fifo_length = 9;
	}
	else
	{
		// mask_size is 2 for length = 12 or 16
		// and so on

		size = mask_size + 3;							// mask value + mask length + command code + flags

		buf[7] = length;								// mask length
		if(length > 0)
		{
			for(i = 0; i < mask_size; i++)
			{
				buf[8 + i] = *(mask + i);
			}
		}
		fifo_length = 8;
	}

	buf[3] = (char) (size >> 8);
	buf[4] = (char) (size << 4);

	Trf7970ResetIrqStatus();

	McuCounterSet();									// TimerA set
	COUNT_VALUE = COUNT_1ms * 30;						// 30ms, not 20ms
	IRQ_CLR;											// PORT2 interrupt flag clear
	IRQ_ON;

	Trf7970RawWrite(&buf[0], mask_size + fifo_length);	// Transmitting 15693 Inventory Command.

	i_reg = 0x01;
	irq_flag = 0x00;
	START_COUNTER;										//	Starting Timeout

	while(irq_flag == 0x00)
	{
	}													// wait for end of TX interrupt
	RESET_COUNTER;

	for(j = 1; j <= no_slots; j++)						// 1 or 16 available time slots
	{	rxtx_state = 1;									// prepare the external counter

		// the first UID will be stored from buf[1] upwards
		McuCounterSet();								// TimerA set
		COUNT_VALUE = COUNT_1ms * 20;
		START_COUNTER;									// start timer up mode

		irq_flag = 0x00;

		while(irq_flag == 0x00)
		{
		}												// wait for interrupt
		RESET_COUNTER;

		while(i_reg == 0x01)							// wait for RX complete
		{
			k++;

			if(k == 0xFFF0)
			{
				i_reg = 0x00;
				rx_error_flag = 0x00;
			}
		}

		command[0] = RSSI_LEVELS;						// read RSSI levels
		Trf7970ReadSingle(command, 1);
		switch(i_reg)
		{


			case 0xFF:									// if received UID in buffer
				if(stand_alone_flag == 1)
				{
					found = 1;
					#ifdef ENABLE_HOST
						Tag_Count++;
						UartPutCrlf();
						UartSendCString("ISO15693/NFC-V  UID:      ");
						UartPutChar('[');

						for(i = 10; i > 2; i--)
						{
							UartPutByte(buf[i]);		// send UID to host
							if(Tag_Count==1)
							{
								UID_holder[10-i] = buf[i];		//store the first UID
								UID_holder[9] = command[0];		//corresponding RSSI value
							}
							else if(Tag_Count==2)
							{
								UID_holder2[10-i] = buf[i];		//store the second UID
								UID_holder2[9] = command[0];		//corresponding RSSI value
							}

						}
						UartPutChar(']');
						UartPutCrlf();
						UartSendCString("RSSI LEVEL:               ");
						UartPutChar('[');
						UartPutByte(command[0]);		// RSSI levels
						UartPutChar(']');
						UartPutCrlf();
					#endif
				}
				break;

			case 0x02:									// collision occurred
				p_slot_no++;							// remember a collision was detected
				*p_slot_no = j;

				//KIM!!!!!!!!!!!!!!!!!!!
				UartPutCrlf();
				UartSendCString("COLLISION");

				found = 1;

				break;

			case 0x00:									// timer interrupt
				if(stand_alone_flag == 1)
				{
					#ifdef ENABLE_HOST
//						UartPutChar('[');				// send no-response message to host
//						UartPutChar(',');
//						UartPutByte(command[0]);		// RSSI levels
//						UartPutChar(']');
////					UartPutCrlf();
					#endif
				}
				break;

			default:
				break;
		}

		Trf7970Reset();									// FIFO has to be reset before receiving the next response

		if((no_slots == 16) && (j < 16))				// if 16 slots used send EOF(next slot)
		{
			Trf7970StopDecoders();
			Trf7970RunDecoders();
			Trf7970TransmitNextSlot();
		}
		else if((no_slots == 16) && (j == 16))			// at the end of slot 16 stop the slot counter
		{	Trf7970StopDecoders();
			Trf7970DisableSlotCounter();
		}
		else if(no_slots == 1)							// 1 slot is used
		{
			break;
		}
	}													// for

	if(found == 1)									// LED on?
	{
		LED_15693_ON;								// LEDs indicate detected ISO15693 tag


		//ADDED!!!
		//determines which RSSI value is higher or lower
		//PRINTS out the corresponding array.
		if(UID_holder[9]<UID_holder2[9])
		{
			UID_chooser = 0;
			for(i = 0;i<8;i++)
			{
				UartPutByte(UID_holder[i]);
			}
			UartPutCrlf();
		}
		else
		{
			UID_chooser = 1;
			for(i = 0;i<8;i++)
			{
				UartPutByte(UID_holder2[i]);
			}
			UartPutCrlf();
		}


		UartSendCString("***************************  ATTEMPTING TO READ!! ************************************");
		UartPutCrlf();

/* DONT READ THE BLOCK DATA
					for (ui8BlockNumber = 0; ui8BlockNumber < 0x02; ui8BlockNumber++) //0x7F support includes TI Tag-It HF-I Plus, Pro, standard and STM M24LR04E tags)
					{
						NFC_TYPEV_READ_SINGLE_BLOCK(ui8BlockNumber);
					}
					*/

	}
	else
	{
		LED_15693_OFF;
		LED_POWER_ON;
	}

	new_length = length + 4; 							// the mask length is a multiple of 4 bits

	mask_size = (((new_length >> 2) + 1) >> 1);

	while((*p_slot_no != 0x00) && (no_slots == 16) && (new_length < 61) && (slot_no[16] != 16))
	{
		*p_slot_no = *p_slot_no - 1;

		for(i = 0; i < 8; i++)
		{
			new_mask[i] = *(mask + i);				//first the whole mask is copied
		}

		if((new_length & BIT2) == 0x00)
		{
			*p_slot_no = *p_slot_no << 4;
		}
		else
		{
			for(i = 7; i > 0; i--)
			{
				new_mask[i] = new_mask[i - 1];
			}
			new_mask[0] &= 0x00;
		}
		new_mask[0] |= *p_slot_no;				// the mask is changed
		McuDelayMillisecond(2);

		Iso15693Anticollision(&new_mask[0], new_length);	// recursive call with new Mask

		p_slot_no--;
	}

	IRQ_OFF;
}														// Iso15693Anticollision

void TRF7970A_Init()
{
		// WDT ~350ms, ACLK=1.5kHz, interval timer
		WDTCTL = WDT_ADLY_16;

		// Enable WDT interrupt
		IE1 |= WDTIE;

		SLAVE_SELECT_PORT_SET;
		SLAVE_SELECT_HIGH;

		ENABLE_PORT_SET;
		ENABLE_TRF;

		// wait until TRF7970A system clock started
		McuDelayMillisecond(2);

		// settings for communication with TRF7970A
		Trf7970CommunicationSetup();

		// Set Clock Frequency and Modulation
		Trf7970InitialSettings();

		// set the DCO to 8 MHz
		McuOscSel(1);

		// Re-configure the USART with this external clock
		Trf7970ReConfig();

		// Configure UART
		UartSetup();
		McuDelayMillisecond(5);
		McuDelayMillisecond(2);

		// General enable interrupts
		__bis_SR_register(GIE);

		// indicates that setting are done
		enable = 1;

		// stand alone mode
		stand_alone_flag = 1;

		// LaunchPad LED1
		P1DIR |= BIT0;


		/****************Kim's interrupt code************************/

		P1SEL &= ~0x08;						// Select Port 1 P1.3 (push button)
	//	P1DIR |= 0x01;						//Set P1.3 as output
		P1DIR &= ~0x08;						// Port 1 P1.3 (push button) as input, 0 is input  pushbutton
		P1REN |= 0x08;						// Enable Port P1.3 (push button) pull-up resistor
	//	P1REN &= ~0x08;						// Enable pull-down resistor
		P1IE |= 0x08;						// Port 1 Interrupt Enable P1.3 (push button)

		P1IFG &= ~0x08;						// Clear interrupt flag





		//setting up timer
		CCTL0 = CCIE;
		CCR0 = 65000;
		TACTL = TASSEL_2 + ID_3 + MC_2;


		_BIS_SR(GIE); //ok interruprts

}

void Find_Tag()
{
			Tag_Count = 0;
			IRQ_OFF;
			DISABLE_TRF;

			// Enter LPM3
			__bis_SR_register(LPM3_bits);

			// launchpad LED1 - Toggle (heartbeat)
			P1OUT ^= BIT0;

			// Clear IRQ Flags before enabling TRF7970A
			IRQ_CLR;
			IRQ_ON;

			ENABLE_TRF;

			// Must wait at least 4.8 ms to allow TRF7970A to initialize.
			__delay_cycles(40000);


			//check for interrupt flag
			if(flag==1)
			{
				#ifdef ENABLE15693
				 	 Iso15693FindTag();						// Scan for 15693 tags
				#endif


				counter++;
				if(counter == 3)
				{
					Tag_Count = UartNibble2Ascii(Tag_Count & 0x0F);	// convert to ASCII
					UartSendCString("Tags Found: ");
					UartPutChar(Tag_Count);
					UartPutCrlf();
					UartSendCString("FINISHED SEARCHING");

				}
				if(counter>2){		//resets interrupt flag and vars
					flag = 0;
					counter = 0;
				}
			}
}

// Watchdog Timer interrupt service routine
#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer(void)
{
   //exit LPM3
   __bic_SR_register_on_exit(LPM3_bits);
}


//push button interrupt
#pragma vector=PORT1_VECTOR
      __interrupt void Port_1(void) {
    P1IFG &= ~0x08;	//clear interrupt flag
    //CCR0 += 1000;	//reset the timer
    flag= 1;
}

