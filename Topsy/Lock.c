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


        
        File:                  $Source: /proj/topsy/ss98/Topsy/Topsy/RCS/Lock.c,v $
        Author(s):             George Fankhauser
        Affiliation:           ETH Zuerich, TIK
        Version:               $Revision: 1.1 $
        Creation Date:         
        Last Date of Change:   $Date: 1997/03/22 12:59:31 $      by: $Author: gfa $
        
        
*/

#include "Lock.h"
#include "Support.h"

/*
 * a simple spin-lock that assumes a bus-interlocked testAndSet function
 * that takes a pointer to a Boolean and returns TRUE if the lock was taken 
 * FALSE if it could not be locked.
 */
 
/* the lock is in unlocked state at the beginning */
void lockInit(Lock lock)
{
    lock->lockVariable = FALSE;
}

/* busy waits until the lock is available */
void lock(Lock lock)
{
    while (!testAndSet(&(lock->lockVariable)))
	;
}

void unlock(Lock lock)
{
    lock->lockVariable = FALSE;
}

/* try once and give up if it can't be locked */
Boolean lockTry(Lock lock)
{
    return (testAndSet(&(lock->lockVariable)));
}
