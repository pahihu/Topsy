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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Memory/mips/RCS/MMDirectMapping.c,v $
 	Author(s):             George Fankhauser
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.6 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1998/04/11 20:22:05 $      by: $Author: gfa $
	
	
*/

#include "MMMapping.h"
#include "Threads.h"
#include "Support.h"
#include "tlb.h"
#include "cpu.h"


Error   mmInitMemoryMapping(Address codeAddr, unsigned long int codeSize,
			    Address dataAddr, unsigned long int dataSize,
			    Address userLoadedInKernelAt)
{
    /* to simulate a directmapped memory on a paged machine we fill
     * the TLB to provide the user with 64*4k memory from 4k to (256+4)k - 1.
     * Pages from [PHYSMEM-256k, PHYSMEM] are mapped to [4, 260k].
     * Page Table Entries are set to valid and global.
     * (setting up a *real* virtual memory system is lot harder) 
     */
    Register lo = ((PHYSMEM-USERMEM+PHYS_BASE) & TLB_LO_PFN_MASK) | 
					    TLB_VALID | TLB_GLOBAL | TLB_DIRTY;
    /* start at 0x00001000 to leave page zero invalid (nil pointer catcher) */
    Register hi = KUSEG_BASE+PAGESIZE;
    unsigned int i;
    
    for (i = 0; i < TLB_SIZE; i++) {
	setTLBEntry(lo, hi, i); 
	hi += PAGESIZE; lo += PAGESIZE; /* yes, physical pages */
    }
    /* once the mapping is done, copy user code and data */ 
    longCopy(codeAddr, userLoadedInKernelAt, codeSize);
    longCopy(dataAddr, (Address)((unsigned long)userLoadedInKernelAt + 
		(unsigned long)dataAddr - (unsigned long)codeAddr), dataSize);
    
    return MM_INITMEMORYMAPPINGOK;
}

Error mmMapPages(Page startPage, Page nOfPages, PageStatus pstat)
{
    return MM_MAPPAGESOK;
}


Error mmUnmapPages(Page startPage, Page nOfPages)
{
    return MM_UNMAPPAGESOK;
}


Error mmMovePage(Page from, Page to)
{
    /* direct mapped memory systems can only copy the page */
    longCopy((Address)(to*LOGICALPAGESIZE), (Address)(from*LOGICALPAGESIZE), 
							    LOGICALPAGESIZE);
    zeroOut((Address)(from*LOGICALPAGESIZE), LOGICALPAGESIZE);
    return MM_MOVEPAGEOK;
} 


Error mmProtectPage(Page page, ProtectionMode pmode)
{
    /* direct mapped memory does nothing */
    return MM_PROTECTPAGEFAILED;
}


Error  mmAddressSpaceRange(AddressSpace space, Address* addressPtr,
					unsigned long int* sizePtr)
{
    if (space == KERNEL) {
    	*addressPtr = (Address)K0SEG_BASE;
	*sizePtr = (PHYSMEM-USERMEM); /* 768kB */
    }
    else if (space == USER) {
    	*addressPtr = (Address)KUSEG_BASE + PAGESIZE; /* zero page invalid */
	*sizePtr = USERMEM;           /* 256kB */
    }
    else return MM_ADDRESSSPACERANGEFAILED;
    return MM_ADDRESSSPACERANGEOK;
}
