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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/IO/RCS/IO.h,v $
 	Author(s):             George Fankhauser
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.14 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1998/03/31 16:02:46 $      by: $Author: gries $
	
	
*/

/* device numbers */
#define IO_SERIAL_A 	0
#define IO_SERIAL_B 	1
#define IO_FPGA_COMM 	2
#define IO_LOOPBACK		3
/* #define TIMER1         4 */
/* do not forget to increment IO_DEVCOUNT */

#define IO_CONSOLE 		IO_SERIAL_B /* alias */

#define IO_DEVCOUNT 4

#include "IOConsole.h"

