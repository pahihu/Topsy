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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Startup/RCS/Startup.c,v $
 	Author(s):             George Fankhauser
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.18 $
 	Creation Date:        
 	Last Date of Change:   $Date: 1997/04/07 07:47:50 $      by: $Author: gfa $
	
	
*/

#include "Startup.h"
#include "Memory.h"
#include "Threads.h"
#include "IO.h"
#include "IOHal.h"
#include "Configuration.h"

/* 
#define SEGMAP address -> SEGMAP is defined in the Makefile for startup 
and bootlinker in order to remain consistent */

void main() {
    Address mmStack, tmStack;
    Address userJump = ((SegMapPtr)SEGMAP)->userJumpAddress;

    initBasicExceptions();			    /* catch traps early */
    ioConsoleInit();				    /* set baud 9600 etc. */
    ioConsolePutString(BOOTMESSAGE);    
    mmInit((SegMapPtr)SEGMAP, &mmStack, &tmStack);  
						    /* startup memory init */
    tmInit(mmStack, tmStack, userJump);            /* startup thread init */
    PANIC("init returned");			  /* mm/tmInit never returns */
}

/* local dummy function needed by gcc. main() jumps always here first. this is
 * not really necessary, it's just for keeping the compiler happy
 */
static void __main() {}
