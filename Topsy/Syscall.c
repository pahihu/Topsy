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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Topsy/RCS/Syscall.c,v $
 	Author(s):             Eckart Zitzler, Christian Conrad, George Fankhauser
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.29 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1998/03/26 20:34:37 $      by: $Author: gfa $
	
	
*/


#include "Topsy.h"
#include "Messages.h"
#include "Memory.h"
#include "Threads.h"
#include "IO.h"
#include "Syscall.h"

#define SyscallError MessageError


/* generic syscall handling 
 * this procedure is used for the regular syscalls but is also intended
 * for special messages sent to any server (drivers, user space servers)
 */
SyscallError genericSyscall(ThreadId to, Message* message, Message* reply)
{
    if (tmMsgSend(to, message) != TM_MSGSENDOK) {
	    return TM_MSGSENDFAILED;
    }
    tmMsgRecv(&to, reply->id, reply, INFINITY);
    return TM_MSGRECVOK;
}


/* memory management */

SyscallError vmAlloc(Address *addressPtr, unsigned long int size)
{
    Message message, reply;
    
    message.id = VM_ALLOC;
    message.msg.vmAlloc.size = size;
    reply.id = VM_ALLOCREPLY;
    
    if (genericSyscall(MMTHREADID, &message, &reply) == TM_MSGSENDFAILED)
    	return VM_ALLOCFAILED;
    
    *addressPtr = reply.msg.vmAllocReply.address;
    return reply.msg.vmAllocReply.errorCode;
}


SyscallError vmFree(Address address)
{
    Message message, reply;
    
    message.id = VM_FREE;
    message.msg.vmFree.address = address;
    reply.id = VM_FREEREPLY;
    
    if (genericSyscall(MMTHREADID, &message, &reply) == TM_MSGSENDFAILED)
    	return VM_FREEFAILED;
    
    return reply.msg.vmFreeReply.errorCode;
}


SyscallError vmMove(Address *addressPtr, ThreadId newOwner)
{
    Message message, reply;
    
    message.id = VM_MOVE;
    message.msg.vmMove.address = *addressPtr;
    message.msg.vmMove.newOwner = newOwner;
    reply.id = VM_MOVEREPLY;
    
    if (genericSyscall(MMTHREADID, &message, &reply) == TM_MSGSENDFAILED)
    	return VM_MOVEFAILED;
    
    *addressPtr = reply.msg.vmMoveReply.address;
    return reply.msg.vmMoveReply.errorCode;
}


SyscallError vmProtect(Address startAddress, unsigned long int size,
		       ProtectionMode pmode)
{
    Message message, reply;
    
    message.id = VM_PROTECT;
    message.msg.vmProtect.startAddress = startAddress;
    message.msg.vmProtect.size = size;
    message.msg.vmProtect.pmode = pmode;
    reply.id = VM_PROTECTREPLY;
    
    if (genericSyscall(MMTHREADID, &message, &reply) == TM_MSGSENDFAILED)
    	return VM_PROTECTFAILED;
    
    return reply.msg.vmProtectReply.errorCode;
}


SyscallError vmCleanup(ThreadId threadId)
{
    Message message;
    
    message.id = VM_CLEANUP;
    message.msg.vmCleanup.threadId = threadId;
    
    if (tmMsgSend(MMTHREADID, &message) != TM_MSGSENDOK) {
        return VM_CLEANUPFAILED;
    }
    return VM_CLEANUPOK;
}


/* thread management */
/* NOTE: tmMsgSend() and tmMsgRecv() are implemented in SyscallMsg.S */

SyscallError tmStart( ThreadId* id,
		      ThreadMainFunction function,
		      ThreadArg parameter, 
		      char *name)
{
    Message message, reply;
    
    message.id = TM_START;
    message.msg.tmStart.fctnAddress = function;
    message.msg.tmStart.parameter = parameter;
    message.msg.tmStart.name = name;
    reply.id = TM_STARTREPLY;
    
    if (genericSyscall(TMTHREADID, &message, &reply) == TM_MSGSENDFAILED)
    	return TM_STARTFAILED;

    *id = reply.msg.tmStartReply.newId;
    return reply.msg.tmStartReply.errorCode;
}


SyscallError tmKill( ThreadId id)
{
    Message message, reply;
    
    message.id = TM_KILL;
    message.msg.tmKill.id = id;
    reply.id = TM_KILLREPLY;
    
    if (genericSyscall(TMTHREADID, &message, &reply) == TM_MSGSENDFAILED)
    	return TM_KILLFAILED;
    
    return reply.msg.tmKillReply.errorCode;
}


void tmYield()
{
    Message message;

    /* Building of a Message */
    message.id = TM_YIELD;
    
    /* Sending of message */
    if (tmMsgSend( TMTHREADID,
		   &message) != TM_MSGSENDOK ) {
    }
    /* No reply is expected */
}


void tmExit()
{
    Message message;

    /* Building of a Message */
    message.id = TM_EXIT;
    
    /* Sending of message */
    while (tmMsgSend(TMTHREADID, &message) != TM_MSGSENDOK) {
	/* try until we succeed */
    }
    /* No reply is expected (i.e. the sender does no longer exist) */
}


SyscallError tmGetInfo(ThreadId about, ThreadId* tid, ThreadId* ptid)
{
    Message message, reply;
    
    message.id = TM_INFO;
    message.msg.tmInfo.kind = SPECIFIC_ID ; 
    message.msg.tmInfo.about = about;
 
    reply.id = TM_INFOREPLY;
    
    if (genericSyscall(TMTHREADID, &message, &reply) == TM_MSGSENDFAILED)
        return TM_INFOFAILED;
    
    *tid = reply.msg.tmInfoReply.info[0] ;
    *ptid = reply.msg.tmInfoReply.info[1] ;
  
    return reply.msg.tmInfoReply.errorCode;
}


SyscallError tmGetFirst(ThreadInfo* info)
{
    Message message, reply;
    
    message.id = TM_INFO ;
    message.msg.tmInfo.kind = GETFIRST ; 
    message.msg.tmInfo.infoPtr = info;
    reply.id = TM_INFOREPLY;
    
    if (genericSyscall(TMTHREADID, &message, &reply) == TM_MSGSENDFAILED)
        return TM_INFOFAILED;
    return reply.msg.tmInfoReply.errorCode;
}


SyscallError tmGetNext(ThreadInfo* info)
{
    Message message, reply;
    
    message.id = TM_INFO ;
    message.msg.tmInfo.kind = GETNEXT ; 
    message.msg.tmInfo.infoPtr = info;
    reply.id = TM_INFOREPLY;
    
    if (genericSyscall(TMTHREADID, &message, &reply) == TM_MSGSENDFAILED)
        return TM_INFOFAILED;
    return reply.msg.tmInfoReply.errorCode;
}

/* io interface */

SyscallError ioOpen(int deviceNumber, ThreadId* id) {
    Message message, reply;
    
    message.id = IO_OPEN;
    message.msg.ioOpen.deviceNumber = deviceNumber;
    reply.id = IO_OPENREPLY;
    
    if (genericSyscall(IOTHREADID, &message, &reply) == TM_MSGSENDFAILED)
    	return IO_OPENFAILED;
    
    *id = reply.msg.ioOpenReply.deviceThreadId;
    return reply.msg.ioOpenReply.errorCode;
}


SyscallError ioClose(ThreadId id) {
    Message message, reply;
    ThreadId any = ANY;
    message.id = IO_CLOSE;
    message.msg.ioClose.deviceThreadId = id;
    reply.id = IO_CLOSEREPLY;
    
    if (tmMsgSend(IOTHREADID, &message) != TM_MSGSENDOK) {
	    return TM_MSGSENDFAILED;
    }
    /* ioCloseReply may come from a driver or a driver (that's why &any) */
    tmMsgRecv(&any, reply.id, &reply, INFINITY);
    return TM_MSGRECVOK;
    
    return reply.msg.ioCloseReply.errorCode;
}


SyscallError ioRead(ThreadId id, char* buffer, unsigned long int* nOfBytes) {
    Message message, reply;
    
    message.id = IO_READ;
    message.msg.ioRead.buffer = buffer;
    message.msg.ioRead.size = *nOfBytes;	
    reply.id = IO_READREPLY;
    
    if (genericSyscall(id, &message, &reply) == TM_MSGSENDFAILED)
    	return IO_READFAILED;
    
    *nOfBytes = reply.msg.ioReadReply.bytesRead;
    return reply.msg.ioReadReply.errorCode;
}


SyscallError ioWrite(ThreadId id, char* buffer, unsigned long int* nOfBytes) {
    Message message, reply;
    
    message.id = IO_WRITE;
    message.msg.ioWrite.buffer = buffer;
    message.msg.ioWrite.size = *nOfBytes;	
    reply.id = IO_WRITEREPLY;
    
    if (genericSyscall(id, &message, &reply) == TM_MSGSENDFAILED)
    	return IO_WRITEFAILED;
    
    *nOfBytes = reply.msg.ioWriteReply.bytesWritten;
    return reply.msg.ioWriteReply.errorCode;
}


SyscallError ioInit(ThreadId id) {
    Message message, reply;
    
    message.id = IO_INIT;
    reply.id = IO_INITREPLY;
    
    if (genericSyscall(id, &message, &reply) == TM_MSGSENDFAILED)
    	return IO_INITFAILED;
    
    return reply.msg.ioInitReply.errorCode;
}
