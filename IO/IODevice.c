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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/IO/RCS/IODevice.c,v $
 	Author(s):             George Fankhauser
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.17 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1997/04/18 16:27:47 $      by: $Author: conrad $
	
	
*/

#include "IODevice.h"
#include "Messages.h"
#include "Syscall.h"
#include "Threads.h"
#include "MMMapping.h"
#include "limits.h"


/* This function introduces an artificial delay of at least nbLoops (actually
 * almost three times more) to cope with timing problems between cpu and devices
 */
int ioDelayAtLeastCycles( int nbLoops)
{
    int i, k=0;
    for (i=0;i<nbLoops;i++)
      k++;
    return k;
}

static Boolean ioCheckBufferAddress(ThreadId id, Address address, 
				    unsigned long size)
{
    Address spaceFrom;
    unsigned long spaceSize;
    
    if (THREAD_MODE(id) == KERNEL) return TRUE;
    
    /* buffer must be in user space (at least). otherwise users could
     * access kernel memory.
     *
     * an exact test would require the address to be inside a vmRegion,
     * but this would be quite an expensive test
     */
    mmAddressSpaceRange(USER, &spaceFrom, &spaceSize);
    if (address < spaceFrom || (unsigned long)address + size > 
					(unsigned long)spaceFrom + spaceSize 
			|| UNSIGNED_LONG_MAX - (unsigned long)address < size)
    return FALSE;
	else
    return TRUE;
}

/* IO device main procedure. 
 * Every device driver loops here and passes messages to its 
 * driver implementation.
 * 
 * If a driver handles special messages (other than read/wrote/close/init)
 * they are forwarded to its handleMsg function.
 */
void ioDeviceMain(IODevice this) {

    Message msg, reply;
    ThreadId	expectedFrom;

    DriverMessage* driverMsg;
    int ret;
    
    while (TRUE) {
	expectedFrom = ANY;
	tmMsgRecv(&expectedFrom, ANYMSGTYPE, &msg, INFINITY);
	switch (msg.id) {
	case IO_READ:
	    reply.id = IO_READREPLY;
	    if (ioCheckBufferAddress(msg.from, msg.msg.ioRead.buffer, 
				     msg.msg.ioRead.size)) {
		ret = this->read(this, msg.from, msg.msg.ioRead.buffer,  
						&(msg.msg.ioRead.size));
		reply.msg.ioReadReply.errorCode = IO_READOK;
		reply.msg.ioReadReply.bytesRead = msg.msg.ioRead.size;
		tmMsgSend(msg.from, &reply);
	    }
	    else {
		reply.msg.ioReadReply.errorCode = IO_READFAILED;
		tmMsgSend(msg.from, &reply);
	    }
	    break;
	case IO_WRITE:
	    reply.id = IO_WRITEREPLY;
	    if (ioCheckBufferAddress(msg.from, msg.msg.ioWrite.buffer, 
				     msg.msg.ioWrite.size)) {
		ret = this->write(this, msg.from, msg.msg.ioWrite.buffer, 
						&(msg.msg.ioWrite.size));
		reply.msg.ioWriteReply.errorCode = IO_WRITEOK;
		reply.msg.ioWriteReply.bytesWritten = msg.msg.ioWrite.size;
		tmMsgSend(msg.from, &reply);
	    }
	    else {
		reply.msg.ioReadReply.errorCode = IO_WRITEFAILED;
		tmMsgSend(msg.from, &reply);
	    }
	    break;
	case IO_DRIVERCLOSE:
	    /* check sender: we do not accept others than ioThread */
	    if (msg.from != IOTHREADID) {
		reply.id = UNKNOWN_SYSCALL;
		tmMsgSend(msg.from, &reply);
		break;
	    }
	    reply.id = IO_CLOSEREPLY;
	    if (this->close != NULL) {
		ret = this->close(this);
		reply.msg.ioCloseReply.errorCode = ret;
	    }
	    else {
		reply.msg.ioCloseReply.errorCode = IO_CLOSEFAILED;
	    }
	    driverMsg = (DriverMessage*)&msg;
	    tmMsgSend(driverMsg->clientThreadId, &reply);				
	    break;
	case IO_INIT:
	    reply.id = IO_INITREPLY;
	    if (this->init != NULL) {
		ret = this->init(this);
		reply.msg.ioInitReply.errorCode = ret;
	    }
	    else {
		reply.msg.ioInitReply.errorCode = IO_INITFAILED;
	    }				
	    tmMsgSend(msg.from, &reply);
	    break;
	default:
	    if (this->handleMsg != NULL) { 
		this->handleMsg(this, &msg);
	    }
	    else {
		reply.id = UNKNOWN_SYSCALL;
		tmMsgSend(msg.from, &reply);
	    }
	}
    }
}
