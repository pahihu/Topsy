#ifndef __ERROR_H
#define __ERROR_H
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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Topsy/RCS/Error.h,v $
 	Author(s):             Christian Conrad
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.4 $
 	Creation Date:         6.3.97
 	Last Date of Change:   $Date: 1998/04/06 10:04:33 $      by: $Author: gfa $
	
	
*/

/*
 * There are 3 debug levels:
 *       INFO_LEVEL     : for information
 *       WARNING_LEVEL  : anomality
 *       ERROR_LEVEL    : error (no panic nessary)
 */
#define INFO_LEVEL      1
#define WARNING_LEVEL   2
#define ERROR_LEVEL     3

/* Not possible to mask panic messages */
#define PANIC(s)       panic( getCurrentThreadName(), __FILE__, __LINE__, s)

#if DEBUG_LEVEL <= INFO_LEVEL
#define INFO(s)        info(getCurrentThreadName(), __FILE__, __LINE__, s)
#else
#define INFO(s)
#endif

#if DEBUG_LEVEL <= WARNING_LEVEL
#define WARNING(s)     warning(getCurrentThreadName(), __FILE__, __LINE__, s)
#else
#define WARNING(s)
#endif

#if DEBUG_LEVEL <= ERROR_LEVEL
#define ERROR(s)       error(getCurrentThreadName(), __FILE__, __LINE__, s)
#else
#define ERROR(s)
#endif


void panic( char* threadName,
	    const char* fileName,
	    int line,
	    char* errorMessage);

void error( char* threadName,
	    const char* fileName,
	    int line,
	    char* errorMessage);

void warning( char* threadName,
	      const char* fileName,
	      int line,
	      char* errorMessage);

void info( char* threadName,
	   const char* fileName,
	   int line,
	   char* errorMessage);

void printRegisters();

#endif /* __ERROR_H */

