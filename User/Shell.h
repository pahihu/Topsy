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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/User/RCS/Shell.h,v $
 	Author(s):             George Fankhauser
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.6 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1998/04/08 08:27:30 $      by: $Author: fiedler $
	
	
*/

#include "Topsy.h"
#include "Syscall.h"

typedef struct ShellFunction_t {
    char* name;
    ThreadMainFunction function;
    ThreadArg arg;
} ShellFunction;

    
#define SHELL_INVALID	0
#define SHELL_START	1
#define SHELL_EXIT	2
#define SHELL_KILL	3
#define SHELL_INFO	4
#define SHELL_HELP	5
#define SHELL_PS	6
#define SHELL_NULL	7

#define BACKGRCOMMAND       '&'

char* commandNames[] = {
	"invalid command\n", 
	"start", "exit", "kill", "info", "help", "ps", "\n"
};

#define WELCOME "\nTopsy Shell 1.1 (c) 1998, ETH Zurich, TIK\n($Id: Shell.h,v 1.6 1998/04/08 08:27:30 fiedler Exp $)\n\n"
#define EXITMESSAGE "exiting shell...\n"
#define PROMPT "> "
#define HELP "    start <function> <arg>   start a new thread\n    exit                     leave this shell  \n    kill <id>                kill a thread     \n    ps                       print thread status\n    help                     show this helptext\n" 

void main(ThreadArg arg);




