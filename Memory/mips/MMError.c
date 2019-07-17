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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Memory/mips/RCS/MMError.c,v $
 	Author(s):             George Fankhauser
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.9 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1998/04/07 15:30:50 $      by: $Author: gfa $
	
	
*/

#include "MMError.h"
#include "Threads.h"
#include "Messages.h"
#include "TMHal.h"


static void mmBusError(ThreadId currentThread);
static void mmAddressError(ThreadId currentThread);
static void mmTLBError(ThreadId currentThread);
static void mmUTLBError(ThreadId currentThread);
static void mmError(ThreadId currentThread, char* errorString);


void mmInstallErrorHandlers(void)
{
    tmSetExceptionHandler(TLB_MOD, mmTLBError);
    tmSetExceptionHandler(TLB_LOAD, mmTLBError);
    tmSetExceptionHandler(TLB_STORE, mmTLBError);
    tmSetExceptionHandler(UTLB_MISS, mmUTLBError);
    tmSetExceptionHandler(ADDR_ERR_LOAD, mmAddressError);
    tmSetExceptionHandler(ADDR_ERR_STORE, mmAddressError);
    tmSetExceptionHandler(BUSERR_INSTR, mmBusError);
    tmSetExceptionHandler(BUSERR_DATA, mmBusError);
}


static void mmError(ThreadId currentThread, char* errorString)
{
    Message msg;
    
    msg.id = TM_KILL;
    msg.from = MMTHREADID;
    msg.msg.tmKill.id = currentThread;
    
    ERROR(errorString);
    printRegisters();

    /* immediate in-kernel delivery without syscall
     * kSend does a schedule after the message arrived
     */
    kSend(TMTHREADID, &msg); 
}

static void mmBusError(ThreadId currentThread)
{
    mmError(currentThread, "bus error");
}


static void mmAddressError(ThreadId currentThread)
{
    mmError(currentThread, "address error");
}

static void mmTLBError(ThreadId currentThread)
{
    mmError(currentThread, "tlb error");
}

static void mmUTLBError(ThreadId currentThread)
{
    mmError(currentThread, "user tlb error");
}
