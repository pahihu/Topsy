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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/User/RCS/CrashMe.h,v $
 	Author(s):             C. Conrad 
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.3 $
 	Creation Date:         21.3.97
 	Last Date of Change:   $Date: 1997/04/08 12:11:04 $      by: $Author: conrad $
	
	
*/


typedef enum {SYSCALL_TEST, RESOURCE_TEST, RANDOM_TEST} TestId;

typedef struct ThreadParam_t {
    ThreadId id;
    ThreadId tty;
} ThreadParam;

/* void crashMeMaster( char* argArray[] ); */
void crashMeMaster( void* pArg );
