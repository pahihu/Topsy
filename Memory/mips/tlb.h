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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Memory/mips/RCS/tlb.h,v $
 	Author(s):             Eckart Zitzler
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.2 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1997/03/09 20:50:04 $      by: $Author: gfa $
	
	
*/

#ifndef _TLB_H_
#define _TLB_H_

#include "Topsy.h"

void setTLBEntry(Register TLBEntryLow, Register TLBEntryHigh, Register Index); 

#endif







