/*
	Copyright (c) 1996-1997 Swiss Federal Institute of Technology, 
	Computer Engineering and Networks Laboratory. All rights reserved.

	TOPSY - A Teachable Operating System. 
		Implementation of a tiny and simple micro kernel for
		teaching purposes.

	Permission to use, copy, modify, and distribute this software and its
	documentation for personal and educational purpose, without fee, and 
	without written agreement is hereby granted, provided that the above 
	copyright notice and the following two paragraphs appear in all copies 
	of this software.


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Topsy/RCS/Error.c,v $
 	Author(s):             C. Conrad
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.11 $
 	Creation Date:         6.3.97
 	Last Date of Change:   $Date: 1998/04/08 15:09:52 $      by: $Author: gfa $
	
	
*/

#include "Topsy.h"
#include "Error.h"
#include "IO.h"
#include "List.h"
#include "SupportMIPS.h"

extern List threadList;
extern int exceptionContextFlag;


void errLog( char* threadName,
	     const char* fileName,
	     int lineNumber,
	     char* errorMessage)
{
    ioConsolePutString(threadName);
    if (exceptionContextFlag == 1) {
	ioConsolePutString(" (*in exc*)");
    }
    ioConsolePutString(", ");
    ioConsolePutString(fileName);
    ioConsolePutString(":");
    ioConsolePutInt(lineNumber);
    ioConsolePutString(" ] ");
    ioConsolePutString(errorMessage);
    ioConsolePutString("\n");
}

void error( char* threadName,
	    const char* fileName,
	    int lineNumber,
	    char* errorMessage)
{
    ioConsolePutString("*** ERROR [ ");
    errLog( threadName, fileName, lineNumber, errorMessage);
}

void warning( char* threadName,
	      const char* fileName,
	      int lineNumber,
	      char* errorMessage)
{
    ioConsolePutString("** WARNING [ ");
    errLog( threadName, fileName, lineNumber, errorMessage);
}

void info( char* threadName,
	   const char* fileName,
	   int lineNumber,
	   char* errorMessage)
{
    ioConsolePutString("* INFO [ ");
    errLog( threadName, fileName, lineNumber, errorMessage);
}

void panic ( char *threadName,
	     const char *fileName,
	     int lineNumber,
	     char *errorMessage)
{
    ioConsolePutString("**** PANIC [ ");
    errLog( threadName, fileName, lineNumber, errorMessage);
    while (TRUE) {
    }
}

void printRegisters()
{
    Register epc, badvm;

    epc = getEPC();
    badvm = getBADVM();
    ioConsolePutString("*** EPC:");
    ioConsolePutHexInt(epc);
    ioConsolePutString("  BADVM:");
    ioConsolePutHexInt(badvm);
    ioConsolePutString("\n");
}

