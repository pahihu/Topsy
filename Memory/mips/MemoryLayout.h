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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Memory/mips/RCS/MemoryLayout.h,v $
 	Author(s):             George Fankhauser
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.3 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1997/03/14 00:01:10 $      by: $Author: gfa $
	
	
*/

#include "cpu.h"

/* available memory */
#define PHYSMEM (1*1024*1024)
#define USERMEM (64*4*1024)

/* PAGESIZE is the machines real page size */
#define PAGEBITS             12
#define PAGESIZE            (1 << PAGEBITS)
#define PAGEFRAMEMASK       (0xffffffff << PAGEBITS)
#define PAGEMASK            (~PAGEFRAMEMASK)
#define PAGENUMBER(addr)    (((unsigned long)addr) >> PAGEBITS)
#define PAGEREMAINDER(addr) (((unsigned long)addr) & PAGEMASK)

/* logical page size is the granularity of vm regions 
 * this may be smaller on a direct mapped memory system. 
 * if paging is used LOGICAL and PHYSICAL have to be the same size 
 */
#define LOGICALPAGEBITS             8
#define LOGICALPAGESIZE            (1 << LOGICALPAGEBITS)
#define LOGICALPAGEFRAMEMASK       (0xffffffff << LOGICALPAGEBITS)
#define LOGICALPAGEMASK            (~LOGICALPAGEFRAMEMASK)
#define LOGICALPAGENUMBER(addr)    (((unsigned long)addr) >> LOGICALPAGEBITS)
#define LOGICALPAGEREMAINDER(addr) (((unsigned long)addr) & LOGICALPAGEMASK)

/* used as a boot and later as an exception stack */
#define BOOTSTACKSIZE	PAGESIZE
#define BOOTSTACKBOTTOM	(K0SEG_BASE)
#define BOOTSTACKTOP	(BOOTSTACKBOTTOM+BOOTSTACKSIZE-4)

