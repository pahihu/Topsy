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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Threads/mips/RCS/TMHalAsm.S,v $
 	Author(s):             Christian Conrad
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.45 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1997/04/28 09:59:23 $      by: $Author: gfa $
	
	
*/

#include "asm.h"
#include "cpu.h"
#include "MemoryLayout.h"
  
/* Offset values in the ProcContext structure (expressed in bytes) */
#define RETURNVALUE0_OFFSET        0*4
#define RETURNVALUE1_OFFSET        1*4
#define ARGUMENT0_OFFSET           2*4
#define ARGUMENT1_OFFSET           3*4
#define ARGUMENT2_OFFSET           4*4
#define ARGUMENT3_OFFSET           5*4
#define CALLERSAVED0_OFFSET        6*4
#define CALLERSAVED1_OFFSET        7*4
#define CALLERSAVED2_OFFSET        8*4
#define CALLERSAVED3_OFFSET        9*4
#define CALLERSAVED4_OFFSET        10*4
#define CALLERSAVED5_OFFSET        11*4
#define CALLERSAVED6_OFFSET        12*4
#define CALLERSAVED7_OFFSET        13*4
#define CALLERSAVED8_OFFSET        14*4
#define CALLERSAVED9_OFFSET        15*4
#define CALLEESAVED0_OFFSET        16*4
#define CALLEESAVED1_OFFSET        17*4
#define CALLEESAVED2_OFFSET        18*4
#define CALLEESAVED3_OFFSET        19*4
#define CALLEESAVED4_OFFSET        20*4
#define CALLEESAVED5_OFFSET        21*4
#define CALLEESAVED6_OFFSET        22*4
#define CALLEESAVED7_OFFSET        23*4
#define GLOBALPOINTER_OFFSET       24*4
#define STACKPOINTER_OFFSET        25*4
#define FRAMEPOINTER_OFFSET        26*4
#define RETURNADDRESS_OFFSET       27*4
#define PROGRAMCOUNTER_OFFSET      28*4
#define HI_OFFSET                  29*4
#define LO_OFFSET                  30*4
#define STATUSREGISTER_OFFSET      31*4
#define AT_OFFSET                  32*4

/* Offset of the processor context in the Thread structure */
#define THREADCONTEXT_OFFSET       0*4
#define THREADID_OFFSET            1*4
#define SCHEDRUNNING_OFFSET        0*4
#define UTLBMISS_OFFSET            32*4
  
/* Some constants that are already defined in other places (attention!)... */
#define SYSCALL_SEND_OP  0

/* General constants */
#define BIT_UNSET 0
#define BIT_SET   1
  
        .text
        .align 2
 
    /* Save context of current thread. All registers but k0, k1, AT and
     * zero are saved in the ProcContext structure given by the first
     * argument to saveContext(), thus register a0. 
     */ 
    FRAME(saveContext)

    .set noat
    
    /* Setting of exceptionContextFlag */
    la      k0, exceptionContextFlag
    li      k1, BIT_SET
    sw      k1, 0(k0)

    sw      a1, ARGUMENT1_OFFSET(a0)
    sw      a2, ARGUMENT2_OFFSET(a0)
    sw      a3, ARGUMENT3_OFFSET(a0)
    sw	    v0, RETURNVALUE0_OFFSET(a0)
    sw	    v1, RETURNVALUE1_OFFSET(a0)
    sw	    AT, AT_OFFSET(a0)
    sw      t0, CALLERSAVED0_OFFSET(a0)
    sw      t1, CALLERSAVED1_OFFSET(a0)
    sw      t2, CALLERSAVED2_OFFSET(a0)
    sw      t3, CALLERSAVED3_OFFSET(a0)
    sw      t4, CALLERSAVED4_OFFSET(a0)
    sw      t5, CALLERSAVED5_OFFSET(a0)
    sw      t6, CALLERSAVED6_OFFSET(a0)
    sw      t7, CALLERSAVED7_OFFSET(a0)
    sw      t8, CALLERSAVED8_OFFSET(a0)
    sw      t9, CALLERSAVED9_OFFSET(a0)
    sw      s0, CALLEESAVED0_OFFSET(a0)
    sw      s1, CALLEESAVED1_OFFSET(a0)
    sw      s2, CALLEESAVED2_OFFSET(a0)
    sw      s3, CALLEESAVED3_OFFSET(a0)
    sw      s4, CALLEESAVED4_OFFSET(a0)
    sw      s5, CALLEESAVED5_OFFSET(a0)
    sw      s6, CALLEESAVED6_OFFSET(a0)
    sw      s7, CALLEESAVED7_OFFSET(a0)
    sw      gp, GLOBALPOINTER_OFFSET(a0)
    sw      fp, FRAMEPOINTER_OFFSET(a0)
    mfhi    k0
    sw      k0, HI_OFFSET(a0)
    mflo    k0
    sw      k0, LO_OFFSET(a0)
    mfc0    k0, c0_status
    nop
    sw      k0, STATUSREGISTER_OFFSET(a0)

    la      k0, exceptionContext
    lw      k1, 0(k0)                     /* save user a0 */
    nop
    sw      k1, ARGUMENT0_OFFSET(a0)
    lw      k1, 4(k0)                     /* save user ra */
    nop
    sw      k1, RETURNADDRESS_OFFSET(a0)
    lw      k1, 8(k0)                     /* save user sp */
    nop
    sw      k1, STACKPOINTER_OFFSET(a0)
    lw      k1, 12(k0)                    /* save user pc */
    nop
    sw      k1, PROGRAMCOUNTER_OFFSET(a0)

    .set at
    
    END(saveContext)


    /* Restore context of new thread. All registers but k0, k1 and
     * zero are restored from the ProcContext structure in the thread
     * structure reference given by the first argument to restoreContext(),
     * thus register a0. 
     */ 
    FRAME(restoreContext)

    .set noreorder
    .set noat

    move    k1, a0				     
    lw      a0, ARGUMENT0_OFFSET(k1)
    lw      a1, ARGUMENT1_OFFSET(k1)
    lw      a2, ARGUMENT2_OFFSET(k1)
    lw      a3, ARGUMENT3_OFFSET(k1)
    lw	    v0, RETURNVALUE0_OFFSET(k1)
    lw	    v1, RETURNVALUE1_OFFSET(k1)
    lw	    AT, AT_OFFSET(k1)
    lw      t0, CALLERSAVED0_OFFSET(k1)
    lw      t1, CALLERSAVED1_OFFSET(k1)
    lw      t2, CALLERSAVED2_OFFSET(k1)
    lw      t3, CALLERSAVED3_OFFSET(k1)
    lw      t4, CALLERSAVED4_OFFSET(k1)
    lw      t5, CALLERSAVED5_OFFSET(k1)
    lw      t6, CALLERSAVED6_OFFSET(k1)
    lw      t7, CALLERSAVED7_OFFSET(k1)
    lw      t8, CALLERSAVED8_OFFSET(k1)
    lw      t9, CALLERSAVED9_OFFSET(k1)
    lw      s0, CALLEESAVED0_OFFSET(k1)
    lw      s1, CALLEESAVED1_OFFSET(k1)
    lw      s2, CALLEESAVED2_OFFSET(k1)
    lw      s3, CALLEESAVED3_OFFSET(k1)
    lw      s4, CALLEESAVED4_OFFSET(k1)
    lw      s5, CALLEESAVED5_OFFSET(k1)
    lw      s6, CALLEESAVED6_OFFSET(k1)
    lw      s7, CALLEESAVED7_OFFSET(k1)
    lw      gp, GLOBALPOINTER_OFFSET(k1)
    lw      sp, STACKPOINTER_OFFSET(k1)
    lw      fp, FRAMEPOINTER_OFFSET(k1)
    lw      ra, RETURNADDRESS_OFFSET(k1)
					    
    lw      k0, HI_OFFSET(k1)
    nop
    mthi    k0
    lw      k0, LO_OFFSET(k1)
    nop
    mtlo    k0
    lw      k0, STATUSREGISTER_OFFSET(k1)
    nop
    mtc0    k0, c0_status

    /* Unsetting of exceptionContextFlag */
    la      k0, exceptionContextFlag
    sw      zero, 0(k0)
    nop
					    
    /* Setting of pc */
    lw      k0, PROGRAMCOUNTER_OFFSET(k1)
    nop
    j       k0
    rfe

    .set reorder
    .set at
    .end restoreContext


    /* SYSCALL exception handler
     * msgDispatch() in TMIPC is called with message parameters:
     *     a0: from thread (thread that caused the exception)
     *     a1: message reference
     *     a2: - (tmMsgSend) / timeout (tmMsgRecv)
     *     a3: either SYSCALL_SEND_OP or SYSCALL_RECV_OP
     *     parameter5: Branch Delay Bit
     * This exception handler is always set per default.
     */ 
    NESTED(syscallExceptionHandler, 48, ra)
    /* A SYSCALL exception occurred, this means that the return address
     * as saved in the exceptionContext must be incremented with 4
     * (if not, the SYSCALL will be generated ad aeternam !).
     * The fifth parameter of msgDispatch() indicates if the Branch Delay
     * Bit was set or not (CAUSE Register) 
     */				    
    mfc0    v0, c0_cause                    /* cause -> v0 */
    la      k1, scheduler                   /* scheduler -> k1 */
    lw      k1, SCHEDRUNNING_OFFSET(k1)     /* Ptr to current thread -> k1 */
    lw      k1, THREADCONTEXT_OFFSET(k1)    /* ProcContext -> k1 */

    lw      k0, PROGRAMCOUNTER_OFFSET(k1)   /* saved pc -> k0 */
    addu    k0, k0, 4                       /* k0 incremented */
    sw      k0, PROGRAMCOUNTER_OFFSET(k1)   /* pc finally updated */
    and     v1, v0, CAUSE_BD                /* branch delay bit -> v1 */

    /* Branch Delay Bit (in v1) set as fifth parameter to msgDispatcher() */
    sw      v1, 16(sp)                      /* push of v1 onto stack */

    jal     msgDispatcher
    nop
    ENDNESTED(syscallExceptionHandler, 48)


    /* Hardware Interrupt exception handler
     * No parameters are required. This exception handler is set per default
     */ 
    NESTED(hwExceptionHandler, 32, ra)

    mfc0    a0, c0_cause                /* cause -> a0 */
    nop
    jal     intDispatcher		/* decode it in a C function */
    mtc0    zero, c0_cause				    
					    
    ENDNESTED(hwExceptionHandler, 32)


    /*
     * does more or less the same as general exception handler but is called
     * seperately and does only dispatch the utlb miss exception.
     */
    FRAME(UTLBMissHandler)

    /* At this point, only SR and PC were modified.
     * Before calling saveContext(), a0, sp, and ra have to be saved
     * in a temporary exception context.
     * NOTE: When an exception occurs, pc is saved in EPC register.
     * If not in a branch delay, epc points onto 'responsible'
     * instruction (see Branch Delay Bit in CAUSE register) 
     */
    .set noreorder
    mfc0    k1, c0_epc               /* epc -> k1 */
    la      k0, exceptionContext     /* exceptionContext -> k0 */
    sw      a0, 0(k0)                /* user a0 in exceptionContext */
    sw      ra, 4(k0)                /* user ra in exceptionContext */
    sw      sp, 8(k0)                /* user sp in exceptionContext */
    sw      k1, 12(k0)               /* user pc in exceptionContext */

    /* sp is now set on the exception stack (BOOTSTACKTOP) */
    la      sp, BOOTSTACKTOP
					    
    /* Calling saveContext() with currentThreadPtr->context in a0 */
    la      k1, scheduler                   /* scheduler -> k1 */
    lw      a0, SCHEDRUNNING_OFFSET(k1)     /* ProcContext -> a0 */
    jal     saveContext
    lw      a0, THREADCONTEXT_OFFSET(a0)    /* ProcContext -> a0 */
					    /* using branch delay slot */

    /* Invoking exception handler located at offset UTLBMISS_OFFSET in
     * exceptionHandlerTable, the current threadId is set in a0 
     */
    la      k0, exceptionHandlerTable     /* address of exc. table -> k0 */
    addu    k0, k0, UTLBMISS_OFFSET       /* address of corresp. handler->k0 */
    lw      k0, 0(k0)                     /* UTLB handler -> k0 */
    la      k1, scheduler                 /* address of scheduler -> k1 */
    lw      a0, SCHEDRUNNING_OFFSET(k1)   /* a0 contains ptr to current 
					     thread */
    /* Jump to exception handler */
    jal     k0                            /* jump to UTLB exc. handler */
    lw      a0, THREADID_OFFSET(a0)       /* a0 contains thread id */

    /* Calling restoreContext() with currentThreadPtr->context in a0,
     * the 'rfe' call is performed in restoreContext().
     * Note that the necessary schedule was done by kSend().
     */
    la      k1, scheduler                 /* scheduler -> k1 */
    lw      a0, SCHEDRUNNING_OFFSET(k1) /* a0 contains ptr to current thread */
    nop
    lw      a0, THREADCONTEXT_OFFSET(a0)  /* a0 set to context of current 
					    thread */
    j       restoreContext                /* never comes back */
    nop
    .end UTLBMissHandler
    LABEL(endUTLBMissHandler)
    .set reorder	     
					     
					     
    /* This code performs transparently to the user a tmThreadExit() when
     * the main thread routine returns 
     */
    FRAME(automaticThreadExit)

    move    a1, ra
    la      t0, endAutomaticThreadExit
    la      t1, automaticThreadExit
    subu    t1, t0, t1
    addu    a1, a1, t1                  /* Exit message address -> a1 */
    li      a3, SYSCALL_SEND_OP				     
    syscall                             /* Sending of Exit Message */

    END(automaticThreadExit)
    LABEL(endAutomaticThreadExit)
					     
					     
    /* General Exception Handler (located at 0x8000_0080) */
  
    FRAME(generalExceptionHandler)
					    
    /* At this point, only SR and PC were modified.
     * Before calling saveContext(), a0, sp, and ra have to be saved
     * in a temporary exception context.
     * NOTE: When an exception occurs, pc is saved in EPC register.
     * If not in a branch delay, epc points onto 'responsible'
     * instruction (see Branch Delay Bit in CAUSE register) 
     */
    mfc0    k1, c0_epc               /* epc -> k1 */
    la      k0, exceptionContext     /* exceptionContext -> k0 */
    sw      a0, 0(k0)                /* user a0 in exceptionContext */
    sw      ra, 4(k0)                /* user ra in exceptionContext */
    sw      sp, 8(k0)                /* user sp in exceptionContext */
    sw      k1, 12(k0)               /* user pc in exceptionContext */

    /* sp is now set on the exception stack (BOOTSTACKTOP) */
    la      sp, BOOTSTACKTOP
					    
    /* Calling saveContext() with currentThreadPtr in a0 */
    la      k1, scheduler                   /* scheduler -> k1 */
    lw      a0, SCHEDRUNNING_OFFSET(k1)     /* ProcContext -> a0 */
    lw      a0, THREADCONTEXT_OFFSET(a0)    /* ProcContext -> a0 */
    jal     saveContext
    nop

    /* fp set to sp after fp was saved */
    move    fp, sp                   /* terminate stack bottom for gdb */

    LABEL(dispatchException)
					    
    /* Exception cause decoding, jump into exceptionHandlerTable */
    mfc0    v0, c0_cause                  /* cause -> v0 */
    nop
    and     v1, v0, CAUSE_EXC_MASK        /* excCode -> v1 */
    la      k0, exceptionHandlerTable     /* address of exc. table -> k0 */
    addu    k0, k0, v1                    /* address of corresp. handler->k0 */
    lw      k0, 0(k0)                     /* handler -> k0 */
    /* General exception handler expects threadId as only parameter.
     * However, tmMsgSend() and tmMsgRecv() require resp. 2 and 3 parameters
     * (see /Topsy/mips/SyscallMsg.S for the exact parameters of these
     * system calls).
     * CONSEQUENCE: exception handler for SYSCALL is always set per default 
     *
     * Setting threadId of current thread into a0, required by
     * exception handlers 
     */
    la      k1, scheduler                  /* scheduler -> k1 */
    lw      a0, SCHEDRUNNING_OFFSET(k1) /* a0 contains ptr to current thread */
    lw      a0, THREADID_OFFSET(a0)        /* a0 contains thread id */
					
    /* Jump to exception handler. It is assumed that a return is performed
     * in each case (thus, jump and link is required) 
     */
    jal     k0                             /* jump to exc. handler */
    nop
					    
    /* Calling restoreContext() with currentThreadPtr->context in a0,
     * the 'rfe' call is performed in restoreContext() 
     */
    la      k1, scheduler                  /* scheduler -> k1 */
    lw      a0, SCHEDRUNNING_OFFSET(k1) /* a0 contains ptr to current thread */
    lw      a0, THREADCONTEXT_OFFSET(a0)   /* a0 contains context of current 
					      thread */
    j       restoreContext                 /* never comes back */
    nop
					    
    END(generalExceptionHandler)
    LABEL(endGeneralExceptionHandler)
