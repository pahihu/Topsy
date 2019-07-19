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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Threads/RCS/TMInit.c,v $
 	Author(s):             Christian Conrad
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.35 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1997/04/23 09:06:36 $      by: $Author: gfa $
	
	
*/

#include "cpu.h"
#include "Topsy.h"
#include "TMHal.h"
#include "TMInit.h"
#include "TMClock.h"
#include "HashList.h"
#include "List.h"
#include "Support.h"
#include "Threads.h"
#include "TMIPC.h"
#include "TMScheduler.h"
#include "TMThread.h"
#include "TMError.h"

/* Global variables */
List threadList;                  /* Contains all threads (user & kernel) */
extern HashList threadHashList;   /* Hash list of all threads */
extern Scheduler scheduler;       /* Scheduler structure */

/* Statically allocated thread structures for tm and mm */
Thread mmThread, tmThread;
ProcContext mmContext, tmContext;

void dummyExceptionHandler(ThreadId id)
{
    WARNING("No exception handler has been set yet");
}

void dummyInterruptHandler(void *ptr)
{
    WARNING("No interrupt handler has been set yet");
}

void initBasicExceptions()
{
    int i;
    
    /* setup the exception and interrupt handling at its correct place.
     * this is highly machine dependent (vectorized exceptions vs. software
     * dispatch (as on the mips).
     */
    tmInstallExceptionCode();
	
    /* Initialization of default dummy exception handlers */
    for (i = 0; i < MAXNBOFEXCEPTIONS; i++) {
	tmSetExceptionHandler(i, dummyExceptionHandler);
    }
    for (i = 0; i < MAXNBOFINTERRUPTS; i++) {
	tmSetInterruptHandler(i, dummyInterruptHandler, NULL);
    }
}


void tmInit( Address mmStack, Address tmStack, Address userInit)
{
    /* Initialisation of the scheduler data structures.
     * The first thread is passed as argument.
     */
    schedulerInit(&tmThread);
    lockInit(threadLock);
    
    /* build mm and tm threads */
    threadBuild( MMTHREADID, 0, "mmThread", &mmContext,
		 mmStack, TM_DEFAULTTHREADSTACKSIZE,
		 mmMain, (ThreadArg)0, KERNEL, FALSE, &mmThread);
    threadBuild( TMTHREADID, 0, "tmThread", &tmContext,
		 tmStack, TM_DEFAULTTHREADSTACKSIZE,
		 tmMain, (ThreadArg)userInit, KERNEL, FALSE, &tmThread);

    /* Store pointer to mm and tm in threadHashList hash table */
    threadHashList = hashListNew();
    hashListAdd( threadHashList, &mmThread, mmThread.id);
    hashListAdd( threadHashList, &tmThread, tmThread.id);

    /* add mm and tm to threadList */
    threadList = listNew();
    listAddInFront(threadList, &mmThread, NULL);
    listAddInFront(threadList, &tmThread, NULL);
    
    /* Setting of default handlers for SYSCALL and Hardware exceptions */
    tmInstallErrorHandlers();

    /* Threads mm and tm are set to ready status, scheduling decision */
    schedulerSetReady(&mmThread);
    schedulerSetReady(&tmThread);

    INFO("MM and TM threads initialized and set to ready status");
    // ioConsolePutString("userInit: ");
    // ioConsolePutHexInt(userInit);
    // ioConsolePutString("\n");
    
    schedule();					/* first thread is picked */

    /* Clock configuration */
    if (setClockValue( CLOCK0, TIMESLICE, RATEGENERATOR ) != TM_OK) {
	PANIC("Clock could not be properly configured");
    }
    
    /* Restoring context of first thread to be activated */
    INFO("clock ready, restoring first kernel thread");
    
    restoreContext(scheduler.running->contextPtr);
    /*** initialization of the kernel ends here, from this point 
     *** we run in kernel mode with interrupts enabled.
     *** now this is the real thing (tm)...
     ***/
}
