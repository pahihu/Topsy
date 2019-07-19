#ifndef __TMHAL_H
#define __TMHAL_H
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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Threads/mips/RCS/TMHal.h,v $
 	Author(s):             Christian Conrad
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.22 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1997/04/23 13:26:49 $
	
	
*/

#include "tm-include.h"
#include "cpu.h"
#include "Exception.h"

#define MAXNBOFEXCEPTIONS    32
#define MAXNBOFINTERRUPTS     8

#define STATUS_INT_ENABLE_KERNEL_PREV (SR_IEp | (SR_KUp & 0))
#define STATUS_INT_ENABLE_USER_PREV (SR_IEp | SR_KUp)

/* Processor dependent thread information */
typedef struct ProcContext_t {
    Register returnValue[2];       /* v0 - v1 */
    Register argument[4];          /* a0 - a3 */
    Register callerSaved[10];      /* t0 - t9 */
    Register calleeSaved[8];       /* s0 - s7 */
    Register globalPointer;        /* gp */
    Register stackPointer;         /* sp */
    Register framePointer;         /* fp */
    Register returnAddress;        /* ra */
    Register programCounter;       /* pc */
    Register hilo[2];              /* hi, lo multiplier registers */
    Register statusRegister;       /* sr after trapping */
    Register assemblerTemp;        /* assembler temp, used also by gcc */
} ProcContext;

/* Saving and restoring current thread context (assembler procedures) */
void saveContext(ProcContextPtr contextPtr);
void restoreContext(ProcContextPtr contextPtr);

/* operations on the thread context are machine dependent and 
 * encapsulated in the following functions. On CISC machines
 * most of these ops must be implemented using the current SP of
 * the context.
 */
void tmSetReturnValue(ProcContextPtr contextPtr, Register value);
void tmSetStackPointer(ProcContextPtr contextPtr, Register value);
void tmSetReturnAddress(ProcContextPtr contextPtr, Register value);
void tmSetProgramCounter(ProcContextPtr contextPtr, Register value);
void tmSetStatusRegister(ProcContextPtr contextPtr, Register value);
void tmSetFramePointer(ProcContextPtr contextPtr, Register value);
void tmSetArgument0(ProcContextPtr contextPtr, Register value);
void tmSetArgument1(ProcContextPtr contextPtr, Register value);

void enableInterruptInContext( InterruptId intId, ProcContextPtr contextPtr);
void disableInterruptInContext( InterruptId intId, ProcContextPtr contextPtr);
void enableAllInterruptsInContext( ProcContextPtr contextPtr);

/* installs code by copying generalExceptionHandler()'s and 
 * UTLBMissHandler()'s code at certain mips specific addresses
 */
void tmInstallExceptionCode();

/* Syscall exception handler (set per default as exception handler) */
void syscallExceptionHandler(ThreadId threadId);

/* Hardware exception handler (set per default as exception handler) */
void hwExceptionHandler();

/* Automatic exit of a thread when stack underflow occurs */
void automaticThreadExit();
void endAutomaticThreadExit();

/* Symbols for the exception handlers (they are implemented in assembly) */
void UTLBMissHandler();
void endUTLBMissHandler();

void generalExceptionHandler();
void endGeneralExceptionHandler();

#endif /* __TMHAL_H */
