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


  
  File:                  $Source: /proj/topsy/ss98/Topsy/User/RCS/IRQControl.h,v $
  Author(s):             Milan Tadjan 
  Affiliation:           ETH Zuerich, TIK
  Version:               $Revision: 1.4 $
  Creation Date:         1.4.97
  Last Date of Change:   $Date: 1997/06/03 06:56:26 $      by: $Author: gfa $
  
  
*/

#define MAXLEN 256
#define ACK '\x06'
#define NACK '\x15'
#define MAX_ERROR_COUNT 10
#define DATAPACKET 2
#define ACKPACKET 3


void IRQ_Send();

void IRQ_Receive();

void IRQ_Timer();

unsigned long CheckCrc(unsigned char buf[]);
unsigned long CheckSeqNr(unsigned long i,unsigned long j);
unsigned long GetSeqNumber(unsigned char *Buf);
unsigned long crc(unsigned char *buf);
unsigned long GetEventId(Message msg);
void BuildIframe(unsigned char *s,unsigned long SequenceNr,unsigned long PacketType);
void CopyBuf(unsigned char *s, unsigned char t[],unsigned long offset,unsigned long length);
void ShiftBuf(unsigned char *s,unsigned long shift,unsigned long packetlength);
void irqInit(unsigned long *ErrorCount,unsigned long *RetxCount,unsigned long *Vs,unsigned long *PresentState,unsigned long *WaitTime);
void ackInit(unsigned char ack[]);
void StopTimer();

/*   Variable   */

/* unsigned int CRCDIV = 0xC180; */
