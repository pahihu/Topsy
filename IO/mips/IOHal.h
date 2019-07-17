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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/IO/mips/RCS/IOHal.h,v $
 	Author(s):             George Fankhauser
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.11 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1997/04/22 12:10:36 $      by: $Author: conrad $
	
	
*/

#include "Configuration.h"

#define MODE_REGISTER 0x0	/* SCN2681 register layout */
#define STATUS_REGISTER 0x4
#define COMMAND_REGISTER 0x8
#define TX_REGISTER 0x0c
#define INTERRUPT_REGISTER 0x14

#define READY_TO_SEND 0x04	/* ready to send status */

#ifdef TOPSY_BIG_ENDIAN
#define UART_A_BASE 0xbfe00003	/* kernel uncached address */
#define UART_B_BASE 0xbfe00023	/* kernel uncached address */
#else
#define UART_A_BASE 0xbfe00000	/* kernel uncached address */
#define UART_B_BASE 0xbfe00020	/* kernel uncached address */
#endif
#define CONSOLE_BASE UART_B_BASE

#define MAXPUTSTRINGLEN 320

/* those two contain hardware driver code */
void ioConsoleInit();
void ioConsolePutChar(char c);
