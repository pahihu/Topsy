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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Threads/RCS/TMScheduler.c,v $
 	Author(s):             Christian Conrad & George Fankhauser
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.30 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1997/04/18 16:28:30 $      by: $Author: conrad $
	
	
*/

#include "TMScheduler.h"

#include "TMIPC.h"	/* ipcCheckPendingFlag */
#include "TMThread.h"	/* threadLock          */
#include "TMClock.h"
#include "Lock.h"

/* globals (need to be accessed in exception handler to get current context) */
Scheduler scheduler;

/* local variables */
static LockDesc sLockDesc;
static Lock schedLock = &sLockDesc;


/* Clock interrupt handler
 * this function is just for readability. we would have called schedule
 * directly.
 * it's also the function where we would place clock updates.
 */
void tmClockHandler(void *arg)
{
    schedule();
}


/* idle thread's main function, quite boring but nevertheless important 
 */
void tmIdleMain()
{
    while (TRUE) {}
}


/*
 * Initialisation of the scheduler with queues and lock.
 * this function is only used at startup to initialize the scheduler with
 * a thread (which saves us the continuous testing on running == NULL) in
 * schedule().
 */
void schedulerInit(Thread* threadPtr)
{
    int i;
    
    lockInit(schedLock);

    scheduler.running = threadPtr;
    for (i = 0; i < NBPRIORITYLEVELS; i++) {
	scheduler.prioList[i].ready = listNew();
	scheduler.prioList[i].blocked = listNew();
    }
}


/* return the current thread 
 */
Thread* schedulerRunning()
{
    return scheduler.running;
}


/*
 * A new thread structure is registered in the scheduler at its corresponding
 * priority level and at the head of the blocked queue.
 */
void schedulerInsert( Thread* threadPtr, ThreadPriority priority)
{
    threadPtr->schedInfo.status = BLOCKED;
    threadPtr->schedInfo.priority = priority;

    lock(schedLock); {
	listAddInFront( scheduler.prioList[priority].blocked, threadPtr,
		    &(threadPtr->schedInfo.hint));
    }
    unlock(schedLock);
}


/*
 * The thread structure passed as argument is removed from any queue
 */
void schedulerRemove( Thread* threadPtr)
{
    ThreadPriority priority = threadPtr->schedInfo.priority;

    lock(schedLock); {    
	if (threadPtr->schedInfo.status == BLOCKED) {
	    listRemove( scheduler.prioList[priority].blocked, threadPtr,
			threadPtr->schedInfo.hint);
	} else if (threadPtr->schedInfo.status == READY) {
	    listRemove( scheduler.prioList[priority].ready, threadPtr,
			threadPtr->schedInfo.hint);
	} else {
	    PANIC("Not possible to remove a RUNNING thread from scheduler");
	}
    }
    unlock(schedLock);
}


/*
 * The thread passed as argument is appended at the end of the ready
 * list of same priority level. It is assumed the thread is already registered
 * in the scheduler queues.
 * ! This function may run in an exception handler.
 */
Error schedulerSetReady( Thread* threadPtr)
{
    ThreadPriority priority = threadPtr->schedInfo.priority;

    if ((threadPtr->schedInfo.status == READY) ||
	(threadPtr->schedInfo.status == RUNNING)) return TM_OK;

    lock(schedLock); {
	/* swap thread from ready-list to blocked-list */
	listSwap(scheduler.prioList[priority].blocked, 
		 scheduler.prioList[priority].ready,
		 threadPtr, threadPtr->schedInfo.hint);
	threadPtr->schedInfo.status = READY;
    }
    unlock(schedLock); 
    return TM_OK;
}


/*
 * The thread passed as argument is added at the beginning of the blocked
 * list of the same priority level. It is assumed the thread is already  
 * registered in the scheduler queues.
 * ! This function may run in an exception handler.
 */
Error schedulerSetBlocked( Thread* threadPtr)
{
    ThreadPriority priority = threadPtr->schedInfo.priority;

    if (threadPtr->schedInfo.status == BLOCKED) return TM_OK;

    lock(schedLock); {
	/* swap thread from ready-list to blocked-list */
	listSwap(scheduler.prioList[priority].ready, 
		 scheduler.prioList[priority].blocked, 
		 threadPtr, threadPtr->schedInfo.hint);

	threadPtr->schedInfo.status = BLOCKED;
	/* remark: at this point, scheduler.running may point to a blocked
	 * thread. so after setBlocked we should call schedule (see kRecv).
	 */
    }
    unlock(schedLock);
    return TM_OK;
}


/*
 * The next thread with the highest priority is set to running.
 * ! This function may run in an interrupt handler.
 */
void schedule()
{
    ThreadPriority priority;
    Thread* newRunning = NULL;
    Thread* oldRunning = scheduler.running;

    /* here we may be in the clock interrupt and possible are in race 
     * condition with the thread manager (e.g. start/exit). this can be 
     * solved by testing schedLock. if it's locked we just give up because
     * we must give the thread manager a chance to release this lock.
     */
    if (!lockTry(schedLock)) return;
    else unlock(schedLock);
    
    /* next we must avoid picking a thread that makes a syscall while the
     * thread manager is in the process of updating the global list/hashlist
     * of threads.
     */
    if (!lockTry(threadLock)) return;
    else unlock(threadLock);
    
    /* Current running thread is put at the end of its ready queue,
     * unless it was previously put to sleep (by doing a msgRecv)
     */
    if (oldRunning->schedInfo.status != BLOCKED) {
	listMoveToEnd(scheduler.prioList[oldRunning->schedInfo.priority].ready,
		    oldRunning, oldRunning->schedInfo.hint);
	oldRunning->schedInfo.status = READY;
    }
    
    /* Loop over all ready queues from higher to lower priority to find
     * next thread that is allowed to run
     */
    for (priority = KERNEL_PRIORITY; priority < NBPRIORITYLEVELS; priority++) {
	listGetFirst(scheduler.prioList[priority].ready, (void**)&newRunning);
	if (newRunning != NULL) {
	    /* yup, we found a non-empty ready queue and we always pick
	     * the first one. we already put the old running thread to its
	     * queue end which results in a fine priority round-robin
	     * scheduling.
	     */
	    newRunning->schedInfo.status = RUNNING;
	    ipcResetPendingFlag(newRunning);
	    break;
	}
	/* the idle thread(s) guarantee that we always find a ready thread */
    }
    scheduler.running = newRunning;
}
