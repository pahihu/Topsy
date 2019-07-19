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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/User/RCS/Shell.c,v $
 	Author(s):             George Fankhauser
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.32 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1998/04/09 12:16:06 $      by: $Author: fiedler $
	
	
*/

#include "Shell.h"
#include "SeatReservation.h"
#include "UserSupport.h"
/*
#include "PacketDriverTest.h"
#include "PacketStackControl.h"
*/
#include "Demo.h"
#include "HelloWorld.h"
#include "ThreadsDemo.h"
#include "testtimer.h"
#include "CrashMe.h"

#define NUSERCOMMANDS        8 /* please set according to contents of userCommands
                                    (see below) */ 
#define MAXNBOFARGUMENTS    10
#define MAXCOMMANDSIZE      80
#define BLANK               ' '  /* blank character, delimitor between arguments */

static char* argArray[MAXNBOFARGUMENTS+1];
static char  cmdLine[MAXCOMMANDSIZE+1]; 

/* one output console per shell, even if cascaded */
static ThreadId tty;

/* please set NUSERCOMMANDS to the right size -> see some lines above */
ShellFunction userCommands[NUSERCOMMANDS] = {
	{"shell", main, (ThreadArg)0},
	{"trap", trap, (ThreadArg)0},
	{"reserve", Start_Reservation, (ThreadArg)0 },
/*
	{"ptest", PacketDriverTest, (ThreadArg)0 }, 
	{"test3", PacketStackControl, (ThreadArg)0 },
*/
	{"testback1", Testback1, (ThreadArg)" 1 2 3" },
	{"hello", Hello, (ThreadArg)argArray},
	{"demo", Demo, (ThreadArg)argArray}, 
	{"timertest", test_timer, (ThreadArg)0 },
	{"crashme", crashMeMaster, (ThreadArg)"1 2 3" }
 };
 
 
void __main() {}

#define CR    13
#define LF    10
#define BS     8
#define ESC   27

char bs[4]={BS,' ',BS,'\0'};
char beep='\a', newl='\n'; /* used as const */

static void readChar(ThreadId driver, char *c) {
  unsigned long int one = 1;

  do {
    one = 1;
    ioRead(driver, c, &one);
  } while (one != 1);
}

/* includes backspace and echo */
static void readLine(ThreadId driver, char* buf, char* oldbuf,int max)
{
  char c = '\0';
  int i = 0;
  unsigned long int one = 1;

  do {
    readChar(driver, &c);
    one = 1;
    if (c==BS) {
      if (i>0) { i--; display(driver, bs); } 
      else { ioWrite(driver, &beep, &one); }
    } 
    else if (c==ESC) {
      readChar(driver, &c);
      if (c=='[') { 
	readChar(driver, &c);
	if (c=='A') { /* up arrow */
	  for (; i>0; i--) display(driver, bs);
	  stringCopy(buf, oldbuf);
	  i = stringLength(buf);
	  display(driver, buf);
	} 
	else { 
	  c='\0';
	}
      } 
      else { 
	c='\0';
      }
    } 
    else if (c==CR || c==LF || (c>=32 && c<128) || (c>=160 && c<255)) {
      /* allowed char */
      if (i < max) { buf[i++] = c; ioWrite(driver, &c, &one); }
      else { ioWrite(driver, &beep, &one); }
    } 
    else { ioWrite(driver, &beep, &one); }
  } 
  while (!((c == CR) || (c == LF)) && (i < max));
  one = 1; 
  ioWrite(driver, &newl, &one);
  if (i>0) i--;
  buf[i] = '\0'; /* replace the CR or LF by '\0' */
}
 
static int getCommand(char* line) 
{
    if (stringNCompare(line, commandNames[SHELL_START],5) == EQUAL)
	return SHELL_START;
    if (stringNCompare(line, commandNames[SHELL_EXIT],4) == EQUAL)
	return SHELL_EXIT;
    if (stringNCompare(line, commandNames[SHELL_KILL],4) == EQUAL)
	return SHELL_KILL;
    if (stringNCompare(line, commandNames[SHELL_INFO],4) == EQUAL)
	return SHELL_INFO;
    if (stringNCompare(line, commandNames[SHELL_HELP],4) == EQUAL)
	return SHELL_HELP;
    if (stringNCompare(line, commandNames[SHELL_PS],2) == EQUAL)
	return SHELL_PS;
    if (stringNCompare(line, commandNames[SHELL_NULL],1) == EQUAL)
	return SHELL_NULL;
    return SHELL_INVALID;
}

/* FIE 9.4.98 this belongs to UserSupport.c (-> strtok in libc) */
static char* getArgument( int* lineIndex, char* line)
{
    char* ret; /* start of argument */
 
    /* looking for first character non BLANK, non '\0', and non '\n' */
    while ( line[*lineIndex] == BLANK
	    && line[*lineIndex] != '\0'
	    && line[*lineIndex] != CR
	    && line[*lineIndex] != '\n') {
	(*lineIndex)++;
    }
 
    if (line[*lineIndex] == '\0' || line[*lineIndex] == '\n' || line[*lineIndex] == CR)
      return NULL;
 
    ret = &(line[*lineIndex]);
 
    /* Positionning lineIndex onto next BLANK or '\0' or '\n' */
    while ( line[*lineIndex] != BLANK
	    && line[*lineIndex] != '\0'
	    && line[*lineIndex] != CR
	    && line[*lineIndex] != '\n' ) {
	(*lineIndex)++;
    }
    
    if (line[*lineIndex] != '\0') {
	line[*lineIndex] = '\0';
	(*lineIndex)++;
    }
    
    return ret;
}

static void listThreads(){
  ThreadInfo threadInfo ;
  char  idString[12] ;
  char* statusString[] = {"running ", "ready   ", "blocked "} ;
  char  outputline[MAXCOMMANDSIZE];
  int   len;
 
  if (tmGetFirst(&threadInfo) != TM_INFOOK) {
    display(tty,"ps: Error - Cannot get first threadInfo!\n") ; 
    return;
  }
  else {
    do {
      initmem((Address)outputline,40,BLANK);
      itoa((int)threadInfo.tid,idString);
      len=stringLength(idString); /* right bound */
      byteCopy((Address)&outputline[6-len],(Address)idString,len);
      len=stringLength(statusString[threadInfo.status]);
      byteCopy((Address)&outputline[10],statusString[threadInfo.status],len);
      len=stringLength(threadInfo.name);  
      byteCopy((Address)&outputline[20],threadInfo.name,len); 
      outputline[20+len+1]='\n';
      outputline[20+len+2]='\0';
      display(tty,outputline);
    } while (tmGetNext(&threadInfo) != TM_INFOFAILED);
  }       
}

static void kill(char* line) {
  int      i;
  ThreadId threadId;
  int      lineIndex = 4; /* sizeof string "kill" */
  int      argIndex = 0;

  while ((argArray[argIndex]=getArgument(&lineIndex, line)) != NULL &&
	 argIndex < MAXNBOFARGUMENTS) {
    argIndex++;
  }
  
  if (argIndex != 1) { 
     display(tty,"Kill: Wrong number of arguments on the command line!\n"); 
     return;
  }
  
  atoi(&i, argArray[0]); threadId=(ThreadId)i;

  if (TM_KILLOK != tmKill(threadId)) { 
     display(tty,"Kill: This thread could not be killed!\n"); return; 
  }
}

/* Setup Thread to memorize the threads parameters */ 
static void setupThread(ThreadArg arg) {
  char     localLine[MAXCOMMANDSIZE+1];       /* memorized cmdLine */
  char*    localArgArray[MAXNBOFARGUMENTS+1]; /* reparsed args */
  int      argIndex = 0;
  int      lineIndex = 5;  /* sizeof string "start" */
  Message  msg;
  ThreadId shell, setup, child;
  int      len=0;     /* to reassemble cmdLine */
  int      arglen;
  int      i;
  char*    argument;

  tmGetInfo(SELF, &setup, &shell);

  /* copy args on cmdLine and setup localArgArray */
  for (i=0; i<MAXNBOFARGUMENTS; i++) {
    argument=((char**)argArray)[i];
    if (argument == NULL) {
      localArgArray[i]=NULL;
      break;
    }
    arglen=stringLength(argument);
    byteCopy((Address)(&localLine[len]),(Address)argument,arglen);
    localArgArray[i]=&localLine[len];
    len+=arglen;
    byteCopy((Address)(&localLine[len]),(Address)"\0",1);
    len++;    
  }

  /* start the thread */
  if (TM_STARTOK != tmStart(&child, ((ShellFunction*)arg)->function,
		    localArgArray, (*((ShellFunction*)arg)).name)) {
    display(tty,"Setup: Background thread could not be started\n");
    return;
  } 

  tmMsgSend(shell, &msg); /* wakeup shell */
  tmMsgRecv(&child, ANYMSGTYPE, &msg, INFINITY); 
}


static void start(char* line)
{
  int      i;
  ThreadId child,setup;
  Message  reply;
  int      lineIndex = 5; /* sizeof string "start" */
  int      argIndex = 0;
  Boolean  backgr = FALSE;
  Boolean  cmdFound = FALSE;
  char*    argument;

  stringNCopy(cmdLine,line,MAXCOMMANDSIZE);

  for(i=0; cmdLine[i] != '\0'; i++) {
    if (cmdLine[i] == BACKGRCOMMAND) { backgr = TRUE; cmdLine[i] = '\0';  break; }
  }

  /* Setting all arguments (thread name included) in a global
     null-terminated static array argArray */
  while ((argArray[argIndex]=getArgument(&lineIndex, cmdLine)) != NULL &&
	 argIndex < MAXNBOFARGUMENTS) {
    argIndex++;
  }

  if (argIndex == MAXNBOFARGUMENTS) {
     display(tty,"Start: Too many arguments on the command line, last ignored !\n");
  }

  for (i = 0; i < NUSERCOMMANDS; i++) {
    if (stringCompare(userCommands[i].name, argArray[0]) == EQUAL) {
      cmdFound=TRUE;
      if (!backgr) {
	if (tmStart( &child, userCommands[i].function, 
		     argArray, userCommands[i].name) != TM_STARTOK) {
	  display(tty,"Start: Shell command could not be started\n");
	  return;
	}
	/* wait for child */
	if (!backgr) tmMsgRecv(&child, ANYMSGTYPE, &reply, INFINITY); 
      }
      else { /* use Setup Thread to start background thread */
	if (tmStart(&setup, setupThread, (ThreadArg)(&userCommands[i]), "setupThread") !=  TM_STARTOK) {
	  display(tty, "Start: Shell couldn't start background setup thread\n");
	  return;
	}
        /* Wait for setup child (until cmdLine is copied) */
	tmMsgRecv(&setup, ANYMSGTYPE, &reply, INFINITY);
      }
      break; 
    }
  }
  if (!cmdFound) { display(tty, "Start: Shell command not found!\n"); }
}


void main(ThreadArg arg)
{
    char              line[MAXCOMMANDSIZE+1];
    char              oldline[MAXCOMMANDSIZE+1];
    unsigned long int size; 
    int               i;
    
    ioOpen(IO_CONSOLE, &tty);
    ioInit(tty);
    size = sizeof(WELCOME);
    ioWrite(tty, WELCOME, &size);

    oldline[0]='\0';

    while (TRUE) {
	size = sizeof(PROMPT);
	ioWrite(tty, PROMPT, &size);       /* write the $, > or whatever */
	readLine(tty, line, oldline, sizeof(line)); /* read next command */
	if (line[0] != '\0') stringCopy(oldline, line);

	switch (getCommand(line)) {
	    case SHELL_START:
		start(line);
		break;
	    case SHELL_EXIT:
		size = sizeof(EXITMESSAGE);
		ioWrite(tty, EXITMESSAGE, &size);
		ioClose(tty);		
		tmExit();
		break;
	    case SHELL_KILL:
                kill(line);
                break;
	    case SHELL_INFO:
	    case SHELL_HELP:
		size = sizeof(HELP);
		ioWrite(tty, HELP, &size);       /* help text */
		display(tty,"\nUser-Commands:"); /* Display the user-Commands */
		for (i=0;i<NUSERCOMMANDS;i++) {
		  display(tty,"\n    ");
		  display(tty,userCommands[i].name);
		}
		display(tty,"\n");
		break;
	    case SHELL_PS:
                listThreads() ;
		break ;
	    case SHELL_INVALID:
	        size = stringLength(commandNames[SHELL_INVALID]);
	        ioWrite(tty,commandNames[SHELL_INVALID],&size);
	        break;
	    case SHELL_NULL:
	    	break;
	    default:
		break;
	}
    }
}





