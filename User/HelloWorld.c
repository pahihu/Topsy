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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/User/RCS/HelloWorld.c,v $
 	Author(s):             Marcus Brunner 
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.2 $
 	Creation Date:         1.4.97
 	Last Date of Change:   $Date: 1998/04/07 17:03:42 $      by: $Author: fiedler $
	
	
*/

#include "../Topsy/Syscall.h"
#include "UserSupport.h"

void Hello() {
    ThreadId console;
    unsigned long int size;
    char hello[] = "Hello World\n";

    size = sizeof(hello);
    ioOpen(IO_CONSOLE, &console);
    ioInit(console);
    ioWrite(console, hello, &size);
    ioClose(console);
}
