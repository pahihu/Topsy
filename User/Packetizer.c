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


  
  File:                  $Source: /proj/topsy/ss98/Topsy/User/RCS/Packetizer.c,v $
  Author(s):             Bernard Stauffer 
  Affiliation:           ETH Zuerich, TIK
  Version:               $Revision: 1.5 $
  Creation Date:         1.5.97
  Last Date of Change:   $Date: 1998/04/08 15:34:25 $      by: $Author: fiedler $
*/  



#include "../Topsy/Topsy.h"
#include "../Topsy/Syscall.h"
#include "UserSupport.h"

#include "datafile.h"
#include "Packetizer.h"
#include "PacketStackGlobals.h"



#define TERMINATECHAR '\a'    /* the ASCII BELL char, used here as program end mark */



/* ---------------------------------------------------------------------------- */
/* Sender thread: reads a byte stream and packetize it                          */
/* 'chardata' is an imported text file                                          */

void PacketSender() {

  Message       sendmsg, sendmsg_ack;
  unsigned long inputdatasize, buffersize, i,j;
  char          buffer[MAXPACKETLEN];
  char*         ptrcharfile = charfile;


  while (!packetStartFlag);                /* Wait for startFlag = true */


/* --- Prepare all vars */

  inputdatasize = stringLength(charfile);       /* Get length of input data */

  sendmsg.id = USER;                      /* Set message id of user message (static) */
  sendmsg.msg.userMsg.p1 = buffer;        /* Set message pointer to databuffer (static) */
  sendmsg.msg.userMsg.p2 = &buffersize;   /* Set message pointer to length (static) */

  buffersize = 0;                         /* Counter for packet length */


/* --- Main packetizer loop over the input data */

  for (i=0; i <= inputdatasize; i++) {          /* i: indices over input data */

    if (*ptrcharfile == '\0') {                 /* Test input data */
      ptrcharfile++;                            /* Skip ESCAPE char */
    }
    else {
      buffer[buffersize++] = *ptrcharfile++ ;   /* Copy input data to buffer */
    }

    if ((buffersize == 249) || (i == inputdatasize)) { /* If packet assembled, send it */
      if (tmMsgSend(My_ThreadList[IRQSEND_ID], &sendmsg) == TM_MSGSENDFAILED) {
        display(My_ThreadList[TTY_ID],"tmMsgSend1 failed in thread Sender.\n");
      }
      else {                                    /* if no error, wait for ack message */
        tmMsgRecv(&My_ThreadList[IRQSEND_ID], ANYMSGTYPE, &sendmsg_ack, INFINITY);
      }
      
      buffersize = 0;                           /* Reset packet length counter */
    } 

  }   /* End: for ... */


/* --- The input is packetized, so send now an end mark char */

  buffer[0] = TERMINATECHAR;                    /* Termination message (BELL char) */ 
  buffersize = 1; 
  if (tmMsgSend(My_ThreadList[IRQSEND_ID], &sendmsg) == TM_MSGSENDFAILED) {
    display(My_ThreadList[TTY_ID],"tmMsgSend2 failed in thread Sender.\n");
  }
                                                /* Wait for ack */
  tmMsgRecv(&My_ThreadList[IRQSEND_ID], ANYMSGTYPE, &sendmsg_ack, INFINITY);


/* --- That was it. Now wait for ever, till i'm killed ... */

  while (TRUE);

}



/* ---------------------------------------------------------------------------- */
/* Receiver thread: receives packets from the IRQ thread and prints them to the CONSOLE */

void PacketReceiver() {

  Message         recvmsg, recvmsg_ack;
  ThreadId        from;
  char            *ptrbuffer;
  unsigned long   buffersize, i;
  
  char s[256];


  while (!packetStartFlag);       /* Wait for startFlag = true */


/* --- Prepare all vars */

  recvmsg_ack.id = USER;                  /* Set message id of user message (static) */
  from = ANY;                             /* Set message receive id (static) */


/* --- Main loop. (Loop for ever ...) */

  while (TRUE) {
 
    if (tmMsgRecv(&from, ANYMSGTYPE, &recvmsg, INFINITY) == TM_MSGRECVFAILED) {
        display(My_ThreadList[TTY_ID],"tmMsgRecv failed in thread Receiver.\n");
     }

    ptrbuffer = (char*) recvmsg.msg.userMsg.p1;     /* Copy pointer to local var */
    buffersize = *(long*) recvmsg.msg.userMsg.p2;   /* Copy length to local var */
    
                                          /* If only one char, test if BELL char. If true, ... */
                                          /* ... send termination message to Control thread.   */
    if ((buffersize == 1) && (*ptrbuffer == TERMINATECHAR)) {
      tmMsgSend(My_ThreadList[CONTROL_ID], &recvmsg_ack);
      while (TRUE);              /* Loop for ever, till i'm killed ... */
    }

                                          /* Print received data to console */
    ioWrite(My_ThreadList[TTY_ID], ptrbuffer, &buffersize); 
    
                                          /* Send back ack message */
    if (tmMsgSend(recvmsg.from, &recvmsg_ack) == TM_MSGSENDFAILED) {
      display(My_ThreadList[TTY_ID],"tmMsgSend failed in thread Receiver.\n");
    }

  }   /* End: while ... */

}


/* ---------------------------------------------------------------------------- */

