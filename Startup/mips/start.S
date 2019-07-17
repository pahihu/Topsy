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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Startup/mips/RCS/start.S,v $
 	Author(s):             George Fankhauser
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.8 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1997/04/23 09:02:17 $      by: $Author: gfa $
	
	
*/

#include "asm.h"
#include "cpu.h"
#include "MemoryLayout.h"

	.text
	.align  2

/* __start: the starting point, the address where the EPROM loader or
 * the simulator jumps to 
 */
    FRAME(__start)
    
	la		gp, 0x80000000 	    /*_gp, value of global pointer */
  	la		sp, BOOTSTACKTOP    /* prepare bootstack */
        move            fp, sp
	subu	        sp, sp, 32
	sw		ra, 28(sp)
	sw		fp, 24(sp)


	mtc0		zero, c0_status	 /* disable interrupts, kernel mode */
	jal		main
	nop
	    
    END(__start)
