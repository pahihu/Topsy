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


	
	File:                  $Source: /proj/topsy/ss98/Topsy/Topsy/RCS/HashList.c,v $
 	Author(s):             George Fankhauser
 	Affiliation:           ETH Zuerich, TIK
 	Version:               $Revision: 1.13 $
 	Creation Date:         
 	Last Date of Change:   $Date: 1997/04/25 18:36:44 $      by: $Author: gfa $
	
	
*/

#include "HashList.h"


static unsigned long int hashFunction(unsigned long int key) {
    return( key % HASHLISTSIZE); /* index := key MOD listsize */
}


HashList hashListNew() {
    Error error;
    int i;
    HashList list;

    error = hmAlloc((Address*)&list, sizeof(HashListDesc));
    if (error != HM_ALLOCOK) return NULL;
    for (i = 0; i < HASHLISTSIZE; i++) {
	list->table[i] = NULL;
    }
    return list;
}


Error hashListFree(HashList list) {
    int i;
    HashListElement toBeFreed, toBeFreedNext;
    
    for (i = 0; i < HASHLISTSIZE; i++) {
	toBeFreed = list->table[i];
	while (toBeFreed != NULL) {
	    toBeFreedNext = toBeFreed->next;
	    hmFree(toBeFreed);
	    toBeFreed = toBeFreedNext;
	}
    }
    hmFree(list);
    return HASHOK;
}


Error hashListAdd(HashList list, void* data, unsigned long int key) {
    Address address;
    Error error;
    HashListElement element;

    address = NULL;
    error = hmAlloc(&address, sizeof(HashListElementDesc));   
    if (error != HM_ALLOCOK) return HASHERROR;

    if (list->table[hashFunction(key)] == NULL) {
	list->table[hashFunction(key)] = (HashListElement)address;
	element = (HashListElement)address;
    }
    else {
	element = list->table[hashFunction(key)];
	if (element->key == key) { 
	    hmFree(address); return HASHDUPLICATEKEY; 
	}
	while (element->next != NULL) {
	    element = element->next;
	    if (element->key == key) { 
		hmFree(address); return HASHDUPLICATEKEY; 
	    }
	}
	element->next = (HashListElement)address;
	element = element->next;
    }
    element->item = data;
    element->key = key;
    element->next = NULL;
    return HASHOK;
}


Error hashListGet(HashList list, void** data, unsigned long int key) {
    HashListElement element = list->table[hashFunction(key)];

    while (element != NULL) {
	if (element->key == key) {
	    *data = element->item;
	    return HASHOK;
	}
	element = element->next;
    }
    *data = NULL;
    return HASHNOTFOUND;
}


Error hashListRemove(HashList list, unsigned long int key) {
    HashListElement previous = NULL;
    HashListElement element = list->table[hashFunction(key)];

    if (element != NULL) {
	if (element->key == key) {
	    list->table[hashFunction(key)] = element->next;
	    hmFree(element);
	    return HASHOK;
	}
	else {
	    previous = element;
	    element = element->next;
	    while (element != NULL) {
		if (element->key == key) {
		    previous->next = element->next;
		    hmFree(element);
		    return HASHOK;
		}
		previous = element;
		element = element->next;
	    }
	}
    }
    return HASHNOTFOUND;
}
