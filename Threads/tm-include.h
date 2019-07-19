#ifndef __TMINCLUDE_H
#define __TMINCLUDE_H
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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Threads/RCS/tm-include.h,v $
 	Author(s):             Christian Conrad
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.17 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1997/04/23 09:04:54 $      by: $Author: gfa $
	
	
*/

#include "Threads.h"
#include "HashList.h"
#include "List.h"
#include "Topsy.h"
#include "Messages.h"
#include "Configuration.h"

/* Internal TM error messages */
#define TM_OK                       1
#define TM_MSGQUEUEOVERFLOW        -1
#define TM_THREADNOTFOUND          -2
#define TM_THREADSTARTFAILED        0     /* as threadStart() returns an id (!= 0) */
#define TM_THREADKILLFAILED        -3
#define TM_NOSUCHMESSAGE           -4
#define TM_FAILED                  -5

/* Forward definitions - these are necessary to prevent cyclical references */
typedef struct Thread_t* ThreadPtr;
typedef struct ProcContext_t* ProcContextPtr;  /* proc. dependent context */

/* Thread status in scheduler */
typedef enum { RUNNING, READY, BLOCKED} SchedStatus;

/* Thread priority level (must match NBPRIORITYLEVELS)
   Decreasing order of priority */
typedef enum { KERNEL_PRIORITY, USER_PRIORITY, IDLE_PRIORITY } ThreadPriority;

/* Two queues for each priority level */
typedef struct SchedPriority_t {
    List ready;
    List blocked;
} SchedPriority;

typedef struct Scheduler_t {
    ThreadPtr running;
    SchedPriority prioList[NBPRIORITYLEVELS];
} Scheduler;

typedef enum { NOT_WAITING, WAITING, FILLED} PendingStatus;

/* Message queue for each thread */
typedef struct MessageQueue_t {
    int freeList;                      /* index of first free cell */
    int busyList;                      /* index of first valid message */
    int ctrl[MAXNBMSGINQUEUE];         /* management of message list */
    Message msg[MAXNBMSGINQUEUE];      /* static array of messages */
    PendingStatus msgPendingStatus;    /* indicates if a message is pending */
    Message* msgPendingPtr;            /* reference on pending message */
    ThreadId threadIdPending;          /* expected message sender */
    MessageId msgIdPending;            /* expected message type */
} MessageQueue;

/* Statistics about threads that may be gathered */
typedef struct ThreadStatistics_t {
    int cpuCycles;
} ThreadStatistics;

/* Scheduler specific data in each Thread structure */
typedef struct SchedulerInfo_t {
    SchedStatus status;		/* BLOCKED, READY, RUNNING */
    ThreadPriority priority;    /* Priority of the thread */
    Address hint;               /* Backwards reference on list descriptor */
} SchedulerInfo;

/* Main thread structure */
typedef struct Thread_t {
    ProcContextPtr contextPtr;	/* Processor dependent context */
    ThreadId id;		/* Processor independent */
    char name[MAXNAMESIZE];	/* Logical name of the thread */
    ThreadId parentId;		/* Thread id of parent thread */
    Address stackStart;		/* Start of stack */
    Address stackEnd;		/* End of stack */
    MessageQueue msgQueue;	/* Message queue */
    SchedulerInfo schedInfo;    /* Scheduler specific data */
    ThreadStatistics stat;	/* Various statistics */
} Thread;


#endif /* __TMINCLUDE_H */
