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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/User/RCS/ThreadsDemo.c,v $
 	Author(s):             
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.2 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1998/04/07 17:04:07 $      by: $Author: fiedler $
	
	
*/

#include "../Topsy/Syscall.h"
#include "UserSupport.h"

void Child() {
	Message sendmsg;
	ThreadId console, myid, parentid;

	ioOpen(IO_CONSOLE, &console);
	ioInit(console);
	display(console, "child started\n");
	
	tmGetInfo(SELF, &myid, &parentid);
	tmMsgSend(parentid, &sendmsg);

	display(console, "child running\n");
}

void Demo() {
	ThreadId console, child;
	Message reply;
	
	ioOpen(IO_CONSOLE, &console);
	ioInit(console);
	display(console, "parent started\n");
	
	tmStart(&child, (ThreadMainFunction)Child, (ThreadArg)0, "child");

	display(console, "parent running\n");
	
	if (tmMsgRecv(&child, ANYMSGTYPE, &reply, INFINITY) == TM_MSGRECVOK) {
		display(console, "parent received message from child\n");
	}

	display(console, "parent continuing\n");

	if (tmMsgRecv(&child, ANYMSGTYPE, &reply, INFINITY) == TM_MSGRECVFAILED) {
		display(console, "exit of child\n");
	}
}
