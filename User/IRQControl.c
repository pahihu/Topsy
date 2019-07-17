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


  
  File:                  $Source: /proj/topsy/ss98/Topsy/User/RCS/IRQControl.c,v $
  Author(s):             Milan Tadjan 
  Affiliation:           ETH Zuerich, TIK
  Version:               $Revision: 1.7 $
  Creation Date:         1.5.97
  Last Date of Change:   $Date: 1998/04/07 16:16:08 $      by: $Author: fiedler $
*/  

#include "../Topsy/Topsy.h"
#include "../Topsy/Syscall.h"
#include "UserSupport.h"

#include "PacketStackGlobals.h"
#include "IRQControl.h"



/* ---------------------------------------------------------------------------- */
/* IRQ sender thread: placeholder for exercice 4.				*/
/* Waits for a job from sender thread and passes data to framer thread.	        */

void IRQ_Send() {
  Message		msg;
  
  while (!packetStartFlag) tmYield;		/* Wait for startFlag = true */
  
  while (TRUE) {
    /* Wait for message from Sender */
    tmMsgRecv(&My_ThreadList[SENDER_ID], ANYMSGTYPE, &msg, INFINITY);
    /* Send message to framer */
    tmMsgSend(My_ThreadList[FRAMER_ID], &msg);
    /* Wait for ack from framer */
    tmMsgRecv(&My_ThreadList[FRAMER_ID], ANYMSGTYPE, &msg, INFINITY);
    /* Send ack to sender */	
    tmMsgSend(My_ThreadList[SENDER_ID], &msg);	
  }
}	

/* ---------------------------------------------------------------------------- */
/* IRQ receiver thread: placeholder for exercice 4.				*/
/* Initiates reading from the IO driver and passes data to receiver thread.	*/

void IRQ_Receive() {
  Message		msg;
  
  msg.id = USER;  /* Set message id of user message (static) */

  while (!packetStartFlag) tmYield;		/* Wait for startFlag = true */

  while (TRUE) {
    /* Initiate a read from IO */
    tmMsgSend(My_ThreadList[DEFRAMER_ID], &msg);
    /* Wait for ack from IO */
    tmMsgRecv(&My_ThreadList[DEFRAMER_ID], ANYMSGTYPE, &msg, INFINITY);
    /* Send message to receiver */
    tmMsgSend(My_ThreadList[RECEIVER_ID], &msg);
    tmMsgRecv(&My_ThreadList[RECEIVER_ID], ANYMSGTYPE, &msg, INFINITY);
  }
}
/* ---------------------------------------------------------------------------- */
