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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Topsy/RCS/Syscall.h,v $
 	Author(s):             Eckart Zitzler, Christian Conrad, George Fankhauser
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.17 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1998/03/26 20:34:37 $      by: $Author: gfa $
	
	
*/

#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include "Topsy.h"
#include "Messages.h"
#include "Memory.h"
#include "Threads.h"
#include "IO.h"

#define SyscallError  MessageError


/* memory management */

SyscallError  vmAlloc(Address *addressPtr, unsigned long int  size);
SyscallError  vmFree(Address address);
SyscallError  vmMove(Address *addressPtr, ThreadId newOwner);
SyscallError  vmProtect(Address  startAdr, unsigned long int  size,
                ProtectionMode  pmode);
SyscallError  vmCleanup(ThreadId threadId);

/* thread management */

SyscallError tmMsgSend( ThreadId to, Message *msg);
SyscallError tmMsgRecv( ThreadId* from,
			MessageId msgId,
			Message* msg,
			int timeOut);
SyscallError tmStart( ThreadId* id,
		      ThreadMainFunction mainFunction, 
		      ThreadArg parameter, 
		      char *name);
SyscallError tmKill(ThreadId id);
void tmExit();
void tmYield();
SyscallError tmGetInfo(ThreadId about, ThreadId* tid, ThreadId* ptid);
SyscallError tmGetFirst(ThreadInfo* info);
SyscallError tmGetNext(ThreadInfo* info);


/* io interface */

SyscallError ioOpen(int deviceNumber, ThreadId* id);
SyscallError ioClose(ThreadId id);
SyscallError ioRead(ThreadId id, char* buffer, unsigned long int* nOfBytes);
SyscallError ioWrite(ThreadId id, char* buffer, unsigned long int* nOfBytes);
SyscallError ioInit(ThreadId id);

#endif
