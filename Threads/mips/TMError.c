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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Threads/mips/RCS/TMError.c,v $
 	Author(s):             Christian Conrad
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.3 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1998/04/07 15:31:38 $      by: $Author: gfa $
	
	
*/

#include "TMError.h"
#include "Threads.h"
#include "Messages.h"
#include "TMHal.h"
#include "TMScheduler.h"


static void tmReservedInstructionError(ThreadId currentThread);
static void tmCPunusableError(ThreadId currentThread);
static void tmOverflowError(ThreadId currentThread);
static void tmBreakpointError(ThreadId currentThread);


void tmInstallErrorHandlers(void)
{
    /* Vital for correct operations
     */
    tmSetExceptionHandler(SYSCALL, syscallExceptionHandler);
    tmSetExceptionHandler(HARDWARE_INT, hwExceptionHandler);
    tmSetInterruptHandler(CLOCKINT_0, tmClockHandler, NULL);

    /* Error handling for faulting threads
     */
    tmSetExceptionHandler(RES_INSTR, tmReservedInstructionError);
    tmSetExceptionHandler(CP_UNUSABLE, tmCPunusableError);
    tmSetExceptionHandler(OVERFLOW, tmOverflowError);
    tmSetExceptionHandler(BREAKPOINT, tmBreakpointError);
}


static void tmError(ThreadId currentThread, char* errorString)
{
    Message msg;
    
    msg.id = TM_KILL;
    msg.from = TMTHREADID;
    msg.msg.tmKill.id = currentThread;
    
    ERROR(errorString);
    printRegisters();

    /* immediate in-kernel delivery without syscall
     * kSend does a schedule after the message arrived
     */
    kSend(TMTHREADID, &msg); 
}

static void tmReservedInstructionError(ThreadId currentThread)
{
    tmError(currentThread, "reserved instruction");
}

static void tmCPunusableError(ThreadId currentThread)
{
    tmError(currentThread, "CP unusable");
}

static void tmOverflowError(ThreadId currentThread)
{
    tmError(currentThread, "arithmetic overflow");
}

static void tmBreakpointError(ThreadId currentThread)
{
    tmError(currentThread, "breakpoint");
}

