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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Memory/mips/RCS/tlb.S,v $
 	Author(s):             Eckart Zitzler
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.6 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1997/07/14 07:31:44 $      by: $Author: gfa $
	
	
*/

#include "asm.h"
#include "cpu.h"


	.text
        .align 2

        FRAME(setTLBEntry)

	mtc0	a0, c0_tlblo
	mtc0	a1, c0_tlbhi
	sll	a2, a2, TLB_IND_INX_SHIFT
	mtc0	a2, c0_tlbind
	tlbwi

	END(setTLBEntry)

