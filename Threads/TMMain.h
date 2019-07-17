#ifndef __TMMAIN_H
#define __TMMAIN_H
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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Threads/RCS/TMMain.h,v $
 	Author(s):             Christian Conrad
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.5 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1997/04/23 09:04:54 $      by: $Author: gfa $
	
	
*/

#include "tm-include.h"

void tmMain( ThreadArg userInitAddress);

#endif __TMMAIN_H
