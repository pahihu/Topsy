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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Threads/mips/RCS/TMClock.c,v $
 	Author(s):             C. Conrad & G. Fankhauser
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.7 $
 	Creation Date:         18.3.97
 	Last Date of Change:   $Date: 1998/03/31 17:13:10 $      by: $Author: gries $
	
	
*/

#include "TMHal.h"
#include "IOHal.h"
#include "IODevice.h"
#include "TMScheduler.h"
#include "Support.h"
#include "TMClock.h"


/* 8254 Register Addressing structure */
typedef struct TimerRegisters_t{
    unsigned char counter0;
    unsigned char pad1[3];
    unsigned char counter1;
    unsigned char pad2[3];
    unsigned char counter2;
    unsigned char pad3[3];
    unsigned char ctrlWord;
    unsigned char pad4[3];
} TimerRegisters;

/*
 * This function initializes the clock on the board.
 */
Error setClockValue( ClockId cId,
		     int period,
		     ClockMode cMode)
{
    TimerRegisters* timerRegisters = (TimerRegisters*)TIMERBASE;
    unsigned short int initialCount;  /* count for counter 0 or 1 */
    unsigned short int initialCount2 = 100;    /* count for counter 2 */
    int frequencyCoeff = 3686 / initialCount2;
    /* Cristal oscillator frequency: 3.6861 MHz.
       Period given in [ms], -> #ticks = 1000*period[ms]*freq[MHz].
       As there are two cascaded timers (c2-c0 or c2-c1), each counter
       should ideally have an initial value of sqrt(initialCount) */
    initialCount = period * frequencyCoeff;
    
    if (cMode == RATEGENERATOR) {  /* single mode supported */
	switch (cId) {
	case CLOCK0:
	  timerRegisters->ctrlWord = (unsigned char)MODE2_COUNTER0;
	  ioDelayAtLeastCycles( NBCYCLESFORDELAY);
	  timerRegisters->counter0 = (unsigned char)initialCount;
	  ioDelayAtLeastCycles( NBCYCLESFORDELAY);
	  timerRegisters->counter0 = (unsigned char)(initialCount >> 8);
	  ioDelayAtLeastCycles( NBCYCLESFORDELAY);
	  break;
	case CLOCK1:
	  timerRegisters->ctrlWord = MODE2_COUNTER1;
	  ioDelayAtLeastCycles( NBCYCLESFORDELAY);
	  timerRegisters->counter1 = (unsigned char)initialCount;
	  ioDelayAtLeastCycles( NBCYCLESFORDELAY);
	  timerRegisters->counter1 = (unsigned char)(initialCount >> 8);
	  ioDelayAtLeastCycles( NBCYCLESFORDELAY);
	  break;
	}
	timerRegisters->ctrlWord = MODE2_COUNTER2;
	ioDelayAtLeastCycles( NBCYCLESFORDELAY);
	timerRegisters->counter2 = (unsigned char)initialCount2;
	ioDelayAtLeastCycles( NBCYCLESFORDELAY);
	timerRegisters->counter2 = (unsigned char)(initialCount2 >> 8);
	ioDelayAtLeastCycles( NBCYCLESFORDELAY);
    }
    else if (cMode == SWTRIGGER) {  /* intel 8254 mode4 */
	switch (cId) {
	case CLOCK0:
	  timerRegisters->ctrlWord = (unsigned char)MODE4_COUNTER0;
	  ioDelayAtLeastCycles( NBCYCLESFORDELAY);
	  timerRegisters->counter0 = (unsigned char)initialCount;
	  ioDelayAtLeastCycles( NBCYCLESFORDELAY);
	  timerRegisters->counter0 = (unsigned char)(initialCount >> 8);
	  ioDelayAtLeastCycles( NBCYCLESFORDELAY);
	  break;
	case CLOCK1:
	  timerRegisters->ctrlWord = MODE4_COUNTER1;
	  ioDelayAtLeastCycles( NBCYCLESFORDELAY);
	  timerRegisters->counter1 = (unsigned char)initialCount;
	  ioDelayAtLeastCycles( NBCYCLESFORDELAY);
	  timerRegisters->counter1 = (unsigned char)(initialCount >> 8);
	  ioDelayAtLeastCycles( NBCYCLESFORDELAY);
	  break;
	}
	timerRegisters->ctrlWord = MODE2_COUNTER2; /* CLOCK2 must use mode2 */
	ioDelayAtLeastCycles( NBCYCLESFORDELAY);
	timerRegisters->counter2 = (unsigned char)initialCount2;
	ioDelayAtLeastCycles( NBCYCLESFORDELAY);
	timerRegisters->counter2 = (unsigned char)(initialCount2 >> 8);
	ioDelayAtLeastCycles( NBCYCLESFORDELAY);
    }
    
    return TM_OK;
}

/* When a clock interrupt is generated, a reset is performed. If not done,
 * the interrupt is generated at once again and again ...
 */
void tmResetClockInterrupt(ClockId cId)
{
    char* x;
    char y;
    
    switch (cId) {
    case CLOCK0:
      x = (char*)RESETCINT0;
      break;
    case CLOCK1:
      x = (char*)RESETCINT1;
      break;
    default:
      return;
    }
    y = *x;
}
    
