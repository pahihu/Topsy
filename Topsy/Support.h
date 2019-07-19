#ifndef __SUPPORT_H
#define __SUPPORT_H
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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Topsy/RCS/Support.h,v $
 	Author(s):             Christian Conrad
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.5 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1998/04/11 20:21:07 $      by: $Author: gfa $
	
	
*/

#include "Topsy.h"


/*
 * Copy nbBytes from address srcAddress to address targetAddress.
 * (equivalent to UNIX memcpy(), new name to avoid confusion).
 */
void byteCopy( Address targetAddress, Address sourceAddress, unsigned long int nbBytes);

/*
 * Copy nbBytes from address srcAddress to address targetAddress.
 * nbBytes modulo 4 must be zero.
 * otherwise too few bytes are copied or an alignment trap happens.
 */
void longCopy( Address targetAddress, Address sourceAddress, unsigned long int nbBytes);

/* fill memory with zeros */
void zeroOut(Address target, unsigned long int size);

/*
 * Copy a NULL-terminated string from source to target.
 * (equivalent to UNIX strcpy(), new name to avoid confusion).
 */
void stringCopy( char* target, char* source );

/* copies a string or at most the first size-1 bytes 
 * and terminates it with 0 
 */
void stringNCopy( char* target, char* source, unsigned long int size);

/* support function (in assembler) to test and set memory
 * in one atomic step
 */
Boolean testAndSet(volatile Boolean* lockvar);

#define FN_PTRDIFF(endFn,startFn) ((char*)(endFn) - (char*)(startFn))

#endif /* __SUPPORT_H */
