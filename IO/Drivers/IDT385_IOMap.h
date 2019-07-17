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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/IO/Drivers/RCS/IDT385_IOMap.h,v $
 	Author(s):             George Fankhauser
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.7 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1997/05/16 14:39:38 $      by: $Author: stauffer $
	
	
*/
#include "Configuration.h"

#ifdef TOPSY_BIG_ENDIAN

#define UART_A_BASE 0xbfe00003	/* kernel uncached address */
#define UART_B_BASE 0xbfe00023	/* kernel uncached address */
#define FPGA_BASE   0xbfa40003	/* kernel uncached address */

#else

#define UART_A_BASE 0xbfe00000	/* kernel uncached address */
#define UART_B_BASE 0xbfe00020	/* kernel uncached address */
#define FPGA_BASE   0xbfa40000	/* kernel uncached address */

#endif

