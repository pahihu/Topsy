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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Topsy/mips/RCS/SyscallMsg.S,v $
 	Author(s):             Eckart Zitzler, Christian Conrad, George Fankhauser
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.17 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1997/04/04 18:23:25 $      by: $Author: gfa $
	
	
*/


#define SYSCALL_SEND_OP      0
#define SYSCALL_RECV_OP      1
#define MESSAGEFROM_OFFSET   0
  
#include "asm.h"

    .text
    .align 2

    /* Basic mechanismus for sending messages (SYSCALL exception),
     * Argument registers are:
     *     a0: destination thread (ThreadId)
     *     a1: message reference (Message *msg)
     */
  
    NESTED(tmMsgSend, 32, ra)
	sw         a0, 0(a1)        /* dest. thread put into msg from field */
	li         a3, SYSCALL_SEND_OP
    
	/* Argument registers are now:
	    a0: - (undefined)
	    a1: message reference (Message*) (destination thread id included)
	    a2: - (undefined)
	    a3: SYSCALL_SEND_OP
	*/
	
	syscall          /* Raising of an exception (trap to kernel mode) */
    
	/* The return value of tmMsgSend() is in v0 is it was stored 
	 * the senders context in msgDispatcher 
	 */
    ENDNESTED(tmMsgSend, 32)



    /* Basic mechanismus for receiving messages (SYSCALL exception),
	Argument registers are:
	    a0: expected thread id (ThreadId*)
	    a1: expected message type (MessageId)
	    a2: message reference (Message*)
	    a3: timeout (int milliseconds)
    */
    NESTED(tmMsgRecv, 32, ra)
        lw         t0, 0(a0)     /* dereference a0 -> t0 */
        sw         t0, 0(a2)     /* threadId (t0) copied into from field */
        sw         a1, 4(a2)     /* msgId (a1) copied into id field of msg */
        move       a1, a2        /* shifting of arguments */
        move       a2, a3        /* shifting of arguments */
        li         a3, SYSCALL_RECV_OP
        
        /* Argument registers are now:
              a0: - (undefined) (actually unchanged)
              a1: message reference (Message*)
              a2: timeout (unsigned long int)
              a3: SYSCALL_RECV_OP
        */

        syscall               /* Raising of an exception (trap) */

        /* from field of tmMsgRecv() is set with msgPtr->from */
        lw         t0, MESSAGEFROM_OFFSET(a1)
        sw         t0, 0(a0)

        /* The return value of tmMsgRecv() is in v0 */
    ENDNESTED(tmMsgRecv, 32)



