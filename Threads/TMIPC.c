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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Threads/RCS/TMIPC.c,v $
 	Author(s):             
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.42 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1998/04/11 20:21:42 $      by: $Author: gfa $
	
	
*/

#include "TMIPC.h"
#include "TMThread.h"
#include "TMScheduler.h"
#include "TMHal.h"
#include "Support.h"
#include "HashList.h"
#include "List.h"

/* Global variables */
extern HashList threadHashList;         /* Hash list of all threads */
extern List threadList;                 /* Linear list of all threads */
    

/*
 * When a thread exits (either exited or killed), a linear search is performed
 * among all receive-blocked threads to make them ready again if they expected
 * anything from the no longer existing thread (no infinite waiting for msg!).
 */
void ipcFreeBlockedThreads( ThreadId exitThreadId)
{
    Thread* threadPtr;
 
    if (listGetFirst( threadList, (void**)&threadPtr) != LIST_OK) {
        PANIC("listGetFirst() did not work");
    }
    while (threadPtr != NULL) {
	if (threadPtr->schedInfo.status == BLOCKED) {
	    if (threadPtr->msgQueue.threadIdPending == exitThreadId) {
		/* threadPtr must be re-activated with a failed return value */
		tmSetReturnValue(threadPtr->contextPtr, TM_MSGRECVFAILED);
		schedulerSetReady(threadPtr);
	    }
	}
	else if (threadPtr->schedInfo.status == READY) {
	} else {
	}
        listGetNext( threadList, (void**)&threadPtr);
    }
}


/*
 * Initialisation of the message queue
 */
void initMsgQueue( MessageQueue* queuePtr)
{
    int i;
    
    queuePtr->freeList = 0;
    queuePtr->busyList = -1;
    for (i=0;i<MAXNBMSGINQUEUE;i++)
      queuePtr->ctrl[i] = (i+1) % MAXNBMSGINQUEUE;
    queuePtr->ctrl[MAXNBMSGINQUEUE-1] = -1;
    queuePtr->msgPendingStatus = NOT_WAITING;
}


/*
 * Check if the pending flag can be removed.
 * This procedure is only invoked by the scheduler before setting a new
 * thread as the next to be scheduled. If the pending flag for the new
 * thread is set, then it can be removed, as it is guaranteed that the
 * new thread will then have access to its message.
 */
void ipcResetPendingFlag(Thread* threadPtr)
{
    threadPtr->msgQueue.msgPendingStatus = NOT_WAITING;
}


/*
 * A new message is inserted in the queue of the receiving thread
 */
static int addMessageInQueue(Thread* threadPtr, Message* msgPtr)
{
    MessageQueue* queue = &(threadPtr->msgQueue);
    int oldFreeList = queue->freeList;
    int tmpBusyList = queue->busyList;
    Message threadKillMsg = {TMTHREADID, TM_KILL, {{NULL, NULL, NULL}}};

    if (queue->freeList == -1) {
	/* Message queue full */
	ERROR("addMessageInQueue(): msg queue is full");

	/* If the thread is in user space, then it is killed */
	if (THREAD_MODE(threadPtr->id) == USER) {
	    ioConsolePutString("(thread ");
	    ioConsolePutString(threadPtr->name);
	    ioConsolePutString(" will be killed)\n");
	    
	    threadKillMsg.msg.tmKill.id = threadPtr->id;
	    kSend( TMTHREADID, &threadKillMsg);
	}
	return TM_MSGQUEUEOVERFLOW;
    } else {
	/* Copy message at position queue->freeList */
	longCopy( &(queue->msg[queue->freeList]),
		  msgPtr,
		  sizeof(Message));
	/* Updating queue->freeList */
	queue->freeList = queue->ctrl[queue->freeList];
	/* Updating queue->busyList */
	if (queue->busyList == -1) {
	    queue->busyList = oldFreeList;
	    queue->ctrl[oldFreeList] = -1;
	} else {
	    while (queue->ctrl[tmpBusyList] != -1) {
		tmpBusyList = queue->ctrl[tmpBusyList];
	    }
	    queue->ctrl[tmpBusyList] = oldFreeList;
	    queue->ctrl[oldFreeList] = -1;
	}
    }
    return TM_OK;
}


/*
 * A single message is retrieved from the message queue.
 * Message must be of type msgPtr->id and sender msgPtr->from as
 * requested by the tmMsgRecv() call.
 */
static int getMessageFromQueue( 
			Thread* threadPtr, /* thread about to receive a msg */
			Message* msgPtr)   /* message structure */
{
    MessageQueue* queue = &(threadPtr->msgQueue);
    int tmpBusyList = queue->busyList;
    int tmp = queue->busyList;
    MessageId expectedMsgId = msgPtr->id;
    ThreadId expectedThreadId = msgPtr->from;
    Boolean notFound = TRUE;
    Message msg;

    if (queue->busyList == -1) {
	return TM_NOSUCHMESSAGE;
    }
    while (tmpBusyList != -1 && notFound) {
	/* Check if message 'msg' at position 'tmpBusyList' matches request */
	msg = queue->msg[tmpBusyList];
	if ( expectedMsgId == ANYMSGTYPE && expectedThreadId == ANY) {
	    notFound = FALSE;
	} else if (expectedMsgId==ANYMSGTYPE && expectedThreadId==msg.from) {
	    notFound = FALSE;
	} else if ( expectedMsgId == msg.id && expectedThreadId == ANY) {
	    notFound = FALSE;
	} else if ( expectedMsgId == msg.id && expectedThreadId == msg.from) {
	    notFound = FALSE;
	}
	if (notFound == FALSE) {
	    longCopy( msgPtr, &msg, sizeof(Message));
	    /* Update of queue->busyList */
	    if (tmpBusyList == queue->busyList) {
		queue->busyList = queue->ctrl[tmpBusyList];
		queue->ctrl[tmpBusyList] = queue->freeList;
		queue->freeList = tmpBusyList;
	    } else {
		while ( queue->ctrl[tmp] != tmpBusyList) {
		    tmp = queue->ctrl[tmp];
		}
		queue->ctrl[tmp] = queue->ctrl[tmpBusyList];
		/* Update of queue->freeList */
		queue->ctrl[tmpBusyList] = queue->freeList;
		queue->freeList = tmpBusyList;
	    }
	}
	tmpBusyList = queue->ctrl[tmpBusyList];
    }
    if (notFound) {
	return TM_NOSUCHMESSAGE;
    } else {
	return TM_OK;
    }
}


Error kSend(ThreadId destThreadId, Message* msgPtr)
{
    Thread* destThreadPtr;
    Error errorCode = TM_OK;
    MessageQueue* queue;  /* used for simplification in below expressions */
    Boolean copyInTmp = FALSE;        /* help flag */

    /* Lookup in hash table to find pointer onto destination thread */
    if ((hashListGet( threadHashList, (void**)(&destThreadPtr), 
		      destThreadId)) != HASHOK) {
	return TM_THREADNOTFOUND;
    }

    switch( destThreadPtr->schedInfo.status) {
    case RUNNING:
    case READY:
      /* Destination thread is not waiting for a message, copy in queue */
      errorCode = addMessageInQueue( destThreadPtr, msgPtr);
      break;
    case BLOCKED:
      /* Thread is expecting a message, check if there is a match */
      queue = &(destThreadPtr->msgQueue);
      /* Check status of pending message */
      if (queue->msgPendingStatus==FILLED || queue->msgPendingStatus==NOT_WAITING) { 
	  copyInTmp = FALSE; /* do not overwrite previous unread message */
      } else if ( queue->threadIdPending == ANY &&
		  queue->msgIdPending == ANYMSGTYPE) {
	  copyInTmp = TRUE;
      } else if ( queue->threadIdPending == schedulerRunning()->id &&
		  queue->msgIdPending == ANYMSGTYPE) {
	  copyInTmp = TRUE;
      } else if ( queue->threadIdPending == ANY &&
		  queue->msgIdPending == msgPtr->id) {
	  copyInTmp = TRUE;
      } else if ( queue->threadIdPending == schedulerRunning()->id &&
		  queue->msgIdPending == msgPtr->id) {
	  copyInTmp = TRUE;
      }
      if (copyInTmp) {
	  queue->msgPendingStatus = FILLED;
	  /* Message must be copied at the address stated in msgPendingPtr */
	  longCopy( queue->msgPendingPtr, msgPtr, sizeof(Message));
	  
	  /* The destination thread is set to READY status (wake up) */
	  schedulerSetReady( destThreadPtr);
      } else {
	  /* Message has to be copied in message queue */
	  errorCode = addMessageInQueue( destThreadPtr, msgPtr);	  
      }
      break;
    }
    if (errorCode == TM_OK) {
	/* A new scheduling decision is necessary if a higher priority
	 * thread has to become active 
	 */
	schedule();
    }
    return errorCode;
}


Error kRecv( Message* msgPtr,   /* message structure (contains exp. 
				 * sender and type) 
				 */
	     Thread* threadPtr) /* thread wishing to receive a message */
{
    Error errorCode;
    MessageQueue* queuePtr = &(threadPtr->msgQueue);

    /* Check to see if the expected message is already in the queue */
    if ((errorCode=getMessageFromQueue( threadPtr, msgPtr)) == TM_OK) {
	/* Expected message was copied in msgPtr */
	return TM_OK;
    } else {
	/* The expected message has not yet arrived in the message queue */
	queuePtr->msgPendingStatus = WAITING;     /* thread waiting for a 
						   * special message */
	queuePtr->msgPendingPtr = msgPtr;         /* address where the message 
	    					   * is expected */
	queuePtr->threadIdPending = msgPtr->from; /* expected message sender */
	queuePtr->msgIdPending = msgPtr->id;      /* expected message type */
	return TM_NOSUCHMESSAGE;
    }
}


/* A SYSCALL exception occurred, thus either tmMsgSend() or tmMsgRecv() was
 * invoked with a message.
 */	  
int msgDispatcher( ThreadId fromId,       /* thread that caused exception */
		   Message* msgPtr,       /* message reference */
		   unsigned long int timeout,/* timeout for receiving */
		   MsgOpCode code,        /* SYSCALL_SEND_OP/SYSCALL_RECV_OP */
		   unsigned long int branchDelayBit)
                                          /* directly from CAUSE Register */
{
    Error errorCode = TM_OK;
    Thread* threadPtr;
    Thread* fromThreadPtr;
    ThreadId to;
    Message killMsg;
    
    /* since we write no silly code we can assume that we never get caught
     * here. if we would allow syscalls in BD slots we would have to interpret
     * the faulting branch in software (which is not funny).
     */     
    if (branchDelayBit == CAUSE_BD) {
	ERROR("Syscall in branch delay slot");
	killMsg.id = TM_KILL;
	killMsg.from = TMTHREADID;
	killMsg.msg.tmKill.id = fromId;
	kSend(TMTHREADID, &killMsg);
	return TM_OK;
    }
    /* Lookup in hash table to find pointer onto threadId */
    if ((errorCode = hashListGet( threadHashList, (void**)(&threadPtr),
				  fromId)) != HASHOK) {
	PANIC("msgDispatcher(): required thread id does not exist");
    }
    switch (code) {
    case SYSCALL_SEND_OP:
	/* Filtering TM_YIELD:
	 * No message is sent to the thread manager, as it would be obliged
	 * to perform a new scheduling decision and a restoreContext() outside
	 * of an exception context, which leads to tricky problems.
	 * Instead, threadYield() is directly invoked from this point in an
	 * exception context.
	 */
	if (msgPtr->id==TM_YIELD && msgPtr->from==TMTHREADID) {
	    tmSetReturnValue(threadPtr->contextPtr, TM_MSGSENDOK);
	    threadYield();
	} else {
	    to = msgPtr->from;
	    msgPtr->from = fromId;		/* copy sender id to msg */
	    if (kSend( to, msgPtr) != TM_OK ) {
		tmSetReturnValue(threadPtr->contextPtr, TM_MSGSENDFAILED);
	    } else {    
		/* message was successfully copied in messageQueue */
		tmSetReturnValue(threadPtr->contextPtr, TM_MSGSENDOK);
	    }
	    /* A new scheduling decision was taken in kSend(), as kSend() may
	     * be called from other modules, independently of msgDispatcher() 
	     */
	}
	break;
	
    case SYSCALL_RECV_OP:

	/* Check if the expected threadId exists */
	if ((hashListGet(threadHashList, (void**)(&fromThreadPtr),msgPtr->from)
			!= HASHOK) && (msgPtr->from != ANY)) {
	    tmSetReturnValue(threadPtr->contextPtr, TM_MSGRECVFAILED);
	} else {
	    if (kRecv( msgPtr, threadPtr) != TM_OK ) {
		/* No corresponding message found, put receiver to sleep, 
		 * and make a new scheduling decision 
		 */

		/*if (threadPtr->id > 0) {
		    ioConsolePutString("thread id ");
		    ioConsolePutInt(threadPtr->id);
		    ioConsolePutString(" blocked, waiting\n");
		}*/
		schedulerSetBlocked(threadPtr);
		schedule();
	    } 
	    /* The expected message was copied in msgPtr */
	    tmSetReturnValue(threadPtr->contextPtr, TM_MSGRECVOK);
	    break;
	}
    }
    return TM_OK;
}
