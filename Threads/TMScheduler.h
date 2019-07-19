#ifndef __TMSCHEDULER_H
#define __TMSCHEDULER_H
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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Threads/RCS/TMScheduler.h,v $
 	Author(s):             Christian Conrad & George Fankhauser
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.9 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1997/03/31 20:30:42 $      by: $Author: gfa $
	
	
*/

#include "tm-include.h"

/*
 * Scheduler interface functions (used by the thread manager)
 */
void schedulerInit(Thread* threadPtr);
void schedulerInsert( Thread* threadPtr, ThreadPriority priority);
void schedulerRemove( Thread* threadPtr);

Error schedulerSetReady( Thread* threadPtr);
Error schedulerSetBlocked( Thread* threadPtr);
Thread* schedulerRunning(void);
void schedule(void);

/* Clock interrupt handler and idle thread main function */
void tmClockHandler(void *arg);
void tmIdleMain(void);

#endif /* __TMSCHEDULER_H */
