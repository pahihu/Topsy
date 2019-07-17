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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Threads/RCS/TMMain.c,v $
 	Author(s):             
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.23 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1998/04/01 11:48:18 $      by: $Author: gfa $
	
	
*/

#include "Syscall.h"
#include "TMMain.h"
#include "IOMain.h"
#include "Memory.h"
#include "TMThread.h"
#include "TMScheduler.h"
#include "HashList.h"

/* Global variables */
HashList blockedThreadsOnRecvHashList;

/* Module global variables */
ThreadId nextUserThreadId=FIRST_USERTHREAD;     /* first user thread */
ThreadId nextKernelThreadId=FIRST_KERNELTHREAD; /* first reg. kernel thread */


void tmMain( ThreadArg userInitAddress)
{
    ThreadId ioThreadId, initThreadId, newThreadId, idleThreadId;
    ThreadId fromId;
    Message message, reply;           /* generic messages */
    Error errorCode;
    long int values[2];
    
    INFO("Entering tmMain()");

    /* Starting of a new thread for IO processing */
    if ((ioThreadId = threadStart(ioMain, NULL, KERNEL, "ioThread", 
    				  TMTHREADID, FALSE)) == TM_THREADSTARTFAILED) {
	PANIC("ioThread could not be started");
    }

    /* Starting of the first user thread (code at userInitAddress) */
    if ((initThreadId = threadStart( (ThreadMainFunction)userInitAddress,
				     NULL, USER, "shell", TMTHREADID, FALSE))
	== TM_THREADSTARTFAILED) {
	PANIC("userThread could not be started");
    }

    /* Starting a lightweight idle thread */
    if ((idleThreadId = threadStart( (ThreadMainFunction)tmIdleMain,
				     NULL, KERNEL, "idleThread", TMTHREADID, TRUE))
	== TM_THREADSTARTFAILED) {
	PANIC("idleThread could not be started");
    }

    /* Entering main loop to process incoming messages */
    while (TRUE) {
	fromId = ANY;
	tmMsgRecv( &fromId, ANYMSGTYPE, &message, INFINITY);
	
	switch (message.id) {
	case TM_START:
	  if ((newThreadId =
	       threadStart( message.msg.tmStart.fctnAddress,
			    message.msg.tmStart.parameter,
			    THREAD_MODE(message.from),
			    message.msg.tmStart.name,
			    message.from,
			    FALSE)) == TM_THREADSTARTFAILED) {
	      reply.msg.tmStartReply.errorCode = TM_STARTFAILED;
	  } else {
	      reply.msg.tmStartReply.errorCode = TM_STARTOK;
	      reply.msg.tmStartReply.newId = newThreadId;
	  }
	  reply.id = TM_STARTREPLY;
	  if ((errorCode = tmMsgSend( message.from,
				      &reply)) != TM_MSGSENDOK ) {
	      WARNING("tmStartReplyMsg could not be sent\n");
	  }
	  break;
	  
	case TM_EXIT:
	  threadExit( message.from);
	  break;
	  
	case TM_KILL:
	  if ((errorCode = threadKill( message.msg.tmKill.id,
				       message.from)) == TM_THREADKILLFAILED) {
	      reply.msg.tmKillReply.errorCode = TM_KILLFAILED; 
	  } else {
	      reply.msg.tmKillReply.errorCode = TM_KILLOK; 
	  }
	  reply.id = TM_KILLREPLY;
	  tmMsgSend(message.from, &reply);
	  break;

	case TM_INFO:
	  if ((errorCode = threadInfo(	message.from,
					message.msg.tmInfo.about,
					message.msg.tmInfo.kind,
					message.msg.tmInfo.infoPtr,
					values)) != TM_OK) {
	      reply.msg.tmInfoReply.errorCode = TM_INFOFAILED; 
	  } else {
	      reply.msg.tmInfoReply.errorCode = TM_INFOOK; 
	      reply.msg.tmInfoReply.info[0] = values[0];
	      reply.msg.tmInfoReply.info[1] = values[1];
	  }
	  reply.id = TM_INFOREPLY;
	  if ((errorCode = tmMsgSend( message.from,
				      &reply)) != TM_MSGSENDOK ) {
	      WARNING("tmInfoReplyMsg could not be sent\n");
	  }
	  break;
	  
	case TM_KILLREPLY:
	    /* simply digest Message (coming from oneself) */
	    break;
	    
	default:
	  /* Unknown system call received */
	  WARNING("thread manager: unknown message type received\n");
	  reply.id = UNKNOWN_SYSCALL;
	  if ((errorCode = tmMsgSend( message.from, &reply)) != TM_MSGSENDOK ){
	      WARNING("syscallReply could not be sent\n");
	  }
	  break; 
	}	
    }
}
