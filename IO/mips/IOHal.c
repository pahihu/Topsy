/*
	Copyright (c) 1996-1997 Swiss Federal Institute of Technology, 
	Computer Engineering and Networks Laboratory. All rights reserved.

	TOPSY - A Teachable Operating System. 
		Implementation of a tiny and simple micro kernel for
		teaching purposes.

	Permission to use, copy, modify, and distribute this software and its
	documentation for any purpose, without fee, and without written 
	agreement is hereby granted, provided that the above copyright notice 
	and the following two paragraphs appear in all copies of this software.


	
	File:                  $Source: /proj/topsy/ss98/Topsy/IO/mips/RCS/IOHal.c,v $
 	Author(s):             George Fankhauser
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.20 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1998/04/08 15:08:11 $      by: $Author: gfa $
	
	
*/

#include "Topsy.h"
#include "IOHal.h"
#include "IODevice.h"


/* very simple console output routine. accesses hardware directly, used at 
 * startup and on catastrophic events/debugging...
 */ 
void ioConsoleInit() {
    char* command = (char*)(CONSOLE_BASE + COMMAND_REGISTER);
    char* mode = (char*)(CONSOLE_BASE + MODE_REGISTER);
    char* status = (char*)(CONSOLE_BASE + STATUS_REGISTER);
    char* interrupt = (char*)(UART_A_BASE + INTERRUPT_REGISTER);

    *command = 0xa;	/* disable rx/tx */
    ioDelayAtLeastCycles( NBCYCLESFORDELAY);
    *mode = 0x13;	/* no parity - 8 bits/char */
    ioDelayAtLeastCycles( NBCYCLESFORDELAY);
    *mode = 0x7;	/* 1 stop bit */
    ioDelayAtLeastCycles( NBCYCLESFORDELAY);
    *status = 0xbb;	/* 9600 baud */
    ioDelayAtLeastCycles( NBCYCLESFORDELAY);
    *interrupt = 0x0;	/* select external clock disable ints */
    ioDelayAtLeastCycles( NBCYCLESFORDELAY);
    *command = 0x5;	/* enable receive/transmit */
    ioDelayAtLeastCycles( NBCYCLESFORDELAY);
}


void ioConsolePutChar(char c)
{
    char* transmit; 
    char* status;

    transmit = (char*)(CONSOLE_BASE + TX_REGISTER);
    status = (char*)(CONSOLE_BASE + STATUS_REGISTER);
    /* Wait for UART to become ready */
    while ((*status & READY_TO_SEND) != READY_TO_SEND) {
	ioDelayAtLeastCycles( NBCYCLESFORDELAY);
    }
    *transmit = c;			/* send it finally */

    /* Translate newlines into carriage returns plus newlines for
     * correct terminal output
     */
    if (c == '\n') {
	while ((*status & READY_TO_SEND) != READY_TO_SEND) {
	    ioDelayAtLeastCycles( NBCYCLESFORDELAY);
	}
	*transmit = '\r';			/* send it finally */
    }
    ioDelayAtLeastCycles( NBCYCLESFORDELAY);
}


