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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/User/RCS/CrashMe.c,v $
 	Author(s):             C. Conrad 
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.21 $
 	Creation Date:         21.3.97
 	Last Date of Change:   $Date: 1998/04/02 09:33:17 $      by: $Author: gfa $
	
	
*/

#include "../Topsy/Topsy.h"
#include "../Topsy/Syscall.h"
#include "CrashMe.h"
#include "UserSupport.h"
#include "limits.h"

#define VM_NBAREAS      10
#define VM_AREASIZE    256

#define ENDTEST          1

#define IA 16807
#define IM 2147483647
#define IQ 127773
#define IR 2836
#define MASK 123459876
long idum = 12345;

char welcome[] = "\n CRASH-ME TEST PROGRAM (MASTER)\n";
static int totalNbOfThreads = 0;
static int totalNbOfMessages = 0;

void empty() {}
void loop() { while (TRUE) {} }
void sleepy() 
{ Message msg; ThreadId id = ANY; tmMsgRecv(&id, ANYMSGTYPE, &msg, INFINITY); }

static long int valuesOfInterest [] = {
  
NULL, 0xffffffff, 0x00000000, CHAR_BIT, SCHAR_MIN,
SCHAR_MAX, UCHAR_MAX, CHAR_MIN, CHAR_MAX, SHORT_MIN,
SHORT_MAX, UNSIGNED_SHORT_MAX, INT_MAX, INT_MIN, UNSIGNED_INT_MAX,
0x80000000, 0x80000000, 0x90000000, 0xa0000000, 0xb0000000,
0xc0000000, 0xd0000000, 0xe0000000, 0xf0000000, 0x00001000,
0x00003000, 0x00010000, 0x000f0000, 0x00100000, 0x00f00000,
0x01000000, 0x70000000
};

int random(long *idum, int range)
{
    long k;
    int ans;
    
    *idum ^= MASK;
    k=(*idum)/IQ;
    *idum=IA*(*idum-k*IQ)-IR*k;
    if (*idum<0)
      *idum+=IM;
    ans=(*idum) % range;
    *idum ^= MASK;
    return ans;
}

long int pickValue()
{
    int value;
    
    /* Random values are picked either in the range [-30..30] or in the set
     * valuesOfInterest[] with an equal probability.
     */
    if (random(&idum, 100) > 30) {
	value = 30 - random(&idum, 60);
    } else {
	value = valuesOfInterest[random(&idum, 32)];
    }
    return value;
}


long int pickDest() {
    return (- (random(&idum, 6) + 1));
}


void dummyThread( ThreadParam* p)
{
    Message childMsg;
    char tid[10];
    char out[100];
    
    display(p->tty, "dummyThread ready to be killed\n");
    int2string(tid, p->id);
    /*display(p->tty, "sending of message to ");
    display(p->tty, tid);
    display(p->tty, "\n");*/
    stringConcat(out, "sending of message to ", tid);
    display(p->tty, out);
    display(p->tty, "\n");
    
    if (tmMsgSend( p->id, &childMsg) != TM_MSGSENDOK) {
	display(p->tty, "!!! CHILD: tmMsgSend() Failed\n");
    }
    totalNbOfMessages++;

    /* busy waiting (actually not necessary, as the user thread is no longer
       scheduled */
    while (1) {
	/*display(p->tty, ":-)");*/
    }
}

void displayMessage(Message* msg, int tty)
{
    int* addr = (int*)msg;
    char out[10];
    int i;

    display(tty, " Message was ");
    for (i=0;i<5;i++) {
	itoa(*(addr+i), out);
	display(tty, out);
	display(tty, " : ");
    }
}
    
void randomThread( ThreadParam* p)
{
    ThreadId childId;
    Message childMsg;
    ThreadId ownId, parentId;
    ThreadParam p2;
    int tty = p->tty;
    int nbMessages = p->id;
    Message message;
    int i,j;
    int intMsg[5];
    /* = {-3,19,-3,24,-14};*/
    char string[10];
    int dest;

    display(tty, "Generating garbage messages ... ");
    itoa(nbMessages, string);
    display(tty, string);
    display(tty, "\n");

    if (tmGetInfo(SELF, &ownId, &parentId) != TM_INFOOK) {
	display(p->tty, "!!! CHILD: tmGetInfo() failed\n");
    } 
    /* Nonsense and garbage overflow */
    for (i=0;i<nbMessages;i++) {
#if 1
    	intMsg[0] = pickDest();
	for (j=1;j<5;j++) {
	    intMsg[j]=pickValue();
	}
	/*if (intMsg[1] < 0) intMsg[1] = - intMsg[1];*/
#endif
	byteCopy( &message, intMsg, 5*4);
	while (message.from==ownId || message.from==parentId || message.from==2)
	  message.from=pickDest();
	if (message.id == TM_KILL) {
	    if (message.from == TMTHREADID) {
		while (message.msg.tmKill.id == parentId || message.msg.tmKill.id == 2) 
		  message.msg.tmKill.id=pickValue();
	    }
	}
	dest = message.from;
	displayMessage(&message, tty);
	display(tty, "\n");
	totalNbOfMessages++;
	if (tmMsgSend( dest, &message) != TM_MSGSENDOK) {
	    /*display(tty, "!!! tmMsgSend() FAILED\n");*/
	} else {
	    /*display(tty, "\ntmMsgSend() OK - (dest=");
	    itoa(dest, string);
	    display(tty, string);
	    display(tty, ")");
	    displayMessage(&message, tty);
	    display(tty, "\n");*/
	}
    }
    
}


/* This random test starts a child thread that issues random (and most of the time
 * nonsense) messages anywhere.
 */
void randomTest(int tty, int nbMessages, int iteration)
{
    ThreadId childId;
    ThreadId fromId=ANY;
    Message childMsg;
    ThreadParam p;
    ThreadParam a;
    char string[10];

    display(tty, "//////////////////////////////////////////////////\n");
    display(tty, "///////////////////////   RANDOM-TEST ////////////\n");
    display(tty, "//////////////////////////////////////////////////\n");
    
    itoa(iteration, string);
    display(tty, string);
    display(tty, ", Total nb of messages sent: ");
    itoa(totalNbOfMessages, string);
    display(tty, string);
    display(tty, "\n");

#if 0
    /* Launching of a dummy test Thread */
    a.tty = tty;
    if (tmStart( &childId, (ThreadMainFunction)dummyThread,
		 &a, "dummyThread") != TM_STARTOK) {
	display(tty, "!!! CHILD: tmStart() failed\n");
    }
#endif
    
    /* Creation of a child thread */
    p.tty = tty;
    p.id = nbMessages;  
    if (tmStart( &childId, (ThreadMainFunction)randomThread,
		 &p, "random") != TM_STARTOK) {
	display(tty, "!!! PARENT: tmStart() failed\n");
    }
    totalNbOfThreads++;

    /* Waiting for a message from child */
    if (tmMsgRecv( &childId, ANYMSGTYPE, &childMsg, INFINITY) != TM_MSGRECVOK) {
	display(tty, "PARENT: Child is no longer alive, end of test\n");
    }
}

void recThread( ThreadParam* p)
{
    ThreadId childId;
    Message childMsg;
    ThreadParam p2;
    Address area;

    /*if (vmAlloc( &area, 1024) != VM_ALLOCOK) {
	display(p->tty, "!!! CHILD: vmAlloc() failed\n");
    }*/

    /* Recursive creation of the same child thread */
    p2.tty = p->tty;
    if (tmStart( &childId, (ThreadMainFunction)recThread,
		 &p2, "child") != TM_STARTOK) {
	display(p->tty, "!!! RECURSIVE THREAD: tmStart() failed\n");
    } else {
	totalNbOfThreads++;
	/* Waiting for termination message from child */
	if (tmMsgRecv( &childId, ANYMSGTYPE, &childMsg, INFINITY) != TM_MSGRECVOK) {
	    /*display(p->tty, "!!! RECURSIVE THREAD: Child is no longer alive\n");*/
	}
    }
}

/* This stress test starts a child thread, that will in turn recursively
 * start the same thread, etc.
 */
void stressTest(int tty)
{
    ThreadId childId;
    Message childMsg;
    ThreadParam p;

    display(tty, "//////////////////////////////////////////////////\n");
    display(tty, "///////////////////////   STRESS-TEST ////////////\n");
    display(tty, "//////////////////////////////////////////////////\n");	    

    /* Creation of a child thread */
    p.tty = tty;
    if (tmStart( &childId, (ThreadMainFunction)recThread,
		 &p, "child") != TM_STARTOK) {
	display(tty, "!!! PARENT: tmStart() failed\n");
    }
    totalNbOfThreads++;

    /* Waiting for a message from any */
    if (tmMsgRecv( &childId, ANYMSGTYPE, &childMsg, INFINITY) != TM_MSGRECVOK) {
	display(tty, "PARENT: Child is no longer alive, end of test\n");
    }
}




    
void childSyscallTest( ThreadParam* p)
{
    Message childMsg;
    ThreadId childId, ownId, parentId;
    Address area1;
    Address area[VM_NBAREAS];
    int i, j;
    char car;
    ThreadParam a;
    int endCode=ENDTEST;
    char ownIdString[10], parentIdString[10];
    char childIdString[10];
    char out[100];
    int k;
    
    display(p->tty, "CHILD: entering main procedure for syscall test\n");

    display(p->tty, "CHILD: looking for threadId and parentId ...\n");
    if (tmGetInfo(SELF, &ownId, &parentId) != TM_INFOOK) {
	display(p->tty, "!!! CHILD: tmGetInfo() failed\n");
    } else {
	int2string( ownIdString, ownId);
	int2string( parentIdString, parentId);
	display(p->tty, "CHILD: tmGetInfo() succeeded\n");
	display(p->tty, "       retrieved info: ownId=");
	display(p->tty, ownIdString);
	display(p->tty, ", parentId=");
	display(p->tty, parentIdString);
	display(p->tty, "\n");
    }

    /* Testing virtual memory */
    for (i=0;i<VM_NBAREAS;i++) {
	if (vmAlloc( &area[i], VM_AREASIZE) != VM_ALLOCOK) {
	    display(p->tty, "!!! CHILD: vmAlloc() failed\n");
	}
    }
    display(p->tty, "CHILD: vmAlloc() all succeeded\n");
    /* Writing/Reading garbage inside allocated regions */
#if 1
    for (i=0;i<VM_NBAREAS*VM_AREASIZE;i++) {
	(char)*((char*)area[i%VM_NBAREAS]+i%VM_AREASIZE) = 'o';
	car = (char)*((char*)area[i%VM_NBAREAS]+i%VM_AREASIZE);
	if (car != 'o') display(p->tty, "CHILD: VM read/write inconsistent\n");
    }
    display(p->tty, "CHILD: all read/write succeeded\n");
#endif
    
    /* Launching of a dummy test Thread */
    a.tty = p->tty;
    a.id = ownId;
    
    if (tmStart( &childId, (ThreadMainFunction)dummyThread,
		 &a, "dummyThread") != TM_STARTOK) {
	display(p->tty, "!!! CHILD: tmStart() failed\n");
    }
    totalNbOfThreads++;

    /* Testing yield ... */
    display(p->tty, "CHILD: testing tmYield\n");
    tmYield();

    /* Testing killing of dummy thread */
    if (tmMsgRecv( &childId, ANYMSGTYPE, &childMsg, 0) != TM_MSGRECVOK) {
	display(p->tty, "!!! CHILD: failed to receive end message from child\n");
    } else {
	if (tmKill( childId) != TM_KILLOK) {
	    display(p->tty, "CHILD: tmKill() failed\n");
	} else {
	    display(p->tty, "CHILD: tmKill() succeeded\n");
	}
    }

    /* Testing vmProtect() */
    if (vmProtect( area[0], VM_AREASIZE, READ_WRITE_REGION) != VM_PROTECTOK) {
	display(p->tty, "CHILD: vmProtect() failed\n");
    } else {
	display(p->tty, "CHILD: vmProtect() succeeded\n");
    }
    
    /* Removing all memory regions */
    for (i=0;i<VM_NBAREAS;i++) {
	if (vmFree( area[i]) != VM_FREEOK) {
	    display(p->tty, "!!! CHILD: vmFree() failed\n");
	}
    }
    display(p->tty, "CHILD: vmFree() all succeeded\n");


    /* Sending of a message to parent (end of testing) */
    childMsg.msg.userMsg.p1 = &endCode;
    if (tmMsgSend( p->id, &childMsg) != TM_MSGSENDOK) {
	display(p->tty, "!!! CHILD: tmMsgSend() Failed\n");
    }
    totalNbOfMessages++;
    display(p->tty, "CHILD: Syscall test successfully completed\n");
}


void crashMeSyscallTest( ThreadId tty)
{
    ThreadParam p;
    ThreadId childId, ownId, parentId;
    char ownIdString[10], parentIdString[10];
    Message childMsg;

    display(tty, "//////////////////////////////////////////////////\n");
    display(tty, "///////////////////////   SYSCALL-TEST ///////////\n");
    display(tty, "//////////////////////////////////////////////////\n");	    
    display(tty, "PARENT: looking for threadId and parentId ...\n");
    if (tmGetInfo(SELF, &ownId, &parentId) != TM_INFOOK) {
	display(tty, "!!! PARENT: tmGetInfo() failed\n");
    } else {
	int2string( ownIdString, ownId);
	int2string( parentIdString, parentId);
	display(tty, "PARENT: tmGetInfo() succeeded\n");
	display(tty, "        retrieved info: ownId=");
	display(tty, ownIdString);
	display(tty, ", parentId=");
	display(tty, parentIdString);
	display(tty, "\n");
    }

    /* Creation of a child thread */
    p.tty = tty;
    p.id = ownId; 
    if (tmStart( &childId, (ThreadMainFunction)childSyscallTest,
		 &p, "childSyscallTest") != TM_STARTOK) {
	display(p.tty, "!!! PARENT: tmStart() failed\n");
    }
    totalNbOfThreads++;

    /* Waiting for the end message from child */
    if (tmMsgRecv( &childId, ANYMSGTYPE, &childMsg, INFINITY) != TM_MSGRECVOK) {
	display(p.tty, "!!! PARENT: failed to receive end message from child\n");
    } else {
	if (*(int*)(childMsg.msg.userMsg.p1) = ENDTEST) {
	    display(p.tty, "PARENT: child successfully completed syscall test\n");
	} else {
	    display(p.tty, "!!! PARENT: child completed syscall test with wrong msg\n");
	}
    }
}

char tryMemory(ThreadArg arg)
{
    char c;
    char* address = (char*)arg;
    
    c = *address;
    return c; /* must be here otherwise it will be swept by the optimizer */
}


void memoryTest(ThreadId tty, Address addr)
{
    ThreadId childId;
    Message childMsg;
    SyscallError err;

    if (tmStart( &childId, (ThreadMainFunction)tryMemory,
		 addr, "try & die") != TM_STARTOK) {
	display(tty, "MEMTEST: tmStart() failed\n");
    }
    totalNbOfThreads++;
    /* Waiting for the end message from child */
    err=tmMsgRecv(&childId, ANYMSGTYPE, &childMsg, INFINITY);
    if (err == TM_MSGRECVFAILED) {
	display(tty, "MEMTEST: child was killed\n");
    }    
}


void memoryTestAll( ThreadId tty)
{
    ThreadId childId;
    Message childMsg;
    SyscallError err;
    int i;

    display(tty, "//////////////////////////////////////////////////\n");
    display(tty, "////////////////////////   MEMORY-TEST ///////////\n");
    display(tty, "//////////////////////////////////////////////////\n");	    
    memoryTest(tty, (Address)0xffffffff);
    memoryTest(tty, (Address)0x00000000);
    memoryTest(tty, (Address)0x80000000);
    memoryTest(tty, (Address)0x90000000);
    memoryTest(tty, (Address)0xa0000000);
    memoryTest(tty, (Address)0xb0000000);
    memoryTest(tty, (Address)0xc0000000);
    memoryTest(tty, (Address)0xd0000000);
    memoryTest(tty, (Address)0xe0000000);
    memoryTest(tty, (Address)0xf0000000);
    memoryTest(tty, (Address)0x00001000);
    memoryTest(tty, (Address)0x00003000);
    memoryTest(tty, (Address)0x00010000);
    memoryTest(tty, (Address)0x000f0000);
    memoryTest(tty, (Address)0x00100000);    
    memoryTest(tty, (Address)0x00f00000);    
    memoryTest(tty, (Address)0x01000000);    
    memoryTest(tty, (Address)0x70000000);    

    display(tty, "\n*** MEMTEST: all addresses done\n");    
    

    for (i = 0; i < 20; i++) {	
        if (tmStart( &childId, (ThreadMainFunction)empty,
		     0, "empty") != TM_STARTOK) {
	    display(tty, "MEMTEST: tmStart() failed\n");
	}
	totalNbOfThreads++;
    }
}


void trapTest( ThreadId tty)
{
    ThreadId childId;
    Message childMsg;
    SyscallError err;
    int i;
    
    long instr[] = { 	0xabcdef01, 0x89745325, 0x78376788, 0x00032686,
    			0x23467666, 0x65778945, 0x0000000c, 0x0000000c,
			0x00000001, 0x20000000, 0x30000000, 0x40000000,
			0x08002000, 0x0000000c, 0x89745325, 0x89745325};

    display(tty, "//////////////////////////////////////////////////\n");
    display(tty, "////////////////////////   TRAP-TEST   ///////////\n");
    display(tty, "//////////////////////////////////////////////////\n");	    

    for (i = 0; i < 16; i++) {	
	tmStart(&childId, (ThreadMainFunction)&(instr[i]), NULL, "trapper");
	err=tmMsgRecv(&childId, ANYMSGTYPE, &childMsg, INFINITY);
	if (err == TM_MSGRECVFAILED) {
	    display(tty, "TRAPPER: child was killed\n");
	}
	totalNbOfThreads++;
    }
    for (i = 0; i < 24; i++) {	
    	instr[0] = random(&idum, 0x7fffffff);
	tmStart(&childId, (ThreadMainFunction)&(instr[0]), NULL, "tripper");
	totalNbOfThreads++;
	err=tmMsgRecv(&childId, ANYMSGTYPE, &childMsg, INFINITY);
	if (err == TM_MSGRECVFAILED) {
	    display(tty, "TRAPPER: child was killed\n");
	}
    	instr[0] = - random(&idum, 0x7fffffff);
	tmStart(&childId, (ThreadMainFunction)&(instr[0]), NULL, "trooper");
	totalNbOfThreads++;
	err=tmMsgRecv(&childId, ANYMSGTYPE, &childMsg, INFINITY);
	if (err == TM_MSGRECVFAILED) {
	    display(tty, "TRAPPER: child was killed\n");
	}
    }
}

void displayStatistics(int tty)
{
    char string[10];

    display(tty, "*****************************************\n");
    display(tty, "*********** CrashMe Results *************\n");
    display(tty, "nb generated threads: ");
    itoa(totalNbOfThreads, string);
    display(tty, string);
    display(tty, "\nnb. of messages sent: ");
    itoa(totalNbOfMessages, string);
    display(tty, string);
    display(tty, "\n*****************************************\n");
    display(tty, "*****************************************\n\n");
}


void crashMeMaster( char* argArray[] )
{
    ThreadId tty;
    int i=0, j, k;
    int id = SYSCALL_TEST;  /* to be changed */
    int nbMessages;
    int nbExecutions;
    char string[10];
    int charSize=1;
    int ascCode;
    
    ioOpen(IO_CONSOLE, &tty);
    ioInit(tty);

#if 0
    /* Display of all arguments, NULL terminated array */
    display(tty, "CrashMeMaster: Display of all arguments on command line:\n");
    while (argArray[i] != NULL) {
	display(tty, "\t");
	/*display(tty, argArray[i]);
	display(tty, "\n");*/
	j=0;
	while ((argArray[i])[j] != '\0') {
	    itoa(j, string);
	    display(tty, string);
	    display(tty, "=");
	    /*display(tty, (argArray[i])[j]);*/
	    ioWrite(tty, &(argArray[i][j]), &charSize);
	    display(tty, ", ");
	    j++;
	}
	if (argArray[i][j-1] != '\0') {
	    display(tty, "\nwhat is this character ? : ascii code is ");
	    ascCode=(int)(argArray[i][j-1] - '\0');
	    itoa(ascCode, string);
	    display(tty, string);
	    display(tty, "\n");
	}
	    
	display(tty, "\n");
	i++;
    }
    display(tty, "CrashMeMaster: Display of all arguments (END)\n");
#endif

    if (stringCompare(argArray[1], "syscall") == EQUAL) {
	crashMeSyscallTest(tty);
	displayStatistics(tty);
    } else if (stringCompare(argArray[1], "stress") == EQUAL) {
	stressTest(tty);
	displayStatistics(tty);
    } else if (stringCompare(argArray[1], "random") == EQUAL) {
	atoi(&nbMessages, argArray[2]);
	atoi(&nbExecutions, argArray[3]);
	itoa(nbExecutions, string);
	display(tty, "random test should start for \n");
	display(tty, string);
	display(tty, " iterations\n");
	for (k=1;k<=nbExecutions;k++)
	  randomTest(tty, nbMessages, k);
	displayStatistics(tty);
    } else if (stringCompare(argArray[1], "all") == EQUAL) {
	atoi(&nbExecutions, argArray[2]);
        for (j=0;j<nbExecutions;j++) {
	    display(tty, "starting of ALL test, iteration ");
	    itoa(j, string);
	    display(tty, string);
	    display(tty, "...\n");
	    crashMeSyscallTest(tty);
	    memoryTestAll(tty);
	    stressTest(tty);
	    trapTest(tty);
	    for (k=1;k<=10;k++)
	      randomTest(tty, 500, k);
	    displayStatistics(tty);
	}
    } else if (stringCompare(argArray[1], "memory") == EQUAL) {
	memoryTestAll(tty);
	displayStatistics(tty);
    } else if (stringCompare(argArray[1], "trap") == EQUAL) {
	trapTest(tty);
	displayStatistics(tty);
    } else {
	display(tty, "sorry, don't know which test to do !\n");
    }
	
}
