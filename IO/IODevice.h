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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/IO/RCS/IODevice.h,v $
 	Author(s):             George Fankhauser
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.10 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1997/04/17 11:01:07 $      by: $Author: conrad $
	
	
*/
#ifndef _IODEVICE_H_
#define _IODEVICE_H_

#include "Topsy.h"
#include "Threads.h"

/* driver messages */
#define IO_DRIVERCLOSE -1


/* Forward declaration of IODevice
 */
typedef struct IODeviceDesc_t * IODevice;

typedef Error(*ReadFunction)(IODevice this, ThreadId, char*, long int*);
typedef Error(*WriteFunction)(IODevice this, ThreadId, char*, long int*);
typedef Error(*CloseFunction)(IODevice this);
typedef Error(*InitFunction)(IODevice this);
typedef void(*MessageHandler)(IODevice this, Message*);

typedef struct IODeviceDesc_t {
	char* name;
	Address base;
	char* buffer;
	InterruptId interrupt;
	InterruptHandler interruptHandler;
	ReadFunction read;
	WriteFunction write;
	CloseFunction close;
	InitFunction init;
	MessageHandler handleMsg;
        Boolean isInitialised;
	void* extension;
} IODeviceDesc;


typedef struct DriverMessage_t {
    ThreadId from;		   /* Sender of the message */
    MessageId id;		   /* Type of the message */
	ThreadId clientThreadId;
	long int empty[2];
} DriverMessage;

void ioDeviceMain(IODevice this);

int ioDelayAtLeastCycles( int nbLoops);

#endif
