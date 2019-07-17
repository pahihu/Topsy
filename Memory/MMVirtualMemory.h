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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Memory/RCS/MMVirtualMemory.h,v $
 	Author(s):             George Fankhauser
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.12 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1997/04/23 09:19:22 $      by: $Author: gfa $
	
	
*/

#ifndef _MMVIRTUALMEMORY_H_
#define _MMVIRTUALMEMORY_H_

#include "Topsy.h"
#include "Memory.h"
#include "Threads.h"
#include "List.h"

#define MM_VMINITOK	0
#define MM_VMINITFAILED 1

#define MM_VMEXACT	2
#define MM_VMINSIDE	3


typedef enum {VM_FREED, VM_ALLOCATED}  RegionStatus;

typedef struct RegionDesc_t {
    /* RegionStatus is implicitely known from which list the region is in */
    unsigned long int  startPage;
    unsigned long int  numOfPages;
    ProtectionMode     pmode;
    ThreadId           owner;
} RegionDesc;

typedef RegionDesc* Region;

typedef struct AddressSpaceDesc_t {
    List               regionList;
    List               freeList;
    unsigned long int  startPage; 
    unsigned long int  endPage;
} AddressSpaceDesc;

typedef AddressSpaceDesc* AddressSpacePtr;

Address mmVmGetHeapAddress(Address kernelDataStart, 
			   unsigned long int kernelDataSize);
Error  mmVmInit(Address kernelCodeStart, unsigned long int kernelCodeSize,
		Address kernelDataStart, unsigned long int kernelDataSize,
		Address userCodeStart, unsigned long int userCodeSize,
		Address userDataStart, unsigned long int userDataSize,
		Address* mmStack, Address* tmStack);
Error mmVmAlloc(Address* addressPtr, unsigned long int size, ThreadId owner);
Error mmVmFree(Address address, ThreadId claimsToBeOwner);
Error mmVmMove(Address* addressPtr, ThreadId newOwner);
Error mmVmProtect(Address startAdr, unsigned long int size,
		ProtectionMode pmode, ThreadId owner);
Error mmVmCleanup(ThreadId id);
		
#endif
