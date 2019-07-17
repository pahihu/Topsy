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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/IO/Drivers/RCS/Loopback.h,v $
 	Author(s):             George Fankhauser
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.1 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1997/05/30 18:03:30 $      by: $Author: gfa $
	
	
*/

#include "Topsy.h"
#include "MMHeapMemory.h"
#include "IODevice.h"
#include "Configuration.h"

#define LOOPBACK_BUFSIZE 512

typedef struct devLoopbackDesc_t {
    unsigned long in, out, count;
} devLoopbackDesc;

typedef devLoopbackDesc* devLoopback;


Error devLoopback_init(IODevice this);
Error devLoopback_read(IODevice this, ThreadId threadId, char* buffer, 
							long int* size);
Error devLoopback_write(IODevice this, ThreadId threadId, char* buffer, 
							long int* size);
Error devLoopback_close(IODevice this);
