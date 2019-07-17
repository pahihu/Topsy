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


	File:                  $Source: /proj/topsy/ss98/Topsy/IO/Drivers/RCS/SCN2681_DUART.c,v $
 	Author(s):             George Fankhauser
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.15 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1998/04/08 15:08:22 $      by: $Author: gfa $
	
	
*/

#include "Drivers/SCN2681_DUART.h"
#include "Messages.h"
#include "IODevice.h"

void devSCN2681_interruptHandler(IODevice this) 
{
    devSCN2681 uartA, uartB;
    IODevice devA, devB;
    char* rxA;
    char* rxB;
    char* statusA;
    char* statusB;

    devA = this;
    uartA = (devSCN2681)(this->extension);
    devB = uartA->otherChannel;
    uartB = (devSCN2681)(devB->extension);
    rxA = (char*)((unsigned long)(devA->base) + RX_REGISTER);
    rxB = (char*)((unsigned long)(devB->base) + RX_REGISTER);
    statusA = (char*)((unsigned long)(devA->base) + STATUS_REGISTER);
    statusB = (char*)((unsigned long)(devB->base) + STATUS_REGISTER);

    /* Since the interrupt on this device is shared, we need to check which
     * channel received a character
     */
    if ((*statusA & RECV_MASK) == RECV_MASK) {
	if ((uartA->in + 1) % SCN2681_BUFSIZE == uartA->out) {
	    WARNING("UARTA overflow\n");
	    return;
	}
	devA->buffer[uartA->in] = *rxA;
	uartA->in = ((uartA->in + 1) % SCN2681_BUFSIZE);
    } 
    if ((*statusB & RECV_MASK) == RECV_MASK) {
	if ((uartB->in + 1) % SCN2681_BUFSIZE == uartB->out) {
	    WARNING("UARTB overflow\n");
	    return;
	}
	devB->buffer[uartB->in] = *rxB;
	uartB->in = ((uartB->in + 1) % SCN2681_BUFSIZE);
    }     
}


	      
Error devSCN2681_init(IODevice this) 
{
    char* command = (char*)((unsigned long)(this->base) + COMMAND_REGISTER);
    char* mode = (char*)((unsigned long)(this->base) + MODE_REGISTER);
    char* status = (char*)((unsigned long)(this->base) + STATUS_REGISTER);
    char* interrupt = (char*)((unsigned long)(this->base) + INTERRUPT_REGISTER);
    void* tmp;
    devSCN2681 uart;

    /* Both UART register sets are identical, except for the interrupt
     * mask register
     */
    if (this->interruptHandler == NULL) {
	interrupt -= UART_REGISTER_SET_SIZE;
    }
    
    /* device init */
    *command = 0xa;	/* disable rx/tx */
    ioDelayAtLeastCycles( NBCYCLESFORDELAY);
    *mode = 0x13;	/* no parity - 8 bits/char */
    ioDelayAtLeastCycles( NBCYCLESFORDELAY);
    *mode = 0x7;	/* 1 stop bit */
    ioDelayAtLeastCycles( NBCYCLESFORDELAY);
    *status = 0xbb;	/* 9600 baud */
    ioDelayAtLeastCycles( NBCYCLESFORDELAY);
    *interrupt = 0x22;	/* both receiver interrupts enabled */
    ioDelayAtLeastCycles( NBCYCLESFORDELAY);
    *command = 0x5;	/* enable receive/transmit */
    ioDelayAtLeastCycles( NBCYCLESFORDELAY);
    
    /* buffers init - be careful to check for existing buffers */
    if (!this->isInitialised) {
	tmp = this->extension;
	hmAlloc((Address*)&(this->buffer), SCN2681_BUFSIZE);
	hmAlloc(&(this->extension), sizeof(devSCN2681Desc));
	uart = (devSCN2681)this->extension;
	uart->otherChannel = (IODevice)tmp;

	this->isInitialised = TRUE;
    }
    else {
	uart = (devSCN2681)this->extension;
    }
    uart->in = 0;
    uart->out = 0;

    return IO_INITOK;
}


Error devSCN2681_read(IODevice this, ThreadId threadId, char* buffer, 
							    long int* size) 
{
    int i = 0;
    devSCN2681 uart;
    uart = (devSCN2681)this->extension;

    while (TRUE) {
    	/* no more characters available, buffer empty
	 */
	if (uart->in == uart->out) {
	    break;
	}
	/* user is satisfied
	 */
	if (i >= *size) {
	    break;
	}
	
	buffer[i++] = this->buffer[uart->out];
	uart->out = ((uart->out + 1) % SCN2681_BUFSIZE);
    }
    *size = i;
    return IO_READOK;
}


Error devSCN2681_write(IODevice this, ThreadId threadId, char* buffer, long int* size) 
{
    char* transmit = (char*)((unsigned long)(this->base) + TX_REGISTER);
    char* status = (char*)((unsigned long)(this->base) + STATUS_REGISTER);
    int i;

    for (i = 0; i < *size; i++) {
	while ((*status & READY_TO_SEND) != READY_TO_SEND) {
	    ioDelayAtLeastCycles( NBCYCLESFORDELAY);
	}				/* wait for uart to become ready */
	*transmit = buffer[i];		/* send it finally */

	/* Translate newlines into carriage returns plus newlines for
	 * correct terminal output
	 */
	if (buffer[i] == '\n') {
	    while ((*status & READY_TO_SEND) != READY_TO_SEND) {
		ioDelayAtLeastCycles( NBCYCLESFORDELAY);
	    }				/* wait for uart to become ready */
	    *transmit = '\r';		/* send it finally */
	}
	ioDelayAtLeastCycles( NBCYCLESFORDELAY);	    
    }    
    return IO_WRITEOK;
}


Error devSCN2681_close(IODevice this) 
{
    /* this message is forwarded from ioThread */
    
    /* release buffers if any */
    
    return IO_CLOSEOK;
}
