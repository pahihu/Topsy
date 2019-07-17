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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/User/RCS/testtimer.c,v $
 	Author(s):             Matthias Gries
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.4 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1998/04/08 15:34:25 $      by: $Author: fiedler $
	
	
*/

#include "testtimer.h"

void test_timer()
{
  ThreadId from, console, timerthread, dummy;
  Message timermsg, msg;
  char hello[] = "\n  Console Init\n";
  int size, i;

  if (IO_OPENOK != ioOpen(IO_CONSOLE, &console)) /* Open and initialise serial IO */
      return;
  else ioInit(console);
  size = stringLength(hello); /* Hello message */
  ioWrite(console, hello, (unsigned long int*) &size);

  if (tmGetInfo(SELF, &from, &dummy) != TM_INFOOK) 
      display(console,"tmGetInfo failed\n");

  /* timermsg.from =      ;
  timermsg.id =        ;
  timermsg.msg.ioTimer.period = 1500; */ /* 1500 ms */

  /* if (IO_OPENOK != ioOpen(        ,           ))*/ /* Open and initialise timer1 */
  /*  {
      display(console, "unable to open timer1\n");
      return;
    }

  ioInit(                 );      

  for (i = 1; i < 10; i++)
    {
      if (tmMsgSend(           ,             ) == TM_MSGSENDFAILED) 
	display(console, "timermsg send fails in thread test_timer\n");
      else display(console,"timer set, waiting for an answer\n");
      
      from = ANY;
      tmMsgRecv(&from, ANYMSGTYPE, &msg, INFINITY);    
      
      if ((msg.from ==              ) && (msg.id ==                 ))  
	display(console, "count down finished\n");
      else display(console, "another msg\n");
    } */ /* for */

  /* ioClose(                  ); */
  ioClose(console);
}



