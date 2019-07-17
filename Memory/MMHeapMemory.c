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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Memory/RCS/MMHeapMemory.c,v $
 	Author(s):             George Fankhauser
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.19 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1997/03/31 20:35:34 $      by: $Author: gfa $
	
	
*/

#include "Topsy.h"
#include "Configuration.h"
#include "Threads.h"
#include "Syscall.h"
#include "MMHeapMemory.h"
#include "MMVirtualMemory.h"
#include "MMMapping.h"
#include "Lock.h"


typedef enum {HM_FREED, HM_ALLOCATED} AreaStatus;

typedef struct HmEntryDesc_t {
  AreaStatus          		status;
  struct HmEntryDesc_t*   	next;
} HmEntryDesc;

typedef HmEntryDesc* HmEntry;

static HmEntry start = NULL;
static HmEntry end = NULL;

/* internal function prototypes */
static HmEntry findFree(unsigned long size);
static void split(HmEntry entry, unsigned long int);
static void compact(HmEntry at);

/* a lock is needed in order to keep the heap pointers consistent. only
 * one kernel thread may be in hmAlloc and hmFree at a time
 */
static LockDesc hmLockDesc;
static Lock hmLock;

#define ALIGN 4
#define LEFTOVER (ALIGN*2)
#define ENTRYSIZE(ptr) ((unsigned long)(ptr->next) - \
			(unsigned long)ptr - sizeof(HmEntryDesc))


Error hmInit(Address addr)
{
    start = (HmEntry)addr;
    start->next = (HmEntry)((unsigned long)addr + KERNELHEAPSIZE + 
							    sizeof(HmEntryDesc));
    start->status = HM_FREED;
    
    end = start->next;
    end->next = NULL;
    end->status = HM_ALLOCATED;
    
    hmLock = &hmLockDesc;
    lockInit( hmLock);
    
    return HM_INITOK;
}


Error hmAlloc(Address* addressPtr, unsigned long int size)
{
    HmEntry entry;

    size = size + (ALIGN - size % ALIGN);   /* round up to multiple of words */
    entry = start;
    *addressPtr = NULL;

    /*** here we enter a critical section */
    lock(hmLock);
    
    /* try to find a free piece of memory */
    entry = findFree(size);
    if (entry == NULL) {
    	/* compact and try again */
	compact(start);
	entry = findFree(size);	
    } 
    if (entry == NULL) {
	unlock( hmLock);
	return HM_ALLOCFAILED;
    }
    split(entry, size);
    *addressPtr = (Address)((unsigned long)entry + sizeof(HmEntryDesc));

    unlock(hmLock);    
    return HM_ALLOCOK;
}


Error hmFree(Address address)
{
    HmEntry  entry, addressEntry;

    /* check first if pointer is inside the heap */
    if (((unsigned long)start > (unsigned long)address) || 
	 ((unsigned long)address >= ((unsigned long)end - LEFTOVER))) {
	WARNING("hmFree got non-heap pointer");
	return HM_FREEFAILED;
    }

    /*** here we enter a critical section */
    lock(hmLock);

    entry = start;
    addressEntry = (HmEntry)((unsigned long)address - sizeof(HmEntryDesc));
    while (entry != NULL) {
	if (entry == addressEntry) break;
	entry = entry->next;
    }
    if (entry == NULL) {
	unlock( hmLock);
	return HM_FREEFAILED;
    }
    entry->status = HM_FREED;

    unlock( hmLock);
    return HM_FREEOK;
}


static HmEntry findFree(unsigned long size)
{
    HmEntry entry = start;
    
    while (entry != NULL) {
	if (entry->status == HM_FREED && ENTRYSIZE(entry) >= size) break;
	entry = entry->next;
    }
    return entry;
}


static void split(HmEntry entry, unsigned long int size)
{
    HmEntry new;
    
    if (ENTRYSIZE(entry) >= (size + sizeof(HmEntryDesc) + LEFTOVER)) {
	new = (HmEntry)((unsigned long)(entry) + sizeof(HmEntryDesc) + size);
	new->next = entry->next;
	new->status = HM_FREED;
	entry->next = new;
    }
    entry->status = HM_ALLOCATED;
}


static void compact(HmEntry at)
{
    HmEntry atNext;

    while (at != NULL) {
	atNext = at->next;
	while ((at->status == HM_FREED) && (atNext != NULL)) {
	    if (atNext->status != HM_FREED) break;
	    at->next = atNext->next;     /* collapse two free entries into one */
	    atNext = atNext->next;
	}
	at = at->next;
    }
}
