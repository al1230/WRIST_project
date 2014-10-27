
//===============================================================
// Program with hardware USART and SPI communication	        ;
// interface with TRF7970A reader chip.                         ;
//                                                              ;
// PORT1.0 - HEART BEAT LED                                     ;
// PORT1.1 - UART RX                                            ;
// PORT1.2 - UART TX                                            ;
// PORT1.5 - SPI DATACLK                                        ;
// PORT1.6 - SPI MISO (REMOVE LED2 JUMPER)                      ;
// PORT1.7 - SPI MOSI                                           ;
//                                                              ;
// PORT2.0 - IRQ (INTERUPT REQUEST from TRF7970A)  ----> P2.4   ;
// PORT2.1 - SLAVE SELECT        ------> P2.3                   ;
// PORT2.2 - TRF7970A ENABLE                                    ;
// PORT2.3 - ISO14443B LED                                      ;
// PORT2.4 - ISO14443A LED                                      ;
// PORT2.5 - ISO15693  LED                                      ;
//===============================================================

/********** HEADER FILES **********/
//===============================================================
#include <msp430.h>                         // processor specific header
#include <stdlib.h>                         // general purpose standard library
#include <stdio.h>                          // standard input/output header
#include "iso14443a.h"
#include "iso14443b.h"
#include "iso15693.h"
#include "mcu.h"
#include "Trf7970.h"
#include "types.h"
#include "uart.h"
//===============================================================

/********** GLOBAL VARIABLES **********/
//===============================================================
u08_t buf[140];					// TX/RX BUFFER FOR TRF7970A
u08_t enable = 0;
u08_t Tag_Count;
u08_t i_reg = 0x01;             // INTERRUPT REGISTER
u08_t irq_flag = 0x00;
u08_t rx_error_flag = 0x00;
s08_t rxtx_state = 1;           // USED FOR TRANSMIT RECEIVE BYTE COUNT
u08_t host_control_flag = 0;
u08_t stand_alone_flag = 1;

//Kim
int UID_chooser = 0;
volatile int flag = 0;
volatile int counter = 0;
u08_t UID_holder[10];
u08_t UID_holder2[10];
//===============================================================

void main(void)
{
	TRF7970A_Init();

	while(1)
	{
		Find_Tag();
	}
}
