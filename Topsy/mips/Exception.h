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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Topsy/mips/RCS/Exception.h,v $
 	Author(s):             George Fankhauser
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.2 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1997/04/23 11:55:28 $      by: $Author: gfa $
	
	
*/
#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_


typedef enum { CLOCKINT_0 = 0,
	       CLOCKINT_1 = 1,
	       CENTRONICS = 3,
	       FPGA_CARD = 4,
	       DUART = 5,
	       SWINT_0 = 6,
	       SWINT_1 = 7
} InterruptId;

/* R3000 defined exceptions (range from 0 to 31) 
 */
typedef enum { HARDWARE_INT,
	       TLB_MOD,
	       TLB_LOAD,
	       TLB_STORE,
	       ADDR_ERR_LOAD, 
	       ADDR_ERR_STORE,
	       BUSERR_INSTR,
	       BUSERR_DATA,
	       SYSCALL, 
	       BREAKPOINT,
	       RES_INSTR,
	       CP_UNUSABLE,
	       OVERFLOW, 
	       UTLB_MISS = 32 
} ExceptionId;

#endif
