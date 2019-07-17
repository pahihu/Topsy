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

	
	File:                  $Source: /proj/topsy/ss98/Topsy/IO/Drivers/RCS/SCN2681_DUART.h,v $
 	Author(s):             George Fankhauser
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.8 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1997/04/14 21:08:16 $      by: $Author: conrad $
	
	
*/
#include "Topsy.h"
#include "MMHeapMemory.h"
#include "IODevice.h"
#include "Configuration.h"

#define MODE_REGISTER 0x0	/* SCN2681 register layout */
#define STATUS_REGISTER 0x4
#define COMMAND_REGISTER 0x8
#define TX_REGISTER 0x0c
#define RX_REGISTER TX_REGISTER
#define INTERRUPT_REGISTER 0x14

#define READY_TO_SEND 0x04	/* ready to send status */
#define RECV_MASK 0x01		/* a byte has arrived */ 

#define SCN2681_BUFSIZE 128

#define UART_REGISTER_SET_SIZE 0x20   /* size for one channel */

typedef struct devSCN2681Desc_t {
    unsigned long in, out;
    IODevice otherChannel;
} devSCN2681Desc;

typedef devSCN2681Desc* devSCN2681;

void devSCN2681_interruptHandler(IODevice this);
Error devSCN2681_init(IODevice this);
Error devSCN2681_read(IODevice this, ThreadId threadId, char* buffer, long int* size);
Error devSCN2681_write(IODevice this, ThreadId threadId, char* buffer, long int* size);
Error devSCN2681_close(IODevice this);
