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


        
        File:                  $Source: /proj/topsy/ss98/Topsy/Topsy/mips/RCS/SupportAsm.S,v $
        Author(s):             George Fankhauser
        Affiliation:           ETH Zuerich, TIK
        Version:               $Revision: 1.5 $
        Creation Date:         
        Last Date of Change:   $Date: 1998/04/08 15:09:29 $      by: $Author: gfa $
        
        
*/

#include "asm.h"
#include "cpu.h"

        .text
        .align 2

        FRAME(testAndSet)

	.set noreorder
		
	mfc0	t0, c0_status
	mtc0	zero, c0_status
	nop			/* interrupts disabled */
	
	lw	t1, 0(a0)	/* load lockVariable contents */
	move	v0, zero	/* prepare return FALSE in load delay */
	bnez	t1, lockIsAlreadySet
	nop
	addiu	v0, 1		/* return TRUE (successful locking) */
	sw	v0, 0(a0)	/* write back locked value */
lockIsAlreadySet:
	mtc0	t0, c0_status	/* re-enable interrupts */	
	
	.set reorder
	
        END(testAndSet)


	FRAME(getEPC)

	.set noreorder

	mfc0	v0, c0_epc 
	nop

	.set reorder

	END(getEPC)

        FRAME(getBADVM)

        .set noreorder

        mfc0    v0, c0_badvm
        nop

        .set reorder

	END(getBADVM)
