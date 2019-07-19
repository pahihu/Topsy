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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Topsy/RCS/HashList.h,v $
 	Author(s):             George Fankhauser
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.11 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1997/04/23 11:56:34 $      by: $Author: gfa $
	
	
*/

#ifndef _HASHLIST_H_
#define _HASHLIST_H_

#include "Topsy.h"
#include "MMHeapMemory.h"


#define HASHLISTSIZE 	17 /* the 7th prime */
#define HASHERROR 	 0
#define HASHOK 		 1
#define HASHNOTFOUND 	 2
#define HASHDUPLICATEKEY 3

typedef struct HashListElementDesc_t {
	void* item;
	unsigned long int key;
	struct HashListElementDesc_t* next;
} HashListElementDesc;

typedef HashListElementDesc* HashListElement;

typedef struct HashList_t {
	HashListElement table[HASHLISTSIZE];
} HashListDesc;

typedef HashListDesc* HashList;


HashList hashListNew(void);
Error hashListAdd(HashList list, void* data, unsigned long int key);
Error hashListGet(HashList list, void** data, unsigned long int key);
Error hashListRemove(HashList list, unsigned long int key);


#endif
