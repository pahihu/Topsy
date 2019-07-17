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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Memory/RCS/MMVirtualMemory.c,v $
 	Author(s):             George Fankhauser
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.21 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1997/04/23 09:19:22 $      by: $Author: gfa $
	
	
*/

#include "MMVirtualMemory.h"

#include "Topsy.h"
#include "Configuration.h"

#include "Memory.h"
#include "Threads.h"
#include "MMMapping.h"
#include "MemoryLayout.h"
#include "List.h"
#include "Configuration.h"

#define STACKSIZE TM_DEFAULTTHREADSTACKSIZE


static void vmInitRegion(List list, Address startAddr, unsigned long int size,
		RegionStatus status, ProtectionMode pmode, ThreadId owner);
static Boolean vmFreeRegion(List list, List freeList, 
		Region region, Address address, ThreadId owner);
static Boolean vmAllocRegion(Region region, Page pages, List list, 
			     List freeList, ThreadId sender, Address* addr);
static Boolean isPageInRegion(Region region, Page page, int mode);
static Region getRegion(Address address, int mode, ThreadId hint);

static unsigned long int roundToPageUp(unsigned long int x);
static unsigned long int roundUp(unsigned long int x);

static AddressSpaceDesc addressSpaces[ADDRESSSPACES];


/* the heap follows always kernel code and data. this function returns
 * the start address in order to allow early initialization of the heap
 */
Address mmVmGetHeapAddress( Address kernelDataStart, 
			    unsigned long int kernelDataSize)
{
    return (Address)roundUp((unsigned long)kernelDataStart + kernelDataSize);
}


/* this is topsy's initial vm region setup. note the two stacks that are
 * made in advance for tmInit to startup properly.
 */
Error  mmVmInit(Address kernelCodeStart, unsigned long int kernelCodeSize,
		Address kernelDataStart, unsigned long int kernelDataSize,
		Address userCodeStart, unsigned long int userCodeSize,
		Address userDataStart, unsigned long int userDataSize,
		Address* mmStackPtr, Address* tmStackPtr)
{
    AddressSpacePtr space;
    Address addr;
    unsigned long int size, loc;

    /*** setup kernel memory regions (a total of 7 initial regions) */
    mmAddressSpaceRange(KERNEL, &addr, &size);
    space = &(addressSpaces[KERNEL]);
    space->regionList = listNew();
    space->freeList = listNew();
    space->startPage = LOGICALPAGENUMBER((unsigned long int)addr);
    space->endPage = roundToPageUp((unsigned long int)addr + size);
          
    /** boot/exception stack */
    vmInitRegion (space->regionList, (Address)BOOTSTACKBOTTOM, BOOTSTACKSIZE, 
    		VM_ALLOCATED, READ_WRITE_REGION, 0);
		
    /** kernel code */
    vmInitRegion (space->regionList, kernelCodeStart, kernelCodeSize, 
		VM_ALLOCATED, READ_ONLY_REGION, 0);
		
    /** kernel data */
    vmInitRegion (space->regionList, kernelDataStart, kernelDataSize, 
		VM_ALLOCATED, READ_WRITE_REGION, 0);
		
    /** heap region */
    loc = (unsigned long)mmVmGetHeapAddress(kernelDataStart, kernelDataSize);
    vmInitRegion (space->regionList, (Address)loc, KERNELHEAPSIZE, 
    		VM_ALLOCATED, READ_WRITE_REGION, 0);
    
    /** mmStack */
    loc += KERNELHEAPSIZE;
    vmInitRegion (space->regionList, (Address)loc, STACKSIZE, 
    		VM_ALLOCATED, READ_WRITE_REGION, 0);
    *mmStackPtr = (Address)loc;

    /** tmStack */
    loc += STACKSIZE;
    vmInitRegion (space->regionList, (Address)loc, STACKSIZE, 
    		VM_ALLOCATED, READ_WRITE_REGION, 0);
    *tmStackPtr = (Address)loc;

    /** free region for kernel memory */
    loc += STACKSIZE;
    vmInitRegion (space->freeList, 
    		(Address)loc, size - (loc - (unsigned long int)addr), 
		VM_FREED, READ_WRITE_REGION, 0);
        
    /*** insert 2 regions for user code and data in users address space */
    mmAddressSpaceRange(USER, &addr, &size);
    space = &(addressSpaces[USER]);
    space->regionList = listNew();
    space->freeList = listNew();
    space->startPage = LOGICALPAGENUMBER((unsigned long int)addr);
    space->endPage = roundToPageUp((unsigned long int)addr + size);
    
    /** user code */
    vmInitRegion (space->regionList, userCodeStart, userCodeSize, 
    		VM_ALLOCATED, READ_ONLY_REGION, 0);
    /** user data */
    vmInitRegion (space->regionList, userDataStart, userDataSize, 
    		VM_ALLOCATED, READ_WRITE_REGION, 0);
    /** free region for user memory */
    vmInitRegion (space->freeList,
    		 (Address)roundUp((unsigned long int)userDataStart + 
								userDataSize), 
		 size - roundUp(userDataSize) - roundUp(userCodeSize),
		 VM_FREED, READ_WRITE_REGION, 0);
    return MM_VMINITOK;
} 


Error mmVmAlloc(Address* addressPtr, unsigned long int size, ThreadId owner)
{
    Region region;
    Boolean success;
    List list = addressSpaces[THREAD_MODE(owner)].regionList;
    List freeList = addressSpaces[THREAD_MODE(owner)].freeList;
    Page pages = roundToPageUp(size);
        
    listGetFirst(freeList, (void**)&region);
    success = vmAllocRegion(region, pages, list, freeList, owner, addressPtr); 
    if (success) return VM_ALLOCOK;
    
    while (region != NULL) {
	listGetNext(freeList, (void**)&region);
	success = vmAllocRegion(region, pages, list, freeList, 
				owner, addressPtr); 
	if (success) {
	    return VM_ALLOCOK;
	}
    }
    return VM_ALLOCFAILED;
}


Error mmVmFree(Address address, ThreadId sender)
{
    List list, freeList;
    Region region;
    Boolean success;
    
    /* test aligned existence of region for given address */
    region = getRegion(address, MM_VMEXACT, MMTHREADID);
    if (region == NULL) {
	WARNING("vmFree failed (no such region)");    
	return VM_FREEFAILED;
    }
    /* kernel threads are allowed to free kernel and user vm regions */
    /* users may only remove their own regions */
    if (THREAD_MODE(region->owner) == KERNEL) {
	if (THREAD_MODE(sender) == USER) {
	    WARNING("vmFree: user tried to free kernel region");
	    return VM_FREEFAILED;
	} 
	list = addressSpaces[KERNEL].regionList;
	freeList = addressSpaces[KERNEL].freeList;
    }
    else {
	list = addressSpaces[USER].regionList;
	freeList = addressSpaces[USER].freeList;
    }
    success = vmFreeRegion(list, freeList, region, address, sender);
    if (success) return VM_FREEOK;

    WARNING("vmFree failed (freeRegion failed)");
    return VM_FREEFAILED;
}


Error mmVmMove(Address* addressPtr, ThreadId newOwner)
{
    Address oldAddress = *addressPtr;
    Region region;
    Page from, to, i;
    Error ret;
    
    if (THREAD_MODE(newOwner) == KERNEL) {
	/* kernel does not want to move regions from kernel to kernel */
	return VM_MOVEFAILED;
    }
    /* moving of pages for virtual memory or copying for direct mapping */
    region = getRegion(oldAddress, MM_VMEXACT, MMTHREADID);
    if (region == NULL) {
	return VM_MOVEFAILED;
    }    
    ret = mmVmAlloc(addressPtr, region->numOfPages * LOGICALPAGESIZE,newOwner);
    if (ret != VM_ALLOCOK) {
	return VM_MOVEFAILED;
    }
    from = LOGICALPAGENUMBER(oldAddress);
    to = LOGICALPAGENUMBER(*addressPtr);
    for (i = 0; i < region->numOfPages; i++) {
	if (mmMovePage(from, to) != MM_MOVEPAGEOK) {
	    mmVmFree(addressPtr, MMTHREADID);
	    WARNING("vmMove: page moving failed");
	    return VM_MOVEFAILED;
	}
	from++; to++;
    }
    mmVmFree(oldAddress, MMTHREADID);
    return VM_MOVEOK;
}


Error mmVmProtect(Address startAddr, unsigned long int size,
		   ProtectionMode pmode, ThreadId owner)
{
    Page page, end;
    Region region;
    
    if (LOGICALPAGEREMAINDER(size) != 0) return VM_PROTECTFAILED;

    /* pages must be in a region to protect */
    region = getRegion(startAddr, MM_VMINSIDE, owner);
    if (region == NULL) return VM_PROTECTFAILED;

    /* users must own the regions */
    if (THREAD_MODE(owner) == USER) {
	if (region->owner != owner) return VM_PROTECTFAILED;
    }
    end = LOGICALPAGENUMBER(size) + LOGICALPAGENUMBER(startAddr);
    for (page = LOGICALPAGENUMBER(startAddr); page < end; page++) {
	if (mmProtectPage(page, pmode) == MM_PROTECTPAGEFAILED)
	    return VM_PROTECTFAILED;
    }
    return VM_PROTECTOK;
}


/* this kernel internal message is used to reclaim all vm resource
 * owned by a thread. just in case it forgets to call vmFree or it
 * crashes before doing it.
 * we assume that kernel threads do not allocate and own vmRegions
 * in user space.
 */
Error mmVmCleanup(ThreadId id)
{
    List list;
    Region region;

    list = addressSpaces[THREAD_MODE(id)].regionList;
    listGetFirst(list, (void**)&region);
    while (region != NULL) {
	if (region->owner == id) 
	    mmVmFree((Address)(region->startPage * LOGICALPAGESIZE), id);
	listGetNext(list, (void**)&region);
    }	
    return VM_CLEANUPOK;
}


static Boolean isPageInRegion(Region region, Page page, int mode)
{
    if (mode == MM_VMEXACT) {
	if (region->startPage == page) return TRUE;
	else return FALSE;
    }
    else if (mode == MM_VMINSIDE) {
	if (page >= region->startPage && 
	    page < region->startPage + region->numOfPages) return TRUE;
	else return FALSE;
    }
    return FALSE;
}


static Region getRegion(Address address, int mode, ThreadId hint)
{    
    List list;
    AddressSpace first, then;
    Region region;
    Page page = LOGICALPAGENUMBER(address);

    if (mode == MM_VMEXACT) {
    	if (LOGICALPAGEREMAINDER(address) != 0) {
	    WARNING("getRegion: got unaligned address");
	    return NULL;
	}
    }	
    if (THREAD_MODE(hint) == USER) { first = USER; then = KERNEL; }
    else { first = KERNEL; then = USER; }

    list = addressSpaces[first].regionList;
    listGetFirst(list, (void**)&region);
    while (region != NULL) {
	if (isPageInRegion(region, page, mode)) return region;
	listGetNext(list, (void**)&region);
    }	
    list = addressSpaces[then].regionList;
    listGetFirst(list, (void**)&region);
    while (region != NULL) {
	if (isPageInRegion(region, page, mode)) return region;
	listGetNext(list, (void**)&region);
    }
    return NULL;
}


static Boolean vmFreeRegion(List list, List freeList, 
			    Region region, Address address, ThreadId sender)
{
    unsigned long int page = LOGICALPAGENUMBER(address);
    
    if (region == NULL) return FALSE;
    
    if (region->startPage == page) {
	if ((region->owner == sender) || (THREAD_MODE(sender) == KERNEL)) {
	    listRemove(list, (void*)region, NULL);
	    listAddInFront(freeList, (void*)region, NULL);
	    mmUnmapPages(region->startPage, region->numOfPages);
	    return TRUE;
	}
    }
    return FALSE;
}


/* allocates a region without mapping pages. pages are mapped either
 * on demand (when a paged memory model is used) or they don't need
 * to be mapped at all for a direct mapped memory model.
 */
static Boolean vmAllocRegion(Region region, Page pages, List list, 
			     List freeList, ThreadId sender, Address* addr)
{
    Region newRegion;
    
    if (region == NULL) return FALSE;

    if (region->numOfPages > pages) {
	/* make new region, split free into an allocated and a free one */
	if (hmAlloc((Address*)&newRegion, sizeof(RegionDesc)) != HM_ALLOCOK)
	    return FALSE;
	newRegion->owner = sender;
	newRegion->startPage = region->startPage;
	newRegion->numOfPages = pages;

	*addr = (Address)((newRegion->startPage) * LOGICALPAGESIZE);
	listAddInFront(list, newRegion, NULL);   /* add new region to list */
	
	/* adjust free region, subtract used space */
	region->owner = ANY;
	region->startPage += pages;
	region->numOfPages -= pages;
	return TRUE;
    }
    else if (region->numOfPages == pages) {
	region->owner = sender;
	*addr = (Address)((region->startPage) * LOGICALPAGESIZE);
	listRemove(freeList, region, NULL);  /* changing places */
	listAddInFront(list, region, NULL);
	return TRUE;
    }
    return FALSE;
}


static void vmInitRegion(List list, Address startAddr, unsigned long int size,
		RegionStatus status, ProtectionMode pmode, ThreadId owner)
{
    Region region;
    
    hmAlloc((Address*)&region, sizeof(RegionDesc));
    region->startPage = LOGICALPAGENUMBER((unsigned long int)startAddr);
    region->numOfPages = roundToPageUp(size);
    region->pmode = pmode;
    region->owner = owner;
    listAddInFront(list, region, NULL);
}


/* round address up to logical page size and return page number 
 */
static unsigned long int roundToPageUp(unsigned long int x)
{
    if (LOGICALPAGEREMAINDER(x) > 0)
    	return LOGICALPAGENUMBER(x) + 1;
    else
    	return LOGICALPAGENUMBER(x);
}

/* round address up to logical page size and return rounded address 
 */
static unsigned long int roundUp(unsigned long int x)
{
    return roundToPageUp(x) * LOGICALPAGESIZE;
}
