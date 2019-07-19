#ifndef __TMIPC_H
#define __TMIPC_H
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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Threads/RCS/TMIPC.h,v $
 	Author(s):             Christian Conrad
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.17 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1997/04/23 09:04:54 $      by: $Author: gfa $
	
	
*/

#include "tm-include.h"

/*
 * IPC interface functions
 */
Error kRecv(Message *msg, Thread* threadPtr);
Error kSend(ThreadId destThreadId, Message* msgPtr);

/* Invoked by the general exception routine to process SYSCALLS */
int msgDispatcher( ThreadId dummyThreadid,    /* not used */
		   Message* msgPtr,           /* message reference */
		   unsigned long int timeout, /* timeout for receiving */
		   MsgOpCode code,       /* SYSCALL_SEND_OP/SYSCALL_RECV_OP */
		   unsigned long int brancheDelayBit);
                                         /* directly from CAUSE Register */

/* Initialisation of the message queue */
void initMsgQueue( MessageQueue* queuePtr);

void ipcResetPendingFlag(Thread* threadPtr);
void ipcFreeBlockedThreads( ThreadId exitThreadId);

#endif /* __TMIPC_H */
