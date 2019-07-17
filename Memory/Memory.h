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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Memory/RCS/Memory.h,v $
 	Author(s):             Eckart Zitzler
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.12 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1997/04/07 10:22:04 $      by: $Author: gfa $
	
	
*/

#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "Topsy.h"


typedef struct SegMapDescriptor_t {      /* needed in bootstrapping process */
    unsigned long int  kernelCodeSize;
    Address            kernelCodeStart;
    unsigned long int  kernelDataSize;
    Address            kernelDataStart;
    unsigned long int  userCodeSize;
    Address            userCodeStart;
    unsigned long int  userDataSize;
    Address            userDataStart;
    Address            userJumpAddress;
    Address            userLoadedInKernelAt;
} SegMapDescriptor;

typedef SegMapDescriptor*  SegMapPtr;

#define ADDRESSSPACES 2
typedef enum {KERNEL, USER}  AddressSpace;

typedef enum {READ_WRITE_REGION, READ_ONLY_REGION, PROTECTED_REGION}
    ProtectionMode;

void mmInit(SegMapPtr segMapPtr, Address* mmStackPtr, Address* tmStackPtr);
void mmMain(ThreadArg arg);

#endif
