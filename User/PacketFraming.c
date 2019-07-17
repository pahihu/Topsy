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


  
  File:                  $Source: /proj/topsy/ss98/Topsy/User/RCS/PacketFraming.c,v $
  Author(s):             Bernard Stauffer 
  Affiliation:           ETH Zuerich, TIK
  Version:               $Revision: 1.6 $
  Creation Date:         1.5.97
  Last Date of Change:   $Date: 1998/04/07 15:21:53 $      by: $Author: fiedler $
*/  


#include "../Topsy/Topsy.h"
#include "../Topsy/Syscall.h"
#include "UserSupport.h"

#include "PacketStackGlobals.h"
#include "PacketFraming.h"

#define DATALINKESCAPE '\\'
#define BEGINOFPACKET '['
#define ENDOFPACKET ']'

/* -------------------------------------------------------------------------------------- */
/* Framer thread: receives packets from the IRQ thread and marks beginning of each packet */
/* with character sequence '|[` and ending of each packet with '|]'. Stuffing of character*/
/* '\\' within the packets */

void PacketFramer() {

  Message         putmsg, putmsg_ack;
  ThreadId        from;                       /* included for IRQControl */
  char            *ptrbuffer;
  char            beginofpacket = BEGINOFPACKET;
  char            endofpacket = ENDOFPACKET;
  char            datalinkescape = DATALINKESCAPE;
  unsigned long   buffersize, one, i;


  /* to be done by students */

}


/* ---------------------------------------------------------------------------- */
/* Deframer thread: gets packets from the IO driver and packetize the stream.   */

void PacketDeframer() {

  Message         getmsg, getmsg_ack;
  char            driverbuffer[MAXPACKETLEN], packetbuffer[MAXPACKETLEN];
  unsigned long   pos_driverbuffer, pos_packetbuffer, packetbuffersize, readlen;
  Boolean         packet_assembled, dle_detected;
  
  /* to be done by students */

}

/* ---------------------------------------------------------------------------- */
