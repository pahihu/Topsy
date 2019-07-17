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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Memory/RCS/MMInit.c,v $
 	Author(s):             George Fankhauser
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.14 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1997/04/23 09:18:20 $      by: $Author: gfa $
	
	
*/

#include "MMInit.h"
#include "Threads.h"

#include "MMHeapMemory.h"
#include "MMVirtualMemory.h"
#include "MMMapping.h"
#include "MMError.h"


void mmInit(SegMapPtr segMapPtr, Address* mmStackPtr, Address* tmStackPtr)
{
    Address heapAddress;

    /* memory error handlers need to be installed */ 
    mmInstallErrorHandlers();

    /* set up page mapping */
    mmInitMemoryMapping(segMapPtr->userCodeStart, segMapPtr->userCodeSize,
	    		segMapPtr->userDataStart, segMapPtr->userDataSize,
			segMapPtr->userLoadedInKernelAt);
    
    /* build dynamic kernel heap memory in one vm region (which will be
     * created later using heap memory for lists and descriptors)
     * the heap always starts after the static kernel data...
     */
    heapAddress = mmVmGetHeapAddress(segMapPtr->kernelDataStart, 
				     segMapPtr->kernelDataSize);
    if (hmInit(heapAddress) != HM_INITOK) 
    	PANIC("heap init failed");

    /* setup memory region lists. beware: vmregionlist/freelist and first
       region descriptors must use static memory
       vmInit makes regions for the following:
       KERNEL:	boot/exception stack (first page in kernel memory)
       		kernel code
		kernel data
		kernel heap (dynamic data)
		stacks for mmStackPtr, tmStackPtr
		free region (what's left in kernel space)
	USER:	user code
		user data
		free region (what's left in user space)
     */
    if (mmVmInit(segMapPtr->kernelCodeStart, segMapPtr->kernelCodeSize,
		 segMapPtr->kernelDataStart, segMapPtr->kernelDataSize,
		 segMapPtr->userCodeStart, segMapPtr->userCodeSize,
		 segMapPtr->userDataStart, segMapPtr->userDataSize,
		 mmStackPtr, tmStackPtr) != MM_VMINITOK) {
	PANIC("vm init failed");
    }
}
