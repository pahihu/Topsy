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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Topsy/RCS/Topsy.h,v $
 	Author(s):             Eckart Zitzler
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.11 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1997/04/07 10:19:28 $      by: $Author: gfa $
	
	
*/

#ifndef _TOPSY_H_
#define _TOPSY_H_

#define MMTHREADID  -1     /* Memory Manager Thread Id. */
#define TMTHREADID  -2     /* Thread Manager Thread Id. */
#define IOTHREADID  -3     /* Input/Output Manager Thread Id. */

#define NULL (void*) 0

typedef enum {FALSE = 0, TRUE}  Boolean;

typedef void*  Address;

typedef long int Error;

typedef unsigned long int Register;

typedef long int ThreadId;       /* Kernel threads: < 0, User threads: > 1 */

typedef void* ThreadArg;
typedef void(*ThreadMainFunction)(ThreadArg);

#endif

