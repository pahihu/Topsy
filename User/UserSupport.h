#ifndef __USERSUPPORT_H
#define __USERSUPPORT_H
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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/User/RCS/UserSupport.h,v $
 	Author(s):             C. Conrad 
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.11 $
 	Creation Date:         21.3.97
 	Last Date of Change:   $Date: 1998/04/09 12:16:44 $      by: $Author: fiedler $
	
	
*/

#include "../Topsy/Topsy.h"
 
#define MAXCOMMENTSIZE     70
#define EQUAL 1
#define BEFORE 2
#define AFTER 3
#define UNDEFINED -1


/*
 * Copy nbBytes from address srcAddress to address targetAddress.
 * (equivalent to UNIX memcpy(), new name to avoid confusion).
 */
void byteCopy( Address targetAddress,
               Address sourceAddress,
               unsigned long int nbBytes);

/* fill memory with zeros */
void zeroOut(Address target, unsigned long int size);

/* initialize mem with a constant char */
void initmem(Address target, unsigned long int size, char c);

/*
 * Copy a NULL-terminated string from source to target.
 * (equivalent to UNIX strcpy(), new name to avoid confusion).
 */
void stringCopy( char* target, char* source );

/* copies a string or at most the first size-1 bytes 
 * and terminates it with 0 
 */
void stringNCopy( char* target, char* source, unsigned long int size);

int stringLength( char* s);

/* concatenate source 1 and source2 into the string dest */
/* dest needs to bee big enough to contain both */
void stringConcat(char *dest, char *source1, char *source2);

/* translate an integer to a string */
void int2string(char *str, int i);

void display( ThreadId tty, char* s);

int stringCompare(char* a, char* b);
int stringNCompare(const char *s1, const char *s2, int n);

int power(int base, int n);

int atoi(int* intValue, char* string);

void reverse(char s[]);

void itoa(int n, char s[]);

#endif __USERSUPPORT_H
