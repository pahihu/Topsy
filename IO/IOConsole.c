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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/IO/RCS/IOConsole.c,v $
 	Author(s):             George Fankhauser
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.1 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1997/04/09 07:44:44 $      by: $Author: conrad $
	
	
*/

#include "Topsy.h"
#include "IOConsole.h"
#include "IOHal.h"

static const char nullString[] = "(null pointer)";
static const char ioHexArray[16] =  {'0','1','2','3','4','5','6','7',
                                     '8','9','a','b','c','d','e','f'};


void ioConsolePutString(const char* s) {
	int i = 0;

	if (s == NULL) s = nullString;
	while ((s[i] != '\0') && (i < MAXPUTSTRINGLEN)) {
		ioConsolePutChar(s[i++]);
	}
}

void ioConsolePutHexInt(int x) {
	unsigned int i;
	for (i = 0; i < sizeof(int) * 2; i++) {
		ioConsolePutChar(ioHexArray[(x >> 28) & 0xf]);
		x = x << 4;
	}
}

void ioConsolePutInt(int x) {
	int divisor = 1;
	Boolean positive = TRUE;
	int y = x;
	
	if (x < 0) { x = -x; y = -y; positive = FALSE; }
	while (y >= 10) { y = y / 10; divisor = divisor * 10; }
	if (!positive) { ioConsolePutChar('-'); }
	y = x;
	while (divisor > 0) {
		ioConsolePutChar(ioHexArray[x/divisor]);
		x = x - (x/divisor)*divisor;
		divisor = divisor / 10;
	}
}
