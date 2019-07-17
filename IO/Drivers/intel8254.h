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

	
	File:                  $Source: /proj/topsy/ss98/Topsy/IO/Drivers/RCS/intel8254.h,v $
 	Author(s):             Matthias Gries
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.2 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1998/03/31 14:47:33 $      by: $Author: gries $
*/
#include "Topsy.h"
#include "MMHeapMemory.h"
#include "IODevice.h"
#include "Configuration.h"

#include "TMClock.h"

void  timer1_interruptHandler(IODevice this);
Error timer1_init(IODevice this);
Error timer1_stop(IODevice this);
void  timer1_messageHandler(IODevice this, Message* msg);
