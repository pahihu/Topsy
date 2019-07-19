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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Threads/RCS/TMThread.c,v $
 	Author(s):             Christian Conrad
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.49 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1998/03/26 20:35:52 $      by: $Author: gfa $
	
	
*/

#include "TMThread.h"
#include "TMScheduler.h"
#include "TMHal.h"
#include "TMIPC.h"
#include "TMMain.h"
#include "HashList.h"
#include "List.h"
#include "Lock.h"
#include "Syscall.h"
#include "Support.h"
#include "limits.h"
#include "MMMapping.h"

#define noRunningThreadString "(in init)"
#define MAXUSERTHREADID    LONG_MAX
#define MAXKERNELTHREADID  LONG_MIN

/* Global variables */
HashList threadHashList;            /* Hash list of all threads */
extern List threadList;             /* Contains all threads (user & kernel) */

LockDesc threadLockDesc;            /* lock to protect global thread lists */
Lock threadLock = &threadLockDesc;

extern ThreadId nextUserThreadId;   /* from TMMain */
extern ThreadId nextKernelThreadId;
int exceptionContextFlag = 0;       /* Set to 1 for exception context */

/*
 * Get a new thread id
 */
ThreadId getThreadId(AddressSpace space)
{
    Thread* threadPtr;
    int wraps = 0;
    Error ret;
    
    if (space == KERNEL) {
    	while (TRUE) {
	    if (nextKernelThreadId > 0) { /* negative kernel id underflow */
		wraps++;
		if (wraps == 2) PANIC("out of kernel thread id's");
		nextKernelThreadId = FIRST_KERNELTHREAD;
	    }
	    ret = hashListGet(threadHashList, (void**)(&threadPtr), 
			      nextKernelThreadId); 
	    if (ret == HASHNOTFOUND) return nextKernelThreadId--;
	    nextKernelThreadId--;
	}
    }
    else {
    	while (TRUE) {
	    if (nextUserThreadId < 0) { /* positive user id overflow */
		wraps++;
		if (wraps == 2) PANIC("out of user thread id's");
		nextUserThreadId = FIRST_USERTHREAD;
	    }
	    ret = hashListGet(threadHashList, (void**)(&threadPtr), 
			      nextUserThreadId); 
	    if (ret == HASHNOTFOUND) return nextUserThreadId++;
	    nextUserThreadId++;
	}
    }
    return 0;
}

/* This procedure initializes a thread structure. resources are passed as
 * arguments so threadBuild doesn't need to know about memory allocation.
 */
void threadBuild( ThreadId id,
		  ThreadId parentId,
		  char* name,
		  ProcContext* contextPtr,
		  Address stackBaseAddress, /* static address or 
					     * as obtained via vmAlloc */
		  unsigned int stackSize,
		  ThreadMainFunction mainFunction,
		  ThreadArg parameter,
		  AddressSpace space,
		  Boolean lightWeight,
		  Thread* threadPtr)
{
    Message threadExitMsg = {TMTHREADID, TM_EXIT, {{NULL, NULL, NULL}}};
    int exitCodeLength = FN_PTRDIFF(endAutomaticThreadExit, automaticThreadExit);
    Register mode;
    Register sp;

    threadPtr->id = id;
    threadPtr->parentId = parentId;

    if (name != NULL) {
	stringNCopy(threadPtr->name, name, sizeof(threadPtr->name));
    }
    else {
	stringNCopy(threadPtr->name, "no name", sizeof(threadPtr->name));    
    }

    initMsgQueue( &(threadPtr->msgQueue));
    /* prepare the value of the status register of the new threads context
     * it is adjusted that the new thread runs either in user or kernel
     * mode with interrupts enabled.
     */
    if (space == USER) {
	schedulerInsert(threadPtr, USER_PRIORITY);
	mode = STATUS_INT_ENABLE_USER_PREV;
    } else {
	if (!lightWeight) {
	    schedulerInsert(threadPtr, KERNEL_PRIORITY);
	} else {
	    schedulerInsert(threadPtr, IDLE_PRIORITY);
	}
	mode = STATUS_INT_ENABLE_KERNEL_PREV; 
    }
    if (!lightWeight) {
	threadPtr->stackStart = (char*)(stackBaseAddress) + stackSize - 4;
	threadPtr->stackEnd = stackBaseAddress;
    } else {
	threadPtr->stackStart = stackBaseAddress;
	threadPtr->stackEnd = stackBaseAddress; /* should never be destroyed */
    }	
    threadPtr->contextPtr = contextPtr;
    
    /* here we setup the context. register access is machine dependent, 
     * so this is done in TMHal
     */
    if (!lightWeight) {
	sp = (Register)
	  ((char*)(threadPtr->stackStart) - sizeof(Message) - exitCodeLength);
    } else {
	sp = (Register)(threadPtr->stackStart);
    }
    tmSetStackPointer(threadPtr->contextPtr, sp);
    
    /* the code at sp+4 does an automatic exit() and will be executed
     * after the main procedure of the thread does a return without an exit()
     * see also the byteCopies a few lines below...
     */
    tmSetReturnAddress(threadPtr->contextPtr, sp + 4);
    tmSetProgramCounter(threadPtr->contextPtr, (Register)mainFunction);
    tmSetFramePointer(threadPtr->contextPtr, 0);
    tmSetArgument0(threadPtr->contextPtr, (Register)parameter);
    tmSetStatusRegister(threadPtr->contextPtr, mode);
    enableAllInterruptsInContext(threadPtr->contextPtr);

    byteCopy((char*)(threadPtr->stackStart) - sizeof(Message) + 4,
	      &threadExitMsg,
	      sizeof(Message));
    byteCopy((char*)(threadPtr->stackStart) - sizeof(Message) - 
    							exitCodeLength + 4,
	      (void*)automaticThreadExit,
	      exitCodeLength);
}


/* This procedure initializes a thread structure. resources are passed as
 * arguments so threadBuild doesn't need to know about memory allocation.
 */
Error threadDestroy( ThreadId id)
{
    Thread* threadPtr;
        
    /* Lookup in hash table to find pointer onto destination thread */
    if (hashListGet( threadHashList, (void**)(&threadPtr),
		     id) != HASHOK) {
	/* inexistent threads can't call exit! */
	WARNING("threadExit(): could not find threadId");
	return -1;
    }
    
    /* Clean up hash table, global thread list and scheduler list */
    lock(threadLock); {
	hashListRemove(threadHashList, id);
	listRemove(threadList, threadPtr, NULL);
    }
    unlock(threadLock);    

    schedulerRemove(threadPtr);

    /* Deallocate stack and thread descriptor.
     * stackEnd is the basis address ! 
     */
    vmFree((Address)(threadPtr->stackEnd));  
    hmFree((Address)threadPtr);
    vmCleanup(threadPtr->id);

    /* Wake all threads that were blocked waiting for 
     * a message from exited thread 
     */
    ipcFreeBlockedThreads( id);
    
    return TM_OK;
}


/*
 *
 */
ThreadId threadStart( ThreadMainFunction fctnAddr, 
		      ThreadArg parameter,
		      AddressSpace space,
		      char* name,
		      ThreadId parentId,
		      Boolean lightWeight)
{
    Thread* threadPtr;
    Address spaceFrom;
    unsigned long spaceSize;
    Address stackBaseAddress;

    /* users may pass illegal pointers to name. this has to be checked here. 
     * we check if the user buffer is inside the user address space and
     * watch for overflow!
     */
    if (THREAD_MODE(parentId) == USER) {
        mmAddressSpaceRange(USER, &spaceFrom, &spaceSize);
	if ((Address)name < spaceFrom || (unsigned long)name + MAXNAMESIZE > 
	    (unsigned long)spaceFrom + spaceSize 
	    || UNSIGNED_LONG_MAX - (unsigned long)name < MAXNAMESIZE)
	  {
	      name = NULL;
	  }
    }    
    /* Preparation of a new Thread entry, memory allocation for both Thread
     * and ProcContext structure. For simplicity, these structures are made
     * contiguous in a single call to hmAlloc(). 
     */
    if (hmAlloc( (Address*)&threadPtr,
		 sizeof(Thread)+sizeof(ProcContext)) == HM_ALLOCFAILED) {
	ERROR("couldn't create thread structure");
	return TM_THREADSTARTFAILED;
    }

    /* Requesting an execution stack for new thread, only for
     * non-lightweight threads */
    if (!lightWeight) {
	if (vmAlloc( &stackBaseAddress,
		     TM_DEFAULTTHREADSTACKSIZE) == VM_ALLOCFAILED) {
	    ERROR("couldn't create stack for new thread");
	    /* Deallocate of threadPtr */
	    hmFree((Address)threadPtr);
	    return TM_THREADSTARTFAILED;
	}
    } else {
	stackBaseAddress = (Address)BOOTSTACKTOP;
    }	    
    /* Three things may fail: we're out of thread id's or we can't insert
     * the thread into either the hash list or the global thread list:
     * in all cases we release the already allocated resources and return
     * a syscall failure.
     */
    threadPtr->id = getThreadId(space); 
    
    lock(threadLock); {
	if ((threadPtr->id == 0) ||
	  (hashListAdd( threadHashList, threadPtr, threadPtr->id) != HASHOK) ||
	  (listAddInFront( threadList, threadPtr, NULL) != LIST_OK)) {
	    unlock(threadLock);
	    /* Deallocate stack and thread descriptor */
	    if (!lightWeight) {
		vmFree(stackBaseAddress);
	    }
	    hmFree((Address)threadPtr);
	    ERROR("got invalid thread id or list operations failed");	
	    return TM_THREADSTARTFAILED;
	}
    }
    unlock(threadLock);
    
    /* If a user thread is required, stack has to be moved to user space */
    if (space == USER) {
	if (vmMove(&stackBaseAddress, threadPtr->id) == VM_MOVEFAILED) {
	    ERROR("user stack could not be moved from kernel space");
	    vmFree(stackBaseAddress);

	    hmFree((Address)threadPtr);
	    return TM_THREADSTARTFAILED;
	}
    }

    /* Building of the thread structure */
    threadBuild( threadPtr->id, parentId, name,
		 (ProcContextPtr)((char*)threadPtr+sizeof(Thread)),
 		 stackBaseAddress, TM_DEFAULTTHREADSTACKSIZE,
		 fctnAddr, parameter, space, lightWeight, threadPtr);
    
    /* Last page of stack is protected to catch most overflows */
    /* vmProtect(...); */

    /* New thread is put in ready status */
    schedulerSetReady(threadPtr);

    /* at this point the new thread is born and may be scheduled (i.e. even
     * before the parent receives the child id as a reply message
     */
    return threadPtr->id;
}


/*
 * Handle tmExit() syscall
 */
void threadExit(ThreadId threadId)
{
    threadDestroy( threadId);
}


/*
 * This procedure is EXCLUSIVELY invoked in an exception context.
 * Thus it is possible to invoke the restoreContext() function.
 */
void threadYield()
{
    /* The next runnable thread is computed */
    schedule();
    
    /* Restoring context of new runnable thread */
    restoreContext(schedulerRunning()->contextPtr);
}


Error threadKill( ThreadId killedId, ThreadId killerId)
{
    /*Thread* threadPtr;
    int exitCodeLength = endAutomaticThreadExit - automaticThreadExit;*/

    INFO(" Entering threadKill");
    
    /* Not allowed for a user thread to kill a kernel thread */
    if (THREAD_MODE(killerId) == USER && THREAD_MODE(killedId) == KERNEL) {
	/* The faulting user thread is killed (recursively) without 
	 * remorse. the new killer id is TMTHREADID so this will hopefully
	 * happen...
	 */
	if (threadKill(killerId, TMTHREADID) == TM_KILLFAILED) {
	    return TM_THREADKILLFAILED;
	} 
	return TM_OK;
    }
    threadDestroy(killedId);
    return TM_OK;
}


/* checking for a valid user space address */
static Boolean tmCheckTMInfoAddress(ThreadId id, Address address)
{
    Address spaceFrom;
    unsigned long spaceSize;
    static unsigned long size = sizeof(struct ThreadInfo_t) ;
    
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


/* answers questions about a thread's identity, nice, huh? 
 */
Error threadInfo( ThreadId id, ThreadId aboutId,
		  ThreadInfoKind kind, ThreadInfo* infoPtr, long int values[])
{
    Thread* threadPtr ;
    
    switch (kind) {
 
    case SPECIFIC_ID:
	if (aboutId == SELF) {
	    aboutId = id;
	}
	/* Lookup in hash table to find pointer onto aboutId thread */
	if (hashListGet( threadHashList, (void**)(&threadPtr),
						    aboutId) != HASHOK) {
	    return TM_FAILED;
	}
	values[0] = aboutId;                 /* own thread id */
	values[1] = threadPtr->parentId;     /* parent thread id */
	break;
 
    case GETFIRST:
	if (listGetFirst(threadList, (void **)(&threadPtr)) != LIST_OK) {	
	    return TM_FAILED; 
	}
	if (threadPtr == NULL) {
	    return TM_FAILED; 
	}
	if (tmCheckTMInfoAddress(id, infoPtr) == FALSE) {
	    return TM_FAILED; 
	}
	infoPtr->tid = threadPtr->id;
	infoPtr->ptid = threadPtr->parentId;
	stringCopy(infoPtr->name,threadPtr->name);
	infoPtr->status = threadPtr->schedInfo.status;
	break;
    
    case GETNEXT:
	if (listGetNext(threadList, (void **)(&threadPtr)) != LIST_OK) { 
	    return TM_FAILED; 
	}
	if (threadPtr == NULL) {
	    return TM_FAILED; 
	}
	if (tmCheckTMInfoAddress(id, infoPtr) == FALSE) {
	    return TM_FAILED;
	}
	infoPtr->tid = threadPtr->id;
	infoPtr->ptid = threadPtr->parentId;
	stringCopy(infoPtr->name, threadPtr->name);
	infoPtr->status = threadPtr->schedInfo.status;
	break;
    
    default:
	WARNING("invalid info requested");
	return TM_FAILED ;
	break;
    }
    return TM_OK;
}


/*
 * Returns name of current running thread. Could be replace by a direct
 * reference to (schedulerRunning())->name, however, such a function
 * provides more transparency (the global scheduler variable would have
 * to be declared everywhere where PANIC or WARNING is used !
 */
char* getCurrentThreadName()
{
    if (schedulerRunning() == NULL) {
	return noRunningThreadString;
    } else {
	return schedulerRunning()->name;
    }
}
