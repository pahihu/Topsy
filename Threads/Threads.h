#ifndef __THREADS_H
#define __THREADS_H
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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Threads/RCS/Threads.h,v $
 	Author(s):             Christian Conrad
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.30 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1997/04/07 13:03:06 $      by: $Author: gfa $
	
	
*/
#include "Topsy.h"
#include "Messages.h"
#include "Error.h"
#include "Exception.h"

#define FIRST_USERTHREAD 2
#define FIRST_KERNELTHREAD IOTHREADID /* besides mm, tm, io */

/* MM thread is *really* the first one (id -1) */
/* this macro makes invalid id's USER          */
#define THREAD_MODE(x) (x <= MMTHREADID ? KERNEL : USER)

/*
 * Exported data types
 */
typedef void (*ExceptionHandler)(ThreadId id);
typedef void (*InterruptHandler)(void* arg);

/*
 * These functions may only be invoked inside kernel
 */
Error kSend( ThreadId dest,
	     Message *msg);
InterruptHandler tmSetInterruptHandler( InterruptId id, 
					InterruptHandler intHdler, void* arg);
ExceptionHandler tmSetExceptionHandler( ExceptionId id, 
					ExceptionHandler excHdler);
/*
 * Init and Main procedures
 */
void tmInit( Address mmStack,
	     Address tmStack,
	     Address userInit);

void tmMain( ThreadArg userInitAddress);

/* Initialisation of basic exception handlers for init phase */
void initBasicExceptions();

char* getCurrentThreadName();

#endif /* __THREADS_H */

