#ifndef __MESSAGES_H
#define __MESSAGES_H
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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Topsy/RCS/Messages.h,v $
 	Author(s):             Christian Conrad
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.27 $
 	Creation Date:         11.2.97
 	Last Date of Change:   $Date: 1998/03/31 15:57:10 $      by: $Author: gries $
	
	
*/

#include "Topsy.h"
#include "Memory.h"
#include "IO.h"
#include "Configuration.h"


#define SELF         0         /* info about one self */
#define ANY          1         /* Receiving from any thread */
#define INFINITY    -1         /* Infinite timeout when receiving message */

/*
 * Message ids and errors
 */

typedef enum {
  ANYMSGTYPE,
  
  TM_START, TM_EXIT, TM_YIELD, TM_KILL, 
  TM_STARTREPLY, TM_KILLREPLY, TM_INFO, TM_INFOREPLY,      /* thread manager */

  VM_ALLOC, VM_FREE, VM_MOVE, VM_PROTECT,                  /* memory manager */
  VM_ALLOCREPLY, VM_FREEREPLY, VM_MOVEREPLY, VM_PROTECTREPLY,
  VM_CLEANUP,
  
  IO_OPEN, IO_CLOSE, IO_READ, IO_WRITE, IO_INIT,	   /* input/output */
  IO_OPENREPLY, IO_CLOSEREPLY, IO_READREPLY, IO_WRITEREPLY, IO_INITREPLY,
  IO_TIMER_START, IO_TIMER_STOP, IO_TIMERREPLY,

  UNKNOWN_SYSCALL                                        /* unknown syscalls */
} MessageId;

typedef enum {
  TM_MSGSENDOK, TM_MSGSENDFAILED, TM_MSGRECVOK, TM_MSGRECVFAILED,
  TM_STARTOK, TM_STARTFAILED, TM_KILLOK, TM_KILLFAILED, 
  TM_INFOOK, TM_INFOFAILED,

  VM_ALLOCOK, VM_ALLOCFAILED, VM_FREEOK, VM_FREEFAILED,
  VM_MOVEOK, VM_MOVEFAILED, VM_PROTECTOK, VM_PROTECTFAILED,
  VM_CLEANUPOK, VM_CLEANUPFAILED,

  IO_OPENOK, IO_OPENFAILED, IO_CLOSEOK, IO_CLOSEFAILED,
  IO_READOK, IO_READFAILED, IO_WRITEOK, IO_WRITEFAILED,
  IO_INITOK, IO_INITFAILED,
} MessageError;

typedef enum { SYSCALL_SEND_OP, SYSCALL_RECV_OP } MsgOpCode;

/* subtype for various infos on threads */ 
typedef enum { SPECIFIC_ID, GETFIRST, GETNEXT } ThreadInfoKind; 

typedef enum { Info_RUNNING, Info_READY, Info_BLOCKED} ThreadInfoStatus;

/* thread structure for users */
typedef struct ThreadInfo_t {        
    ThreadId tid;
    ThreadId ptid ;
    ThreadInfoStatus status ;
    char name[MAXNAMESIZE] ;
} ThreadInfo;


/*
 * Message structures: Threads
 */
typedef struct TMStartMsg_t { 	     /* Message to create a thread */
    ThreadMainFunction fctnAddress;
    ThreadArg parameter;
    char *name;
} TMStartMsg;

typedef struct TMStartReplyMsg_t {   /* Message to create a thread (reply) */
    ThreadId newId;    
    MessageError errorCode;
} TMStartReplyMsg;

typedef struct TMKillMsg_t { 	     /* Message to kill a thread */
    ThreadId id;
} TMKillMsg;

typedef struct TMKillReplyMsg_t {    /* Message to kill a thread (reply) */
    MessageError errorCode;
} TMKillReplyMsg;

typedef struct TMExitMsg_t { 	     /* Message to "exit" a thread */
    ThreadId id;
} TMExitMsg;

typedef struct TMGetInfoMsg_t {      /* Message to get info about threads */
    ThreadId about;
    ThreadInfoKind kind;
    ThreadInfo* infoPtr;
} TMGetInfoMsg;

typedef struct TMGetInfoReplyMsg_t { /* Message to get info (reply) */
    MessageError errorCode;
    long int info[2];
} TMGetInfoReplyMsg;


/*
 * Message structures: Memory
 */
typedef struct VMAllocMsg_t {        /* Message to get virtual memory */
    unsigned long int size;
} VMAllocMsg;

typedef struct VMAllocReplyMsg_t {   /* Message to get virtual memory (reply) */
    MessageError errorCode;
    Address address;
} VMAllocReplyMsg;

typedef struct VMFreeMsg_t {         /* Message to free virtual memory */
    Address address;
} VMFreeMsg;

typedef struct VMFreeReplyMsg_t {    /* Message to free virtual memory (reply) */
    MessageError errorCode;
} VMFreeReplyMsg;

typedef struct VMMoveMsg_t {         /* Message to move memory region */
    Address address;
    ThreadId newOwner;
} VMMoveMsg;

typedef struct VMMoveReplyMsg_t {    /* Message to move memory region (reply) */
    MessageError errorCode;
    Address address;
} VMMoveReplyMsg;

typedef struct VMProtectMsg_t {      /* Message to protect memory region */
    Address startAddress;
    unsigned long int size;
    ProtectionMode pmode;
} VMProtectMsg;

typedef struct VMProtectReplyMsg_t { /* protect memory region (reply) */
    MessageError errorCode;
} VMProtectReplyMsg;

typedef struct VMCleanupMsg_t {      /* cleanup memory regions for thread */
    ThreadId threadId;
} VMCleanupMsg;


/*
 * Message structures: IO
 */
typedef struct IOOpenMsg_t {
    long int deviceNumber;
} IOOpenMsg;

typedef struct IOOpenReplyMsg_t {
    MessageError errorCode;
    long int deviceThreadId;
} IOOpenReplyMsg;

typedef struct IOCloseMsg_t {
    long int deviceThreadId;
} IOCloseMsg;

typedef struct IOCloseReplyMsg_t {
    MessageError errorCode;
} IOCloseReplyMsg;

typedef struct IOReadMsg_t {
    Address buffer;
    unsigned long int size;
} IOReadMsg;

typedef struct IOReadReplyMsg_t {
    MessageError errorCode;
    unsigned long int bytesRead;
} IOReadReplyMsg;

typedef struct IOWriteMsg_t {
    Address buffer;
    unsigned long int size;
} IOWriteMsg;

typedef struct IOWriteReplyMsg_t {
    MessageError errorCode;
    unsigned long int bytesWritten;
} IOWriteReplyMsg;

typedef struct IOInitReplyMsg_t {
    MessageError errorCode;
} IOInitReplyMsg;

typedef struct { /* Message for IO-Device timer1 */
    int period;
} IOTimerMsg;


/*
 * Message structures: Generic messages
 */
typedef struct SyscallReply_t {
    MessageError errorCode;
} SyscallReply;

typedef struct UserMessage_t {  
    void* p1;
    void* p2;
    void* p3;
} UserMessage;



typedef union SpecMsg_u {            /* Union type of all possible messages */
    UserMessage userMsg;

    TMStartMsg tmStart; TMStartReplyMsg tmStartReply;
    TMKillMsg tmKill; TMKillReplyMsg tmKillReply;
    TMGetInfoMsg tmInfo; TMGetInfoReplyMsg tmInfoReply;
    
    VMAllocMsg vmAlloc; VMAllocReplyMsg vmAllocReply;
    VMFreeMsg vmFree; VMFreeReplyMsg vmFreeReply;
    VMMoveMsg vmMove; VMMoveReplyMsg vmMoveReply;
    VMProtectMsg vmProtect; VMProtectReplyMsg vmProtectReply;
    VMCleanupMsg vmCleanup;

    IOOpenMsg ioOpen; IOOpenReplyMsg ioOpenReply;
    IOCloseMsg ioClose; IOCloseReplyMsg ioCloseReply;
    IOReadMsg ioRead; IOReadReplyMsg ioReadReply;
    IOWriteMsg ioWrite; IOWriteReplyMsg ioWriteReply;
                        IOInitReplyMsg ioInitReply;
    IOTimerMsg ioTimer;
    SyscallReply syscallReply;
} SpecMessage;

typedef struct Message_t {     /* Main message structure */
    ThreadId from;		   /* Sender of the message */
    MessageId id;		   /* Type of the message */
    SpecMessage msg;		   /* Specific message contents */
} Message;


#endif __MESSAGES_H
