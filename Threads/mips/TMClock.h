#ifndef __TMCLOCK_H
#define __TMCLOCK_H
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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Threads/mips/RCS/TMClock.h,v $
 	Author(s):             C. Conrad & G. Fankhauser
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.5 $
 	Creation Date:         18.3.97: 1997/03/18 16:33:01 $
	
	
*/

#include "tm-include.h"
#include "cpu.h"
#include "Configuration.h"

/* 82C54 Programmable Timer */
#ifdef TOPSY_BIG_ENDIAN
#define TIMERBASE          (0x1f800000+K1SEG_BASE+3)
#else
#define TIMERBASE          (0x1f800000+K1SEG_BASE)
#endif
#define COUNTER0           (TIMERBASE+0x0)
#define COUNTER1           (TIMERBASE+0x4)
#define COUNTER2           (TIMERBASE+0x8)
#define TIMERCTRLREGISTER  (TIMERBASE+0xc)
#define RESETCINT0         (TIMERBASE+0x10)
#define RESETCINT1         (TIMERBASE+0x14)

#define MODE2_COUNTER0  0x34     /* specific control word */
#define MODE2_COUNTER1  0x74     /* specific control word */
#define MODE2_COUNTER2  0xb4     /* specific control word */

#define MODE4_COUNTER0  0x38     /* specific control word */
#define MODE4_COUNTER1  0x78     /* specific control word */
#define MODE4_COUNTER2  0xb8     /* specific control word */

#define MODE5_COUNTER0  0x3a     /* specific control word */
#define MODE5_COUNTER1  0x7a     /* specific control word */
#define MODE5_COUNTER2  0xba     /* specific control word */

/* The 2 timers of the 82C54 programmable interval timer */
typedef enum { CLOCK0, CLOCK1 } ClockId;

/* Used modes for the 82C54, currently a single mode */
typedef enum { RATEGENERATOR, SWTRIGGER } ClockMode;

/* Configuration of the 82C54 (each counter separately) */
Error setClockValue( ClockId cId, int period, ClockMode cMode);

void tmResetClockInterrupt( ClockId cId);

#endif /* __TMCLOCK_H */
