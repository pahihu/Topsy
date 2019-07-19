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


        
        File:                  $Source: /proj/topsy/ss98/Topsy/Topsy/RCS/List.h,v $
        Author(s):             George Fankhauser
        Affiliation:           ETH Zuerich, TIK
        Version:               $Revision: 1.8 $
        Creation Date:         
        Last Date of Change:   $Date: 1997/03/31 20:27:18 $      by: $Author: gfa $
        
        
*/
#ifndef _LIST_H_
#define _LIST_H_

#include "Topsy.h"
#include "MMHeapMemory.h"

#define LIST_ERROR 0
#define LIST_OK 1

typedef struct ListElement_t {
    void* item;
    struct ListElement_t* next;
    struct ListElement_t* prev;
} ListElementDesc;

typedef ListElementDesc* ListElement;

typedef struct ListDesc_t {
    ListElement first;
    ListElement current;
    ListElement last;
} ListDesc;

typedef ListDesc* List;

/* basic list functions */
List listNew(void);
Error listFree(List list);

Error listAddInFront(List list, void* item, Address* hint);
Error listAddAtEnd(List list, void* item, Address* hint);
Error listRemove(List list, void* item, Address hint);

/* advanced operations that cause no alloc/free */
Error listMoveToEnd(List list, void* item, Address hint);
Error listSwap(List listFrom, List listTo, void* item, Address hint);

/* iteration */
Error listGetFirst(List list, void** itemPtr);
Error listGetNext(List list, void** itemPtr);

#endif
