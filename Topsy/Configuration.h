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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Topsy/RCS/Configuration.h,v $
 	Author(s):             George Fankhauser
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.8 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1998/03/26 19:46:33 $      by: $Author: gfa $
	
	
*/
#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

/* general, messages, prompts, look and feel 
 */
#define BOOTMESSAGE "Topsy 1.1 (c) 1996-1998, ETH Zurich, TIK\n($Id: Configuration.h,v 1.8 1998/03/26 19:46:33 gfa Exp $)\n\n"

#if defined(__MIPSEB__) || defined(__BIG_ENDIAN__)
#define TOPSY_BIG_ENDIAN
#endif


/* memory parameters 
 */
#define KERNELHEAPSIZE   (64*1024)   /* kernel heap size */


/* threads, IPC 
 */
#define TIMESLICE         10   /* how long a thread may run, in milliseconds */
#define NBPRIORITYLEVELS   3   /* 0 for highest priority */
#define MAXNBMSGINQUEUE    4   /* message queue length */
#define MAXNAMESIZE       24   /* max thread name size (in characters) */
#define TM_DEFAULTTHREADSTACKSIZE   1024


/* io features 
 */
#define NBCYCLESFORDELAY 10    /* each time a write occurs onto a driver, */
                               /* at least NULL-operations are performed  */

#endif
