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


  
  File:          $Source: /proj/topsy/ss98/Topsy/User/RCS/PacketStackGlobals.h,v $
  Author(s):         Bernard Stauffer 
  Affiliation:       ETH Zuerich, TIK
  Version:         $Revision: 1.3 $
  Creation Date:       1.4.97
  Last Date of Change:   $Date: 1997/06/03 06:55:17 $    by: $Author: gfa $
  
  
*/


#ifndef _packetstack_globals
#define _packetstack_globals



/* Constants */

#define MAXPACKETLEN  256

                                  /* Used as indices for My_ThreadList */
#define TTY_ID        0           /* Driver threads (Kernel) */
#define DRIVER_ID     1
#define CONTROL_ID    2           /* User threads */
#define SENDER_ID     3
#define RECEIVER_ID   4
#define IRQSEND_ID    7
#define IRQRECV_ID    8
#define FRAMER_ID     5
#define DEFRAMER_ID   6
#define IRQTIMER_ID   9

#define THREAD_COUNT  10



/* Vars */

ThreadId  My_ThreadList[THREAD_COUNT];    /* List of all used thread id's */

volatile Boolean    packetStartFlag;      /* HAS TO BE VOLATILE !!!!! */

unsigned long TimerEnable;



#endif

