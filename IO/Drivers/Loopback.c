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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/IO/Drivers/RCS/Loopback.c,v $
 	Author(s):             George Fankhauser
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.1 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1997/05/30 18:03:30 $      by: $Author: gfa $
	
	
*/

#include "Drivers/Loopback.h"
#include "Messages.h"

Error devLoopback_init(IODevice this) 
{
    devLoopback loopback;
    
    /* buffers init - be careful to check for existing buffers */
    if (!this->isInitialised) {
	hmAlloc((Address*)&(this->buffer), LOOPBACK_BUFSIZE);
	hmAlloc(&(this->extension), sizeof(devLoopbackDesc));
	loopback = (devLoopback)this->extension;

	this->isInitialised = TRUE;
    }
    else {
	loopback = (devLoopback)this->extension;
    }
    loopback->in = 0;
    loopback->out = 0;
    loopback->count = 0;

    return IO_INITOK;
}

Error devLoopback_read(IODevice this, ThreadId threadId, char* buffer, 
								long int* size) 
{
    int i;
    devLoopback lb;
    lb = (devLoopback)this->extension;

    for (i = 0; i < *size; i++) {

    	if (lb->count == 0) break;

	buffer[i] = this->buffer[lb->out];
	lb->out = (lb->out + 1) % LOOPBACK_BUFSIZE;
	lb->count--;
    }
    *size = i;    
    return IO_READOK;
}															
															
Error devLoopback_write(IODevice this, ThreadId threadId, char* buffer, 
								long int* size)
{
    int i;
    devLoopback lb;
    lb = (devLoopback)this->extension;

    for (i = 0; i < *size; i++) {
	
	if (lb->count == LOOPBACK_BUFSIZE) break;
	
	this->buffer[lb->in] = buffer[i];
	lb->in = (lb->in + 1) % LOOPBACK_BUFSIZE;
	lb->count++;
    }    
    *size = i;
    return IO_WRITEOK;
}

Error devLoopback_close(IODevice this)
{
	return IO_CLOSEOK;
}
