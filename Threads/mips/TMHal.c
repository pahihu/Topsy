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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Threads/mips/RCS/TMHal.c,v $
 	Author(s):             Christian Conrad
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.21 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1997/04/23 09:10:28 $      by: $Author: gfa $
	
	
*/

#include "TMHal.h"
#include "IOHal.h"
#include "Support.h"
#include "TMClock.h"

/* Exception Context, used by the general exception handler before calling
 * saveContext(). This is a minimal save that allows to call saveContext as
 * a proper procedure.
 * - exceptionContext[0] <- a0
 * - exceptionContext[1] <- ra
 * - exceptionContext[2] <- sp
 * - exceptionContext[3] <- pc
 */
Register exceptionContext[4];

/* Exception service branch table (exception handlers).
 * - 16 exception handlers,
 * - 16 reserved (not set)
 * - 1 UTLB handler (offset 32 in array) 
 */
ExceptionHandler exceptionHandlerTable[MAXNBOFEXCEPTIONS+1];

/* Hardware interrupt service branch table (interrupt handlers) and
 * corresponding arguments that may be passed to the interrupt handler.
 * - 6 hw interrupts
 * - 2 sw interrupts 
 */
InterruptHandler interruptHandlerTable[MAXNBOFINTERRUPTS];
void* interruptHandlerArgTable[MAXNBOFINTERRUPTS];


/* 8254 Register Addressing structure */
typedef struct TimerRegisters_t{
    unsigned char counter0;
    unsigned char pad1[3];
    unsigned char counter1;
    unsigned char pad2[3];
    unsigned char counter2;
    unsigned char pad3[3];
    unsigned char ctrlWord;
    unsigned char pad4[3];
} TimerRegisters;


void tmInstallExceptionCode() 
{
    /* Installation of default basic exception handlers */
    /* INSTALL generalExceptionHandler AT LOCATION 0x8000_0080 */
    byteCopy( (Address)VEC_EXCEPTION, generalExceptionHandler, 
	      endGeneralExceptionHandler-generalExceptionHandler);

    /* INSTALL UTLBMissHandler AT LOCATION 0x8000_0000 */
    if (endUTLBMissHandler-UTLBMissHandler > 0x00000080) 
	PANIC("utlbmisshandler larger than 0x80 bytes");
    byteCopy( (Address)VEC_TLB_UMISS, UTLBMissHandler,  
	      endUTLBMissHandler-UTLBMissHandler);

    /* RESETHandler AT LOCATION 0xbfc0_0000 is in EPROMs */
}    


InterruptHandler tmSetInterruptHandler( InterruptId intId, 
					InterruptHandler intHdler, void* arg)
{
    InterruptHandler oldIntHdler = interruptHandlerTable[intId];

    if (intId == SWINT_0 || intId == SWINT_1) {
	interruptHandlerTable[intId-6] = intHdler;
	interruptHandlerArgTable[intId-6] = arg;
    } else {
	interruptHandlerTable[intId+2] = intHdler;
	interruptHandlerArgTable[intId+2] = arg;
    }
    return oldIntHdler;
}


ExceptionHandler tmSetExceptionHandler( ExceptionId excId, 
					ExceptionHandler excHdler)
{
    ExceptionHandler oldExcHdler = exceptionHandlerTable[excId];

    exceptionHandlerTable[excId] = excHdler;
    return oldExcHdler;
}


void intDispatcher( Register causeReg)
{
    int offset=-1;

    causeReg &= CAUSE_IP_MASK;
    causeReg >>= CAUSE_IP_SHIFT;

    /* The invariant here is causeReg>0, otherwise an hardware
       error would have occurred */
    while (causeReg != 0) {
	causeReg >>= 1;
	offset += 1;
    }
    
    /* we call here the handler for interrupt offset with
     * its (optionally) defined argument
     */
    (interruptHandlerTable[offset])(interruptHandlerArgTable[offset]);

    tmResetClockInterrupt(offset-2);
}


void tmSetReturnValue(ProcContextPtr contextPtr, Register value)
{
    contextPtr->returnValue[0] = value;
}

void tmSetStackPointer(ProcContextPtr contextPtr, Register value)
{
    contextPtr->stackPointer = value;
}

void tmSetReturnAddress(ProcContextPtr contextPtr, Register value)
{
    contextPtr->returnAddress = value;
}

void tmSetProgramCounter(ProcContextPtr contextPtr, Register value)
{
    contextPtr->programCounter = value;
}

void tmSetStatusRegister(ProcContextPtr contextPtr, Register value)
{
    contextPtr->statusRegister = value;
}

void tmSetFramePointer(ProcContextPtr contextPtr, Register value)
{
    contextPtr->framePointer = value;
}

void tmSetArgument0(ProcContextPtr contextPtr, Register value)
{
    contextPtr->argument[0] = value;
}

void tmSetArgument1(ProcContextPtr contextPtr, Register value)
{
    contextPtr->argument[1] = value;
}

void enableInterruptInContext( InterruptId intId, ProcContextPtr contextPtr)
{
    unsigned long int bitToSet = 1;
    
    if (intId < SWINT_0) {  /* either CLOCKINT, CENTRONICS, or DUART */
	bitToSet <<= (INTMASK_SHIFT+2+intId);
    } else {
	bitToSet <<= (INTMASK_SHIFT+intId-SWINT_0);
    }
    contextPtr->statusRegister |= bitToSet;
}
    
void disableInterruptInContext( InterruptId intId, ProcContextPtr contextPtr)
{
    unsigned long int bitToSet = 0;
    
    if (intId < SWINT_0) {  /* either CLOCKINT, CENTRONICS, or DUART */
	bitToSet <<= (INTMASK_SHIFT+2+intId);
    } else {
	bitToSet <<= (INTMASK_SHIFT+intId-SWINT_0);
    }
    contextPtr->statusRegister |= bitToSet;
}
    
void enableAllInterruptsInContext( ProcContextPtr contextPtr)
{
    contextPtr->statusRegister |= SR_INT_MASK;
}
