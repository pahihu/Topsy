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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Memory/mips/RCS/MMMapping.h,v $
 	Author(s):             George Fankhauser
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.3 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1997/04/13 16:22:33 $      by: $Author: gfa $
	
	
*/

#ifndef _MMMAPPING_H_
#define _MMMAPPING_H_

#include "Topsy.h"
#include "Memory.h"
#include "MemoryLayout.h"

/* error codes */
#define MM_INITMEMORYMAPPINGFAILED  1
#define MM_INITMEMORYMAPPINGOK      2
#define MM_MAPPAGESFAILED           3
#define MM_MAPPAGESOK               4
#define MM_UNMAPPAGESFAILED         5
#define MM_UNMAPPAGESOK             6
#define MM_MOVEPAGEFAILED           7
#define MM_MOVEPAGEOK               8
#define MM_ADDRESSSPACERANGEFAILED  9
#define MM_ADDRESSSPACERANGEOK      10
#define MM_PROTECTPAGEFAILED        11
#define MM_PROTECTPAGEOK            12


typedef enum {READ_WRITE_PAGE, READ_ONLY_PAGE, PROTECTED_PAGE, INVALID_PAGE}
    PageStatus;
typedef unsigned long int Page;

Error mmInitMemoryMapping(Address  codeAddr,
                       unsigned long int  codeSize, Address  dataAddr,
                       unsigned long int  dataSize, 
		       Address userLoadedInKernelAt);
Error mmMapPages(Page startPage, Page endPage, PageStatus pstat);
Error mmUnmapPages(Page startPage, Page nOfPages);
Error mmMovePage(Page from, Page to);
Error mmProtectPage(Page page, ProtectionMode pmode);
Error mmAddressSpaceRange(AddressSpace space, Address*  addressPtr,
                     unsigned long int*  sizePtr);

#endif
