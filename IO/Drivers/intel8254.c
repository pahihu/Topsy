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


	File:                  $Source: /proj/topsy/ss98/Topsy/IO/Drivers/RCS/intel8254.c,v $
 	Author(s):             Matthias Gries
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.4 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1998/04/01 09:44:20 $      by: $Author: gries $
	
	
*/


#include "Drivers/intel8254.h"
#include "Messages.h"
#include "IODevice.h"
#include "Syscall.h"

typedef struct {
  ThreadId callerid; /* ThreadId of calling thread */
  ThreadId timerid;  /* ThreadId of Timer-driver thread */
}timer_temp;


void timer1_interruptHandler(IODevice this) 
{
  /* tmResetClockInterrupt(CLOCK1);*/ /* disables interrupt line */
  /* this is already done in "intDispatcher()" in TMHal.c */

  /* ioConsolePutString("interrupt handler timer1\n"); */

   if (this->extension != NULL) /* timer has been started using msg handler */ 
     { 
       /* Message sendmsg; 
      
       sendmsg.from =                 ; 
       sendmsg.id   =                 ;  

       if ( kSend(                       ,               ) != TM_OK )  
         PANIC("timer interrupt handler unable to send message"); 
       
       hmFree(                            );  
       this->extension = NULL; */ 

       /* count-down finished, no further interrupts */
     } 
}


Error timer1_init(IODevice this) 
{
  /* ioConsolePutString("timer init\n"); */

  if (this->isInitialised == FALSE) 
    {
      this->isInitialised = TRUE;
      this->extension = NULL; /* just to be sure */
    }
  return IO_INITOK;
}


Error timer1_stop(IODevice this) 
{
  /* ioConsolePutString("timer stop\n"); */

  if (this->isInitialised) 
    this->isInitialised = FALSE; /* Unset Initialized flag */    
  
  if (this->extension != NULL) hmFree((Address) this->extension);
  this->extension = NULL;
  
  return IO_CLOSEOK;
}


void timer1_messageHandler(IODevice this, Message* msg)
{
  if ((msg->id == IO_TIMER_START) && (msg->msg.ioTimer.period != 0))
    {
      ThreadId dummy;
      /* ioConsolePutString("timer message handler : start message received\n"); */
      
      if ((msg->msg.ioTimer.period > 0) && (msg->msg.ioTimer.period <= 1750))
	{
	  if (setClockValue(CLOCK1, msg->msg.ioTimer.period, SWTRIGGER) != TM_OK)
	    PANIC("timer1 could not be properly configured");
	}
      else 
        {
	  ioConsolePutString("period not within timer range (1 to 1750 [ms]) \n");
	  return;
      	}
      
      if (HM_ALLOCFAILED == hmAlloc((Address*) &(this->extension), sizeof(timer_temp)))
	PANIC("no heap space left in timer1_messageHandler");
      else ((timer_temp*)(this->extension))->callerid = msg->from; /* used in msg interrupt handler */
	  
      if (tmGetInfo(SELF, &(((timer_temp*)(this->extension))->timerid), &dummy) != TM_INFOOK)  
	/* get own ThreadId */ 
	ioConsolePutString("tmGetInfo failed in timer1-msghandler\n");	
    }

  else ioConsolePutString("WARNING: message not understood by timer1 message handler\n");
}


















