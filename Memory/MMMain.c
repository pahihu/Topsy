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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Memory/RCS/MMMain.c,v $
 	Author(s):             George Fankhauser
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.16 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1997/04/23 09:19:22 $      by: $Author: gfa $
	
	
*/

#include "MMMain.h"
#include "Topsy.h"
#include "Messages.h"
#include "Syscall.h"
#include "Threads.h"
#include "MMHeapMemory.h"
#include "MMVirtualMemory.h"


void mmMain(ThreadArg arg)
{
    Message            msg, reply;
    Address            address;
    ThreadId	expectedFrom;

    INFO("Entering mmMain()");
    
    reply.from = MMTHREADID; /* sent always from this thread */

    while (TRUE) {
	expectedFrom = ANY;
        if (tmMsgRecv(&expectedFrom, ANYMSGTYPE, &msg, INFINITY) == 
							TM_MSGRECVFAILED)
	    PANIC("message receive error!");
	switch (msg.id) {
	case VM_ALLOC:
	    reply.id = VM_ALLOCREPLY;
	    reply.msg.vmAllocReply.errorCode = 
			mmVmAlloc(&address, msg.msg.vmAlloc.size, msg.from);
	    reply.msg.vmAllocReply.address = address;
	    tmMsgSend(msg.from, &reply);
	    break;
	case VM_FREE:
	    reply.id = VM_FREEREPLY;
	    reply.msg.vmFreeReply.errorCode = 
				    mmVmFree(msg.msg.vmFree.address, msg.from);
	    tmMsgSend(msg.from, &reply);
	    break;
        case VM_PROTECT: 
	    reply.id = VM_PROTECTREPLY;
	    reply.msg.vmProtectReply.errorCode = mmVmProtect(
						msg.msg.vmProtect.startAddress, 
						msg.msg.vmProtect.size,
						msg.msg.vmProtect.pmode, 
						msg.from);
	    tmMsgSend(msg.from, &reply);	    
	    break;
	case VM_MOVE:				/* must be a kernel thread */
	    reply.id = VM_MOVEREPLY;
	    if (THREAD_MODE(msg.from) == KERNEL) {
		address = msg.msg.vmMove.address;
		reply.msg.vmMoveReply.errorCode = mmVmMove(&address, 
						msg.msg.vmMove.newOwner);
		reply.msg.vmMoveReply.address = address;
	    }
	    else {
		reply.msg.vmMoveReply.errorCode = VM_MOVEFAILED;
	    }
	    tmMsgSend(msg.from, &reply);	    
	    break;
	case VM_CLEANUP:			/* must be a kernel thread */
	    if (THREAD_MODE(msg.from) == KERNEL) {
		mmVmCleanup(msg.msg.vmCleanup.threadId);
	    }
	    /* this kernel internal message does not reply */
	    break;
	case TM_KILLREPLY:
	    /* simply digest Message (coming from tmThread) */
	    break;
	default:
	    reply.id = UNKNOWN_SYSCALL;
	    tmMsgSend(msg.from, &reply);
	    break;
	} 
    } 
} 
