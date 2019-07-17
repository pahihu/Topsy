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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Threads/mips/RCS/TMException.c,v $
 	Author(s):             
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.1 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1997/02/13 07:30:17 $      by: $Author: conrad $
	
	
*/

#include "TMException.h"
#include "TMHal.h"
#include "TMIPC.h"


/* Table contains all exception handlers for any exception AND UTLB */
ExceptionHandler exceptionCauseTable[32+1];


void exception()
{

}

void dispatchException()
{

}

void dispatchInterrupt()
{

}

