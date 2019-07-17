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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Memory/RCS/MMHeapMemory.h,v $
 	Author(s):             George Fankhauser
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.8 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1997/03/26 09:35:17 $      by: $Author: gfa $
	
	
*/

#ifndef _MMKERNELMEMORY_H_
#define _MMKERNELMEMORY_H_

#include "Topsy.h"
#include "Memory.h"

/* error codes */
#define HM_INITFAILED   1
#define HM_INITOK       2

#define HM_ALLOCOK	3
#define HM_ALLOCFAILED	4
#define HM_FREEOK	5
#define HM_FREEFAILED	6


Error hmInit(Address addr);
Error hmAlloc(Address* addressPtr, unsigned long int size);
Error hmFree(Address address);

#endif







