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


  
  File:                  $Source: /proj/topsy/ss98/Topsy/User/RCS/PacketStackControl.c,v $
  Author(s):             Bernard Stauffer 
  Affiliation:           ETH Zuerich, TIK
  Version:               $Revision: 1.10 $
  Creation Date:         1.5.97
  Last Date of Change:   $Date: 1998/04/08 15:34:25 $      by: $Author: fiedler $
*/  


#include "../Topsy/Topsy.h"
#include "../Topsy/Syscall.h"
#include "UserSupport.h"

#include "PacketStackGlobals.h"
#include "PacketStackControl.h"
#include "Packetizer.h"
#include "IRQControl.h"
#include "PacketFraming.h"


/* ---------------------------------------------------------------------------- */
/* Starts the other threads and opens the IO drivers.                           */

void PacketStackControl() {

  Message         endmsg;
  ThreadId       any=ANY, self, parent;
  unsigned long   i, size;

  char hello[] = "\n   Hello to the PacketDriverStack\n";
  char byebye[] = "\n   Byebye ... (PacketDriverStack end)\n";


/* Get my own thread ID */
  if (tmGetInfo(SELF, &self, &parent) != TM_INFOOK) {
    display(My_ThreadList[TTY_ID],"tmGetInfo failed\n");
  }
  else {
    My_ThreadList[CONTROL_ID] = self;
  }
  packetStartFlag = FALSE;


/* Open and initialize the used drivers */
  ioOpen(IO_CONSOLE, &My_ThreadList[TTY_ID]);       /* Open and initialise serial IO */
  ioInit(My_ThreadList[TTY_ID]);

/* for test purposes FIE 30.3.97 
  ioOpen(IO_FPGA_COMM, &My_ThreadList[DRIVER_ID]);     Open and initialise FPGA IO */
  ioOpen(IO_LOOPBACK, &My_ThreadList[DRIVER_ID]);
  ioInit(My_ThreadList[DRIVER_ID]);

  display(My_ThreadList[TTY_ID],"0\n");

  size = stringLength(hello);                             /* Hello message */
  ioWrite(My_ThreadList[TTY_ID], hello, &size);


/* Start the threads (take care about the proper sequence) */

  if (tmStart(&My_ThreadList[RECEIVER_ID], PacketReceiver, (ThreadArg)0, "PacketReceiver") == TM_STARTFAILED) {
    display(My_ThreadList[TTY_ID],"Start PacketReceiver thread failed\n");
  } 
  if (tmStart(&My_ThreadList[DEFRAMER_ID], PacketDeframer, (ThreadArg)0, "PacketDeframer") == TM_STARTFAILED) {
    display(My_ThreadList[TTY_ID],"Start PacketDeframer thread failed\n");
  } 
  if (tmStart(&My_ThreadList[IRQRECV_ID], IRQ_Receive, (ThreadArg)0, "IRQ_Receive") == TM_STARTFAILED) {
    display(My_ThreadList[TTY_ID],"Start IRQ_Receive thread failed\n");
  } 
  if (tmStart(&My_ThreadList[FRAMER_ID], PacketFramer, (ThreadArg)0, "PacketFramer") == TM_STARTFAILED) {
    display(My_ThreadList[TTY_ID],"Start PacketFramer thread failed\n");
  } 
  if (tmStart(&My_ThreadList[IRQSEND_ID], IRQ_Send, (ThreadArg)0, "IRQ_Send") == TM_STARTFAILED) {
    display(My_ThreadList[TTY_ID],"Start IRQ_Send thread failed\n");
  } 
  if (tmStart(&My_ThreadList[SENDER_ID], PacketSender, (ThreadArg)0, "PacketSender") == TM_STARTFAILED) {
    display(My_ThreadList[TTY_ID],"Start PacketSender thread failed\n");
  }

/* Now, every thread is ready to work: we can set the start flag ...                   */
/* This is necessary, because both partners of a message channel have to be started,   */
/* before they can exchange message (else, you would get errors).                      */

  packetStartFlag = TRUE;
  

/* Now, every thread should work and we wait for the end ... */
                      
                                                /* Wait for the end message */
  tmMsgRecv(&any, ANYMSGTYPE, &endmsg, INFINITY);


  

  size = stringLength(byebye);                        /* Bye bye message */
  ioWrite(My_ThreadList[TTY_ID], byebye, &size);
  
  tmKill(My_ThreadList[SENDER_ID]);
  tmKill(My_ThreadList[RECEIVER_ID]);
  tmKill(My_ThreadList[IRQSEND_ID]);
  tmKill(My_ThreadList[IRQRECV_ID]);
  tmKill(My_ThreadList[FRAMER_ID]);
  tmKill(My_ThreadList[DEFRAMER_ID]);
}


/* ---------------------------------------------------------------------------- */
