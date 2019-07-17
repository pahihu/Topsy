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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Topsy/RCS/Support.c,v $
 	Author(s):             Christian Conrad
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.5 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1998/04/11 20:21:07 $      by: $Author: gfa $
	
	
*/

#include "Support.h"


void byteCopy( Address targetAddress,
	       Address sourceAddress,
	       unsigned long int nbBytes)
{
    char* src = (char*)sourceAddress;
    char* tar = (char*)targetAddress;
    unsigned long int i;
    
    for (i = 0; i < nbBytes; i++) {
	*tar++ = *src++;
    }
}

void longCopy( Address targetAddress,
	       Address sourceAddress,
	       unsigned long int nbBytes)
{
    long* src = (long*)sourceAddress;
    long* tar = (long*)targetAddress;
    unsigned long int i;
    
    for (i = 0; i < (nbBytes / sizeof(long)); i++) {
	*tar++ = *src++;
    }
}

void zeroOut(Address target, unsigned long int size)
{
    char* tar = (char*)target;
    unsigned long int i;
    
    for (i = 0; i < size; i++) *tar++ = 0;
}

void stringCopy( char* target,
		 char* source)
{
    while ((*target++ = *source++) != '\0')
      ;
}

void stringNCopy(char* target, char* source, unsigned long int size)
{
    while (((*target++ = *source++) != '\0') && ((--size) > 1)) ;
    *target = '\0';
}
