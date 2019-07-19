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


        
        File:                  $Source: /proj/topsy/ss98/Topsy/Topsy/RCS/Lock.h,v $
        Author(s):             George Fankhauser
        Affiliation:           ETH Zuerich, TIK
        Version:               $Revision: 1.2 $
        Creation Date:         
        Last Date of Change:   $Date: 1997/03/31 20:27:41 $      by: $Author: gfa $
        
        
*/
#ifndef _LOCK_H_
#define _LOCK_H_

#include "Topsy.h"


typedef struct LockDesc_t {
    volatile Boolean lockVariable;
} LockDesc;

typedef LockDesc* Lock;


void lockInit(Lock lock);
void lock(Lock lock);
void unlock(Lock lock);
Boolean lockTry(Lock lock);

#endif

