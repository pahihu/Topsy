#ifndef __TMTHREAD_H
#define __TMTHREAD_H
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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Threads/RCS/TMThread.h,v $
 	Author(s):             Christian Conrad
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.15 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1998/03/26 20:35:52 $      by: $Author: gfa $
	
	
*/

#include "tm-include.h"
#include "Lock.h"

extern Lock threadLock;

/*
 * Thread interface functions
 */
ThreadId threadStart( ThreadMainFunction fctnAddr, 
		      ThreadArg parameter,
		      AddressSpace mode,
		      char *name,
		      ThreadId parentId,
		      Boolean lightWeight);
void threadExit( ThreadId threadId);
void threadYield();
Error threadKill( ThreadId id, ThreadId killerId);
Error threadInfo( ThreadId id, ThreadId aboutId,
		  ThreadInfoKind kind, ThreadInfo* infoptr, long int values[]);

void threadBuild( ThreadId id,
		  ThreadId parentId,
		  char* name,
		  ProcContextPtr contextPtr,
		  Address stackBaseAddress,
		  unsigned int stackSize,
		  ThreadMainFunction mainFunction,
		  ThreadArg parameter,
		  AddressSpace space,
		  Boolean lightWeightThread,
		  Thread* threadPtr);

ThreadId startMinimalIdleThread();

#endif __TMTHREAD_H
