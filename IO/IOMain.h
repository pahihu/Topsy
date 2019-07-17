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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/IO/RCS/IOMain.h,v $
 	Author(s):             George Fankhauser
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.10 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1998/03/31 16:41:35 $      by: $Author: gries $
	
	
*/

#include "IO.h"
#include "IODevice.h"
#include "HashList.h"
#include "Threads.h"

#include "Drivers/IDT385_IOMap.h"
#include "Drivers/SCN2681_DUART.h"
#include "Drivers/FPGA_Comm.h"
#include "Drivers/Loopback.h"
#include "Drivers/intel8254.h"

void ioMain(ThreadArg arg);
